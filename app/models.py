from dataclasses import dataclass


@dataclass(frozen=True)
class Verse:
    reference: str
    text: str
