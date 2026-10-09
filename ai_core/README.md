# ARES C++ AI Core

Native C++20/CMake analysis core.

Current implementation is a deterministic, uncalibrated threshold baseline only. It is not Gemma inference and is not a validated survivor detector. The model-path constructor argument is reserved for a future runtime adapter; it does not load the model today.

Pipeline target:

validated SensorFrame → time alignment/windowing → deterministic fusion → optional Gemma advisory → versioned result

The output sets confidence to null, advisory_only to true, and validated to false until a calibrated model and validation evidence exist. Invalid timestamps, non-finite values and generic out-of-range values are rejected.

Expected future model file: models/google_gemma-4-E2B-it-Q5_K_M.gguf. The model is a host-side runtime dependency, not something to run on the Deneyap Kart V2. The model binary is not committed to Git. See [model runtime notes](../models/README.md) for source and license terms.

The AI core does not own flight stabilization, motor PWM, arming, or failsafe decisions. No model inference or survivor-detection accuracy is claimed until the runtime adapter and controlled validation tests exist.
