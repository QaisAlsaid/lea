#pragma once

#include <string>
#include "token.hpp"

//TODO: unicode support

namespace my_ns
{
  class lexer
  {
  public:
    lexer(const std::string& input);

    token next_token();
  private:
    void read_char();
    char peek_char();
    std::string read_identifier();
    std::string read_number();
    std::string read_string(char str_start);
    void consume_whitespace();
  private:
    std::string m_input;
    int m_current_position = 0;
    int m_read_position = 0;
    char m_current_char = 0;
  };
}
