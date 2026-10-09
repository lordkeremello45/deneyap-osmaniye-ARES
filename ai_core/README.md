# ARES C++ AI Core

Native C++20/CMake analysis core.

Pipeline:

`SensorFrame → validation/features → deterministic fusion → Gemma 4 E2B Q5_K_M (llama.cpp) → versioned result`

The current executable contains the deterministic fusion path and the stable result contract first. The llama.cpp model adapter is kept behind the same engine boundary so model/runtime validation does not contaminate sensor transport code.

Expected model file: `models/google_gemma-4-E2B-it-Q5_K_M.gguf`. The model is a host-side runtime dependency, not something to run on the Deneyap Kart V2. The model binary is not committed to Git. See [model runtime notes](../models/README.md) for the source and license terms.

The model is advisory only. It does not own flight stabilization, motor PWM, or failsafe decisions. No model inference or survivor-detection accuracy is claimed until the adapter and validation tests exist.
