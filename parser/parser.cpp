#include "parser.h"

#include "class_file.h"
#include "base.h"

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

    return ClassFile{
            magic,
            minor_version,
            major_version,
            std::move(const_pool)
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

    m_reader.read_u1(); // Skipping a byte because it is 0 for some reason even though
                        // it should not be 0.

    std::vector<std::unique_ptr<ConstPoolEntry>> const_pool;
    // cp_info constant_pool[constant_pool_count-1]
    const_pool.reserve(const_pool_count - 1);
    for (int i = 1; i < const_pool_count; i++)
    {
        u1 t{ m_reader.read_u1() };
        ConstPoolEntryTag tag{ matchConstPoolEntryTag(t) };

        // TODO: Remove the below if statement when you're done!!!
        if (i == 12)
        {
            std::cout << "i = " << i << "\ntag = " << tag << "\n";
            log_info("t = %d\n", t);
            break;
        }

        if (tag == ConstPoolEntryTag::INVALID)
            log_fatal("Constant pool entry at index %d has an invalid tag.", i);

        switch(tag)
        {
        case ConstPoolEntryTag::CONSTANT_Class:
            const_pool.push_back(parse_const_class_info(tag));
            break;
        case ConstPoolEntryTag::CONSTANT_Fieldref:
            log_fixme("Implement constant pool entry 'CONSTANT_Fieldref' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Methodref:
            log_fixme("Implement constant pool entry 'CONSTANT_Methodref' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
            log_fixme("Implement constant pool entry 'CONSTANT_InterfaceMethodref' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_String:
            log_fixme("Implement constant pool entry 'CONSTANT_String' parser!");
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
            log_fixme("Implement constant pool entry 'CONSTANT_NameAndType' parser!");
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

std::unique_ptr<ConstDoubleInfo> ClassFileParser::parse_const_double_info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Double &&
            "Non 'CONSTANT_Double' constant pool entry tag value was passed to the CONSTANT_Double_info parser");
    u4 high_bytes{ m_reader.read_u4() };
    u4 low_bytes{ m_reader.read_u4() };

    return std::make_unique<ConstDoubleInfo>(tag, high_bytes, low_bytes);
}

std::unique_ptr<ConstInvokeDynamicInfo> ClassFileParser::parse_const_invoke_dynamic_info(ConstPoolEntryTag tag)
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
    for (std::size_t i = 0; i < length; i++)
    {
        u1 b{ m_reader.read_u1() };
        // TODO: VALIDATE that:
        //          - no byte may have the value (byte)`0`.
        //          - no byte may lie in the range (byte)`0xf0` to (byte)`0xff`
        utf8_bytes.push_back(b);
    }

    return std::make_unique<ConstUtf8Info>(tag, utf8_bytes);
}

static void print(std::ostream& os, const ConstPoolEntry& e, const std::string& indent)
{
    os << indent << "{" << "\n"
       << indent << "\ttag: " << e.tag << ",\n";

    switch(e.tag)
    {
    case ConstPoolEntryTag::CONSTANT_Class:
    {
        const ConstClassInfo& cci = static_cast<const ConstClassInfo&>(e);
        os << std::dec
           << indent << "\tname_index: " << cci.name_index << ",\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Fieldref:
        break;
    case ConstPoolEntryTag::CONSTANT_Methodref:
        break;
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        break;
    case ConstPoolEntryTag::CONSTANT_String:
        break;
    case ConstPoolEntryTag::CONSTANT_Integer:
        break;
    case ConstPoolEntryTag::CONSTANT_Float:
        break;
    case ConstPoolEntryTag::CONSTANT_Long:
        break;
    case ConstPoolEntryTag::CONSTANT_Double:
    {
        const ConstDoubleInfo& cdi = static_cast<const ConstDoubleInfo&>(e);
        os << std::hex
           << indent << "\thigh_bytes: " << cdi.high_bytes << ",\n"
           << indent << "\tlow_bytes: " << cdi.low_bytes << "\n"
           << std::dec;
        break;
    }
    case ConstPoolEntryTag::CONSTANT_NameAndType:
        break;
    case ConstPoolEntryTag::CONSTANT_Utf8:
    {
        const ConstUtf8Info& cutf8i = static_cast<const ConstUtf8Info&>(e);
        os << std::hex << indent << "\tbytes: [ ";
        for (std::size_t i = 0; i < cutf8i.bytes.size(); i++)
        {
           os << cutf8i.bytes[i] << " ";
        }
        os << "]\n" << std::dec;
        break;
    }
    case ConstPoolEntryTag::CONSTANT_MethodHandle:
        break;
    case ConstPoolEntryTag::CONSTANT_MethodType:
        break;
    case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
    {
        const ConstInvokeDynamicInfo& cidi = static_cast<const ConstInvokeDynamicInfo&>(e);
        os << std::dec
           << indent << "\tbootstrap_method_attr_index: " << cidi.bootstrap_method_attr_index << ",\n"
           << indent << "\tname_and_type_index: " << cidi.name_and_type_index << "\n";
        break;
    }
    }

    os << indent << "},\n";
}

std::ostream& operator<<(std::ostream& os, const ClassFile& cf)
{
    os << "{\n"
       << "\tmagic: " << std::hex << cf.magic << ",\n"
       << std::dec
       << "\tminor_version: " << cf.minor_version << ",\n"
       << "\tmajor_version: " << cf.major_version << "\n";

    if (cf.const_pool.size() > 0)
    {
        os << "\tconst_pool: [\n";
        for (std::size_t i = 0; i < cf.const_pool.size(); i++)
        {
            print(os, *cf.const_pool[i], "\t\t");
        }
        os << "\t]\n";
    }

    os << "}";

    os << std::dec;

    return os;
}
