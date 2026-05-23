#ifndef PARSER_CONST_POOL_H
#define PARSER_CONST_POOL_H

#include "base.h"

#include <ostream>
#include <vector>

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

struct ConstMethodrefInfo
    : ConstPoolEntry
{
    /*
     * The value of the `class_index` item must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_Class_info` structure representing a class, NOT an interface, type
     * that has the method as a member.
     */
    u2 class_index;

    /*
     * The value of the `name_and_type_index` item must be a valid index into
     * the `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_NameAndType_info` structure. This `constant_pool` entry indicates
     * the name and descriptor of the method. The descriptor MUST be a method descriptor.
     *
     * If the name of the method begins with a '<' ('\u003c'), then the name must
     * be the special name `<init>`, representing an instance initialization method.
     * The return type of such a method must be `void`.
     */
    u2 name_and_type_index;

    ConstMethodrefInfo(ConstPoolEntryTag t, u2 class_idx, u2 name_and_type_idx);
};

struct ConstFieldrefInfo
    : ConstPoolEntry
{
    /*
     * The value of the `class_index` item must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_Class_info` structure representing a class or an interface type
     * that has the field as a member.
     */
    u2 class_index;

    /*
     * The value of the `name_and_type_index` item must be a valid index into
     * the `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_NameAndType_info` structure. This `constant_pool` entry indicates
     * the name and descriptor of the field. The descriptor MUST be a field descriptor.
     */
    u2 name_and_type_index;

    ConstFieldrefInfo(ConstPoolEntryTag t, u2 class_idx, u2 name_and_type_idx);
};

struct ConstStringInfo
    : ConstPoolEntry
{
    /*
     * The value of the `string_index` item must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_Utf8_info` structure representing the sequence of Unicode
     * code points to which the String object is to be initialized.
     */
    u2 string_index;

    ConstStringInfo(ConstPoolEntryTag t, u2 string_idx);
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

    ConstClassInfo(ConstPoolEntryTag t, u2 name_idx);
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

struct ConstInvokeDynamicInfo
    : ConstPoolEntry
{
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;

    ConstInvokeDynamicInfo(ConstPoolEntryTag t, u2 bootstrp_method_attr_idx, u2 name_and_type_idx);
};

/*
 * The `ConstUtf8Info` structure is used to represent constant string values.
 */
struct ConstUtf8Info
    : ConstPoolEntry
{
    /*
     * The bytes array contains the bytes of the string.
     * No byte may have the value (byte)`0`.
     * No byte may lie in the range (byte)`0xf0` to (byte)`0xff`.
     */
    std::vector<u1> bytes;

    ConstUtf8Info(ConstPoolEntryTag t, std::vector<u1> bs);
};

#endif // PARSER_CONST_POOL_H
