#!/usr/bin/env python3
"""Deploy package artifacts to GitLab Generic Package Registry.

This script is a Linux-friendly equivalent of deploy-package-to-website.ps1.
"""

from __future__ import annotations

import argparse
import datetime as dt
import fnmatch
import hashlib
import json
import os
import platform
from pathlib import Path
import re
import shutil
import sys
import tempfile
import urllib.error
import urllib.parse
import urllib.request


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Deploy package to GitLab registry")
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
        default=os.environ.get("NLC_WEBSITE_ROOT", r"F:\nlc-nolimitconnect.com"),
    )
    parser.add_argument(
        "-GitLabProjectPath",
        "--gitlab-project-path",
        dest="gitlab_project_path",
        default="nolimitcode/nolimitconnect",
    )
    parser.add_argument(
        "-GitLabBaseUrl",
        "--gitlab-base-url",
        dest="gitlab_base_url",
        default="https://gitlab.com",
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
        },
        "linux": {
            "source_dir": "package/linux",
            "include": ["*.deb"],
            "exclude": [],
            "display_name": "Linux",
            "notes": "Debian package for Linux x64.",
        },
        "android-signed": {
            "source_dir": "package/android",
            "include": ["*-signed.apk"],
            "exclude": [],
            "display_name": "Android",
            "notes": "Signed APK intended for release distribution.",
        },
        "flatpak": {
            "source_dir": "package/flatpack",
            "include": ["*.flatpak"],
            "exclude": [],
            "display_name": "Flatpak",
            "notes": "Flatpak bundle for Linux desktops with Flatpak support.",
        },
    }
    return configs[package_type]


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


def normalize_arch_label(raw_arch: str) -> str:
    arch = raw_arch.strip().lower()
    if arch in {"x86_64", "amd64", "x64"}:
        return "x64"
    if arch in {"aarch64", "arm64"}:
        return "arm64"
    if arch in {"armv7l", "armv7", "armhf"}:
        return "armv7"
    return re.sub(r"[^a-z0-9]+", "-", arch).strip("-") or "unknown"


def detect_deploy_architecture(package_type: str, artifact_name: str) -> str:
    name = artifact_name.lower()
    for candidate in ("arm64", "aarch64", "x86_64", "amd64", "x64", "armv7", "armhf"):
        if candidate in name:
            return normalize_arch_label(candidate)

    if package_type == "flatpak":
        output = ""
        try:
            output = os.popen("flatpak --default-arch").read().strip()
        except Exception:
            output = ""
        if output:
            return normalize_arch_label(output)

    return normalize_arch_label(platform.machine())


def get_registry_package_name(package_type: str, arch_label: str) -> str:
    if package_type in {"linux", "flatpak"}:
        return f"{package_type}-{arch_label}"
    return package_type


def get_index_key(package_type: str, arch_label: str) -> str:
    if package_type in {"linux", "flatpak"}:
        return f"{package_type}-{arch_label}"
    return package_type


def write_sha256_sidecar(artifact_path: Path, output_dir: Path) -> Path:
    digest = hashlib.sha256(artifact_path.read_bytes()).hexdigest()
    out = output_dir / f"{artifact_path.name}.sha256"
    out.write_text(f"{digest}  {artifact_path.name}\n", encoding="ascii")
    return out


def gitlab_request(
    method: str,
    url: str,
    token: str,
    data: bytes | None = None,
    content_type: str | None = None,
) -> bytes:
    headers = {"PRIVATE-TOKEN": token}
    if content_type:
        headers["Content-Type"] = content_type
    request = urllib.request.Request(url=url, data=data, method=method, headers=headers)
    try:
        with urllib.request.urlopen(request) as response:
            return response.read()
    except urllib.error.HTTPError as exc:
        detail = ""
        try:
            body = exc.read().decode("utf-8", errors="replace")
            detail = f" Response: {body}"
        except Exception:
            detail = ""
        raise RuntimeError(f"GitLab API error {exc.code} on {method} {url}.{detail}") from exc


def assert_gitlab_package_registry_enabled(base_url: str, project_path: str, token: str) -> None:
    encoded_project = urllib.parse.quote(project_path, safe="")
    project_url = f"{base_url}/api/v4/projects/{encoded_project}"
    payload = gitlab_request("GET", project_url, token)
    project = json.loads(payload.decode("utf-8"))
    access_level = project.get("package_registry_access_level")
    packages_enabled = bool(project.get("packages_enabled", True))
    if access_level == "disabled" or not packages_enabled:
        web_project_url = f"{base_url}/{project_path}"
        raise RuntimeError(
            "GitLab Generic Package Registry is disabled for project "
            f"'{project_path}'. Enable it at: {web_project_url}/-/settings/general"
        )


def invoke_gitlab_upload(
    file_path: Path,
    base_url: str,
    project_path: str,
    package_name: str,
    version: str,
    token: str,
) -> str:
    encoded_project = urllib.parse.quote(project_path, safe="")
    file_name = file_path.name
    upload_url = (
        f"{base_url}/api/v4/projects/{encoded_project}/packages/generic/"
        f"{package_name}/{version}/{file_name}"
    )
    print(f"  Uploading: {file_name}")
    print(f"         to: {upload_url}")
    gitlab_request(
        "PUT",
        upload_url,
        token,
        data=file_path.read_bytes(),
        content_type="application/octet-stream",
    )
    return upload_url


def remove_old_gitlab_generic_package_versions(
    base_url: str,
    project_path: str,
    package_name: str,
    token: str,
    keep_latest: int,
) -> int:
    if keep_latest <= 0:
        return 0

    encoded_project = urllib.parse.quote(project_path, safe="")
    encoded_package_name = urllib.parse.quote(package_name, safe="")
    list_url = (
        f"{base_url}/api/v4/projects/{encoded_project}/packages?"
        f"package_type=generic&package_name={encoded_package_name}"
        "&order_by=created_at&sort=desc&per_page=100"
    )
    payload = gitlab_request("GET", list_url, token)
    packages = json.loads(payload.decode("utf-8"))

    ordered = [
        pkg
        for pkg in packages
        if pkg.get("name") == package_name and str(pkg.get("version", "")).strip()
    ]
    ordered.sort(key=lambda p: p.get("created_at", ""), reverse=True)

    unique_by_version = []
    seen_versions: set[str] = set()
    for pkg in ordered:
        version = str(pkg.get("version", "")).strip()
        if version in seen_versions:
            continue
        seen_versions.add(version)
        unique_by_version.append(pkg)

    to_delete = unique_by_version[keep_latest:]
    for pkg in to_delete:
        package_id = pkg.get("id")
        version = pkg.get("version")
        delete_url = f"{base_url}/api/v4/projects/{encoded_project}/packages/{package_id}"
        print(f"  Removing old package version: {version} (id={package_id})")
        gitlab_request("DELETE", delete_url, token)
    return len(to_delete)


def new_latest_release_index_json(
    index_key: str,
    package_type: str,
    architecture: str,
    display_name: str,
    version: str,
    artifact_name: str,
    artifact_url: str,
    hash_name: str,
    hash_url: str,
    notes: str,
    output_directory: Path,
) -> Path:
    payload = {
        "schemaVersion": 1,
        "packageType": package_type,
        "architecture": architecture,
        "displayName": display_name,
        "version": version,
        "publishedAtUtc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "artifact": {
            "name": artifact_name,
            "url": artifact_url,
        },
        "sha256": {
            "name": hash_name,
            "url": hash_url,
        },
        "notes": notes,
    }
    out_file = output_directory / f"{index_key}.json"
    out_file.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    return out_file


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

    gitlab_token = os.environ.get("GITLAB_TOKEN", "").strip()
    if not gitlab_token:
        raise RuntimeError(
            "GITLAB_TOKEN environment variable is not set. "
            "Set it to a GitLab Personal Access Token with 'api' scope before running deploy."
        )

    workspace_root = get_workspace_root(args.workspace_root)
    config = get_package_config(args.package_type)

    can_update_website = False
    if not args.skip_website_update:
        website_root = Path(args.website_root)
        if website_root.exists():
            can_update_website = True
        else:
            print(
                f"Warning: website repository not found at '{website_root}'. "
                "Continuing without website download page updates.",
                file=sys.stderr,
            )

    assert_gitlab_package_registry_enabled(
        base_url=args.gitlab_base_url,
        project_path=args.gitlab_project_path,
        token=gitlab_token,
    )

    version = get_project_version(workspace_root)
    artifact_dir = workspace_root / str(config["source_dir"])
    artifact = get_latest_artifact(
        directory=artifact_dir,
        include=list(config["include"]),
        exclude=list(config["exclude"]),
    )
    arch_label = detect_deploy_architecture(args.package_type, artifact.name)
    registry_package_name = get_registry_package_name(args.package_type, arch_label)
    index_key = get_index_key(args.package_type, arch_label)

    temp_dir = Path(tempfile.mkdtemp(prefix="nlc-deploy-"))
    try:
        sha256_file_path = write_sha256_sidecar(artifact, temp_dir)
        sha256_file_name = sha256_file_path.name

        artifact_url = invoke_gitlab_upload(
            file_path=artifact,
            base_url=args.gitlab_base_url,
            project_path=args.gitlab_project_path,
            package_name=registry_package_name,
            version=version,
            token=gitlab_token,
        )
        hash_url = invoke_gitlab_upload(
            file_path=sha256_file_path,
            base_url=args.gitlab_base_url,
            project_path=args.gitlab_project_path,
            package_name=registry_package_name,
            version=version,
            token=gitlab_token,
        )

        latest_index_json_path = new_latest_release_index_json(
            index_key=index_key,
            package_type=args.package_type,
            architecture=arch_label,
            display_name=str(config["display_name"]),
            version=version,
            artifact_name=artifact.name,
            artifact_url=artifact_url,
            hash_name=sha256_file_name,
            hash_url=hash_url,
            notes=str(config["notes"]),
            output_directory=temp_dir,
        )

        latest_index_url = invoke_gitlab_upload(
            file_path=latest_index_json_path,
            base_url=args.gitlab_base_url,
            project_path=args.gitlab_project_path,
            package_name="download-index",
            version="v1",
            token=gitlab_token,
        )

        deleted_version_count = remove_old_gitlab_generic_package_versions(
            base_url=args.gitlab_base_url,
            project_path=args.gitlab_project_path,
            package_name=registry_package_name,
            token=gitlab_token,
            keep_latest=args.keep_latest_versions,
        )

        timestamp = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")
        if can_update_website:
            update_download_page_section(
                website_repo_root=Path(args.website_root),
                section_key=args.package_type,
                display_name=str(config["display_name"]),
                artifact_name=artifact.name,
                artifact_url=artifact_url,
                hash_name=sha256_file_name,
                hash_url=hash_url,
                notes=str(config["notes"]),
                timestamp=timestamp,
            )

        print("")
        print(f"Deployed package type : {args.package_type}")
        print(f"  Version             : {version}")
        print(f"  Architecture        : {arch_label}")
        print(f"  Source artifact     : {artifact}")
        print(f"  GitLab project      : {args.gitlab_project_path}")
        print(f"  GitLab package      : {registry_package_name} / {version}")
        print(f"  Artifact URL        : {artifact_url}")
        print(f"  SHA-256 URL         : {hash_url}")
        print(f"  Index URL           : {latest_index_url}")
        print(f"  Retention keep      : {args.keep_latest_versions} latest version(s)")
        print(f"  Versions removed    : {deleted_version_count}")
        if can_update_website:
            print(f"  Download page       : {Path(args.website_root) / 'docs' / 'download.md'}")
        print("")
        if can_update_website:
            print(
                "Next: review and commit the website repository changes to publish "
                "the updated download page."
            )
        else:
            print(
                "Next: have the website fetch the Index URL for this package type "
                "(GitLab as source of truth)."
            )
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pylint: disable=broad-except
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
