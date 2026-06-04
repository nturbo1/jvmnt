#include "classFile.h"

#include <string_view>
#include <cassert>

FieldInfo::FieldInfo(
        u2 accessFgs,
        u2 nameIdx,
        u2 descriptorIdx,
        std::vector<std::unique_ptr<AttrInfo>> attrs)
    : accessFlags{ accessFgs },
    nameIndex{ nameIdx },
    descriptorIndex{ descriptorIdx },
    attributes{ std::move(attrs) }
{}

MethodInfo::MethodInfo(
        u2 accessFgs,
        u2 nameIdx,
        u2 descriptorIdx,
        std::vector<std::unique_ptr<AttrInfo>> attrs)
    : accessFlags{ accessFgs },
    nameIndex{ nameIdx },
    descriptorIndex{ descriptorIdx },
    attributes{ std::move(attrs) }
{}

void ClassFile::formatCheck()
{
    // The `magic` number is validated during the class file parsing.
    // If not, validate it here.

    // TODO: Validate the version support!      About `m_minor_version` and `m_major_version`

    checkThisClass();
    checkSuperClass();
    checkInterfaces();

    // TODO: VALIDATE that:
    //       - each value in the `interfaces` array must be a valid index into the
    //         `constant_pool` table.
    //       - the `constant_pool` entry at each value of `interfaces[i]`, where
    //         `0 ≤ i < interfaces_count`, must be a `CONSTANT_Class_info` structure
    //         representing an interface that is a direct superinterface of this
    //         class or interface type, in the left-to-right order given in the
    //         source for the type.
}

void ClassFile::checkThisClass()
{
    checkConstPoolIndexTypeConstClass(m_thisClass);
    std::string_view className(m_filename.begin(), m_filename.end() - 6);
    ConstClassInfo* constClass{ dynamic_cast<ConstClassInfo*>(m_constPool.at(m_thisClass - 1).get()) };
    assert(constClass != nullptr &&
            "The constant pool entry was not type checked for `ConstClassInfo` struct.");

    u2 classNameIndex{ constClass->nameIndex };
    checkConstPoolIndexTypeConstUtf8(classNameIndex);
    ConstUtf8Info* constUtf8{ dynamic_cast<ConstUtf8Info*>(m_constPool.at(classNameIndex - 1).get()) };
    assert(constUtf8 != nullptr &&
            "The constant pool entry was not type checked for `ConstUtf8Info` struct.");

    std::string_view constClassName(
            reinterpret_cast<const char*>(constUtf8->bytes.data()),
            constUtf8->bytes.size()
    );

    if (constClassName != className)
        logFatal("Class filename %s and class name %s don't match.", m_filename, constClassName);
}

void ClassFile::checkSuperClass()
{
    bool isInterface{
        static_cast<bool>( m_accessFlags & static_cast<const u2>(AccMasks::ACC_INTERFACE) )
    };

    if (m_superClass == 0)
    {
        if (isInterface)
            logFatal("In class file %s: For an interface, the value of the `super_class` item"
                    " MUST always be a valid index into the `constant_pool` table.",
                    m_filename);

        // Assumes the `this_class` field of the `ClassFile` structure is already checked.
        ConstClassInfo* thisClassInfo {
            dynamic_cast<ConstClassInfo*>(m_constPool.at(m_thisClass - 1).get())
        };
        assert(thisClassInfo != nullptr &&
                "The `this_class` field of `ClassFile` structure must have been format "
                "checked already!");

        ConstUtf8Info* constUtf8Info {
            dynamic_cast<ConstUtf8Info*>(m_constPool.at(thisClassInfo->nameIndex - 1).get())
        };
        assert(constUtf8Info != nullptr &&
                "The `this_class` field of `ClassFile` structure must have been format "
                "checked already!");

        std::string_view thisClassName(
                reinterpret_cast<const char*>(constUtf8Info->bytes.data()),
                constUtf8Info->bytes.size()
        );

        if (thisClassName != std::string_view("Object"))
            logFatal(
                    "In class file %s: Only Object class can have NO direct superclass.",
                    m_filename.c_str()
            );
    }
    else
    {
        checkConstPoolIndexTypeConstClass(m_superClass);
        ConstClassInfo* superClassInfo {
            static_cast<ConstClassInfo*>(m_constPool.at(m_superClass - 1).get())
        };
        checkConstPoolIndexTypeConstUtf8(superClassInfo->nameIndex);
        ConstUtf8Info* superClassConstUtf8Info {
            static_cast<ConstUtf8Info*>(m_constPool.at(superClassInfo->nameIndex - 1).get())
        };
        if (isInterface)
        {
            std::string_view superClassName(
                    reinterpret_cast<const char*>(superClassConstUtf8Info->bytes.data()),
                    superClassConstUtf8Info->bytes.size()
            );

            if (superClassName != std::string_view("Object"))
                logFatal(
                    "In class file %s: For an interface, the `constant_pool` entry at index "
                    "`super_class` MUST be a `CONSTANT_Class_info` structure representing the "
                    "class `Object`",
                    m_filename.c_str()
                );
        }

        // TODO: Check that:
        //       neither the direct superclass nor any of its superclasses may have
        //       the `ACC_FINAL` flag set in the `access_flags` item of its `ClassFile`
        //       structure.
    }
}

void ClassFile::checkInterfaces()
{
    for (u2 interfaceIdx : m_interfaces)
    {
        checkConstPoolIndexTypeConstClass(interfaceIdx);
    }
}

void ClassFile::checkFields()
{
    for (const FieldInfo& field : m_fields)
    {
        checkConstPoolIndexTypeConstUtf8(field.nameIndex);
        checkConstPoolIndexTypeConstUtf8(field.descriptorIndex);
        for (std::size_t i{0}; i < field.attributes.size(); ++i)
        {
            checkAttr( *(field.attributes[i].get()) );
        }
    }
}

void ClassFile::checkMethods()
{
    for (const MethodInfo& method : m_methods)
    {
        checkConstPoolIndexTypeConstUtf8(method.nameIndex);
        checkConstPoolIndexTypeConstUtf8(method.descriptorIndex);
        for (std::size_t i{0}; i < method.attributes.size(); ++i)
        {
            checkAttr( *(method.attributes[i].get()) );
        }
    }
}

void ClassFile::checkConstPoolIndex(std::size_t index)
{
    if (index < 1 || m_constPool.size() < index)
        logFatal("In class file %s: Invalid constant pool index: %zu",
                m_filename.c_str(),
                index);
}

void ClassFile::checkConstPoolIndexTypeConstClass(std::size_t index)
{
    checkConstPoolIndex(index);

    if (!dynamic_cast<ConstClassInfo*>(m_constPool.at(index - 1).get()))
        logFatal("In class file %s: Expected the constant pool entry at index %d"
                "to be of type CONSTANT_Class",
                m_filename.c_str(),
                index);
}

void ClassFile::checkConstPoolIndexTypeConstUtf8(std::size_t index)
{
    checkConstPoolIndex(index);

    if (!dynamic_cast<ConstUtf8Info*>(m_constPool.at(index - 1).get()))
        logFatal("In class file %s: Expected the constant pool entry at index %d"
                "to be of type CONSTANT_Utf8",
                m_filename.c_str(),
                index);
}
