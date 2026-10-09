# ARES model runtime

## Selected model

- Model: **Gemma 4 E2B Instruct**
- Quantization: **Q5_K_M**
- Expected local file: `models/google_gemma-4-E2B-it-Q5_K_M.gguf`
- Approximate artifact size: **3.66 GB**; actual download size can vary by hosting metadata.
- GGUF source: [bartowski/google_gemma-4-E2B-it-GGUF](https://huggingface.co/bartowski/google_gemma-4-E2B-it-GGUF)
- Upstream model family and terms: [Google Gemma](https://ai.google.dev/gemma/terms)
- Runtime target: `llama.cpp` on the **host computer**, not the Deneyap Kart V2.

The binary model is intentionally not committed to Git. Download it from the model source and place it at the path above. Review Google's Gemma terms and the quantization repository's notices before use or redistribution; ARES's GPL/AGPL source licenses do not override model terms.

## Inference boundary

`validated sensor features → prompt/structured input → Gemma → structured analysis result`

Raw high-rate sensor streams never go directly into the model. Deterministic validation and sensor fusion remain authoritative; model output is advisory and must never drive flight stabilization, motor PWM, or failsafe control.

**Status:** this selects the model artifact and path only. It does not claim that the model has been downloaded, that a llama.cpp adapter is implemented, or that inference has been tested.
