#ifndef PARSER_ATTRIBUTES_H
#define PARSER_ATTRIBUTES_H

#include "base.h"
#include "const_pool.h"

#include <vector>
#include <memory>

enum class AttrType
{
    ConstantValue,
    Code,
    StackMapTable,
    Exceptions,
    BootstrapMethods,

    Unknown // used for unknown/nonexistent/invalid attribute types
};

AttrType resolve_attr_type(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        u2 attr_name_index
);

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
     * For all attributes, the `attribute_name_index` MUST be a valid unsigned
     * 16-bit index into the `constant_pool` of the class.
     *
     * The `constant_pool` entry at `attribute_name_index` MUST be a
     * `CONSTANT_Utf8_info` structure representing the name of the attribute.
     */
    u2 attr_name_index;

    AttrInfo(u2 attr_name_idx);
};

/*
 * Each entry in the exception_table array describes one exception handler in
 * the code array. The order of the handlers in the exception_table array is
 * significant.
 *
 * The values of the two items `start_pc` and `end_pc` indicate the ranges in the
 * code array at which the exception handler is active. The value of `start_pc`
 * must be a valid index into the code array of the opcode of an instruction.
 * The value of `end_pc` either must be a valid index into the code array of the
 * opcode of an instruction or must be equal to `code_length`, the length of the
 * code array. The value of `start_pc` must be less than the value of `end_pc`.
 *
 * The `start_pc` is inclusive and `end_pc` is exclusive; that is, the exception
 * handler must be active while the program counter is within the interval
 * `[start_pc, end_pc)`.
 */
struct ExceptionTableEntry
{
    u2 start_pc;
    u2 end_pc;

    /*
     * The value of the `handler_pc` item indicates the start of the exception
     * handler. The value of the item must be a valid index into the code array
     * and must be the index of the opcode of an instruction.
     */
    u2 handler_pc;

    /*
     * If the value of the `catch_type` item is nonzero, it must be a valid index
     * into the `constant_pool` table. The `constant_pool` entry at that index
     * must be a `CONSTANT_Class_info` structure representing a class of
     * exceptions that this exception handler is designated to catch. The exception
     * handler will be called only if the thrown exception is an instance of the
     * given class or one of its subclasses.
     *
     * The verifier checks that the class is `Throwable` or a subclass of `Throwable`.
     *
     * If the value of the `catch_type` item is zero, this exception handler is called
     * for all exceptions. This is used to implement `finally`.
     */
    u2 catch_type;
};

/*
 * The Code attribute is a variable-length attribute in the attributes table of
 * a `method_info` structure. A Code attribute contains the Java Virtual
 * Machine instructions and auxiliary information for a method, including an
 * instance initialization method or a class or interface initialization method.
 *
 * If the method is either `native` or `abstract`, its `method_info` structure
 * MUST not have a Code attribute in its attributes table. Otherwise, its
 * `method_info` structure MUST have exactly one Code attribute in its
 * attributes table.
 *
 * The Code attribute has the following format:
 *
 *     Code_attribute {
 *         u2 attribute_name_index;
 *         u4 attribute_length;
 *         u2 max_stack;
 *         u2 max_locals;
 *         u4 code_length;
 *         u1 code[code_length];
 *         u2 exception_table_length;
 *
 *         {
 *             u2 start_pc;
 *             u2 end_pc;
 *             u2 handler_pc;
 *             u2 catch_type;
 *         } exception_table[exception_table_length];
 *
 *         u2 attributes_count;
 *         attribute_info attributes[attributes_count];
 *     }
 */
struct CodeAttrInfo
    : AttrInfo
{
    u2 max_stack;
    u2 max_locals;
    std::vector<u1> code;
    std::vector<ExceptionTableEntry> exception_table;
    std::vector<std::unique_ptr<AttrInfo>> attributes;

    CodeAttrInfo(u2 attr_name_idx);
};

#endif // PARSER_ATTRIBUTES_H
