#pragma once
#include <cstring>
#include <string_view>

#include "token.hpp"

namespace voyage {
class Scanner {
public:
  using iterator = std::string_view::const_iterator;

private:
  iterator m_start;
  iterator m_cursor;
  size_t   m_line;

  static bool isDigit(char c) noexcept { return c >= '0' && c <= '9'; }
  static bool isID(char c) noexcept {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
  }

  Token make(Token::Kind kind) const noexcept {
    std::string_view text{m_start, m_cursor};
    return {kind, text, m_line};
  }

  Token error(std::string_view msg) const noexcept {
    return {Token::ERROR, msg, m_line};
  }

  char next() noexcept {
    m_cursor++;
    return m_cursor[-1];
  }

  char peek() const noexcept { return *m_cursor; }
  char peekNext() const noexcept {
    if (atEnd()) {
      return '\0';
    }

    return m_cursor[1];
  }

  void skipWhitespace() noexcept {
    while (true) {
      switch (peek()) {
      case '\n':
        m_line++;
        [[fallthrough]];
      case ' ':
      case '\r':
      case '\t':
        next();
        break;

      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !atEnd()) {
            next();
          }
          break;
        }
        [[fallthrough]];
      default:
        return;
      }
    }
  }

  bool match(char c) noexcept {
    if (atEnd()) {
      return false;
    }

    if (peek() != c) {
      return false;
    }

    m_cursor++;
    return true;
  }

  Token string() noexcept {
    while (peek() != '\n' && !atEnd()) {
      if (peek() == '\n') {
        m_line++;
      }

      next();
    }

    if (atEnd()) {
      return error("Unterminated string.");
    }

    next();
    return make(Token::STRING);
  }

  Token number() noexcept {
    while (isDigit(peek())) {
      next();
    }

    if (peek() == '.' && isDigit(peekNext())) {
      next();

      while (isDigit(peek())) {
        next();
      }
    }

    return make(Token::NUMBER);
  }

  Token::Kind checkKeyword(int begin, int length, const char *rest,
                           Token::Kind kind) const noexcept {
    if ((std::distance(m_start, m_cursor) == (begin + length)) &&
        (memcmp(std::to_address(m_start + begin), rest, length) == 0)) {
      return kind;
    }
    return Token::IDENTIFIER;
  }

  Token::Kind idOrKeyword() {
    // #NOTE peek at the char that brught us here.
    // if it starts a keyword, check if the identifier
    // is the keyword. if none of the initial letters
    // prefix a keyword we fallthrough the switch.
    switch (m_start[0]) {
    case 'a':
      return checkKeyword(1, 2, "nd", Token::AND);
    case 'c':
      return checkKeyword(1, 4, "lass", Token::CLASS);
    case 'e':
      return checkKeyword(1, 3, "lse", Token::ELSE);
    case 'f':
      // #NOTE 'f' can prefix three kewords, and we can distinguish
      // between them by switching on the second letter of each.
      if (std::distance(m_start, m_cursor) > 1) {
        switch (m_start[1]) {
        case 'a':
          return checkKeyword(2, 3, "lse", Token::FALSE);
        case 'o':
          return checkKeyword(2, 1, "r", Token::FOR);
        case 'u':
          return checkKeyword(2, 1, "n", Token::FUN);
        }
      }
      break;
    case 'i':
      return checkKeyword(1, 1, "f", Token::IF);
    case 'n':
      return checkKeyword(1, 2, "il", Token::NIL);
    case 'o':
      return checkKeyword(1, 1, "r", Token::OR);
    case 'p':
      return checkKeyword(1, 4, "rint", Token::PRINT);
    case 'r':
      return checkKeyword(1, 5, "eturn", Token::RETURN);
    case 's':
      return checkKeyword(1, 4, "uper", Token::SUPER);
    case 't':
      if (std::distance(m_start, m_cursor) > 1) {
        switch (m_start[1]) {
        case 'h':
          return checkKeyword(2, 2, "is", Token::THIS);
        case 'r':
          return checkKeyword(2, 2, "ue", Token::TRUE);
        }
      }
      break;
    case 'v':
      return checkKeyword(1, 2, "ar", Token::VAR);
    case 'w':
      return checkKeyword(1, 4, "hile", Token::WHILE);
    }

    return Token::IDENTIFIER;
  }

  Token identifier() noexcept {
    while (isID(peek()) || isDigit(peek())) {
      next();
    }

    return make(idOrKeyword());
  }

public:
  Scanner() noexcept : m_line(1) {}

  void reset() noexcept {
    m_start = m_cursor = iterator{};
    m_line             = 1;
  }

  void set(std::string_view text) noexcept {
    m_start = m_cursor = text.begin();
  }

  bool atEnd() const noexcept { return *m_cursor == '\0'; }

  size_t line() const noexcept { return m_line; }

  Token scan() noexcept {
    skipWhitespace();
    m_start = m_cursor;

    if (atEnd()) {
      return make(Token::END);
    }

    char c = next();
    switch (c) {
    case '(':
      return make(Token::LEFT_PAREN);
    case ')':
      return make(Token::RIGHT_PAREN);
    case '{':
      return make(Token::LEFT_BRACE);
    case '}':
      return make(Token::RIGHT_BRACE);
    case ';':
      return make(Token::SEMICOLON);
    case ',':
      return make(Token::COMMA);
    case '.':
      return make(Token::DOT);
    case '+':
      return make(Token::PLUS);
    case '-':
      return make(Token::MINUS);
    case '/':
      return make(Token::SLASH);
    case '*':
      return make(Token::STAR);

    case '!':
      return make(match('=') ? Token::BANG_EQUAL : Token::BANG);
    case '=':
      return make(match('=') ? Token::EQUAL_EQUAL : Token::EQUAL);
    case '<':
      return make(match('=') ? Token::LESS_EQUAL : Token::LESS);
    case '>':
      return make(match('=') ? Token::GREATER_EQUAL : Token::EQUAL);

    case '"':
      return string();

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return number();

    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      return identifier();
    }

    return error("Unexpected character.");
  }
};
} // namespace voyage
