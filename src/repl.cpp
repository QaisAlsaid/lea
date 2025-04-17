#include "lexer.hpp"
#include "object.hpp"
#include "parser.hpp"
#include "token.hpp"
#include "evaluator.hpp"

#include <iostream>
#include <ostream>
#include <string_view>

using namespace std::string_view_literals;

namespace std
{
  std::ostream& operator << (std::ostream& os, const my_ns::token& tok)
  {
    os << "token: " << "type: '" << my_ns::token_type_to_string(tok.type) << "' literal: '" << tok.literal << "'\n";
    return os;
  }
}

namespace my_ns
{
  constexpr auto prompt = ">> "sv;

  static void print_parse_errors(const parser::errors& errors);

  void start_repl()
  {
    bool running = true;
    auto env = std::make_shared<environment>();
    while(running)
    {
      std::cout << prompt;
      std::string line;
      running = static_cast<bool>(std::getline(std::cin, line));
      if(!running) break;

      lexer lx(line);
      parser ps(&lx);
      
      auto program = ps.parse_program();
      
      const auto& errors = ps.get_errors();
      if(errors.size())
      {
        print_parse_errors(errors);
        continue;
      }

      auto evaluated = eval(program, env);
      if(evaluated)
        std::cout << evaluated->inspect() << "\n";
    }
  }

  void print_parse_errors(const parser::errors& errors)
  {
    for(const auto& msg : errors)
      std::cerr << "parse error: " << msg << "\n";
  }
}
