#include <array>
#include <iostream>

#include "bytecode.hpp"
#include "virtual_machine.hpp"

static void repl(voyage::VirtualMachine &vm) {
  //   std::string line;
  //   while (true) {
  //     std::cout << "> ";

  //     std::getline(std::cin, line);
  //     if (std::cin.eof() || std::cin.fail()) {
  //       std::cout << "\n";
  //       break;
  //     }
  //     line.append(1, '\n');
  //
  //     auto bytecode = compile(line);
  //     vm.interpret(bytecode);
  //     line.clear();
  //   }
}

static void script(voyage::VirtualMachine &vm, char *file) {}

int main(int argc, char *argv[]) {
  voyage::VirtualMachine vm;

  if (argc == 1) {
    repl(vm);
  } else if (argc == 2) {
    script(vm, argv[1]);
  } else {
    std::cerr << "Usage: voyage [path]\n";
  }

  return EXIT_SUCCESS;
}