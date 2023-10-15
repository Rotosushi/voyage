#pragma once
#include <string_view>

namespace voyage {
struct Token {
  enum Kind {
    ERROR,
    END,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    SEMICOLON,
    MINUS,
    PLUS,
    SLASH,
    STAR,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFIER,
    STRING,
    NUMBER,

    AND,
    CLASS,
    ELSE,
    FALSE,
    FOR,
    FUN,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
  };

  Kind             kind;
  std::string_view text;
  size_t           line;

  Token() noexcept : kind(ERROR), line(0) {}
  Token(Kind kind) noexcept : kind(kind), line(0) {}
  Token(Kind kind, std::string_view text, size_t line) noexcept
      : kind(kind), text(text), line(line) {}
};
} // namespace voyage
