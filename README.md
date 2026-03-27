# Holy Bible Web App (Docker-ready)

This repository now provides a web API for Bible verse lookup using FastAPI.

## Features
- `GET /health` for health checks.
- `GET /search?q=<query>` for:
  - exact references (e.g., `John 3:16`)
  - book chapter lookups (e.g., `Romans 8`)
  - chapter ranges (e.g., `Psalms 1-3`)
  - keyword search (whole-word match)

## Run locally (without Docker)
```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```

Then open: `http://localhost:8000/docs`

## Run with Docker
```bash
docker build -t holy-bible-web .
docker run --rm -p 8000:8000 holy-bible-web
```

Or with Docker Compose:
```bash
docker compose up --build
```

## Example query
```bash
curl "http://localhost:8000/search?q=John%203:16"
```
