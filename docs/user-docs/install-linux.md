# Installing No Limit Connect on Linux

No Limit Connect is available via Flatpak:

1. Install Flatpak: [https://flathub.org/](https://flathub.org/)
2. Install or update the source repository if building locally.
3. Checkout source if building locally:
```bash
git clone https://github.com/nolimitconnect/NoLimitConnect.git
```

4. Allow Flatpak submodules:
```bash
git submodule add -f https://github.com/flathub/shared-modules.git
```

5. Build and install:
```bash
flatpak-builder --user --install --force-clean build-dir com.nolimitconnect.NoLimitConnect.yml
```

6. Run
```bash
flatpak run com.nolimitconnect.NoLimitConnect
```

7. Uninstall:
```bash
flatpak uninstall --delete-data com.nolimitconnect.NoLimitConnect
```
