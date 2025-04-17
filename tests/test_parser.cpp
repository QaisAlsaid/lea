#include <gtest/gtest.h>
#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"

namespace my_ns {

TEST(ParserTest, TestVarStatement) {
    std::string input = "var x = 5;";
    lexer l(input);
    parser p(&l);
    auto prog = p.parse_program();

    ASSERT_EQ(prog->m_statements.size(), 1) << "Program should have 1 statement";
    ASSERT_EQ(p.get_errors().size(), 0) << "Parser should have no errors";

    auto var_stmt = std::dynamic_pointer_cast<var>(prog->m_statements[0]);
    ASSERT_NE(var_stmt, nullptr) << "Statement should be a var statement";
    EXPECT_EQ(var_stmt->name.value, "x");
    EXPECT_EQ(var_stmt->name.token_literal(), "x");

    auto int_lit = std::dynamic_pointer_cast<integer_literal>(var_stmt->value);
    ASSERT_NE(int_lit, nullptr) << "Value should be an integer literal";
    EXPECT_EQ(int_lit->value, 5);
    EXPECT_EQ(int_lit->token_literal(), "5");
}

TEST(ParserTest, TestReturnStatement) {
    std::string input = "ret 42;";
    lexer l(input);
    parser p(&l);
    auto prog = p.parse_program();

    ASSERT_EQ(prog->m_statements.size(), 1);
    ASSERT_EQ(p.get_errors().size(), 0);

    auto ret_stmt = std::dynamic_pointer_cast<ret>(prog->m_statements[0]);
    ASSERT_NE(ret_stmt, nullptr);
    EXPECT_EQ(ret_stmt->token_literal(), "ret");

    auto int_lit = std::dynamic_pointer_cast<integer_literal>(ret_stmt->return_value);
    ASSERT_NE(int_lit, nullptr);
    EXPECT_EQ(int_lit->value, 42);
}

TEST(ParserTest, TestInfixExpression) {
    std::string input = "5 + 3;";
    lexer l(input);
    parser p(&l);
    auto prog = p.parse_program();

    ASSERT_EQ(prog->m_statements.size(), 1);
    ASSERT_EQ(p.get_errors().size(), 0);

    auto expr_stmt = std::dynamic_pointer_cast<expression_statement>(prog->m_statements[0]);
    ASSERT_NE(expr_stmt, nullptr);

    auto _infix = std::dynamic_pointer_cast<infix>(expr_stmt->_expression);
    ASSERT_NE(_infix, nullptr);
    EXPECT_EQ(_infix->_operator, "+");

    auto left = std::dynamic_pointer_cast<integer_literal>(_infix->left);
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value, 5);

    auto right = std::dynamic_pointer_cast<integer_literal>(_infix->right);
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->value, 3);
}

TEST(ParserTest, TestIfExpression) {
    std::string input = "if (x < y) { x } else { y }";
    lexer l(input);
    parser p(&l);
    auto prog = p.parse_program();

    ASSERT_EQ(prog->m_statements.size(), 1);
    ASSERT_EQ(p.get_errors().size(), 0);

    auto expr_stmt = std::dynamic_pointer_cast<expression_statement>(prog->m_statements[0]);
    auto if_expr = std::dynamic_pointer_cast<_if>(expr_stmt->_expression);
    ASSERT_NE(if_expr, nullptr);

    auto cond = std::dynamic_pointer_cast<infix>(if_expr->condition);
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->_operator, "<");
    EXPECT_EQ(std::dynamic_pointer_cast<identifire>(cond->left)->value, "x");
    EXPECT_EQ(std::dynamic_pointer_cast<identifire>(cond->right)->value, "y");

    ASSERT_EQ(if_expr->consequence->statements.size(), 1);
    ASSERT_EQ(if_expr->alternative->statements.size(), 1);
}

TEST(ParserTest, TestFunctionLiteral) {
    std::string input = "fun(x, y) { x + y; }";
    lexer l(input);
    parser p(&l);
    auto prog = p.parse_program();

    ASSERT_EQ(prog->m_statements.size(), 1);
    ASSERT_EQ(p.get_errors().size(), 0);

    auto expr_stmt = std::dynamic_pointer_cast<expression_statement>(prog->m_statements[0]);
    auto fun_lit = std::dynamic_pointer_cast<fun_literal>(expr_stmt->_expression);
    ASSERT_NE(fun_lit, nullptr);
    ASSERT_EQ(fun_lit->parameters.size(), 2);
    EXPECT_EQ(fun_lit->parameters[0]->value, "x");
    EXPECT_EQ(fun_lit->parameters[1]->value, "y");

    ASSERT_EQ(fun_lit->body->statements.size(), 1);
}

}  // namespace my_ns
