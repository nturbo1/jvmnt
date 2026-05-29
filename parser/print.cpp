#include "class_file.h"
#include "base.h"

#include <string>
#include <ostream>
#include <iomanip>
#include <cstddef>

static void print_const_pool_entry(std::ostream& os, const ConstPoolEntry& e, const std::string& indent)
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

static void print_exception_table_entry(
        std::ostream& os,
        const ExceptionTableEntry& ete,
        const std::string& indent)
{
    os << indent << "{\n"
       << std::hex
       << indent << "\tstart_pc: " << ete.start_pc << ",\n"
       << indent << "\tend_pc: " << ete.end_pc << ",\n"
       << indent << "\thandler_pc: " << ete.handler_pc << ",\n"
       << std::dec
       << indent << "\tcatch_type: " << ete.catch_type << "\n"
       << indent << "}";
}

static void print_attr(
        std::ostream& os,
        const AttrInfo& ai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool);

static void print_code_attr(
        std::ostream& os,
        const CodeAttrInfo& cai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    os << indent << "{\n"
       << indent << "\tattr_name_index: " << cai.attr_name_index << ",\n"
       << indent << "\tmax_stack: " << cai.max_stack << ",\n"
       << indent << "\tmax_locals: " << cai.max_locals << ",\n"
       << indent << "\tcode: ";

    std::size_t code_size{ cai.code.size() };
    if (code_size > 0)
    {
        os << "[";
        for (std::size_t i{0}; i < code_size - 1; i++)
        {
            os << cai.code[i] << ", ";
        }
        os << cai.code[code_size - 1] << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << indent << "\texception_table: ";
    std::size_t exception_table_size{ cai.exception_table.size() };
    if (exception_table_size > 0)
    {
        os << "[\n";
        std::string ete_indent{ indent + "\t\t" };
        for (std::size_t i{0}; i < exception_table_size - 1; i++)
        {
            print_exception_table_entry(os, cai.exception_table[i], ete_indent);
            os << ",\n";
        }
        print_exception_table_entry(os, cai.exception_table[exception_table_size - 1], ete_indent);
        os << "\n" << indent << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << indent << "\tattributes: ";
    std::size_t attributes_size{ cai.attributes.size() };
    if (attributes_size > 0)
    {
        os << "[\n";
        std::string ete_indent{ indent + "\t\t" };
        for (std::size_t i{0}; i < attributes_size - 1; i++)
        {
            print_attr(os, *(cai.attributes[i]), ete_indent, const_pool);
            os << ",\n";
        }
        print_attr(os, *(cai.attributes[attributes_size - 1]), ete_indent, const_pool);
        os << "\n" << indent << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    // CodeAttrInfo closing brace
    os << indent << "}";
}

static void print_linenumbertable_entry(
        std::ostream& os,
        const LineNumberTableEntry& lnte,
        const std::string& indent)
{
    os << indent << "{\n"
       << indent << "\tstart_pc: " << lnte.start_pc << ",\n"
       << indent << "\tline_number: " << lnte.line_number << "\n"
       << indent << "}";
}

static void print_linenumbertable_attr(
        std::ostream& os,
        const LineNumberTableAttrInfo& lntai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    os << indent << "{\n"
       << indent << "\tattr_name_index: " << lntai.attr_name_index << ",\n"
       << indent << "\tline_number_table: ";

    std::size_t line_number_table_size{ lntai.line_number_table.size() };
    if (line_number_table_size > 0)
    {
        os << "[\n";
        std::string lnte_indent{ indent + "\t\t" };
        for (std::size_t i{0}; i < line_number_table_size - 1; i++)
        {
            print_linenumbertable_entry(os, lntai.line_number_table[i], lnte_indent);
            os << ",\n";
        }
        print_linenumbertable_entry(os, lntai.line_number_table[line_number_table_size - 1], lnte_indent);
        os << "\n" << indent << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    // LineNumberTableAttr closing brace
    os << indent << "}";
}

static void print_sourcefile_attr(
        std::ostream& os,
        const SourceFileAttrInfo& sfai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    os << indent << "{\n"
       << indent << "\tattr_name_index: " << sfai.attr_name_index << ",\n"
       << indent << "\tsourcefile_index: " << sfai.sourcefile_index << ",\n"
       << indent << "}";
}

static void print_attr(
        std::ostream& os,
        const AttrInfo& ai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    AttrType attr_type{ resolve_attr_type(const_pool, ai.attr_name_index) };
    switch (attr_type)
    {
    case AttrType::ConstantValue:
        log_fixme("IMPLEMENT ConstantValue attribute printer!!!");
        break;

    case AttrType::Code:
    {
        print_code_attr(os, static_cast<const CodeAttrInfo&>(ai), indent, const_pool);
        break;
    }

    case AttrType::StackMapTable:
        log_fixme("IMPLEMENT StackMapTable attribute print logic!!!");
        break;

    case AttrType::Exceptions:
        log_fixme("IMPLEMENT Exceptions attribute print logic!!!");
        break;

    case AttrType::BootstrapMethods:
        log_fixme("IMPLEMENT BootstrapMethods attribute print logic!!!");
        break;

    case AttrType::InnerClasses:
        log_fixme("IMPLEMENT InnerClasses attribute print logic!!!");
        break;

    case AttrType::EnclosingMethod:
        log_fixme("IMPLEMENT EnclosingMethod attribute print logic!!!");
        break;

    case AttrType::Synthetic:
        log_fixme("IMPLEMENT Synthetic attribute print logic!!!");
        break;

    case AttrType::Signature:
        log_fixme("IMPLEMENT Signature attribute print logic!!!");
        break;

    case AttrType::RuntimeVisibleAnnotations:
        log_fixme("IMPLEMENT RuntimeVisibleAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeInvisibleAnnotations:
        log_fixme("IMPLEMENT RuntimeInvisibleAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeVisibleParameterAnnotations:
        log_fixme("IMPLEMENT RuntimeVisibleParameterAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeInvisibleParameterAnnotations:
        log_fixme("IMPLEMENT RuntimeInvisibleParameterAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeVisibleTypeAnnotations:
        log_fixme("IMPLEMENT RuntimeVisibleTypeAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeInvisibleTypeAnnotations:
        log_fixme("IMPLEMENT RuntimeInvisibleTypeAnnotations attribute print logic!!!");
        break;

    case AttrType::AnnotationDefault:
        log_fixme("IMPLEMENT AnnotationDefault attribute print logic!!!");
        break;

    case AttrType::MethodParameters:
        log_fixme("IMPLEMENT MethodParameters attribute print logic!!!");
        break;

    case AttrType::SourceFile:
    {
        print_sourcefile_attr(os, static_cast<const SourceFileAttrInfo&>(ai), indent, const_pool);
        break;
    }

    case AttrType::SourceDebugExtension:
        log_fixme("IMPLEMENT SourceDebugExtension attribute print logic!!!");
        break;

    case AttrType::LineNumberTable:
    {
        print_linenumbertable_attr(os, static_cast<const LineNumberTableAttrInfo&>(ai), indent, const_pool);
        break;
    }

    case AttrType::LocalVariableTable:
        log_fixme("IMPLEMENT LocalVariableTable attribute print logic!!!");
        break;

    case AttrType::LocalVariableTypeTable:
        log_fixme("IMPLEMENT LocalVariableTypeTable attribute print logic!!!");
        break;

    case AttrType::Deprecated:
        log_fixme("IMPLEMENT Deprecated attribute print logic!!!");
        break;

    default:
        log_error("Unknown `AttrType` enum value: %d", attr_type);
    }
}

static void print_field(
        std::ostream& os,
        const FieldInfo& fi,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
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
            print_attr(os, *(fi.attributes[i]), indent + "\t\t", const_pool);
            os << ",\n";
        }
        print_attr(os, *(fi.attributes[attributes_size - 1]), indent + "\t\t", const_pool);
        os << "\n" << indent << "\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    // FieldInfo closing brace
    os << indent << "}";
}

static void print_method(
        std::ostream& os,
        const MethodInfo& mi,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
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
        std::string attr_elem_indent{ indent + "\t\t" };
        for (std::size_t i{0}; i < attributes_size - 1; i++)
        {
            print_attr(os, *(mi.attributes[i]), attr_elem_indent, const_pool);
            os << ",\n";
        }
        print_attr(os, *(mi.attributes[attributes_size - 1]), attr_elem_indent, const_pool);
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
            print_const_pool_entry(os, *cf.const_pool[i], "\t\t");
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
            print_field(os, cf.fields[i], "\t\t", cf.const_pool);
            os << ",\n";
        }
        print_field(os, cf.fields[fields_size - 1], "\t\t", cf.const_pool);
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
            print_method(os, cf.methods[i], "\t\t", cf.const_pool);
            os << ",\n";
        }
        print_method(os, cf.methods[methods_size - 1], "\t\t", cf.const_pool);
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
            print_attr(os, *(cf.attributes[i]), "\t\t", cf.const_pool);
            os << ",\n";
        }
        print_attr(os, *(cf.attributes[attributes_size - 1]), "\t\t", cf.const_pool);
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
