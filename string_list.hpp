#ifndef STRING_LIST_HPP_
#define STRING_LIST_HPP_

typedef char CharType;
typedef CharType* mString;
typedef const CharType* cString;
typedef mString* StringList;
typedef size_t SizeType;

enum class ErrorCode
{
	Success,
	LackOfMemory,
	NullPointerInput,
};

ErrorCode string_list_init(StringList* list);
ErrorCode string_list_destroy(StringList* list);

ErrorCode string_list_is_empty(StringList list, bool* result);

ErrorCode string_list_add(StringList* list, cString str);
ErrorCode string_list_remove(StringList list, cString str);

ErrorCode string_list_size(StringList list, SizeType* result);
ErrorCode string_list_index_of(StringList list, cString str, SizeType* result);

ErrorCode string_list_remove_duplicates(StringList* list);
ErrorCode string_list_replace_in_strings(StringList list, cString before, cString after);
ErrorCode string_list_sort(StringList list);

#endif // !STRING_LIST_HPP_
