#ifndef PARSER_CLASS_FILE_H
#define PARSER_CLASS_FILE_H

#include "base.h"

#include <vector>

const u4 CLASS_FILE_MAGIC = 0xCAFEBABE;

struct ConstPoolEntry
{
    virtual ~ConstPoolEntry() = default;
};

struct FieldInfo
{};

struct MethodInfo
{};

struct AttrInfo
{};

/*
 * ClassFile {
 *     u4 magic;
 *     u2 minor_version;
 *     u2 major_version;
 *     u2 constant_pool_count;
 *     cp_info constant_pool[constant_pool_count-1];
 *     u2 access_flags;
 *     u2 this_class;
 *     u2 super_class;
 *     u2 interfaces_count;
 *     u2 interfaces[interfaces_count];
 *     u2 fields_count;
 *     field_info fields[fields_count];
 *     u2 methods_count;
 *     method_info methods[methods_count];
 *     2 attributes_count;
 *     attribute_info attributes[attributes_count];
 * }
 */
struct ClassFile
{
    const u4 magic;
    const u2 minor_version;
    const u2 major_version;

    const std::vector<ConstPoolEntry> const_pool;

    const u2 access_flags;
    const u2 this_class;
    const u2 super_class;

    const std::vector<u2> interfaces;
    const std::vector<FieldInfo> fields;
    const std::vector<MethodInfo> methods;
    const std::vector<AttrInfo> attributes;
};

#endif // PARSER_CLASS_FILE_H
