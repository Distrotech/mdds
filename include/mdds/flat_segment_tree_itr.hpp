/*************************************************************************
 *
 * Copyright (c) 2010-2014 Kohei Yoshida
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

#ifndef __MDDS_FLAT_SEGMENT_TREE_ITR_HPP__
#define __MDDS_FLAT_SEGMENT_TREE_ITR_HPP__

namespace mdds { namespace __fst {

/**
 * Handler for forward iterator
 */
template<typename _FstType>
struct itr_forward_handler
{
    typedef _FstType fst_type;

    static const typename fst_type::node* init_pos(const fst_type* _db, bool _end)
    {
        return _end ? _db->m_right_leaf.get() : _db->m_left_leaf.get();
    }

    static void inc(const fst_type* _db, const typename fst_type::node*& p, bool& end)
    {
        if (p == _db->m_right_leaf.get())
            end = true;
        else
            p = p->next.get();
    }

    static void dec(const typename fst_type::node*& p, bool& end)
    {
        if (end)
            end = false;
        else
            p = p->prev.get();
    }
};

/**
 * Handler for reverse iterator
 */
template<typename _FstType>
struct itr_reverse_handler
{
    typedef _FstType fst_type;

    static const typename fst_type::node* init_pos(const fst_type* _db, bool _end)
    {
        return _end ? _db->m_left_leaf.get() : _db->m_right_leaf.get();
    }

    static void inc(const fst_type* _db, const typename fst_type::node*& p, bool& end)
    {
        if (p == _db->m_left_leaf.get())
            end = true;
        else
            p = p->prev.get();
    }

    static void dec(const typename fst_type::node*& p, bool& end)
    {
        if (end)
            end = false;
        else
            p = p->next.get();
    }
};

template<typename _FstType, typename _Hdl>
class const_iterator_base
{
    typedef _Hdl     handler_type;
public:
    typedef _FstType fst_type;

    // iterator traits
    typedef ::std::pair<typename fst_type::key_type, typename fst_type::value_type> value_type;
    typedef value_type*     pointer;
    typedef value_type&     reference;
    typedef ptrdiff_t       difference_type;
    typedef ::std::bidirectional_iterator_tag iterator_category;

    explicit const_iterator_base(const fst_type* _db, bool _end) : 
        m_db(_db), m_pos(NULL), m_end_pos(_end)
    {
        if (!_db)
            return;

        m_pos = handler_type::init_pos(_db, _end);
    }

    explicit const_iterator_base(const fst_type* _db, const typename fst_type::node* pos) :
        m_db(_db), m_pos(pos), m_end_pos(false) {}

    const_iterator_base(const const_iterator_base& r) :
        m_db(r.m_db), m_pos(r.m_pos), m_end_pos(r.m_end_pos) {}

    const_iterator_base& operator=(const const_iterator_base& r)
    {
        m_db = r.m_db;
        m_pos = r.m_pos;
        m_end_pos = r.m_end_pos;
        return *this;
    }

    const value_type* operator++()
    {
        assert(m_pos);
        handler_type::inc(m_db, m_pos, m_end_pos);
        return operator->();
    }

    const value_type* operator--()
    {
        assert(m_pos);
        handler_type::dec(m_pos, m_end_pos);
        return operator->();
    }

    bool operator==(const const_iterator_base& r) const
    {
        if (m_db != r.m_db)
            return false;

        return (m_pos == r.m_pos) && (m_end_pos == r.m_end_pos);
    }

    bool operator!=(const const_iterator_base& r) const
    {
        return !operator==(r);
    }

    const value_type& operator*()
    {
        return get_current_node_pair();
    }

    const value_type* operator->()
    {
        return &get_current_node_pair();
    }

protected:
    const typename fst_type::node* get_pos() const { return m_pos; }
    const fst_type* get_parent() const { return m_db; }

private:
    const value_type& get_current_node_pair()
    {
        m_current_pair = value_type(m_pos->value_leaf.key, m_pos->value_leaf.value);
        return m_current_pair;
    }

    const fst_type* m_db;
    const typename fst_type::node* m_pos;
    value_type      m_current_pair;
    bool            m_end_pos;
};

}}

#endif
