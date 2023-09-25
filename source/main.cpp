#include <iostream>

#include "print.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  voyage::Bytecode bytecode;

  voyage::Value value = 0.001;
  for (size_t i = 0; i < UINT16_MAX + 100; ++i) {
    bytecode.emitConstant(value);
    value += 0.001;
  }

  print(std::cout, bytecode);

  return EXIT_SUCCESS;
}