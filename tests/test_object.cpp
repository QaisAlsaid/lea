#include <gtest/gtest.h>
#include "object.hpp"

namespace my_ns {

TEST(ObjectTest, TestInteger) {
    auto int_obj = std::make_shared<integer>(42);
    EXPECT_EQ(int_obj->get_type(), object_type::integer);
    EXPECT_EQ(int_obj->inspect(), "42");
    EXPECT_EQ(int_obj->get_value(), 42);
}

TEST(ObjectTest, TestString) {
    auto str_obj = std::make_shared<string>("hello");
    EXPECT_EQ(str_obj->get_type(), object_type::string);
    EXPECT_EQ(str_obj->inspect(), "hello");
    EXPECT_EQ(str_obj->get_value(), "hello");
}

TEST(ObjectTest, TestBoolean) {
    auto bool_obj = std::make_shared<boolean>(true);
    EXPECT_EQ(bool_obj->get_type(), object_type::boolean);
    EXPECT_EQ(bool_obj->inspect(), "true");
    EXPECT_EQ(bool_obj->get_value(), true);
}

TEST(ObjectTest, TestArray) {
    std::vector<std::shared_ptr<object>> elems = {
        std::make_shared<integer>(1),
        std::make_shared<integer>(2)
    };
    auto arr_obj = std::make_shared<array>(elems);
    EXPECT_EQ(arr_obj->get_type(), object_type::array);
    EXPECT_EQ(arr_obj->inspect(), "[1, 2, ]");  // Note: trailing comma and space due to current impl
}

TEST(ObjectTest, TestEnvironment) {
    auto env = std::make_shared<environment>();
    auto val = std::make_shared<integer>(10);
    env->set("x", val);
    auto retrieved = env->get("x");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(std::static_pointer_cast<integer>(retrieved.value())->get_value(), 10);
}

}  // namespace my_ns
