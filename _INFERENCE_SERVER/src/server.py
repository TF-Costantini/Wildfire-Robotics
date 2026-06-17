import asyncio
import base64
import json
import logging
import os
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

import websockets

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)

HOST = os.getenv("HOST", "0.0.0.0")
PORT = int(os.getenv("PORT", 8765))          # YOLO inference WebSocket (ROS)
RAW_PORT = int(os.getenv("RAW_PORT", 8766))  # raw camera passthrough WebSocket (ROS)
WEB_PORT = int(os.getenv("WEB_PORT", 8080))  # browser web UI


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


# ─── Web UI frame hubs ───────────────────────────────────────────────────────
# Two fully independent pipes:
#   _inf_hub — annotated frames from the YOLO inference path (:8765).
#   _raw_hub — raw camera frames passed straight through (:8766), no inference.
# Each holds the latest JPEG and is consumed by any number of browser MJPEG
# streams. Decoupled from the asyncio loop so a slow browser can never stall it.
class FrameHub:
    def __init__(self):
        self._cond = threading.Condition()
        self._jpeg = None
        self._version = 0

    def publish(self, jpeg: bytes):
        with self._cond:
            self._jpeg = jpeg
            self._version += 1
            self._cond.notify_all()

    def get_after(self, last_version: int, timeout: float = 1.0):
        """Block until a frame newer than last_version exists (or timeout).
        Returns (jpeg_bytes, version). On timeout returns the current frame so
        the MJPEG stream stays alive while idle."""
        with self._cond:
            if self._version == last_version:
                self._cond.wait(timeout)
            return self._jpeg, self._version


_inf_hub = FrameHub()
_raw_hub = FrameHub()


def _placeholder_jpeg(text: str) -> bytes:
    img = np.zeros((240, 320, 3), dtype=np.uint8)
    cv2.putText(img, text, (14, 125),
                cv2.FONT_HERSHEY_SIMPLEX, 0.55, (200, 200, 200), 1, cv2.LINE_AA)
    ok, buf = cv2.imencode(".jpg", img)
    return buf.tobytes()


def _publish_frame(frame, box_xyxy, conf, inference_ms):
    """Burn the returned detection box onto the frame and push to the inference
    hub. Best-effort: never raise into the inference reply path."""
    try:
        if box_xyxy is not None:
            x1, y1, x2, y2 = (int(v) for v in box_xyxy)
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            label = f"person {conf:.2f} | {inference_ms} ms"
            cv2.putText(frame, label, (x1, max(14, y1 - 8)),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2, cv2.LINE_AA)
        else:
            cv2.putText(frame, f"no detection | {inference_ms} ms", (8, 20),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2, cv2.LINE_AA)
        ok, buf = cv2.imencode(".jpg", frame)
        if ok:
            _inf_hub.publish(buf.tobytes())
    except Exception as e:
        log.warning("web annotate/encode failed: %s", e)


def process_with_yolo(image_bytes: bytes) -> dict:
    """
    Receives a JPEG/PNG as raw bytes (sent by the ROS client).
    Returns a dict with the largest-area person detection, or detection=None.
    Side effect: publishes the annotated frame to the inference web hub.
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

    detection = None
    box_xyxy = None
    conf = None

    boxes = results[0].boxes
    if boxes is not None and len(boxes) > 0:
        # Filter: person class + minimum confidence
        person_boxes = [
            b for b in boxes
            if int(b.cls[0].item()) == COCO_PERSON
               and float(b.conf[0].item()) >= CONF_THRESHOLD
        ]
        if person_boxes:
            # Pick the highest-confidence detection (mirrors the ROS node logic)
            largest = max(person_boxes, key=lambda b: float(b.conf[0]))
            x1, y1, x2, y2 = largest.xyxy[0].cpu().numpy()
            conf = float(largest.conf[0].item())
            box_xyxy = (x1, y1, x2, y2)
            detection = {
                "cx":   float((x1 + x2) / 2.0),
                "cy":   float((y1 + y2) / 2.0),
                "area": float((x2 - x1) * (y2 - y1)),
                "conf": conf,
                "img_w": img_w,
                "img_h": img_h,
            }

    # Push the annotated frame to the browser stream (best-effort).
    _publish_frame(frame, box_xyxy, conf, inference_ms)

    return {"status": "ok", "detection": detection, "inference_ms": inference_ms}
# ─────────────────────────────────────────────────────────────────────────────


# ─── Web UI HTTP server (separate daemon thread) ─────────────────────────────
def _page(title: str, stream_path: str, other_label: str, other_href: str) -> bytes:
    return (f"""<!DOCTYPE html>
<html><head><meta charset="utf-8"><title>{title}</title>
<style>
  body{{margin:0;background:#111;color:#ddd;font-family:monospace;text-align:center}}
  h1{{font-size:16px;font-weight:normal;padding:10px;margin:0;color:#888}}
  a{{color:#6cf;text-decoration:none}}
  nav{{padding:6px;font-size:13px}}
  img{{max-width:100%;height:auto;border:1px solid #333}}
</style></head>
<body>
  <h1>{title}</h1>
  <nav><a href="{other_href}">&rarr; {other_label}</a></nav>
  <img src="{stream_path}" alt="live stream">
</body></html>""").encode("utf-8")


RAW_HTML = _page("Camera &mdash; raw live feed", "/raw_stream",
                 "YOLO inference view", "/inference")
INFERENCE_HTML = _page("YOLO inference &mdash; live (largest person)", "/inference_stream",
                       "raw camera view", "/")


class WebHandler(BaseHTTPRequestHandler):
    def log_message(self, *args):  # silence per-request logging
        pass

    def do_GET(self):
        if self.path in ("/", "/index.html"):
            self._serve_html(RAW_HTML)
        elif self.path == "/inference":
            self._serve_html(INFERENCE_HTML)
        elif self.path == "/raw_stream":
            self._serve_stream(_raw_hub)
        elif self.path in ("/inference_stream", "/stream"):  # /stream = back-compat alias
            self._serve_stream(_inf_hub)
        else:
            self.send_error(404)

    def _serve_html(self, html: bytes):
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(html)))
        self.end_headers()
        self.wfile.write(html)

    def _serve_stream(self, hub: FrameHub):
        self.send_response(200)
        self.send_header("Age", "0")
        self.send_header("Cache-Control", "no-cache, private")
        self.send_header("Pragma", "no-cache")
        self.send_header("Content-Type", "multipart/x-mixed-replace; boundary=frame")
        self.end_headers()
        last = -1
        try:
            while True:
                jpeg, last = hub.get_after(last, timeout=1.0)
                if jpeg is None:
                    continue
                self.wfile.write(b"--frame\r\n")
                self.wfile.write(b"Content-Type: image/jpeg\r\n")
                self.wfile.write(f"Content-Length: {len(jpeg)}\r\n\r\n".encode())
                self.wfile.write(jpeg)
                self.wfile.write(b"\r\n")
        except (BrokenPipeError, ConnectionResetError):
            pass  # browser tab closed — normal
        except Exception as e:
            log.debug("stream client ended: %s", e)


def _start_web_server():
    server = ThreadingHTTPServer((HOST, WEB_PORT), WebHandler)
    t = threading.Thread(target=server.serve_forever, daemon=True)
    t.start()
    log.info("Web UI on http://localhost:%s ( / = raw camera, /inference = YOLO )", WEB_PORT)


# ─── WebSocket handlers ──────────────────────────────────────────────────────
#  Inference pipe (:8765) — runs YOLO, replies JSON, publishes annotated frame.
async def handle_client(connection):
    print("[INFO] inference connection open")
    try:
        async for message in connection:
            # 'message' contiene già TUTTI i byte del singolo frame JPEG inviato
            result = process_with_yolo(message)

            print(f"[INFO] Inference Time: {result.get('inference_ms')}")

            # Rispondi IMMEDIATAMENTE al client sullo stesso socket ancora aperto
            await connection.send(json.dumps(result))

    except websockets.exceptions.ConnectionClosed:
        print("[INFO] inference connection closed gracefully")
    except Exception as e:
        print(f"[ERROR] Error reading inference stream: {e}")


#  Raw camera pipe (:8766) — pure JPEG passthrough, NO inference. Receives
#  /camera/image_raw frames (already JPEG-encoded by the ROS sender) and pushes
#  them straight to the raw web hub. Independent of the inference pipe.
async def handle_raw_client(connection):
    print("[INFO] raw camera connection open")
    try:
        async for message in connection:
            _raw_hub.publish(message)   # JPEG bytes passthrough
    except websockets.exceptions.ConnectionClosed:
        print("[INFO] raw camera connection closed gracefully")
    except Exception as e:
        print(f"[ERROR] Error reading raw camera stream: {e}")


async def main():
    # Seed both pages so they render before any ROS client connects.
    _inf_hub.publish(_placeholder_jpeg("waiting for inference..."))
    _raw_hub.publish(_placeholder_jpeg("waiting for camera..."))
    _start_web_server()

    log.info("Inference WebSocket on %s:%s", HOST, PORT)
    log.info("Raw camera WebSocket on %s:%s", HOST, RAW_PORT)
    async with websockets.serve(handle_client, HOST, PORT, max_size=None), \
               websockets.serve(handle_raw_client, HOST, RAW_PORT, max_size=None):
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    asyncio.run(main())
