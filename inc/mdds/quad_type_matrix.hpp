/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#ifndef __MDDS_QUAD_TYPE_MATRIX_HPP__
#define __MDDS_QUAD_TYPE_MATRIX_HPP__

#include "mdds/global.hpp"

#include <iostream>
#include <cstdlib>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>

namespace mdds {

enum matrix_density_t
{
    matrix_density_filled_zero,
    matrix_density_filled_empty,
    matrix_density_sparse_zero,
    matrix_density_sparse_empty
};

enum matrix_element_t
{ 
    element_empty   = 0, 
    element_numeric = 1, 
    element_boolean = 2, 
    element_string  = 3 
};

enum matrix_init_element_t
{
    matrix_init_element_zero,
    matrix_init_element_empty
};

class matrix_error : public ::mdds::general_error
{
public:
    matrix_error(const ::std::string& msg) : general_error(msg) {}
};

matrix_init_element_t get_init_element_type(matrix_density_t density)
{
    switch (density)
    {
        case matrix_density_filled_empty:
        case matrix_density_sparse_empty:
            return matrix_init_element_empty;
        case matrix_density_filled_zero:
        case matrix_density_sparse_zero:
            return matrix_init_element_zero;
        default:
            throw matrix_error("unknown matrix density type.");
    }
}

/**
 * This data structure represents a matrix where each individual element may
 * be of one of four types: value, boolean, string, or empty.
 */
template<typename _String>
class quad_type_matrix
{
public:
    typedef _String     string_type;
    typedef size_t      size_type;

    /**
     * Default constructor.
     */
    quad_type_matrix();

    /**
     * Construct an empty matrix with specified density type.
     */
    quad_type_matrix(matrix_density_t density);

    /**
     * Construct a matrix of specified size with specified density type. 
     */
    quad_type_matrix(size_t rows, size_t cols, matrix_density_t density);

    quad_type_matrix(const quad_type_matrix& r);
    ~quad_type_matrix();

    /**
     * Get the type of element specified by its position.  The type can be one 
     * of empty, string, numeric, or boolean. 
     * 
     * @return element type.
     */
    matrix_element_t get_type(size_t row, size_t col) const;

    double get_numeric(size_t row, size_t col) const;
    bool get_boolean(size_t row, size_t col) const;
    string_type get_string(size_t row, size_t col) const;

    void set_numeric(size_t row, size_t col, double val);
    void set_boolean(size_t row, size_t col, bool val);
    void set_string(size_t row, size_t col, string_type* str);
    void set_empty(size_t row, size_t col);

    /**
     * Return the size of matrix as a pair.  The first value is the row size,
     * while the second value is the column size.
     * 
     * @return matrix size as a value pair.
     */
    ::std::pair<size_t,size_t> size() const;
    
    /**
     * It populates the passed matrix object with the transpose of stored
     * matrix data.  The passed matrix object will inherit the same backend
     * storage as the original matrix object in case the storage types of the
     * two matrix objects differ.  The passed matrix object will also get 
     * automatically re-sized in order to fit the transposed matrix data. 
     *  
     * @param r passed matrix object to store the transposed matrix data in. 
     */
    void transpose(quad_type_matrix& r);

    /**
     * Resize the matrix to specified size.  This method supports resizing to
     * zero-sized matrix; however, either specifying the row or column size to
     * zero will resize the matrix to 0 x 0.
     * 
     * @param row new row size
     * @param col new column size
     */
    void resize(size_t row, size_t col);
    void clear();
    bool empty();
    void swap(quad_type_matrix& r);

#ifdef UNIT_TEST
    void dump() const;
#endif

private:
    struct element
    {
        matrix_element_t m_type:2;

        union
        {
            double       m_numeric;
            bool         m_boolean;
            string_type* mp_string;
        };

        element() : m_type(element_empty) {}
        element(const element& r) : m_type(r.m_type)
        {
            switch (m_type)
            {
                case element_boolean:
                    m_boolean = r.m_boolean;
                    break;
                case element_numeric:
                    m_numeric = r.m_numeric;
                    break;
                case element_string:
                    mp_string = new string_type(*r.mp_string);
                    break;
                case element_empty:
                default:
                    ;
            }
        }

        explicit element(double v) : m_type(element_numeric), m_numeric(v) {}
        explicit element(bool v) : m_type(element_boolean), m_boolean(v) {}
        explicit element(string_type* p) : m_type(element_string), mp_string(p) {}

        bool operator== (const element& r) const
        {
            if (m_type != r.m_type)
                return false;

            switch (m_type)
            {
                case element_boolean:
                    return m_boolean == r.m_boolean;
                case element_numeric:
                    return m_numeric == r.m_numeric;
                case element_string:
                    return *mp_string == *r.mp_string;
                case element_empty:
                default:
                    ;
            }

            return true;
        }

        ~element()
        {
            if (m_type == element_string)
                delete mp_string;
        }

        void set_empty()
        {
            if (m_type == element_string)
                delete mp_string;
            m_type = element_empty;
        }

        void set_numeric(double val)
        {
            if (m_type == element_string)
                delete mp_string;
            m_type = element_numeric;
            m_numeric = val;
        }

        void set_boolean(bool val)
        {
            if (m_type == element_string)
                delete mp_string;
            m_type = element_boolean;
            m_boolean = val;
        }

        void set_string(string_type* str)
        {
            if (m_type == element_string)
                delete mp_string;
            m_type = element_string;
            mp_string = str;
        }
    };

    class storage_base
    {
    public:
        storage_base(matrix_init_element_t init) : m_init_type(init) {}
        storage_base(const storage_base& r) : m_init_type(r.m_init_type) {}

        virtual ~storage_base() {}

        virtual element& get_element(size_t row, size_t col) = 0;

        virtual matrix_element_t get_type(size_t row, size_t col) const = 0;

        virtual double get_numeric(size_t row, size_t col) const = 0;
        virtual string_type get_string(size_t row, size_t col) const = 0;
        virtual bool get_boolean(size_t row, size_t col) const = 0;

        virtual size_t rows() const = 0;
        virtual size_t cols() const = 0;

        virtual void transpose() = 0;
        virtual void resize(size_t row, size_t col) = 0;
        virtual void clear() = 0;
        virtual bool empty() = 0;

        virtual storage_base* clone() const = 0;

    protected:
        matrix_init_element_t get_init_type() const { return m_init_type; }

    private:
        matrix_init_element_t m_init_type;
    };

    /**
     * This storage creates instance for every single element, even for the
     * empty elements. 
     */
    class storage_filled : public storage_base
    {
        typedef ::boost::ptr_vector<element>  row_type;
        typedef ::boost::ptr_vector<row_type> rows_type;

    public:
        storage_filled(size_t rows, size_t cols, matrix_init_element_t init_type) :
            storage_base(init_type)
        {
            m_rows.reserve(rows);
            for (size_t i = 0; i < rows; ++i)
            {
                m_rows.push_back(new row_type);
                init_row(m_rows.back(), cols);
            }
        }

        storage_filled(const storage_filled& r) :
            storage_base(r),
            m_rows(r.m_rows) {}

        virtual ~storage_filled() {}

        virtual element& get_element(size_t row, size_t col)
        {
            return m_rows.at(row).at(col);
        }

        virtual matrix_element_t get_type(size_t row, size_t col) const
        {
            return m_rows.at(row).at(col).m_type;
        }

        virtual double get_numeric(size_t row, size_t col) const
        {
            const element& elem = m_rows.at(row).at(col);
            if (elem.m_type != element_numeric)
                throw matrix_error("element type is not numeric.");

            return elem.m_numeric;
        }

        virtual string_type get_string(size_t row, size_t col) const
        {
            const element& elem = m_rows.at(row).at(col);
            if (elem.m_type != element_string)
                throw matrix_error("element type is not string.");

            return *elem.mp_string;
        }

        virtual bool get_boolean(size_t row, size_t col) const
        {
            const element& elem = m_rows.at(row).at(col);
            if (elem.m_type != element_boolean)
                throw matrix_error("element type is not boolean.");

            return elem.m_boolean;
        }

        virtual size_t rows() const
        {
            return m_rows.size();
        }

        virtual size_t cols() const
        {
            return m_rows.empty() ? 0 : m_rows[0].size();
        }

        virtual void transpose()
        {
            rows_type trans_mx;
            size_t row_size = rows(), col_size = cols();
            trans_mx.reserve(col_size);
            for (size_t col = 0; col < col_size; ++col)
            {
                trans_mx.push_back(new row_type);
                row_type& trans_row = trans_mx.back();
                trans_row.reserve(row_size);
                for (size_t row = 0; row < row_size; ++row)
                    trans_row.push_back(new element(m_rows[row][col]));
            }
            m_rows.swap(trans_mx);
        }

        virtual void resize(size_t row, size_t col)
        {
            if (!row || !col)
            {
                // Empty the matrix.
                m_rows.clear();
                return;
            }

            size_t cur_rows = rows(), cur_cols = cols();

            if (row > cur_rows)
            {
                // Insert extra rows...
                size_t new_row_count = row - cur_rows;
                m_rows.reserve(row);
                for (size_t i = 0; i < new_row_count; ++i)
                {
                    m_rows.push_back(new row_type);
                    init_row(m_rows.back(), col);
                }

                resize_rows(cur_rows-1, cur_cols, col);
            }
            else if (cur_rows > row)
            {
                // Remove rows to new size.
                m_rows.resize(row);
                resize_rows(row-1, cur_cols, col);
            }
            else
            {
                assert(cur_rows == row);
                resize_rows(cur_rows-1, cur_cols, col);
            }
        }

        virtual void clear()
        {
            m_rows.clear();
        }

        virtual bool empty()
        {
            return m_rows.empty();
        }

        virtual storage_base* clone() const
        {
            return new storage_filled(*this);
        }

    private:

        /**
         * Resize rows to a new column size, from row 0 up to specified upper 
         * row. 
         */
        void resize_rows(size_t upper_row, size_t cur_cols, size_t new_cols)
        {
            for (size_t i = 0; i <= upper_row; ++i)
            {
                // Resize pre-existing rows to new column size.
                if (new_cols > cur_cols)
                {
                    size_t new_col_count = new_cols - cur_cols;
                    for (size_t j = 0; j < new_col_count; ++j)
                        insert_new_elem(m_rows[i]);
                }
                else if (new_cols < cur_cols)
                    m_rows[i].resize(new_cols);
            }
        }

        void init_row(row_type& row, size_t col_size)
        {
            row.reserve(col_size);
            for (size_t j = 0; j < col_size; ++j)
                insert_new_elem(row);
        }

        void insert_new_elem(row_type& row)
        {
            matrix_init_element_t init_type = storage_base::get_init_type();
            switch (init_type)
            {
                case matrix_init_element_zero:
                    row.push_back(new element(static_cast<double>(0.0)));
                break;
                case matrix_init_element_empty:
                    row.push_back(new element);
                break;
                default:
                    throw matrix_error("unknown init type.");
            }
        }

    private:
        rows_type m_rows;
    };

    /**
     * This storage stores only non-empty elements. 
     */
    class storage_sparse : public storage_base
    {
        typedef ::boost::ptr_map<size_t, element>  row_type;
        typedef ::boost::ptr_map<size_t, row_type> rows_type;

    public:
        storage_sparse(size_t rows, size_t cols, matrix_init_element_t init_type) : 
            storage_base(init_type),
            m_row_size(rows), m_col_size(cols)
        {
            switch (storage_base::get_init_type())
            {
                case matrix_init_element_zero:
                    m_empty_elem.m_type = element_numeric;
                    m_empty_elem.m_numeric = 0.0;
                break;
                default:
                    m_empty_elem.m_type = element_empty;
            }
        }

        storage_sparse(const storage_sparse& r) :
            storage_base(r),
            m_rows(r.m_rows), 
            m_empty_elem(r.m_empty_elem), 
            m_row_size(r.m_row_size), 
            m_col_size(r.m_col_size) {}

        virtual ~storage_sparse() {}

        virtual element & get_element(size_t row, size_t col)
        {
            typename rows_type::iterator itr = m_rows.find(row);
            if (itr == m_rows.end())
            {
                // Insert a brand-new row.
                ::std::pair<typename rows_type::iterator, bool> r = m_rows.insert(row, new row_type);
                if (!r.second)
                    throw matrix_error("failed to insert a new row instance into storage_sparse.");
                itr = r.first;
            }

            row_type& row_store = *itr->second;
            typename row_type::iterator itr_elem = row_store.find(col);
            if (itr_elem == row_store.end())
            {
                // Insert a new element at this column position.
                ::std::pair<typename row_type::iterator, bool> r = row_store.insert(col, new element);
                if (!r.second)
                    throw matrix_error("failed to insert a new element instance.");
                itr_elem = r.first;
            }
            return *itr_elem->second;
        }

        virtual matrix_element_t get_type(size_t row, size_t col) const
        {
            typename rows_type::const_iterator itr = m_rows.find(row);
            if (itr == m_rows.end())
                return m_empty_elem.m_type;

            const row_type& row_store = *itr->second;
            typename row_type::const_iterator itr_elem = row_store.find(col);
            if (itr_elem == row_store.end())
                return m_empty_elem.m_type;

            return itr_elem->second->m_type;
        }

        virtual double get_numeric(size_t row, size_t col) const
        {
            matrix_element_t elem_type = get_type(row, col);
            if (elem_type != element_numeric)
                throw matrix_error("element type is not numeric.");

            return get_non_empty_element(row, col).m_numeric;
        }

        virtual string_type get_string(size_t row, size_t col) const
        {
            matrix_element_t elem_type = get_type(row, col);
            if (elem_type != element_string)
                throw matrix_error("element type is not string.");

            return *get_non_empty_element(row, col).mp_string;
        }

        virtual bool get_boolean(size_t row, size_t col) const
        {
            matrix_element_t elem_type = get_type(row, col);
            if (elem_type != element_boolean)
                throw matrix_error("element type is not string.");

            return get_non_empty_element(row, col).m_boolean;
        }

        virtual size_t rows() const
        {
            return m_row_size;
        }

        virtual size_t cols() const
        {
            return m_col_size;
        }

        typedef ::std::pair<size_t, size_t> elem_pos_type;

        struct elem_pos_sorter : ::std::binary_function<elem_pos_type, elem_pos_type, bool>
        {
            bool operator() (const elem_pos_type& left, const elem_pos_type& right) const
            {
                if (left.first != right.first)
                    return left.first < right.first;
                return left.second < right.second;
            }
        };

        virtual void transpose()
        {
            using namespace std;

            rows_type trans;

            // First, pick up the positions of all non-empty elements.
            vector<elem_pos_type> filled_elems;
            typename rows_type::const_iterator itr_row = m_rows.begin(), itr_row_end = m_rows.end();
            for (; itr_row != itr_row_end; ++itr_row)
            {
                size_t row_idx = itr_row->first;
                const row_type& row = *itr_row->second;
                typename row_type::const_iterator itr_col = row.begin(), itr_col_end = row.end();
                for (; itr_col != itr_col_end; ++itr_col)
                {
                    // Be sure to swap the row and column indices.
                    filled_elems.push_back(elem_pos_type(itr_col->first, row_idx));
                }
            }
            // Sort by row index first, then by column index.
            sort(filled_elems.begin(), filled_elems.end(), elem_pos_sorter());

            // Iterate through the non-empty element positions and perform 
            // transposition.
            typename vector<elem_pos_type>::const_iterator 
                itr_pos = filled_elems.begin(), itr_pos_end = filled_elems.end();
            while (itr_pos != itr_pos_end)
            {
                // First item of the new row.
                size_t row_idx = itr_pos->first;
                size_t col_idx = itr_pos->second;
                pair<typename rows_type::iterator, bool> r = trans.insert(row_idx, new row_type);
                if (!r.second)
                    throw matrix_error("failed to insert a new row instance during transposition.");

                typename rows_type::iterator itr_row = r.first;
                row_type& row = *itr_row->second;
                pair<typename row_type::iterator, bool> r2 = 
                    row.insert(col_idx, new element(m_rows[col_idx][row_idx]));
                if (!r2.second)
                    throw matrix_error("afiled to insert a new element instance during transposition.");

                // Keep iterating until we get a different row index.
                for (++itr_pos; itr_pos != itr_pos_end && itr_pos->first == row_idx; ++itr_pos)
                {
                    col_idx = itr_pos->second;
                    r2 = row.insert(col_idx, new element(m_rows[col_idx][row_idx]));
                    if (!r2.second)
                        throw matrix_error("afiled to insert a new element instance during transposition.");
                }
            }

            m_rows.swap(trans);
            ::std::swap(m_row_size, m_col_size);
        }

        virtual void resize(size_t row, size_t col)
        {
            if (!row || !col)
            {
                clear();
                return;
            }

            // Resizing a sparse matrix need to modify the data only when 
            // shrinking.

            if (m_row_size > row)
            {
                // Remove all rows where the row index is greater than or 
                // equal to 'row'.
                typename rows_type::iterator itr = m_rows.lower_bound(row);
                m_rows.erase(itr, m_rows.end());
            }

            if (m_col_size > col)
            {
                typename rows_type::iterator itr = m_rows.begin(), itr_end = m_rows.end();
                for (; itr != itr_end; ++itr)
                {
                    // Now, remove all columns where the column index is 
                    // greater than or equal to 'col'.
                    row_type& row = *itr->second;
                    typename row_type::iterator itr_elem = row.lower_bound(col);
                    row.erase(itr_elem, row.end());
                }
            }

            m_row_size = row;
            m_col_size = col;
        }

        virtual void clear()
        {
            m_rows.clear();
            m_row_size = 0;
            m_col_size = 0;
        }

        virtual bool empty()
        {
            // If one of row and column sizes are zero, the other size must be
            // zero, and vise versa.
            assert((!m_row_size && !m_col_size) || (m_row_size && m_col_size));

            return m_row_size == 0 && m_col_size == 0;
        }

        virtual storage_base* clone() const
        {
            return new storage_sparse(*this);
        }

    private:
        const element& get_non_empty_element(size_t row, size_t col) const
        {
            typename rows_type::const_iterator itr = m_rows.find(row);
            if (itr == m_rows.end())
                return m_empty_elem;

            const row_type& row_store = *itr->second;
            typename row_type::const_iterator itr_elem = row_store.find(col);
            if (itr_elem == row_store.end())
                return m_empty_elem;
            return *itr_elem->second;
        }

    private:
        rows_type   m_rows;
        element     m_empty_elem;
        size_t      m_row_size;
        size_t      m_col_size;
    };

private:
    static storage_base* create_storage(size_t rows, size_t cols, matrix_density_t density);

private:
    storage_base* mp_storage;
};

template<typename _String>
typename quad_type_matrix<_String>::storage_base*
quad_type_matrix<_String>::create_storage(size_t rows, size_t cols, matrix_density_t density)
{
    switch (density)
    {
        case matrix_density_filled_zero:
            return new storage_filled(rows, cols, matrix_init_element_zero);
        case matrix_density_filled_empty:
            return new storage_filled(rows, cols, matrix_init_element_empty);
        case matrix_density_sparse_zero:
            return new storage_sparse(rows, cols, matrix_init_element_zero);
        case matrix_density_sparse_empty:
            return new storage_sparse(rows, cols, matrix_init_element_empty);
        default:
            throw matrix_error("unknown density type");
    }
    return NULL;
}

template<typename _String>
quad_type_matrix<_String>::quad_type_matrix() :
    mp_storage(NULL)
{
    mp_storage = create_storage(0, 0, matrix_density_filled_zero);
}

template<typename _String>
quad_type_matrix<_String>::quad_type_matrix(matrix_density_t density) :
    mp_storage(NULL)
{
    mp_storage = create_storage(0, 0, density);
}

template<typename _String>
quad_type_matrix<_String>::quad_type_matrix(size_t rows, size_t cols, matrix_density_t density) :
    mp_storage(NULL)
{
    mp_storage = create_storage(rows, cols, density);
}

template<typename _String>
quad_type_matrix<_String>::quad_type_matrix(const quad_type_matrix& r) :
    mp_storage(r.mp_storage->clone())
{
}

template<typename _String>
quad_type_matrix<_String>::~quad_type_matrix()
{
    delete mp_storage;
}

template<typename _String>
matrix_element_t quad_type_matrix<_String>::get_type(size_t row, size_t col) const
{
    return mp_storage->get_type(row, col);
}

template<typename _String>
double quad_type_matrix<_String>::get_numeric(size_t row, size_t col) const
{
    return mp_storage->get_numeric(row, col);
}

template<typename _String>
bool quad_type_matrix<_String>::get_boolean(size_t row, size_t col) const
{
    return mp_storage->get_boolean(row, col);
}

template<typename _String>
typename quad_type_matrix<_String>::string_type
quad_type_matrix<_String>::get_string(size_t row, size_t col) const
{
    return mp_storage->get_string(row, col);
}

template<typename _String>
void quad_type_matrix<_String>::set_numeric(size_t row, size_t col, double val)
{
    mp_storage->get_element(row, col).set_numeric(val);
}

template<typename _String>
void quad_type_matrix<_String>::set_boolean(size_t row, size_t col, bool val)
{
    mp_storage->get_element(row, col).set_boolean(val);
}

template<typename _String>
void quad_type_matrix<_String>::set_string(size_t row, size_t col, string_type* str)
{
    mp_storage->get_element(row, col).set_string(str);
}

template<typename _String>
void quad_type_matrix<_String>::set_empty(size_t row, size_t col)
{
    mp_storage->get_element(row, col).set_empty();
}

template<typename _String>
::std::pair<size_t,size_t> quad_type_matrix<_String>::size() const
{
    ::std::pair<size_t,size_t> size_pair(mp_storage->rows(), mp_storage->cols());
    return size_pair;
}

template<typename _String>
void quad_type_matrix<_String>::transpose(quad_type_matrix& r)
{
    delete r.mp_storage;
    r.mp_storage = mp_storage->clone();
    r.mp_storage->transpose();
}

template<typename _String>
void quad_type_matrix<_String>::resize(size_t row, size_t col)
{
    mp_storage->resize(row, col);
}

template<typename _String>
void quad_type_matrix<_String>::clear()
{
    mp_storage->clear();
}

template<typename _String>
bool quad_type_matrix<_String>::empty()
{
    return mp_storage->empty();
}

template<typename _String>
void quad_type_matrix<_String>::swap(quad_type_matrix& r)
{
    ::std::swap(mp_storage, r.mp_storage);
}

#ifdef UNIT_TEST
template<typename _String>
void quad_type_matrix<_String>::dump() const
{
    using namespace std;
    size_t rows = mp_storage->rows(), cols = mp_storage->cols();
    cout << "rows: " << mp_storage->rows() << "  cols: " << mp_storage->cols() << endl;
    for (size_t i = 0; i < rows; ++i)
    {
        cout << "row " << i << ": ";
        for (size_t j = 0; j < cols; ++j)
        {
            matrix_element_t etype = mp_storage->get_type(i, j);
            if (j > 0)
                cout << ", ";
            cout << "(col " << j << ": ";
            switch (etype)
            {
                case element_boolean:
                    cout << boolalpha << mp_storage->get_boolean(i, j) << noboolalpha;
                    break;
                case element_empty:
                    cout << "-";
                    break;
                case element_numeric:
                    cout << mp_storage->get_numeric(i, j);
                    break;
                case element_string:
                    cout << "'" << mp_storage->get_string(i, j) << "'";
                    break;
                default:
                    ;
            }
            cout << ")";
        }
        cout << endl;
    }
}
#endif

}

#endif
