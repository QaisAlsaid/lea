#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>

//TODO: use fmt ASAP

namespace my_ns
{
  parser::parser(lexer* l)
    : m_lexer(l)
  {
    next_token();
    next_token();

    const auto prefix_fun = [this]() -> auto { return parse_prefix(); };
    const auto infix_fun  = [this](const auto& expr) -> auto { return parse_infix(expr); };
    const auto boolean_fun = [this]() -> auto { return parse_boolean(); };

    //prefix
    m_prefix_funs[token_type::identifire] = [this]() -> auto { return parse_identifire(); };
    m_prefix_funs[token_type::integer]    = [this]() -> auto { return parse_integer_literal(); };
    m_prefix_funs[token_type::string]     = [this]() -> auto { return parse_string_literal(); };
    m_prefix_funs[token_type::l_paren]    = [this]() -> auto { return parse_grouped(); };
    m_prefix_funs[token_type::_if]        = [this]() -> auto { return parse_if(); };
    m_prefix_funs[token_type::fun]        = [this]() -> auto { return parse_fun_literal(); };
    m_prefix_funs[token_type::l_bracket]  = [this]() -> auto { return parse_open_bracket(); };
    m_prefix_funs[token_type::l_brace]    = [this]() -> auto { return parse_open_brace(); };

    m_prefix_funs[token_type::bang]       = prefix_fun;
    m_prefix_funs[token_type::minus]      = prefix_fun;
    m_prefix_funs[token_type::_true]      = boolean_fun;
    m_prefix_funs[token_type::_false]     = boolean_fun;

    //infix
    m_infix_funs[token_type::equal]     = infix_fun;
    m_infix_funs[token_type::not_equal] = infix_fun;
    m_infix_funs[token_type::less]      = infix_fun;
    m_infix_funs[token_type::greater]   = infix_fun;
    m_infix_funs[token_type::plus]      = infix_fun;
    m_infix_funs[token_type::minus]     = infix_fun;
    m_infix_funs[token_type::astrisk]   = infix_fun;
    m_infix_funs[token_type::slash]     = infix_fun;
    m_infix_funs[token_type::l_paren]   = [this](const auto& expr) -> auto { return parse_call(expr); };
    m_infix_funs[token_type::l_bracket] = [this](const auto& expr) -> auto { return parse_index(expr); };
  }

  void parser::next_token()
  {
    m_current_token = m_peek_token;
    m_peek_token = m_lexer->next_token();
  }

  std::shared_ptr<program> parser::parse_program()
  {
    auto prog = std::make_shared<program>();
    while(m_current_token.type != token_type::eof)
    {
      //auto stmt = parse_statement();
      if(auto stmt = parse_statement())
      {
        prog->m_statements.push_back(std::move(stmt));
      }
      next_token();
    }
    return prog;
  }

  std::shared_ptr<statement> parser::parse_statement()
  {
    switch(m_current_token.type)
    {
      case token_type::var: return parse_var_statement();
      case token_type::ret: return parse_ret_statement();
      default:              return parse_expression_statement();
    }
  }

  std::shared_ptr<var> parser::parse_var_statement()
  {
    auto var_stmt =  std::make_shared<var>(m_current_token);

    if(!expect_next(token_type::identifire))
      return nullptr;

    var_stmt->name = { m_current_token, m_current_token.literal }; //why?

    if(!expect_next(token_type::assign))
      return nullptr;

    next_token();
    var_stmt->value = parse_expression(precedence::lowest);

    if(m_peek_token.type == token_type::semicolon) //optional semicolon
      next_token();

    return var_stmt;
  }

  std::shared_ptr<ret> parser::parse_ret_statement()
  {
    auto ret_stmt = std::make_shared<ret>(m_current_token);

    next_token();
    ret_stmt->return_value = parse_expression(precedence::lowest);

    if(m_peek_token.type == token_type::semicolon) //optional semicolon
      next_token();

    return ret_stmt;
  }

  std::shared_ptr<expression_statement> parser::parse_expression_statement()
  {
    auto expr_stmt = std::make_shared<expression_statement>(m_current_token);
    expr_stmt->_expression = parse_expression(precedence::lowest);

    if(m_peek_token.type == token_type::semicolon)
      next_token();

    return expr_stmt;
  }

  std::shared_ptr<expression> parser::parse_expression(precedence p)
  {
    const auto& it = m_prefix_funs.find(m_current_token.type);
    if(it == m_prefix_funs.end())
    {
      no_prefix_parse_fun_error(m_current_token.type);
      return nullptr;
    }

    const auto& prefix = it->second;
    auto left_expr = prefix();

    while(m_peek_token.type != token_type::semicolon && (p < get_precedence(m_peek_token.type)))
    {
      const auto& in_it = m_infix_funs.find(m_peek_token.type);
      if(in_it == m_infix_funs.end())
        return left_expr;

      next_token();
      left_expr = in_it->second(left_expr);
    }
    return left_expr;
  }

  std::shared_ptr<identifire> parser::parse_identifire()
  {
    return std::make_shared<identifire>(m_current_token, m_current_token.literal);
  }

  bool parser::expect_next(token_type tok)
  {
    if(m_peek_token.type == tok)
    {
      next_token();
      return true;
    }
    peek_error(tok); //LGTM!
    return false;
  }

  std::shared_ptr<integer_literal> parser::parse_integer_literal()
  {
    auto int_lit = std::make_shared<integer_literal>(m_current_token);

    auto _int = std::stol(m_current_token.literal); //TODO: error handleing
    int_lit->value = _int;
    return int_lit;
  }

  std::shared_ptr<string_literal> parser::parse_string_literal()
  {
    return std::make_shared<string_literal>(m_current_token, m_current_token.literal);
  }

  std::shared_ptr<boolean_literal> parser::parse_boolean()
  {
    return std::make_shared<boolean_literal>(m_current_token, m_current_token.type == token_type::_true);
  }

  std::shared_ptr<prefix> parser::parse_prefix()
  {
    auto _prefix = std::make_shared<prefix>(m_current_token, m_current_token.literal);
    next_token();
    _prefix->right = parse_expression(precedence::prefix);
    return _prefix;
  }

  std::shared_ptr<infix> parser::parse_infix(const std::shared_ptr<expression>& expr)
  {
    auto _expr = std::make_shared<infix>(m_current_token, m_current_token.literal, expr);
    auto preced = get_precedence(m_current_token.type);
    next_token();
    _expr->right = parse_expression(preced);
    return _expr;
  }

  std::shared_ptr<expression> parser::parse_grouped()
  {
    next_token();
    auto exp = parse_expression(precedence::lowest);
    if(!expect_next(token_type::r_paren))
      return nullptr;
    return exp;
  }

  std::shared_ptr<block> parser::parse_block()
  {
    auto block_stmt = std::make_shared<block>(m_current_token);
    next_token();
    while(m_current_token.type != token_type::r_brace && m_current_token.type != token_type::eof)
    {
      auto stmt = parse_statement();
      if(stmt)
        block_stmt->statements.push_back(stmt);
      next_token();
    }
    return block_stmt;
  }

  std::shared_ptr<_if> parser::parse_if()
  {
    auto if_expr = std::make_shared<_if>(m_current_token);
    if(!expect_next(token_type::l_paren))
      return nullptr;

    next_token();
    if_expr->condition = parse_expression(precedence::lowest);

    if(!expect_next(token_type::r_paren))
      return nullptr;
    if(!expect_next(token_type::l_brace))
      return nullptr;

    if_expr->consequence = parse_block();

    if(m_peek_token.type == token_type::_else)
    {
      next_token();
      if(!expect_next(token_type::l_brace)) //support for non block blocks // i mean with no {} for one liner blocks like c/c++ // ithink it's easy but brings alot of trouble if the block is messing
        return nullptr;

      if_expr->alternative = parse_block();
    }
    return if_expr;
  }

  std::shared_ptr<fun_literal> parser::parse_fun_literal()
  {
    auto fun = std::make_shared<fun_literal>(m_current_token);
    if(!expect_next(token_type::l_paren))
      return nullptr;

    fun->parameters = parse_fun_params();
    if(!expect_next(token_type::l_brace)) //support for non block blocks
      return nullptr;

    fun->body = parse_block();
    return fun;
  }

  std::vector<std::shared_ptr<identifire>> parser::parse_fun_params()
  {
    std::vector<std::shared_ptr<identifire>> ids;

    if(m_peek_token.type == token_type::r_paren) //no params
    {
      next_token();
      return ids;
    }

    next_token();
    ids.emplace_back(std::make_shared<identifire>(m_current_token, m_current_token.literal));

    while(m_peek_token.type == token_type::comma)
    {
      next_token();
      next_token();
      ids.emplace_back(std::make_shared<identifire>(m_current_token, m_current_token.literal));
    }

    if(!expect_next(token_type::r_paren))
      return {};

    return ids;
  }

  std::shared_ptr<call> parser::parse_call(const std::shared_ptr<expression>& expr)
  {
    auto call_expr = std::make_shared<call>(m_current_token, expr);
    //call_expr->arguments = parse_call_args();
    call_expr->arguments = parse_expression_list(token_type::r_paren);
    return call_expr;
  }

  //TODO: deprecate
  std::vector<std::shared_ptr<expression>> parser::parse_call_args()
  {
    std::vector<std::shared_ptr<expression>> args;
    if(m_peek_token.type == token_type::r_paren) //no args
    {
      next_token();
      return args;
    }

    next_token();
    args.emplace_back(parse_expression(precedence::lowest));

    while(m_peek_token.type == token_type::comma)
    {
      next_token();
      next_token();
      args.emplace_back(parse_expression(precedence::lowest));
    }

    if(!expect_next(token_type::r_paren))
      return {};

    return args;
  }

  std::shared_ptr<expression> parser::parse_open_bracket()
  {
    return parse_array_literal();
  }

  std::shared_ptr<expression> parser::parse_array_literal()
  {
    auto array = std::make_shared<array_literal>(m_current_token);
    array->elements = parse_expression_list(token_type::r_bracket);
    return array;
  }

  std::vector<std::shared_ptr<expression>> parser::parse_expression_list(token_type expect_end)
  {
    std::vector<std::shared_ptr<expression>> list;
    if(m_peek_token.type == expect_end)
    {
      next_token();
      return list;
    }

    next_token();
    list.emplace_back(parse_expression(precedence::lowest));

    while(m_peek_token.type == token_type::comma)
    {
      next_token();
      next_token();
      list.emplace_back(parse_expression(precedence::lowest));
    }

    if(!expect_next(expect_end))
      return {};

    return list;
  }

  std::shared_ptr<expression> parser::parse_index(const std::shared_ptr<expression>& left)
  {
    auto exp = std::make_shared<index>(m_current_token, left);

    next_token();
    exp->right = parse_expression(precedence::lowest);

    if(!expect_next(token_type::r_bracket))
      return nullptr;

    return exp;
  }

  std::shared_ptr<expression> parser::parse_open_brace()
  {
    return parse_map_literal();
  }

  std::shared_ptr<expression> parser::parse_map_literal()
  {
    auto map = std::make_shared<map_literal>(m_current_token);
    
    while(m_peek_token.type != token_type::r_brace)
    {
      next_token();
      auto key = parse_expression(precedence::lowest);
      
      if(!expect_next(token_type::colon))
        return nullptr;

      next_token();
      auto value = parse_expression(precedence::lowest);
      map->pairs[key] = value;
      
      if(m_peek_token.type != token_type::r_brace && !expect_next(token_type::comma))
        return nullptr;
    }
    
    if(!expect_next(token_type::r_brace))
      return nullptr;

    return map;
  }


  void parser::peek_error(token_type tok)
  {
    //i mean WTF!!
    m_errors.emplace_back("expected next token to be: '" + std::string(token_type_to_string(tok)) + "' got: '" + std::string(token_type_to_string(m_peek_token.type)) + "'." );
  }

  void parser::no_prefix_parse_fun_error(token_type tok)
  {
    m_errors.emplace_back("no prefix parse function found for: '" + std::string(token_type_to_string(tok)) + "'.");
  }

  auto parser::get_precedence(token_type tok) -> precedence
  {
    switch(tok)
    {
      case token_type::equal:
      case token_type::not_equal:
        return precedence::equals;
      case token_type::less:
      case token_type::greater:
        return precedence::less_greater;
      case token_type::plus:
      case token_type::minus:
        return precedence::sum;
      case token_type::astrisk:
      case token_type::slash:
        return precedence::product;
      case token_type::l_paren:
        return precedence::call;
      case token_type::l_bracket:
        return precedence::index;
      default:
        return precedence::lowest;
    }
  }
}
