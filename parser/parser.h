#ifndef PARSER_H
#define PARSER_H

#include "classFile.h"
#include "classReader.h"

#include <string>

class ClassFileParser
{
public:
    const std::string m_filename;

    explicit ClassFileParser(ClassFileReader& reader);
    explicit ClassFileParser(ClassFileReader& reader, const std::string& filename);

    ClassFile parse();


private:
    ClassFileReader& m_reader;

    std::vector<std::unique_ptr<ConstPoolEntry>> parseConstPool();
    std::unique_ptr<ConstMethodrefInfo> parseConstMethodrefInfo(ConstPoolEntryTag tag);
    std::unique_ptr<ConstFieldrefInfo> parseConstFieldrefInfo(ConstPoolEntryTag tag);
    std::unique_ptr<ConstStringInfo> parseConstStringInfo(ConstPoolEntryTag tag);
    std::unique_ptr<ConstDoubleInfo> parseConstDoubleInfo(ConstPoolEntryTag tag);
    std::unique_ptr<ConstInvokeDynamicInfo> parseConstInvokeDynamicInfo(ConstPoolEntryTag tag);
    std::unique_ptr<ConstClassInfo> parseConstClassInfo(ConstPoolEntryTag tag);
    std::unique_ptr<ConstUtf8Info> parseConstUtf8Info(ConstPoolEntryTag tag);
    std::unique_ptr<ConstNameAndTypeInfo> parseConstNameandtypeInfo(ConstPoolEntryTag tag);
    std::vector<FieldInfo> parseFields(const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);
    std::vector<MethodInfo> parseMethods(const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);

    std::vector<std::unique_ptr<AttrInfo>> parseAttributes(
            const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);

    std::unique_ptr<AttrInfo> parseAttr(const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);

    void parseAttrCode(
            CodeAttrInfo& codeAttr,
            const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);

    std::vector<ExceptionTableEntry> parseExceptionTable();

    void parseAttrLineNumberTable(
            LineNumberTableAttrInfo& lineNumberTableAttr,
            const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);

    std::vector<LineNumberTableEntry> parseLineNumberTable();

    void parseAttrSourcefile(
            SourceFileAttrInfo& sourcefileAttr,
            const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool);
};

std::ostream& operator<<(std::ostream& os, const ClassFile& cf);

#endif // PARSER_H
