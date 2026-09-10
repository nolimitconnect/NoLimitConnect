#!/usr/bin/env python3
# Copyright (C) 2026 Brett R. Jones
# See file COPYING and LEGAL in root of the No Limit Connect project
#
# Regenerates docs/assets/social-card.png, the Open Graph preview image used
# when nolimitconnect.org is linked anywhere. Run after any logo change.
#
#   python tools/make-social-card.py
#
# Requires Pillow only (no Cairo), so it stays runnable on Windows.

import base64
import io
import re
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "docs" / "assets" / "social-card.png"

W, H = 1200, 630
# Light ground on purpose: the wordmark lettering is a dark violet drawn for light
# backgrounds, and goes muddy on the site's dark slate palette.
BG = (244, 245, 250)

card = Image.new("RGB", (W, H), BG).convert("RGBA")

mark = Image.open(ROOT / "icons/1024x1024/org.nolimitconnect.NoLimitConnect.png").convert("RGBA")
mark_h = 250
mark = mark.resize((int(mark.width * mark_h / mark.height), mark_h), Image.LANCZOS)

# Reuse the original wordmark lettering embedded in the site logo, so the card
# never drifts from the real artwork and no font substitution is involved.
svg = (ROOT / "docs/assets/nlc-logo.svg").read_text(encoding="utf-8")
raw = base64.b64decode(re.search(r'href="data:image/png;base64,([^"]+)"', svg).group(1))
word = Image.open(io.BytesIO(raw)).convert("RGBA")
word_w = 600
word = word.resize((word_w, int(word.height * word_w / word.width)), Image.LANCZOS)

gap = 46
x = (W - (mark.width + gap + word.width)) // 2
cy = 250
card.paste(mark, (x, cy - mark.height // 2), mark)
card.paste(word, (x + mark.width + gap, cy - word.height // 2), word)

draw = ImageDraw.Draw(card)


def font(size, bold=False):
    for path in (r"C:\Windows\Fonts\segoeuib.ttf" if bold else r"C:\Windows\Fonts\segoeui.ttf",
                 "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold
                 else "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"):
        try:
            return ImageFont.truetype(path, size)
        except OSError:
            continue
    return ImageFont.load_default()


def centered(text, y, fnt, fill):
    width = draw.textbbox((0, 0), text, font=fnt)[2]
    draw.text(((W - width) // 2, y), text, font=fnt, fill=fill)


centered("Decentralized social networking you host yourself", 432, font(40), (38, 40, 54))
centered("Messaging \u00b7 Voice & video \u00b7 File sharing \u00b7 No central server", 492,
         font(28), (96, 100, 118))
centered("nolimitconnect.org", 556, font(26, bold=True), (88, 60, 190))

card.convert("RGB").save(OUT, optimize=True)
print(f"wrote {OUT.relative_to(ROOT)} ({W}x{H})")
