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

#include <stdexcept>

#if UNIT_TEST
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace mdds { namespace __gridmap {


template<typename _Trait>
column<_Trait>::block::block() : m_size(0), mp_data(NULL) {}

template<typename _Trait>
column<_Trait>::block::block(row_key_type _size) : m_size(_size), mp_data(NULL) {}

template<typename _Trait>
column<_Trait>::block::~block()
{
    cell_block_modifier::delete_block(mp_data);
}

template<typename _Trait>
column<_Trait>::column(row_key_type max_row_size) : m_cur_size(max_row_size)
{
    // Initialize with an empty block that spans from 0 to max.
    m_blocks.push_back(new block(max_row_size));
}

template<typename _Trait>
column<_Trait>::~column()
{
    std::for_each(m_blocks.begin(), m_blocks.end(), default_deleter<block>());
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::set_cell(row_key_type row, const _T& cell)
{
    if (row < 0 || row >= m_cur_size)
        throw std::out_of_range("Specified row index is out-of-bound.");

    cell_category_type cat = get_type(cell);

    // Find the right block ID from the row ID.
    row_key_type start_row = 0; // row ID of the first cell in a block.
    size_t block_index = 0;
    get_block_position(row, start_row, block_index);

    block* blk = m_blocks[block_index];
    assert(blk->m_size > 0); // block size should never be zero at any time.

    row_key_type pos_in_block = row - start_row;
    assert(pos_in_block < blk->m_size);

    if (!blk->mp_data)
    {
        // This is an empty block.
        set_cell_to_empty_block(block_index, pos_in_block, cell);
        return;
    }

    assert(blk->mp_data);
    cell_category_type blk_cat = get_block_type(*blk->mp_data);

    if (blk_cat == cat)
    {
        // This block is of the same type as the cell being inserted.
        row_key_type i = row - start_row;
        cell_block_modifier::set_value(blk->mp_data, i, cell);
        return;
    }

    assert(blk_cat != cat);

    if (row == start_row)
    {
        // Insertion point is at the start of the block.
        if (blk->m_size == 1)
        {
            set_cell_to_block_of_size_one(block_index, cell);
            return;
        }

        assert(blk->m_size > 1);
        if (block_index == 0)
        {
            // No preceding block.
            set_cell_to_top_of_data_block(0, cell);
            return;
        }

        // Append to the previous block if the types match.
        block* blk_prev = m_blocks[block_index-1];
        if (!blk_prev->mp_data)
        {
            // Previous block is empty.
            set_cell_to_top_of_data_block(block_index, cell);
            return;
        }

        cell_category_type blk_cat_prev = get_block_type(*blk_prev->mp_data);
        if (blk_cat_prev == cat)
        {
            // Append to the previous block.
            blk->m_size -= 1;
            cell_block_modifier::erase(blk->mp_data, 0);
            blk_prev->m_size += 1;
            cell_block_modifier::append_value(blk_prev->mp_data, cell);
            return;
        }

        set_cell_to_top_of_data_block(block_index, cell);
        return;
    }

    if (row < (start_row + blk->m_size - 1))
    {
        // Insertion point is somewhere in the middle of the block.
        set_cell_to_middle_of_block(block_index, pos_in_block, cell);
        return;
    }

    // Insertion point is at the end of the block.
    assert(row == (start_row + blk->m_size - 1));
    assert(row > start_row);
    assert(blk->m_size > 1);

    if (block_index == 0)
    {
        if (m_blocks.size() == 1)
        {
            // This is the only block.  Pop the last value from the
            // previous block, and insert a new block for the cell being
            // inserted.
            set_cell_to_bottom_of_data_block(0, cell);
            return;
        }

        assert(block_index < m_blocks.size()-1);
        block* blk_next = m_blocks[block_index+1];
        if (!blk_next->mp_data)
        {
            // Next block is empty.  Pop the last cell of the current
            // block, and insert a new block with the new cell.
            set_cell_to_bottom_of_data_block(0, cell);
            return;
        }

        // Next block is not empty.
        cell_category_type blk_cat_next = get_block_type(*blk_next->mp_data);
        if (blk_cat_next != cat)
        {
            set_cell_to_bottom_of_data_block(0, cell);
            return;
        }

        // Pop the last cell off the current block, and prepend the
        // new cell to the next block.
        cell_block_modifier::erase(blk->mp_data, blk->m_size-1);
        blk->m_size -= 1;
        cell_block_modifier::prepend_value(blk_next->mp_data, cell);
        return;
    }

    assert(block_index > 0);

    if (block_index == m_blocks.size()-1)
    {
        // This is the last block.
        set_cell_to_bottom_of_data_block(block_index, cell);
        return;
    }

    block* blk_next = m_blocks[block_index+1];
    if (!blk_next->mp_data)
    {
        // Next block is empty.
        set_cell_to_bottom_of_data_block(block_index, cell);
        return;
    }

    cell_category_type cat_blk_next = get_block_type(*blk_next->mp_data);
    if (cat_blk_next != cat)
    {
        // Next block is of different type than that of the cell being inserted.
        set_cell_to_bottom_of_data_block(block_index, cell);
        return;
    }

    // Pop the last element from the current block, and prepend the cell
    // into the next block.
    cell_block_modifier::erase(blk->mp_data, blk->m_size-1);
    blk->m_size -= 1;
    cell_block_modifier::prepend_value(blk_next->mp_data, cell);
    blk_next->m_size += 1;
}

template<typename _Trait>
void column<_Trait>::get_block_position(
    row_key_type row, row_key_type& start_row, size_t& block_index, size_t start_block) const
{
    start_row = 0;
    for (size_t i = start_block, n = m_blocks.size(); i < n; ++i)
    {
        const block& blk = *m_blocks[i];
        if (row < start_row + blk.m_size)
        {
            // Row is in this block.
            block_index = i;
            return;
        }

        // Specified row is not in this block.
        start_row += blk.m_size;
    }

    assert(!"Block position not found.");
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::create_new_block_with_new_cell(cell_block_type*& data, const _T& cell)
{
    cell_category_type cat = get_type(cell);

    if (data)
        cell_block_modifier::delete_block(data);

    // New cell block is always size 1.
    data = cell_block_modifier::create_new_block(cat);
    if (!data)
        throw general_error("Failed to create new block.");

    cell_block_modifier::set_value(data, 0, cell);
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::set_cell_to_middle_of_block(
    size_t block_index, row_key_type pos_in_block, const _T& cell)
{
    block* blk = m_blocks[block_index];

    assert(pos_in_block > 0 && pos_in_block < blk->m_size - 1);
    assert(blk->m_size >= 3);
    row_key_type orig_size = blk->m_size;

    m_blocks.insert(m_blocks.begin()+block_index+1, new block(1));
    block* blk_new = m_blocks[block_index+1];
    m_blocks.insert(m_blocks.begin()+block_index+2, new block(orig_size-pos_in_block-1));
    block* blk_tail = m_blocks[block_index+2];

    if (blk->mp_data)
    {
        cell_category_type blk_cat = get_block_type(*blk->mp_data);

        // Transfer the tail values from the original to the new block.
        blk_tail->mp_data = cell_block_modifier::create_new_block(blk_cat);
        cell_block_modifier::assign_values(
            blk_tail->mp_data, blk->mp_data, pos_in_block+1, orig_size-pos_in_block-1);

        // Shrink the original block.
        cell_block_modifier::resize_block(blk->mp_data, pos_in_block);
    }

    blk->m_size = pos_in_block;

    create_new_block_with_new_cell(blk_new->mp_data, cell);
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::append_cell_to_block(size_t block_index, const _T& cell)
{
    block* blk = m_blocks[block_index];
    blk->m_size += 1;
    cell_block_modifier::append_value(blk->mp_data, cell);
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::set_cell_to_empty_block(
    size_t block_index, row_key_type pos_in_block, const _T& cell)
{
    block* blk = m_blocks[block_index];

    if (block_index == 0)
    {
        // Topmost block.
        if (m_blocks.size() == 1)
        {
            // this is the only block.
            assert(blk->m_size == m_cur_size);
            if (m_cur_size == 1)
            {
                // This column is allowed to have only one row!
                assert(pos_in_block == 0);
                create_new_block_with_new_cell(blk->mp_data, cell);
            }
            else
            {
                // block has multiple rows.
                if (pos_in_block == 0)
                {
                    // Insert into the first cell in block.
                    blk->m_size -= 1;
                    assert(blk->m_size > 0);

                    m_blocks.insert(m_blocks.begin(), new block(1));
                    blk = m_blocks[block_index];
                    create_new_block_with_new_cell(blk->mp_data, cell);
                }
                else if (pos_in_block == blk->m_size - 1)
                {
                    // Insert into the last cell in block.
                    blk->m_size -= 1;
                    assert(blk->m_size > 0);

                    m_blocks.push_back(new block(1));
                    blk = m_blocks.back();

                    create_new_block_with_new_cell(blk->mp_data, cell);
                }
                else
                {
                    // Insert into the middle of the block.
                    set_cell_to_middle_of_block(block_index, pos_in_block, cell);
                }
            }
        }
        else
        {
            // this empty block is followed by a non-empty block.
            assert(block_index < m_blocks.size()-1);
            if (pos_in_block == 0)
            {
                if (blk->m_size == 1)
                {
                    // Top empty block with only one cell size.
                    block* blk_next = m_blocks[block_index+1];
                    assert(blk_next->mp_data);
                    cell_category_type cat = get_type(cell);
                    cell_category_type cat_next = get_block_type(*blk_next->mp_data);

                    if (cat == cat_next)
                    {
                        // Remove this one-cell empty block from the top, and
                        // prepend the cell to the next block.
                        delete m_blocks.front();
                        m_blocks.erase(m_blocks.begin());
                        blk = m_blocks.front();
                        blk->m_size += 1;
                        cell_block_modifier::prepend_value(blk->mp_data, cell);
                    }
                    else
                        create_new_block_with_new_cell(blk->mp_data, cell);
                }
                else
                {
                    assert(blk->m_size > 1);
                    blk->m_size -= 1;
                    m_blocks.insert(m_blocks.begin(), new block(1));
                    blk = m_blocks.front();
                    create_new_block_with_new_cell(blk->mp_data, cell);
                }
            }
            else if (pos_in_block == blk->m_size - 1)
            {
                // Immediately above a non-empty block.
                block* blk_next = m_blocks[block_index+1];
                assert(blk_next->mp_data);
                cell_category_type cat = get_type(cell);
                cell_category_type cat_next = get_block_type(*blk_next->mp_data);
                assert(blk->m_size > 1);

                if (cat == cat_next)
                {
                    // Shrink this empty block by one, and prepend the cell to the next block.
                    blk->m_size -= 1;
                    blk_next->m_size += 1;
                    cell_block_modifier::prepend_value(blk_next->mp_data, cell);
                }
                else
                {
                    blk->m_size -= 1;
                    typename blocks_type::iterator it = m_blocks.begin();
                    std::advance(it, block_index+1);
                    m_blocks.insert(it, new block(1));
                    blk = m_blocks[block_index+1];
                    create_new_block_with_new_cell(blk->mp_data, cell);
                }
            }
            else
            {
                // Inserting into the middle of an empty block.
                set_cell_to_middle_of_block(block_index, pos_in_block, cell);
            }
        }

        return;
    }

    // This empty block is right below a non-empty block.
    assert(block_index > 0 && m_blocks[block_index-1]->mp_data != NULL);

    if (pos_in_block == 0)
    {
        // New cell is right below the non-empty block.
        cell_category_type blk_cat_prev = get_block_type(*m_blocks[block_index-1]->mp_data);
        cell_category_type cat = get_type(cell);
        if (blk_cat_prev == cat)
        {
            // Extend the previous block by one to insert this cell.
            if (blk->m_size == 1)
            {
                // Check if we need to merge with the following block.
                if (block_index == m_blocks.size()-1)
                {
                    // Last block.  Delete this block and extend the previous
                    // block by one.
                    delete m_blocks[block_index];
                    m_blocks.pop_back();
                    append_cell_to_block(block_index-1, cell);
                }
                else
                {
                    // Block exists below.
                    block* blk_next = m_blocks[block_index+1];
                    cell_block_type* data_next = blk_next->mp_data;
                    assert(data_next); // Empty block must not be followed by another empty block.
                    cell_category_type blk_cat_next = get_block_type(*data_next);
                    if (blk_cat_prev == blk_cat_next)
                    {
                        // We need to merge the previous and next blocks, then
                        // delete the current and next blocks.
                        block* blk_prev = m_blocks[block_index-1];
                        blk_prev->m_size += 1 + blk_next->m_size;
                        cell_block_modifier::append_value(blk_prev->mp_data, cell);
                        cell_block_modifier::append_values(blk_prev->mp_data, data_next);

                        delete blk;
                        delete blk_next;
                        typename blocks_type::iterator it = m_blocks.begin() + block_index;
                        m_blocks.erase(it, it+2);
                    }
                    else
                    {
                        // Ignore the next block. Just extend the previous block.
                        delete m_blocks[block_index];
                        m_blocks.erase(m_blocks.begin() + block_index);
                        append_cell_to_block(block_index-1, cell);
                    }
                }
            }
            else
            {
                // Extend the previous block to append the cell.
                assert(blk->m_size > 1);
                blk->m_size -= 1;
                append_cell_to_block(block_index-1, cell);
            }
        }
        else
        {
            // Cell type is different from the type of the previous block.
            if (blk->m_size == 1)
            {
                if (block_index == m_blocks.size()-1)
                {
                    // There is no more block below.
                    create_new_block_with_new_cell(blk->mp_data, cell);
                }
                else
                {
                    // Check the type of the following non-empty block.
                    assert(block_index < m_blocks.size()-1);
                    block* blk_next = m_blocks[block_index+1];
                    assert(blk_next->mp_data);
                    cell_category_type blk_cat_next = get_block_type(*blk_next->mp_data);
                    if (cat == blk_cat_next)
                    {
                        // Remove this empty block, and prepend the cell to the next block.
                        blk_next->m_size += 1;
                        cell_block_modifier::prepend_value(blk_next->mp_data, cell);
                        delete m_blocks[block_index];
                        m_blocks.erase(m_blocks.begin()+block_index);
                    }
                    else
                        create_new_block_with_new_cell(blk->mp_data, cell);
                }
            }
            else
            {
                create_new_block_with_new_cell(blk->mp_data, cell);
                m_blocks.push_back(new block(blk->m_size-1));
                blk->m_size = 1;
            }
        }
    }
    else if (pos_in_block == blk->m_size - 1)
    {
        // New cell is at the last cell position.
        assert(blk->m_size > 1);
        if (block_index == m_blocks.size()-1)
        {
            // This is the last block.
            blk->m_size -= 1;
            m_blocks.push_back(new block(1));
            blk = m_blocks.back();
            create_new_block_with_new_cell(blk->mp_data, cell);
        }
        else
        {
            // A non-empty block exists below.
            cell_category_type cat = get_type(cell);
            block* blk_next = m_blocks[block_index+1];
            assert(blk_next->mp_data);
            cell_category_type blk_cat_next = get_block_type(*blk_next->mp_data);
            if (cat == blk_cat_next)
            {
                // Shrink this empty block and extend the next block.
                blk->m_size -= 1;
                blk_next->m_size += 1;
                cell_block_modifier::prepend_value(blk_next->mp_data, cell);
            }
            else
            {
                // Just insert this new cell.
                blk->m_size -= 1;
                m_blocks.insert(m_blocks.begin()+1, new block(1));
                blk = m_blocks[block_index+1];
                create_new_block_with_new_cell(blk->mp_data, cell);
            }
        }
    }
    else
    {
        // New cell is somewhere in the middle of an empty block.
        set_cell_to_middle_of_block(block_index, pos_in_block, cell);
    }
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::set_cell_to_block_of_size_one(size_t block_index, const _T& cell)
{
    block* blk = m_blocks[block_index];
    assert(blk->m_size == 1);
    assert(blk->mp_data);
    cell_category_type cat = get_type(cell);
    cell_category_type blk_cat = get_block_type(*blk->mp_data);
    assert(blk_cat != cat);

    if (block_index == 0)
    {
        // This is the topmost block of size 1.
        if (block_index == m_blocks.size()-1)
        {
            // This is the only block.
            create_new_block_with_new_cell(blk->mp_data, cell);
            return;
        }

        // There is an existing block below.
        block* blk_next = m_blocks[block_index+1];
        if (!blk_next->mp_data)
        {
            // Next block is empty.
            create_new_block_with_new_cell(blk->mp_data, cell);
            return;
        }

        // Next block is not empty.
        cell_category_type blk_cat_next = get_block_type(*blk_next->mp_data);
        if (blk_cat_next != cat)
        {
            // Cell being inserted is of different type than that of the next block.
            create_new_block_with_new_cell(blk->mp_data, cell);
            return;
        }

        // Delete the current block, and prepend the cell to the next block.
        blk_next->m_size += 1;
        cell_block_modifier::prepend_value(blk_next->mp_data, cell);
        delete blk;
        m_blocks.erase(m_blocks.begin()+block_index);
        return;
    }

    assert(block_index > 0);

    if (block_index == m_blocks.size()-1)
    {
        // This is the last block, and a block exists above.
        block* blk_prev = m_blocks[block_index-1];
        if (!blk_prev->mp_data)
        {
            // Previous block is empty. Replace the current block with a new one.
            create_new_block_with_new_cell(blk->mp_data, cell);
            return;
        }

        cell_category_type blk_cat_prev = get_block_type(*blk_prev->mp_data);
        if (blk_cat_prev == cat)
        {
            // Append the cell to the previos block, and remove the
            // current block.
            cell_block_modifier::append_value(blk_prev->mp_data, cell);
            blk_prev->m_size += 1;
            delete blk;
            m_blocks.erase(m_blocks.begin()+block_index);
            return;
        }

        // Simply replace the current block with a new block of new type.
        create_new_block_with_new_cell(blk->mp_data, cell);
        return;
    }

    // Remove the current block, and check if the cell can be append
    // to the previous block, or prepended to the following block.
    // Also check if the blocks above and below need to be combined.

    block* blk_prev = m_blocks[block_index-1];
    block* blk_next = m_blocks[block_index+1];
    if (!blk_prev->mp_data)
    {
        // Previous block is empty.
        if (!blk_next->mp_data)
        {
            // Next block is empty too.
            create_new_block_with_new_cell(blk->mp_data, cell);
            return;
        }

        // Previous block is empty, but the next block is not.
        cell_category_type blk_cat_next = get_block_type(*blk_next->mp_data);
        if (blk_cat_next == cat)
        {
            // Delete the current block, and prepend the new cell to the next block.
            delete blk;
            m_blocks.erase(m_blocks.begin()+block_index);
            blk = m_blocks[block_index];
            blk->m_size += 1;
            cell_block_modifier::prepend_value(blk->mp_data, cell);
            return;
        }

        assert(blk_cat_next != cat);
        create_new_block_with_new_cell(blk->mp_data, cell);
        return;
    }

    if (!blk_next->mp_data)
    {
        // Next block is empty.
        assert(blk_prev->mp_data);
        cell_category_type blk_cat_prev = get_block_type(*blk_prev->mp_data);
        if (blk_cat_prev == cat)
        {
            // Append to the previous block.
            blk_prev->m_size += 1;
            cell_block_modifier::append_value(blk_prev->mp_data, cell);
            delete blk;
            m_blocks.erase(m_blocks.begin()+block_index);
            return;
        }

        // Just overwrite the current block.
        create_new_block_with_new_cell(blk->mp_data, cell);
        return;
    }

    assert(blk_prev && blk_prev->mp_data);
    assert(blk_next && blk_next->mp_data);
    cell_category_type blk_cat_prev = get_block_type(*blk_prev->mp_data);
    cell_category_type blk_cat_next = get_block_type(*blk_next->mp_data);

    if (blk_cat_prev == blk_cat_next)
    {
        if (blk_cat_prev == cat)
        {
            // Merge the previous block with the cell being inserted and
            // the next block.
            blk_prev->m_size += 1 + blk_next->m_size;
            cell_block_modifier::append_value(blk_prev->mp_data, cell);
            cell_block_modifier::append_values(blk_prev->mp_data, blk_next->mp_data);

            // Delete the current and next blocks.
            delete blk;
            delete blk_next;
            typename blocks_type::iterator it = m_blocks.begin() + block_index;
            typename blocks_type::iterator it_last = it + 2;
            m_blocks.erase(it, it_last);
            return;
        }

        // Just overwrite the current block.
        create_new_block_with_new_cell(blk->mp_data, cell);
        return;
    }

    assert(blk_cat_prev != blk_cat_next);

    if (blk_cat_prev == cat)
    {
        // Append to the previous block.
        blk_prev->m_size += 1;
        cell_block_modifier::append_value(blk_prev->mp_data, cell);
        delete blk;
        m_blocks.erase(m_blocks.begin()+block_index);
        return;
    }

    if (blk_cat_next == cat)
    {
        // Prepend to the next block.
        blk_next->m_size += 1;
        cell_block_modifier::prepend_value(blk_next->mp_data, cell);
        delete blk;
        m_blocks.erase(m_blocks.begin()+block_index);
        return;
    }

    // Just overwrite the current block.
    create_new_block_with_new_cell(blk->mp_data, cell);
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::set_cell_to_top_of_data_block(size_t block_index, const _T& cell)
{
    block* blk = m_blocks[block_index];
    blk->m_size -= 1;
    cell_block_modifier::erase(blk->mp_data, 0);
    m_blocks.insert(m_blocks.begin()+block_index, new block(1));
    blk = m_blocks[block_index];
    create_new_block_with_new_cell(blk->mp_data, cell);
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::set_cell_to_bottom_of_data_block(size_t block_index, const _T& cell)
{
    assert(block_index < m_blocks.size());
    block* blk = m_blocks[block_index];
    cell_block_modifier::erase(blk->mp_data, blk->m_size-1);
    blk->m_size -= 1;
    m_blocks.insert(m_blocks.begin()+block_index+1, new block(1));
    blk = m_blocks[block_index+1];
    create_new_block_with_new_cell(blk->mp_data, cell);
}

template<typename _Trait>
template<typename _T>
void column<_Trait>::get_cell(row_key_type row, _T& cell) const
{
    if (row >= m_cur_size)
        throw std::out_of_range("Specified row index is out-of-bound.");

    row_key_type start_row = 0;
    size_t block_index = static_cast<size_t>(-1);
    get_block_position(row, start_row, block_index);
    const block* blk = m_blocks[block_index];
    assert(blk);

    if (!blk->mp_data)
    {
        // empty cell block.
        cell_block_modifier::get_empty_value(cell);
        return;
    }

    assert(row >= start_row);
    assert(blk->mp_data); // data for non-empty blocks should never be NULL.
    row_key_type idx = row - start_row;
    cell_block_modifier::get_value(blk->mp_data, idx, cell);
}

template<typename _Trait>
bool column<_Trait>::is_empty(row_key_type row) const
{
    if (row < 0 || row >= m_cur_size)
        throw std::out_of_range("Specified row index is out-of-bound.");

    row_key_type start_row;
    size_t block_index;
    get_block_position(row, start_row, block_index);

    return m_blocks[block_index]->mp_data == NULL;
}

template<typename _Trait>
void column<_Trait>::set_empty(row_key_type start_row, row_key_type end_row)
{
    if (start_row > end_row)
        throw std::out_of_range("Start row is larger than the end row.");

    if (start_row < 0 || end_row >= m_cur_size)
        throw std::out_of_range("Specified range is not permitted.");

    row_key_type start_row_in_block1, start_row_in_block2;
    size_t block_pos1, block_pos2;
    get_block_position(start_row, start_row_in_block1, block_pos1);
    get_block_position(end_row, start_row_in_block2, block_pos2, block_pos1);

    if (block_pos1 == block_pos2)
    {
        set_empty_in_single_block(start_row, end_row, block_pos1, start_row_in_block1);
        return;
    }

    assert(block_pos1 < block_pos2);

    {
        // Empty the lower part of the first block.
        block* blk = m_blocks[block_pos1];
        if (blk->mp_data)
        {
            if (start_row_in_block1 == start_row)
            {
                // Empty the whole block.
                cell_block_modifier::delete_block(blk->mp_data);
                blk->mp_data = NULL;
            }
            else
            {
                // Empty the lower part.
                size_t new_size = start_row - start_row_in_block1;
                cell_block_modifier::resize_block(blk->mp_data, new_size);
                blk->m_size = new_size;
            }
        }
    }

    {
        // Empty the upper part of the last block.
        block* blk = m_blocks[block_pos2];
        if (blk->mp_data)
        {
            row_key_type last_row_in_block = start_row_in_block2 + blk->m_size - 1;
            if (last_row_in_block == end_row)
            {
                // Delete the whole block.
                delete blk;
                m_blocks.erase(m_blocks.begin()+block_pos2);
            }
            else
            {
                // Empty the upper part.
                row_key_type size_to_erase = end_row - start_row_in_block2 + 1;
                cell_block_modifier::erase(blk->mp_data, 0, size_to_erase);
                blk->m_size -= size_to_erase;
            }
        }
    }

    if (block_pos2 - block_pos1 > 1)
    {
        // Remove all blocks in-between, from block_pos1+1 to block_pos2-1.

        for (size_t i = block_pos1 + 1; i < block_pos2; ++i)
            delete m_blocks[i];

        typename blocks_type::iterator it = m_blocks.begin() + block_pos1 + 1;
        typename blocks_type::iterator it_end = m_blocks.begin() + block_pos2;
        m_blocks.erase(it, it_end);
    }

    // Insert a single empty block.
    block* blk = m_blocks[block_pos1];
    row_key_type empty_block_size = end_row - start_row + 1;
    if (blk->mp_data)
    {
        // Insert a new empty block after the first block.
        m_blocks.insert(m_blocks.begin()+block_pos1+1, new block(empty_block_size));
    }
    else
    {
        // Current block is already empty. Just extend its size.
        blk->m_size = empty_block_size;
    }
}

template<typename _Trait>
void column<_Trait>::set_empty_in_single_block(
    row_key_type start_row, row_key_type end_row, size_t block_index, row_key_type start_row_in_block)
{
    // Range is within a single block.
    block* blk = m_blocks[block_index];
    if (!blk->mp_data)
        // This block is already empty.  Do nothing.
        return;

    row_key_type end_row_in_block = start_row_in_block + blk->m_size - 1;
    size_t empty_block_size = end_row - start_row + 1;

    if (start_row == start_row_in_block)
    {
        // start row coincides with the start of a block.

        if (end_row == end_row_in_block)
        {
            // Set the whole block empty.
            cell_block_modifier::delete_block(blk->mp_data);
            blk->mp_data = NULL;
            return;
        }

        // Set the upper part of the block empty.
        cell_block_modifier::erase(blk->mp_data, 0, empty_block_size);
        blk->m_size -= empty_block_size;

        // Insert a new empty block before the current one.
        m_blocks.insert(m_blocks.begin()+block_index, new block(empty_block_size));
        return;
    }

    if (end_row == end_row_in_block)
    {
        // end row coincides with the end of a block.
        assert(start_row > start_row_in_block);

        // Set the lower part of the block empty.
        cell_block_modifier::erase(blk->mp_data, end_row_in_block-empty_block_size+1, empty_block_size);
        blk->m_size -= empty_block_size;

        // Insert a new empty block after the current one.
        m_blocks.insert(m_blocks.begin()+block_index+1, new block(empty_block_size));
        return;
    }

    // Empty the middle part of a block.
    assert(end_row_in_block - end_row > 0);

    // First, insert two new blocks at position past the current block.
    row_key_type lower_block_size = end_row_in_block - end_row;
    m_blocks.insert(m_blocks.begin()+block_index+1, 2, NULL);
    m_blocks[block_index+1] = new block(empty_block_size); // empty block.
    m_blocks[block_index+2] = new block(lower_block_size);

    // Copy the lower values from the current block to the new non-empty block.
    block* blk_lower = m_blocks[block_index+2];
    assert(blk_lower->m_size == lower_block_size);
    cell_category_type blk_cat = get_block_type(*blk->mp_data);
    blk_lower->mp_data = cell_block_modifier::create_new_block(blk_cat);
    cell_block_modifier::assign_values(
        blk_lower->mp_data, blk->mp_data, end_row_in_block-lower_block_size+1, lower_block_size);

    // Shrink the current data block.
    cell_block_modifier::erase(
        blk->mp_data, start_row-start_row_in_block, end_row_in_block-start_row+1);
    blk->m_size = start_row - start_row_in_block;
}

}}
