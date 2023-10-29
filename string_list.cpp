#include "string_list.hpp"
#include <stdlib.h>
#include <string.h>

#define PRIVATE static
#define PUBLIC

static const SizeType INITIAL_CAPACITY = 0u;
static const SizeType FIELDS_BLOCK_SIZE = sizeof(SizeType) << 1;


PRIVATE inline size_t allocating_bytes_count(const SizeType capacity);
PRIVATE void move_to_the_fields_block(StringList* list_ptr);
PRIVATE void move_to_the_strings_block(StringList* list_ptr);
PRIVATE void set_fields_block(StringList list);
PRIVATE SizeType* get_size_ptr(StringList list);
PRIVATE SizeType* get_capacity_ptr(StringList list);
PRIVATE SizeType string_list_capacity(StringList list);
PRIVATE ErrorCode extend_string_list(StringList* list_ptr, const SizeType new_capacity);
PRIVATE inline SizeType next_capacity(const SizeType old_capacity);
PRIVATE ErrorCode place_element(StringList list, const SizeType index, cString str);
PRIVATE void replace_in_string(mString string, cString before, cString after);


PUBLIC ErrorCode string_list_init(StringList* list_ptr)
{
    const size_t bytes_to_allocate_count = allocating_bytes_count(INITIAL_CAPACITY);
    const bool size_type_overflow_detected = bytes_to_allocate_count < INITIAL_CAPACITY ||
        bytes_to_allocate_count < FIELDS_BLOCK_SIZE;
    if (size_type_overflow_detected)
    {
        return ErrorCode::LackOfMemory;
    }

    void* allocated_chunk = malloc(bytes_to_allocate_count);

    if (allocated_chunk == nullptr)
    {
        return ErrorCode::LackOfMemory;
    }

    *list_ptr = (StringList)allocated_chunk;
    set_fields_block(*list_ptr);
    move_to_the_strings_block(list_ptr);

    return ErrorCode::Success;
}

PUBLIC ErrorCode string_list_destroy(StringList* list)
{
    move_to_the_fields_block(list);
    free(*list);
    *list = nullptr;

    return ErrorCode::Success;
}

PUBLIC bool string_list_is_empty(StringList list)
{
    return string_list_size(list) == 0u;
}

PUBLIC SizeType string_list_size(StringList list)
{
    return *get_size_ptr(list);
}

PUBLIC SizeType string_list_index_of(StringList list, cString str)
{
    for (SizeType i = 0u; i < string_list_size(list); ++i)
    {
        if (strcmp(list[i], str) == 0)
        {
            return i;
        }
    }

    return (SizeType)(-1);
}

PUBLIC ErrorCode string_list_remove_duplicates(StringList* list)
{
    StringList result;
    string_list_init(&result);

    for (SizeType i = 0u; i < string_list_size(*list); ++i)
    {
        mString read_string = (*list)[i];

        if (string_list_index_of(result, read_string) == -1)
        {
            string_list_add(&result, read_string);
        }
    }

    string_list_destroy(list);
    *list = result;

    return ErrorCode::Success;
}

PUBLIC ErrorCode string_list_add(StringList* list_ptr, cString str)
{
    const SizeType size = string_list_size(*list_ptr);
    const SizeType capacity = string_list_capacity(*list_ptr);

    if (size == capacity)
    {
        const SizeType new_capacity = next_capacity(capacity);
        ErrorCode result_code = extend_string_list(list_ptr, new_capacity);

        if (result_code != ErrorCode::Success)
        {
            return result_code;
        }
    }

    ErrorCode result_code = place_element(*list_ptr, size, str);

    if (result_code != ErrorCode::Success)
    {
        return result_code;
    }

    SizeType* size_ptr = get_size_ptr(*list_ptr);
    ++(*size_ptr);

    return ErrorCode::Success;
}

PUBLIC ErrorCode string_list_remove(StringList list, cString str)
{
    if (string_list_is_empty(list))
    {
        return ErrorCode::Success;
    }

    StringList writing_ptr = list;
    StringList reading_ptr = list;
    StringList end_ptr = list + string_list_size(list);
    SizeType new_size = 0u;

    for (; reading_ptr != end_ptr; ++reading_ptr)
    {
        cString read_word = *reading_ptr;

        if (strcmp(read_word, str) != 0)
        {
            *writing_ptr = *reading_ptr;
            ++writing_ptr;
            ++new_size;
        }
    }

    SizeType* size_ptr = get_size_ptr(list);
    *size_ptr = new_size;

    return ErrorCode::Success;
}

PUBLIC ErrorCode string_list_replace_in_strings(StringList list, cString before, cString after)
{
    for (SizeType i = 0u; i < string_list_size(list); ++i)
    {
        replace_in_string(list[i], before, after);
    }

    return ErrorCode::Success;
}

PUBLIC ErrorCode string_list_sort(StringList list)
{
    if (string_list_is_empty(list))
    {
        return ErrorCode::Success;
    }

    for (SizeType i = 0u; i < string_list_size(list) - 1; ++i)
    {
        SizeType min_index = i;
        for (SizeType j = i + 1; j < string_list_size(list); ++j)
        {
            if (strcmp(list[j], list[min_index]) < 0)
            {
                min_index = j;
            }
        }

        mString temp = list[i];
        list[i] = list[min_index];
        list[min_index] = temp;
    }

    return ErrorCode::Success;
}


PRIVATE inline size_t allocating_bytes_count(const SizeType capacity)
{
    return capacity * sizeof(mString) + FIELDS_BLOCK_SIZE;
}

PRIVATE void move_to_the_fields_block(StringList* list_ptr)
{
    StringList list = *list_ptr;
    SizeType* reinterpreted_list = (SizeType*)list;
    reinterpreted_list -= 2;
    *list_ptr = (StringList)reinterpreted_list;
}

PRIVATE void move_to_the_strings_block(StringList* list_ptr)
{
    StringList list = *list_ptr;
    SizeType* reinterpreted_list = (SizeType*)list;
    reinterpreted_list += 2;
    *list_ptr = (StringList)reinterpreted_list;
}

PRIVATE void set_fields_block(StringList list)
{
    const SizeType initial_size = 0u;
    SizeType* fields_view = (SizeType*)list;
    fields_view[0] = initial_size;
    fields_view[1] = INITIAL_CAPACITY;
}

PRIVATE SizeType* get_size_ptr(StringList list)
{
    move_to_the_fields_block(&list);
    return (SizeType*)list;
}

PRIVATE SizeType* get_capacity_ptr(StringList list)
{
    move_to_the_fields_block(&list);
    return (SizeType*)list + 1;
}

PRIVATE SizeType string_list_capacity(StringList list)
{
    return *get_capacity_ptr(list);
}

PRIVATE ErrorCode extend_string_list(StringList* list_ptr, const SizeType new_capacity)
{
    StringList old_strings_view = *list_ptr;
    const SizeType realloc_bytes_count = allocating_bytes_count(new_capacity);
    move_to_the_fields_block(list_ptr);
    void* new_chunk = realloc(*list_ptr, realloc_bytes_count);

    if (new_chunk == nullptr)
    {
        return ErrorCode::LackOfMemory;
    }

    if (new_chunk != *list_ptr)
    {
        const SizeType old_capacity = string_list_capacity(old_strings_view);
        const SizeType old_bytes_count = allocating_bytes_count(old_capacity);
        memcpy(new_chunk, *list_ptr, old_bytes_count);
        *list_ptr = (StringList)new_chunk;
    }

    move_to_the_strings_block(list_ptr);

    SizeType* capacity_ptr = get_capacity_ptr(*list_ptr);
    *capacity_ptr = new_capacity;

    return ErrorCode::Success;
}

PRIVATE inline SizeType next_capacity(const SizeType old_capacity)
{
    return 1 + (old_capacity << 1);
}

PRIVATE ErrorCode place_element(StringList list, const SizeType index, cString str)
{
    const size_t allocated_bytes_count = strlen(str) + 1;
    void* allocated_memory = malloc(allocated_bytes_count);

    if (allocated_memory == nullptr)
    {
        return ErrorCode::LackOfMemory;
    }

    list[index] = (mString)allocated_memory;
    strcpy_s(list[index], allocated_bytes_count, str);

    return ErrorCode::Success;
}


PRIVATE void replace_in_string(mString string, cString before, cString after)
{
    const SizeType string_len = strlen(string);
    const SizeType before_len = strlen(before);
    const SizeType after_len = strlen(after);

    if (string_len == 0u)
    {
        return;
    }

    for (SizeType i = 0; i <= string_len - before_len; ++i)
    {
        if (strncmp(string + i, before, before_len) == 0)
        {
            memmove(string + i + after_len, string + i + before_len, string_len - i - before_len + 1);
            memcpy(string + i, after, after_len);
            i += after_len - 1;
        }
    }
}
