#include "classFile.h"
#include "base.h"

#include <string>
#include <ostream>
#include <iomanip>
#include <cstddef>

static void printConstPoolEntry(std::ostream& os, const ConstPoolEntry& e, const std::string& indent)
{
    os << indent << "{" << "\n"
       << indent << "\ttag: " << e.tag << ",\n";

    switch(e.tag)
    {
    case ConstPoolEntryTag::CONSTANT_Class:
    {
        const ConstClassInfo& cci = static_cast<const ConstClassInfo&>(e);
        os << std::dec
           << indent << "\tnameIndex: " << cci.name_index << ",\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Fieldref:
    {
        const ConstFieldrefInfo& cfri = static_cast<const ConstFieldrefInfo&>(e);
        os << indent << "\tclassIndex: " << cfri.class_index << ",\n"
           << indent << "\tnameAndTypeIndex: " << cfri.name_and_type_index << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Methodref:
    {
        const ConstMethodrefInfo& cmri = static_cast<const ConstMethodrefInfo&>(e);
        os << indent << "\tclassIndex: " << cmri.class_index << ",\n"
           << indent << "\tnameAndTypeIndex: " << cmri.name_and_type_index << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        break;
    case ConstPoolEntryTag::CONSTANT_String:
    {
        const ConstStringInfo& csi = static_cast<const ConstStringInfo&>(e);
        os << indent << "\tstringIndex: " << csi.string_index << ",\n";
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
           << indent << "\thighBytes: " << "0x" << cdi.high_bytes << ",\n"
           << indent << "\tlowBytes: " << "0x" << cdi.low_bytes << "\n"
           << std::dec;
        break;
    }
    case ConstPoolEntryTag::CONSTANT_NameAndType:
    {
        const ConstNameAndTypeInfo& cnati = static_cast<const ConstNameAndTypeInfo&>(e);
        os << indent << "\tnameIndex: " << cnati.name_index << ",\n"
           << indent << "\tdescriptorIndex: " << cnati.descriptor_index << "\n";
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
           << indent << "\tbootstrapMethodAttrIndex: " << cidi.bootstrap_method_attr_index << ",\n"
           << indent << "\tnameAndTypeIndex: " << cidi.name_and_type_index << "\n";
        break;
    }
    }

    os << indent << "},\n";
}

static void printExceptionTableEntry(
        std::ostream& os,
        const ExceptionTableEntry& ete,
        const std::string& indent)
{
    os << indent << "{\n"
       << std::hex
       << indent << "\tstartPc: " << ete.start_pc << ",\n"
       << indent << "\tendPc: " << ete.end_pc << ",\n"
       << indent << "\thandlerPc: " << ete.handler_pc << ",\n"
       << std::dec
       << indent << "\tcatchType: " << ete.catch_type << "\n"
       << indent << "}";
}

static void printAttr(
        std::ostream& os,
        const AttrInfo& ai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);

static void printCodeAttr(
        std::ostream& os,
        const CodeAttrInfo& cai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\tattrNameIndex: " << cai.attr_name_index << ",\n"
       << indent << "\tmaxStack: " << cai.max_stack << ",\n"
       << indent << "\tmaxLocals: " << cai.max_locals << ",\n"
       << indent << "\tcode: ";

    std::size_t codeSize{ cai.code.size() };
    if (codeSize > 0)
    {
        os << "[";
        os << std::hex;
        for (std::size_t i{0}; i < codeSize - 1; i++)
        {
            os << static_cast<u4>(cai.code[i]) << ", ";
        }
        os << static_cast<u4>(cai.code[codeSize - 1]) << "],\n";
        os << std::dec;
    }
    else
    {
        os << "[],\n";
    }

    os << indent << "\texceptionTable: ";
    std::size_t exceptionTableSize{ cai.exception_table.size() };
    if (exceptionTableSize > 0)
    {
        os << "[\n";
        std::string eteIndent{ indent + "\t\t" };
        for (std::size_t i{0}; i < exceptionTableSize - 1; i++)
        {
            printExceptionTableEntry(os, cai.exception_table[i], eteIndent);
            os << ",\n";
        }
        printExceptionTableEntry(os, cai.exception_table[exceptionTableSize - 1], eteIndent);
        os << "\n\t" << indent << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << indent << "\tattributes: ";
    std::size_t attributesSize{ cai.attributes.size() };
    if (attributesSize > 0)
    {
        os << "[\n";
        std::string eteIndent{ indent + "\t\t" };
        for (std::size_t i{0}; i < attributesSize - 1; i++)
        {
            printAttr(os, *(cai.attributes[i]), eteIndent, constPool);
            os << ",\n";
        }
        printAttr(os, *(cai.attributes[attributesSize - 1]), eteIndent, constPool);
        os << "\n\t" << indent << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    // CodeAttrInfo closing brace
    os << indent << "}";
}

static void printLineNumberTableEntry(
        std::ostream& os,
        const LineNumberTableEntry& lnte,
        const std::string& indent)
{
    os << indent << "{\n"
       << indent << "\tstartPc: " << lnte.start_pc << ",\n"
       << indent << "\tlineNumber: " << lnte.line_number << "\n"
       << indent << "}";
}

static void printLineNumberTableAttr(
        std::ostream& os,
        const LineNumberTableAttrInfo& lntai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\tattrNameIndex: " << lntai.attr_name_index << ",\n"
       << indent << "\tlineNumberTable: ";

    std::size_t lineNumberTableSize{ lntai.line_number_table.size() };
    if (lineNumberTableSize > 0)
    {
        os << "[\n";
        std::string lnteIndent{ indent + "\t\t" };
        for (std::size_t i{0}; i < lineNumberTableSize - 1; i++)
        {
            printLineNumberTableEntry(os, lntai.line_number_table[i], lnteIndent);
            os << ",\n";
        }
        printLineNumberTableEntry(os, lntai.line_number_table[lineNumberTableSize - 1], lnteIndent);
        os << "\n\t" << indent << "],\n";
    }
    else
    {
        os << "[],\n";
    }

    // LineNumberTableAttr closing brace
    os << indent << "}";
}

static void printSourcefileAttr(
        std::ostream& os,
        const SourceFileAttrInfo& sfai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\tattrNameIndex: " << sfai.attr_name_index << ",\n"
       << indent << "\tsourcefileIndex: " << sfai.sourcefile_index << ",\n"
       << indent << "}";
}

static void printAttr(
        std::ostream& os,
        const AttrInfo& ai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    AttrType attrType{ resolve_attr_type(constPool, ai.attr_name_index) };
    switch (attrType)
    {
    case AttrType::ConstantValue:
        log_fixme("IMPLEMENT ConstantValue attribute printer!!!");
        break;

    case AttrType::Code:
    {
        printCodeAttr(os, static_cast<const CodeAttrInfo&>(ai), indent, constPool);
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
        printSourcefileAttr(os, static_cast<const SourceFileAttrInfo&>(ai), indent, constPool);
        break;
    }

    case AttrType::SourceDebugExtension:
        log_fixme("IMPLEMENT SourceDebugExtension attribute print logic!!!");
        break;

    case AttrType::LineNumberTable:
    {
        printLineNumberTableAttr(os, static_cast<const LineNumberTableAttrInfo&>(ai), indent, constPool);
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
        log_error("Unknown `AttrType` enum value: %d", attrType);
    }
}

static void print_field(
        std::ostream& os,
        const FieldInfo& fi,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\taccessFlags: " << std::hex << "0x" << fi.access_flags << ",\n"
       << indent << "\tnameIndex: " << std::dec << fi.name_index << ",\n"
       << indent << "\tdescriptorIndex: " << fi.descriptor_index << ",\n"
       << indent << "\tattributes: ";

    std::size_t attributesSize{ fi.attributes.size() };
    if (attributesSize > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < attributesSize - 1; i++)
        {
            printAttr(os, *(fi.attributes[i]), indent + "\t\t", constPool);
            os << ",\n";
        }
        printAttr(os, *(fi.attributes[attributesSize - 1]), indent + "\t\t", constPool);
        os << "\n\t" << indent << "\t],\n";
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
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\taccessFlags: " << std::hex << "0x" << mi.access_flags << ",\n"
       << indent << "\tnameIndex: " << std::dec << mi.name_index << ",\n"
       << indent << "\tdescriptorIndex: " << mi.descriptor_index << ",\n"
       << indent << "\tattributes: ";

    std::size_t attributesSize{ mi.attributes.size() };
    if (attributesSize > 0)
    {
        os << "[\n";
        std::string attrElemIndent{ indent + "\t\t" };
        for (std::size_t i{0}; i < attributesSize - 1; i++)
        {
            printAttr(os, *(mi.attributes[i]), attrElemIndent, constPool);
            os << ",\n";
        }
        printAttr(os, *(mi.attributes[attributesSize - 1]), attrElemIndent, constPool);
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
       << "\tm_magic: " << std::hex << "0x" << cf.m_magic << ",\n"
       << std::dec
       << "\tm_minorVersion: " << cf.m_minor_version << ",\n"
       << "\tm_majorVersion: " << cf.m_major_version << ",\n";

    os << "\tm_constPool: ";
    if (cf.m_const_pool.size() > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < cf.m_const_pool.size(); i++)
        {
            printConstPoolEntry(os, *cf.m_const_pool[i], "\t\t");
        }
        os << "\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\tm_accessFlags: " << std::hex << "0x" << cf.m_access_flags << "\n" << std::dec;
    os << "\tm_thisClass: " << cf.m_this_class << "\n";
    os << "\tm_superClass: " << cf.m_super_class << "\n";

    os << "\tm_interfaces: [";
    std::size_t interfacesSize{ cf.m_interfaces.size() };
    if (interfacesSize > 0)
    {
        for (std::size_t i{0}; i < interfacesSize - 1; i++)
        {
            os << cf.m_interfaces[i] << ", ";
        }
        os << cf.m_interfaces[interfacesSize - 1];
    }
    os << "],\n";

    os << "\tm_fields: ";
    std::size_t fieldsSize{ cf.m_fields.size() };
    if (fieldsSize > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < fieldsSize - 1; i++)
        {
            print_field(os, cf.m_fields[i], "\t\t", cf.m_const_pool);
            os << ",\n";
        }
        print_field(os, cf.m_fields[fieldsSize - 1], "\t\t", cf.m_const_pool);
        os << "\n\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\tm_methods: ";
    std::size_t methodsSize{ cf.m_methods.size() };
    if (methodsSize > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < methodsSize - 1; i++)
        {
            print_method(os, cf.m_methods[i], "\t\t", cf.m_const_pool);
            os << ",\n";
        }
        print_method(os, cf.m_methods[methodsSize - 1], "\t\t", cf.m_const_pool);
        os << "\n\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\tm_attributes: ";
    std::size_t attributesSize{ cf.m_attributes.size() };
    if (attributesSize > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < attributesSize - 1; i++)
        {
            printAttr(os, *(cf.m_attributes[i]), "\t\t", cf.m_const_pool);
            os << ",\n";
        }
        printAttr(os, *(cf.m_attributes[attributesSize - 1]), "\t\t", cf.m_const_pool);
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
