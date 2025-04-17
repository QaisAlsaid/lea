#include <gtest/gtest.h>
#include "evaluator.hpp"
#include "parser.hpp"
#include "lexer.hpp"

namespace my_ns {

std::shared_ptr<object> test_eval(const std::string& input) {
    lexer l(input);
    parser p(&l);
    auto prog = p.parse_program();
    auto env = std::make_shared<environment>();
    return eval(prog, env);
}

TEST(EvaluatorTest, TestIntegerExpression) {
    struct TestCase {
        std::string input;
        int64_t expected;
    };
    std::vector<TestCase> tests = {
        {"5", 5},
        {"10 + 2", 12},
        {"5 * 2 + 10", 20},
        {"-50 + 100", 50}
    };

    for (const auto& test : tests) {
        auto result = test_eval(test.input);
        auto int_obj = std::dynamic_pointer_cast<integer>(result);
        ASSERT_NE(int_obj, nullptr) << "Input: " << test.input;
        EXPECT_EQ(int_obj->get_value(), test.expected) << "Input: " << test.input;
    }
}

TEST(EvaluatorTest, TestBooleanExpression) {
    struct TestCase {
        std::string input;
        bool expected;
    };
    std::vector<TestCase> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 == 1", true},
        {"!true", false}
    };

    for (const auto& test : tests) {
        auto result = test_eval(test.input);
        auto bool_obj = std::dynamic_pointer_cast<boolean>(result);
        ASSERT_NE(bool_obj, nullptr) << "Input: " << test.input;
        EXPECT_EQ(bool_obj->get_value(), test.expected) << "Input: " << test.input;
    }
}

TEST(EvaluatorTest, TestIfElse) {
    struct TestCase {
        std::string input;
        std::string expected;
    };
    std::vector<TestCase> tests = {
        {"if (true) { 10 }", "10"},
        {"if (false) { 10 }", "null"},
        {"if (1 < 2) { 20 } else { 30 }", "20"}
    };

    for (const auto& test : tests) {
        auto result = test_eval(test.input);
        EXPECT_EQ(result->inspect(), test.expected) << "Input: " << test.input;
    }
}

TEST(EvaluatorTest, TestFunctionApplication) {
    std::string input = "var add = fun(x, y) { x + y; }; add(5, 10);";
    auto result = test_eval(input);
    auto int_obj = std::dynamic_pointer_cast<integer>(result);
    ASSERT_NE(int_obj, nullptr);
    EXPECT_EQ(int_obj->get_value(), 15);
}

TEST(EvaluatorTest, TestBuiltins) {
    struct TestCase {
        std::string input;
        std::string expected;
    };
    std::vector<TestCase> tests = {
        {"str_len(\"hello\")", "5"},
        {"len([1, 2, 3])", "3"},
        {"to_string(42)", "42"}
    };

    for (const auto& test : tests) {
        auto result = test_eval(test.input);
        EXPECT_EQ(result->inspect(), test.expected) << "Input: " << test.input;
    }
}

TEST(EvaluatorTest, TestRecursion) {
    std::string input = "var factorial = fun(x) { if (x == 0) { 1 } else { x * factorial(x - 1) } }; factorial(5);";
    auto result = test_eval(input);
    auto int_obj = std::dynamic_pointer_cast<integer>(result);
    ASSERT_NE(int_obj, nullptr);
    EXPECT_EQ(int_obj->get_value(), 120);
}

TEST(EvaluatorTest, TestNestedFunctions) {
    std::string input = "var add = fun(x) { fun(y) { x + y } }; var add5 = add(5); add5(10);";
    auto result = test_eval(input);
    auto int_obj = std::dynamic_pointer_cast<integer>(result);
    ASSERT_NE(int_obj, nullptr);
    EXPECT_EQ(int_obj->get_value(), 15);
}

TEST(EvaluatorTest, TestClosures) {
    std::string input = "var newAdder = fun(x) { fun(y) { x + y } }; var addTwo = newAdder(2); addTwo(3);";
    auto result = test_eval(input);
    auto int_obj = std::dynamic_pointer_cast<integer>(result);
    ASSERT_NE(int_obj, nullptr);
    EXPECT_EQ(int_obj->get_value(), 5);
}


TEST(EvaluatorTest, TestComplexExpressions) {
    struct TestCase {
        std::string input;
        std::string expected;
    };
    std::vector<TestCase> tests = {
        {"(5 + 10 * 2 + 15 / 3) * 2 - 10;", "50"},
        {"if (10 > 1) { if (10 < 20) { 1 } else { 0 } } else { 0 }", "1"},
        {"[1, 2, 3][1];", "2"},
        {"{\"one\": 1, \"two\": 2}[\"one\"];", "1"},
        //error {"var fib = fun(n) { puts(\"fib\"); puts(to_string(n)); if (n <= 1) { n } else { fib(n - 1) + fib(n - 2) } }; fib(5);", "55"}
    };

    for (const auto& test : tests) {
        auto result = test_eval(test.input);
        EXPECT_EQ(result->inspect(), test.expected) << "Input: " << test.input;
    }
}

}  // namespace my_ns
