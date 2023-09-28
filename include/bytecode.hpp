#pragma once
#include <cassert>
#include <format>
#include <ostream>
#include <vector>

#include "common.hpp"
#include "constants.hpp"
#include "instructions.hpp"

namespace voyage {
class Bytecode {
public:
  using Chunk = std::vector<u8>;
  using iterator = Chunk::iterator;
  using pointer = Chunk::pointer;
  using reference = Chunk::reference;
  using const_iterator = Chunk::const_iterator;
  using const_pointer = Chunk::const_pointer;
  using const_reference = Chunk::const_reference;

  class Lines {
  public:
    struct Run {
      size_t m_length;
      size_t m_line;
    };

  private:
    std::vector<Run> m_runs;

  public:
    void add(size_t line) noexcept {
      if (!m_runs.empty()) {
        auto &back = m_runs.back();
        if (back.m_line == line) {
          back.m_length++;
          return;
        }
      }
      m_runs.emplace_back(Run{1, line});
    }

    // each time we insert an instruction, we add it's line
    // to the run length encoding. thus, given an
    // instruction offset, the line that instruction appears
    // on is the line number in the run assciated with that
    // offset. the association is implicit in the fact that
    // we either add one to an existing run or add a new run
    // when we add an instructions line number.
    size_t get(size_t instruction_offset) const noexcept {
      size_t i = 0;
      for (auto &run : m_runs) {
        i += run.m_length;
        // the first time i is greater than or equal to the offset
        // we know that the instruction appears on the line
        // represented by that run.
        if (i >= instruction_offset) {
          return run.m_line;
        }
      }
      // the offset given is not within the
      // run length encoding. this is an error.
      // return an invalid line number.
      return 0;
    }
  };

private:
  Chunk m_chunk;
  Constants m_constants;
  Lines m_lines;

  size_t addConstant(Value value) { return m_constants.write(value); }

  void write(u8 byte, size_t line) {
    m_chunk.push_back(byte);
    m_lines.add(line);
  }
  void write(Instruction instruction, size_t line) {
    m_chunk.push_back(static_cast<u8>(instruction));
    m_lines.add(line);
  }

  void writeImmediate(size_t immediate, size_t bytes, size_t line) {
    assert((bytes == sizeof(u8)) || (bytes == sizeof(u16)) ||
           (bytes == sizeof(u32)) || (bytes == sizeof(u64)));

    for (size_t i = 0, j = bytes; i < bytes; ++i, --j) {
      size_t shift = (j * 8) - 8;
      u8 byte = (immediate >> shift) & 0xFF;
      write(byte, line);
    }
  }

public:
  size_t getLine(iterator i) const noexcept {
    return getLine((size_t)(i - begin()));
  }
  size_t getLine(const_iterator i) const noexcept {
    return getLine((size_t)(i - begin()));
  }
  size_t getLine(size_t offset) const noexcept { return m_lines.get(offset); }

  Constants::reference constantAt(size_t position) noexcept {
    return m_constants[position];
  }
  Constants::const_reference constantAt(size_t position) const noexcept {
    return m_constants[position];
  }

  bool empty() const noexcept { return m_chunk.empty(); }
  size_t size() const noexcept { return m_chunk.size(); }

  size_t readImmediate(iterator i, size_t bytes) const noexcept {
    return readImmediate((size_t)(i - begin()), bytes);
  }
  size_t readImmediate(const_iterator i, size_t bytes) const noexcept {
    return readImmediate((size_t)(i - begin()), bytes);
  }
  size_t readImmediate(size_t offset, size_t bytes) const noexcept {
    assert((bytes == sizeof(u8)) || (bytes == sizeof(u16)) ||
           (bytes == sizeof(u32)) || (bytes == sizeof(u64)));
    assert(offset + bytes <= size());

    size_t result = 0;
    for (size_t i = 0, j = bytes; i < bytes; ++i, --j) {
      size_t shift = (8 * j) - 8;
      u8 byte = m_chunk[offset + i];
      result |= (size_t)byte << shift;
    }
    return result;
  }

  [[nodiscard]] u8 operator[](size_t position) noexcept {
    assert(position < size());
    return m_chunk[position];
  }

  [[nodiscard]] u8 operator[](size_t position) const noexcept {
    assert(position < size());
    return m_chunk[position];
  }

  [[nodiscard]] iterator begin() noexcept { return m_chunk.begin(); }
  [[nodiscard]] iterator end() noexcept { return m_chunk.end(); }
  [[nodiscard]] const_iterator begin() const noexcept {
    return m_chunk.begin();
  }
  [[nodiscard]] const_iterator end() const noexcept { return m_chunk.end(); }

  void emitReturn(size_t line) { write(Instruction::RETURN, line); }

  void emitConstant(Value value, size_t line) {
    size_t index = addConstant(value);
    if (index <= UINT8_MAX) {
      write(Instruction::CONSTANT_U8, line);
      writeImmediate(index, sizeof(u8), line);
    } else if (index <= UINT16_MAX) {
      write(Instruction::CONSTANT_U16, line);
      writeImmediate(index, sizeof(u16), line);
    } else if (index <= UINT32_MAX) {
      write(Instruction::CONSTANT_U32, line);
      writeImmediate(index, sizeof(u32), line);
    } else {
      write(Instruction::CONSTANT_U64, line);
      writeImmediate(index, sizeof(u64), line);
    }
  }

  void emitNegate(size_t line) { write(Instruction::NEGATE, line); }
  void emitAdd(size_t line) { write(Instruction::ADD, line); }
  void emitSub(size_t line) { write(Instruction::SUB, line); }
  void emitMul(size_t line) { write(Instruction::MUL, line); }
  void emitDiv(size_t line) { write(Instruction::DIV, line); }
};

size_t print_simple(std::ostream &out, const char *name,
                    size_t offset) noexcept {
  out << std::format("{}\n", name);
  return offset + 1;
}

size_t print_constant(std::ostream &out, const char *name,
                      Bytecode const &bytecode, size_t offset, size_t bytes) {
  size_t index = bytecode.readImmediate(offset + 1, bytes);
  out << std::format("{:16s} {:4d} '", name, index);
  print(out, bytecode.constantAt(index));
  out << "'\n";
  return offset + 1 + bytes;
}

size_t print_dispatch(std::ostream &out, Bytecode const &bytecode,
                      size_t offset) noexcept {
  auto instruction = static_cast<Instruction>(bytecode[offset]);
  switch (instruction) {
  case Instruction::RETURN:
    return print_simple(out, "RETURN", offset);

  case Instruction::CONSTANT_U8:
    return print_constant(out, "CONSTANT_U8", bytecode, offset, sizeof(u8));
  case Instruction::CONSTANT_U16:
    return print_constant(out, "CONSTANT_U16", bytecode, offset, sizeof(u16));
  case Instruction::CONSTANT_U32:
    return print_constant(out, "CONSTANT_U32", bytecode, offset, sizeof(u32));
  case Instruction::CONSTANT_U64:
    return print_constant(out, "CONSTANT_U64", bytecode, offset, sizeof(u64));

  case Instruction::NEGATE:
    return print_simple(out, "NEGATE", offset);

  case Instruction::ADD:
    return print_simple(out, "ADD", offset);
  case Instruction::SUB:
    return print_simple(out, "SUB", offset);
  case Instruction::MUL:
    return print_simple(out, "MUL", offset);
  case Instruction::DIV:
    return print_simple(out, "DIV", offset);

  default:
    assert(false && "unreachable");
  }
}

size_t print_instruction(std::ostream &out, Bytecode const &bytecode,
                         size_t offset) {
  out << std::format("{:04d} {:4d} ", offset, bytecode.getLine(offset));
  return print_dispatch(out, bytecode, offset);
}

void print(std::ostream &out, Bytecode const &bytecode) noexcept {

  size_t prev_line = 0;
  auto instruction = [&](size_t offset) -> size_t {
    out << std::format("{:04d} ", offset);
    if (offset == 0) {
      prev_line = bytecode.getLine(offset);
      out << std::format("{:4d} ", prev_line);
    } else {
      size_t line = bytecode.getLine(offset);
      if (prev_line == line) {
        out << "   | ";
      } else {
        prev_line = line;
        out << std::format("{:4d} ", line);
      }
    }

    return print_dispatch(out, bytecode, offset);
  };

  for (size_t offset = 0; offset < bytecode.size();) {
    offset = instruction(offset);
  }
}

std::ostream &operator<<(std::ostream &out, Bytecode const &bytecode) noexcept {
  print(out, bytecode);
  return out;
}
} // namespace voyage
