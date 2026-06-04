#ifndef CLASS_FILE_CLASS_FILE_H
#define CLASS_FILE_CLASS_FILE_H

#include "base.h"
#include "constPool.h"
#include "attributes.h"

#include <vector>
#include <memory>
#include <string>

const u4 CLASS_FILE_MAGIC = 0xCAFEBABE;

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
    u2 accessFlags;

    /*
     * The value of the `name_index` item MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index MUST be a
     * `CONSTANT_Utf8_info` structure which represents a valid unqualified
     * name denoting a field.
     */
    u2 nameIndex;

    /*
     * The value of the `descriptor_index` item MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index MUST be a
     * `CONSTANT_Utf8_info` structure which represents a valid field descriptor.
     */
    u2 descriptorIndex;

    std::vector<std::unique_ptr<AttrInfo>> attributes;

    FieldInfo(
        u2 accessFgs,
        u2 nameIdx,
        u2 descriptorIdx,
        std::vector<std::unique_ptr<AttrInfo>> attributes
    );
};

/*
 * Each method, including each instance initialization method and the class or
 * interface initialization method, is described by a `method_info` structure.
 *
 * No two methods in one class file may have the same name and descriptor.
 *
 * The structure has the following format:
 *
 *     method_info {
 *         u2 access_flags;
 *         u2 name_index;
 *         u2 descriptor_index;
 *         u2 attributes_count;
 *         attribute_info attributes[attributes_count];
 *     }
 */
struct MethodInfo
{
    u2 accessFlags;

    /*
     * The value of the `name_index` item MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index MUST be a
     * `CONSTANT_Utf8_info` structure representing either one of the special
     * method names `<init>` or `<clinit>`, or a valid unqualified name
     * denoting a method.
     */
    u2 nameIndex;

    /*
     * The value of the `descriptor_index` item MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index MUST be
     * a `CONSTANT_Utf8_info` structure representing a valid method descriptor.
     */
    u2 descriptorIndex;

    std::vector<std::unique_ptr<AttrInfo>> attributes;

    MethodInfo(
        u2 accessFgs,
        u2 nameIdx,
        u2 descriptorIdx,
        std::vector<std::unique_ptr<AttrInfo>> attributes
    );
};

/*
 * The `ClassFile` class contains information about the `ClassFile` structure,
 * which is the layout of a `.class` file, and some additional related information.
 *
 * The `ClassFile` structure content:
 *
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
class ClassFile
{
public:
    /*
     * ===============================================================================
     * ==================== The `ClassFile` structure information ====================
     * ===============================================================================
     */

    const u4 m_magic;
    const u2 m_minorVersion;
    const u2 m_majorVersion;

    const std::vector<std::unique_ptr<ConstPoolEntry>> m_constPool;

    const u2 m_accessFlags;

    /*
     * The value of the `this_class` item MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index MUST be a
     * `CONSTANT_Class_info` structure representing the class or interface
     * defined by this class file.
     */
    const u2 m_thisClass;

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
    const u2 m_superClass;

    /*
     * Each value in the `interfaces` array MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at each value of
     * `interfaces[i]`, where `0 ≤ i < interfaces_count`, MUST be a
     * `CONSTANT_Class_info` structure representing an interface that is a direct
     * superinterface of this class or interface type, in the left-to-right order
     * given in the source for the type.
     */
    const std::vector<u2> m_interfaces;
    const std::vector<FieldInfo> m_fields;
    const std::vector<MethodInfo> m_methods;
    const std::vector<std::unique_ptr<AttrInfo>> m_attributes;

    /*
     * ===============================================================================
     * =========================== Some additional stuff =============================
     * ===============================================================================
     */

    const std::string m_filename; // name of the class file

    /*
     * Checks all the formatting criteria for a given class file structure.
     *
     * If any of the formattign criteria fails, then throws a `std::runtime_error`
     * exception, which should not be caught leading to the termination of the program.
     */
    void formatCheck();

    /*
     * Checks if:
     *     - the value of the `this_class` item is a valid index into the
     *       `constant_pool` table.
     *
     *     - the `constant_pool` entry at that index is a `CONSTANT_Class_info`
     *       structure representing the class or interface defined by this class file.
     *
     * If any of the above checks fails, then throws a `std::runtime_error` exception,
     * which should not be caught leading to the termination of the program.
     */
    void checkThisClass();

    /*
     * Checks if:
     *      - the value of the `super_class` item is either zero or a valid index into
     *        the `constant_pool` table.
     *
     *      - in case the value of the `super_class` item is nonzero, the `constant_pool`
     *        entry at that index is a `CONSTANT_Class_info` structure representing
     *        the direct superclass of the class defined by this class file.
     *
     *        Neither the
     *        direct superclass nor any of its superclasses may have the `ACC_FINAL` flag
     *        set in the `access_flags` item of its `ClassFile` structure.
     *
     *      - if the value of the `super_class` item is zero, then this class file MUST represent
     *        the class `Object`, the only class or interface without a direct superclass.
     *
     *      - for an interface, the value of the `super_class` item MUST always be a valid
     *        index into the `constant_pool` table. The `constant_pool` entry at that index
     *        MUST be a `CONSTANT_Class_info` structure representing the class `Object`.
     */
    void checkSuperClass();

    /*
     * Checks if:
     *      - Each value in the `interfaces` array is a valid index into the
     *        `constant_pool` table.
     *      - the `constant_pool` entry at each value of `interfaces[i]`, where
     *        `0 ≤ i < interfaces_count`, is a `CONSTANT_Class_info` structure representing
     *        an interface that is a direct superinterface of this class or interface
     *        type, in the left-to-right order given in the source for the type.
     */
    void checkInterfaces();

    /*
     * Checks whether a given `index` is a valid index into the given `const_pool`.
     *
     * If the `index` is invalid, then throws a `std::runtime_error` exception,
     * which should not be caught leading to the termination of the program.
     */
    void checkConstPoolIndex(std::size_t index);

    /*
     * Check whether the `constant_pool` entry at that index is a `CONSTANT_Class_info`
     * structure representing the class or interface defined by this class file.
     *
     * If the entry at that index is of wrong type and/or format, then throws a
     * `std::runtime_error` exception, which should not be caught leading to the
     * termination of the program.
     */
    void checkConstPoolIndexTypeConstClass(std::size_t index);

    /*
     * Check whether the `constant_pool` entry at that index is a `CONSTANT_Utf8_info`
     * structure.
     *
     * If the entry at that index is of wrong type and/or format, then throws a
     * `std::runtime_error` exception, which should not be caught leading to the
     * termination of the program.
     */
    void checkConstPoolIndexTypeConstUtf8(std::size_t index);
};

enum class AccMasks
{
    ACC_PUBLIC = 0x0001, // Declared public; may be accessed from outside its
                        // package.

    ACC_FINAL = 0x0010, // Declared final; no subclasses allowed.

    ACC_SUPER = 0x0020, // Treat superclass methods specially when invoked by
                        // the invokespecial instruction.

    ACC_INTERFACE = 0x0200, // Is an interface, not a class.

    ACC_ABSTRACT = 0x0400, // Declared abstract; must not be instantiated.

    ACC_SYNTHETIC = 0x1000, // Declared synthetic; not present in the source code.

    ACC_ANNOTATION = 0x2000, // Declared as an annotation type.

    ACC_ENUM = 0x4000 // Declared as an enum type.
};

#endif // CLASS_FILE_CLASS_FILE_H
