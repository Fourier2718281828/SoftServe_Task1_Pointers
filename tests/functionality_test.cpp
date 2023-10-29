#include <gtest/gtest.h>
#include "../string_list.hpp"

class StringListFunctionalityTest : public ::testing::Test
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

TEST(StringListAdditionalTest, IsNullWhenDestroyed)
{
    StringList list = nullptr;
    string_list_init(&list);
    EXPECT_TRUE(list != nullptr);
    string_list_destroy(&list);
    EXPECT_TRUE(list == nullptr);
}

TEST_F(StringListFunctionalityTest, InitAndDestroy) 
{
    bool list_is_empty;
    string_list_is_empty(list, &list_is_empty);
    EXPECT_TRUE(list_is_empty);
    EXPECT_TRUE(list != nullptr);
}

SizeType size_of_list(StringList list)
{
    SizeType size = 0u;
    string_list_size(list, &size);

    return size;
}

TEST_F(StringListFunctionalityTest, AddAndSize) 
{
    const cString str1 = "string_1";
    const cString str2 = "string_2";
    const cString str3 = "string_3";
    const cString str4 = "string_4";
    const SizeType expected_size = 4u;

    string_list_add(&list, str1);
    string_list_add(&list, str2);
    string_list_add(&list, str3);
    string_list_add(&list, str4);

    bool list_is_empty;
    string_list_is_empty(list, &list_is_empty);

    EXPECT_FALSE(list_is_empty);
    EXPECT_EQ(expected_size, size_of_list(list));

    EXPECT_STREQ(list[0], str1);
    EXPECT_STREQ(list[1], str2);
    EXPECT_STREQ(list[2], str3);
    EXPECT_STREQ(list[3], str4);
}

TEST_F(StringListFunctionalityTest, Remove) 
{
    const cString strToRepeat = "string_1";
    const SizeType expected_size = 6u;

    string_list_add(&list, strToRepeat);
    string_list_add(&list, "0");
    string_list_add(&list, "1");
    string_list_add(&list, strToRepeat);
    string_list_add(&list, strToRepeat);
    string_list_add(&list, "2");
    string_list_add(&list, "3");
    string_list_add(&list, strToRepeat);
    string_list_add(&list, "4");
    string_list_add(&list, "5");
    string_list_add(&list, strToRepeat);

    string_list_remove(list, strToRepeat);

    SizeType found_index = 0u;
    string_list_index_of(list, strToRepeat, &found_index);

    EXPECT_EQ((SizeType)-1, found_index);
    EXPECT_EQ(expected_size, size_of_list(list));

    for (SizeType i = 0u; i < expected_size; ++i)
    {
        EXPECT_STREQ(std::to_string(i).c_str(), list[i]);
    }
}

TEST_F(StringListFunctionalityTest, IndexOf)
{
    // 1 - 5
    string_list_add(&list, "0");
    string_list_add(&list, "1");
    string_list_add(&list, "2");
    string_list_add(&list, "3");
    string_list_add(&list, "4");
    string_list_add(&list, "5");

    // noise
    string_list_add(&list, "1");
    string_list_add(&list, "5");
    string_list_add(&list, "2");
    string_list_add(&list, "3");

    for (SizeType i = 0u; i < 6u; ++i)
    {
        SizeType index = (SizeType)(-1);
        string_list_index_of(list, std::to_string(i).c_str(), &index);
        EXPECT_EQ(index, i);
    }
} 

TEST_F(StringListFunctionalityTest, RemoveDuplicates)
{
    const size_t unique_string_count = 3;
    cString unique_strings[unique_string_count] { "0", "1", "2" };

    string_list_add(&list, unique_strings[1]);
    string_list_add(&list, unique_strings[2]);
    string_list_add(&list, unique_strings[0]);
    string_list_add(&list, unique_strings[0]);
    string_list_add(&list, unique_strings[1]);
    string_list_add(&list, unique_strings[0]);
    string_list_add(&list, unique_strings[2]);
    string_list_add(&list, unique_strings[1]);
    string_list_add(&list, unique_strings[2]);
    string_list_add(&list, unique_strings[1]);
    string_list_add(&list, unique_strings[0]);

    string_list_remove_duplicates(&list);

    EXPECT_EQ(size_of_list(list), unique_string_count);
    
    for (SizeType i = 0u; i < unique_string_count; ++i)
    {
        SizeType index = 0u;
        string_list_index_of(list, std::to_string(i).c_str(), &index);
        EXPECT_NE(index, (SizeType)(-1));
    }
}

TEST_F(StringListFunctionalityTest, ReplaceInStrings)
{
    cString before = "ab";
    cString after  = "ba";

    cString str1 = "abbb";
    cString res1 = "babb";
    
    cString str2 = "aaab";
    cString res2 = "aaba";
    
    cString str3 = "abab";
    cString res3 = "baba";
    
    cString str4 = "aaaa";
    cString res4 = "aaaa";
    
    cString str5 = "";
    cString res5 = "";

    string_list_add(&list, str1);
    string_list_add(&list, str2);
    string_list_add(&list, str3);
    string_list_add(&list, str4);
    string_list_add(&list, str5);

    string_list_replace_in_strings(list, before, after);

    EXPECT_STREQ(list[0], res1);
    EXPECT_STREQ(list[1], res2);
    EXPECT_STREQ(list[2], res3);
    EXPECT_STREQ(list[3], res4);
    EXPECT_STREQ(list[4], res5);
}

static bool is_sorted(StringList list)
{
    const SizeType size = size_of_list(list);
    if (size <= 1) 
    {
        return true;
    }

    for (SizeType i = 1u; i < size; ++i) {
        if (strcmp(list[i], list[i - 1]) < 0) 
        {
            return false;
        }
    }

    return true;
}

TEST_F(StringListFunctionalityTest, Sort)
{
    string_list_add(&list, "2");
    string_list_add(&list, "5");
    string_list_add(&list, "0");
    string_list_add(&list, "1");
    string_list_add(&list, "1");
    string_list_add(&list, "1");
    string_list_add(&list, "3");
    string_list_add(&list, "1");
    string_list_add(&list, "5");
    string_list_add(&list, "4");
    string_list_add(&list, "2");
    string_list_add(&list, "3");
    string_list_add(&list, "3");
    string_list_add(&list, "3");
    string_list_add(&list, "3");
    string_list_add(&list, "2");
    string_list_add(&list, "3");
    string_list_add(&list, "2");
    string_list_add(&list, "3");

    string_list_sort(list);

    EXPECT_TRUE(is_sorted(list));
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
