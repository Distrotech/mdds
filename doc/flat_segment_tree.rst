.. highlight:: cpp

Flat Segment Tree
=================

Code Example
------------

Simple use case
^^^^^^^^^^^^^^^

The following code demonstrates a simple use case of storing non-overlapping
ranged values and performing queries using :cpp:class:`~mdds::flat_segment_tree`::

    #include <mdds/flat_segment_tree.hpp>
    #include <string>
    #include <iostream>

    using namespace std;

    typedef mdds::flat_segment_tree<long, int> fst_type;

    int main()
    {
        // Define the begin and end points of the whole segment, and the default
        // value.
        fst_type db(0, 500, 0);

        db.insert_front(10, 20, 10);
        db.insert_back(50, 70, 15);
        db.insert_back(60, 65, 5);

        int value = -1;
        long beg = -1, end = -1;

        // Perform linear search.  This doesn't require the tree to be built
        // beforehand.  Note that the begin and end point parameters are optional.
        db.search(15, value, &beg, &end);
        cout << "The value at 15 is " << value << ", and this segment spans from " << beg << " to " << end << endl;;

        // Don't forget to build tree before calling search_tree().
        db.build_tree();

        // Perform tree search.  Tree search is generally a lot faster than linear
        // search, but requires the tree to be built beforehand.
        db.search_tree(62, value, &beg, &end);
        cout << "The value at 62 is " << value << ", and this segment spans from " << beg << " to " << end << endl;;
    }

Two types of searches are available.  One is linear search via
:cpp:func:`~mdds::flat_segment_tree::search` which performs its search only
through the leaf nodes.

Another one is tree search via :cpp:func:`~mdds::flat_segment_tree::search_tree`
which performs better than the linear search counterpart, but it does require
the search tree to be built ahead of time by calling
:cpp:func:`~mdds::flat_segment_tree::build_tree`.

API Reference
-------------

.. doxygenclass:: mdds::flat_segment_tree
   :members:
