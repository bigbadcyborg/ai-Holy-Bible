from __future__ import annotations

import re
import string
from typing import Iterable

from app.models import Verse


BOOK_ORDER: dict[str, int] = {
    "genesis": 1,
    "exodus": 2,
    "leviticus": 3,
    "numbers": 4,
    "deuteronomy": 5,
    "joshua": 6,
    "judges": 7,
    "ruth": 8,
    "1 samuel": 9,
    "2 samuel": 10,
    "1 kings": 11,
    "2 kings": 12,
    "1 chronicles": 13,
    "2 chronicles": 14,
    "ezra": 15,
    "nehemiah": 16,
    "esther": 17,
    "job": 18,
    "psalms": 19,
    "proverbs": 20,
    "ecclesiastes": 21,
    "song of solomon": 22,
    "isaiah": 23,
    "jeremiah": 24,
    "lamentations": 25,
    "ezekiel": 26,
    "daniel": 27,
    "hosea": 28,
    "joel": 29,
    "amos": 30,
    "obadiah": 31,
    "jonah": 32,
    "micah": 33,
    "nahum": 34,
    "habakkuk": 35,
    "zephaniah": 36,
    "haggai": 37,
    "zechariah": 38,
    "malachi": 39,
    "matthew": 40,
    "mark": 41,
    "luke": 42,
    "john": 43,
    "acts": 44,
    "romans": 45,
    "1 corinthians": 46,
    "2 corinthians": 47,
    "galatians": 48,
    "ephesians": 49,
    "philippians": 50,
    "colossians": 51,
    "1 thessalonians": 52,
    "2 thessalonians": 53,
    "1 timothy": 54,
    "2 timothy": 55,
    "titus": 56,
    "philemon": 57,
    "hebrews": 58,
    "james": 59,
    "1 peter": 60,
    "2 peter": 61,
    "1 john": 62,
    "2 john": 63,
    "3 john": 64,
    "jude": 65,
    "revelation": 66,
}


class BibleSearchService:
    """Provides scripture search capabilities over loaded verses."""

    def __init__(self, verses: Iterable[Verse]) -> None:
        self._verses = list(verses)

    def query(self, keyword: str) -> list[Verse]:
        normalized = keyword.strip().lower()
        if not normalized:
            return []

        exact_match = [verse for verse in self._verses if verse.reference.lower() == normalized]
        if exact_match:
            return exact_match

        references = self._resolve_reference_expression(normalized)
        if references:
            return self._sort_verses(references)

        full_word_matches = [
            verse
            for verse in self._verses
            if self._contains_whole_word(verse.reference, normalized)
            or self._contains_whole_word(verse.text, normalized)
        ]
        return self._sort_verses(full_word_matches)

    def _resolve_reference_expression(self, normalized: str) -> list[Verse]:
        tokens = normalized.split()
        if not tokens:
            return []

        book_tokens: list[str] = []
        idx = 0
        while idx < len(tokens) and not any(ch.isdigit() for ch in tokens[idx]):
            book_tokens.append(tokens[idx])
            idx += 1

        book = " ".join(book_tokens).strip()
        if not book:
            return []

        if idx == len(tokens):
            return [verse for verse in self._verses if verse.reference.lower().startswith(f"{book} ")]

        tail = " ".join(tokens[idx:])
        range_match = re.fullmatch(r"(\d+)[–-](\d+)", tail)
        chapter_match = re.fullmatch(r"(\d+)", tail)

        if range_match:
            start = int(range_match.group(1))
            end = int(range_match.group(2))
            prefixes = [f"{book} {chapter}:" for chapter in range(start, end + 1)]
            return [
                verse
                for verse in self._verses
                if any(verse.reference.lower().startswith(prefix) for prefix in prefixes)
            ]

        if chapter_match:
            prefix = f"{book} {chapter_match.group(1)}:"
            return [verse for verse in self._verses if verse.reference.lower().startswith(prefix)]

        return []

    @staticmethod
    def _contains_whole_word(text: str, keyword: str) -> bool:
        words = [token.strip(string.punctuation).lower() for token in text.split()]
        return keyword.lower() in words

    def _sort_verses(self, verses: Iterable[Verse]) -> list[Verse]:
        return sorted(verses, key=lambda verse: self._parse_reference(verse.reference))

    @staticmethod
    def _parse_reference(reference: str) -> tuple[int, int, int]:
        tokens = reference.split()
        chapter_verse = next((token for token in tokens if ":" in token), "0:0")
        book = reference.rsplit(f" {chapter_verse}", maxsplit=1)[0].lower()

        chapter, verse = 0, 0
        if ":" in chapter_verse:
            chapter_str, verse_str = chapter_verse.split(":", maxsplit=1)
            chapter = int(chapter_str)
            verse = int(verse_str)

        return BOOK_ORDER.get(book, 999), chapter, verse
