#include <gtest/gtest.h>
#include "../string_list.hpp"

class StringListValidationTest : public ::testing::Test
{
protected:
    StringList list;

    void SetUp() override 
    {
        list = nullptr;
        string_list_init(&list);
    }

    void TearDown() override 
    {
        string_list_destroy(&list);
    }
};

TEST(StringListValidationDestroyTest, StringListDestroyNotNull)
{
    StringList list;
    string_list_init(&list);

    EXPECT_EQ(string_list_destroy(nullptr), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_destroy(&list)  , ErrorCode::NullPointerInput);
}

TEST_F(StringListValidationTest, StringListIsEmptyNotNull)
{
    EXPECT_TRUE(false);
}

TEST_F(StringListValidationTest, StringListAddNotNull)
{
    EXPECT_EQ(string_list_add(&list, nullptr)  , ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_add(nullptr, "asdas"), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_add(nullptr, nullptr), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_add(&list, "abcdefg"), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_add(&list, "fghijfg"), ErrorCode::NullPointerInput);
}

TEST_F(StringListValidationTest, StringListRemoveNotNull)
{
    EXPECT_EQ(string_list_remove(list   , nullptr), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_remove(nullptr, "asdas"), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_remove(nullptr, nullptr), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_remove(list   , "defga"), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_remove(list   , "ijfga"), ErrorCode::NullPointerInput);
}

TEST_F(StringListValidationTest, StringListSizeNotNull)
{
    EXPECT_TRUE(false);
}

TEST_F(StringListValidationTest, StringListIndexOfNotNull)
{
    EXPECT_TRUE(false);
}

TEST_F(StringListValidationTest, StringListRemoveDuplicatesNotNull)
{
    EXPECT_EQ(string_list_remove_duplicates(nullptr), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_remove_duplicates(&list)  , ErrorCode::NullPointerInput);
}

TEST_F(StringListValidationTest, StringListReplaceInStringsNotNull)
{
    EXPECT_EQ(string_list_replace_in_strings(nullptr, nullptr, nullptr), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_replace_in_strings(list   , nullptr, nullptr), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_replace_in_strings(nullptr, "abcde", nullptr), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_replace_in_strings(nullptr, nullptr, "abcde"), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_replace_in_strings(list   , "abcde", nullptr), ErrorCode::NullPointerInput);
    EXPECT_EQ(string_list_replace_in_strings(list   , nullptr, "abcde"), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_replace_in_strings(list   , "abcde", "fghij"), ErrorCode::NullPointerInput);
}

TEST_F(StringListValidationTest, StringListSortNotNull)
{
    EXPECT_EQ(string_list_sort(nullptr), ErrorCode::NullPointerInput);
    EXPECT_NE(string_list_sort(list)   , ErrorCode::NullPointerInput);
}

