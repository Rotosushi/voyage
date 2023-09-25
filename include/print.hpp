#pragma once
#include <format>
#include <ostream>

#include "bytecode.hpp"
#include "common.hpp"
#include "value.hpp"

namespace voyage {

void print(std::ostream &out, Value const &value) {
  out << std::format("{:g}", value);
}

void print(std::ostream &out, Bytecode const &bytecode) noexcept {
  auto simple = [&](const char *name, size_t offset) -> size_t {
    out << std::format("{}\n", name);
    return offset + 1;
  };

  auto constant = [&](const char *name, size_t offset, size_t bytes) -> size_t {
    size_t index = bytecode.readImmediate(offset + 1, bytes);
    out << std::format("{:16s} {:4d} '", name, index);
    print(out, bytecode.constantAt(index));
    out << "'\n";
    return offset + 1 + bytes;
  };

  auto dispatch = [&](size_t offset) -> size_t {
    out << std::format("{:04d} ", offset);

    auto instruction = static_cast<Instruction>(bytecode[offset]);
    switch (instruction) {
    case Instruction::HALT:
      return simple("HALT", offset);
    case Instruction::RETURN:
      return simple("RETURN", offset);

    case Instruction::CONSTANT_U8:
      return constant("CONSTANT_U8", offset, sizeof(u8));
    case Instruction::CONSTANT_U16:
      return constant("CONSTANT_U16", offset, sizeof(u16));
    case Instruction::CONSTANT_U32:
      return constant("CONSTANT_U32", offset, sizeof(u32));
    case Instruction::CONSTANT_U64:
      return constant("CONSTANT_U64", offset, sizeof(u64));

    default:
      assert(false && "unreachable");
    }
  };

  for (size_t offset = 0; offset < bytecode.size();) {
    offset = dispatch(offset);
  }
}
} // namespace voyage
