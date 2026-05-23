#ifndef PARSER_H
#define PARSER_H

#include "class_file.h"
#include "class_reader.h"

#include <string>

class ClassFileParser
{
public:
    explicit ClassFileParser(ClassFileReader& reader);

    ClassFile parse();

private:
    ClassFileReader& m_reader;

    std::vector<std::unique_ptr<ConstPoolEntry>> parse_const_pool();
    std::unique_ptr<ConstMethodrefInfo> parse_const_methodref_info(ConstPoolEntryTag tag);
    std::unique_ptr<ConstFieldrefInfo> parse_const_fieldref_info(ConstPoolEntryTag tag);
    std::unique_ptr<ConstDoubleInfo> parse_const_double_info(ConstPoolEntryTag tag);
    std::unique_ptr<ConstInvokeDynamicInfo> parse_const_invoke_dynamic_info(ConstPoolEntryTag tag);
    std::unique_ptr<ConstClassInfo> parse_const_class_info(ConstPoolEntryTag tag);
    std::unique_ptr<ConstUtf8Info> parse_const_utf8_info(ConstPoolEntryTag tag);
};

std::ostream& operator<<(std::ostream& os, const ClassFile& cf);

#endif // PARSER_H
