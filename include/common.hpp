#pragma once

#include <cfloat>
#include <cstddef>
#include <cstdint>

#include <utility>

namespace voyage {
#if !defined(NDEBUG)
constexpr inline auto debug       = true;
constexpr inline auto debug_print = true;
#else
constexpr inline auto debug       = false;
constexpr inline auto debug_print = false;
#endif

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;
} // namespace voyage
