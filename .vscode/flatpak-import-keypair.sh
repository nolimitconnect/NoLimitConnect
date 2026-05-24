#!/usr/bin/env bash
set -euo pipefail

workspace_root="${1:-$(pwd)}"
gpg_homedir="${NLC_FLATPAK_GPG_HOMEDIR:-${workspace_root}/build/flatpak-gnupg}"
public_in="${2:-/tmp/nlc-flatpak-public.asc}"
private_in="${3:-/tmp/nlc-flatpak-private.asc}"
keyid_file="${workspace_root}/build/flatpak-gpg-key-id.txt"

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
