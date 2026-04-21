#!/usr/bin/env bash
set -euo pipefail

input_path="${1:-}" 

if [[ -z "${input_path}" ]]; then
    deb_path=$(ls -1t package/linux/*.deb 2>/dev/null | head -n 1 || true)
elif [[ -d "${input_path}" ]]; then
    deb_path=$(ls -1t "${input_path}"/*.deb 2>/dev/null | head -n 1 || true)
else
    deb_path="${input_path}"
fi

if [[ -z "${deb_path}" ]]; then
    echo "ERROR: no .deb package found to validate" >&2
    exit 1
fi

if [[ ! -f "${deb_path}" ]]; then
    echo "ERROR: deb package not found: ${deb_path}" >&2
    exit 1
fi

work_dir=$(mktemp -d /tmp/nlc-deb-check.XXXXXX)
trap 'rm -rf "${work_dir}"' EXIT

dpkg-deb -x "${deb_path}" "${work_dir}"

binary_path="${work_dir}/usr/bin/nolimitconnect"
if [[ ! -x "${binary_path}" ]]; then
    echo "ERROR: expected executable missing from package: ${binary_path}" >&2
    exit 1
fi

missing_libs=$(ldd "${binary_path}" | grep "not found" || true)
if [[ -n "${missing_libs}" ]]; then
    echo "ERROR: unresolved runtime dependencies detected in ${deb_path}" >&2
    echo "${missing_libs}" >&2
    exit 1
fi

echo "OK: runtime dependency check passed for ${deb_path}"
