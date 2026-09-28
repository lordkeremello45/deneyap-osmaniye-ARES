# ARES C++ AI Core

Native C++20/CMake analysis core.

Pipeline:

`SensorFrame → validation/features → deterministic fusion → Gemma 3 1B Q5_K_M (llama.cpp) → versioned result`

The current executable contains the deterministic fusion path and the stable result contract first. The llama.cpp model adapter is kept behind the same engine boundary so model/runtime validation does not contaminate sensor transport code.

Model file expected at `models/gemma-3-1b-it-Q5_K_M.gguf`.
