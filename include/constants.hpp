#pragma once
#include <cassert>
#include <vector>

#include "common.hpp"
#include "value.hpp"

namespace voyage {
class Constants {
public:
  using Array           = std::vector<Value>;
  using iterator        = Array::iterator;
  using pointer         = Array::pointer;
  using reference       = Array::reference;
  using const_iterator  = Array::const_iterator;
  using const_pointer   = Array::const_pointer;
  using const_reference = Array::const_reference;

private:
  Array m_array;

public:
  size_t write(Value value) {
    m_array.push_back(value);
    return m_array.size() - 1;
  }

  reference operator[](size_t position) noexcept {
    assert(position < m_array.size());
    return m_array[position];
  }

  const_reference operator[](size_t position) const noexcept {
    assert(position < m_array.size());
    return m_array[position];
  }
};
} // namespace voyage
