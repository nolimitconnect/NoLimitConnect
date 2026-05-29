# Installing NoLimitConnect on Linux

NoLimitConnect is available via Flatpak:

1. Install Flatpak: [https://flathub.org/](https://flathub.org/)
2. Install or update the source repository if building locally.
3. Checkout source if building locally:
```bash
git clone https://github.com/nolimitconnect/NoLimitConnect.git
```

4. Build and install:
```bash
flatpak-builder --user --install --force-clean build-dir org.nolimitconnect.NoLimitConnect.yml
```

5. Run
```bash
flatpak run org.nolimitconnect.NoLimitConnect
```

6. Uninstall:
```bash
flatpak uninstall --delete-data org.nolimitconnect.NoLimitConnect
```
