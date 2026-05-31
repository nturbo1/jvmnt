#ifndef CHECK_CLASS_FORMAT_H
#define CHECK_CLASS_FORMAT_H

#include "class_file.h"

#include <cstddef>

/*
 * This header file contains classes/structs, methods, values, etc. that
 * are used for a class file `format checking` process.
 *
 * When a prospective class file is loaded by the Java Virtual Machine, the
 * Java Virtual Machine first ensures that the file has the basic format of
 * a class file. This process is known as format checking. The checks are
 * as follows:
 *
 *     • The first four bytes must contain the right magic number.
 *     • All recognized attributes must be of the proper length.
 *     • The class file must not be truncated or have extra bytes at the end.
 *
 *     • The constant pool must satisfy the constraints documented throughout.
 *          For example, each `CONSTANT_Class_info` structure in the constant pool must contain
 *          in its `name_index` item a valid constant pool index for a `CONSTANT_Utf8_info`
 *          structure.
 *
 *     • All field references and method references in the constant pool must have valid
 *       names, valid classes, and valid descriptors.
 *
 * Format checking does not ensure that the given field or method actually exists
 * in the given class, nor that the descriptors given refer to real classes. Format
 * checking ensures only that these items are well formed. More detailed checking
 * is performed when the bytecodes themselves are verified, and during resolution.
 */



/*
 * Checks all the formatting criteria for a given class file structure.
 *
 * If any of the formattign criteria fails, then throws a `std::runtime_error`
 * exception, which should not be caught leading to the termination of the program.
 */
void check_class_file(const ClassFile& cf);

/*
 * Checks whether a given `index` is a valid index into the given `const_pool`.
 *
 * If the `index` is invalid, then throws a `std::runtime_error` exception,
 * which should not be caught leading to the termination of the program.
 */
void check_const_pool_index(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        std::size_t index);

/*
 * Check whether the `constant_pool` entry at that index is a `CONSTANT_Class_info`
 * structure representing the class or interface defined by this class file.
 *
 * If the entry at that index is of wrong type and/or format, then throws a
 * `std::runtime_error` exception, which should not be caught leading to the
 * termination of the program.
 */
void check_const_pool_index_type_const_class(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        std::size_t index);

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
void check_this_class(const ClassFile& cf);

#endif // CHECK_CLASS_FORMAT_H
