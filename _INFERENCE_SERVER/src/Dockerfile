FROM python:3.11-slim

# Install system dependencies required for OpenCV on ARM
RUN apt-get update && apt-get install -y \
    libgl1 \
    libglib2.0-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 1. FORCE install CPU-only PyTorch first from the official PyTorch repo
RUN pip install --no-cache-dir torch torchvision --index-url https://download.pytorch.org/whl/cpu

# 2. Now copy and install the rest of your requirements
COPY requirements.txt .
RUN pip install --default-timeout=1000 --no-cache-dir -r requirements.txt

COPY server.py .

CMD ["python", "server.py"]