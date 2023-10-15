#pragma once
#include <format>
#include <ostream>

#include <vector>

namespace voyage {
template <class T> class Stack {
public:
  using Data           = std::vector<T>;
  using iterator       = Data::iterator;
  using const_iterator = Data::const_iterator;

private:
  Data   m_data;
  size_t m_top = 0;

public:
  void reset() noexcept {
    m_data.clear();
    m_top = 0;
  }
  [[nodiscard]] bool   empty() const noexcept { return m_data.empty(); }
  [[nodiscard]] size_t size() const noexcept { return m_data.size(); }

  void push(T element) noexcept {
    m_data.push_back(std::move(element));
    m_top++;
  }
  T pop() noexcept {
    T t = m_data.back();
    m_data.pop_back();
    m_top--;
    return t;
  }
  T &peek(size_t offset = 0) noexcept {
    size_t index = m_top - 1U - offset;
    return m_data[index];
  }

  [[nodiscard]] iterator       begin() noexcept { return m_data.begin(); }
  [[nodiscard]] iterator       end() noexcept { return m_data.end(); }
  [[nodiscard]] const_iterator begin() const noexcept { return m_data.begin(); }
  [[nodiscard]] const_iterator end() const noexcept { return m_data.end(); }
};

template <class T>
void print(std::ostream &out, Stack<T> const &stack) noexcept {
  for (T const &element : stack) {
    out << "[ " << element << " ]";
  }
}

template <class T>
std::ostream &operator<<(std::ostream &out, Stack<T> const &stack) noexcept {
  print(out, stack);
  return out;
}
} // namespace voyage
