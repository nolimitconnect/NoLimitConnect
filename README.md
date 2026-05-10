# No Limit Connect (NLC)

> **Seeking Lead Maintainer & Community Founders**  
> NLC is transitioning from a solo-developed engine to a community-led organization. We are looking for technical and community leaders to help scale to 1,000,000 users. [See Recruitment below](#-recruitment-lead-the-future-of-nlc).

## Summary
**Tagline:** Independent, user-hosted social networks — no registration, no ads, no data mining, no limits.  
**Stack:** C, C++, Assembly, Qt, OpenGL  
**Audience:** Developers and DIY enthusiasts building private or decentralized communities.  
**Core Values:** Privacy, sovereignty, and freedom from centralized platforms.  

---

### 💡 What it does
*   **User-Controlled Infrastructure:** Lets anyone host their own social network, group, or chat room.
*   **Hybrid P2P/Relay Architecture:** Uses user-hosted nodes to list communities and act as relays for users behind restrictive firewalls (no port-forwarding required).
*   **Privacy First:** Designed for security, zero spam, and no data collection.
*   **Native Performance:** Written for maximum efficiency with minimal dependencies.

### 🚀 Tech Highlights
Unlike modern web-based social apps, NLC is built for speed and low overhead:
*   **Pure C/C++ Core:** With hand-tuned **Assembly** for performance-critical components.
*   **Smart Networking:** Intelligent relaying for seamless connectivity across all network types.
*   **Media Engine:** Built on **Kodi/FFmpeg** with **OpenGL** shader-based rendering.
*   **Low-Latency Audio:** **Opus** for VOIP with **WebRTC** echo cancellation and **RNNoise** suppression.
*   **Cross-Platform:** Native builds for Windows, Linux (x64/ARM), and Android.

---

### 🛠 Under the Hood
- **GUI:** Qt-based interface with support for 12 languages.
- **Communications:** Video Chat, VOIP Phone, Push-to-Talk, and Messenger.
- **Media:** Streaming service for files, built-in media player, and Web Cam server.
- **Social Features:** Storyboard blogs, "About Me" pages, Chat Rooms, and a "Truth or Dare" video game.
- **Permissions:** Granular control (Ignore, Guest, Friend, Admin).

---

### 📥 Getting Started

**Precompiled Binaries & Installers:**  
Available at: [://nolimitconnect.com](https://://nolimitconnect.com)  
*(Windows NSIS, Android APK, Linux .deb, and Flatpak for x64/ARM64/Pi)*

**Source Code:**
```bash
git clone https://github.com/nolimitconnect/NoLimitConnect.git
```

---

### 🗺️ Roadmap to 1 Million Users
- [ ] **Establish Organization:** Migrate to GitHub and recruit core leadership.
- [ ] **Global Visibility:** Promote NLC as the premier privacy-first community tool.
- [ ] **App Stores:** Publish to the Google Play Store and Flathub.
- [ ] **Apple Ecosystem:** Port and validate for macOS and iOS (Help Wanted).
- [ ] **Documentation:** Migrate all developer docs to the repository and `.org` site.

---

### 🤝 Recruitment: Lead the Future of NLC
NLC has been developed primarily by a single maintainer (Brett R. Jones) over many years. While I will continue to develop the core C++/Assembly engine, my professional environment requires me to remain behind the scenes regarding public-facing social media.

**We are seeking a Lead Maintainer and Community Manager to take the reins of:**
1.  **Public Representation:** Acting as the "voice" of the project in the decentralized space.
2.  **Organization Management:** Directing the GitHub Org and the `nolimitconnect.org` community hub.
3.  **Community Outreach:** Moderating channels, managing bug reports, and onboarding users.

If you are passionate about privacy and want to help lead a project with a 1-million-user goal, please reach out:
**📧 Contact:** `bjones.engineer(AT)gmail.com`

---

### ⚖️ License & Legal
*   **Code by Brett R. Jones:** Dual-licensed (similar to Ruby’s license); see individual source files.
*   **Third-Party:** Various open-source licenses apply (see `LICENSE` and `LEGAL` files).
*   **Principles:** No ads. No data mining. No analytics. No tracking.

**Project Status:** Active Development (v1.1.0)
