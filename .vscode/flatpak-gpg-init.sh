#!/usr/bin/env bash
# Initialize a dedicated GPG keyring for Flatpak repo signing and export the public key.
#
# Usage:
#   ./.vscode/flatpak-gpg-init.sh [workspace_root]
#
# Optional environment variables:
#   NLC_FLATPAK_GPG_NAME        (default: NoLimitConnect Flatpak Repo)
#   NLC_FLATPAK_GPG_EMAIL       (default: release@nolimitconnect.org)
#   NLC_FLATPAK_GPG_COMMENT     (default: Flatpak repo signing key)
#   NLC_FLATPAK_GPG_EXPIRE      (default: 2y)
#   NLC_FLATPAK_GPG_KEY_ID      (reuse existing key id/fingerprint)
#   NLC_FLATPAK_GPG_HOMEDIR     (default: <workspace>/build/flatpak-gnupg)
#   NLC_FLATPAK_PUBLIC_KEY_OUT  (default: <workspace>/docs/nlc-flatpak-public.gpg)

set -euo pipefail

workspace_root="${1:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
gpg_homedir="${NLC_FLATPAK_GPG_HOMEDIR:-${workspace_root}/build/flatpak-gnupg}"
public_key_out="${NLC_FLATPAK_PUBLIC_KEY_OUT:-${workspace_root}/docs/nlc-flatpak-public.gpg}"
key_hint="${NLC_FLATPAK_GPG_KEY_ID:-}"
key_name="${NLC_FLATPAK_GPG_NAME:-NoLimitConnect Flatpak Repo}"
key_email="${NLC_FLATPAK_GPG_EMAIL:-release@nolimitconnect.org}"
key_comment="${NLC_FLATPAK_GPG_COMMENT:-Flatpak repo signing key}"
key_expire="${NLC_FLATPAK_GPG_EXPIRE:-2y}"

mkdir -p "${gpg_homedir}" "$(dirname "${public_key_out}")" "${workspace_root}/build"
chmod 700 "${gpg_homedir}"

key_id=""
if [[ -n "${key_hint}" ]]; then
    if gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons "${key_hint}" >/dev/null 2>&1; then
        key_id="${key_hint}"
    fi
fi

if [[ -z "${key_id}" ]]; then
    existing_fpr="$(gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons 2>/dev/null | awk -F: '$1 == "fpr" { print $10; exit }')"
    if [[ -n "${existing_fpr}" ]]; then
        key_id="${existing_fpr}"
    fi
fi

if [[ -z "${key_id}" ]]; then
    echo "Generating new Flatpak signing key in ${gpg_homedir}"

    batch_file="$(mktemp)"
    cat > "${batch_file}" <<EOF
%no-protection
Key-Type: RSA
Key-Length: 4096
Subkey-Type: RSA
Subkey-Length: 4096
Name-Real: ${key_name}
Name-Comment: ${key_comment}
Name-Email: ${key_email}
Expire-Date: ${key_expire}
%commit
EOF

    gpg --homedir "${gpg_homedir}" --batch --pinentry-mode loopback --generate-key "${batch_file}"
    rm -f "${batch_file}"

    key_id="$(gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons | awk -F: '$1 == "fpr" { print $10; exit }')"
fi

if [[ -z "${key_id}" ]]; then
    echo "Failed to resolve a Flatpak signing key id." >&2
    exit 1
fi

gpg --homedir "${gpg_homedir}" --batch --yes --output "${public_key_out}" --export "${key_id}"
printf '%s\n' "${key_id}" > "${workspace_root}/build/flatpak-gpg-key-id.txt"

echo "Flatpak GPG key ready"
echo "  Key ID/Fingerprint : ${key_id}"
echo "  GPG homedir        : ${gpg_homedir}"
echo "  Public key         : ${public_key_out}"
echo "  Key id file        : ${workspace_root}/build/flatpak-gpg-key-id.txt"
