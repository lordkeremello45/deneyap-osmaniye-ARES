# ARES — XVF3800 USB Audio Class 2.0 host agent

This agent is for the **XMOS XVF3800 UA/USB firmware configuration** connected to a computer as a standard USB Audio Class 2.0 input. It does not program the XVF3800 firmware or support a bare XVF3800 chip without its required board, clocking, power, flash and USB circuitry.

## Setup

1. Confirm the physical hardware is a supported XVF3800 evaluation/production assembly with UA/UAC2 firmware. The XMOS firmware image selects USB or I²S at build time; these modes are not runtime-switchable.
2. Start the ARES Go bridge on the same computer. It binds to `127.0.0.1:8080` by default.
3. Install Python 3.10+ and dependencies:

   ```sh
   python -m pip install -r requirements.txt
   python xvf3800_uac2.py --list
   ```

4. Set `ARES_XVF3800_DEVICE` to the exact device index or a unique substring from the listed device names. The script intentionally refuses to capture an arbitrary microphone.
5. Set `ARES_XVF3800_SAMPLE_RATE` to the firmware's configured rate (default 16000 Hz), and optionally set `ARES_XVF3800_CHANNELS` (default: up to four available input channels).
6. Run `python xvf3800_uac2.py`.

Example PowerShell:

```powershell
$env:ARES_XVF3800_DEVICE = "XVF3800"
$env:ARES_XVF3800_SAMPLE_RATE = "16000"
python .\xvf3800_uac2.py
```

## Data and safety

- The agent computes normalized aggregate RMS and peak from each audio block. It does not save raw audio or upload raw audio.
- The agent posts one summary to `POST /api/v1/audio/rms`; `GET /api/v1/audio` returns the latest summary while it is fresh (3 seconds).
- A fresh summary is **not** evidence of a trapped person. Acoustic features require separate signal-processing design, calibration and validation.
- The Go bridge remains loopback-only. Do not expose it to a network without authentication and transport security.
- If the UAC2 device's sample rate/channel layout differs from the configured values, set the environment variables to the exact firmware configuration. Do not guess channel layout based on microphone count.
