#include "parser.h"

#include "classFile.h"
#include "base.h"
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

ClassFileParser::ClassFileParser(ClassFileReader& reader, const std::string& filename)
    : m_reader{ reader },
    m_filename{ filename }
{}

ClassFile ClassFileParser::parse()
{
    u4 magic{ m_reader.readU4() };
    if (magic != CLASS_FILE_MAGIC)
        logFatal("Incompatible magic value %d in class file %s", magic, m_filename.c_str());

    u2 minorVersion{ m_reader.readU2() };
    u2 majorVersion{ m_reader.readU2() };

    std::vector<std::unique_ptr<ConstPoolEntry>> constPool{ parseConstPool() };
    u2 accessFlags{ m_reader.readU2() };
    u2 thisClass{ m_reader.readU2() };

    u2 superClass{ m_reader.readU2() };

    u2 interfacesCount{ m_reader.readU2() };
    std::vector<u2> interfaces;
    interfaces.reserve(interfacesCount);
    for (u2 i{0}; i < interfacesCount; ++i)
    {
        u2 intface{ m_reader.readU2() };
        interfaces.push_back(intface);
    }

    std::vector<FieldInfo> fields{ parseFields(constPool) };
    std::vector<MethodInfo> methods{ parseMethods(constPool) };
    std::vector<std::unique_ptr<AttrInfo>> attributes{ parseAttributes(constPool) };

    return ClassFile{
            magic,
            minorVersion,
            majorVersion,
            std::move(constPool),
            accessFlags,
            thisClass,
            superClass,
            interfaces,
            std::move(fields),
            std::move(methods),
            std::move(attributes),
            m_filename
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

std::vector<std::unique_ptr<ConstPoolEntry>> ClassFileParser::parseConstPool()
{
    u2 constPoolCount{ m_reader.readU2() };

    std::vector<std::unique_ptr<ConstPoolEntry>> constPool;
    // cp_info constant_pool[constant_poolCount-1]
    constPool.reserve(constPoolCount - 1);

    // `constant_pool` indexes are in the range `1, 2, ..., constant_poolCount - 1`.
    for (int i{1}; i < constPoolCount; ++i)
    {
        u1 t{ m_reader.readU1() };
        ConstPoolEntryTag tag{ matchConstPoolEntryTag(t) };

        if (tag == ConstPoolEntryTag::INVALID)
            logFatal("Constant pool entry at index %d has an invalid tag.", i);

        switch(tag)
        {
        case ConstPoolEntryTag::CONSTANT_Class:
            constPool.push_back(parseConstClassInfo(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Fieldref:
            constPool.push_back(parseConstFieldrefInfo(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Methodref:
            constPool.push_back(parseConstMethodrefInfo(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
            logFixme("Implement constant pool entry 'CONSTANT_InterfaceMethodref' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_String:
            constPool.push_back(parseConstStringInfo(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Integer:
            logFixme("Implement constant pool entry 'CONSTANT_Integer' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_Float:
            logFixme("Implement constant pool entry 'CONSTANT_Float' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_Long:
            logFixme("Implement constant pool entry 'CONSTANT_Long' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_Double:
            constPool.push_back(parseConstDoubleInfo(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_NameAndType:
            constPool.push_back(parseConstNameandtypeInfo(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_Utf8:
            constPool.push_back(parseConstUtf8Info(tag));
            break;

        case ConstPoolEntryTag::CONSTANT_MethodHandle:
            logFixme("Implement constant pool entry 'CONSTANT_MethodHandle' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_MethodType:
            logFixme("Implement constant pool entry 'CONSTANT_MethodType' parser!");
            break;

        case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
            constPool.push_back(parseConstInvokeDynamicInfo(tag));
            break;

        default:
            assert(false && "The constant pool entry tag was not validated beforehand!");
        }
    }

    return constPool;
}

std::unique_ptr<ConstMethodrefInfo> ClassFileParser::parseConstMethodrefInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Methodref &&
            "Non 'CONSTANT_Methodref' constant pool entry tag value was passed to the CONSTANT_Methodref_info parser");

    u2 classIndex{ m_reader.readU2() };
    // TODO: VALIDATE that the value of the `classIndex` item must be a valid index into the
    //       `constant_pool` table. The `constant_pool` entry at that index must be a
    //       `CONSTANT_Class_info` structure representing a class, NOT an interface, type
    //       that has the method as a member.

    u2 nameAndTypeIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `nameAndTypeIndex` item must be a valid index into
    //         the `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_NameAndType_info` structure. This `constant_pool` entry indicates
    //         the name and descriptor of the method.
    //
    //       - if the name of the method begins with a '<' ('\u003c'), then the name must
    //         be the special name `<init>`, representing an instance initialization method.
    //         The return type of such a method must be `void`.

    return std::make_unique<ConstMethodrefInfo>(tag, classIndex, nameAndTypeIndex);
}

std::unique_ptr<ConstFieldrefInfo> ClassFileParser::parseConstFieldrefInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Fieldref &&
            "Non 'CONSTANT_Fieldref' constant pool entry tag value was passed to the CONSTANT_Fieldref_info parser");

    u2 classIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `classIndex` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Class_info` structure representing a class or an interface type
    //         that has the field as a member.

    u2 nameAndTypeIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `nameAndTypeIndex` item must be a valid index into
    //         the `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_NameAndType_info` structure. This `constant_pool` entry indicates
    //         the name and descriptor of the field. The descriptor MUST be a field descriptor.

    return std::make_unique<ConstFieldrefInfo>(tag, classIndex, nameAndTypeIndex);
}

std::unique_ptr<ConstStringInfo> ClassFileParser::parseConstStringInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_String &&
            "Non 'CONSTANT_String' constant pool entry tag value was passed to the CONSTANT_String_info parser");

    u2 stringIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `stringIndex` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Utf8_info` structure representing the sequence of Unicode
    //         code points to which the String object is to be initialized.

    return std::make_unique<ConstStringInfo>(tag, stringIndex);
}

std::unique_ptr<ConstDoubleInfo> ClassFileParser::parseConstDoubleInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Double &&
            "Non 'CONSTANT_Double' constant pool entry tag value was passed to the CONSTANT_Double_info parser");
    u4 highBytes{ m_reader.readU4() };
    u4 lowBytes{ m_reader.readU4() };

    return std::make_unique<ConstDoubleInfo>(tag, highBytes, lowBytes);
}

std::unique_ptr<ConstInvokeDynamicInfo>
ClassFileParser::parseConstInvokeDynamicInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_InvokeDynamic &&
            "Non 'CONSTANT_InvokeDynamic' constant pool entry tag value was passed to the"
            "CONSTANT_InvokeDynamic_info parser");

    u2 bootstrapMethodAttrIndex{ m_reader.readU2() };
    // TODO: VALIDATE that the `bootstrapMethodAttrIndex` item must be a valid index into the
    //       `bootstrap_methods` array of the bootstrap method table of this class file!

    u2 nameAndTypeIndex{ m_reader.readU2() };
    // TODO: VALIDATE that the value of the `nameAndTypeIndex` item must be a valid index into the
    //       `constant_pool` table. The `constant_pool` entry at that index must be a
    //       `CONSTANT_NameAndType_info` structure representing a method name and method descriptor.

    return std::make_unique<ConstInvokeDynamicInfo>(tag, bootstrapMethodAttrIndex, nameAndTypeIndex);
}

std::unique_ptr<ConstClassInfo> ClassFileParser::parseConstClassInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Class &&
            "Non 'CONSTANT_Class' constant pool entry tag value was passed to the 'CONSTANT_Class_info' parser");

    u2 nameIndex{ m_reader.readU2() };
    // TODO: VALIDATE the value of the `nameIndex` item must be a valid index into
    //       the `constant_pool` table. The `constant_pool` entry at that index must be a
    //       `CONSTANT_Utf8_info` structure representing a valid binary class or interface
    //       name encoded in internal form.

    return std::make_unique<ConstClassInfo>(tag, nameIndex);
}

std::unique_ptr<ConstUtf8Info> ClassFileParser::parseConstUtf8Info(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Utf8 &&
            "Non 'CONSTANT_Utf8' constant pool entry tag value was passed to the 'CONSTANT_Utf8_info' parser");

    u2 length{ m_reader.readU2() };
    std::vector<u1> utf8Bytes;
    utf8Bytes.reserve(length);
    for (std::size_t i{0}; i < length; ++i)
    {
        u1 b{ m_reader.readU1() };

        if (b == 0)
            logFatal("`Constant UTF8` bytes cannot be `0`!");
        if (0xf0 <= b && b <= 0xff)
            logFatal("`Constant UTF8` bytes cannot lie in the range (byte)`0xf0` to (byte)`0xff`!");

        utf8Bytes.push_back(b);
    }

    return std::make_unique<ConstUtf8Info>(tag, utf8Bytes);
}

std::unique_ptr<ConstNameAndTypeInfo>
ClassFileParser::parseConstNameandtypeInfo(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_NameAndType &&
            "Non 'CONSTANT_NameAndType' constant pool entry tag value was passed"
            "to the 'CONSTANT_NameAndType_info' parser");

    u2 nameIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `nameIndex` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Utf8_info` structure representing either the special method
    //         name `<init>` or a valid unqualified name denoting a field or method.

    u2 descriptorIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `descriptorIndex` item must be a valid index into the
    //         `constant_pool` table. The `constant_pool` entry at that index must be a
    //         `CONSTANT_Utf8_info` structure representing a valid field descriptor
    //         or method descriptor.

    return std::make_unique<ConstNameAndTypeInfo>(tag, nameIndex, descriptorIndex);
}

std::unique_ptr<AttrInfo>
ClassFileParser::parseAttr(const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    u2 attrNameIndex{ m_reader.readU2() };
    AttrType attrType{ resolveAttrType(constPool, attrNameIndex) };

    switch (attrType)
    {
    case AttrType::ConstantValue:
        logFatal("IMPLEMENT ConstantValue parser!!!");
        break;

    case AttrType::Code:
    {
        std::unique_ptr<CodeAttrInfo> codeAttr{ std::make_unique<CodeAttrInfo>(attrNameIndex) };
        parseAttrCode(*codeAttr, constPool);
        return codeAttr;
    }

    case AttrType::StackMapTable:
        logFatal("IMPLEMENT StackMapTable parser!!!");
        break;

    case AttrType::Exceptions:
        logFatal("IMPLEMENT Exceptions parser!!!");
        break;

    case AttrType::BootstrapMethods:
        logFatal("IMPLEMENT BootstrapMethods parser!!!");
        break;

    case AttrType::InnerClasses:
        logFatal("IMPLEMENT InnerClasses parser!!!");
        break;

    case AttrType::EnclosingMethod:
        logFatal("IMPLEMENT EnclosingMethod parser!!!");
        break;

    case AttrType::Synthetic:
        logFatal("IMPLEMENT Synthetic parser!!!");
        break;

    case AttrType::Signature:
        logFatal("IMPLEMENT Signature parser!!!");
        break;

    case AttrType::RuntimeVisibleAnnotations:
        logFatal("IMPLEMENT RuntimeVisibleAnnotations parser!!!");
        break;

    case AttrType::RuntimeInvisibleAnnotations:
        logFatal("IMPLEMENT RuntimeInvisibleAnnotations parser!!!");
        break;

    case AttrType::RuntimeVisibleParameterAnnotations:
        logFatal("IMPLEMENT RuntimeVisibleParameterAnnotations parser!!!");
        break;

    case AttrType::RuntimeInvisibleParameterAnnotations:
        logFatal("IMPLEMENT RuntimeInvisibleParameterAnnotations parser!!!");
        break;

    case AttrType::RuntimeVisibleTypeAnnotations:
        logFatal("IMPLEMENT RuntimeVisibleTypeAnnotations parser!!!");
        break;

    case AttrType::RuntimeInvisibleTypeAnnotations:
        logFatal("IMPLEMENT RuntimeInvisibleTypeAnnotations parser!!!");
        break;

    case AttrType::AnnotationDefault:
        logFatal("IMPLEMENT AnnotationDefault parser!!!");
        break;

    case AttrType::MethodParameters:
        logFatal("IMPLEMENT MethodParameters parser!!!");
        break;

    case AttrType::SourceFile:
    {
        std::unique_ptr<SourceFileAttrInfo> sourcefileAttr{
            std::make_unique<SourceFileAttrInfo>(attrNameIndex)
        };
        parseAttrSourcefile(*sourcefileAttr, constPool);
        return sourcefileAttr;
    }

    case AttrType::SourceDebugExtension:
        logFatal("IMPLEMENT SourceDebugExtension parser!!!");
        break;

    case AttrType::LineNumberTable:
    {
        std::unique_ptr<LineNumberTableAttrInfo> lineNumberTableAttr{
            std::make_unique<LineNumberTableAttrInfo>(attrNameIndex)
        };
        parseAttrLineNumberTable(*lineNumberTableAttr, constPool);
        return lineNumberTableAttr;
    }

    case AttrType::LocalVariableTable:
        logFatal("IMPLEMENT LocalVariableTable parser!!!");
        break;

    case AttrType::LocalVariableTypeTable:
        logFatal("IMPLEMENT LocalVariableTypeTable parser!!!");
        break;

    case AttrType::Deprecated:
        logFatal("IMPLEMENT Deprecated parser!!!");
        break;

    default:
        logFatal("Unknown `AttrType` enum value: %d", attrType);
    }
}

std::vector<std::unique_ptr<AttrInfo>>
ClassFileParser::parseAttributes(const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    u2 attributesCount{ m_reader.readU2() };
    std::vector<std::unique_ptr<AttrInfo>> attributes;
    attributes.reserve(attributesCount);

    for (u2 i{0}; i < attributesCount; ++i)
    {
        attributes.push_back(parseAttr(constPool));
    }

    return attributes;
}

std::vector<FieldInfo> ClassFileParser::parseFields(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    u2 fieldsCount{ m_reader.readU2() };
    std::vector<FieldInfo> fields;
    fields.reserve(fieldsCount);

    for (u2 i{0}; i < fieldsCount; ++i)
    {
        u2 accessFlags{ m_reader.readU2() };
        u2 nameIndex{ m_reader.readU2() };
        // TODO: VALIDATE that:
        //       - the value of the `nameIndex` item must be a valid index into the
        //        `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //        structure which represents a valid unqualified name denoting a field.

        u2 descriptorIndex{ m_reader.readU2() };
        // TODO: VALIDATE that:
        //       - the value of the `descriptorIndex` item must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //         structure which represents a valid field descriptor.

        std::vector<std::unique_ptr<AttrInfo>> attributes{ parseAttributes(constPool) };
        fields.emplace_back(accessFlags, nameIndex, descriptorIndex, std::move(attributes));
    }

    return fields;
}

std::vector<MethodInfo>
ClassFileParser::parseMethods(const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    u2 methodsCount{ m_reader.readU2() };
    std::vector<MethodInfo> methods;
    methods.reserve(methodsCount);

    for (u2 i = 0; i < methodsCount; ++i)
    {
        u2 accessFlags{ m_reader.readU2() };
        u2 nameIndex{ m_reader.readU2() };
        // TODO: VALIDATE that:
        //       - the value of the `nameIndex` item must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //         structure representing either one of the special method names `<init>`
        //         or `<clinit>`, or a valid unqualified name denoting a method.

        u2 descriptorIndex{ m_reader.readU2() };
        // TODO: VALIDATE that:
        //       - the value of the `descriptorIndex` item must be a valid index into the
        //         `constant_pool` table.
        //       - the `constant_pool` entry at that index must be a `CONSTANT_Utf8_info`
        //         structure representing a valid method descriptor.

        std::vector<std::unique_ptr<AttrInfo>> attributes{ parseAttributes(constPool) };
        methods.emplace_back(accessFlags, nameIndex, descriptorIndex, std::move(attributes));
    }

    return methods;
}

// It's assumed that the `attributeNameIndex` field bytes have been read and processed
// to determine the specific type of the attribute, so those bytes are skipped.
void ClassFileParser::parseAttrCode(
        CodeAttrInfo& codeAttr,
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    m_reader.readU4(); // deliberately skipping `attributeLength`

    u2 maxStack{ m_reader.readU2() };
    u2 maxLocals{ m_reader.readU2() };

    u4 codeLength{ m_reader.readU4() };
    std::vector<u1> code;
    code.reserve(codeLength);
    for (u4 i = 0; i < codeLength; i++)
    {
        code.push_back(m_reader.readU1());
    }

    codeAttr.maxStack = maxStack;
    codeAttr.maxLocals = maxLocals;
    codeAttr.code = code;
    codeAttr.exceptionTable = parseExceptionTable();
    codeAttr.attributes = parseAttributes(constPool);
}

std::vector<ExceptionTableEntry> ClassFileParser::parseExceptionTable()
{
    u2 exceptionTableLength{ m_reader.readU2() };
    std::vector<ExceptionTableEntry> exceptionTable;
    exceptionTable.reserve(exceptionTableLength);
    for (u2 i = 0; i < exceptionTableLength; i++)
    {
        u2 startPc{ m_reader.readU2() };
        u2 endPc{ m_reader.readU2() };
        u2 handlerPc{ m_reader.readU2() };
        u2 catchType{ m_reader.readU2() };

        exceptionTable.emplace_back(startPc, endPc, handlerPc, catchType);
    }

    return exceptionTable;
}

// It's assumed that the `attributeNameIndex` field bytes have been read and processed
// to determine the specific type of the attribute, so those bytes are skipped.
void ClassFileParser::parseAttrLineNumberTable(
            LineNumberTableAttrInfo& lineNumberTableAttr,
            const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    m_reader.readU4(); // deliberately skipping `attributeLength`
    lineNumberTableAttr.lineNumberTable = parseLineNumberTable();
}

std::vector<LineNumberTableEntry> ClassFileParser::parseLineNumberTable()
{
    u2 lineNumberTableLength{ m_reader.readU2() };
    std::vector<LineNumberTableEntry> lineNumTable;
    lineNumTable.reserve(lineNumberTableLength);

    for (u2 i{0}; i < lineNumberTableLength; i++)
    {
        u2 startPc{ m_reader.readU2() };
        u2 lineNumber{ m_reader.readU2() };

        lineNumTable.emplace_back(startPc, lineNumber);
    }

    return lineNumTable;
}

// It's assumed that the `attributeNameIndex` field bytes have been read and processed
// to determine the specific type of the attribute, so those bytes are skipped.
void ClassFileParser::parseAttrSourcefile(
            SourceFileAttrInfo& sourcefileAttr,
            const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    m_reader.readU4(); // deliberately skipping `attributeLength`
    u2 sourcefileIndex{ m_reader.readU2() };
    // TODO: VALIDATE that:
    //       - the value of the `sourcefileIndex` item MUST be a valid index into the
    //         `constant_pool` table.
    //       - the `constant_pool` entry at that index MUST be a `CONSTANT_Utf8_info`
    //         structure representing a string.

    sourcefileAttr.sourcefileIndex = sourcefileIndex;
}
