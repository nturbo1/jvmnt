#include "class_file.h"

FieldInfo::FieldInfo(
        u2 access_fgs,
        u2 name_idx,
        u2 descriptor_idx,
        std::vector<std::unique_ptr<AttrInfo>> attrs)
    : access_flags{ access_fgs },
    name_index{ name_idx },
    descriptor_index{ descriptor_idx },
    attributes{ std::move(attrs) }
{}

MethodInfo::MethodInfo(
        u2 access_fgs,
        u2 name_idx,
        u2 descriptor_idx,
        std::vector<std::unique_ptr<AttrInfo>> attrs)
    : access_flags{ access_fgs },
    name_index{ name_idx },
    descriptor_index{ descriptor_idx },
    attributes{ std::move(attrs) }
{}
