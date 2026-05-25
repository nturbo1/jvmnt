#include "parser.h"

#include "class_file.h"
#include "base.h"
#include "class_file_validation.h"
#include "attributes.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <cstddef>

ClassFileParser::ClassFileParser(ClassFileReader& reader)
    : m_reader{ reader }
{}

ClassFile ClassFileParser::parse()
{
    u4 magic{ m_reader.read_u4() };
    // TODO: Validate the magic number!

    u2 minor_version{ m_reader.read_u2() };
    u2 major_version{ m_reader.read_u2() };
    // TODO: Validate the version support!

    std::vector<std::unique_ptr<ConstPoolEntry>> const_pool{ parse_const_pool() };
    u2 access_flags{ m_reader.read_u2() };
    u2 this_class{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - The value of the `this_class` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Class_info` structure representing the class or interface
    //         defined by this class file.

    u2 super_class{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - for a class, the value of the `super_class` item either MUST be zero or
    //         MUST be a valid index into the `constant_pool` table.
    //
    //       - if the value of the `super_class` item is nonzero, the `constant_pool`
    //         entry at that index MUST be a `CONSTANT_Class_info` structure representing
    //         the direct superclass of the class defined by this class file. Neither the
    //         direct superclass nor any of its superclasses may have the `ACC_FINAL` flag
    //         set in the `access_flags` item of its `ClassFile` structure.
    //
    //       - if the value of the `super_class` item is zero, then this class file MUST represent
    //         the class `Object`, the only class or interface without a direct superclass.
    //
    //       - for an interface, the value of the `super_class` item MUST always be a valid
    //         index into the `constant_pool` table. The `constant_pool` entry at that index
    //         MUST be a `CONSTANT_Class_info` structure representing the class `Object`.

    u2 interfaces_count{ m_reader.read_u2() };
    std::vector<u2> interfaces;
    interfaces.reserve(interfaces_count);
    for (u2 i{0}; i < interfaces_count; ++i)
    {
        u2 intface{ m_reader.read_u2() };
        // TODO: VALIDATE that:
        //       - each value in the `interfaces` array must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at each value of `interfaces[i]`, where
        //         `0 ≤ i < interfaces_count`, must be a `CONSTANT_Class_info` structure
        //         representing an interface that is a direct superinterface of this
        //         class or interface type, in the left-to-right order given in the
        //         source for the type.

        interfaces.push_back(intface);
    }

    std::vector<FieldInfo> fields{ parse_fields(const_pool) };
    std::vector<MethodInfo> methods{ parse_methods(const_pool) };
    std::vector<std::unique_ptr<AttrInfo>> attributes{ parse_attributes(const_pool) };

    return ClassFile{
            magic,
            minor_version,
            major_version,
            std::move(const_pool),
            access_flags,
            this_class,
            super_class,
            interfaces,
            std::move(fields),
            std::move(methods),
            std::move(attributes)
    };
}

ConstPoolEntryTag matchConstPoolEntryTag(u1 tag)
{
    ConstPoolEntryTag t{ static_cast<ConstPoolEntryTag>(tag) };
    switch(t) {
    case ConstPoolEntryTag::CONSTANT_Class:
        return ConstPoolEntryTag::CONSTANT_Class;

    case ConstPoolEntryTag::CONSTANT_Fieldref:
        return ConstPoolEntryTag::CONSTANT_Fieldref;

    case ConstPoolEntryTag::CONSTANT_Methodref:
        return ConstPoolEntryTag::CONSTANT_Methodref;

    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        return ConstPoolEntryTag::CONSTANT_InterfaceMethodref;

    case ConstPoolEntryTag::CONSTANT_String:
        return ConstPoolEntryTag::CONSTANT_String;

    case ConstPoolEntryTag::CONSTANT_Integer:
        return ConstPoolEntryTag::CONSTANT_Integer;

    case ConstPoolEntryTag::CONSTANT_Float:
        return ConstPoolEntryTag::CONSTANT_Float;

    case ConstPoolEntryTag::CONSTANT_Long:
        return ConstPoolEntryTag::CONSTANT_Long;

    case ConstPoolEntryTag::CONSTANT_Double:
        return ConstPoolEntryTag::CONSTANT_Double;

    case ConstPoolEntryTag::CONSTANT_NameAndType:
        return ConstPoolEntryTag::CONSTANT_NameAndType;

    case ConstPoolEntryTag::CONSTANT_Utf8:
        return ConstPoolEntryTag::CONSTANT_Utf8;

    case ConstPoolEntryTag::CONSTANT_MethodHandle:
        return ConstPoolEntryTag::CONSTANT_MethodHandle;

    case ConstPoolEntryTag::CONSTANT_MethodType:
        return ConstPoolEntryTag::CONSTANT_MethodType;

    case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
        return ConstPoolEntryTag::CONSTANT_InvokeDynamic;

    default:
        return ConstPoolEntryTag::INVALID;
    }
}

std::vector<std::unique_ptr<ConstPoolEntry>> ClassFileParser::parse_const_pool()
{
    u2 const_pool_count{ m_reader.read_u2() };

    std::vector<std::unique_ptr<ConstPoolEntry>> const_pool;
    // cp_info constant_pool[constant_pool_count-1]
    const_pool.reserve(const_pool_count - 1);

    // `constant_pool` indexes are in the range `1, 2, ..., constant_pool_count - 1`.
    for (int i{1}; i < const_pool_count; ++i)
    {
        u1 t{ m_reader.read_u1() };
        ConstPoolEntryTag tag{ matchConstPoolEntryTag(t) };

        if (tag == ConstPoolEntryTag::INVALID)
            log_fatal("Constant pool entry at index %d has an invalid tag.", i);

        switch(tag)
        {
        case ConstPoolEntryTag::CONSTANT_Class:
            const_pool.push_back(parse_const_class_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Fieldref:
            const_pool.push_back(parse_const_fieldref_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Methodref:
            const_pool.push_back(parse_const_methodref_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
            log_fixme("Implement constant pool entry 'CONSTANT_InterfaceMethodref' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_String:
            const_pool.push_back(parse_const_string_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Integer:
            log_fixme("Implement constant pool entry 'CONSTANT_Integer' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_Float:
            log_fixme("Implement constant pool entry 'CONSTANT_Float' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_Long:
            log_fixme("Implement constant pool entry 'CONSTANT_Long' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_Double:
            const_pool.push_back(parse_const_double_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_NameAndType:
            const_pool.push_back(parse_const_nameandtype_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Utf8:
            const_pool.push_back(parse_const_utf8_info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_MethodHandle:
            log_fixme("Implement constant pool entry 'CONSTANT_MethodHandle' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_MethodType:
            log_fixme("Implement constant pool entry 'CONSTANT_MethodType' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
            const_pool.push_back(parse_const_invoke_dynamic_info(tag));
            break;

        default:
            assert(false && "The constant pool entry tag was not validated beforehand!");
        }
    }

    return const_pool;
}

std::unique_ptr<ConstMethodrefInfo> ClassFileParser::parse_const_methodref_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Methodref &&
            "Non 'CONSTANT_Methodref' constant pool entry tag value was passed to the CONSTANT_Methodref_info parser");

    u2 class_index{ m_reader.read_u2() };
    // TODO: VALIDATE that the value of the `class_index` item must be a valid index into the
    //       `constant_pool` table. The `constant_pool` entry at that index must be a
    //       `CONSTANT_Class_info` structure representing a class, NOT an interface, type
    //       that has the method as a member.

    u2 name_and_type_index{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - the value of the `name_and_type_index` item must be a valid index into
    //         the `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_NameAndType_info` structure. This `constant_pool` entry indicates
    //         the name and descriptor of the method.
    //
    //       - if the name of the method begins with a '<' ('\u003c'), then the name must
    //         be the special name `<init>`, representing an instance initialization method.
    //         The return type of such a method must be `void`.

    return std::make_unique<ConstMethodrefInfo>(tag, class_index, name_and_type_index);
}

std::unique_ptr<ConstFieldrefInfo> ClassFileParser::parse_const_fieldref_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Fieldref &&
            "Non 'CONSTANT_Fieldref' constant pool entry tag value was passed to the CONSTANT_Fieldref_info parser");

    u2 class_index{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - the value of the `class_index` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Class_info` structure representing a class or an interface type
    //         that has the field as a member.

    u2 name_and_type_index{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - the value of the `name_and_type_index` item must be a valid index into
    //         the `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_NameAndType_info` structure. This `constant_pool` entry indicates
    //         the name and descriptor of the field. The descriptor MUST be a field descriptor.

    return std::make_unique<ConstFieldrefInfo>(tag, class_index, name_and_type_index);
}

std::unique_ptr<ConstStringInfo> ClassFileParser::parse_const_string_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_String &&
            "Non 'CONSTANT_String' constant pool entry tag value was passed to the CONSTANT_String_info parser");

    u2 string_index{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - the value of the `string_index` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Utf8_info` structure representing the sequence of Unicode
    //         code points to which the String object is to be initialized.

    return std::make_unique<ConstStringInfo>(tag, string_index);
}

std::unique_ptr<ConstDoubleInfo> ClassFileParser::parse_const_double_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Double &&
            "Non 'CONSTANT_Double' constant pool entry tag value was passed to the CONSTANT_Double_info parser");
    u4 high_bytes{ m_reader.read_u4() };
    u4 low_bytes{ m_reader.read_u4() };

    return std::make_unique<ConstDoubleInfo>(tag, high_bytes, low_bytes);
}

std::unique_ptr<ConstInvokeDynamicInfo>
ClassFileParser::parse_const_invoke_dynamic_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_InvokeDynamic &&
            "Non 'CONSTANT_InvokeDynamic' constant pool entry tag value was passed to the"
            "CONSTANT_InvokeDynamic_info parser");

    u2 bootstrap_method_attr_index{ m_reader.read_u2() };
    // TODO: VALIDATE that the `bootstrap_method_attr_index` item must be a valid index into the
    //       `bootstrap_methods` array of the bootstrap method table of this class file!

    u2 name_and_type_index{ m_reader.read_u2() };
    // TODO: VALIDATE that the value of the `name_and_type_index` item must be a valid index into the
    //       `constant_pool` table. The `constant_pool` entry at that index must be a
    //       `CONSTANT_NameAndType_info` structure representing a method name and method descriptor.

    return std::make_unique<ConstInvokeDynamicInfo>(tag, bootstrap_method_attr_index, name_and_type_index);
}

std::unique_ptr<ConstClassInfo> ClassFileParser::parse_const_class_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Class &&
            "Non 'CONSTANT_Class' constant pool entry tag value was passed to the 'CONSTANT_Class_info' parser");

    u2 name_index{ m_reader.read_u2() };
    // TODO: VALIDATE the value of the `name_index` item must be a valid index into
    //       the `constant_pool` table. The `constant_pool` entry at that index must be a
    //       `CONSTANT_Utf8_info` structure representing a valid binary class or interface
    //       name encoded in internal form.

    return std::make_unique<ConstClassInfo>(tag, name_index);
}

std::unique_ptr<ConstUtf8Info> ClassFileParser::parse_const_utf8_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Utf8 &&
            "Non 'CONSTANT_Utf8' constant pool entry tag value was passed to the 'CONSTANT_Utf8_info' parser");

    u2 length{ m_reader.read_u2() };
    std::vector<u1> utf8_bytes;
    utf8_bytes.reserve(length);
    for (std::size_t i{0}; i < length; ++i)
    {
        u1 b{ m_reader.read_u1() };

        if (b == 0)
            log_fatal("`Constant UTF8` bytes cannot be `0`!");
        if (0xf0 <= b && b <= 0xff)
            log_fatal("`Constant UTF8` bytes cannot lie in the range (byte)`0xf0` to (byte)`0xff`!");

        utf8_bytes.push_back(b);
    }

    return std::make_unique<ConstUtf8Info>(tag, utf8_bytes);
}

std::unique_ptr<ConstNameAndTypeInfo>
ClassFileParser::parse_const_nameandtype_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_NameAndType &&
            "Non 'CONSTANT_NameAndType' constant pool entry tag value was passed"
            "to the 'CONSTANT_NameAndType_info' parser");

    u2 name_index{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - the value of the `name_index` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Utf8_info` structure representing either the special method
    //         name `<init>` or a valid unqualified name denoting a field or method.

    u2 descriptor_index{ m_reader.read_u2() };
    // TODO: VALIDATE that:
    //       - the value of the `descriptor_index` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Utf8_info` structure representing a valid field descriptor
    //         or method descriptor.

    return std::make_unique<ConstNameAndTypeInfo>(tag, name_index, descriptor_index);
}

std::unique_ptr<AttrInfo>
ClassFileParser::parse_attr(const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    u2 attr_name_index{ m_reader.read_u2() };
    validate_const_pool_index(const_pool, attr_name_index);

    AttrType attr_type{ resolve_attr_type(const_pool, attr_name_index) };

    switch (attr_type)
    {
    case AttrType::ConstantValue:
        log_fixme("IMPLEMENT ConstantValue parser!!!");
        break;

    case AttrType::Code:
    {
        std::unique_ptr<CodeAttrInfo> code_attr{ std::make_unique<CodeAttrInfo>(attr_name_index) };
        parse_attr_code(*code_attr, const_pool);
        return code_attr;
    }

    case AttrType::StackMapTable:
        log_fixme("IMPLEMENT StackMapTable parser!!!");
        break;

    case AttrType::Exceptions:
        log_fixme("IMPLEMENT Exceptions parser!!!");
        break;

    case AttrType::BootstrapMethods:
        log_fixme("IMPLEMENT BootstrapMethods parser!!!");
        break;

    default:
        log_fatal("Unknown `AttrType` enum value: %d", attr_type);
    }
}

std::vector<std::unique_ptr<AttrInfo>>
ClassFileParser::parse_attributes(const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    u2 attributes_count{ m_reader.read_u2() };
    std::vector<std::unique_ptr<AttrInfo>> attributes;
    attributes.reserve(attributes_count);

    for (u2 i{0}; i < attributes_count; ++i)
    {
        attributes.push_back(parse_attr(const_pool));
    }

    return attributes;
}

std::vector<FieldInfo> ClassFileParser::parse_fields(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    u2 fields_count{ m_reader.read_u2() };
    std::vector<FieldInfo> fields;
    fields.reserve(fields_count);

    for (u2 i{0}; i < fields_count; ++i)
    {
        u2 access_flags{ m_reader.read_u2() };
        u2 name_index{ m_reader.read_u2() };
        // TODO: VALIDATE that:
        //       - the value of the `name_index` item must be a valid index into the
        //        `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //        structure which represents a valid unqualified name denoting a field.

        u2 descriptor_index{ m_reader.read_u2() };
        // TODO: VALIDATE that:
        //       - the value of the `descriptor_index` item must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //         structure which represents a valid field descriptor.

        std::vector<std::unique_ptr<AttrInfo>> attributes{ parse_attributes(const_pool) };
        fields.emplace_back(access_flags, name_index, descriptor_index, std::move(attributes));
    }

    return fields;
}

std::vector<MethodInfo>
ClassFileParser::parse_methods(const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    u2 methods_count{ m_reader.read_u2() };
    std::vector<MethodInfo> methods;
    methods.reserve(methods_count);

    for (u2 i = 0; i < methods_count; ++i)
    {
        u2 access_flags{ m_reader.read_u2() };
        u2 name_index{ m_reader.read_u2() };
        // TODO: VALIDATE that:
        //       - the value of the `name_index` item must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //         structure representing either one of the special method names `<init>`
        //         or `<clinit>`, or a valid unqualified name denoting a method.

        u2 descriptor_index{ m_reader.read_u2() };
        // TODO: VALIDATE that:
        //       - the value of the `descriptor_index` item must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //         structure representing a valid method descriptor.

        std::vector<std::unique_ptr<AttrInfo>> attributes{ parse_attributes(const_pool) };
        methods.emplace_back(access_flags, name_index, descriptor_index, std::move(attributes));
    }

    return methods;
}

// It's assumed that the `attribute_name_index` field bytes have been read and processed
// to determine the specific type of the attribute, so those bytes are skipped.
void ClassFileParser::parse_attr_code(
        CodeAttrInfo& code_attr,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    m_reader.read_u4(); // deliberately skipping `attribute_length`

    u2 max_stack{ m_reader.read_u2() };
    u2 max_locals{ m_reader.read_u2() };

    u4 code_length{ m_reader.read_u4() };
    std::vector<u1> code;
    code.reserve(code_length);
    for (u4 i = 0; i < code_length; i++)
    {
        code.push_back(m_reader.read_u1());
    }

    code_attr.max_stack = max_stack;
    code_attr.max_locals = max_locals;
    code_attr.code = code;
    code_attr.exception_table = parse_exception_table();
    code_attr.attributes = parse_attributes(const_pool);
}

std::vector<ExceptionTableEntry> ClassFileParser::parse_exception_table()
{
    log_fixme("IMPLEMENT exception table parser!!!");
    // u2 exception_table_length{ m_reader.read_u2() };
    // for (u2 i = 0; i < exception_table_length; i++)
    // {
    // }
    return std::vector<ExceptionTableEntry>();
}
