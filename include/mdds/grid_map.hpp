/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#ifndef __MDDS_GRID_MAP_HPP__
#define __MDDS_GRID_MAP_HPP__

#include "mdds/global.hpp"
#include "mdds/grid_map_sheet.hpp"

#include <vector>
#include <algorithm>

namespace mdds {

/**
 * This container stores grid cells in a 3-dimensional hyperplane.  Cells
 * are accessed via 3-key combinations.
 */
template<typename _CellT, typename _SheetKeyT, typename _RowKeyT, typename _ColKeyT>
class grid_map
{
public:
    typedef _SheetKeyT sheet_key_type;
    typedef _RowKeyT   row_key_type;
    typedef _ColKeyT   col_key_type;

    typedef _CellT cell_type;
    typedef __gridmap::sheet<cell_type> sheet_type;

    grid_map();
    ~grid_map();

private:
    std::vector<sheet_type*> m_sheets;
};

}

#include "grid_map_def.inl"

#endif