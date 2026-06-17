# Inference Server Web UI — Design Spec

**Date:** 2026-06-17
**Component:** `_INFERENCE_SERVER/src/server.py`
**Status:** Approved design, pending implementation

## Goal

Add a localhost web page that displays the live camera frames flowing into the
YOLO inference server, with the detection result (bounding box) drawn on top.
The page must be reachable from the host browser at `http://localhost:8080`
while the server runs inside Docker.

## Context

The inference server is a standalone Dockerized YOLOv8n WebSocket service. The
ROS node `person_detector_node` JPEG-encodes camera frames, ships them over a
WebSocket to `:8765`, and the server replies with JSON describing the single
largest-area person detection (`cx`, `cy`, `area`, `conf`, `img_w`, `img_h`,
`inference_ms`).

Notes on current state (differs from `CLAUDE.md`):
- Only `server.py` exists. The previously documented `stream_server.py` (debug
  GUI on `:8766`, XQuartz) and `start.sh` are gone. This web UI replaces that
  removed debug stream with a browser-based one.
- `requirements.txt` uses `opencv-python-headless`. The web UI must not require
  full OpenCV / HighGUI — it uses only `cv2.rectangle`, `cv2.putText`,
  `cv2.imencode`, none of which need a GUI.

The web UI can only display frames that flow through the server (the frames the
ROS client sends). The server has no camera of its own.

## Decisions (locked during brainstorming)

1. **Box scope:** Draw only the single returned largest-person box — exact
   mirror of the JSON payload sent to ROS. Not all persons, not all classes.
2. **Annotation location:** Server-side. The server burns the box onto the JPEG
   with OpenCV and streams an annotated MJPEG. No browser-side canvas / JS
   drawing.
3. **View mode:** Live auto-stream (continuous MJPEG `multipart/x-mixed-replace`),
   not poll-on-refresh.

## Architecture

Single container, single Python process, two ports, decoupled:

- `:8765` — existing WebSocket inference path for ROS. **Behavior unchanged.**
- `:8080` — new HTTP web server, run in a **daemon thread** using stdlib
  `http.server.ThreadingHTTPServer`. No asyncio coupling, so a slow/blocked
  browser client cannot stall the inference event loop.

### Shared state

A module-level holder guarded by a `threading.Condition`:

- `latest_jpeg: bytes` — the most recent annotated JPEG.
- `version: int` — incremented every time `latest_jpeg` is replaced.

The websockets coroutine (producer) writes; HTTP stream handlers (consumers)
wait on the condition for a new `version`, then send the latest JPEG.

### Data flow

1. Frame arrives from ROS on `:8765`.
2. `process_with_yolo(image_bytes)` decodes the JPEG → BGR frame and runs YOLO
   (unchanged).
3. After picking the largest person box, the same function (which already holds
   the decoded `frame` and the box coords) draws:
   - `cv2.rectangle` around `(x1,y1)-(x2,y2)`.
   - `cv2.putText` label: `person {conf:.2f} | {inference_ms} ms`.
   - When no detection: frame is published as-is (optionally a small "no
     detection" label).
   It then `cv2.imencode(".jpg", frame)` → bytes and publishes to shared state
   (set `latest_jpeg`, bump `version`, `notify_all`). No second decode.
4. HTTP endpoints:
   - `GET /` → minimal HTML page containing `<img src="/stream">`.
   - `GET /stream` → response `Content-Type: multipart/x-mixed-replace;
     boundary=frame`; a loop waits on the condition for a new `version` and
     writes one JPEG part per update.

### Startup

`main()`:
1. Build a placeholder JPEG ("waiting for frames…") and seed `latest_jpeg` so
   the page renders before ROS connects.
2. Start the HTTP server in a daemon thread on `WEB_PORT`.
3. `await websockets.serve(...)` as today.

## Configuration

- New env var `WEB_PORT` (default `8080`). Existing `HOST`/`PORT`/`MODEL_PATH`/
  `DEVICE`/`CONF_THRESHOLD` unchanged.
- `Dockerfile`: add `EXPOSE 8080` (documentation; publish is done at `run`).
- `run_inference_server.sh`: add `-p 8080:8080` alongside `-p 8765:8765`.

## Error handling

- Browser disconnect mid-stream → `BrokenPipeError`/`ConnectionResetError` in the
  HTTP handler is caught and the stream loop exits cleanly; never propagates to
  the inference path.
- `cv2.imdecode` returns `None` → existing error path returns JSON error; the web
  frame is left unchanged (last good frame stays on screen).
- Annotation/encode failure → logged and swallowed; must not break the JSON reply
  to ROS. The inference reply on `:8765` is the critical product; the web frame
  is best-effort.
- Multiple browser tabs → each `/stream` request gets its own consumer loop over
  the same shared frame. Supported.

## Performance

Per frame, added work beyond existing YOLO + decode: one `cv2.rectangle`, one
`cv2.putText`, one `cv2.imencode`. Negligible against YOLO CPU inference. The
HTTP thread only re-sends on a new `version`, so idle browsers do no encode work.

## Testing / verification

- `docker build` succeeds with no new dependency (stdlib + existing opencv).
- `run_inference_server.sh` publishes `:8080`; `http://localhost:8080` loads the
  page from the host browser (outside Docker).
- Before any ROS frame: page shows the placeholder.
- With the ROS client (or a manual WebSocket test client) pushing JPEG frames:
  the page shows live frames; when a person is present, a box + `conf`/`ms`
  label is drawn; the `:8765` JSON reply is unchanged.
- Killing a browser tab does not disturb inference (server logs stay clean on the
  inference path).

## Out of scope (YAGNI)

- Drawing all persons or all COCO classes.
- Browser-side canvas overlay / interactive toggles.
- Authentication, HTTPS, recording/playback, multi-camera.
- Any change to the `:8765` JSON contract.
