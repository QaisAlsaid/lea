#pragma once
#include <string>
#include <string_view>

//TODO: use an enum

namespace my_ns
{
  enum class token_type
  {
    illegal, eof, identifire, integer, string,
    assign, plus, minus, astrisk, slash,
    comma, semicolon, colon, bang,
    equal, not_equal, less, greater,
    l_paren, r_paren, l_brace, r_brace, l_bracket, r_bracket,
    fun, var, _if, _else, _true, _false, ret
  };

  struct token
  {
    token() = default;
    token(token_type tp, char ch)
      : type(tp)
    {
      literal = { ch };
    }

    token(token_type tp, const std::string& str)
      : type(tp), literal(str)
    {
    }

    token_type type;
    std::string literal;
  };

  token_type lookup_identifier(const std::string& id);

  constexpr std::string_view token_type_to_string(const token_type t)
  {
    using namespace std::string_view_literals;
    switch(t)
    {
      case token_type::illegal:    return "illegal"sv;
      case token_type::eof:        return "eof"sv;
      case token_type::identifire: return "identifire"sv;
      case token_type::integer:    return "integer"sv;
      case token_type::string:     return "string"sv;
      case token_type::assign:     return "assign"sv;
      case token_type::plus:       return "plus"sv;
      case token_type::minus:      return "minus"sv;
      case token_type::astrisk:    return "astrisk"sv;
      case token_type::slash:      return "slash"sv;
      case token_type::comma:      return "comma"sv;
      case token_type::semicolon:  return "semicolon"sv;
      case token_type::colon:      return "colon"sv;
      case token_type::bang:       return "bang"sv;
      case token_type::equal:      return "equal"sv;
      case token_type::not_equal:  return "not_equal"sv;
      case token_type::less:       return "less"sv;
      case token_type::greater:    return "greater"sv;
      case token_type::l_paren:    return "l_paren"sv;
      case token_type::r_paren:    return "r_paren"sv;
      case token_type::l_brace:    return "l_brace"sv;
      case token_type::l_bracket:  return "r_brace"sv;
      case token_type::r_bracket:  return "r_bracket"sv;
      case token_type::r_brace:    return "r_bracket"sv;
      case token_type::fun:        return "fun"sv;
      case token_type::var:        return "var"sv;
      case token_type::_if:        return "if"sv;
      case token_type::_else:      return "else"sv;
      case token_type::_true:      return "true"sv;
      case token_type::_false:     return "false"sv;
      case token_type::ret:        return "ret"sv;
    }
    return "unknown"sv;
  }

  /*
  constexpr auto illegal = "ILLEGAL";
  constexpr auto eof = "EOF";

  // Identifiers + literals
  constexpr auto ident = "IDENT"; // add, foobar, x, y, ...
  constexpr auto _int = "INT"; // 1343456

  // Operators
  constexpr auto assign = "=";
  constexpr auto plus = "+";

  // Delimiters
  constexpr auto comma = ",";
  constexpr auto semicolon = ";";
  constexpr auto l_paren = "(";
  constexpr auto r_paren = ")";
  constexpr auto l_brace = "{";
  constexpr auto r_brace = "}"

  // Keywords
  constexpr auto function = "FUNCTION";
  constexpr auto var = "VAR";
  */
}
