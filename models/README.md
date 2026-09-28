# ARES model runtime

Expected local model artifact:

- `gemma-3-1b-it-Q5_K_M.gguf`
- Runtime: llama.cpp
- Location: `models/`

The binary model is intentionally not committed to Git. `ai_core` keeps the model path as a runtime configuration so the same native core can be tested without shipping a large binary in the repository.

The inference boundary is:

`validated sensor features → prompt/structured input → Gemma → structured analysis result`

Raw high-rate sensor streams never go directly into the model.
