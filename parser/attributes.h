#ifndef PARSER_ATTRIBUTES_H
#define PARSER_ATTRIBUTES_H

#include "base.h"
#include "const_pool.h"

#include <vector>
#include <memory>

enum class AttrType
{
    // The next five attributes are CRITICAL to correct interpretation of the
    // class file by the Java Virtual Machine:
    ConstantValue,
    Code,
    StackMapTable,
    Exceptions,
    BootstrapMethods,

    // The next twelve attributes are critical to correct interpretation of the
    // class file by the class libraries of the Java SE platform:
    InnerClasses,
    EnclosingMethod,
    Synthetic,
    Signature,
    RuntimeVisibleAnnotations,
    RuntimeInvisibleAnnotations,
    RuntimeVisibleParameterAnnotations,
    RuntimeInvisibleParameterAnnotations,
    RuntimeVisibleTypeAnnotations,
    RuntimeInvisibleTypeAnnotations,
    AnnotationDefault,
    MethodParameters,

    // The next six attributes are NOT critical to correct interpretation of the
    // class file by either the Java Virtual Machine or the class libraries of
    // the Java SE platform, but are useful for tools:
    SourceFile,
    SourceDebugExtension,
    LineNumberTable,
    LocalVariableTable,
    LocalVariableTypeTable,
    Deprecated,

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
 * MUST be a valid index into the code array of the opcode of an instruction.
 * The value of `end_pc` either MUST be a valid index into the code array of the
 * opcode of an instruction or MUST be equal to `code_length`, the length of the
 * code array. The value of `start_pc` MUST be less than the value of `end_pc`.
 *
 * The `start_pc` is inclusive and `end_pc` is exclusive; that is, the exception
 * handler MUST be active while the program counter is within the interval
 * `[start_pc, end_pc)`.
 */
struct ExceptionTableEntry
{
    u2 start_pc;
    u2 end_pc;

    /*
     * The value of the `handler_pc` item indicates the start of the exception
     * handler. The value of the item MUST be a valid index into the code array
     * and MUST be the index of the opcode of an instruction.
     */
    u2 handler_pc;

    /*
     * If the value of the `catch_type` item is nonzero, it MUST be a valid index
     * into the `constant_pool` table. The `constant_pool` entry at that index
     * MUST be a `CONSTANT_Class_info` structure representing a class of
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

    ExceptionTableEntry(u2 start, u2 end, u2 handler, u2 catch_t);
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

struct LineNumberTableEntry
{
    u2 start_pc;
    u2 line_number;

    LineNumberTableEntry(u2 start, u2 line_num);
};

/*
 * The `LineNumberTable` attribute is an optional variable-length attribute
 * in the attributes table of a Code attribute. It may be used by debuggers
 * to determine which part of the code array corresponds to a given line
 * number in the original source file.
 *
 * The `LineNumberTable` attribute has the following format:
 *
 *     LineNumberTable_attribute {
 *         u2 attribute_name_index;
 *         u4 attribute_length;
 *         u2 line_number_table_length;
 *
 *         {
 *             u2 start_pc;
 *             u2 line_number;
 *         } line_number_table[line_number_table_length];
 *     }
 */
struct LineNumberTableAttrInfo
    : AttrInfo
{
    std::vector<LineNumberTableEntry> line_number_table;

    LineNumberTableAttrInfo(u2 attr_name_idx);
};

/*
 * The `SourceFile` attribute is an optional fixed-length attribute in the attributes
 * table of a `ClassFile` structure.
 *
 * There may be at most one `SourceFile` attribute in the attributes table of a
 * `ClassFile` structure.
 *
 * The SourceFile attribute has the following format:
 *
 *     SourceFile_attribute {
 *         u2 attribute_name_index;
 *         u4 attribute_length;
 *         u2 sourcefile_index;
 *     }
 */
struct SourceFileAttrInfo
    : AttrInfo
{
    /*
     * The value of the `sourcefile_index` item MUST be a valid index into the
     * `constant_pool` table. The `constant_pool` entry at that index MUST be a
     * `CONSTANT_Utf8_info` structure representing a string.
     *
     * The string referenced by the `sourcefile_index` item will be interpreted as indicating the
     * name of the source file from which this class file was compiled. It will NOT be interpreted
     * as indicating the name of a directory containing the file or an absolute path name for the file;
     * such platform-specific additional information MUST be supplied by the run-time interpreter
     * or development tool at the time the file name is actually used
     */
    u2 sourcefile_index;

    SourceFileAttrInfo(u2 attr_name_idx);
};

#endif // PARSER_ATTRIBUTES_H
