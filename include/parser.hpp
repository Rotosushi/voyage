#pragma once
#include <charconv>
#include <format>
#include <iostream>
#include <optional>
#include <sstream>

#include "bytecode.hpp"
#include "error.hpp"
#include "scanner.hpp"

namespace voyage {

class Parser {
public:
  enum Precedence {
    NONE,
    ASSIGNMENT, // =
    OR,         // or
    AND,        // and
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * /
    UNARY,      // ! -
    CALL,       // . ()
    PRIMARY,
  };

  using ParseFn = void (Parser::*)(Bytecode &bc);

  struct ParseRule {
    ParseFn    prefix;
    ParseFn    infix;
    Precedence precedence;
  };

private:
  bool    had_error;
  bool    panic_mode;
  Scanner scanner;
  Token   current;
  Token   previous;

  void errorAt(Token &token, std::string_view msg) {
    if (panic_mode) {
      return; // suppress errors until we reach syncronization
    }

    panic_mode = true;
    had_error  = true;
    std::cerr << std::format("[line {:d}] Error", token.line);

    if (token.kind == Token::END) {
      std::cerr << " at end";
    } else if (token.kind == Token::ERROR) {
      // nothing
    } else {
      std::cerr << std::format(" at '{:s}'", token.text);
    }

    std::cerr << std::format(": {:s}\n", msg);
  }

  void errorAtCurrent(std::string_view msg) { errorAt(current, msg); }
  void error(std::string_view msg) { errorAt(previous, msg); }
  void error(std::errc ec) {
    errorAt(previous, std::make_error_code(ec).message());
  }

  void next() noexcept {
    previous = current;

    while (true) {
      current = scanner.scan();
      if (current.kind != Token::ERROR) {
        break;
      }

      errorAtCurrent(current.text);
    }
  }

  void expect(Token::Kind kind, std::string_view msg) {
    if (current.kind == kind) {
      next();
      return;
    }

    errorAtCurrent(msg);
  }

  ParseRule *getRule(Token::Kind kind);
  void       expression(Bytecode &bc);
  void       parsePrecedence(Bytecode &bc, Precedence precedence);

  void number(Bytecode &bc) {
    double value = 0.0;
    auto [ptr, ec] =
        std::from_chars(previous.text.begin(), previous.text.end(), value);
    if (ec != std::errc{}) {
      error(ec);
    }
    bc.emitConstant({value}, previous.line);
  }

  void grouping(Bytecode &bc) {
    expression(bc);
    expect(Token::RIGHT_PAREN, "Expect ')' after expression.");
  }

  void unary(Bytecode &bc) {
    Token::Kind op = previous.kind;

    parsePrecedence(bc, Precedence::UNARY);

    switch (op) {
    case Token::MINUS: {
      bc.emitNegate(previous.line);
      break;
    }

    default: {
      std::unreachable();
    }
    }
  }

  void binary(Bytecode &bc) {
    Token::Kind operator_kind = previous.kind;
    ParseRule  *rule          = getRule(operator_kind);
    parsePrecedence(bc, (Precedence)(rule->precedence + 1));

    switch (operator_kind) {
    case Token::PLUS:
      bc.emitAdd(previous.line);
      break;

    case Token::MINUS:
      bc.emitSub(previous.line);
      break;

    case Token::STAR:
      bc.emitMul(previous.line);
      break;

    case Token::SLASH:
      bc.emitDiv(previous.line);
      break;

    default: {
      std::unreachable();
    }
    }
  }

public:
  Parser() noexcept : had_error(false), panic_mode(false) {}

  std::optional<Bytecode> parse(std::string_view text) {
    scanner.set(text);

    Bytecode bc;
    next();
    expression(bc);

    if (had_error) {
      return std::nullopt;
    } else {
      bc.emitReturn(previous.line);

      if constexpr (debug_print) {
        print(std::cout, bc);
      }

      return bc;
    }
  }
};

void Parser::expression(Bytecode &bc) {
  parsePrecedence(bc, Precedence::ASSIGNMENT);
}

void Parser::parsePrecedence(Bytecode &bc, Precedence precedence) {
  next();
  ParseFn prefix = getRule(previous.kind)->prefix;
  if (prefix == nullptr) {
    error("Expect expression.");
    return;
  }

  (this->*prefix)(bc);

  while (precedence <= getRule(current.kind)->precedence) {
    next();
    ParseFn infix = getRule(previous.kind)->infix;
    (this->*infix)(bc);
  }
}

Parser::ParseRule *Parser::getRule(Token::Kind kind) {
  static ParseRule rules[] = {
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },

      {&Parser::grouping, nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },

      {&Parser::unary,    &Parser::binary, Precedence::TERM  },
      {nullptr,           &Parser::binary, Precedence::TERM  },
      {nullptr,           &Parser::binary, Precedence::FACTOR},
      {nullptr,           &Parser::binary, Precedence::FACTOR},

      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },

      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {&Parser::number,   nullptr,         Precedence::NONE  },

      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
      {nullptr,           nullptr,         Precedence::NONE  },
  };

  return &rules[kind];
}

} // namespace voyage
