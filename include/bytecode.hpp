#pragma once
#include <cassert>
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

private:
  Chunk m_chunk;
  Constants m_constants;

  size_t addConstant(Value value) { return m_constants.write(value); }

  void write(u8 byte) { m_chunk.push_back(byte); }
  void write(Instruction instruction) {
    m_chunk.push_back(static_cast<u8>(instruction));
  }

  void writeImmediate(size_t immediate, size_t bytes) {
    assert((bytes == sizeof(u8)) || (bytes == sizeof(u16)) ||
           (bytes == sizeof(u32)) || (bytes == sizeof(u64)));

    for (size_t i = 0, j = bytes; i < bytes; ++i, --j) {
      size_t shift = (j * 8) - 8;
      u8 byte = (immediate >> shift) & 0xFF;
      write(byte);
    }
  }

public:
  Constants::reference constantAt(size_t position) noexcept {
    return m_constants[position];
  }
  Constants::const_reference constantAt(size_t position) const noexcept {
    return m_constants[position];
  }

  bool empty() const noexcept { return m_chunk.empty(); }
  size_t size() const noexcept { return m_chunk.size(); }

  size_t readImmediate(size_t offset, size_t bytes) const {
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

  void emitHalt() { write(Instruction::HALT); }

  void emitReturn() { write(Instruction::RETURN); }

  void emitConstant(Value value) {
    size_t index = addConstant(value);
    if (index <= UINT8_MAX) {
      write(Instruction::CONSTANT_U8);
      writeImmediate(index, sizeof(u8));
    } else if (index <= UINT16_MAX) {
      write(Instruction::CONSTANT_U16);
      writeImmediate(index, sizeof(u16));
    } else if (index <= UINT32_MAX) {
      write(Instruction::CONSTANT_U32);
      writeImmediate(index, sizeof(u32));
    } else {
      write(Instruction::CONSTANT_U64);
      writeImmediate(index, sizeof(u64));
    }
  }
};
} // namespace voyage
