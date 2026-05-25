#include "class_file_validation.h"
#include "base.h"

#include <vector>
#include <memory>

void validate_const_pool_index(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        u2 index)
{
    std::size_t const_pool_size{ const_pool.size() };

    // The class file format uses `1-based` indexes.
    // However, the parsed `const_pool` vector stores the parsed constant pool items
    // in usual `0-based` indexes internally, so
    //     class file `index` == parsed `const_pool` vector `index - 1`.
    if (index < 1 || index > const_pool_size)
    {
        log_fatal("%d is an invalid index into the `constant_pool` table.", index);
    }
}
