# FAQ

---

## Hardware Compatibility

### Supported Hardware

NLC is designed to run on a wide range of hardware, with the following requirements:

- **Operating System:** 64-bit  
- **CPU Architecture:** 64-bit  
  - *Note:* NLC can be compiled for 32-bit systems, but **32-bit is not officially supported**.
- **Not Supported - Apple devices:**  
    - This is due to high cost and Apple's restrictive platform policies.

---

## Developer Test Bed

The following devices make up the project maintainer test environment used for regular development and validation:

![Test Bed](assets/faq/test-bed.png)

**Devices:**

1. **GMKtek K11** — Windows 11  
2. **Galaxy Tab A8** — Android 14  
3. **Raspberry Pi 5** — Raspberry Pi OS  
4. **GMKtek G9** — Ubuntu 24.04  
5. **Galaxy Flip G5** — Android 15  
6. **Steetek KVM Switch** — Shares keyboard/mouse/monitor between: Raspberry Pi 5 and GMKtek G9

---

## Recommended Hardware for a NLC Network

To run NLC as a private network host, the following options are recommended:

1. **An old 64-bit Android phone**  
   -- No cellular service required  
   -- Low-power and suitable for always-on use

2. **Raspberry Pi 5**  
   -- Cost-effective  
   -- Low energy usage  
   -- Ideal for 24/7 operation

3. **Any machine capable of running Linux**  
   -- Desktops, laptops, mini PCs, etc.

---

## Do I Need Dedicated Hardware to Host a NLC Network?

No. Hosting a private NLC network does **not** require specialized or dedicated hardware.  
There are only two essential requirements for any type of NLC hosting:

1. **An active NoLimitConnect application instance**  
2. **An open network port**  
   — Refer to the *Connectivity* section for detailed guidance.

With these basics in place, you can host a private network on virtually any compatible device.

---

## How do I setup my own NLC Network

➡️ [How To Setup A NLC Network Host](howto-docs/howto-nlc-network-overview.md)

---

## Why We Encourage Network Hosting

NLC is designed with a strong focus on privacy and freedom of expression. While the platform operates legally and transparently, history has shown that services promoting open communication can still face external pressures or attempts at deplatforming.

By enabling users to host their own networks, we eliminate reliance on any single organization or centralized service. This decentralized model ensures that:

- **No single point of failure exists** — individual networks remain operational even if others are targeted.
- **Users maintain control** over their data, uptime, and community policies.
- **Censorship resistance is strengthened**, as there is no central entity that can be taken down to silence the platform.

Encouraging distributed hosting empowers the community and protects the long-term resilience of NLC.

---

## Encryption, Public and Privete Network Keys

➡️ [Encryption Overview](encryption-overview.md)

---

## Permission Levels

Permission to each plugin depends on permission level assigned to the plugin and permission level assigned to user

➡️ [Permission Levels](howto-docs/howto-permission-levels.md)

---

## Why Use NoLimitConnect Instead of Other Services?

NoLimitConnect is built for people who want to take control back. Here's who it serves:

### Privacy and Data Control

**You don't want corporations storing your data**

- NoLimitConnect runs on your hardware or devices you control—not on someone else's servers
- Your files, messages, and communications stay in your network. Period.
- No data collection, no algorithms profiling you, no "cloud" accounts required

**You're tired of being the product**

- No ads, no tracking, no selling your information to third parties
- You own your data—full stop

### Practical Communication and File Sharing

**You want to transfer files to friends without corporate middlemen**

- Send large files directly peer-to-peer without signing up for third-party services like Google Drive, Dropbox, or WeTransfer
- No file size limits imposed by some company's terms of service

**You have family or friends who need to stay in touch but can't afford data plans**

- Set up NoLimitConnect on a low-power device (old phone, Raspberry Pi, etc.) and let family connect over WiFi or your local network
- Free communication for anyone on your network

**You want a private video/security camera system without corporate access**

- Host your own security system locally without signing up with a cloud service
- Your recordings stay on your hardware; no company has access to footage of your home or business

### Freedom and Control

**Your country restricts access to social media or communication platforms**

- NoLimitConnect operates on your own infrastructure, independent of what's blocked by your government or ISP
- You can communicate privately with your community without relying on globally-blocked services

**You want to build a private communication hub for your community or organization**

- Create a local network for your group—church, club, neighborhood, workplace, etc.
- Complete control over who joins, what's shared, and how it operates
- No corporate policies or algorithm deciding what your community can say or see

**You're tired of account suspensions and arbitrary content moderation**

- No corporation controls your account or decides what you can post
- You set the rules for your network

### The Bottom Line

NoLimitConnect is for people who want their tools to work *for them*, not *against them*. It's not about being anti-social—it's about being anti-corporate-control and pro-privacy. You communicate on your terms, not theirs.

---