#include "token.hpp"
#include <unordered_map>
#include <string_view>

namespace my_ns
{
  using namespace std::string_view_literals;

  static std::unordered_map<std::string_view, token_type> s_keywords = {
    { "fun"sv, token_type::fun },
    { "var"sv, token_type::var },
    { "if"sv, token_type::_if },
    { "else"sv, token_type::_else },
    { "true"sv, token_type::_true },
    { "false"sv, token_type::_false },
    { "ret"sv, token_type::ret }
  };

  token_type lookup_identifier(const std::string& id)
  {
    auto it = s_keywords.find(id);
    return it != s_keywords.end() ? it->second : token_type::identifire;
  }
}
