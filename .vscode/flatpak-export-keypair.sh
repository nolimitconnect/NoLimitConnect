#!/usr/bin/env bash
set -euo pipefail

workspace_root="${1:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
gpg_homedir="${NLC_FLATPAK_GPG_HOMEDIR:-${workspace_root}/build/flatpak-gnupg}"
keyid_file="${workspace_root}/build/flatpak-gpg-key-id.txt"
public_out="${2:-/tmp/nlc-flatpak-public.asc}"
private_out="${3:-/tmp/nlc-flatpak-private.asc}"

if [[ ! -d "${gpg_homedir}" ]]; then
  echo "Missing GPG homedir: ${gpg_homedir}" >&2
  exit 1
fi

key_id=""
if [[ -f "${keyid_file}" ]]; then
  key_id="$(tr -d '[:space:]' < "${keyid_file}")"
fi

if [[ -z "${key_id}" ]]; then
  key_id="$(gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons 2>/dev/null | awk -F: '$1=="fpr"{print $10; exit}')"
fi

if [[ -z "${key_id}" ]]; then
  echo "No secret key found in ${gpg_homedir}" >&2
  exit 1
fi

echo "Using key fingerprint: ${key_id}"
gpg --homedir "${gpg_homedir}" --list-secret-keys --fingerprint "${key_id}"

gpg --homedir "${gpg_homedir}" --armor --export "${key_id}" > "${public_out}"
gpg --homedir "${gpg_homedir}" --armor --export-secret-keys "${key_id}" > "${private_out}"

wc -c "${public_out}" "${private_out}"
sha256sum "${public_out}" "${private_out}"

echo
printf 'Public key  : %s\n' "${public_out}"
printf 'Private key : %s\n' "${private_out}"
