from __future__ import annotations

import os

from fastapi import FastAPI, HTTPException, Query

from app.repository import BibleRepository
from app.search_service import BibleSearchService

app = FastAPI(title="Holy Bible Search API", version="1.0.0")

BIBLE_PATH = os.environ.get("BIBLE_PATH", "factBase/kjv.txt")
repository = BibleRepository(BIBLE_PATH)
search_service: BibleSearchService | None = None


@app.on_event("startup")
def startup_event() -> None:
    global search_service
    repository.load()
    search_service = BibleSearchService(repository.all_verses())


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok"}


@app.get("/search")
def search(q: str = Query(..., min_length=1, description="Reference or keyword")) -> dict[str, object]:
    if search_service is None:
        raise HTTPException(status_code=503, detail="Service not ready")

    matches = search_service.query(q)
    return {
        "query": q,
        "count": len(matches),
        "results": [{"reference": verse.reference, "text": verse.text} for verse in matches],
    }
