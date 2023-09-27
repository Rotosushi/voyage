#include <iostream>

#include "bytecode.hpp"
#include "virtual_machine.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  voyage::Bytecode bytecode;
  bytecode.emitConstant({1.2}, 1);
  bytecode.emitReturn(2);

  print(std::cout, bytecode);

  voyage::VirtualMachine vm;

  auto result = vm.interpret(bytecode);
  if (result) {
    std::cout << result.value() << "\n";
  } else {
    std::cout << result.error() << "\n";
  }

  return EXIT_SUCCESS;
}