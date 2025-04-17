#pragma once 

#include "ast.hpp"
#include "object.hpp"

#include <memory>

//TODO: don't print the errors return them like the parser

namespace my_ns
{
  using trampoline_result = std::variant<std::shared_ptr<object>, std::function<std::shared_ptr<object>()>>;
  trampoline_result eval_trampoline(const std::shared_ptr<node>&, const std::shared_ptr<environment>&, size_t depth = 0);
  std::shared_ptr<object> eval(const std::shared_ptr<node>&, const std::shared_ptr<environment>&, size_t depth = 0);
  std::shared_ptr<object> eval_program(const std::shared_ptr<program>&, const std::shared_ptr<environment>&);
  std::shared_ptr<object> eval_block_statement(const std::shared_ptr<block>&, const std::shared_ptr<environment>& env);
 
  std::vector<std::shared_ptr<object>> eval_expressions(const std::vector<std::shared_ptr<expression>>&, const std::shared_ptr<environment>&);
  std::shared_ptr<object> eval_if_expression(const std::shared_ptr<_if>&, const std::shared_ptr<environment>& env);


  std::shared_ptr<object> eval_identifire(const std::shared_ptr<identifire>&, const std::shared_ptr<environment>&);

  std::shared_ptr<object> eval_prefix_expression(const std::string& op, const std::shared_ptr<object>& right);
  std::shared_ptr<object> eval_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right);
  
  std::shared_ptr<object> eval_bang_operator_expression(const std::shared_ptr<object>&);
  std::shared_ptr<object> eval_minus_prefix_operator_expression(const std::shared_ptr<object>&);


  std::shared_ptr<object> eval_integer_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right);
  std::shared_ptr<object> eval_boolean_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right);
  std::shared_ptr<object> eval_string_infix_expression(const std::string& op, const std::shared_ptr<object>& left, const std::shared_ptr<object>& right);

  std::shared_ptr<object> invoke_function(const std::shared_ptr<object>&, const std::vector<std::shared_ptr<object>>&);

  std::shared_ptr<object> eval_index_expression(const std::shared_ptr<object>& left, const std::shared_ptr<object>& right);
  std::shared_ptr<object> eval_array_index_expression(const std::shared_ptr<object>& arr, const std::shared_ptr<object>& index);
  std::shared_ptr<object> eval_hash_index_expression(const std::shared_ptr<object>& arr, const std::shared_ptr<object>& index);
  std::shared_ptr<object> eval_map(const std::shared_ptr<map_literal>&, const std::shared_ptr<environment>&);

  std::shared_ptr<error> add_error(const std::string& message);
}
