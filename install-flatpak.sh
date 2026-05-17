#!/bin/bash
# One-click Flatpak remote setup and install for NoLimitConnect
# Usage: curl -fsSL https://nolimitconnect.org/install-flatpak.sh | bash

set -e

echo "=== NoLimitConnect Flatpak Installer ==="
echo "This script will add the custom Flatpak remote and install NoLimitConnect."
echo

if ! command -v flatpak &> /dev/null; then
  echo "Error: flatpak is not installed."
  echo "Please install Flatpak from your distribution first:"
  echo "  Ubuntu/Debian: sudo apt install flatpak"
  echo "  Fedora: sudo dnf install flatpak"
  echo "  Arch: sudo pacman -S flatpak"
  exit 1
fi

REMOTE_NAME="nlc"
REMOTE_URL="https://nolimitconnect.org/nlc-repo"
REMOTE_KEY_URL="https://nolimitconnect.org/nlc-flatpak-public.gpg"
REMOTE_SUMMARY_SIG_URL="https://nolimitconnect.org/nlc-repo/summary.sig"
REMOTE_SUMMARY_IDX_SIG_URL="https://nolimitconnect.org/nlc-repo/summary.idx.sig"
APP_ID="org.nolimitconnect.NoLimitConnect"

if ! command -v curl &> /dev/null; then
  echo "Error: curl is not installed."
  echo "Please install curl and re-run this script."
  exit 1
fi

if ! curl -fsSI "$REMOTE_SUMMARY_SIG_URL" >/dev/null || ! curl -fsSI "$REMOTE_SUMMARY_IDX_SIG_URL" >/dev/null; then
  arch="$(uname -m | sed -E 's/(aarch64|arm64)/arm64/;s/(x86_64|amd64|x64)/x64/')"
  bundle_url="https://github.com/nolimitconnect/NoLimitConnect/releases/latest/download/nolimitconnect-${arch}.flatpak"
  bundle_file="$(mktemp --suffix=.flatpak)"
  trap 'rm -f "$bundle_file"' EXIT

  echo "Warning: Flatpak summary signature files are not available on the remote."
  echo "         Falling back to direct bundle install for this architecture."
  echo "Downloading Flatpak bundle from: $bundle_url"
  curl -fL "$bundle_url" -o "$bundle_file"

  echo "Installing $APP_ID from downloaded bundle in user scope..."
  flatpak install -y --noninteractive --user "$bundle_file"

  echo
  echo "=== Installation Complete ==="
  echo "Launch NoLimitConnect with:"
  echo "  flatpak run $APP_ID"
  echo
  echo "Or search for 'NoLimitConnect' in your application menu."
  exit 0
fi

key_file="$(mktemp)"
trap 'rm -f "$key_file"' EXIT

echo "Downloading Flatpak public key from: $REMOTE_KEY_URL"
curl -fsSL "$REMOTE_KEY_URL" -o "$key_file"

if flatpak remotes --columns=name | grep -Fxq "$REMOTE_NAME"; then
  echo "Updating existing Flatpak remote key: $REMOTE_NAME"
  flatpak remote-modify --gpg-import="$key_file" "$REMOTE_NAME"
  flatpak remote-modify --gpg-verify "$REMOTE_NAME"
else
  echo "Adding Flatpak remote: $REMOTE_NAME ($REMOTE_URL)"
  flatpak remote-add --if-not-exists --gpg-import="$key_file" "$REMOTE_NAME" "$REMOTE_URL"
fi

echo "Installing $APP_ID from $REMOTE_NAME remote..."
flatpak install -y --noninteractive "$REMOTE_NAME" "$APP_ID"

echo
echo "=== Installation Complete ==="
echo "Launch NoLimitConnect with:"
echo "  flatpak run $APP_ID"
echo
echo "Or search for 'NoLimitConnect' in your application menu."
