#include "string_list.hpp"
#include <stdlib.h>
#include <string.h>

#define PRIVATE static
#define PUBLIC

static const SizeType INITIAL_CAPACITY = 0u;
static const SizeType FIELDS_BLOCK_SIZE = sizeof(SizeType) << 1;

// Forwarded declarations of basic implementations
ErrorCode impl_string_list_init(StringList* list_ptr);
ErrorCode impl_string_list_destroy(StringList* list);
bool impl_string_list_is_empty(StringList list);
ErrorCode impl_string_list_add(StringList* list_ptr, cString str);
ErrorCode impl_string_list_remove(StringList list, cString str);
SizeType impl_string_list_size(StringList list);
SizeType impl_string_list_index_of(StringList list, cString str);
ErrorCode impl_string_list_remove_duplicates(StringList* list);
ErrorCode impl_string_list_replace_in_strings(StringList list, cString before, cString after);
ErrorCode impl_string_list_sort(StringList list);

// Forwarded declarations of utilities
size_t allocating_bytes_count(const SizeType capacity);
void move_to_the_fields_block(StringList* list_ptr);
void move_to_the_strings_block(StringList* list_ptr);
void set_fields_block(StringList list);
SizeType* get_size_ptr(StringList list);
SizeType* get_capacity_ptr(StringList list);
SizeType string_list_capacity(StringList list);
ErrorCode extend_string_list(StringList* list_ptr, const SizeType new_capacity);
SizeType next_capacity(const SizeType old_capacity);
ErrorCode place_element(StringList list, const SizeType index, cString str);
void replace_in_string(mString string, cString before, cString after);

// Validators forwarded declarations
ErrorCode validate_input_string_list_ptr(StringList*);
ErrorCode validate_input_string_list(StringList);
ErrorCode validate_input_string(cString);

// Validational decorators

PUBLIC ErrorCode string_list_init(StringList* list_ptr)
{
    return impl_string_list_init(list_ptr);
}

PUBLIC ErrorCode string_list_destroy(StringList* list)
{
    ErrorCode validation_error = validate_input_string_list_ptr(list);

    if (validation_error != ErrorCode::Success)
    {
        return validation_error;
    }

    return impl_string_list_destroy(list);
}

PUBLIC bool string_list_is_empty(StringList list)
{
    ErrorCode validation_error = validate_input_string_list(list);

    if (validation_error != ErrorCode::Success)
    {
        return true;
    }

    return impl_string_list_is_empty(list);
}

PUBLIC ErrorCode string_list_add(StringList* list_ptr, cString str)
{
    ErrorCode list_ptr_validation_error = validate_input_string_list_ptr(list_ptr);
    ErrorCode string_validation_error = validate_input_string(str);

    if (list_ptr_validation_error != ErrorCode::Success)
    {
        return list_ptr_validation_error;
    }

    if (string_validation_error != ErrorCode::Success)
    {
        return string_validation_error;
    }

    return impl_string_list_add(list_ptr, str);
}

PUBLIC ErrorCode string_list_remove(StringList list, cString str)
{
    ErrorCode list_validation_error = validate_input_string_list(list);
    ErrorCode string_validation_error = validate_input_string(str);

    if (list_validation_error != ErrorCode::Success)
    {
        return list_validation_error;
    }

    if (string_validation_error != ErrorCode::Success)
    {
        return string_validation_error;
    }

    return impl_string_list_remove(list, str);
}

PUBLIC SizeType string_list_size(StringList list)
{
    ErrorCode list_validation_error = validate_input_string_list(list);

    if (list_validation_error != ErrorCode::Success)
    {
        const SizeType default_size = 0u;
        return default_size;
    }

    return impl_string_list_size(list);
}

PUBLIC SizeType string_list_index_of(StringList list, cString str)
{
    ErrorCode list_validation_error = validate_input_string_list(list);
    ErrorCode string_validation_error = validate_input_string(str);
    const SizeType default_index = (SizeType)(-1);

    if (list_validation_error != ErrorCode::Success)
    {
        return default_index;
    }

    if (string_validation_error != ErrorCode::Success)
    {
        return default_index;
    }

    return impl_string_list_index_of(list, str);
}

PUBLIC ErrorCode string_list_remove_duplicates(StringList* list)
{
    ErrorCode list_ptr_validation_error = validate_input_string_list_ptr(list);

    if (list_ptr_validation_error != ErrorCode::Success)
    {
        return list_ptr_validation_error;
    }

    return impl_string_list_remove_duplicates(list);
}

PUBLIC ErrorCode string_list_replace_in_strings(StringList list, cString before, cString after)
{
    ErrorCode list_validation_error = validate_input_string_list(list);
    ErrorCode string1_validation_error = validate_input_string(before);
    ErrorCode string2_validation_error = validate_input_string(after);

    if (list_validation_error != ErrorCode::Success)
    {
        return list_validation_error;
    }

    if (string1_validation_error != ErrorCode::Success)
    {
        return string1_validation_error;
    }

    if (string2_validation_error != ErrorCode::Success)
    {
        return string2_validation_error;
    }

    return impl_string_list_replace_in_strings(list, before, after);
}

PUBLIC ErrorCode string_list_sort(StringList list)
{
    ErrorCode list_validation_error = validate_input_string_list(list);

    if (list_validation_error != ErrorCode::Success)
    {
        return list_validation_error;
    }

    return impl_string_list_sort(list);
}


// Actual implementations

PRIVATE ErrorCode impl_string_list_init(StringList* list_ptr)
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

PRIVATE ErrorCode impl_string_list_destroy(StringList* list)
{
    for (SizeType i = 0u; i < string_list_size(*list); ++i)
    {
        free((*list)[i]);
    }

    move_to_the_fields_block(list);
    free(*list);
    *list = nullptr;

    return ErrorCode::Success;
}

PRIVATE bool impl_string_list_is_empty(StringList list)
{
    return string_list_size(list) == 0u;
}

PRIVATE ErrorCode impl_string_list_add(StringList* list_ptr, cString str)
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

PRIVATE ErrorCode impl_string_list_remove(StringList list, cString str)
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
        mString read_word = *reading_ptr;

        if (strcmp(read_word, str) != 0)
        {
            *writing_ptr = *reading_ptr;
            ++writing_ptr;
            ++new_size;
        }
        else
        {
            free(read_word);
        }
    }

    SizeType* size_ptr = get_size_ptr(list);
    *size_ptr = new_size;

    return ErrorCode::Success;
}

PRIVATE SizeType impl_string_list_size(StringList list)
{
    return *get_size_ptr(list);
}

PRIVATE SizeType impl_string_list_index_of(StringList list, cString str)
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

PRIVATE ErrorCode impl_string_list_remove_duplicates(StringList* list)
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

PRIVATE ErrorCode impl_string_list_replace_in_strings(StringList list, cString before, cString after)
{
    for (SizeType i = 0u; i < string_list_size(list); ++i)
    {
        replace_in_string(list[i], before, after);
    }

    return ErrorCode::Success;
}

PRIVATE ErrorCode impl_string_list_sort(StringList list)
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


// Additional utilities

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


// Validators implementations

PRIVATE inline ErrorCode validate_not_nullptr(const void* ptr)
{
    return ptr == nullptr
         ? ErrorCode::NullPointerInput
         : ErrorCode::Success;
}

PRIVATE inline ErrorCode validate_input_string_list_ptr(StringList* list_ptr)
{
    return validate_not_nullptr(list_ptr);
}

PRIVATE inline ErrorCode validate_input_string_list(StringList list)
{
    return validate_not_nullptr(list);
}

PRIVATE inline ErrorCode validate_input_string(cString str)
{
    return validate_not_nullptr(str);
}