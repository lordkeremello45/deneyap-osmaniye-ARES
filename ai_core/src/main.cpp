#include "engine.hpp"
#include <iostream>
int main() {
  ares::Engine engine("models/gemma-3-1b-it-Q5_K_M.gguf");
  ares::SensorFrame frame{};
  std::cout << engine.analyze(frame) << std::endl;
  return 0;
}
