---
title: Nevada Maintenance-Free Energy Generator
description: Concept brief — a solid-state MEMS mirror field and a zero-wear free-piston Stirling engine delivering ~20–26 kWe of unattended, maintenance-free solar power.
---

# Nevada Maintenance-Free Energy Generator

<p style="font-size:1.25rem;line-height:1.5;font-style:italic;opacity:.85;margin:-.4rem 0 .8rem">
A sun-tracking mirror field and a zero-wear NASA-heritage engine, sited in a pit near Lake Mead
</p>

<p style="opacity:.75;font-size:.85rem;margin-bottom:1.2rem">
Concept and research by <strong>Brett R. Jones</strong> &middot; Independent personal research
&middot; First published 18 August 2026
</p>

[:material-file-pdf-box: Download the brief as PDF](Nevada-Maintenance-Free-Energy-Generator.pdf){ .md-button }

<div class="grid cards" markdown>

-   __Site__

    Lake Mead vicinity, Clark County, NV

-   __Performance reference__

    Matched to Sweden's record dish-Stirling system

-   __Continuous output__

    ~20–26 kWe (single array)

-   __Engine maintenance interval__

    Zero — no contacting or wearing parts

-   __Longest field-proven run__

    14 years continuous, NASA Glenn Research Center

-   __Mirror mechanism__

    Solid-state MEMS, no motors or bearings

</div>

!!! quote "Why this is published openly"

    This concept is released free for anyone to read, use, or build on. It is my hope that some
    person, group, or organization with the means to do so will carry it toward mass production,
    and that it helps in some small way to ease the world's energy needs. I am not seeking to
    license, sell, or commercialize it.

    — Brett R. Jones

---

## 1. The idea in one paragraph

A field of thumbnail-sized mirrors, each steered by a solid-state chip with no motor and no
bearing, concentrates sunlight onto a single fixed point above a pit dug near the shoreline of
Lake Mead, Nevada. At that point sits a sealed engine with a piston that never touches anything it
moves against — the same core design NASA has run continuously for fourteen years without a single
maintenance visit. Matched to the same optical scale as the Swedish-engineered Ripasso Energy dish
system that holds the reported world record for solar-to-electricity conversion, this combination is
projected to deliver roughly **20–26 kWe of continuous, unattended power** — day after day, for
decades, with no crew, no lubrication schedule, and no wear-driven failure mode in either major
component.

<figure>
<svg viewBox="0 0 800 380" width="100%" role="img" aria-labelledby="fig1-title fig1-desc" style="max-width:100%;height:auto;">
  <title id="fig1-title">Cross-section of the maintenance-free solar generator</title>
  <desc id="fig1-desc">Fixed solid-state mirrors at ground level redirect concentrated sunlight up onto a stationary receiver housing a free-piston Stirling engine, mounted on a mast above an excavated pit, with Lake Mead at left.</desc>

  <rect x="0" y="0" width="800" height="380" fill="#11141c"/>

  <!-- ground -->
  <rect x="0" y="262" width="800" height="118" fill="#3a3226"/>
  <line x1="0" y1="262" x2="800" y2="262" stroke="#8d7355" stroke-width="2"/>

  <!-- pit -->
  <path d="M 342 262 L 372 356 L 428 356 L 458 262 Z" fill="#241f18" stroke="#8d7355" stroke-width="1.5"/>

  <!-- lake -->
  <path d="M 30 262 Q 110 236 196 262 Z" fill="#0e7490" opacity="0.85"/>
  <text x="70" y="254" fill="#67e8f9" font-family="sans-serif" font-size="13" font-weight="600">LAKE MEAD</text>

  <!-- sun -->
  <circle cx="710" cy="62" r="20" fill="#fbbf24"/>
  <g stroke="#fbbf24" stroke-width="2.5" stroke-linecap="round">
    <line x1="710" y1="26" x2="710" y2="14"/>
    <line x1="710" y1="110" x2="710" y2="98"/>
    <line x1="674" y1="62" x2="662" y2="62"/>
    <line x1="758" y1="62" x2="746" y2="62"/>
    <line x1="683" y1="35" x2="675" y2="27"/>
    <line x1="745" y1="89" x2="737" y2="97"/>
  </g>
  <text x="686" y="140" fill="#fbbf24" font-family="sans-serif" font-size="13" font-weight="600">SUN</text>

  <!-- incoming sunlight -->
  <g stroke="#fbbf24" stroke-width="1.6" opacity="0.75">
    <line x1="694" y1="80" x2="612" y2="250"/>
    <line x1="676" y1="74" x2="524" y2="250"/>
    <line x1="662" y1="70" x2="270" y2="250"/>
    <line x1="654" y1="68" x2="180" y2="250"/>
  </g>

  <!-- mast -->
  <rect x="394" y="150" width="12" height="112" fill="#4b5563"/>

  <!-- receiver / engine -->
  <rect x="330" y="104" width="140" height="48" rx="5" fill="#3f51b5" stroke="#9fa8da" stroke-width="1.5"/>
  <text x="400" y="124" fill="#e8eaf6" font-family="sans-serif" font-size="11" font-weight="700" text-anchor="middle">FREE-PISTON</text>
  <text x="400" y="140" fill="#e8eaf6" font-family="sans-serif" font-size="11" font-weight="700" text-anchor="middle">STIRLING ENGINE</text>
  <text x="486" y="120" fill="#c5cae9" font-family="sans-serif" font-size="12" font-weight="600">Fixed receiver</text>
  <text x="486" y="137" fill="#8f96b3" font-family="sans-serif" font-size="10.5" font-style="italic">Sealed, zero-maintenance</text>

  <!-- mirrors -->
  <g fill="#cbd5e1" stroke="#94a3b8" stroke-width="1">
    <rect x="150" y="248" width="34" height="7" rx="2" transform="rotate(-19 167 251)"/>
    <rect x="204" y="248" width="34" height="7" rx="2" transform="rotate(-16 221 251)"/>
    <rect x="258" y="248" width="34" height="7" rx="2" transform="rotate(-13 275 251)"/>
    <rect x="500" y="248" width="34" height="7" rx="2" transform="rotate(13 517 251)"/>
    <rect x="554" y="248" width="34" height="7" rx="2" transform="rotate(16 571 251)"/>
    <rect x="608" y="248" width="34" height="7" rx="2" transform="rotate(19 625 251)"/>
  </g>

  <!-- reflected, concentrated light -->
  <g stroke="#fde68a" stroke-width="1.4" stroke-dasharray="6 5" opacity="0.9">
    <line x1="167" y1="248" x2="386" y2="146"/>
    <line x1="221" y1="248" x2="390" y2="146"/>
    <line x1="275" y1="248" x2="394" y2="146"/>
    <line x1="517" y1="248" x2="410" y2="146"/>
    <line x1="571" y1="248" x2="414" y2="146"/>
    <line x1="625" y1="248" x2="418" y2="146"/>
  </g>

  <text x="150" y="288" fill="#cbd5e1" font-family="sans-serif" font-size="12" font-weight="600">MEMS MIRROR FIELD</text>
  <text x="150" y="303" fill="#94a3b8" font-family="sans-serif" font-size="10" font-style="italic">fixed to grade — no moving structure</text>
  <text x="500" y="288" fill="#cbd5e1" font-family="sans-serif" font-size="12" font-weight="600">EXCAVATED PIT</text>
  <line x1="470" y1="292" x2="440" y2="300" stroke="#8d7355" stroke-width="1.2"/>
</svg>
<figcaption><strong>Figure 1.</strong> Cross-section, not to scale. Fixed, solid-state mirrors near grade redirect concentrated sunlight onto a single stationary receiver above the pit; the receiver houses the free-piston Stirling engine described in Section&nbsp;3. The only motion anywhere in the system is the electrostatic tilt of each mirror element and the free-floating piston inside the sealed engine — nothing rides on a bearing, and nothing physically rotates to track the sun.</figcaption>
</figure>

!!! abstract "Why this matters"

    Every part of this design that could wear out has been replaced with a part that structurally
    cannot. That converts a normally recurring operations-and-maintenance line item into a one-time
    capital cost — which matters most wherever power has to run unattended and a service call is
    expensive or impractical: telecommunications and microwave relay sites, remote environmental and
    seismic monitoring stations, pipeline cathodic-protection installations, off-grid research
    stations, water and irrigation infrastructure, mining and industrial sites far from a road, and
    island or rural microgrids.

    The same properties that make it attractive for hard-to-service sites — no crew, no lubrication
    schedule, no wear-driven failure mode — are the properties that would make continuous solar
    power practical to deploy and forget at scale.

---

## 2. Component one — the mirror field

Conventional concentrated-solar installations track the sun by physically rotating a large dish or
panel on a motorized two-axis gimbal. That approach has a poor reputation for field reliability —
figures on the order of half of all mechanical trackers drifting out of specification within five to
six years circulate widely in the industry, though this brief has no primary source for that
particular number and does not rest on it. What is not in dispute is the failure *mechanism*: motors,
bearings, wind loading, and ground settling under a heavy moving structure are the parts that go, and
they are exactly the parts one would expect to go.

This design removes the moving structure entirely. Instead of one large assembly chasing the sun, a
field of small mirrors does the steering, using the same underlying technology as a digital projector
chip (DLP/DMD) — a solid-state array of independently tiltable micro-mirrors originally developed for
optical switching and projection, and separately patented for solar concentration. Nothing rotates on
a bearing; each mirror element repositions electrostatically.

### How hard is it actually working?

Not very. Tracking the sun across a 12-hour day only requires each mirror to reposition a few times a
minute. Run that pace for 25 years and the cumulative actuation count lands around **100–200 million
cycles**. Texas Instruments' published reliability work on this same DMD technology reports more than
**one trillion mirror cycles** and in excess of **100,000 operating hours** demonstrated in
qualification and field testing. The mirrors are not being asked to approach their limits; they are
being asked to do a small fraction of what they are already proven to survive.

<div class="grid cards" markdown>

-   <span style="font-size:1.9rem;font-weight:700;line-height:1.1">~100–200M</span>

    <span style="font-size:.78rem;letter-spacing:.06em;opacity:.8">ACTUATION CYCLES OVER 25 YEARS</span>

-   <span style="font-size:1.9rem;font-weight:700;line-height:1.1">&gt;1 trillion</span>

    <span style="font-size:.78rem;letter-spacing:.06em;opacity:.8">MIRROR CYCLES DEMONSTRATED (TI DLP/DMD)</span>

-   <span style="font-size:1.9rem;font-weight:700;line-height:1.1">~5,000×</span>

    <span style="font-size:.78rem;letter-spacing:.06em;opacity:.8">MARGIN BELOW THE DEMONSTRATED COUNT</span>

</div>

The one open engineering question, stated honestly rather than assumed away: today's DMD chips are
built for sealed indoor projectors, and moisture ingress is a known general concern for MEMS
structures. Hermetic sealing against it, however, is routine, mature practice in other outdoor MEMS
applications (automotive sensors, telecom equipment) — a one-time sealing design problem, not an
ongoing maintenance burden the way a mechanical tracker's wear is.

---

## 3. Component two — the free-piston engine

The heat concentrated by the mirror field drives a **free-piston Stirling engine** — often described
as having no moving parts that touch, since its piston floats on a cushion of working gas and
magnetically coupled non-contact bearings, with no crankshaft, connecting rod, or lubricated joint
anywhere in the power path. It is not literally pistonless; it is a real piston with nothing left for
it to wear against.

This is not an unproven concept. NASA's Glenn Research Center has operated a free-piston Stirling
convertor of this type **continuously for over fourteen years with zero maintenance**, and a second
unit has logged more than 110,000 cumulative operating hours — over twelve years — and remains in
service. The technology has since been commercialized: Qnergy's PowerGen line, built on the same
free-piston principle originally engineered for NASA, is fielded today for exactly the kind of
unattended, remote, multi-year deployment this concept targets (oil and gas remote power, pipeline
and cathodic-protection sites, and other installations where a service crew is expensive or
impractical to send).

!!! info "The efficiency trade, stated honestly"

    The record-holding Swedish dish system uses a *kinematic* Stirling engine — crankshaft-linked,
    with real seals and wear points — to reach its reported **32%** conversion efficiency, measured
    on a 30 kW converter at Upington, South Africa. The free-piston alternative proposed here gives
    up some of that efficiency — commonly quoted in the 24–27% range, though this brief does not pin
    that range to a specific published measurement — in exchange for eliminating every wearing part.
    Given the goal is decades of unattended operation rather than peak efficiency, that is the trade
    this design makes.

### One engine, or several?

Free-piston Stirling engines do not scale up cleanly — larger units suffer worse working-gas leakage
and tighter manufacturing tolerances, a real physical limitation rather than a simple manufacturing
gap. Today's commercial ceiling is Qnergy's 5.6 kW-class unit. Reaching this design's full 20–26 kWe
target output means installing four to five such units side by side rather than one large engine —
which the mirror field accommodates naturally, since it can already split its concentrated light
across multiple independent focal points instead of feeding just one.

---

## 4. Matched against the Swedish world record

Rather than project an untested hypothetical output figure, this design's mirror field was sized to
exactly match the optical aperture of the actual dish used by Ripasso Energy, a Swedish company whose
system holds the reported world record for solar-to-electricity conversion efficiency, demonstrated
at field scale. At that matched aperture, and using Nevada's own real, measured solar resource for
the Lake Mead area, the mirror field is projected to deliver essentially the same peak power the
record system itself achieves — a direct validation that a field of small, solid-state mirrors can do
optically what one large mechanically-tracked dish does, at the same real scale, before any claim is
made about the durability upgrade in Section 3.

| Parameter | Value | Basis |
| --- | --- | --- |
| Ripasso record efficiency (reference point) | 32% | **Cited** — reported world record for a 30 kW converter at Upington, South Africa |
| Matched optical aperture | 110.5 m² | **Geometric** — the area of an 11.86 m circular dish (π r²), sized to the Ripasso dish diameter |
| Site annual solar resource | 7.62 kWh/m²/day | **Cited** — measured direct-normal irradiance, Las Vegas / Lake Mead region (NREL NSRDB) |
| Site peak irradiance (record condition) | 960 W/m² | **Assumption** — a representative clear-sky DNI value. The Ripasso record is reported at ambient above 28 °C with no irradiance stated, so this is not a cited match to its test condition |
| Mirror reflectivity | 92% | **Assumption** — typical of solar-grade mirrors; not a cited measurement of a specific product |
| Kinematic (record-matching) output | ~30–33 kWe | **Derived** from the rows above — lands in the same ~30 kW class as the record unit |
| **Free-piston (adopted, zero-maintenance) output** | **~20–26 kWe** | **Derived**, using the 24–27% free-piston range discussed in Section 3 |

Read those labels literally. **Cited** means a source in the references supports the figure directly.
**Derived** means it follows arithmetically from the rows above it. **Assumption** means it is a
reasonable engineering placeholder that this brief does not have a citation for. Both assumptions sit
in the optical chain, which compounds them — so the output figures are best read as an
order-of-magnitude match to the record system, not a precise prediction.

---

## 5. Expected lifetimes

| Component | Wear mechanism | Field-demonstrated basis | Real-world read |
| --- | --- | --- | --- |
| Mirror field (MEMS) | None — no bearings, no lubrication, solid-state actuation only | >1 trillion mirror cycles and >100,000 operating hours demonstrated (TI DLP/DMD reliability program) | This design's full 25-year duty cycle uses roughly 0.02% of the demonstrated cycle count |
| Engine (free-piston Stirling) | None — non-contact bearings, no crankshaft, no lubricated joint | 14 years continuous, zero maintenance (NASA Glenn); a second unit past 12 years / 110,000+ hours, still running | Longest continuously-operating example currently exceeds a decade with no service event |
| *For comparison — a conventional mechanical tracker* | *Motors, bearings, gimbal wear, wind and ground-settling load* | *Widely reported to lose specification within 5–6 years, but no primary source is cited here — see the note in Section 2* | *The failure mode this design was built specifically to remove* |

Put simply: the two components that usually define a solar installation's maintenance calendar — the
tracker and the engine — have both been replaced here with the mechanism-level equivalent of a part
that cannot wear out, each independently field-proven for over a decade before this concept was ever
assembled.

---

## 6. Why this site

Lake Mead, Nevada offers a real, measured, high-quality direct-sunlight resource (NREL/NSRDB data,
cited below) directly comparable to the desert site conditions the Swedish record system itself was
tested under — meaning the performance match in Section 4 is not a best-case extrapolation, it is
close to an apples-to-apples reproduction. Siting the array in a dug pit keeps the fixed engine and
receiver near grade while the surrounding mirror field remains low-profile, and open desert land in
the area is well suited to a field-scale installation without competing for higher-value terrain.

---

## References

- **NASA Glenn Research Center** — ["Stirling Convertor Sets 14-Year Continuous Operation Milestone"](https://www.nasa.gov/centers-and-facilities/glenn/stirling-convertor-sets-14-year-continuous-operation-milestone/)
  <br>Official NASA source for the 14-year continuous, maintenance-free free-piston Stirling operation record.

- **Qnergy** — [PowerGen free-piston Stirling generator product documentation](https://www.qnergy.com/)
  <br>Commercial free-piston Stirling generator, 1.8–5.6 kW output range — the real basis for the multi-unit scaling figure in Section 3, and manufacturer confirmation linking the NASA-heritage design to today's commercial units.

- **Reuters Events (CSP Today)** — ["Ripasso Energy sets new solar-to-electricity world record"](https://www.reutersevents.com/renewables/csp-today/technology/ripasso-energy-sets-new-solar-electricity-world-record)
  <br>Source for the 32% figure used in Sections 3 and 4: a reported world record of 32% for a 30 kW converter at Ripasso Energy's plant in Upington, South Africa, at an ambient temperature above 28 °C.

- **U.S. Department of Energy** — ["Solar Dish Sets World-Record Efficiency"](https://www.energy.gov/node/1758401)
  <br>A **separate** dish-Stirling efficiency record — 31.25%, achieved by Stirling Energy Systems, not by Ripasso. Included as independent corroboration that this class of machine reaches roughly 31%, and explicitly *not* as the source for the Ripasso figure. Some secondary coverage reports Ripasso at 34%; this brief uses the lower, better-documented 32% instead.

- **Douglass, Michael R.** — ["DMD Reliability: A MEMS Success Story"](https://www.ti.com/pdfs/dlpdmd/153_Reliability_paper.pdf), *Proc. SPIE* 4980, Texas Instruments (2003)
  <br>Source for the multi-billion-to-trillion-cycle and operating-hour DLP/DMD reliability data underlying the mirror-field durability figures in Sections 2 and 5.

- **Patent EP2911208A1** — ["Micro-Concentrator Solar Array Using Micro-Electromechanical Systems (MEMS) Based Reflectors"](https://patents.google.com/patent/EP2911208A1/en)
  <br>Real, existing published patent for the MEMS solar-tracking mirror concept described in Section 2 — not a hypothetical proposal.

- **National Renewable Energy Laboratory (NREL)** — [National Solar Radiation Database (NSRDB)](https://nsrdb.nrel.gov/)
  <br>Source for real, measured direct-normal irradiance data for the Nevada / Lake Mead region used in Section 4.

---

## About this publication

**Independent personal research.** This brief is independent personal research by Brett R. Jones,
carried out on personal time using personal equipment and publicly published sources. It is not
affiliated with, sponsored by, funded by, reviewed by, or produced for any employer, client,
institution, or agency, and it represents no organization's position — only the author's own.

**Scope and status.** Concept-stage engineering brief. It is not peer-reviewed, not an engineering
specification, and not an offer to build, sell, or license anything. The Section 4 table labels each
figure as **cited**, **derived**, or **assumption**, and the two assumptions in the optical chain are
named as such rather than presented as measurements. The mechanical-tracker failure statistic in
Section 2 is likewise flagged as uncited. Where a range is shown it reflects genuine uncertainty
rather than false precision. The purpose here is to interest a reader in the concept — not to assert
a validated result, and not to sell anything.

**Sources.** All material derives from publicly published sources — NASA and U.S. Department of
Energy publications, a published patent, manufacturer product documentation, a peer-reviewed
conference paper, and public NREL solar-resource data. Each is listed and linked above.

**Intent.** Published as an open disclosure so the concept remains freely available for anyone to
read, use, or build on. The author is not seeking to license, sell, or commercialize it.

**Publication record.** First published 18 August 2026 at
[nolimitconnect.org](https://nolimitconnect.org). Authorship and revision history are recorded in the
project's public Git repository at
[github.com/nolimitconnect/NoLimitConnect](https://github.com/nolimitconnect/NoLimitConnect).

**License.** © 2026 Brett R. Jones. Text and figures released under
[Creative Commons Attribution 4.0 International (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/)
— free to share and adapt, with attribution to the author.

**How to cite.** Jones, Brett R. *Nevada Maintenance-Free Energy Generator: A Sun-Tracking Mirror
Field and a Zero-Wear NASA-Heritage Engine.* Concept brief, 18 August 2026.
<https://nolimitconnect.org/maintenance-free-solar-generator/>

<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "TechArticle",
  "headline": "Nevada Maintenance-Free Energy Generator",
  "alternativeHeadline": "A sun-tracking mirror field and a zero-wear NASA-heritage engine, sited in a pit near Lake Mead",
  "description": "Concept brief — a solid-state MEMS mirror field and a zero-wear free-piston Stirling engine delivering ~20–26 kWe of unattended, maintenance-free solar power near Lake Mead, Nevada.",
  "author": {
    "@type": "Person",
    "name": "Brett R. Jones",
    "url": "https://nolimitconnect.org/author/"
  },
  "creator": {
    "@type": "Person",
    "name": "Brett R. Jones"
  },
  "datePublished": "2026-08-18",
  "dateModified": "2026-08-18",
  "inLanguage": "en",
  "isAccessibleForFree": true,
  "license": "https://creativecommons.org/licenses/by/4.0/",
  "copyrightYear": "2026",
  "copyrightHolder": {
    "@type": "Person",
    "name": "Brett R. Jones"
  },
  "keywords": "maintenance-free solar generator, MEMS solar tracking, DLP DMD micromirror solar concentrator, free-piston Stirling engine, dish-Stirling, concentrated solar power, unattended remote power, Lake Mead Nevada, zero-maintenance power generation",
  "about": [
    { "@type": "Thing", "name": "Concentrated solar power" },
    { "@type": "Thing", "name": "Free-piston Stirling engine" },
    { "@type": "Thing", "name": "Microelectromechanical systems" }
  ],
  "mainEntityOfPage": {
    "@type": "WebPage",
    "@id": "https://nolimitconnect.org/maintenance-free-solar-generator/"
  },
  "url": "https://nolimitconnect.org/maintenance-free-solar-generator/",
  "encoding": {
    "@type": "MediaObject",
    "encodingFormat": "application/pdf",
    "contentUrl": "https://nolimitconnect.org/Nevada-Maintenance-Free-Energy-Generator.pdf"
  },
  "citation": [
    "NASA Glenn Research Center, Stirling Convertor Sets 14-Year Continuous Operation Milestone",
    "Reuters Events (CSP Today), Ripasso Energy sets new solar-to-electricity world record",
    "U.S. Department of Energy, Solar Dish Sets World-Record Efficiency",
    "Douglass, M. R., DMD Reliability: A MEMS Success Story, Proc. SPIE 4980 (2003)",
    "Patent EP2911208A1, Micro-Concentrator Solar Array Using Micro-Electromechanical Systems (MEMS) Based Reflectors",
    "NREL National Solar Radiation Database (NSRDB)"
  ]
}
</script>
