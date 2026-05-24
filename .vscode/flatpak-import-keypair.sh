#!/usr/bin/env bash
set -euo pipefail

workspace_root="${1:-$(pwd)}"
default_gpg_homedir="${HOME}/gpg/flatpak-gnupg"
legacy_gpg_homedir="${workspace_root}/build/flatpak-gnupg"
gpg_homedir="${NLC_FLATPAK_GPG_HOMEDIR:-${default_gpg_homedir}}"
if [[ -z "${NLC_FLATPAK_GPG_HOMEDIR:-}" && ! -d "${gpg_homedir}" && -d "${legacy_gpg_homedir}" ]]; then
  gpg_homedir="${legacy_gpg_homedir}"
fi
public_in="${2:-/tmp/nlc-flatpak-public.asc}"
private_in="${3:-/tmp/nlc-flatpak-private.asc}"
default_keyid_file="${HOME}/gpg/flatpak-gpg-key-id.txt"
legacy_keyid_file="${workspace_root}/build/flatpak-gpg-key-id.txt"
keyid_file="${NLC_FLATPAK_GPG_KEY_ID_FILE:-${default_keyid_file}}"
if [[ -z "${NLC_FLATPAK_GPG_KEY_ID_FILE:-}" && ! -f "${keyid_file}" && -f "${legacy_keyid_file}" ]]; then
  keyid_file="${legacy_keyid_file}"
fi

if [[ ! -f "${public_in}" ]]; then
  echo "Missing public key file: ${public_in}" >&2
  exit 1
fi

if [[ ! -f "${private_in}" ]]; then
  echo "Missing private key file: ${private_in}" >&2
  exit 1
fi

mkdir -p "${gpg_homedir}" "$(dirname "${keyid_file}")"
chmod 700 "${gpg_homedir}"

gpg --homedir "${gpg_homedir}" --import "${private_in}"
gpg --homedir "${gpg_homedir}" --import "${public_in}"

fingerprint="$(gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons 2>/dev/null | awk -F: '$1=="fpr"{print $10; exit}')"

if [[ -z "${fingerprint}" ]]; then
  echo "Import failed: no secret key present in ${gpg_homedir}" >&2
  exit 1
fi

printf '%s\n' "${fingerprint}" > "${keyid_file}"

echo "Imported key fingerprint: ${fingerprint}"
echo "Wrote key id file: ${keyid_file}"
gpg --homedir "${gpg_homedir}" --list-secret-keys --fingerprint "${fingerprint}"
