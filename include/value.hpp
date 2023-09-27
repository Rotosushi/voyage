#pragma once
#include <format>
#include <ostream>

namespace voyage {
struct Value {
  double data;
};

void print(std::ostream &out, Value const &value) {
  out << std::format("{:10.5g}", value.data);
}

std::ostream &operator<<(std::ostream &out, Value const &value) {
  print(out, value);
  return out;
}
} // namespace voyage
