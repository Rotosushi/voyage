#pragma once
#include "common.hpp"

namespace voyage {
enum class Instruction : u8 {
  RETURN,

  CONSTANT_U8,
  CONSTANT_U16,
  CONSTANT_U32,
  CONSTANT_U64,
};
} // namespace voyage
