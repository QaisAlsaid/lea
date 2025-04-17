#pragma once

#include "token.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace my_ns
{
  enum class node_type 
  { 
    node, statement, expression, program, identifire,
    var, ret, expression_statement, block,
    integer, boolean, string, array, index, map,
    prefix, infix, _if, fun, call
  };

  //TODO
  /*
  enum class operator_type
  {
    bang,
    assign,
    equals,
    less, 
    greater,
    astrisk,
    slash,
    plus,
    minus,
    call,
  };
  */

  class node
  {
  public:
    virtual ~node() = default;
    virtual std::string token_literal() = 0;
    virtual std::string to_string() = 0;

    inline node_type get_type()
    {
      return m_type;
    }
  protected:
    node() = delete;
    node(node_type t)
      : m_type(t)
    {
    }
  protected:
    node_type m_type = node_type::node;
  };

  class statement : public node
  {
  public:
    statement()
      : node(node_type::statement)
    {
    }

    statement(node_type t)
      : node(t)
    {
    }
  };

  class expression : public node
  {
  public:
    expression()
      : node(node_type::expression)
    {
    }

    expression(node_type t)
      : node(t)
    {
    }
  };

  class program : public node
  {
  public:
    program()
      : node(node_type::program)
    {
    }

    std::string token_literal() override
    {
      if(m_statements.empty())
        return "";
      return m_statements[0]->token_literal();
    }

    std::string to_string() override
    {
      std::stringstream ss;
      for(const auto& stmt : m_statements)
        ss << stmt->to_string();
      return ss.str();
    }
  public:
    std::vector<std::shared_ptr<statement>> m_statements;
  };

  class identifire : public expression
  {
  public:
    identifire() = default;
    identifire(token t, const std::string& val)
      : expression(node_type::identifire), _token(t), value(val)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      return value;
    }

  public:
    token _token;
    std::string value;
  };

  class var : public statement
  {
  public:
    var(token tok)
      : statement(node_type::var), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << token_literal() << " " << name.to_string() << " = ";
      if(value)
        ss << value->to_string();
      ss << ";";
      return ss.str();
    }
  public:
    token _token;
    identifire name;
    std::shared_ptr<expression> value;
  };

  class ret : public statement
  {
  public:
    ret(token tok)
      : statement(node_type::ret), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << token_literal() << " ";
      if(return_value)
        ss << return_value->to_string();
      ss << ";";
      return ss.str();
    }
  public:
    token _token;
    std::shared_ptr<expression> return_value;
  };

  class expression_statement : public statement
  {
  public:
    expression_statement(token tok)
      : statement(node_type::expression_statement), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      if(_expression)
        return _expression->to_string();
      return "";
    }
  public:
    token _token;
    std::shared_ptr<expression> _expression;
  };

  class block : public statement
  {
  public:
    block(token tok)
      : statement(node_type::block), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      for(const auto& stmt : statements)
      {
        ss << stmt->to_string() << " ";
      }
      return ss.str();
    }
  public:
    token _token;
    std::vector<std::shared_ptr<statement>> statements;
  };

  class integer_literal : public expression
  {
  public:
    integer_literal(token tok)
      : expression(node_type::integer), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      return _token.literal;
    }
  public:
    token _token;
    int64_t value;
  };

  class string_literal : public expression
  {
  public:
    string_literal(token tok, const std::string& val)
      : expression(node_type::string), _token(tok), value(val)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      return _token.literal;
    }
  public:
    token _token;
    std::string value;
  };


  class boolean_literal : public expression
  {
  public:
    boolean_literal(token tok, bool val)
      : expression(node_type::boolean), _token(tok), value(val)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      return _token.literal;
    }
  public:
    token _token;
    bool value;
  };

  class array_literal : public expression
  {
  public:
    array_literal(token tok)
      : expression(node_type::array), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << "[";
      for(const auto& elem : elements)
        ss << elem->to_string() << ", ";
      ss << "]";

      return ss.str();
    }
  public:
    token _token;
    std::vector<std::shared_ptr<expression>> elements;
  };

  class map_literal : public expression
  {
  public:
    map_literal(token tok)
      : expression(node_type::map), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << "{";
      for(const auto& pair : pairs)
        ss << pair.first->to_string() << ":" << pair.second->to_string() << ", ";
      ss << "}";

      return ss.str();
    }
  public:
    token _token;
    std::unordered_map<std::shared_ptr<expression>, std::shared_ptr<expression>> pairs;
  };

  class index : public expression 
  {
  public:
    index(token tok, const std::shared_ptr<expression>& left)
      : expression(node_type::index), _token(tok), left(left)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << "(" << left->to_string() << "[" << right->to_string() << "])"; 
      return ss.str();
    }

  public:
    token _token;
    std::shared_ptr<expression> left;
    std::shared_ptr<expression> right;
  };

  class prefix : public expression
  {
  public:
    prefix(token tok, const std::string& op)
      : expression(node_type::prefix), _token(tok), _operator(op)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << "(" << _operator << right->to_string() << ")";
      return ss.str();
    }
  public:
    token _token;
    std::string _operator;
    std::shared_ptr<expression> right;
  };

  class infix : public expression
  {
  public:
    infix(token tok, const std::string& op, const std::shared_ptr<expression> expr)
      : expression(node_type::infix), _token(tok), _operator(op), left(expr)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << "(" << left->to_string() << " " << _operator << " " << right->to_string() << ")";
     return ss.str();
    }
  public:
    token _token;
    std::string _operator;
    std::shared_ptr<expression> left;
    std::shared_ptr<expression> right;
  };

  class _if : public expression
  {
  public:
    _if(token tok)
      : expression(node_type::_if), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << "if " << condition->to_string() << " " << consequence->to_string();
      if(alternative)
        ss << "else " << alternative->to_string();

      return ss.str();
    }
  public:
    token _token;
    std::shared_ptr<expression> condition;
    std::shared_ptr<block> consequence;
    std::shared_ptr<block> alternative;
  };

  class fun_literal : public expression
  {
  public:
    fun_literal(token tok)
      : expression(node_type::fun), _token(tok)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;

      ss << token_literal() << "(";
      for(const auto& param : parameters)
        ss << param->to_string() << ", ";
      ss << ") " << body->to_string();

      return ss.str();
    }
  public:
    token _token;
    std::vector<std::shared_ptr<identifire>> parameters;
    std::shared_ptr<block> body;
  };

  class call : public expression
  {
  public:
    call(token tok, const std::shared_ptr<expression>& fun)
      : expression(node_type::call), _token(tok), function(fun)
    {
    }

    std::string token_literal() override
    {
      return _token.literal;
    }

    std::string to_string() override
    {
      std::stringstream ss;
      ss << function->to_string() << "(";
      for(auto arg : arguments)
        ss << arg->to_string() << ", ";
      ss << ")";

      return ss.str();
    }
  public:
    token _token;
    std::shared_ptr<expression> function;
    std::vector<std::shared_ptr<expression>> arguments;
  };
}
