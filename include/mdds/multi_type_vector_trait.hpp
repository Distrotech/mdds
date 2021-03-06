/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __MDDS_MULTI_TYPE_VECTOR_TRAIT_HPP__
#define __MDDS_MULTI_TYPE_VECTOR_TRAIT_HPP__

#include "multi_type_vector_types.hpp"

#include <vector>

namespace mdds { namespace mtv {

struct element_block_func_base
{
    inline static base_element_block* create_new_block(element_t type, size_t init_size);

    inline static base_element_block* clone_block(const base_element_block& block);

    inline static void delete_block(const base_element_block* p);

    inline static void resize_block(base_element_block& block, size_t new_size);

    inline static void print_block(const base_element_block& block);

    inline static void erase(base_element_block& block, size_t pos);

    inline static void erase(base_element_block& block, size_t pos, size_t size);

    inline static void append_values_from_block(base_element_block& dest, const base_element_block& src);

    inline static void append_values_from_block(
        base_element_block& dest, const base_element_block& src, size_t begin_pos, size_t len);

    inline static void assign_values_from_block(
        base_element_block& dest, const base_element_block& src, size_t begin_pos, size_t len);

    inline static void prepend_values_from_block(
        base_element_block& dest, const base_element_block& src, size_t begin_pos, size_t len);

    inline static void swap_values(
        base_element_block& blk1, base_element_block& blk2, size_t pos1, size_t pos2, size_t len);

    inline static bool equal_block(const base_element_block& left, const base_element_block& right);

    /**
     * This method gets called when cell values are being overwritten by new
     * values.  This provides the client code an opportunity to delete
     * overwritten instances in case the block stores pointers to managed
     * objects.  For blocks that don't need to manage their stored objects (or
     * store primitive values), this method can be left empty.
     */
    inline static void overwrite_values(base_element_block& block, size_t pos, size_t len);

    inline static void shrink_to_fit(base_element_block& block);
};

base_element_block* element_block_func_base::create_new_block(element_t type, size_t init_size)
{
    switch (type)
    {
        case element_type_numeric:
            return numeric_element_block::create_block(init_size);
        case element_type_string:
            return string_element_block::create_block(init_size);
        case element_type_short:
            return short_element_block::create_block(init_size);
        case element_type_ushort:
            return ushort_element_block::create_block(init_size);
        case element_type_int:
            return int_element_block::create_block(init_size);
        case element_type_uint:
            return uint_element_block::create_block(init_size);
        case element_type_long:
            return long_element_block::create_block(init_size);
        case element_type_ulong:
            return ulong_element_block::create_block(init_size);
        case element_type_boolean:
            return boolean_element_block::create_block(init_size);
        case element_type_char:
            return char_element_block::create_block(init_size);
        case element_type_uchar:
            return uchar_element_block::create_block(init_size);
        default:
            throw general_error("create_new_block: failed to create a new block of unknown type.");
    }
}

base_element_block* element_block_func_base::clone_block(const base_element_block& block)
{
    switch (get_block_type(block))
    {
        case element_type_numeric:
            return numeric_element_block::clone_block(block);
        case element_type_string:
            return string_element_block::clone_block(block);
        case element_type_short:
            return short_element_block::clone_block(block);
        case element_type_ushort:
            return ushort_element_block::clone_block(block);
        case element_type_int:
            return int_element_block::clone_block(block);
        case element_type_uint:
            return uint_element_block::clone_block(block);
        case element_type_long:
            return long_element_block::clone_block(block);
        case element_type_ulong:
            return ulong_element_block::clone_block(block);
        case element_type_boolean:
            return boolean_element_block::clone_block(block);
        case element_type_char:
            return char_element_block::clone_block(block);
        case element_type_uchar:
            return uchar_element_block::clone_block(block);
        default:
            throw general_error("clone_block: failed to clone a block of unknown type.");
    }
}

void element_block_func_base::delete_block(const base_element_block* p)
{
    if (!p)
        return;

    switch (get_block_type(*p))
    {
        case element_type_numeric:
            numeric_element_block::delete_block(p);
        break;
        case element_type_string:
            string_element_block::delete_block(p);
        break;
        case element_type_short:
            short_element_block::delete_block(p);
        break;
        case element_type_ushort:
            ushort_element_block::delete_block(p);
        break;
        case element_type_int:
            int_element_block::delete_block(p);
        break;
        case element_type_uint:
            uint_element_block::delete_block(p);
        break;
        case element_type_long:
            long_element_block::delete_block(p);
        break;
        case element_type_ulong:
            ulong_element_block::delete_block(p);
        break;
        case element_type_boolean:
            boolean_element_block::delete_block(p);
        break;
        case element_type_char:
            char_element_block::delete_block(p);
        break;
        case element_type_uchar:
            uchar_element_block::delete_block(p);
        break;
        default:
            throw general_error("delete_block: failed to delete a block of unknown type.");
    }
}

void element_block_func_base::resize_block(base_element_block& block, size_t new_size)
{
    switch (get_block_type(block))
    {
        case element_type_numeric:
            numeric_element_block::resize_block(block, new_size);
        break;
        case element_type_string:
            string_element_block::resize_block(block, new_size);
        break;
        case element_type_short:
            short_element_block::resize_block(block, new_size);
        break;
        case element_type_ushort:
            ushort_element_block::resize_block(block, new_size);
        break;
        case element_type_int:
            int_element_block::resize_block(block, new_size);
        break;
        case element_type_uint:
            uint_element_block::resize_block(block, new_size);
        break;
        case element_type_long:
            long_element_block::resize_block(block, new_size);
        break;
        case element_type_ulong:
            ulong_element_block::resize_block(block, new_size);
        break;
        case element_type_boolean:
            boolean_element_block::resize_block(block, new_size);
        break;
        case element_type_char:
            char_element_block::resize_block(block, new_size);
        break;
        case element_type_uchar:
            uchar_element_block::resize_block(block, new_size);
        break;
        default:
            throw general_error("resize_block: failed to resize a block of unknown type.");
    }
}

void element_block_func_base::print_block(const base_element_block& block)
{
    switch (get_block_type(block))
    {
        case element_type_numeric:
            numeric_element_block::print_block(block);
        break;
        case element_type_string:
            string_element_block::print_block(block);
        break;
        case element_type_short:
            short_element_block::print_block(block);
        break;
        case element_type_ushort:
            ushort_element_block::print_block(block);
        break;
        case element_type_int:
            int_element_block::print_block(block);
        break;
        case element_type_uint:
            uint_element_block::print_block(block);
        break;
        case element_type_long:
            long_element_block::print_block(block);
        break;
        case element_type_ulong:
            ulong_element_block::print_block(block);
        break;
        case element_type_boolean:
            boolean_element_block::print_block(block);
        break;
        case element_type_char:
            char_element_block::print_block(block);
        break;
        case element_type_uchar:
            uchar_element_block::print_block(block);
        break;
        default:
            throw general_error("print_block: failed to print a block of unknown type.");
    }
}

void element_block_func_base::erase(base_element_block& block, size_t pos)
{
    switch (get_block_type(block))
    {
        case element_type_numeric:
            numeric_element_block::erase_block(block, pos);
        break;
        case element_type_string:
            string_element_block::erase_block(block, pos);
        break;
        case element_type_short:
            short_element_block::erase_block(block, pos);
        break;
        case element_type_ushort:
            ushort_element_block::erase_block(block, pos);
        break;
        case element_type_int:
            int_element_block::erase_block(block, pos);
        break;
        case element_type_uint:
            uint_element_block::erase_block(block, pos);
        break;
        case element_type_long:
            long_element_block::erase_block(block, pos);
        break;
        case element_type_ulong:
            ulong_element_block::erase_block(block, pos);
        break;
        case element_type_boolean:
            boolean_element_block::erase_block(block, pos);
        break;
        case element_type_char:
            char_element_block::erase_block(block, pos);
        break;
        case element_type_uchar:
            uchar_element_block::erase_block(block, pos);
        break;
        default:
            throw general_error("erase: failed to erase an element from a block of unknown type.");
    }
}

void element_block_func_base::erase(base_element_block& block, size_t pos, size_t size)
{
    switch (get_block_type(block))
    {
        case element_type_numeric:
            numeric_element_block::erase_block(block, pos, size);
        break;
        case element_type_string:
            string_element_block::erase_block(block, pos, size);
        break;
        case element_type_short:
            short_element_block::erase_block(block, pos, size);
        break;
        case element_type_ushort:
            ushort_element_block::erase_block(block, pos, size);
        break;
        case element_type_int:
            int_element_block::erase_block(block, pos, size);
        break;
        case element_type_uint:
            uint_element_block::erase_block(block, pos, size);
        break;
        case element_type_long:
            long_element_block::erase_block(block, pos, size);
        break;
        case element_type_ulong:
            ulong_element_block::erase_block(block, pos, size);
        break;
        case element_type_boolean:
            boolean_element_block::erase_block(block, pos, size);
        break;
        case element_type_char:
            char_element_block::erase_block(block, pos, size);
        break;
        case element_type_uchar:
            uchar_element_block::erase_block(block, pos, size);
        break;
        default:
            throw general_error("erase: failed to erase elements from a block of unknown type.");
    }
}

void element_block_func_base::append_values_from_block(base_element_block& dest, const base_element_block& src)
{
    switch (get_block_type(dest))
    {
        case element_type_numeric:
            numeric_element_block::append_values_from_block(dest, src);
        break;
        case element_type_string:
            string_element_block::append_values_from_block(dest, src);
        break;
        case element_type_short:
            short_element_block::append_values_from_block(dest, src);
        break;
        case element_type_ushort:
            ushort_element_block::append_values_from_block(dest, src);
        break;
        case element_type_int:
            int_element_block::append_values_from_block(dest, src);
        break;
        case element_type_uint:
            uint_element_block::append_values_from_block(dest, src);
        break;
        case element_type_long:
            long_element_block::append_values_from_block(dest, src);
        break;
        case element_type_ulong:
            ulong_element_block::append_values_from_block(dest, src);
        break;
        case element_type_boolean:
            boolean_element_block::append_values_from_block(dest, src);
        break;
        case element_type_char:
            char_element_block::append_values_from_block(dest, src);
        break;
        case element_type_uchar:
            uchar_element_block::append_values_from_block(dest, src);
        break;
        default:
            throw general_error("append_values: failed to append values to a block of unknown type.");
    }
}

void element_block_func_base::append_values_from_block(
    base_element_block& dest, const base_element_block& src, size_t begin_pos, size_t len)
{
    switch (get_block_type(dest))
    {
        case element_type_numeric:
            numeric_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_string:
            string_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_short:
            short_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_ushort:
            ushort_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_int:
            int_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_uint:
            uint_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_long:
            long_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_ulong:
            ulong_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_boolean:
            boolean_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_char:
            char_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_uchar:
            uchar_element_block::append_values_from_block(dest, src, begin_pos, len);
        break;
        default:
            throw general_error("append_values: failed to append values to a block of unknown type.");
    }
}

void element_block_func_base::assign_values_from_block(
    base_element_block& dest, const base_element_block& src, size_t begin_pos, size_t len)
{
    switch (get_block_type(dest))
    {
        case element_type_numeric:
            numeric_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_string:
            string_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_short:
            short_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_ushort:
            ushort_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_int:
            int_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_uint:
            uint_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_long:
            long_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_ulong:
            ulong_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_boolean:
            boolean_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_char:
            char_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_uchar:
            uchar_element_block::assign_values_from_block(dest, src, begin_pos, len);
        break;
        default:
            throw general_error("assign_values_from_block: failed to assign values to a block of unknown type.");
    }
}

void element_block_func_base::prepend_values_from_block(
    base_element_block& dest, const base_element_block& src, size_t begin_pos, size_t len)
{
    switch (get_block_type(dest))
    {
        case element_type_numeric:
            numeric_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_string:
            string_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_short:
            short_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_ushort:
            ushort_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_int:
            int_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_uint:
            uint_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_long:
            long_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_ulong:
            ulong_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_boolean:
            boolean_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_char:
            char_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        case element_type_uchar:
            uchar_element_block::prepend_values_from_block(dest, src, begin_pos, len);
        break;
        default:
            throw general_error("prepend_values_from_block: failed to prepend values to a block of unknown type.");
    }
}

void element_block_func_base::swap_values(
    base_element_block& blk1, base_element_block& blk2, size_t pos1, size_t pos2, size_t len)
{
    element_t blk1_type = get_block_type(blk1);
    assert(blk1_type == get_block_type(blk2));
    switch (blk1_type)
    {
        case element_type_numeric:
            numeric_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_string:
            string_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_short:
            short_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_ushort:
            ushort_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_int:
            int_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_uint:
            uint_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_long:
            long_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_ulong:
            ulong_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_boolean:
            boolean_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_char:
            char_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        case element_type_uchar:
            uchar_element_block::swap_values(blk1, blk2, pos1, pos2, len);
        break;
        default:
            throw general_error("swap_values: block of unknown type.");
    }
}

bool element_block_func_base::equal_block(const base_element_block& left, const base_element_block& right)
{
    element_t block_type = get_block_type(left);
    if (block_type != get_block_type(right))
        return false;

    switch (block_type)
    {
        case element_type_numeric:
            return numeric_element_block::get(left) == numeric_element_block::get(right);
        case element_type_string:
            return string_element_block::get(left) == string_element_block::get(right);
        case element_type_short:
            return short_element_block::get(left) == short_element_block::get(right);
        case element_type_ushort:
            return ushort_element_block::get(left) == ushort_element_block::get(right);
        case element_type_int:
            return int_element_block::get(left) == int_element_block::get(right);
        case element_type_uint:
            return uint_element_block::get(left) == uint_element_block::get(right);
        case element_type_long:
            return long_element_block::get(left) == long_element_block::get(right);
        case element_type_ulong:
            return ulong_element_block::get(left) == ulong_element_block::get(right);
        case element_type_boolean:
            return boolean_element_block::get(left) == boolean_element_block::get(right);
        case element_type_char:
            return char_element_block::get(left) == char_element_block::get(right);
        case element_type_uchar:
            return uchar_element_block::get(left) == uchar_element_block::get(right);
        default:
            ;
    }
    return false;
}

void element_block_func_base::overwrite_values(base_element_block&, size_t, size_t)
{
    // Do nothing for the standard types.
}

void element_block_func_base::shrink_to_fit(base_element_block& block)
{
    switch (get_block_type(block))
    {
        case element_type_numeric:
            numeric_element_block::shrink_to_fit(block);
        break;
        case element_type_string:
            string_element_block::shrink_to_fit(block);
        break;
        case element_type_short:
            short_element_block::shrink_to_fit(block);
        break;
        case element_type_ushort:
            ushort_element_block::shrink_to_fit(block);
        break;
        case element_type_int:
            int_element_block::shrink_to_fit(block);
        break;
        case element_type_uint:
            uint_element_block::shrink_to_fit(block);
        break;
        case element_type_long:
            long_element_block::shrink_to_fit(block);
        break;
        case element_type_ulong:
            ulong_element_block::shrink_to_fit(block);
        break;
        case element_type_boolean:
            boolean_element_block::shrink_to_fit(block);
        break;
        case element_type_char:
            char_element_block::shrink_to_fit(block);
        break;
        case element_type_uchar:
            uchar_element_block::shrink_to_fit(block);
        break;
        default:
            throw general_error("shrink_to_fit: failed to print a block of unknown type.");
    }
}

/**
 * Default cell block function definitions.  Implementation can use this if
 * it only uses the default block types implemented by the library.
 */
struct element_block_func : public element_block_func_base {};

}}

#endif
