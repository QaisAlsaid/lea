#pragma once

#include "lexer.hpp"
#include "ast.hpp"
#include "token.hpp"
#include <cmath>
#include <functional>
#include <memory>
#include <unordered_map>

//TODO: solve the shared ptr problem i don't think every thing have to be shared!

namespace my_ns
{
  class parser
  {
  public:
    using errors = std::vector<std::string>;
    using prefix_parse_fun = std::function<std::shared_ptr<expression>(void)>;
    using infix_parse_fun = std::function<std::shared_ptr<expression>(const std::shared_ptr<expression>& expr)>;

    enum class precedence
    {
      lowest = 0,
      equals, // ==
      less_greater, // > or <
      sum, // + or -
      product, // * or /
      prefix, // -X or !X
      call, // fun(x)
      index // arr[idx]
    };
  public:
    parser(lexer* l);
    std::shared_ptr<program> parse_program();
    inline const errors& get_errors() const
    {
      return m_errors;
    }
  private:
    void next_token();

    bool expect_next(token_type tok);

    std::shared_ptr<statement> parse_statement();
    std::shared_ptr<var> parse_var_statement();
    std::shared_ptr<ret> parse_ret_statement();

    std::shared_ptr<expression_statement> parse_expression_statement();

    std::shared_ptr<expression> parse_expression(precedence p);
    std::shared_ptr<identifire> parse_identifire();
    std::shared_ptr<integer_literal> parse_integer_literal();
    std::shared_ptr<string_literal> parse_string_literal();
    std::shared_ptr<boolean_literal> parse_boolean();
    std::shared_ptr<prefix> parse_prefix();
    std::shared_ptr<infix> parse_infix(const std::shared_ptr<expression>& expr);
    std::shared_ptr<expression> parse_grouped();
    std::shared_ptr<block> parse_block();
    std::shared_ptr<_if> parse_if();
    std::shared_ptr<fun_literal> parse_fun_literal();
    std::vector<std::shared_ptr<identifire>> parse_fun_params();
    std::shared_ptr<call> parse_call(const std::shared_ptr<expression>& expr);
    std::vector<std::shared_ptr<expression>> parse_call_args();

    std::shared_ptr<expression> parse_open_bracket();
    std::shared_ptr<expression> parse_array_literal();
    std::vector<std::shared_ptr<expression>> parse_expression_list(token_type expect_end);
    std::shared_ptr<expression> parse_index(const std::shared_ptr<expression>&);
    std::shared_ptr<expression> parse_open_brace();
    std::shared_ptr<expression> parse_map_literal();

    void peek_error(token_type tok);
    void no_prefix_parse_fun_error(token_type tok);
  private:
    static precedence get_precedence(token_type tok);
  private:
    lexer* m_lexer;
    token m_current_token;
    token m_peek_token;
    std::unordered_map<token_type, prefix_parse_fun> m_prefix_funs;
    std::unordered_map<token_type, infix_parse_fun> m_infix_funs;
    errors m_errors;
  };
}
