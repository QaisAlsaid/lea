#include <gtest/gtest.h>
#include "lexer.hpp"
#include "token.hpp"

namespace my_ns {

TEST(LexerTest, TestBasicTokens) {
    std::string input = "=+(){},;";
    lexer l(input);
    std::vector<token> expected = {
        {token_type::assign, "="},
        {token_type::plus, "+"},
        {token_type::l_paren, "("},
        {token_type::r_paren, ")"},
        {token_type::l_brace, "{"},
        {token_type::r_brace, "}"},
        {token_type::comma, ","},
        {token_type::semicolon, ";"},
        {token_type::eof, ""}
    };

    for (const auto& exp : expected) {
        token tok = l.next_token();
        EXPECT_EQ(tok.type, exp.type) << "Token type mismatch for literal: " << exp.literal;
        EXPECT_EQ(tok.literal, exp.literal) << "Token literal mismatch for type: " << token_type_to_string(exp.type);
    }
}

TEST(LexerTest, TestComplexInput) {
    std::string input = R"(
        var five = 5;
        var ten = 10;
        var add = fun(x, y) { x + y; };
        var result = add(five, ten);
        !-/*5;
        5 < 10 > 0;
        if (5 < 10) { ret true; } else { ret false; }
        "hello" 'world'
    )";
    lexer l(input);
    std::vector<token> expected = {
        {token_type::var, "var"}, {token_type::identifire, "five"}, {token_type::assign, "="}, {token_type::integer, "5"}, {token_type::semicolon, ";"},
        {token_type::var, "var"}, {token_type::identifire, "ten"}, {token_type::assign, "="}, {token_type::integer, "10"}, {token_type::semicolon, ";"},
        {token_type::var, "var"}, {token_type::identifire, "add"}, {token_type::assign, "="}, {token_type::fun, "fun"},
        {token_type::l_paren, "("}, {token_type::identifire, "x"}, {token_type::comma, ","}, {token_type::identifire, "y"}, {token_type::r_paren, ")"},
        {token_type::l_brace, "{"}, {token_type::identifire, "x"}, {token_type::plus, "+"}, {token_type::identifire, "y"}, {token_type::semicolon, ";"}, {token_type::r_brace, "}"},
        {token_type::semicolon, ";"},
        {token_type::var, "var"}, {token_type::identifire, "result"}, {token_type::assign, "="}, {token_type::identifire, "add"},
        {token_type::l_paren, "("}, {token_type::identifire, "five"}, {token_type::comma, ","}, {token_type::identifire, "ten"}, {token_type::r_paren, ")"},
        {token_type::semicolon, ";"},
        {token_type::bang, "!"}, {token_type::minus, "-"}, {token_type::slash, "/"}, {token_type::astrisk, "*"}, {token_type::integer, "5"}, {token_type::semicolon, ";"},
        {token_type::integer, "5"}, {token_type::less, "<"}, {token_type::integer, "10"}, {token_type::greater, ">"}, {token_type::integer, "0"}, {token_type::semicolon, ";"},
        {token_type::_if, "if"}, {token_type::l_paren, "("}, {token_type::integer, "5"}, {token_type::less, "<"}, {token_type::integer, "10"}, {token_type::r_paren, ")"},
        {token_type::l_brace, "{"}, {token_type::ret, "ret"}, {token_type::_true, "true"}, {token_type::semicolon, ";"}, {token_type::r_brace, "}"},
        {token_type::_else, "else"}, {token_type::l_brace, "{"}, {token_type::ret, "ret"}, {token_type::_false, "false"}, {token_type::semicolon, ";"}, {token_type::r_brace, "}"},
        {token_type::string, "hello"}, {token_type::string, "world"},
        {token_type::eof, ""}
    };

    for (const auto& exp : expected) {
        token tok = l.next_token();
        EXPECT_EQ(tok.type, exp.type) << "Token type mismatch for literal: " << exp.literal;
        EXPECT_EQ(tok.literal, exp.literal) << "Token literal mismatch for type: " << token_type_to_string(exp.type);
    }
}

TEST(LexerTest, TestEqualityOperators) {
    std::string input = "== != =";
    lexer l(input);
    std::vector<token> expected = {
        {token_type::equal, "=="},
        {token_type::not_equal, "!="},
        {token_type::assign, "="},
        {token_type::eof, ""}
    };

    for (const auto& exp : expected) {
        token tok = l.next_token();
        EXPECT_EQ(tok.type, exp.type);
        EXPECT_EQ(tok.literal, exp.literal);
    }
}

}  // namespace my_ns
