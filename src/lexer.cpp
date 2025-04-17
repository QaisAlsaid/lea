#include "lexer.hpp"
#include "token.hpp"

namespace my_ns
{
  constexpr static bool is_literal(char ch);
  constexpr static bool is_digit(char ch);

  lexer::lexer(const std::string& input)
    : m_input(input)
  {
    read_char(); //set to valid state
  }

  token lexer::next_token()
  {
    consume_whitespace();
    token tok;
    switch(m_current_char)
    {
      case ';':
      {
        tok = { token_type::semicolon, m_current_char };
        break;
      }
      case ':':
      {
        tok = { token_type::colon, m_current_char };
        break;
      }
      case ',':
      {
        tok = { token_type::comma, m_current_char };
        break;
      }
      case '(':
      {
        tok = { token_type::l_paren, m_current_char };
        break;
      }
      case ')':
      {
        tok = { token_type::r_paren, m_current_char };
        break;
      }
      case '{':
      {
        tok = { token_type::l_brace, m_current_char };
        break;
      }
      case '}':
      {
        tok = { token_type::r_brace, m_current_char };
        break;
      }
      case '[':
      {
        tok = { token_type::l_bracket, m_current_char };
        break;
      }
      case ']':
      {
        tok = { token_type::r_bracket, m_current_char };
        break;
      }
      case '=':
      {
        if(peek_char() == '=')
        {
          auto ch = m_current_char;
          read_char();
          tok = { token_type::equal, std::string{ch} + std::string{m_current_char} }; //FIXME: stupid std::string!
        }
        else
        {
          tok = { token_type::assign, m_current_char };
        }
        break;
      }
      case '!':
      {
        if(peek_char() == '=')
        {
          auto ch = m_current_char;
          read_char();
          tok = { token_type::not_equal, std::string{ch} + std::string{m_current_char} }; //FIXME: stupid std::string!
        }
        else
        {
          tok = { token_type::bang, m_current_char };
        }
        break;
      }
      case '+':
      {
        tok = { token_type::plus, m_current_char };
        break;
      }
      case '-':
      {
        tok = { token_type::minus, m_current_char };
        break;
      }
      case '*':
      {
        tok = { token_type::astrisk, m_current_char };
        break;
      }
      case '/':
      {
        tok = { token_type::slash, m_current_char };
        break;
      }
      case '<':
      {
        tok = { token_type::less, m_current_char };
        break;
      }
      case '>':
      {
        tok = { token_type::greater, m_current_char };
        break;
      }
      case '\'':
      {
        tok.type = token_type::string ;
        tok.literal = read_string('\'');
        break;
      }
      case '"':
      {
        tok.type = token_type::string ;
        tok.literal = read_string('"');
        break;
      }
      case 0:
      {
        tok = { token_type::eof, "" };
        break;
      }
      default:
      {
        if(is_literal(m_current_char))
        {
          tok.literal = read_identifier();
          tok.type = lookup_identifier(tok.literal);
          //return here because read_identifire calls read_char multiple times
          return tok;
        }
        else if(is_digit(m_current_char))
        {
          tok.literal = read_number();
          tok.type = token_type::integer;
          return tok;
        }
        else
        {
          tok = { token_type::illegal, m_current_char };
        }
        break;
      }
    }
    //advance
    read_char();
    return tok;
  }

  void lexer::read_char()
  {
    if(m_read_position >= m_input.size())
      m_current_char = 0; //null either eof or haven't started yet
    else [[likely]]
      m_current_char = m_input[m_read_position];

    m_current_position = m_read_position++;
  }

  char lexer::peek_char()
  {
    if(m_read_position >= m_input.size())
      return 0;
    return m_input[m_read_position];
  }

  std::string lexer::read_identifier()
  {
    auto pos = m_current_position;
    while(is_literal(m_current_char) || is_digit(m_current_char))
    {
      read_char();
    }
    return m_input.substr(pos, m_current_position - pos);
  }

  std::string lexer::read_number()
  {
    auto pos = m_current_position;
    while(is_digit(m_current_char))
    {
      read_char();
    }
    return m_input.substr(pos, m_current_position - pos);
  }

  std::string lexer::read_string(char start)
  {
    auto pos = m_current_position + 1;
    while(true)
    {
      read_char();
      //TODO: escape seq and error if not closed
      if(m_current_char == start)
        break;
      else if(m_current_char == 0)
        break; //error 
    }
    return m_input.substr(pos, m_current_position - pos);
  }

  void lexer::consume_whitespace()
  {
    while(std::isspace(m_current_char))
    {
      read_char();
    }
  }

  constexpr static bool is_literal(char ch)
  {
    return std::isalpha(ch) || ch == '_';
  }

  constexpr static bool is_digit(char ch)
  {
    return std::isdigit(ch);
  }
}
