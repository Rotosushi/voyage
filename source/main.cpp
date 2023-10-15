#include <array>
#include <fstream>
#include <iostream>

#include "parser.hpp"
#include "virtual_machine.hpp"

static void repl(voyage::VirtualMachine &vm) {
  voyage::Parser parser;
  std::string    line;
  while (true) {
    std::cout << "> ";

    std::getline(std::cin, line);
    if (std::cin.eof() || std::cin.fail()) {
      std::cout << "\n";
      break;
    }
    line.append(1, '\n');

    auto parse_result = parser.parse(line);
    if (!parse_result) {
      line.clear();
      continue;
    }
    auto &bytecode         = parse_result.value();
    auto  interpret_result = vm.interpret(bytecode);
    if (!interpret_result) {
      auto &error = interpret_result.error();
      std::cerr << "Interpreter Error: " << error << "\n";
    }
    auto &value = interpret_result.value();
    std::cout << "-> " << value << "\n";
    line.clear();
  }
}

static std::string readFile(std::string_view path) {
  std::ifstream file{path.data()};
  if (!file.is_open()) {
    std::cerr << "Unable to open file [ " << path << " ]\n";
    std::exit(EXIT_FAILURE);
  }

  file.seekg(std::ios_base::end);
  size_t length = file.tellg();
  file.seekg(std::ios_base::beg);
  std::string buffer;
  buffer.reserve(length + 1);

  file.read(buffer.data(), length);
  return buffer;
}

static void script(voyage::VirtualMachine &vm, std::string_view file) {
  voyage::Parser parser;
  auto           source       = readFile(file);
  auto           parse_result = parser.parse(source);
  if (!parse_result) {
    std::exit(EXIT_FAILURE);
  }
  auto &bytecode = parse_result.value();

  auto interpret_result = vm.interpret(bytecode);
  if (!interpret_result) {
    std::cerr << interpret_result.error() << "\n";
    std::exit(EXIT_FAILURE);
  }
}

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