#ifndef PARSER_CLASS_FILE_VALIDATION_H
#define PARSER_CLASS_FILE_VALIDATION_H

#include "base.h"
#include "class_file.h"

#include <vector>
#include <memory>

/*
 * Checks whether a given `index` is a valid index into the given `const_pool`.
 *
 * If the `index` is invalid, then throws a `std::runtime_error` exception,
 * which should not be caught leading to the termination of the program.
 */
void validate_const_pool_index(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        u2 index);

#endif // PARSER_CLASS_FILE_VALIDATION_H
