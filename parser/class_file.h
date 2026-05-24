#ifndef PARSER_CLASS_FILE_H
#define PARSER_CLASS_FILE_H

#include "base.h"
#include "const_pool.h"

#include <vector>
#include <memory>

const u4 CLASS_FILE_MAGIC = 0xCAFEBABE;

/*
 * Attributes are used in the `ClassFile`, `field_info`, `method_info`, and
 * `Code_attribute` structures of the class file format.
 *
 * All attributes have the following general format:
 *
 *     attribute_info {
 *         u2 attribute_name_index;
 *         u4 attribute_length;
 *         u1 info[attribute_length];
 *     }
 */
struct AttrInfo
{
    /*
     * For all attributes, the `attribute_name_index` must be a valid unsigned
     * 16-bit index into the `constant_pool` of the class.
     *
     * The `constant_pool` entry at `attribute_name_index` must be a
     * `CONSTANT_Utf8_info` structure representing the name of the attribute.
     */
    u2 attr_name_index;
};

/*
 * Each field is described by a `field_info` structure.
 * No two fields in one class file may have the same name and descriptor.
 *
 * The structure has the following format:
 *
 *     field_info {
 *         u2 access_flags;
 *         u2 name_index;
 *         u2 descriptor_index;
 *         u2 attributes_count;
 *         attribute_info attributes[attributes_count];
 *     }
 */
struct FieldInfo
{
    /*
     * The value of the access_flags item is a mask of flags used to denote
     * access permission to and properties of this field.
     */
    u2 access_flags;

    /*
     * The value of the `name_index` item must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_Utf8_info` structure which represents a valid unqualified
     * name denoting a field.
     */
    u2 name_index;

    /*
     * The value of the `descriptor_index` item must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_Utf8_info` structure which represents a valid field descriptor.
     */
    u2 descriptor_index;

    std::vector<std::unique_ptr<AttrInfo>> attributes;

    FieldInfo(
        u2 access_fgs,
        u2 name_idx,
        u2 descriptor_idx,
        std::vector<std::unique_ptr<AttrInfo>> attributes
    );
};

struct MethodInfo
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
 *     u2 attributes_count;
 *     attribute_info attributes[attributes_count];
 * }
 */
struct ClassFile
{
    const u4 magic;
    const u2 minor_version;
    const u2 major_version;

    const std::vector<std::unique_ptr<ConstPoolEntry>> const_pool;

    const u2 access_flags;

    /*
     * The value of the `this_class` item must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index must be a
     * `CONSTANT_Class_info` structure representing the class or interface
     * defined by this class file.
     */
    const u2 this_class;

    /*
     * For a class, the value of the `super_class` item either MUST be zero or
     * MUST be a valid index into the `constant_pool` table.
     *
     * If the value of the `super_class` item is nonzero, the `constant_pool`
     * entry at that index MUST be a `CONSTANT_Class_info` structure representing
     * the direct superclass of the class defined by this class file. Neither the
     * direct superclass nor any of its superclasses may have the `ACC_FINAL` flag
     * set in the `access_flags` item of its `ClassFile` structure.
     *
     * If the value of the `super_class` item is zero, then this class file MUST represent
     * the class `Object`, the only class or interface without a direct superclass.
     *
     * For an interface, the value of the `super_class` item MUST always be a valid
     * index into the `constant_pool` table. The `constant_pool` entry at that index
     * MUST be a `CONSTANT_Class_info` structure representing the class `Object`.
     */
    const u2 super_class;

    /*
     * Each value in the `interfaces` array must be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at each value of
     * `interfaces[i]`, where `0 ≤ i < interfaces_count`, must be a
     * `CONSTANT_Class_info` structure representing an interface that is a direct
     * superinterface of this class or interface type, in the left-to-right order
     * given in the source for the type.
     */
    const std::vector<u2> interfaces;
    const std::vector<FieldInfo> fields;
    const std::vector<MethodInfo> methods;
    const std::vector<AttrInfo> attributes;
};

#endif // PARSER_CLASS_FILE_H
