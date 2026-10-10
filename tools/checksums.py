#!/usr/bin/env python3
"""Print SHA-256, SHA-512, and SHA3-512 digests for a file."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path


def digest_file(path: Path) -> dict[str, str]:
    algorithms = {
        "sha256": hashlib.sha256(),
        "sha512": hashlib.sha512(),
        "sha3-512": hashlib.sha3_512(),
    }
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            for digest in algorithms.values():
                digest.update(chunk)
    return {name: digest.hexdigest() for name, digest in algorithms.items()}


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate SHA-256, SHA-512, and SHA3-512 file checksums."
    )
    parser.add_argument("file", type=Path, help="File to hash")
    args = parser.parse_args()
    if not args.file.is_file():
        parser.error(f"not a file: {args.file}")
    print(json.dumps({"file": str(args.file), "checksums": digest_file(args.file)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
