#!/usr/bin/env python3
"""Capture normalized RMS/peak metrics from an XMOS XVF3800 UAC2 input.

Raw audio is processed in memory and is never written to disk or transmitted.
Only one-second RMS/peak summaries are sent to the loopback ARES Go bridge.
"""
from __future__ import annotations

import argparse
import json
import os
import ipaddress
import queue
import sys
import time
import urllib.error
import urllib.request
from datetime import datetime, timezone
from urllib.parse import urlsplit

import numpy as np
import sounddevice as sd

SOURCE = "xvf3800_uac2"


def choose_device(selector: str | None):
    devices = sd.query_devices()
    candidates = []
    for index, device in enumerate(devices):
        if int(device["max_input_channels"]) < 1:
            continue
        name = str(device["name"])
        if selector:
            if selector.isdigit():
                if index == int(selector):
                    candidates.append((index, device))
            elif selector.casefold() in name.casefold():
                candidates.append((index, device))
        elif any(token in name.casefold() for token in ("xvf3800", "xmos", "vocalfusion")):
            candidates.append((index, device))

    if len(candidates) != 1:
        available = [
            f"{i}: {d['name']} (input_channels={d['max_input_channels']})"
            for i, d in enumerate(devices)
            if int(d["max_input_channels"]) > 0
        ]
        if selector and not candidates:
            reason = f"No input device matches ARES_XVF3800_DEVICE={selector!r}."
        elif len(candidates) > 1:
            reason = "Device selector matches more than one input device."
        else:
            reason = "No unambiguous XMOS/VocalFusion input device was found."
        raise RuntimeError(reason + "\nInput devices:\n  " + "\n  ".join(available))
    return candidates[0]


def validate_bridge_url(raw_url: str) -> str:
    parsed = urlsplit(raw_url)
    if parsed.scheme != "http" or parsed.username or parsed.password:
        raise ValueError("Bridge URL must use plain HTTP only to a numeric loopback IP; external hosts and credentials are forbidden.")
    if parsed.query or parsed.fragment or parsed.path != "/api/v1/audio/rms":
        raise ValueError("Bridge URL must target exactly /api/v1/audio/rms without query or fragment.")
    try:
        host = ipaddress.ip_address(parsed.hostname or "")
    except ValueError as exc:
        raise ValueError("Bridge hostname must be a numeric loopback IP (127.0.0.1 or ::1); DNS names are forbidden.") from exc
    if not host.is_loopback:
        raise ValueError("Bridge URL must target a loopback IP; audio metrics must not be sent to external hosts.")
    try:
        port = parsed.port
    except ValueError as exc:
        raise ValueError("Bridge URL has an invalid port.") from exc
    if port is None or not (1 <= port <= 65535):
        raise ValueError("Bridge URL must include a valid explicit port.")
    return raw_url


def post_sample(url: str, sample: dict, token: str) -> None:
    body = json.dumps(sample, separators=(",", ":")).encode("utf-8")
    request = urllib.request.Request(
        url, data=body, headers={"Content-Type": "application/json", "Authorization": f"Bearer {token}"}, method="POST"
    )
    with urllib.request.urlopen(request, timeout=2.0) as response:
        if response.status != 202:
            raise RuntimeError(f"Bridge rejected audio sample: HTTP {response.status}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list", action="store_true", help="list available audio input devices")
    args = parser.parse_args()

    if args.list:
        print(sd.query_devices())
        return 0

    selector = os.environ.get("ARES_XVF3800_DEVICE")
    try:
        device_index, device = choose_device(selector)
        sample_rate = int(os.environ.get("ARES_XVF3800_SAMPLE_RATE", "16000"))
        configured_channels = os.environ.get("ARES_XVF3800_CHANNELS")
        channels = int(configured_channels) if configured_channels else min(4, int(device["max_input_channels"]))
        if sample_rate < 8000 or sample_rate > 96000:
            raise ValueError("ARES_XVF3800_SAMPLE_RATE must be between 8000 and 96000.")
        if channels < 1 or channels > min(8, int(device["max_input_channels"])):
            raise ValueError("ARES_XVF3800_CHANNELS exceeds the device's available input channels.")
        sd.check_input_settings(device=device_index, channels=channels, samplerate=sample_rate, dtype="float32")
    except Exception as exc:
        print(f"XVF3800 setup error: {exc}", file=sys.stderr)
        return 2

    bridge_url = os.environ.get("ARES_BRIDGE_AUDIO_URL", "http://127.0.0.1:8080/api/v1/audio/rms")
    bridge_token = os.environ.get("ARES_BRIDGE_TOKEN", "")
    try:
        validate_bridge_url(bridge_url)
        if len(bridge_token) < 32:
            raise ValueError("ARES_BRIDGE_TOKEN must be a random secret of at least 32 characters.")
    except ValueError as exc:
        print(f"Bridge security configuration error: {exc}", file=sys.stderr)
        return 2
    samples: queue.Queue[dict] = queue.Queue(maxsize=1)

    def audio_callback(indata, frames, timing, status):
        if status:
            print(f"Audio stream warning: {status}", file=sys.stderr)
        data = np.asarray(indata, dtype=np.float64)
        rms = float(np.sqrt(np.mean(np.square(data)))) if data.size else 0.0
        peak = float(np.max(np.abs(data))) if data.size else 0.0
        sample = {
            "source": SOURCE,
            "captured_at": datetime.now(timezone.utc).isoformat(timespec="milliseconds").replace("+00:00", "Z"),
            "sample_rate_hz": sample_rate,
            "channels": channels,
            "rms": min(1.0, max(0.0, rms)),
            "peak": min(1.0, max(0.0, peak)),
        }
        try:
            samples.put_nowait(sample)
        except queue.Full:
            try:
                samples.get_nowait()
            except queue.Empty:
                pass
            try:
                samples.put_nowait(sample)
            except queue.Full:
                pass

    print(f"XVF3800 UAC2 input: {device['name']} (device={device_index}, {sample_rate} Hz, {channels} channel(s))")
    print(f"Sending RMS/peak summaries to {bridge_url}; raw audio is not stored.")
    try:
        with sd.InputStream(
            device=device_index,
            samplerate=sample_rate,
            channels=channels,
            dtype="float32",
            blocksize=sample_rate,
            callback=audio_callback,
        ):
            while True:
                try:
                    sample = samples.get(timeout=2.0)
                except queue.Empty:
                    print("No audio callback received within 2 seconds.", file=sys.stderr)
                    continue
                try:
                    post_sample(bridge_url, sample, bridge_token)
                except (urllib.error.URLError, TimeoutError, RuntimeError) as exc:
                    print(f"Bridge upload failed: {exc}", file=sys.stderr)
                else:
                    print(json.dumps(sample, separators=(",", ":")))
                time.sleep(0.01)
    except KeyboardInterrupt:
        print("\nXVF3800 capture stopped.")
        return 0
    except Exception as exc:
        print(f"XVF3800 stream error: {exc}", file=sys.stderr)
        return 3


if __name__ == "__main__":
    raise SystemExit(main())
