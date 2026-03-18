#!/usr/bin/env python3
"""Generate translated Qt .ts files for supported NoLimitConnect languages.

Uses translate.googleapis.com (client=gtx) for automated translation.
"""

from __future__ import annotations

import json
import argparse
import re
import sys
import time
from pathlib import Path
from typing import Dict, List, Tuple
import xml.etree.ElementTree as ET

import requests


BASE_TS = Path(r"f:\nolimitconnect\translations\nolimitconnect_en_EN.ts")
OUT_DIR = Path(r"f:\nolimitconnect\translations")

SUPPORTED = [
    ("de", "de_DE"),
    ("zh-CN", "zh_CN"),
    ("es", "es_ES"),
    ("fr", "fr_FR"),
    ("ar", "ar_SA"),
    ("hi", "hi_IN"),
    ("pt", "pt_PT"),
    ("ja", "ja_JP"),
    ("ko", "ko_KR"),
    ("ru", "ru_RU"),
    ("id", "id_ID"),
]

TOKEN_PATTERNS = [
    re.compile(r"https?://\S+", re.IGNORECASE),
    re.compile(r"%\d+"),
    re.compile(r"%n"),
    re.compile(r"%L\d+"),
    re.compile(r"<[^>]+>"),
]

SEP = "⟪NLCSEP_4271⟫"
USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)"


def make_batches(items: List[str], max_items: int = 30, max_chars: int = 2500) -> List[List[str]]:
    batches: List[List[str]] = []
    current: List[str] = []
    chars = 0
    for item in items:
        add_chars = len(item) + len(SEP) + 2
        if current and (len(current) >= max_items or chars + add_chars > max_chars):
            batches.append(current)
            current = []
            chars = 0
        current.append(item)
        chars += add_chars
    if current:
        batches.append(current)
    return batches


def mask_tokens(text: str) -> Tuple[str, Dict[str, str]]:
    mapping: Dict[str, str] = {}
    masked = text
    idx = 0

    def replace_match(match: re.Match[str]) -> str:
        nonlocal idx
        token = f"__NLC_TOKEN_{idx}__"
        mapping[token] = match.group(0)
        idx += 1
        return token

    for pat in TOKEN_PATTERNS:
        masked = pat.sub(replace_match, masked)

    return masked, mapping


def unmask_tokens(text: str, mapping: Dict[str, str]) -> str:
    out = text
    for token, original in mapping.items():
        out = out.replace(token, original)
    return out


def translate_batch(lines: List[str], target_lang: str) -> List[str]:
    payload = ("\n" + SEP + "\n").join(lines)
    params = {
        "client": "gtx",
        "sl": "en",
        "tl": target_lang,
        "dt": "t",
        "q": payload,
    }

    last_err = None
    for attempt in range(4):
        try:
            response = requests.get(
                "https://translate.googleapis.com/translate_a/single",
                params=params,
                headers={"User-Agent": USER_AGENT},
                timeout=40,
            )
            response.raise_for_status()
            data = response.json()
            translated = "".join(part[0] for part in data[0])
            split = translated.split(SEP)
            if len(split) != len(lines):
                raise RuntimeError(f"separator split mismatch: expected {len(lines)}, got {len(split)}")
            return [s.strip("\n") for s in split]
        except Exception as exc:  # pylint: disable=broad-except
            last_err = exc
            time.sleep(1.0 + attempt * 1.5)

    raise RuntimeError(f"batch translation failed after retries: {last_err}")


def translate_one(line: str, target_lang: str) -> str:
    return translate_batch([line], target_lang)[0]


def load_cache(path: Path) -> Dict[str, str]:
    if path.exists():
        return json.loads(path.read_text(encoding="utf-8"))
    return {}


def save_cache(path: Path, cache: Dict[str, str]) -> None:
    path.write_text(json.dumps(cache, ensure_ascii=False, indent=2, sort_keys=True), encoding="utf-8")


def translate_language(root: ET.Element, target_lang: str, ts_locale: str) -> ET.ElementTree:
    cache_path = OUT_DIR / f".cache_{ts_locale}.json"
    cache = load_cache(cache_path)

    sources: List[str] = []
    for msg in root.findall(".//message"):
        source_el = msg.find("source")
        if source_el is None or source_el.text is None:
            continue
        src = source_el.text
        if src.strip() and src not in sources:
            sources.append(src)

    pending = [s for s in sources if s not in cache]
    print(f"[{ts_locale}] unique={len(sources)} pending={len(pending)}")

    if pending:
        for batch in make_batches(pending):
            masked_batch: List[str] = []
            token_maps: List[Dict[str, str]] = []
            for item in batch:
                masked, mapping = mask_tokens(item)
                masked_batch.append(masked)
                token_maps.append(mapping)

            try:
                translated_batch = translate_batch(masked_batch, target_lang)
                for src, translated, mapping in zip(batch, translated_batch, token_maps):
                    cache[src] = unmask_tokens(translated, mapping)
            except Exception as batch_exc:  # pylint: disable=broad-except
                print(f"[{ts_locale}] batch failed ({batch_exc}); falling back to single-item translation")
                for src, masked_src, mapping in zip(batch, masked_batch, token_maps):
                    try:
                        translated = translate_one(masked_src, target_lang)
                        cache[src] = unmask_tokens(translated, mapping)
                    except Exception as one_exc:  # pylint: disable=broad-except
                        print(f"[{ts_locale}] single translate failed, using source: {one_exc}")
                        cache[src] = src

            save_cache(cache_path, cache)
            print(f"[{ts_locale}] translated {len(batch)} (cache {len(cache)})")

    tree = ET.ElementTree(ET.fromstring(ET.tostring(root, encoding="utf-8")))
    out_root = tree.getroot()
    out_root.set("language", ts_locale)

    for msg in out_root.findall(".//message"):
        source_el = msg.find("source")
        if source_el is None or source_el.text is None:
            continue

        src = source_el.text
        tr_el = msg.find("translation")
        if tr_el is None:
            tr_el = ET.SubElement(msg, "translation")

        tr_el.attrib.pop("type", None)
        translated_text = cache.get(src, src)
        tr_el.text = translated_text

    return tree


def indent_xml(elem: ET.Element, level: int = 0) -> None:
    indent = "    "
    i = "\n" + level * indent
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + indent
        for child in elem:
            indent_xml(child, level + 1)
        if not elem[-1].tail or not elem[-1].tail.strip():
            elem[-1].tail = i
    if level and (not elem.tail or not elem.tail.strip()):
        elem.tail = i


def write_ts(tree: ET.ElementTree, out_path: Path) -> None:
    root = tree.getroot()
    indent_xml(root)
    xml = ET.tostring(root, encoding="utf-8")
    content = b'<?xml version="1.0" encoding="utf-8"?>\n<!DOCTYPE TS>\n' + xml + b"\n"
    out_path.write_bytes(content)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--locales",
        nargs="*",
        help="Optional list of TS locales to generate, e.g. de_DE zh_CN",
    )
    args = parser.parse_args()

    if not BASE_TS.exists():
        print(f"Base TS not found: {BASE_TS}")
        return 1

    base_tree = ET.parse(BASE_TS)
    base_root = base_tree.getroot()

    selected = set(args.locales) if args.locales else None
    for google_lang, ts_locale in SUPPORTED:
        if selected and ts_locale not in selected:
            continue
        out_tree = translate_language(base_root, google_lang, ts_locale)
        out_file = OUT_DIR / f"nolimitconnect_{ts_locale}.ts"
        write_ts(out_tree, out_file)
        print(f"Wrote: {out_file}")

    print("Done.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
