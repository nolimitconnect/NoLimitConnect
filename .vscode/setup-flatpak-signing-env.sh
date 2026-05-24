#!/usr/bin/env bash
# Configure shell startup to enforce Flatpak signing fingerprint for NoLimitConnect.
#
# Usage:
#   ./.vscode/setup-flatpak-signing-env.sh [workspace_root] [shell_rc]
#
# Defaults:
#   workspace_root = current working directory
#   shell_rc       = ~/.bashrc

set -euo pipefail

workspace_root="${1:-$(pwd)}"
shell_rc="${2:-${HOME}/.bashrc}"
default_keyid_file="${HOME}/gpg/flatpak-gpg-key-id.txt"
legacy_keyid_file="${workspace_root}/build/flatpak-gpg-key-id.txt"
keyid_file="${NLC_FLATPAK_GPG_KEY_ID_FILE:-${default_keyid_file}}"
if [[ -z "${NLC_FLATPAK_GPG_KEY_ID_FILE:-}" && ! -f "${keyid_file}" && -f "${legacy_keyid_file}" ]]; then
  keyid_file="${legacy_keyid_file}"
fi
default_gpg_homedir="${HOME}/gpg/flatpak-gnupg"
legacy_gpg_homedir="${workspace_root}/build/flatpak-gnupg"
gpg_homedir="${NLC_FLATPAK_GPG_HOMEDIR:-${default_gpg_homedir}}"
if [[ -z "${NLC_FLATPAK_GPG_HOMEDIR:-}" && ! -d "${gpg_homedir}" && -d "${legacy_gpg_homedir}" ]]; then
  gpg_homedir="${legacy_gpg_homedir}"
fi
managed_begin="# >>> NLC Flatpak signing guard >>>"
managed_end="# <<< NLC Flatpak signing guard <<<"

if [[ ! -d "${workspace_root}" ]]; then
  echo "Workspace root does not exist: ${workspace_root}" >&2
  exit 1
fi

if [[ ! -f "${keyid_file}" ]]; then
  echo "Missing key id file: ${keyid_file}" >&2
  echo "Run ./.vscode/flatpak-gpg-init.sh first (or import keypair) so a key-id file exists." >&2
  exit 1
fi

mkdir -p "$(dirname "${shell_rc}")"
touch "${shell_rc}"

# Remove old managed block if present.
if grep -qF "${managed_begin}" "${shell_rc}"; then
  tmp_file="$(mktemp)"
  awk -v begin="${managed_begin}" -v end="${managed_end}" '
    $0 == begin { in_block = 1; next }
    $0 == end { in_block = 0; next }
    !in_block { print }
  ' "${shell_rc}" > "${tmp_file}"
  mv "${tmp_file}" "${shell_rc}"
fi

cat >> "${shell_rc}" <<EOF

${managed_begin}
if [[ -d "${workspace_root}" && -f "${keyid_file}" ]]; then
  export NLC_FLATPAK_REQUIRED_FPR="\$(tr -d '[:space:]' < "${keyid_file}")"
  export NLC_FLATPAK_GPG_HOMEDIR="${gpg_homedir}"
  export NLC_FLATPAK_GPG_KEY_ID_FILE="${keyid_file}"
fi
${managed_end}
EOF

echo "Updated shell config: ${shell_rc}"
echo "Workspace root      : ${workspace_root}"
echo "Required fingerprint: $(tr -d '[:space:]' < "${keyid_file}")"
echo
echo "Activate now with:"
echo "  source ${shell_rc}"
