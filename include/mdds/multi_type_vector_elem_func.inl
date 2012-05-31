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

namespace mdds {

typedef mtv::default_element_block<mtv::element_type_numeric, double>     numeric_cell_block;
typedef mtv::default_element_block<mtv::element_type_string, std::string> string_cell_block;
typedef mtv::default_element_block<mtv::element_type_index, size_t>       index_cell_block;
typedef mtv::default_element_block<mtv::element_type_boolean, bool>       boolean_cell_block;

mtv::element_t mdds_mtv_get_element_type(double)
{
    return mtv::element_type_numeric;
}

mtv::element_t mdds_mtv_get_element_type(const std::string&)
{
    return mtv::element_type_string;
}

mtv::element_t mdds_mtv_get_element_type(size_t)
{
    return mtv::element_type_index;
}

mtv::element_t mdds_mtv_get_element_type(bool)
{
    return mtv::element_type_boolean;
}

void mdds_mtv_set_value(mtv::base_element_block& block, size_t pos, double val)
{
    numeric_cell_block::set_value(block, pos, val);
}

void mdds_mtv_set_value(mtv::base_element_block& block, size_t pos, const std::string& val)
{
    string_cell_block::set_value(block, pos, val);
}

void mdds_mtv_set_value(mtv::base_element_block& block, size_t pos, size_t val)
{
    index_cell_block::set_value(block, pos, val);
}

void mdds_mtv_set_value(mtv::base_element_block& block, size_t pos, bool val)
{
    boolean_cell_block::set_value(block, pos, val);
}

}
