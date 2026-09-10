# NoLimitConnect (NLC)

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
Available at: [https://nolimitconnect.org](https://nolimitconnect.org)  
*(Windows NSIS, Android APK, Linux .deb, and Flatpak for x64/ARM64/Pi)*

**Source Code:**
```bash
git clone https://github.com/nolimitconnect/NoLimitConnect.git
```

### 🤝 Contributing
Contributions are welcome and currently follow a simple fork + pull request workflow.

- Contribution guide: [CONTRIBUTING.md](CONTRIBUTING.md)
- Issue tracker: <https://github.com/nolimitconnect/NoLimitConnect/issues>

---

### ⚖️ License & Legal
*   **Code by Brett R. Jones:** Dual-licensed (similar to Ruby’s license); see individual source files.
*   **Third-Party:** Various open-source licenses apply (see `LICENSE` and `LEGAL` files).
*   **Trademarks:** The **NoLimitConnect** name and logo are trademarks of Brett R. Jones and are **not** covered by the GPL — see [`TRADEMARK.md`](TRADEMARK.md). You may redistribute unmodified builds, and package the project for app stores and distribution repositories, under the mark; branding a functionally modified fork with it requires permission.
*   **Principles:** No ads. No data mining. No analytics. No tracking.

**Project Status:** Active Development (v1.1.5)
