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

#include "test_global.hpp"
#include "mdds/mixed_type_matrix.hpp"

#include <sstream>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;
using namespace mdds;

typedef mixed_type_matrix<string, uint8_t> mx_type;
typedef void (test_func_type)(matrix_density_t);

/**
 * Run specified test on all matrix density types.
 *
 * @param func function pointer to the test function to be performed.
 */
void run_tests_on_all_density_types(test_func_type* func)
{
    func(matrix_density_filled_zero);
    func(matrix_density_filled_empty);
    func(matrix_density_sparse_zero);
    func(matrix_density_sparse_empty);
}

string get_mx_density_name(matrix_density_t dens)
{
    switch (dens)
    {
        case matrix_density_filled_zero:
            return "density filled with zero base elements";
        case matrix_density_filled_empty:
            return "density filled with empty base elements";
        case matrix_density_sparse_zero:
            return "density sparse with zero base elements";
        case matrix_density_sparse_empty:
            return "density sparse with empty base elements";
        default:
            ;
    }
    return "unknown density";
}

void print_mx_density_type(matrix_density_t dens)
{
    cout << "matrix density type: " << get_mx_density_name(dens) << endl;
}

template<typename _Elem>
string print_element(const _Elem& e)
{
    ostringstream os;
    switch (e.m_type)
    {
        case element_empty:
            os << "empty";
        break;
        case element_numeric:
            os << "numeric (" << e.m_numeric << ")";
        break;
        case element_boolean:
            os << "boolean";
        break;
        case element_string:
            os << "string";
        break;
        default:
            os << "<unkwown>";
    }
    return os.str();
}

template<typename _Mx>
bool verify_transposed_matrix(const _Mx& original, const _Mx& transposed)
{
    pair<size_t, size_t> mx_size = original.size(), mx_size_trans = transposed.size();
    if (mx_size.first != mx_size_trans.second || mx_size.second != mx_size_trans.first)
        return false;

    for (size_t row = 0; row < mx_size.first; ++row)
    {
        for (size_t col = 0; col < mx_size.second; ++col)
        {
            matrix_element_t elem_type = original.get_type(row, col);
            if (elem_type != transposed.get_type(col, row))
                return false;

            switch (elem_type)
            {
                case element_boolean:
                    if (original.get_boolean(row, col) != transposed.get_boolean(col, row))
                        return false;
                    break;
                case element_numeric:
                    if (original.get_numeric(row, col) != transposed.get_numeric(col, row))
                        return false;
                    break;
                case element_string:
                    if (*original.get_string(row, col) != *transposed.get_string(col, row))
                        return false;
                    break;
                case element_empty:
                default:
                    ;
            }
        }
    }
    return true;
}

template<typename _Mx>
bool verify_init_zero(const _Mx& mx)
{
    pair<size_t,size_t> mxsize = mx.size();
    for (size_t row = 0; row < mxsize.first; ++row)
    {
        for (size_t col = 0; col < mxsize.second; ++col)
        {
            if (mx.get_type(row, col) != element_numeric)
                return false;
            if (mx.get_numeric(row, col) != 0.0)
                return false;
        }
    }
    return true;
}

template<typename _Mx>
bool verify_init_empty(const _Mx& mx)
{
    pair<size_t,size_t> mxsize = mx.size();
    for (size_t row = 0; row < mxsize.first; ++row)
    {
        for (size_t col = 0; col < mxsize.second; ++col)
        {
            if (mx.get_type(row, col) != element_empty)
                return false;
        }
    }
    return true;
}

template<typename _Mx>
bool verify_assign(const _Mx& mx1, const _Mx& mx2)
{
    size_t row_count = min(mx1.size().first,  mx2.size().first);
    size_t col_count = min(mx1.size().second, mx2.size().second);
    for (size_t i = 0; i < row_count; ++i)
    {
        for (size_t j = 0; j < col_count; ++j)
        {
            matrix_element_t elem_type = mx1.get_type(i, j);
            if (elem_type != mx2.get_type(i, j))
                return false;

            switch (elem_type)
            {
                case element_boolean:
                    if (mx1.get_boolean(i, j) != mx2.get_boolean(i, j))
                        return false;
                    break;
                case element_numeric:
                    if (mx1.get_numeric(i, j) != mx2.get_numeric(i, j))
                        return false;
                    break;
                case element_string:
                    if (*mx1.get_string(i, j) != *mx2.get_string(i, j))
                        return false;
                    break;
                case element_empty:
                default:
                    ;
            }
        }
    }
    return true;
}

template<typename _Mx>
bool verify_assignment(const _Mx& mx1, const _Mx& mx2)
{
    if (mx1.size() != mx2.size())
        return false;

    return verify_assign<_Mx>(mx1, mx2);
}

void mtm_test_resize(matrix_density_t density)
{
    stack_printer __stack_printer__("::mtm_test_resize");
    print_mx_density_type(density);
    pair<size_t,size_t> mxsize;
    pair<size_t,size_t> mxstoragesize;

    {
        // Start with an empty matrix, and resize into a non-empty one.
        mx_type mx(0, 0, density);
        mxsize = mx.size();
        mxstoragesize = mx.get_storage_size();
        assert(mxsize.first == 0);
        assert(mxsize.second == 0);
        assert(mxsize == mxstoragesize);
        mx.resize(1, 1);
        mxsize = mx.size();
        mxstoragesize = mx.get_storage_size();
        assert(mxsize.first == 1);
        assert(mxsize.second == 1);
        assert(mxsize == mxstoragesize);
        // Back to an empty matrix again.
        mx.resize(0, 0);
        mxsize = mx.size();
        mxstoragesize = mx.get_storage_size();
        assert(mxsize.first == 0);
        assert(mxsize.second == 0);
        assert(mxsize == mxstoragesize);
        // Resize to a non-square matrix.
        mx.resize(5, 10);
        mxsize = mx.size();
        mxstoragesize = mx.get_storage_size();
        assert(mxsize.first == 5);
        assert(mxsize.second == 10);
        assert(mxsize == mxstoragesize);
    }

    mx_type mx(3, 3, density);
    mx.dump();
    mxsize = mx.size();
    mxstoragesize = mx.get_storage_size();
    assert(mxsize.first == 3);
    assert(mxsize.second == 3);
    assert(mxsize == mxstoragesize);

    mx.set_string(0, 0, new string("test"));
    mx.set_numeric(0, 1, 2.3);
    mx.set_boolean(1, 1, false);
    mx.set_numeric(1, 2, 45.4);
    mx.set_empty(2, 0);
    mx.set_empty(2, 1);
    mx.set_empty(2, 2);
    mx.dump();
    mx.resize(6, 4);
    mx.dump();
    mxsize = mx.size();
    mxstoragesize = mx.get_storage_size();
    assert(mxsize.first == 6);
    assert(mxsize.second == 4);
    assert(mxsize == mxstoragesize);

    mx.resize(6, 6);
    mx.set_boolean(5, 5, true);
    mx.dump();
    mxsize = mx.size();
    mxstoragesize = mx.get_storage_size();
    assert(mxsize.first == 6);
    assert(mxsize.second == 6);
    assert(mxsize == mxstoragesize);

    mx.resize(3, 6);
    mx.set_numeric(2, 2, 4.2);
    mx.set_numeric(2, 3, 3.2);
    mx.set_numeric(2, 4, 2.2);
    mx.set_numeric(2, 5, 1.2);
    mx.dump();
    mxsize = mx.size();
    mxstoragesize = mx.get_storage_size();
    assert(mxsize.first == 3);
    assert(mxsize.second == 6);
    assert(mxsize == mxstoragesize);

    mx.resize(3, 3);
    mx.dump();
    mxsize = mx.size();
    mxstoragesize = mx.get_storage_size();
    assert(mxsize.first == 3);
    assert(mxsize.second == 3);
    assert(mxsize == mxstoragesize);

    mx.resize(0, 0);
    mx.dump();
    mxsize = mx.size();
    mxstoragesize = mx.get_storage_size();
    assert(mxsize.first == 0);
    assert(mxsize.second == 0);
    assert(mxsize == mxstoragesize);
    assert(mx.empty());
}

void mtm_test_value_store(matrix_density_t density)
{
    stack_printer __stack_printer__("::mtm_test_value_store");
    print_mx_density_type(density);
    mx_type mx(5, 5, density);
    mx.dump();
    pair<size_t,size_t> mxsize = mx.size();
    assert(mxsize.first == 5);
    assert(mxsize.second == 5);
    assert(!mx.empty());

    // Make sure all elements have been initialized properly according to the
    // matrix type.
    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            matrix_element_t elem_type = mx.get_type(i, j);
            switch (density)
            {
                case matrix_density_filled_zero:
                case matrix_density_sparse_zero:
                {
                    // filled matrices are initialized to numeric elements
                    // having a value of 0.
                    assert(elem_type == element_numeric);
                    double val = mx.get_numeric(i, j);
                    assert(val == 0.0);
                }
                break;
                case matrix_density_filled_empty:
                case matrix_density_sparse_empty:
                    // sparse matrices are initialized to empty elements.
                    assert(elem_type == element_empty);
                break;
            }
        }
    }

    // Insert strings into all elements.
    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            ostringstream os;
            os << "(" << i << "," << j << ")";
            mx.set_string(i, j, new string(os.str()));
        }
    }
    mx.dump();

    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            matrix_element_t elem_type = mx.get_type(i, j);
            assert(elem_type == element_string);
            string s = *mx.get_string(i, j);
            cout << s << " ";
        }
        cout << endl;
    }

    // Now, boolean values.  Note that these operations should de-alloocate
    // all previously stored strings.
    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            bool b = (i+j)%2 ? true : false;
            mx.set_boolean(i, j, b);
        }
    }
    mx.dump();

    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            matrix_element_t elem_type = mx.get_type(i, j);
            assert(elem_type == element_boolean);
            bool stored = mx.get_boolean(i, j);
            bool expected = (i+j)%2 ? true : false;
            assert(stored == expected);
        }
    }

    // Make all elements empty.
    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            mx.set_empty(i, j);
        }
    }
    mx.dump();

    for (size_t i = 0; i < 5; ++i)
    {
        for (size_t j = 0; j < 5; ++j)
        {
            matrix_element_t elem_type = mx.get_type(i, j);
            assert(elem_type == element_empty);
        }
    }
}

template<typename _Mx>
void print_transposed_mx(const _Mx& original, const _Mx& transposed)
{
    cout << "original matrix:" << endl;
    original.dump();
    cout << "transposed matrix:" << endl;
    transposed.dump();
}

void mtm_test_transpose(matrix_density_t density)
{
    stack_printer __stack_printer__("::mtm_test_transpose");
    print_mx_density_type(density);

    {
        // Transposition of square matrix.
        mx_type mx(3, 3, density);
        mx.set_numeric(0, 1, 1);
        mx.set_numeric(0, 2, 1);
        mx.set_numeric(1, 2, 1);
        mx.set_numeric(1, 0, 2);
        mx.set_numeric(2, 0, 2);
        mx.set_numeric(2, 1, 2);
        mx_type mx_trans(mx);
        mx_trans.transpose();
        print_transposed_mx(mx, mx_trans);
        bool success = verify_transposed_matrix(mx, mx_trans);
        assert(success);
    }

    {
        // Non-square matrix.
        mx_type mx(5, 3, density);
        mx.set_numeric(0, 0, 10);
        mx.set_boolean(1, 0, true);
        mx.set_boolean(2, 0, false);
        mx.set_numeric(3, 0, 23);
        mx.set_string(3, 2, new string("test"));
        mx.set_empty(4, 0);
        mx_type mx_trans(mx);
        mx_trans.transpose();
        print_transposed_mx(mx, mx_trans);
        bool success = verify_transposed_matrix(mx, mx_trans);
        assert(success);
    }

    {
        // Empty matrix.
        mx_type mx, mx_trans;
        mx_trans.transpose();
        print_transposed_mx(mx, mx_trans);
        bool success = verify_transposed_matrix(mx, mx_trans);
        assert(success);
    }
}

void mtm_test_initial_elements()
{
    stack_printer __stack_printer__("::mtm_test_initial_elements");
    {
        mx_type mx(3, 3, matrix_density_filled_zero);
        mx.dump();
        bool success = verify_init_zero(mx);
        assert(success);
        assert(mx.numeric());
        mx.resize(15, 14);
        assert(mx.numeric());
    }

    {
        mx_type mx(3, 3, matrix_density_filled_empty);
        mx.dump();
        bool success = verify_init_empty(mx);
        assert(success);
        assert(!mx.numeric());
    }

    {
        mx_type mx(3, 3, matrix_density_sparse_zero);
        mx.dump();
        bool success = verify_init_zero(mx);
        assert(success);
        assert(mx.numeric());
        mx.resize(10, 32);
        assert(mx.numeric());
    }

    {
        mx_type mx(3, 3, matrix_density_sparse_empty);
        mx.dump();
        bool success = verify_init_empty(mx);
        assert(success);
        assert(!mx.numeric());
    }
}

void mtm_test_numeric_matrix()
{
    stack_printer __stack_printer__("::mtm_test_numeric_matrix");
    {
        print_mx_density_type(matrix_density_filled_zero);
        mx_type mx(3, 3, matrix_density_filled_zero);
        mx.dump();
        assert(mx.numeric());
        mx.set_empty(0, 0);
        mx.dump();
        assert(!mx.numeric());
        mx.resize(5, 5);
        mx.dump();
        assert(!mx.numeric());
        mx.resize(2, 2);
        mx.dump();
        assert(!mx.numeric());
        mx.set_numeric(0, 0, 50);
        mx.dump();
        assert(mx.numeric());
        mx.set_boolean(1, 1, true);
        mx.dump();
        assert(mx.numeric());
        assert(mx.get_numeric(1, 1) == 1.0);
        mx.set_string(1, 0, new string("test"));
        mx.dump();
        assert(!mx.numeric());
    }

    {
        print_mx_density_type(matrix_density_sparse_zero);
        mx_type mx(3, 3, matrix_density_sparse_zero);
        mx.dump();
        assert(mx.numeric());
        mx.set_empty(0, 0);
        mx.dump();
        assert(!mx.numeric());
        mx.resize(5, 5);
        mx.dump();
        assert(!mx.numeric());
        mx.resize(2, 2);
        mx.dump();
        assert(!mx.numeric());
        mx.set_numeric(0, 0, 50);
        mx.dump();
        assert(mx.numeric());
        mx.set_boolean(1, 1, true);
        mx.dump();
        assert(mx.numeric());
        assert(mx.get_numeric(1, 1) == 1.0);
        mx.set_string(1, 0, new string("test"));
        mx.dump();
        assert(!mx.numeric());
    }
}

void mtm_test_assign(matrix_density_t dens1, matrix_density_t dens2)
{
    stack_printer __stack_printer__("::mtm_test_assign");
    print_mx_density_type(dens1);
    print_mx_density_type(dens2);

    // Assigning from a smaller matrix to a bigger one.
    mx_type mx1(5, 5, dens1), mx2(2, 2, dens2);
    mx2.set(0, 0, 1.2);
    mx2.set(1, 1, true);
    mx2.set(0, 1, new string("test"));
    mx2.set(1, 0, new string("foo"));
    cout << "matrix 1:" << endl;
    mx1.dump();
    cout << "matrix 2:" << endl;
    mx2.dump();
    mx1.assign(mx2);
    cout << "matrix 1 after assign:" << endl;
    mx1.dump();

    bool success = verify_assign(mx1, mx2);
    assert(success);

    mx2.resize(8, 8);
    mx2.assign(mx1);
    cout << "matrix 2 after resize and assign:" << endl;
    mx2.dump();
    success = verify_assign(mx1, mx2);
    assert(success);

    // from a larger matrix to a smaller one.
    mx1.set(0, 0, new string("test1"));
    mx2.set(0, 0, new string("test2"));
    mx2.set(4, 4, true);
    mx2.set(7, 7, false);
    mx1.assign(mx2);
    cout << "matrix 1 after assign:" << endl;
    mx1.dump();
    success = verify_assign(mx1, mx2);
    assert(success);

    // self assignment (should be no-op).
    mx1.assign(mx1);
    success = verify_assign(mx1, mx1);
    assert(success);

    mx2.assign(mx2);
    success = verify_assign(mx2, mx2);
    assert(success);
}

void mtm_test_assignment(matrix_density_t density)
{
    stack_printer __stack_printer__("::mtm_test_assignment");
    print_mx_density_type(density);
    mx_type mx1(0, 0, density), mx2(3, 3, density);
    mx2.set(0, 0, 3.4);
    mx2.set(2, 1, new string("hmm..."));
    mx2.set(1, 2, new string("help"));
    cout << "matrix 1 initial:" << endl;
    mx1.dump();
    cout << "matrix 2 initial:" << endl;
    mx2.dump();
    mx1 = mx2;
    cout << "matrix 1 after assignment:" << endl;
    mx1.dump();
    bool success = verify_assignment(mx1, mx2);
    assert(success);

    // self assignment.
    mx2 = mx2;
    cout << "matrix 2 after self-assignment:" << endl;
    mx2.dump();
    success = verify_assignment(mx1, mx2);
    assert(success);

    mx1.set(0, 1, true);
    mx1.set(1, 0, false);
    mx2 = mx1;
    cout << "matrix 2 after assignment:" << endl;
    mx2.dump();
    success = verify_assignment(mx1, mx2);
    assert(success);

    // Assigning an empty matrix.
    mx2 = mx_type();
    assert(mx2.empty());
}

void mtm_test_flag_storage(matrix_density_t density)
{
    stack_printer __stack_printer__("::mtm_test_flag_storage");
    print_mx_density_type(density);
    mx_type mx(3, 3, density);
    mx.dump();
    mx.dump_flags();
    assert(mx.get_flag(0, 0) == 0);

    mx_type::flag_type flag = 0x01;
    mx.set_flag(0, 0, flag);
    mx.dump_flags();
    assert(mx.get_flag(0, 0) == flag);

    flag |= 0xF0;
    mx.set_flag(0, 0, flag);
    mx.set_flag(2, 1, flag);
    mx.dump_flags();
    assert(mx.get_flag(0, 0) == flag);
    assert(mx.get_flag(2, 1) == flag);
    assert(mx.get_flag(2, 2) == 0);

    mx.clear_flag(0, 0);
    mx.dump_flags();
    assert(mx.get_flag(0, 0) == 0);
    assert(mx.get_flag(2, 1) == flag);
}

template<typename _StoreType>
void traverse_itr_access(typename _StoreType::const_itr_access& itr_access)
{
    typedef _StoreType store_type;
    if (itr_access.empty())
    {
        cout << "no element stored." << endl;
        return;
    }

    cout << "increment" << endl;
    long i = 0;
    do
    {
        cout << i++ << ": " << print_element(itr_access.get()) << endl;
    }
    while (itr_access.inc());

    cout << "decrement" << endl;

    while (itr_access.dec())
    {
        cout << --i << ": " << print_element(itr_access.get()) << endl;
    }
    assert(i == 0);
}

template<typename _FilledStoreType>
void mtm_test_iterator_access_filled(size_t rows, size_t cols)
{
    stack_printer __stack_printer__("::mtm_test_iterator_access_filled");
    typedef _FilledStoreType store_type;

    store_type store(rows, cols, matrix_init_element_zero);
    {
        cout << "rows: " << rows << "  cols: " << cols << endl;
        typename store_type::const_itr_access* itr_access = store.get_const_itr_access();
        traverse_itr_access<store_type>(*itr_access);
        delete itr_access;
    }
}

void mtm_test_iterator_access_sparse()
{
    stack_printer __stack_printer__("::mem_test_iterator_access_sparse");
    typedef mx_type::sparse_storage_type store_type;
    store_type store(5, 5, matrix_init_element_empty);
    {
        store_type::const_itr_access* itr_access = store.get_const_itr_access();
        assert(itr_access->empty());
        delete itr_access;
    }
    {
        store_type::element& elem = store.get_element(0, 0);
        elem.m_type = element_numeric;
        elem.m_numeric = 3.5;
        store_type::const_itr_access* itr_access = store.get_const_itr_access();
        assert(!itr_access->empty());
        traverse_itr_access<store_type>(*itr_access);
        delete itr_access;
    }
    {
        store_type::element& elem = store.get_element(4, 4);
        elem.m_type = element_numeric;
        elem.m_numeric = 12;
        store_type::const_itr_access* itr_access = store.get_const_itr_access();
        assert(!itr_access->empty());
        traverse_itr_access<store_type>(*itr_access);
        delete itr_access;
    }
    {
        store_type::element& elem = store.get_element(3, 2);
        elem.m_type = element_numeric;
        elem.m_numeric = 26.567;
        store_type::const_itr_access* itr_access = store.get_const_itr_access();
        assert(!itr_access->empty());
        traverse_itr_access<store_type>(*itr_access);
        delete itr_access;
    }
}

class find_value : public unary_function<bool, mx_type::element>
{
public:
    find_value(double val) : m_val(val) {}
    bool operator() (const mx_type::element& elem) const
    {
        return elem.m_type == element_numeric && elem.m_numeric == m_val;
    }
private:
    double m_val;
};

void mtm_test_const_iterator()
{
    stack_printer __stack_printer__("::mtm_test_const_iterator");
    // Test with an empty matrix.
    mx_type mx(0, 0, matrix_density_filled_zero);
    assert(mx.empty());
    mx_type::const_iterator itr, itr_beg = mx.begin(), itr_end = mx.end();

    // Test for assignment and equality.
    assert(itr != itr_beg);
    assert(itr != itr_end);
    itr = itr_beg;
    assert(itr == itr_beg);
    itr = itr_end;
    assert(itr == itr_end);

    // For an empty matrix, the begin and end position must equal.
    assert(itr_beg == itr_end);

    // Now, test with a non-empty matrix.
    mx.resize(2, 3);
    double v = 0.0;
    mx.set(0, 0, ++v);
    mx.set(0, 1, ++v);
    mx.set(0, 2, ++v);
    mx.set(1, 0, ++v);
    mx.set(1, 1, ++v);
    mx.set(1, 2, ++v);
    assert(!mx.empty());
    itr = mx_type::const_iterator();
    itr_beg = mx.begin();
    itr_end = mx.end();
    assert(itr_beg != itr_end);

    // Test for assignment and equality again.
    assert(itr != itr_beg);
    assert(itr != itr_end);
    itr = itr_beg;
    assert(itr == itr_beg);
    itr = itr_end;
    assert(itr == itr_end);

    // Create another matrix instance with the same dimension.
    mx_type mx2(2, 3, matrix_density_filled_zero);
    assert(mx.size() == mx2.size());
    assert(mx.begin() != mx2.begin());
    assert(mx.end() != mx2.end());

    cout << "incrementing iterators." << endl;
    for (itr = itr_beg; itr != itr_end; ++itr)
        cout << print_element(*itr) << endl;
    assert(itr == itr_end);

    cout << "decrementing iterators." << endl;
    while (true)
    {
        cout << print_element(*(--itr)) << endl;;
        if (itr == itr_beg)
            break;
    }
    assert(itr == itr_beg);

    cout << "using std::advance" << endl;
    advance(itr, 3);
    assert(itr->m_numeric == 4.0);
    cout << print_element(*itr) << endl;
    advance(itr, 2);
    assert(itr->m_numeric == 6.0);
    cout << print_element(*itr) << endl;
    advance(itr, 1);
    assert(itr == itr_end);

    cout << "using ::std::find_if" << endl;
    for (double val = 1.0; val <= 6.0; ++val)
    {
        itr = find_if(itr_beg, itr_end, find_value(val));
        cout << print_element(*itr) << endl;
        assert(itr->m_type == element_numeric);
        assert(itr->m_numeric == val);
    }
    itr = find_if(itr_beg, itr_end, find_value(1.5));
    assert(itr == itr_end); // not found.
}

/**
 * Measure the performance of object instantiation for filled storage.
 */
void mtm_perf_test_storage_creation()
{
    cout << "measuring performance on matrix object creation." << endl;
    size_t rowsize = 5000;
    size_t obj_count = 30000;
    cout << "row size: " << rowsize << "  object count: " << obj_count << endl;
    cout << "--- filled zero" << endl;
    for (size_t colsize = 1; colsize <= 5; ++colsize)
    {
        stack_watch sw;
        for (size_t i = 0; i < obj_count; ++i)
            mx_type mx(rowsize, colsize, matrix_density_filled_zero);

        cout << "column size: " << colsize << "  duration: " << sw.get_duration() << " sec" << endl;
    }
    cout << endl;
}

void mtm_perf_test_storage_set_numeric()
{
    cout << "measuring performance on matrix object creation and populating it with numeric data." << endl;
    size_t rowsize = 3000;
    size_t obj_count = 30000;
    cout << "row size: " << rowsize << "  object count: " << obj_count << endl;
    cout << "--- filled zero" << endl;
    for (size_t colsize = 1; colsize <= 5; ++colsize)
    {
        stack_watch sw;
        for (size_t i = 0; i < obj_count; ++i)
        {
            mx_type mx(rowsize, colsize, matrix_density_filled_zero);
            for (size_t row = 0; row < rowsize; ++row)
            {
                for (size_t col = 0; col < colsize; ++col)
                    mx.set_numeric(row, col, 1.0);
            }
        }
        cout << "column size: " << colsize << "  duration: " << sw.get_duration() << " sec" << endl;
    }
    cout << endl;
}

void mtm_perf_test_iterate_elements()
{
    cout << "measuring performance on iterating over all numeric elements." << endl;
    size_t rowsize = 100000;
    size_t colsize = 1000;
    cout << "row size: " << rowsize << "  column size: " << colsize << endl;
    mx_type mx(rowsize, colsize, matrix_density_filled_zero);
    {
        stack_watch sw;
        double val = 1.0;
        for (size_t i = 0; i < rowsize; ++i)
        {
            for (size_t j = 0; j < colsize; ++j)
            {
                mx.set(i, j, val);
                val += 0.001;
            }
        }
        cout << "element values inserted.  (duration: " << sw.get_duration() << " sec)" << endl;
    }

    {
        stack_watch sw;
        double val = 0.0;
        mx_type::const_iterator it = mx.begin(), it_end = mx.end();
        for (; it != it_end; ++it)
            val += it->m_numeric;

        cout << "all element values added.  (answer: " << val << ")  (duration: " << sw.get_duration() << " sec)" << endl;
    }
}

int main(int argc, char** argv)
{
    cmd_options opt;
    if (!parse_cmd_options(argc, argv, opt))
        return EXIT_FAILURE;

    if (opt.test_func)
    {
        run_tests_on_all_density_types(mtm_test_resize);
        run_tests_on_all_density_types(mtm_test_value_store);
        run_tests_on_all_density_types(mtm_test_transpose);
        run_tests_on_all_density_types(mtm_test_assignment);

        mtm_test_initial_elements();
        mtm_test_numeric_matrix();
        mtm_test_assign(matrix_density_filled_zero, matrix_density_filled_zero);
        mtm_test_assign(matrix_density_filled_empty, matrix_density_filled_zero);
        mtm_test_assign(matrix_density_filled_zero, matrix_density_filled_empty);
        mtm_test_assign(matrix_density_filled_empty, matrix_density_filled_empty);

        run_tests_on_all_density_types(mtm_test_flag_storage);

        mtm_test_iterator_access_filled<mx_type::filled_storage_type>(1, 1);
        mtm_test_iterator_access_filled<mx_type::filled_storage_type>(3, 1);
        mtm_test_iterator_access_filled<mx_type::filled_storage_type>(1, 3);
        mtm_test_iterator_access_filled<mx_type::filled_storage_type>(3, 3);
        mtm_test_iterator_access_filled<mx_type::filled_storage_type>(0, 0);

        mtm_test_iterator_access_filled<mx_type::filled_storage_zero_type>(1, 1);
        mtm_test_iterator_access_filled<mx_type::filled_storage_zero_type>(3, 1);
        mtm_test_iterator_access_filled<mx_type::filled_storage_zero_type>(1, 3);
        mtm_test_iterator_access_filled<mx_type::filled_storage_zero_type>(3, 3);
        mtm_test_iterator_access_filled<mx_type::filled_storage_zero_type>(0, 0);

        mtm_test_iterator_access_sparse();

        mtm_test_const_iterator();
    }

    if (opt.test_perf)
    {
        mtm_perf_test_storage_creation();
        mtm_perf_test_storage_set_numeric();
        mtm_perf_test_iterate_elements();
    }

    cout << "Test finished successfully!" << endl;
    return EXIT_SUCCESS;
}
