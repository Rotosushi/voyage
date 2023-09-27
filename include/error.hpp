#pragma once
#include <cassert>
#include <format>
#include <ostream>
#include <string>

namespace voyage {
class Error {
public:
  enum class Kind {
    Comptime,
    Runtime,
  };

private:
  Kind m_kind;
  std::string m_msg;
  size_t m_line;

public:
  Error(Kind kind, std::string_view msg, size_t line) noexcept
      : m_kind(kind), m_msg(msg), m_line(line) {}

  std::string_view kind() const noexcept {
    switch (m_kind) {
    case Kind::Comptime:
      return "Comptime";
    case Kind::Runtime:
      return "Runtime";
    default:
      assert(false && "unreachable");
    }
  }
  std::string_view msg() const noexcept { return m_msg; }
  size_t line() const noexcept { return m_line; }
};

void print(std::ostream &out, Error const &error) {
  out << std::format("[line: {:4d}] {:8s} {:s}", error.line(), error.kind(),
                     error.msg());
}

std::ostream &operator<<(std::ostream &out, Error const &error) {
  print(out, error);
  return out;
}
} // namespace voyage
