#ifndef PARSER_CONST_POOL_H
#define PARSER_CONST_POOL_H

#include "base.h"

#include <ostream>

enum class ConstPoolEntryTag
{
    INVALID = -1,
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18,
};

std::ostream& operator<<(std::ostream& os, const ConstPoolEntryTag& t);

/*
 * The base class for the `constant_pool` items in a `class file`.
 */
struct ConstPoolEntry
{
    /*
     * Each item in the constant_pool table must begin with a 1-byte tag indicating
     * the kind of cp_info entry.
     */
    const ConstPoolEntryTag tag;

    ConstPoolEntry(ConstPoolEntryTag t);

    virtual ~ConstPoolEntry() = default;
};

/*
 * The ConstClassInfo structure is used to represent a class or an interface.
 */
struct ConstClassInfo
    : ConstPoolEntry
{
    /*
     * The value of the name_index item must be a valid index into the `constant_pool`
     * table. The `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
     * structure representing a valid binary class or interface name encoded in
     * internal form.
     */
    const u2 name_index;
};

struct ConstLongInfo
    : ConstPoolEntry
{
    u4 high_bytes;
    u4 low_bytes;
};

struct ConstDoubleInfo
    : ConstPoolEntry
{
    u4 high_bytes;
    u4 low_bytes;

    ConstDoubleInfo(ConstPoolEntryTag t, u4 h_bytes, u4 l_bytes);
};

#endif // PARSER_CONST_POOL_H
