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

namespace mdds { namespace __mtv {

/**
 * Node that represents the content of each iterator.  The private data part
 * is an implementation detail that should never be accessed externally.
 */
template<typename _SizeT, typename _ElemBlkT>
struct iterator_value_node
{
    typedef _SizeT size_type;
    typedef _ElemBlkT element_block_type;

    mdds::mtv::element_t type;
    size_type size;
    element_block_type* data;

    iterator_value_node(size_type start_pos, size_type block_index) :
        type(mdds::mtv::element_type_empty), size(0), data(NULL), __private_data(start_pos, block_index) {}

    iterator_value_node(const iterator_value_node& other) :
        type(other.type), size(other.size), data(other.data), __private_data(other.__private_data) {}

    void swap(iterator_value_node& other)
    {
        std::swap(type, other.type);
        std::swap(size, other.size);
        std::swap(data, other.data);

        __private_data.swap(other.__private_data);
    }

    struct private_data
    {
        size_type start_pos;
        size_type block_index;

        private_data() : start_pos(0), block_index(0) {}
        private_data(size_type _start_pos, size_type _block_index) :
            start_pos(_start_pos), block_index(_block_index) {}
        private_data(const private_data& other) :
            start_pos(other.start_pos), block_index(other.block_index) {}

        void swap(private_data& other)
        {
            std::swap(start_pos, other.start_pos);
            std::swap(block_index, other.block_index);
        }
    };
    private_data __private_data;
};

template<typename _NodeT>
struct private_data_no_update
{
    typedef _NodeT node_type;

    static void inc(node_type&) {}
    static void dec(node_type&) {}
};

template<typename _NodeT>
struct private_data_forward_update
{
    typedef _NodeT node_type;

    static void inc(node_type& nd)
    {
        ++nd.__private_data.block_index;
        nd.__private_data.start_pos += nd.size;
    }

    static void dec(node_type& nd)
    {
        --nd.__private_data.block_index;
        nd.__private_data.start_pos -= nd.size;
    }
};

/**
 * Common base for both const and non-const iterators.  Its protected inc()
 * and dec() methods have non-const return type, and the derived classes
 * wrap them and return values with their respective const modifiers.
 */
template<typename _Trait>
class iterator_common_base
{
protected:
    typedef typename _Trait::parent parent_type;
    typedef typename _Trait::blocks blocks_type;
    typedef typename _Trait::base_iterator base_iterator_type;

    typedef typename parent_type::size_type size_type;
    typedef iterator_value_node<size_type, typename parent_type::element_block_type> node;

    iterator_common_base() : m_blocks(NULL), m_cur_node(0, 0) {}

    iterator_common_base(
        const base_iterator_type& pos, const base_iterator_type& end,
        const blocks_type* blks, size_type start_pos, size_type block_index) :
        m_blocks(blks),
        m_cur_node(start_pos, block_index),
        m_pos(pos),
        m_end(end)
    {
        if (m_pos == m_end)
        {
            if (!blks || blks->empty())
                return;

            m_cur_node.size = blks->back()->m_size;
        }
        else
            update_node();
    }

    iterator_common_base(const iterator_common_base& other) :
        m_blocks(other.m_blocks),
        m_cur_node(other.m_cur_node),
        m_pos(other.m_pos),
        m_end(other.m_end)
    {
    }

    void update_node()
    {
#ifdef MDDS_MULTI_TYPE_VECTOR_DEBUG
        if (m_pos == m_end)
            throw general_error("Current node position should never equal the end position during node update.");
#endif
        // blocks_type::value_type is a pointer to multi_type_vector::block.
        typename blocks_type::value_type blk = *m_pos;
        if (blk->mp_data)
            m_cur_node.type = mdds::mtv::get_block_type(*blk->mp_data);
        else
            m_cur_node.type = mdds::mtv::element_type_empty;

        m_cur_node.size = blk->m_size;
        m_cur_node.data = blk->mp_data;
    }

    node* inc()
    {
        ++m_pos;
        if (m_pos == m_end)
            return NULL;

        update_node();
        return &m_cur_node;
    }

    node* dec()
    {
        --m_pos;
        update_node();
        return &m_cur_node;
    }

    const blocks_type* m_blocks;
    node m_cur_node;
    base_iterator_type m_pos;
    base_iterator_type m_end;

public:
    bool operator== (const iterator_common_base& other) const
    {
        return m_pos == other.m_pos && m_end == other.m_end;
    }

    bool operator!= (const iterator_common_base& other) const
    {
        return !operator==(other);
    }

    iterator_common_base& operator= (const iterator_common_base& other)
    {
        iterator_common_base assigned(other);
        swap(assigned);
        return *this;
    }

    void swap(iterator_common_base& other)
    {
        m_cur_node.swap(other.m_cur_node);
        std::swap(m_pos, other.m_pos);
        std::swap(m_end, other.m_end);
    }
};

template<typename _Trait, typename _NodeUpdateFunc>
class iterator_base : public iterator_common_base<_Trait>
{
    typedef _Trait trait;
    typedef _NodeUpdateFunc node_update_func;
    typedef iterator_common_base<trait> common_base;

    typedef typename trait::blocks blocks_type;
    typedef typename trait::base_iterator base_iterator_type;
    typedef typename common_base::size_type size_type;

    using common_base::inc;
    using common_base::dec;
    using common_base::m_cur_node;
    using common_base::m_pos;
    using common_base::m_end;

public:

    // iterator traits
    typedef typename common_base::node value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef ptrdiff_t   difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

public:
    iterator_base() {}
    iterator_base(
        const base_iterator_type& pos, const base_iterator_type& end,
        const blocks_type* blks, size_type start_pos, size_type block_index) :
        common_base(pos, end, blks, start_pos, block_index) {}

    iterator_base(const iterator_base& other) :
        common_base(other) {}

    value_type& operator*()
    {
        return m_cur_node;
    }

    const value_type& operator*() const
    {
        return m_cur_node;
    }

    value_type* operator->()
    {
        return &m_cur_node;
    }

    const value_type* operator->() const
    {
        return &m_cur_node;
    }

    value_type* operator++()
    {
        node_update_func::inc(m_cur_node);
        return inc();
    }

    value_type* operator--()
    {
        value_type* ret = dec();
        node_update_func::dec(m_cur_node);
        return ret;
    }

    /**
     * These method are public only to allow const_iterator_base to
     * instantiate from iterator_base.
     */
    const base_iterator_type& get_pos() const { return m_pos; }
    const base_iterator_type& get_end() const { return m_end; }
};

template<typename _Trait, typename _NonConstItrBase>
class const_iterator_base : public iterator_common_base<_Trait>
{
    typedef _Trait trait;
    typedef iterator_common_base<trait> common_base;

    typedef typename trait::blocks blocks_type;
    typedef typename trait::base_iterator base_iterator_type;
    typedef typename common_base::size_type size_type;

    using common_base::inc;
    using common_base::dec;
    using common_base::m_cur_node;

public:

    typedef _NonConstItrBase iterator_base;

    // iterator traits
    typedef typename common_base::node value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef ptrdiff_t   difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

public:
    const_iterator_base() : common_base() {}
    const_iterator_base(
        const base_iterator_type& pos, const base_iterator_type& end,
        const blocks_type* blks, size_type start_pos, size_type block_index) :
        common_base(pos, end, blks, start_pos, block_index) {}

    const_iterator_base(const const_iterator_base& other) :
        common_base(other) {}

    /**
     * Take the non-const iterator counterpart to create a const iterator.
     */
    const_iterator_base(const iterator_base& other) :
        common_base(other.get_pos(), other.get_end(), NULL, 0, 0) {}

    const value_type& operator*() const
    {
        return m_cur_node;
    }

    const value_type* operator->() const
    {
        return &m_cur_node;
    }

    const value_type* operator++()
    {
        return inc();
    }

    const value_type* operator--()
    {
        return dec();
    }
};

}}
