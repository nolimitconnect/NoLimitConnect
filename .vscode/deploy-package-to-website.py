#!/usr/bin/env python3
"""Deploy package artifacts to GitHub Releases.

This script is the Linux-friendly equivalent of deploy-package-to-website.ps1.
"""

from __future__ import annotations

import argparse
import datetime as dt
import fnmatch
import hashlib
import json
import platform
import os
from pathlib import Path
import re
import shutil
import sys
import tempfile
import urllib.error
import urllib.parse
import urllib.request


FLATPAK_REMOTE_NAME = "nlc"
FLATPAK_REMOTE_URL = "https://nolimitconnect.org/nlc-repo"
FLATPAK_PUBLIC_KEY_URL = "https://nolimitconnect.org/nlc-flatpak-public.gpg"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Deploy package to GitHub Releases")
    parser.add_argument(
        "-PackageType",
        "--package-type",
        dest="package_type",
        choices=["windows", "linux", "android-signed", "flatpak"],
        required=True,
    )
    parser.add_argument(
        "-WorkspaceRoot",
        "--workspace-root",
        dest="workspace_root",
        default="",
    )
    parser.add_argument(
        "-WebsiteRoot",
        "--website-root",
        dest="website_root",
        default=os.environ.get("NLC_WEBSITE_ROOT", ""),
    )
    parser.add_argument(
        "-GitHubRepository",
        "--github-repository",
        dest="github_repository",
        default="nolimitconnect/NoLimitConnect",
    )
    parser.add_argument(
        "-GitHubApiBaseUrl",
        "--github-api-base-url",
        dest="github_api_base_url",
        default="https://api.github.com",
    )
    parser.add_argument(
        "-GitHubReleaseTag",
        "--github-release-tag",
        dest="github_release_tag",
        default="",
    )
    parser.add_argument(
        "-SkipWebsiteUpdate",
        "--skip-website-update",
        dest="skip_website_update",
        action="store_true",
    )
    parser.add_argument(
        "-KeepLatestVersions",
        "--keep-latest-versions",
        dest="keep_latest_versions",
        type=int,
        default=0,
    )
    parser.add_argument(
        "-FlatpakArch",
        "--flatpak-arch",
        dest="flatpak_arch",
        choices=["auto", "x64", "arm64"],
        default="auto",
        help="Architecture label for flatpak artifacts. Defaults to auto-detect.",
    )
    return parser.parse_args()


def get_workspace_root(cli_value: str) -> Path:
    if cli_value.strip():
        return Path(cli_value).resolve()
    return Path(__file__).resolve().parent.parent


def get_package_config(package_type: str) -> dict[str, object]:
    configs: dict[str, dict[str, object]] = {
        "windows": {
            "source_dir": "package/windows",
            "include": ["*.exe"],
            "exclude": [],
            "display_name": "Windows",
            "notes": "NSIS installer for Windows x64.",
            "section_key": "windows",
        },
        "linux": {
            "source_dir": "package/linux",
            "include": ["*.deb"],
            "exclude": [],
            "display_name": "Linux",
            "notes": "Debian package for Linux x64.",
            "section_key": "linux",
        },
        "android-signed": {
            "source_dir": "package/android",
            "include": ["*-signed.apk"],
            "exclude": [],
            "display_name": "Android",
            "notes": "Signed APK intended for release distribution.",
            "section_key": "android-signed",
        },
        "flatpak": {
            "source_dir": "package/flatpack",
            "include": ["*.flatpak"],
            "exclude": [],
            "display_name": "Flatpak",
            "notes": "GPG-signed Flatpak repository and bundle for Linux desktops.",
            "section_key": "flatpak",
        },
    }
    return configs[package_type]


def resolve_flatpak_arch(artifact_name: str, override: str) -> str:
    if override in {"x64", "arm64"}:
        return override

    lower_name = artifact_name.lower()
    if any(token in lower_name for token in ("arm64", "aarch64")):
        return "arm64"
    if any(token in lower_name for token in ("x64", "amd64", "x86_64")):
        return "x64"

    machine = platform.machine().lower()
    if machine in {"aarch64", "arm64"}:
        return "arm64"
    return "x64"


def ensure_arch_named_flatpak(artifact: Path, flatpak_arch: str, temp_dir: Path) -> Path:
    lower_name = artifact.name.lower()
    if any(token in lower_name for token in ("arm64", "aarch64", "x64", "amd64", "x86_64")):
        return artifact

    renamed = temp_dir / f"{artifact.stem}-{flatpak_arch}{artifact.suffix}"
    shutil.copy2(artifact, renamed)
    return renamed


def with_flatpak_arch_config(config: dict[str, object], flatpak_arch: str) -> dict[str, object]:
    updated = dict(config)
    if flatpak_arch == "arm64":
        updated["display_name"] = "Flatpak (ARM64)"
        updated["notes"] = "Flatpak bundle for Linux ARM64 desktops with Flatpak support."
        updated["section_key"] = "flatpak-arm64"
    else:
        updated["display_name"] = "Flatpak (x64)"
        updated["notes"] = "Flatpak bundle for Linux x64 desktops with Flatpak support."
        updated["section_key"] = "flatpak"
    return updated


def get_project_version(root: Path) -> str:
    version_file = root / "cmake" / "version.cmake"
    if not version_file.exists():
        raise RuntimeError(f"Version file not found: {version_file}")
    content = version_file.read_text(encoding="utf-8")
    match = re.search(r"set\(NLC_VERSION\s+([^\)]+)\)", content)
    if not match:
        raise RuntimeError(f"Could not parse NLC_VERSION from {version_file}")
    return match.group(1).strip()


def get_latest_artifact(directory: Path, include: list[str], exclude: list[str]) -> Path:
    if not directory.exists():
        raise RuntimeError(f"Package directory not found: {directory}")
    files: list[Path] = []
    for child in directory.iterdir():
        if not child.is_file():
            continue
        name = child.name
        include_match = any(fnmatch.fnmatch(name, pattern) for pattern in include)
        if not include_match:
            continue
        if any(fnmatch.fnmatch(name, pattern) for pattern in exclude):
            continue
        files.append(child)
    if not files:
        raise RuntimeError(
            f"No package artifact found in {directory} matching: {', '.join(include)}"
        )
    files.sort(key=lambda p: p.stat().st_mtime, reverse=True)
    return files[0]


def write_sha256_sidecar(artifact_path: Path, output_dir: Path) -> Path:
    digest = hashlib.sha256(artifact_path.read_bytes()).hexdigest()
    out = output_dir / f"{artifact_path.name}.sha256"
    out.write_text(f"{digest}  {artifact_path.name}\n", encoding="ascii")
    return out


def github_request(
    method: str,
    url: str,
    token: str,
    data: bytes | None = None,
    content_type: str | None = None,
) -> bytes:
    headers = {
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
        "User-Agent": "NoLimitConnect-Deploy-Script",
    }
    if content_type:
        headers["Content-Type"] = content_type
    request = urllib.request.Request(url=url, data=data, method=method, headers=headers)
    try:
        with urllib.request.urlopen(request) as response:
            return response.read()
    except urllib.error.HTTPError as exc:
        detail = ""
        try:
            detail = exc.read().decode("utf-8", errors="replace")
        except Exception:
            detail = ""
        raise RuntimeError(
            f"GitHub API error {exc.code} on {method} {url}."
            f" {detail}".strip()
        ) from exc


def get_release_by_tag(base_url: str, repository: str, tag: str, token: str) -> dict[str, object] | None:
    encoded_tag = urllib.parse.quote(tag, safe="")
    url = f"{base_url}/repos/{repository}/releases/tags/{encoded_tag}"
    try:
        payload = github_request("GET", url, token)
        return json.loads(payload.decode("utf-8"))
    except RuntimeError as exc:
        if "error 404" in str(exc).lower():
            return None
        raise


def create_release(base_url: str, repository: str, tag: str, token: str) -> dict[str, object]:
    url = f"{base_url}/repos/{repository}/releases"
    body = {
        "tag_name": tag,
        "name": tag,
        "draft": False,
        "prerelease": False,
        "generate_release_notes": True,
    }
    payload = github_request(
        "POST",
        url,
        token,
        data=json.dumps(body).encode("utf-8"),
        content_type="application/json",
    )
    return json.loads(payload.decode("utf-8"))


def get_or_create_release(base_url: str, repository: str, tag: str, token: str) -> dict[str, object]:
    release = get_release_by_tag(base_url, repository, tag, token)
    if release:
        return release
    print(f"  Release '{tag}' was not found. Creating it now.")
    return create_release(base_url, repository, tag, token)


def remove_existing_asset(
    base_url: str,
    repository: str,
    release: dict[str, object],
    asset_name: str,
    token: str,
) -> None:
    assets = release.get("assets") or []
    for asset in assets:
        if str(asset.get("name", "")) != asset_name:
            continue
        asset_id = asset.get("id")
        if not asset_id:
            continue
        print(f"  Removing existing asset: {asset_name}")
        delete_url = f"{base_url}/repos/{repository}/releases/assets/{asset_id}"
        github_request("DELETE", delete_url, token)
        return


def refresh_release(base_url: str, repository: str, release_id: int, token: str) -> dict[str, object]:
    url = f"{base_url}/repos/{repository}/releases/{release_id}"
    payload = github_request("GET", url, token)
    return json.loads(payload.decode("utf-8"))


def upload_release_asset(
    base_url: str,
    repository: str,
    release: dict[str, object],
    file_path: Path,
    token: str,
) -> str:
    remove_existing_asset(base_url, repository, release, file_path.name, token)

    upload_url_template = str(release.get("upload_url", ""))
    upload_base = upload_url_template.replace("{?name,label}", "")
    upload_url = f"{upload_base}?name={urllib.parse.quote(file_path.name, safe='')}"

    print(f"  Uploading: {file_path.name}")
    payload = github_request(
        "POST",
        upload_url,
        token,
        data=file_path.read_bytes(),
        content_type="application/octet-stream",
    )
    asset = json.loads(payload.decode("utf-8"))
    return str(asset.get("browser_download_url", ""))


def update_download_page_section(
    website_repo_root: Path,
    section_key: str,
    display_name: str,
    artifact_name: str,
    artifact_url: str,
    hash_name: str,
    hash_url: str,
    notes: str,
    timestamp: str,
) -> None:
    download_page = website_repo_root / "docs" / "download.md"
    if not download_page.exists():
        raise RuntimeError(f"Website download page not found: {download_page}")

    begin_marker = f"<!-- BEGIN SECTION: {section_key} -->"
    end_marker = f"<!-- END SECTION: {section_key} -->"
    content = download_page.read_text(encoding="utf-8")
    if begin_marker not in content or end_marker not in content:
        raise RuntimeError(
            f"Per-section markers for '{section_key}' are missing in {download_page}."
        )

    if section_key == "flatpak":
        new_section = "\n".join(
            [
                begin_marker,
                "## Flatpak (Universal - All Architectures)",
                "",
                "**Recommended for Linux users.** One-click installation via custom remote.",
                "",
                '<div id="flatpak-signature-warning" class="flatpak-signature-warning" hidden>',
                "  <strong>Flatpak remote signature status warning:</strong>",
                "  Signed repository metadata is temporarily unavailable on this mirror.",
                "  Remote install with mandatory summary verification may fail until signatures are republished.",
                "</div>",
                "",
                "### Install via Remote (Recommended)",
                "",
                "```bash",
                f"curl -fsSL {FLATPAK_PUBLIC_KEY_URL} -o /tmp/nlc-flatpak-public.gpg && \\",
                f"flatpak remote-add --if-not-exists --gpg-import=/tmp/nlc-flatpak-public.gpg --gpg-verify-summary=false {FLATPAK_REMOTE_NAME} {FLATPAK_REMOTE_URL} && \\",
                f"flatpak remote-modify --gpg-verify-summary=false {FLATPAK_REMOTE_NAME} && \\",
                "flatpak install -y nlc org.nolimitconnect.NoLimitConnect",
                "```",
                "",
                f"Public key URL: <{FLATPAK_PUBLIC_KEY_URL}>",
                "",
                "### Direct Download",
                f"- Latest package: [{artifact_name}]({artifact_url})",
                f"- SHA-256: [{hash_name}]({hash_url})",
                f"- Last updated: {timestamp}",
                f"- Notes: {notes}",
                end_marker,
            ]
        )
    else:
        new_section = "\n".join(
            [
                begin_marker,
                f"## {display_name}",
                f"- Latest package: [{artifact_name}]({artifact_url})",
                f"- SHA-256: [{hash_name}]({hash_url})",
                f"- Last updated: {timestamp}",
                f"- Notes: {notes}",
                end_marker,
            ]
        )
    pattern = re.compile(
        re.escape(begin_marker) + r".*?" + re.escape(end_marker),
        flags=re.DOTALL,
    )
    updated = pattern.sub(new_section, content, count=1)
    download_page.write_text(updated, encoding="utf-8")


def main() -> int:
    args = parse_args()
    if args.keep_latest_versions < 0 or args.keep_latest_versions > 100:
        raise RuntimeError("KeepLatestVersions must be between 0 and 100")

    github_token = os.environ.get("GITHUB_RELEASES_TOKEN", "").strip()
    if not github_token:
        github_token = os.environ.get("GITHUB_TOKEN", "").strip()
    if not github_token:
        raise RuntimeError(
            "GITHUB_RELEASES_TOKEN (or GITHUB_TOKEN) environment variable is not set."
        )

    workspace_root = get_workspace_root(args.workspace_root)
    website_root = Path(args.website_root).resolve() if args.website_root.strip() else workspace_root
    config = get_package_config(args.package_type)

    can_update_website = False
    if not args.skip_website_update:
        if website_root.exists():
            can_update_website = True
        else:
            print(
                f"Warning: website repository not found at '{website_root}'. "
                "Continuing without website download page updates.",
                file=sys.stderr,
            )

    version = get_project_version(workspace_root)
    artifact_dir = workspace_root / str(config["source_dir"])
    artifact = get_latest_artifact(
        directory=artifact_dir,
        include=list(config["include"]),
        exclude=list(config["exclude"]),
    )

    flatpak_arch = ""
    if args.package_type == "flatpak":
        flatpak_arch = resolve_flatpak_arch(artifact.name, args.flatpak_arch)
        config = with_flatpak_arch_config(config, flatpak_arch)

    release_tag = args.github_release_tag.strip() or f"v{version}"
    release = get_or_create_release(
        base_url=args.github_api_base_url,
        repository=args.github_repository,
        tag=release_tag,
        token=github_token,
    )

    temp_dir = Path(tempfile.mkdtemp(prefix="nlc-deploy-"))
    try:
        upload_artifact = artifact
        if args.package_type == "flatpak":
            upload_artifact = ensure_arch_named_flatpak(artifact, flatpak_arch, temp_dir)

        sha256_file_path = write_sha256_sidecar(upload_artifact, temp_dir)
        sha256_file_name = sha256_file_path.name

        artifact_url = upload_release_asset(
            base_url=args.github_api_base_url,
            repository=args.github_repository,
            release=release,
            file_path=upload_artifact,
            token=github_token,
        )

        release_id = int(release.get("id", 0))
        release = refresh_release(
            base_url=args.github_api_base_url,
            repository=args.github_repository,
            release_id=release_id,
            token=github_token,
        )

        hash_url = upload_release_asset(
            base_url=args.github_api_base_url,
            repository=args.github_repository,
            release=release,
            file_path=sha256_file_path,
            token=github_token,
        )

        if args.keep_latest_versions > 0:
            print(
                "Warning: KeepLatestVersions currently does not prune GitHub releases. "
                "No release cleanup was performed.",
                file=sys.stderr,
            )

        timestamp = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")
        if can_update_website:
            update_download_page_section(
                website_repo_root=website_root,
                section_key=str(config["section_key"]),
                display_name=str(config["display_name"]),
                artifact_name=upload_artifact.name,
                artifact_url=artifact_url,
                hash_name=sha256_file_name,
                hash_url=hash_url,
                notes=str(config["notes"]),
                timestamp=timestamp,
            )

        print("")
        print(f"Deployed package type : {args.package_type}")
        if args.package_type == "flatpak":
            print(f"  Flatpak arch         : {flatpak_arch}")
        print(f"  Version             : {version}")
        print(f"  Release tag         : {release_tag}")
        print(f"  Source artifact     : {artifact}")
        if upload_artifact != artifact:
            print(f"  Uploaded as         : {upload_artifact.name}")
        print(f"  GitHub repository   : {args.github_repository}")
        print(f"  Artifact URL        : {artifact_url}")
        print(f"  SHA-256 URL         : {hash_url}")
        if can_update_website:
            print(f"  Download page       : {website_root / 'docs' / 'download.md'}")
        print("")
        if can_update_website:
            print("Next: review and commit docs/download.md updates.")
        else:
            print("Next: verify assets on the GitHub release page.")
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pylint: disable=broad-except
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
