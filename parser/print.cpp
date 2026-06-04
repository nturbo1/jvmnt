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
           << indent << "\tnameIndex: " << cci.nameIndex << ",\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Fieldref:
    {
        const ConstFieldrefInfo& cfri = static_cast<const ConstFieldrefInfo&>(e);
        os << indent << "\tclassIndex: " << cfri.classIndex << ",\n"
           << indent << "\tnameAndTypeIndex: " << cfri.nameAndTypeIndex << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_Methodref:
    {
        const ConstMethodrefInfo& cmri = static_cast<const ConstMethodrefInfo&>(e);
        os << indent << "\tclassIndex: " << cmri.classIndex << ",\n"
           << indent << "\tnameAndTypeIndex: " << cmri.nameAndTypeIndex << "\n";
        break;
    }
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        break;
    case ConstPoolEntryTag::CONSTANT_String:
    {
        const ConstStringInfo& csi = static_cast<const ConstStringInfo&>(e);
        os << indent << "\tstringIndex: " << csi.stringIndex << ",\n";
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
           << indent << "\thighBytes: " << "0x" << cdi.highBytes << ",\n"
           << indent << "\tlowBytes: " << "0x" << cdi.lowBytes << "\n"
           << std::dec;
        break;
    }
    case ConstPoolEntryTag::CONSTANT_NameAndType:
    {
        const ConstNameAndTypeInfo& cnati = static_cast<const ConstNameAndTypeInfo&>(e);
        os << indent << "\tnameIndex: " << cnati.nameIndex << ",\n"
           << indent << "\tdescriptorIndex: " << cnati.descriptorIndex << "\n";
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
           << indent << "\tbootstrapMethodAttrIndex: " << cidi.bootstrapMethodAttrIndex << ",\n"
           << indent << "\tnameAndTypeIndex: " << cidi.nameAndTypeIndex << "\n";
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
       << indent << "\tstartPc: " << ete.startPc << ",\n"
       << indent << "\tendPc: " << ete.endPc << ",\n"
       << indent << "\thandlerPc: " << ete.handlerPc << ",\n"
       << std::dec
       << indent << "\tcatchType: " << ete.catchType << "\n"
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
       << indent << "\tattrNameIndex: " << cai.attrNameIndex << ",\n"
       << indent << "\tmaxStack: " << cai.maxStack << ",\n"
       << indent << "\tmaxLocals: " << cai.maxLocals << ",\n"
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
    std::size_t exceptionTableSize{ cai.exceptionTable.size() };
    if (exceptionTableSize > 0)
    {
        os << "[\n";
        std::string eteIndent{ indent + "\t\t" };
        for (std::size_t i{0}; i < exceptionTableSize - 1; i++)
        {
            printExceptionTableEntry(os, cai.exceptionTable[i], eteIndent);
            os << ",\n";
        }
        printExceptionTableEntry(os, cai.exceptionTable[exceptionTableSize - 1], eteIndent);
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
       << indent << "\tstartPc: " << lnte.startPc << ",\n"
       << indent << "\tlineNumber: " << lnte.lineNumber << "\n"
       << indent << "}";
}

static void printLineNumberTableAttr(
        std::ostream& os,
        const LineNumberTableAttrInfo& lntai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\tattrNameIndex: " << lntai.attrNameIndex << ",\n"
       << indent << "\tlineNumberTable: ";

    std::size_t lineNumberTableSize{ lntai.lineNumberTable.size() };
    if (lineNumberTableSize > 0)
    {
        os << "[\n";
        std::string lnteIndent{ indent + "\t\t" };
        for (std::size_t i{0}; i < lineNumberTableSize - 1; i++)
        {
            printLineNumberTableEntry(os, lntai.lineNumberTable[i], lnteIndent);
            os << ",\n";
        }
        printLineNumberTableEntry(os, lntai.lineNumberTable[lineNumberTableSize - 1], lnteIndent);
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
       << indent << "\tattrNameIndex: " << sfai.attrNameIndex << ",\n"
       << indent << "\tsourcefileIndex: " << sfai.sourcefileIndex << ",\n"
       << indent << "}";
}

static void printAttr(
        std::ostream& os,
        const AttrInfo& ai,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    AttrType attrType{ resolveAttrType(constPool, ai.attrNameIndex) };
    switch (attrType)
    {
    case AttrType::ConstantValue:
        logFixme("IMPLEMENT ConstantValue attribute printer!!!");
        break;

    case AttrType::Code:
    {
        printCodeAttr(os, static_cast<const CodeAttrInfo&>(ai), indent, constPool);
        break;
    }

    case AttrType::StackMapTable:
        logFixme("IMPLEMENT StackMapTable attribute print logic!!!");
        break;

    case AttrType::Exceptions:
        logFixme("IMPLEMENT Exceptions attribute print logic!!!");
        break;

    case AttrType::BootstrapMethods:
        logFixme("IMPLEMENT BootstrapMethods attribute print logic!!!");
        break;

    case AttrType::InnerClasses:
        logFixme("IMPLEMENT InnerClasses attribute print logic!!!");
        break;

    case AttrType::EnclosingMethod:
        logFixme("IMPLEMENT EnclosingMethod attribute print logic!!!");
        break;

    case AttrType::Synthetic:
        logFixme("IMPLEMENT Synthetic attribute print logic!!!");
        break;

    case AttrType::Signature:
        logFixme("IMPLEMENT Signature attribute print logic!!!");
        break;

    case AttrType::RuntimeVisibleAnnotations:
        logFixme("IMPLEMENT RuntimeVisibleAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeInvisibleAnnotations:
        logFixme("IMPLEMENT RuntimeInvisibleAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeVisibleParameterAnnotations:
        logFixme("IMPLEMENT RuntimeVisibleParameterAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeInvisibleParameterAnnotations:
        logFixme("IMPLEMENT RuntimeInvisibleParameterAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeVisibleTypeAnnotations:
        logFixme("IMPLEMENT RuntimeVisibleTypeAnnotations attribute print logic!!!");
        break;

    case AttrType::RuntimeInvisibleTypeAnnotations:
        logFixme("IMPLEMENT RuntimeInvisibleTypeAnnotations attribute print logic!!!");
        break;

    case AttrType::AnnotationDefault:
        logFixme("IMPLEMENT AnnotationDefault attribute print logic!!!");
        break;

    case AttrType::MethodParameters:
        logFixme("IMPLEMENT MethodParameters attribute print logic!!!");
        break;

    case AttrType::SourceFile:
    {
        printSourcefileAttr(os, static_cast<const SourceFileAttrInfo&>(ai), indent, constPool);
        break;
    }

    case AttrType::SourceDebugExtension:
        logFixme("IMPLEMENT SourceDebugExtension attribute print logic!!!");
        break;

    case AttrType::LineNumberTable:
    {
        printLineNumberTableAttr(os, static_cast<const LineNumberTableAttrInfo&>(ai), indent, constPool);
        break;
    }

    case AttrType::LocalVariableTable:
        logFixme("IMPLEMENT LocalVariableTable attribute print logic!!!");
        break;

    case AttrType::LocalVariableTypeTable:
        logFixme("IMPLEMENT LocalVariableTypeTable attribute print logic!!!");
        break;

    case AttrType::Deprecated:
        logFixme("IMPLEMENT Deprecated attribute print logic!!!");
        break;

    default:
        logError("Unknown `AttrType` enum value: %d", attrType);
    }
}

static void print_field(
        std::ostream& os,
        const FieldInfo& fi,
        const std::string& indent,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    os << indent << "{\n"
       << indent << "\taccessFlags: " << std::hex << "0x" << fi.accessFlags << ",\n"
       << indent << "\tnameIndex: " << std::dec << fi.nameIndex << ",\n"
       << indent << "\tdescriptorIndex: " << fi.descriptorIndex << ",\n"
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
       << indent << "\taccessFlags: " << std::hex << "0x" << mi.accessFlags << ",\n"
       << indent << "\tnameIndex: " << std::dec << mi.nameIndex << ",\n"
       << indent << "\tdescriptorIndex: " << mi.descriptorIndex << ",\n"
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
       << "\tm_minorVersion: " << cf.m_minorVersion << ",\n"
       << "\tm_majorVersion: " << cf.m_majorVersion << ",\n";

    os << "\tm_constPool: ";
    if (cf.m_constPool.size() > 0)
    {
        os << "[\n";
        for (std::size_t i{0}; i < cf.m_constPool.size(); i++)
        {
            printConstPoolEntry(os, *cf.m_constPool[i], "\t\t");
        }
        os << "\t],\n";
    }
    else
    {
        os << "[],\n";
    }

    os << "\tm_accessFlags: " << std::hex << "0x" << cf.m_accessFlags << "\n" << std::dec;
    os << "\tm_thisClass: " << cf.m_thisClass << "\n";
    os << "\tm_superClass: " << cf.m_superClass << "\n";

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
            print_field(os, cf.m_fields[i], "\t\t", cf.m_constPool);
            os << ",\n";
        }
        print_field(os, cf.m_fields[fieldsSize - 1], "\t\t", cf.m_constPool);
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
            print_method(os, cf.m_methods[i], "\t\t", cf.m_constPool);
            os << ",\n";
        }
        print_method(os, cf.m_methods[methodsSize - 1], "\t\t", cf.m_constPool);
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
            printAttr(os, *(cf.m_attributes[i]), "\t\t", cf.m_constPool);
            os << ",\n";
        }
        printAttr(os, *(cf.m_attributes[attributesSize - 1]), "\t\t", cf.m_constPool);
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
