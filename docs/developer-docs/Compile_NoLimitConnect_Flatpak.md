# Compile NoLimitConnect Using Flatpak

1. Checkout NoLimitConnect:
```bash
git clone https://github.com/nolimitconnect/NoLimitConnect.git
cd nolimitconnect
```

2. Build and install:
```bash
flatpak-builder --user --install --force-clean build-dir org.nolimitconnect.NoLimitConnect.yml
```

3. Run:
```bash
flatpak run org.nolimitconnect.NoLimitConnect
```

4. List flatpak apps:
```bash
flatpak list --app
```

5. Show NLC info:
```bash
flatpak info --show-location org.nolimitconnect.NoLimitConnect
```

6. Uninstall:
```bash
flatpak uninstall --delete-data org.nolimitconnect.NoLimitConnect
```

7. Change FlatPak Version:

If you have selected the wrong Patform and SDK version you 
can install the correct version with

```bash
flatpak install flathub org.kde.Sdk//6.9
```
and

```bash
flatpak install flathub org.kde.Sdk//6.9
```