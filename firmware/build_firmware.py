#!/usr/bin/env python3
"""Build ARES Card 1/Card 2 PlatformIO firmware and record binary hashes."""
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = ROOT / "firmware"
OUTPUT = FIRMWARE / "build_artifacts"
ENVIRONMENTS = {
    "card1": ("ares-card1-gateway", "card1-gateway.bin"),
    "card2": ("ares-card2-sensors", "card2-sensors.bin"),
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_revision() -> str | None:
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "HEAD"], cwd=ROOT, text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except (OSError, subprocess.CalledProcessError):
        return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--role", choices=("card1", "card2", "both"), default="both")
    args = parser.parse_args()

    pio = shutil.which("pio") or shutil.which("platformio")
    if not pio:
        print("PlatformIO Core was not found. Install it and ensure pio is on PATH.", file=sys.stderr)
        return 2

    roles = ("card1", "card2") if args.role == "both" else (args.role,)
    OUTPUT.mkdir(parents=True, exist_ok=True)
    manifest_path = OUTPUT / "manifest.json"
    manifest_path.unlink(missing_ok=True)
    manifest = {
        "project": "ARES",
        "built_at_utc": datetime.now(timezone.utc).isoformat(),
        "git_revision": git_revision(),
        "artifacts": [],
    }

    for role in roles:
        environment, filename = ENVIRONMENTS[role]
        command = [pio, "run", "-d", str(FIRMWARE), "-e", environment]
        print("+", " ".join(command))
        result = subprocess.run(command, cwd=ROOT, check=False)
        if result.returncode != 0:
            print(f"Build failed for {environment}; no success manifest written.", file=sys.stderr)
            return result.returncode or 1

        binary = FIRMWARE / ".pio" / "build" / environment / "firmware.bin"
        if not binary.is_file() or binary.stat().st_size == 0:
            print(f"Expected non-empty firmware binary not found: {binary}", file=sys.stderr)
            return 3

        destination = OUTPUT / filename
        shutil.copy2(binary, destination)
        manifest["artifacts"].append({
            "role": role,
            "platformio_environment": environment,
            "file": destination.name,
            "size_bytes": destination.stat().st_size,
            "sha256": sha256(destination),
        })
        print(f"Artifact: {destination} ({destination.stat().st_size} bytes)")

    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"Build manifest: {manifest_path}")
    print("Build artifacts are compiled binaries only; this tool does not flash hardware.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
