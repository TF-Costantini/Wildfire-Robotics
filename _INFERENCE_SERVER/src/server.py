import asyncio
import base64
import json
import logging
import os
import time

import websockets

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)

HOST = os.getenv("HOST", "0.0.0.0")
PORT = int(os.getenv("PORT", 8765))


# ─── YOLO setup ──────────────────────────────────────────────────────────────
import numpy as np
import cv2
from ultralytics import YOLO

MODEL_PATH     = os.getenv("MODEL_PATH", "yolov8n.pt")
DEVICE         = os.getenv("DEVICE", "cpu")          # "cuda:0" on GPU
CONF_THRESHOLD = float(os.getenv("CONF_THRESHOLD", "0.5"))
COCO_PERSON    = 0                                    # COCO class index for person

log.info("Loading YOLO model from '%s' on device '%s' …", MODEL_PATH, DEVICE)
_model = YOLO(MODEL_PATH)
log.info("Model loaded.")


def process_with_yolo(image_bytes: bytes) -> dict:
    """
    Receives a JPEG/PNG as raw bytes (sent by the ROS client).
    Returns a dict with the largest-area person detection, or found=False.
    """
    # Decode JPEG → OpenCV BGR frame
    arr   = np.frombuffer(image_bytes, dtype=np.uint8)
    frame = cv2.imdecode(arr, cv2.IMREAD_COLOR)
    if frame is None:
        return {"status": "error", "error": "could not decode image"}

    img_h, img_w = frame.shape[:2]

    t0 = time.perf_counter()
    try:
        results = _model(frame, verbose=False, device=DEVICE)
    except Exception as e:
        log.error("YOLO inference error: %s", e)
        return {"status": "error", "error": str(e)}
    inference_ms = round((time.perf_counter() - t0) * 1000, 2)

    boxes = results[0].boxes
    if boxes is None or len(boxes) == 0:
        return {"status": "ok", "detection": None, "inference_ms": inference_ms}

    # Filter: person class + minimum confidence
    person_boxes = [
        b for b in boxes
        if int(b.cls[0].item()) == COCO_PERSON
           and float(b.conf[0].item()) >= CONF_THRESHOLD
    ]
    if not person_boxes:
        return {"status": "ok", "detection": None, "inference_ms": inference_ms}

    # Pick the highest-confidence detection (mirrors your ROS node logic)
    largest = max(person_boxes, key=lambda b: float(b.conf[0]))
    x1, y1, x2, y2 = largest.xyxy[0].cpu().numpy()

    return {
        "status": "ok",
        "detection": {
            "cx":   float((x1 + x2) / 2.0),
            "cy":   float((y1 + y2) / 2.0),
            "area": float((x2 - x1) * (y2 - y1)),
            "conf": float(largest.conf[0].item()),
            "img_w": img_w,
            "img_h": img_h,
        },
        "inference_ms": inference_ms,
    }
# ─────────────────────────────────────────────────────────────────────────────


async def handle_client(websocket):
    remote = websocket.remote_address
    log.info("Client connected via CLI/websocat: %s:%s", *remote)

    # Create an empty byte buffer to gather incoming file chunks
    image_buffer = bytearray()
    t0 = time.perf_counter()

    try:
        # Loop over fragments as they arrive
        async for message in websocket:
            if isinstance(message, bytes):
                image_buffer.extend(message)
            else:
                try:
                    payload = json.loads(message)
                    image_buffer.extend(base64.b64decode(payload["image"]))
                except Exception:
                    # If it's a raw text fragment, append it as text data
                    image_buffer.extend(message.encode('utf-8'))

    except websockets.exceptions.ConnectionClosedOK:
        log.info("Client finished streaming data.")
    except Exception as exc:
        log.error("Error reading stream: %s", exc)
    finally:
        # Once the stream finishes or breaks, process the accumulated frame
        if len(image_buffer) > 0:
            result = process_with_yolo(bytes(image_buffer))
            result["elapsed_ms"] = round((time.perf_counter() - t0) * 1000, 2)

            # Send the final single evaluation response back
            try:
                await websocket.send(json.dumps(result))
            except Exception:
                log.info("Result summary: %s", json.dumps(result))

async def main():
    log.info("Starting YOLO WebSocket server on %s:%s", HOST, PORT)
    async with websockets.serve(handle_client, HOST, PORT):
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    asyncio.run(main())