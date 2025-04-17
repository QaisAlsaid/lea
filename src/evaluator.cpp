#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include <cstdio>
#include <iostream>
#include <memory>

namespace my_ns
{
  static std::shared_ptr<boolean> get_true();
  static std::shared_ptr<boolean> get_false();
  static std::shared_ptr<null> get_null();
  static std::shared_ptr<boolean> to_boolean(bool b);
  static std::shared_ptr<boolean> to_boolean(const std::shared_ptr<object>& obj);

  static bool is_error(const std::shared_ptr<object>& obj);
 
  //TODO: libraries
  static environment builtin_env{
    { "str_len", std::make_shared<builtin>([](const std::vector<std::shared_ptr<object>>& args) -> std::shared_ptr<object>
      {
        if(args.size() != 1)
          return add_error("str_len: expected: 1 argument, got: " + std::to_string(args.size()));
        else 
        {
          const auto& arg = args[0];
          switch(arg->get_type())
          {
            case object_type::string:
            {
              auto str = std::static_pointer_cast<string>(arg);
              return std::make_shared<integer>(str->get_value().size());
            }
            default:
            {
              return add_error("str_len: expects argument to be of type: 'string', got: " + std::to_string((uint32_t)arg->get_type()));
            }
          }
        }
      })
    },
    { "len", std::make_shared<builtin>([](const std::vector<std::shared_ptr<object>>& args) -> std::shared_ptr<object>
    {
      if(args.size() != 1)
        return add_error("len: expected: 1 argument, got: " + std::to_string(args.size()));
      else
      {
        const auto& arg = args[0];
        switch(arg->get_type())
        {
          case object_type::array:
          {
            auto arr = std::static_pointer_cast<array>(arg);
            return std::make_shared<integer>(arr->get_elements().size());
          }
          default:
          {
            return add_error("len: expects argument to be of type: 'array', got: " + std::to_string((uint32_t)arg->get_type()));
          }
        }
      }
      })
    },
    { "push", std::make_shared<builtin>([](const std::vector<std::shared_ptr<object>>& args) -> std::shared_ptr<object>
      {
        auto sz = args.size();
        if(sz > 3 || sz < 2)
          return add_error("push: expected at least: 2 arguments, got: " + std::to_string(args.size()));
        else
        {
          if(args[0]->get_type() != object_type::array) 
            return add_error("push: expects argument 0 to be of type: 'array', got: " + std::to_string((uint32_t)args[0]->get_type()));
 
          const auto& arr = std::static_pointer_cast<array>(args[0]);

          size_t pos = arr->get_elements().size();
          if(sz == 3)
          {
            if(args[2]->get_type() != object_type::integer)  
              return add_error("push: expects argument 2 to be of type: 'integer', got: " + std::to_string((uint32_t)args[3]->get_type()));
            auto pos_obj = std::static_pointer_cast<integer>(args[2]);
            pos = pos_obj->get_value();
            if(pos > arr->get_elements().size() - 1 || pos < 0)
              return get_null(); //maybe error
          }


          std::vector<std::shared_ptr<object>> vec;
          vec.reserve(arr->get_elements().size() + 1);
          for(const auto& elem : arr->get_elements())
            vec.push_back(elem);
          auto insert_pos = vec.begin() + pos;
          vec.emplace(insert_pos, args[1]);
          auto new_arr = std::make_shared<array>(std::move(vec));

          return new_arr;
        }
      })
    },
    { "puts", std::make_shared<builtin>([](const std::vector<std::shared_ptr<object>>& args) -> std::shared_ptr<object>
      {
        if(args.size() != 1)
          return add_error("puts: expected: 1 argument, got: " + std::to_string(args.size()));
        else 
        {
          if(args[0]->get_type() != object_type::string)
            return add_error("puts: expects: argument of type 'string', got: " + std::to_string((uint32_t)args[0]->get_type()));

          auto str = std::static_pointer_cast<string>(args[0]);
          std::puts(str->get_value().c_str());
          
          return get_null();
        }
      }) 
    },
  { "to_string", std::make_shared<builtin>([](const std::vector<std::shared_ptr<object>>& args) -> std::shared_ptr<object>
      {
        if(args.size() != 1)
          return add_error("to_string: expected: 1 argument, got: " + std::to_string(args.size()));
        else 
          return std::make_shared<string>(args[0]->inspect());  
      }) 
    },
  };

  trampoline_result eval_trampoline(const std::shared_ptr<node>& n, const std::shared_ptr<environment>& env, size_t depth) 
  {
    if(depth > 7000) return add_error("recursion depth exceeded");
    switch(n->get_type())
    {
      case node_type::program:
      {
        auto program_node = std::static_pointer_cast<program>(n);
        return eval_program(program_node, env);
      }
      case node_type::expression_statement:
      {
        auto exp_stmt_node = std::static_pointer_cast<expression_statement>(n);
        return eval(exp_stmt_node->_expression, env);
      }
      case node_type::integer:
      {
        auto int_node = std::static_pointer_cast<integer_literal>(n);
        return std::make_shared<integer>(int_node->value);
      }
      case node_type::string:
      {
        auto string_node = std::static_pointer_cast<string_literal>(n);
        return std::make_shared<string>(string_node->value);
      }
      case node_type::array:
      {
        auto arr_node = std::static_pointer_cast<array_literal>(n);
        auto elements = eval_expressions(arr_node->elements, env);

        if(elements.size() == 1 && is_error(elements[0]))
          return elements[0];

        return std::make_shared<array>(elements);
      }
      case node_type::map:
      {
        auto map_node = std::static_pointer_cast<map_literal>(n);
        return eval_map(map_node, env);
      }
      case node_type::boolean:
      {
        auto bool_node = std::static_pointer_cast<boolean_literal>(n);
        return to_boolean(bool_node->value);
      }
      case node_type::prefix:
      {
        auto prefix_node = std::static_pointer_cast<prefix>(n);
        auto right_eval = eval(prefix_node->right, env);
        
        if(is_error(right_eval))
          return right_eval;

        return eval_prefix_expression(prefix_node->_operator, right_eval);
      }
      case node_type::infix:
      {
        auto infix_node = std::static_pointer_cast<infix>(n);
        auto left_eval = eval(infix_node->left, env);
        
        if(is_error(left_eval))
          return left_eval;

        auto right_eval = eval(infix_node->right, env);
        
        if(is_error(right_eval))
          return right_eval;

        return eval_infix_expression(infix_node->_operator, left_eval, right_eval);
      }
      case node_type::index:
      {
        auto index_node = std::static_pointer_cast<index>(n);
        
        auto left = eval(index_node->left, env);
    
        if(is_error(left))
          return left;
        
        auto right = eval(index_node->right, env);

        if(is_error(right))
          return right;

        return eval_index_expression(left, right);
      }
      case node_type::block:
      {
        auto block_node = std::static_pointer_cast<block>(n);
        return eval_block_statement(block_node, env);
      }
      case node_type::_if:
      {
        auto if_node = std::static_pointer_cast<_if>(n);
        return eval_if_expression(if_node, env);
      }
      case node_type::ret:
      {
        auto ret_node = std::static_pointer_cast<ret>(n);
        auto val = eval(ret_node->return_value, env);
        
        if(is_error(val))
          return val;

        return val;
      }
      case node_type::var:
      {
        auto var_node = std::static_pointer_cast<var>(n);
        auto val = eval(var_node->value, env);
        
        if(is_error(val))
          return val;

        env->set(var_node->name.value, val);
        break;
      }
      case node_type::identifire:
      {
        auto ident_node = std::static_pointer_cast<identifire>(n);
        return eval_identifire(ident_node, env);
      }
      case node_type::fun:
      {
        auto fun_node = std::static_pointer_cast<fun_literal>(n);
        return std::make_shared<fun>(fun_node->parameters, fun_node->body, env);
      }
      case node_type::call:
      {
        auto call_node = std::static_pointer_cast<call>(n);
        auto function = eval(call_node->function, env);

        if(is_error(function))
          return function;
        
        auto args = eval_expressions(call_node->arguments, env);
        if(args.size() == 1 && is_error(args[0]))
          return args[0];

        return invoke_function(function, args);
      }
    }
    return std::make_shared<void_object>();

        if(n->get_type() == node_type::call){
            // instead of calling eval directly, return a lambda that will call it
            return [n, env](){
                auto call_node = std::static_pointer_cast<call>(n);
                auto function = eval(call_node->function, env);
                if(is_error(function)) return function;
                auto args = eval_expressions(call_node->arguments, env);
                if(args.size() == 1 && is_error(args[0])) return args[0];
                return invoke_function(function, args);
            };
        }
        return eval(n, env);
    }

  std::shared_ptr<object> eval(const std::shared_ptr<node>& n, const std::shared_ptr<environment>& env, size_t depth)
  {
    auto result = eval_trampoline(n, env, depth++);
    while(std::holds_alternative<std::function<std::shared_ptr<object>()>>(result))
    {
      auto next_call = std::get<std::function<std::shared_ptr<object>()>>(result);
      result = next_call();
    }
    return std::get<std::shared_ptr<object>>(result);
  }

  std::shared_ptr<object> eval_program(const std::shared_ptr<program>& prog, const std::shared_ptr<environment>& env) 
  {
    std::shared_ptr<object> res;
    for(const auto& stmt: prog->m_statements)
    {
      res = eval(stmt, env);
      
      //TODO: look into the program return statement!
      if(res)
      {
        if(res->get_type() == object_type::ret_value)
        {
          //unwrap
          auto ret_val = std::static_pointer_cast<ret_value>(res);
          return ret_val->get_value(); //the actual return value not the object 
        }
        else if(res->get_type() == object_type::error)
        {
          return res;
        }
      }
    }
    return res;
  }

  std::shared_ptr<object> eval_block_statement(const std::shared_ptr<block>& block_stmt, const std::shared_ptr<environment>& env)
  {
    std::shared_ptr<object> res;
    for(const auto& stmt : block_stmt->statements)
    {
      res = eval(stmt, env);
      //we never unwrap here 
      //and for now the unwrapping is execlusive to eval_program
      //TODO: unwrap in function call
      if(res)
      {
        if(res->get_type() == object_type::ret_value || res->get_type() == object_type::error)
        return res;
      }
    }

    return res;
  }

  std::vector<std::shared_ptr<object>> eval_expressions(const std::vector<std::shared_ptr<expression>>& exprs, const std::shared_ptr<environment>& env)
  {
    std::vector<std::shared_ptr<object>> res;
    //res.reserve(exprs.size());

    for(const auto& expr : exprs)
    {
      auto evaluated = eval(expr, env);
      
      if(is_error(evaluated))
        return { evaluated };

      res.push_back(evaluated);
    }
    return res;
  }

  std::shared_ptr<object> eval_if_expression(const std::shared_ptr<_if>& if_expr, const std::shared_ptr<environment>& env)
  {
    auto cond_eval = eval(if_expr->condition, env);

    if(is_error(cond_eval))
      return cond_eval;

    if(to_boolean(cond_eval)->get_value())
      return eval(if_expr->consequence, env);
    else if(if_expr->alternative)
      return eval(if_expr->alternative, env);
    
    return get_null();
  }


  std::shared_ptr<object> eval_identifire(const std::shared_ptr<identifire>& ident, const std::shared_ptr<environment>& env)
  {
    auto ret = env->get(ident->value);
    
    if(!ret.has_value())
    {
      auto builtin_ret = builtin_env.get(ident->value);
      if(!builtin_ret.has_value())
        return add_error("identifire not found: " + ident->value); 
      
      return builtin_ret.value();
    }
    return ret.value();
  }

  //TODO: operator overloading
  std::shared_ptr<object> eval_prefix_expression(const std::string& op, const std::shared_ptr<object>& right)
  {
    //TODO: operator enum
    if(op == "!")
      return eval_bang_operator_expression(right);
    else if(op == "-")
      return eval_minus_prefix_operator_expression(right);

    return add_error("unknown operator: " + op + " " + std::to_string((uint32_t)right->get_type()));
  }


  //TODO: operator overloading
  std::shared_ptr<object> eval_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right)
  {
    if(left->get_type() == object_type::integer && right->get_type() == object_type::integer)
      return eval_integer_infix_expression(op, left, right);
    else if(left->get_type() == object_type::boolean && right->get_type() == object_type::boolean)
      return eval_boolean_infix_expression(op, left, right);
    else if(left->get_type() == object_type::string && right->get_type() == object_type::string)
      return eval_string_infix_expression(op, left, right);
    if(left->get_type() != right->get_type())
      return add_error("type mismatch: " + std::to_string((uint32_t)left->get_type()) + " " + op + " " + std::to_string((uint32_t)right->get_type()));
    
    return add_error("unknown operator: " + op + " " + std::to_string((uint32_t)left->get_type()) + " " + std::to_string((uint32_t)right->get_type()));
  }


  std::shared_ptr<object> eval_bang_operator_expression(const std::shared_ptr<object>& obj)
  {
    switch(obj->get_type())
    {
      case object_type::boolean:
      {
        auto bool_obj = std::static_pointer_cast<boolean>(obj);
        return to_boolean(!bool_obj->get_value());
      }
      case object_type::integer:
      {
        auto integer_obj = std::static_pointer_cast<integer>(obj);
        return to_boolean(integer_obj->get_value());
      }
      case object_type::null:
      {
        return get_true();
      }
      default: return get_false();
    }
  }
 
  std::shared_ptr<object> eval_minus_prefix_operator_expression(const std::shared_ptr<object>& obj)
  {
    if(obj->get_type() != object_type::integer)
      return add_error("unknown operator: " + std::string(" - ") + std::to_string((uint32_t)obj->get_type()));

    auto int_obj = std::static_pointer_cast<integer>(obj);
    return std::make_shared<integer>(-int_obj->get_value());
  }
 
  std::shared_ptr<object> eval_integer_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right)
  {
    auto left_int_obj = std::static_pointer_cast<integer>(left);
    auto right_int_obj = std::static_pointer_cast<integer>(right);
    
    auto left_val = left_int_obj->get_value();
    auto right_val = right_int_obj->get_value();

    //TODO: operator enum

    if(op == "+")
      return std::make_shared<integer>(left_val + right_val);
    if(op == "-")
      return std::make_shared<integer>(left_val - right_val);
    if(op == "*")
      return std::make_shared<integer>(left_val * right_val);
    if(op == "/")
      return std::make_shared<integer>(left_val / right_val);
   
    if(op == "<")
      return to_boolean(left_val < right_val);
    if(op == ">")
      return to_boolean(left_val > right_val);
    if(op == "==")
      return to_boolean(left_val == right_val);
    if(op == "!=")
      return to_boolean(left_val != right_val);

    return add_error("unknown operator: " + op + " " + std::to_string((uint32_t)left->get_type()) + " " + std::to_string((uint32_t)right->get_type()));
  }

  std::shared_ptr<object> eval_boolean_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right)
  {
    auto left_bool_obj = std::static_pointer_cast<boolean>(left);
    auto right_bool_obj = std::static_pointer_cast<boolean>(right);
    
    auto left_val = left_bool_obj->get_value();
    auto right_val = right_bool_obj->get_value();

    //TODO: operator enum
    
    if(op == "==")
      return to_boolean(left_val == right_val);
    if(op == "!=")
      return to_boolean(left_val != right_val);

    return get_null();
  }

  std::shared_ptr<object> eval_string_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right)
  {
    auto left_string_obj = std::static_pointer_cast<string>(left);
    auto right_string_obj = std::static_pointer_cast<string>(right);

    if(op != "+")
      return add_error("unknown operator: " + op + std::to_string((uint32_t)left_string_obj->get_type()) + std::to_string((uint32_t)right_string_obj->get_type()));

    return std::make_shared<string>(left_string_obj->get_value() + right_string_obj->get_value());
  }


  std::shared_ptr<environment> extend_function_environment(const std::shared_ptr<fun>&, const std::vector<std::shared_ptr<object>>&);
  std::shared_ptr<object> unwrap_return_value(const std::shared_ptr<object>&);

  std::shared_ptr<object> invoke_function(const std::shared_ptr<object>& fun_obj, const std::vector<std::shared_ptr<object>>& args)
  { 
    if(fun_obj->get_type() == object_type::fun)
    {
      auto _fun = std::static_pointer_cast<fun>(fun_obj);
      auto ext_env = extend_function_environment(_fun, args);
      auto evaluated = eval(_fun->body, ext_env);

      return unwrap_return_value(evaluated);
    }
    else if(fun_obj->get_type() == object_type::builtin)
    {
      auto builtin_fun = std::static_pointer_cast<builtin>(fun_obj);
      return builtin_fun->_fun(args);
    }
    else 
      return add_error("expression is not a function: " + std::to_string((uint32_t)fun_obj->get_type()));
  }

  std::shared_ptr<environment> extend_function_environment(const std::shared_ptr<fun>&_fun, const std::vector<std::shared_ptr<object>>& args)
  {
    auto ext_env = std::make_shared<environment>(_fun->env);
    for(auto i = 0; const auto& param : _fun->parameters) 
    {
      if(i >= args.size()) 
      {
        //return add_error("too few arguments");
        std::cout << "too few arguments\n";
      }
      ext_env->set(param->value, args[i++]);
    }
    return ext_env;
  }

  std::shared_ptr<object> unwrap_return_value(const std::shared_ptr<object>& ret_val_obj)
  {
    if(ret_val_obj->get_type() == object_type::ret_value)
    {
      auto rv = std::static_pointer_cast<ret_value>(ret_val_obj);
      return rv->get_value();
    }
    return ret_val_obj;
  }

  std::shared_ptr<object> eval_index_expression(const std::shared_ptr<object>& left, const std::shared_ptr<object>& right)
  {
    if(left->get_type() == object_type::array && right->get_type() == object_type::integer)
      return eval_array_index_expression(left, right);
    
    if(left->get_type() == object_type::map)
      return eval_hash_index_expression(left, right);
    
    return add_error("index operator not supported for: " + std::to_string((uint32_t)left->get_type()));
  }

  std::shared_ptr<object> eval_array_index_expression(const std::shared_ptr<object>& arr_obj, const std::shared_ptr<object>& index_obj)
  {
    auto arr = std::static_pointer_cast<array>(arr_obj);
    auto idx = std::static_pointer_cast<integer>(index_obj)->get_value();
    auto last = arr->get_elements().size();
    
    if(idx < 0 || idx >= last)
      return get_null();

    return arr->get_elements()[idx];
  }

  std::shared_ptr<object> eval_hash_index_expression(const std::shared_ptr<object>& m, const std::shared_ptr<object>& index)
  {
    auto _map = std::static_pointer_cast<map>(m);
    auto key = std::dynamic_pointer_cast<hashable>(index);
    if(!key)
      return add_error("type: " + std::to_string((uint32_t)index->get_type()) + " is not hashable");

    const auto& hm = _map->get_map();
    auto elem_it = hm.find(key->hash());
    if(elem_it == hm.end())
      return get_null();

    return elem_it->second.value;
  }

  std::shared_ptr<object> eval_map(const std::shared_ptr<map_literal>& hm, const std::shared_ptr<environment>& env)
  {
    std::unordered_map<hash_t, map::hash_pair> _map;
    for(const auto& elem : hm->pairs)
    {
      auto key = eval(elem.first, env);
      if(is_error(key))
        return key;

      auto hash_tp = std::dynamic_pointer_cast<hashable>(key);
      if(!hash_tp)
        return add_error("type: " + std::to_string((uint32_t)key->get_type()) + " not hashable");

      auto value = eval(elem.second, env);
      if(is_error(value))
        return value;

      auto hashed = hash_tp->hash();
      _map[hashed] = map::hash_pair{ .key = key, .value = value };
    }

    return std::make_shared<map>(_map);
  }

  std::shared_ptr<error> add_error(const std::string& message)
  {
    return std::make_shared<error>(message);
  }

  //using functions to leverage to static var
  static std::shared_ptr<boolean> get_true()
  {
    static auto _true = std::make_shared<boolean>(true); 
    return _true;
  }

  static std::shared_ptr<boolean> get_false()
  {
    static auto _false = std::make_shared<boolean>(false);
    return _false;
  }

  std::shared_ptr<null> get_null()
  {
    static auto _null = std::make_shared<null>();
    return _null;
  }

  std::shared_ptr<boolean> to_boolean(bool b)
  {
    return b ? get_true() : get_false();
  }

  std::shared_ptr<boolean> to_boolean(const std::shared_ptr<object>& obj)
  {
    switch(obj->get_type())
    {
      case object_type::integer:
      {
        auto int_obj = std::static_pointer_cast<integer>(obj);
        return to_boolean(int_obj->get_value()); // 0 == false : true
      }
      case object_type::boolean:
      {
        return std::static_pointer_cast<boolean>(obj);
      }
      case object_type::null:
      {
        return get_false();
      }
      default: return get_false();
    }
  }

  static bool is_error(const std::shared_ptr<object>& obj)
  {
    if(obj)
      return obj->get_type() == object_type::error;
    return false;
  }
}
