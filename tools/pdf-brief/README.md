# PDF print master — Nevada Maintenance-Free Energy Generator

`nevada-energy-generator-print.html` is the print master used to regenerate
`docs/Nevada-Maintenance-Free-Energy-Generator.pdf`.

It lives here, **outside `docs/`**, on purpose: MkDocs copies everything under `docs/` into the
published site, so an `.html` file there would be served at its own URL and compete with
`docs/maintenance-free-solar-generator.md` as duplicate content.

## Regenerating the PDF

1. Open `nevada-energy-generator-print.html` in Chrome or Edge and press `Ctrl+P`.
2. Destination **Save as PDF**, paper **Letter**, margins **Default**, scale **100%**.
3. Under *More settings*: **Background graphics ON** (without it the navy headers, callout boxes and
   stat panels print blank) and **Headers and footers OFF**.
4. Save over `docs/Nevada-Maintenance-Free-Energy-Generator.pdf`.

The yellow instruction banner at the top of the file is screen-only and does not print.

## Keep the two in sync

The web page at `docs/maintenance-free-solar-generator.md` and this print master carry the same text.
Edit both, or the PDF and the site will drift apart.

Two divergences from the original 2026-08-18 PDF are deliberate — do not reintroduce the old wording:

- The Section 2 callout was retitled from *"Why this is worth a government official's attention"* to
  **"Why this matters"**, and its application list changed from forward installations / border and
  land-management infrastructure / federal land facilities to civilian and commercial examples
  (telecom relays, remote monitoring, pipeline cathodic protection, microgrids, and so on).
- The mirror-field reliability figures were changed to the ones the cited Texas Instruments paper
  verifiably supports — **>1 trillion mirror cycles**, **>100,000 operating hours**, **~5,000x
  margin**, and **~0.02%** of the demonstrated cycle count in the Section 5 table. They previously
  read 20 billion cycles / 650,000-hour MTBF / ~1%, neither of which appears in the cited source.

## Known open item

The brief states Ripasso Energy's record as **34%**. DOE and Reuters coverage report **32%** on a
30 kW unit at Upington, South Africa; 34% appears mainly in secondary coverage. The figure is
currently hedged as "reported" in both files and still needs a decision.
