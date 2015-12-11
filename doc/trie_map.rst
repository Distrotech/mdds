.. highlight:: cpp


Trie maps
=========

Trie map
--------

.. doxygenclass:: mdds::trie_map
   :members:


Packed trie map
---------------

.. doxygenclass:: mdds::packed_trie_map
   :members:

Example
-------

The following code example illustrates how to populate a trie_map instance and
perform prefix searches.  The later part of the example also shows how you can
create a packed version of the content for faster lookups and reduced memory
footprint.

::

    #include <mdds/global.hpp>  // for MDDS_ASCII
    #include <mdds/trie_map.hpp>
    #include <iostream>

    using namespace std;

    typedef mdds::trie_map<mdds::trie::std_string_trait, int> trie_map_type;

    int main()
    {
        // Cities in North Carolina and their populations in 2013.
        trie_map_type nc_cities(-1); // Use -1 as the null value.

        // Insert key-value pairs.
        nc_cities.insert(MDDS_ASCII("Charlotte"),     792862);
        nc_cities.insert(MDDS_ASCII("Raleigh"),       431746);
        nc_cities.insert(MDDS_ASCII("Greensboro"),    279639);
        nc_cities.insert(MDDS_ASCII("Durham"),        245475);
        nc_cities.insert(MDDS_ASCII("Winston-Salem"), 236441);
        nc_cities.insert(MDDS_ASCII("Fayetteville"),  204408);
        nc_cities.insert(MDDS_ASCII("Cary"),          151088);
        nc_cities.insert(MDDS_ASCII("Wilmington"),    112067);
        nc_cities.insert(MDDS_ASCII("High Point"),    107741);
        nc_cities.insert(MDDS_ASCII("Greenville"),    89130);
        nc_cities.insert(MDDS_ASCII("Asheville"),     87236);
        nc_cities.insert(MDDS_ASCII("Concord"),       83506);
        nc_cities.insert(MDDS_ASCII("Gastonia"),      73209);
        nc_cities.insert(MDDS_ASCII("Jacksonville"),  69079);
        nc_cities.insert(MDDS_ASCII("Chapel Hill"),   59635);
        nc_cities.insert(MDDS_ASCII("Rocky Mount"),   56954);
        nc_cities.insert(MDDS_ASCII("Burlington"),    51510);
        nc_cities.insert(MDDS_ASCII("Huntersville"),  50458);
        nc_cities.insert(MDDS_ASCII("Wilson"),        49628);
        nc_cities.insert(MDDS_ASCII("Kannapolis"),    44359);
        nc_cities.insert(MDDS_ASCII("Apex"),          42214);
        nc_cities.insert(MDDS_ASCII("Hickory"),       40361);
        nc_cities.insert(MDDS_ASCII("Goldsboro"),     36306);

        cout << "Cities that start with 'Cha' and their populations:" << endl;
        auto matches = nc_cities.prefix_search(MDDS_ASCII("Cha"));
        for (const auto& kv : matches)
        {
            cout << "  " << kv.first << ": " << kv.second << endl;
        }

        cout << "Cities that start with 'W' and their populations:" << endl;
        matches = nc_cities.prefix_search(MDDS_ASCII("W"));
        for (const auto& kv : matches)
        {
            cout << "  " << kv.first << ": " << kv.second << endl;
        }

        // Create a compressed version of the container.  It works nearly identically.
        auto packed = nc_cities.pack();

        cout << "Cities that start with 'C' and their populations:" << endl;
        matches = packed.prefix_search(MDDS_ASCII("C"));
        for (const auto& kv : matches)
        {
            cout << "  " << kv.first << ": " << kv.second << endl;
        }

        // Individual search.
        int result = packed.find(MDDS_ASCII("Wilmington"));
        cout << "Population of Wilmington: " << result << endl;

        // You get a "null value" when the container doesn't have specified key.
        result = packed.find(MDDS_ASCII("Asheboro"));
        cout << "Population of Asheboro: " << result << endl;

        return EXIT_SUCCESS;
    }

You'll get the following output when compiling the above code and executing it::

    Cities that start with 'Cha' and their populations:
      Chapel Hill: 59635
      Charlotte: 792862
    Cities that start with 'W' and their populations:
      Wilmington: 112067
      Wilson: 49628
      Winston-Salem: 236441
    Cities that start with 'C' and their populations:
      Cary: 151088
      Chapel Hill: 59635
      Charlotte: 792862
      Concord: 83506
    Population of Wilmington: 112067
    Population of Asheboro: -1
