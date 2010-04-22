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

#ifndef __MDDS_POINT_QUAD_TREE_HPP__
#define __MDDS_POINT_QUAD_TREE_HPP__

#include "mdds/quad_node.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

namespace mdds {

template<typename _Key, typename _Data>
class point_quad_tree
{
public:
    typedef _Key    key_type;
    typedef _Data   data_type;
    typedef size_t  size_type;

    struct node;
    typedef ::boost::intrusive_ptr<node> node_ptr;

    struct node : quad_node_base<node_ptr, node, key_type>
    {
        data_type* data;
        node(key_type _x, key_type _y, data_type* _data) :
            quad_node_base<node_ptr, node, key_type>(_x, _y),
            data(_data) {}

        void dispose()
        {
        }
    };

    point_quad_tree();
    ~point_quad_tree();

    void insert(key_type x, key_type y, data_type* data);

    void dump_tree(const ::std::string& fpath) const;

private:
    void dump_node(const node* p, ::std::ofstream& file) const;

private:
    node_ptr    m_root;
};

template<typename _Key, typename _Data>
point_quad_tree<_Key,_Data>::point_quad_tree() :
    m_root(NULL)
{
}

template<typename _Key, typename _Data>
point_quad_tree<_Key,_Data>::~point_quad_tree()
{
}

template<typename _Key, typename _Data>
void point_quad_tree<_Key,_Data>::insert(key_type x, key_type y, data_type* data)
{
    if (!m_root)
    {
        // The very first node.
        m_root.reset(new node(x, y, data));
        return;
    }

    node_ptr cur_node = m_root;
    while (true)
    {
        if (cur_node->x == x && cur_node->y == y)
        {
            // Replace the current data with this, and we are done!
            cur_node->data = data;
            return;
        }

        node_quadrant_t quad = cur_node->get_quadrant(x, y);
        switch (quad)
        {
            case quad_north_east:
                if (cur_node->northeast)
                    cur_node = cur_node->northeast;
                else
                {
                    cur_node->northeast.reset(new node(x, y, data));
                    cur_node->northeast->parent = cur_node;
                    return;
                }
                break;
            case quad_north_west:
                if (cur_node->northwest)
                    cur_node = cur_node->northwest;
                else
                {
                    cur_node->northwest.reset(new node(x, y, data));
                    cur_node->northwest->parent = cur_node;
                    return;
                }
                break;
            case quad_south_east:
                if (cur_node->southeast)
                    cur_node = cur_node->southeast;
                else
                {
                    cur_node->southeast.reset(new node(x, y, data));
                    cur_node->southeast->parent = cur_node;
                    return;
                }
                break;
            case quad_south_west:
                if (cur_node->southwest)
                    cur_node = cur_node->southwest;
                else
                {
                    cur_node->southwest.reset(new node(x, y, data));
                    cur_node->southwest->parent = cur_node;
                    return;
                }
                break;
            default:
                throw general_error("unknown quadrant");
        }
    }
    assert(!"This should never be reached.");
}

template<typename _Key, typename _Data>
void point_quad_tree<_Key,_Data>::dump_tree(const ::std::string& fpath) const
{
    using namespace std;
    ofstream file(fpath.c_str());
    file << "<svg width=\"12cm\" height=\"12cm\" viewBox=\"0 0 200 200\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << endl;
    dump_node(m_root.get(), file);
    file << "</svg>" << endl;
}

template<typename _NodePtr>
void draw_svg_arrow(::std::ofstream& file, const _NodePtr start, const _NodePtr end)
{
    using namespace std;
    file << "<g stroke=\"green\">" << endl;
    file << "<line x1=\"" << start->x << "\" y1=\"" << start->y << "\" x2=\"" 
        << end->x << "\" y2=\"" << end->y << "\" stroke-width=\"0.5\"/>" << endl;
    file << "</g>" << endl;
}

template<typename _Key, typename _Data>
void point_quad_tree<_Key,_Data>::dump_node(const node* p, ::std::ofstream& file) const
{
    using namespace std;

    if (!p)
        return;

    file << "<circle cx=\"" << p->x << "\" cy=\"" << p->y << "\" r=\"0.5\""
        << " fill=\"black\" stroke=\"black\"/>" << endl;

    if (p->northwest)
        draw_svg_arrow<const node*>(file, p, p->northwest.get());

    if (p->northeast)
        draw_svg_arrow<const node*>(file, p, p->northeast.get());

    if (p->southwest)
        draw_svg_arrow<const node*>(file, p, p->southwest.get());

    if (p->southeast)
        draw_svg_arrow<const node*>(file, p, p->southeast.get());

    dump_node(p->northeast.get(), file);
    dump_node(p->northwest.get(), file);
    dump_node(p->southeast.get(), file);
    dump_node(p->southwest.get(), file);
}

}

#endif