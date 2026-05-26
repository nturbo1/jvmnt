#include "class_file.h"
#include "base.h"

#include <string>
#include <ostream>
#include <iomanip>
#include <cstddef>

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
    {
        const ConstFieldrefInfo& cfri = static_cast<const ConstFieldrefInfo&>(e);
        os << indent << "\tclass_index: " << cfri.class_index << ",\n"
           << indent << "\tname_and_type_index: " << cfri.name_and_type_index << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Methodref:
    {
        const ConstMethodrefInfo& cmri = static_cast<const ConstMethodrefInfo&>(e);
        os << indent << "\tclass_index: " << cmri.class_index << ",\n"
           << indent << "\tname_and_type_index: " << cmri.name_and_type_index << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        break;
    case ConstPoolEntryTag::CONSTANT_String:
    {
        const ConstStringInfo& csi = static_cast<const ConstStringInfo&>(e);
        os << indent << "\tstring_index: " << csi.string_index << ",\n";
        break;
    }
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
           << indent << "\thigh_bytes: " << "0x" << cdi.high_bytes << ",\n"
           << indent << "\tlow_bytes: " << "0x" << cdi.low_bytes << "\n"
           << std::dec;
        break;
    }
    case ConstPoolEntryTag::CONSTANT_NameAndType:
    {
        const ConstNameAndTypeInfo& cnati = static_cast<const ConstNameAndTypeInfo&>(e);
        os << indent << "\tname_index: " << cnati.name_index << ",\n"
           << indent << "\tdescriptor_index: " << cnati.descriptor_index << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Utf8:
    {
        const ConstUtf8Info& cutf8i = static_cast<const ConstUtf8Info&>(e);
        os << indent << "\tbytes: [";
        for (std::size_t i{0}; i < cutf8i.bytes.size(); i++)
        {
           os << cutf8i.bytes[i];
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

static void print(std::ostream& os, const AttrInfo& ai, const std::string& indent)
{
    os << indent << "{\n"
       << indent << "\tattr_name_index: " << ai.attr_name_index << ",\n"
       << indent << "}";
}

static void print(std::ostream& os, const FieldInfo& fi, const std::string& indent)
{
    os << indent << "{\n"
       << indent << "\taccess_flags: " << std::hex << "0x" << fi.access_flags << ",\n"
       << indent << "\tname_index: " << std::dec << fi.name_index << ",\n"
       << indent << "\tdescriptor_index: " << fi.descriptor_index << ",\n"
       << indent << "\tattributes: ";

    std::size_t attributes_size{ fi.attributes.size() };
    if (attributes_size > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < attributes_size - 1; i++)
        {
            print(os, *(fi.attributes[i]), indent + "\t\t");
            os << ",\n";
        }
        print(os, *(fi.attributes[attributes_size - 1]), indent + "\t\t");
        os << "\n" << indent << "\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    // FieldInfo closing brace
    os << indent << "}";
}

static void print(std::ostream& os, const MethodInfo& mi, const std::string& indent)
{
    os << indent << "{\n"
       << indent << "\taccess_flags: " << std::hex << "0x" << mi.access_flags << ",\n"
       << indent << "\tname_index: " << std::dec << mi.name_index << ",\n"
       << indent << "\tdescriptor_index: " << mi.descriptor_index << ",\n"
       << indent << "\tattributes: ";

    std::size_t attributes_size{ mi.attributes.size() };
    if (attributes_size > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < attributes_size - 1; i++)
        {
            print(os, *(mi.attributes[i]), indent + "\t\t");
            os << ",\n";
        }
        print(os, *(mi.attributes[attributes_size - 1]), indent + "\t\t");
        os << "\n" << indent << "\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    // MethodInfo closing brace
    os << indent << "}";
}

std::ostream& operator<<(std::ostream& os, const ClassFile& cf)
{
    os << "{\n"
       << "\tmagic: " << std::hex << "0x" << cf.magic << ",\n"
       << std::dec
       << "\tminor_version: " << cf.minor_version << ",\n"
       << "\tmajor_version: " << cf.major_version << ",\n";

    os << "\tconst_pool: ";
    if (cf.const_pool.size() > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < cf.const_pool.size(); i++)
        {
            print(os, *cf.const_pool[i], "\t\t");
        }
        os << "\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\taccess_flags: " << std::hex << "0x" << cf.access_flags << "\n" << std::dec;
    os << "\tthis_class: " << cf.this_class << "\n";
    os << "\tsuper_class: " << cf.super_class << "\n";

    os << "\tinterfaces: [";
    std::size_t interfaces_size{ cf.interfaces.size() };
    if (interfaces_size > 0)
    {
        for (std::size_t i{0}; i < interfaces_size - 1; i++)
        {
            os << cf.interfaces[i] << ", ";
        }
        os << cf.interfaces[interfaces_size - 1];
    }
    os << "],\n";

    os << "\tfields: ";
    std::size_t fields_size{ cf.fields.size() };
    if (fields_size > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < fields_size - 1; i++)
        {
            print(os, cf.fields[i], "\t\t");
            os << ",\n";
        }
        print(os, cf.fields[fields_size - 1], "\t\t");
        os << "\n\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\tmethods: ";
    std::size_t methods_size{ cf.methods.size() };
    if (methods_size > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < methods_size - 1; i++)
        {
            print(os, cf.methods[i], "\t\t");
            os << ",\n";
        }
        print(os, cf.methods[methods_size - 1], "\t\t");
        os << "\n\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\tattributes: ";
    std::size_t attributes_size{ cf.attributes.size() };
    if (attributes_size > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < attributes_size - 1; i++)
        {
            print(os, *(cf.attributes[i]), "\t\t");
            os << ",\n";
        }
        print(os, *(cf.attributes[attributes_size - 1]), "\t\t");
        os << "\n\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    // ClassFile closing brace
    os << "}";

    os << std::dec;

    return os;
}
