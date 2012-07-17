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

#ifndef __MDDS_MULTI_TYPE_VECTOR_MACRO_HPP__
#define __MDDS_MULTI_TYPE_VECTOR_MACRO_HPP__

#define MDDS_MTV_DEFINE_ELEMENT_CALLBACKS(_type_,_type_id_,_empty_val_,_block_) \
 \
inline mtv::element_t mdds_mtv_get_element_type(const _type_&) \
{ \
    return _type_id_; \
} \
 \
inline void mdds_mtv_get_empty_value(_type_& val) \
{ \
    val = _empty_val_; \
} \
 \
inline void mdds_mtv_set_value(mtv::base_element_block& block, size_t pos, const _type_& val) \
{ \
    _block_::set_value(block, pos, val); \
} \
 \
inline void mdds_mtv_get_value(const mtv::base_element_block& block, size_t pos, _type_& val) \
{ \
    _block_::get_value(block, pos, val); \
} \
 \
template<typename _Iter> \
void mdds_mtv_set_values( \
    mtv::base_element_block& block, size_t pos, const _type_&, const _Iter& it_begin, const _Iter& it_end) \
{ \
    _block_::set_values(block, pos, it_begin, it_end); \
} \
 \
inline void mdds_mtv_append_value(mtv::base_element_block& block, const _type_& val) \
{ \
    _block_::append_value(block, val); \
} \
 \
inline void mdds_mtv_prepend_value(mtv::base_element_block& block, const _type_& val) \
{ \
    _block_::prepend_value(block, val); \
} \
 \
template<typename _Iter> \
void mdds_mtv_prepend_values(mtv::base_element_block& block, const _type_&, const _Iter& it_begin, const _Iter& it_end) \
{ \
    _block_::prepend_values(block, it_begin, it_end); \
} \
 \
template<typename _Iter> \
void mdds_mtv_append_values(mtv::base_element_block& block, const _type_&, const _Iter& it_begin, const _Iter& it_end) \
{ \
    _block_::append_values(block, it_begin, it_end); \
} \
 \
template<typename _Iter> \
void mdds_mtv_assign_values(mtv::base_element_block& dest, const _type_&, const _Iter& it_begin, const _Iter& it_end) \
{ \
    _block_::assign_values(dest, it_begin, it_end); \
} \
 \
template<typename _Iter> \
void mdds_mtv_insert_values( \
    mtv::base_element_block& block, size_t pos, const _type_&, const _Iter& it_begin, const _Iter& it_end) \
{ \
    _block_::insert_values(block, pos, it_begin, it_end); \
} \
 \
inline mtv::base_element_block* mdds_mtv_create_new_block(size_t init_size, const _type_& val) \
{ \
    return _block_::create_block_with_value(init_size, val); \
}

#endif
