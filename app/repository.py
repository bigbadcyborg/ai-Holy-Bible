from __future__ import annotations

from pathlib import Path
from typing import Iterable

from app.models import Verse


class BibleRepository:
    """Loads and stores scripture verses from a plain-text source file."""

    def __init__(self, bible_path: str) -> None:
        self._bible_path = Path(bible_path)
        self._verses: list[Verse] = []

    def load(self) -> None:
        if self._verses:
            return

        if not self._bible_path.exists():
            raise FileNotFoundError(f"Bible text file was not found: {self._bible_path}")

        loaded: list[Verse] = []
        with self._bible_path.open("r", encoding="utf-8") as bible_file:
            for raw_line in bible_file:
                line = raw_line.strip()
                if not line:
                    continue

                tokens = line.split()
                chapter_index = next((i for i, token in enumerate(tokens) if ":" in token), -1)
                if chapter_index <= 0 or chapter_index >= len(tokens) - 1:
                    continue

                book_name = " ".join(tokens[:chapter_index])
                chapter_verse = tokens[chapter_index]
                verse_text = " ".join(tokens[chapter_index + 1 :])
                loaded.append(Verse(reference=f"{book_name} {chapter_verse}", text=verse_text))

        self._verses = loaded

    def all_verses(self) -> Iterable[Verse]:
        return self._verses
