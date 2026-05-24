#!/usr/bin/env bash
# Build and sign NoLimitConnect Flatpak repository artifacts.
#
# Usage:
#   ./.vscode/package-flatpak-signed.sh [workspace_root]
#
# Optional environment variables:
#   NLC_FLATPAK_GPG_KEY_ID      (key id/fingerprint to sign with)
#   NLC_FLATPAK_GPG_HOMEDIR     (default: <workspace>/build/flatpak-gnupg)
#   NLC_FLATPAK_PUBLIC_KEY_OUT  (default: <workspace>/docs/nlc-flatpak-public.gpg)
#   NLC_FLATPAK_REQUIRED_FPR    (fail if resolved key fingerprint does not match)

set -euo pipefail

workspace_root="${1:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
manifest="${workspace_root}/org.nolimitconnect.NoLimitConnect.yml"
repo_dir="${workspace_root}/build/flatpak-repo"
build_dir="${workspace_root}/build/flatpak-build-release-dir"
package_dir="${workspace_root}/package/flatpack"
gpg_homedir="${NLC_FLATPAK_GPG_HOMEDIR:-${workspace_root}/build/flatpak-gnupg}"
public_key_out="${NLC_FLATPAK_PUBLIC_KEY_OUT:-${workspace_root}/docs/nlc-flatpak-public.gpg}"
key_id="${NLC_FLATPAK_GPG_KEY_ID:-}"
required_fpr="${NLC_FLATPAK_REQUIRED_FPR:-}"

if ! grep -q -- "-DCMAKE_BUILD_TYPE=Release" "${manifest}"; then
    echo "Flatpak manifest must set -DCMAKE_BUILD_TYPE=Release" >&2
    exit 1
fi

mkdir -p "${repo_dir}" "${build_dir}" "${package_dir}" "$(dirname "${public_key_out}")"

if [[ ! -d "${gpg_homedir}" ]]; then
    echo "Missing GPG homedir: ${gpg_homedir}" >&2
    echo "Run ./.vscode/flatpak-gpg-init.sh first." >&2
    exit 1
fi

if [[ -z "${key_id}" && -f "${workspace_root}/build/flatpak-gpg-key-id.txt" ]]; then
    key_id="$(head -n 1 "${workspace_root}/build/flatpak-gpg-key-id.txt" | tr -d '[:space:]')"
fi

if [[ -z "${key_id}" ]]; then
    key_id="$(gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons 2>/dev/null | awk -F: '$1 == "fpr" { print $10; exit }')"
fi

if [[ -z "${key_id}" ]]; then
    echo "No Flatpak signing key found." >&2
    echo "Run ./.vscode/flatpak-gpg-init.sh first or set NLC_FLATPAK_GPG_KEY_ID." >&2
    exit 1
fi

if [[ -n "${required_fpr}" ]]; then
    normalized_required_fpr="$(printf '%s' "${required_fpr}" | tr -d '[:space:]' | tr '[:lower:]' '[:upper:]')"
    resolved_fpr="$(gpg --homedir "${gpg_homedir}" --batch --list-secret-keys --with-colons "${key_id}" 2>/dev/null | awk -F: '$1 == "fpr" { print $10; exit }')"
    resolved_fpr="$(printf '%s' "${resolved_fpr}" | tr -d '[:space:]' | tr '[:lower:]' '[:upper:]')"

    if [[ -z "${resolved_fpr}" ]]; then
        echo "Unable to resolve fingerprint for signing key: ${key_id}" >&2
        exit 1
    fi

    if [[ "${resolved_fpr}" != "${normalized_required_fpr}" ]]; then
        echo "Refusing to sign Flatpak repo with unexpected key." >&2
        echo "  Required fingerprint: ${normalized_required_fpr}" >&2
        echo "  Resolved fingerprint: ${resolved_fpr}" >&2
        echo "Set NLC_FLATPAK_GPG_KEY_ID/NLC_FLATPAK_GPG_HOMEDIR to the canonical key or update NLC_FLATPAK_REQUIRED_FPR intentionally." >&2
        exit 1
    fi
fi

flatpak-builder --force-clean --repo="${repo_dir}" "${build_dir}" "${manifest}"

# Re-sign repository metadata after each build so remote-add can verify commits.
flatpak build-update-repo --gpg-sign="${key_id}" --gpg-homedir="${gpg_homedir}" --generate-static-deltas "${repo_dir}"

gpg --homedir "${gpg_homedir}" --batch --yes --output "${public_key_out}" --export "${key_id}"

nlc_version="$(grep -oP 'set\(NLC_VERSION\s+\K[^)]+' "${workspace_root}/cmake/version.cmake")"
arch="$(uname -m | sed -E 's/(aarch64|arm64)/arm64/;s/(x86_64|amd64|x64)/x64/')"

flatpak build-bundle "${repo_dir}" "${package_dir}/nolimitconnect-${nlc_version}-${arch}.flatpak" org.nolimitconnect.NoLimitConnect

echo "Flatpak package created and repository signed"
echo "  Signing key         : ${key_id}"
echo "  Repository          : ${repo_dir}"
echo "  Public key          : ${public_key_out}"
echo "  Remote URL          : https://nolimitconnect.org/nlc-repo"
echo "  Public key URL      : https://nolimitconnect.org/nlc-flatpak-public.gpg"
