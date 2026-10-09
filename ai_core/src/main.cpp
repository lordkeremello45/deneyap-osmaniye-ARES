#include "engine.hpp"
#include <iostream>

int main() {
  ares::Engine engine("models/google_gemma-4-E2B-it-Q5_K_M.gguf");
  ares::SensorFrame frame{};
  std::cout << engine.analyze(frame) << std::endl;
  return 0;
}
