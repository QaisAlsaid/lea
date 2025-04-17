#include <gtest/gtest.h>
#include "ast.hpp"

namespace my_ns {

TEST(ASTTest, TestProgramString) {
    auto prog = std::make_shared<program>();
    EXPECT_EQ(prog->to_string(), "") << "Empty program should return empty string";

    auto stmt = std::make_shared<var>(token{token_type::var, "var"});
    stmt->name = identifire{token{token_type::identifire, "x"}, "x"};
    stmt->value = std::make_shared<integer_literal>(token{token_type::integer, "5"});
    prog->m_statements.push_back(stmt);

    std::string expected = "var x = 5;";
    EXPECT_EQ(prog->to_string(), expected) << "Program string representation failed";
}

TEST(ASTTest, TestIdentifier) {
    identifire id{token{token_type::identifire, "myVar"}, "myVar"};
    EXPECT_EQ(id.token_literal(), "myVar");
    EXPECT_EQ(id.to_string(), "myVar");
}

TEST(ASTTest, TestIntegerLiteral) {
    integer_literal int_lit{token{token_type::integer, "42"}};
    int_lit.value = 42;
    EXPECT_EQ(int_lit.token_literal(), "42");
    EXPECT_EQ(int_lit.to_string(), "42");
}

TEST(ASTTest, TestPrefixExpression) {
    auto prefix = std::make_shared<prefix>(token{token_type::bang, "!"}, "!");
    prefix->right = std::make_shared<integer_literal>(token{token_type::integer, "5"});
    EXPECT_EQ(prefix->to_string(), "(!5)");
}

}  // namespace my_ns
