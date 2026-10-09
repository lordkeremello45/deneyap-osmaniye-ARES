#include "engine.hpp"

#include <iostream>

int main() {
  // Live telemetry is not connected yet. The default frame has timestamp 0,
  // so this reports invalid input rather than presenting it as sensor evidence.
  ares::Engine engine("models/google_gemma-4-E2B-it-Q5_K_M.gguf");
  const ares::SensorFrame frame{};
  std::cout << engine.analyze(frame) << '\n';
  return 0;
}
