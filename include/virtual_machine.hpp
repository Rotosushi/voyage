#pragma once
#if !defined(NDEBUG)
#include <iostream>
#endif

#if __cplusplus > 202002L && __cpp_concepts >= 202002L
#include <expected>
#else
#error "<expected> unavailable"
#endif

#include "bytecode.hpp"
#include "common.hpp"
#include "error.hpp"
#include "stack.hpp"

namespace voyage {
class VirtualMachine {
public:
private:
  Stack<Value> m_stack;

  auto result(Value value) -> std::expected<Value, Error> { return {value}; }

  auto result(Error error) -> std::expected<Value, Error> {
    return std::unexpected{std::move(error)};
  }

public:
  std::expected<Value, Error> interpret(Bytecode &bytecode) noexcept {
    Bytecode::iterator ip = bytecode.begin();
    auto read_byte = [&]() { return *ip++; };
    auto read_constant = [&](size_t bytes) -> Value {
      auto index = bytecode.readImmediate(ip, bytes);
      ip += (std::ptrdiff_t)(bytes);
      return bytecode.constantAt(index);
    };

    while (true) {
      if constexpr (debug) {
        print_instruction(std::cerr, bytecode,
                          (size_t)(std::distance(bytecode.begin(), ip)));
      }

      switch ((Instruction)(read_byte())) {
      case Instruction::RETURN: {
        if (m_stack.empty()) {
          // #TODO return nil
          return result({0.0});
        }
        return result(m_stack.pop());
      }

      case Instruction::CONSTANT_U8: {
        Value value = read_constant(sizeof(u8));
        m_stack.push(value);
        break;
      }

      case Instruction::CONSTANT_U16: {
        Value value = read_constant(sizeof(u16));
        m_stack.push(value);
        break;
      }

      case Instruction::CONSTANT_U32: {
        Value value = read_constant(sizeof(u32));
        m_stack.push(value);
        break;
      }

      case Instruction::CONSTANT_U64: {
        Value value = read_constant(sizeof(u64));
        m_stack.push(value);
        break;
      }

      default: {
        return result(Error{Error::Kind::Runtime, "unknown instruction",
                            bytecode.getLine(ip - 1)});
      }
      }

      if constexpr (debug) {
        print(std::cerr, m_stack);
        std::cerr << "\n";
      }
    }
  }
};
} // namespace voyage
