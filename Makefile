prefix=/usr/local
datarootdir=${prefix}/share
PACKAGE_TARNAME=mdds

OBJDIR=obj
SRCDIR=src
INCDIR=include
MISCDIR=misc
QUICKCHECKDIR=quickcheck

CPPFLAGS= -Wall -Os -g -pedantic-errors -DMDDS_DEBUG_NODE_BASE -DMDDS_UNIT_TEST -std=c++0x -I$(INCDIR)
LDFLAGS=

EXECS= \
	flat_segment_tree_test \
	segment_tree_test \
	point_quad_tree_test \
	mixed_type_matrix_test \
	multi_type_matrix_test \
	rectangle_set_test \
	multi_type_vector_test_default \
	multi_type_vector_test_custom \
	sorted_string_map_test

HEADERS= \
	./$(INCDIR)/mdds/compat/unique_ptr.hpp \
	./$(INCDIR)/mdds/flat_segment_tree_def.inl \
	./$(INCDIR)/mdds/flat_segment_tree.hpp \
	./$(INCDIR)/mdds/flat_segment_tree_itr.hpp \
	./$(INCDIR)/mdds/global.hpp \
	./$(INCDIR)/mdds/hash_container/map.hpp \
	./$(INCDIR)/mdds/mixed_type_matrix_def.inl \
	./$(INCDIR)/mdds/mixed_type_matrix_element.hpp \
	./$(INCDIR)/mdds/mixed_type_matrix_flag_storage.hpp \
	./$(INCDIR)/mdds/mixed_type_matrix.hpp \
	./$(INCDIR)/mdds/mixed_type_matrix_storage_filled_linear.inl \
	./$(INCDIR)/mdds/mixed_type_matrix_storage.hpp \
	./$(INCDIR)/mdds/mixed_type_matrix_storage_sparse.inl \
	./$(INCDIR)/mdds/multi_type_matrix.hpp \
	./$(INCDIR)/mdds/multi_type_matrix_def.inl \
	./$(INCDIR)/mdds/node.hpp \
	./$(INCDIR)/mdds/point_quad_tree.hpp \
	./$(INCDIR)/mdds/quad_node.hpp \
	./$(INCDIR)/mdds/rectangle_set_def.inl \
	./$(INCDIR)/mdds/rectangle_set.hpp \
	./$(INCDIR)/mdds/segment_tree.hpp \
	./$(INCDIR)/mdds/sorted_string_map_def.inl \
	./$(INCDIR)/mdds/sorted_string_map.hpp \
	./$(INCDIR)/mdds/multi_type_vector.hpp \
	./$(INCDIR)/mdds/multi_type_vector_custom_func1.hpp \
	./$(INCDIR)/mdds/multi_type_vector_custom_func2.hpp \
	./$(INCDIR)/mdds/multi_type_vector_custom_func3.hpp \
	./$(INCDIR)/mdds/multi_type_vector_itr.hpp \
	./$(INCDIR)/mdds/multi_type_vector_trait.hpp \
	./$(INCDIR)/mdds/multi_type_vector_types.hpp \
	./$(INCDIR)/mdds/multi_type_vector_macro.hpp \
	./$(INCDIR)/mdds/multi_type_vector_def.inl

DEPENDS= \
	$(HEADERS)

TESTS = \
	test.fst \
	test.recset \
	test.pqt \
	test.st \
	test.mtm \
	test.mtv \
	test.mtmatrix \
	test.ssmap

TESTS_MEM = \
	test.pqt.mem \
	test.recset.mem \
	test.fst.mem \
	test.st.mem \
	test.mtm.mem \
	test.mtmatrix \
	test.ssmap

# You may add $(TESTS_MEM) if valgrind is available on your platform
ALL_TESTS = $(TESTS)

nothing:
	@echo There is nothing to build since this is a header-only library.

all: $(EXECS)

pre:
	mkdir -p $(OBJDIR)

$(OBJDIR)/flat_segment_tree_test.o: ./$(SRCDIR)/flat_segment_tree_test.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/flat_segment_tree_test.cpp

$(OBJDIR)/segment_tree_test.o: ./$(SRCDIR)/segment_tree_test.cpp  $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/segment_tree_test.cpp

$(OBJDIR)/rectangle_set_test.o: ./$(SRCDIR)/rectangle_set_test.cpp  $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/rectangle_set_test.cpp

$(OBJDIR)/point_quad_tree_test.o: ./$(SRCDIR)/point_quad_tree_test.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/point_quad_tree_test.cpp

$(OBJDIR)/mixed_type_matrix_test.o: ./$(SRCDIR)/mixed_type_matrix_test.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/mixed_type_matrix_test.cpp

$(OBJDIR)/multi_type_vector_test_default.o: ./$(SRCDIR)/multi_type_vector_test_default.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/multi_type_vector_test_default.cpp

$(OBJDIR)/multi_type_vector_test_custom.o: ./$(SRCDIR)/multi_type_vector_test_custom.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/multi_type_vector_test_custom.cpp

$(OBJDIR)/multi_type_vector_test_perf.o: ./$(SRCDIR)/multi_type_vector_test_perf.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/multi_type_vector_test_perf.cpp

$(OBJDIR)/multi_type_matrix_test.o: ./$(SRCDIR)/multi_type_matrix_test.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/multi_type_matrix_test.cpp

$(OBJDIR)/sorted_string_map_test.o: ./$(SRCDIR)/sorted_string_map_test.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/sorted_string_map_test.cpp

flat_segment_tree_test: pre $(OBJDIR)/flat_segment_tree_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/flat_segment_tree_test.o -o $@

segment_tree_test: pre $(OBJDIR)/segment_tree_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/segment_tree_test.o -o $@

rectangle_set_test: pre $(OBJDIR)/rectangle_set_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/rectangle_set_test.o -o $@

point_quad_tree_test: pre $(OBJDIR)/point_quad_tree_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/point_quad_tree_test.o -o $@

mixed_type_matrix_test: pre $(OBJDIR)/mixed_type_matrix_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/mixed_type_matrix_test.o -o $@

multi_type_vector_test_default: pre $(OBJDIR)/multi_type_vector_test_default.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/multi_type_vector_test_default.o -o $@

multi_type_vector_test_custom: pre $(OBJDIR)/multi_type_vector_test_custom.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/multi_type_vector_test_custom.o -o $@

multi_type_vector_test_perf: pre $(OBJDIR)/multi_type_vector_test_perf.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/multi_type_vector_test_perf.o -o $@

multi_type_matrix_test: pre $(OBJDIR)/multi_type_matrix_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/multi_type_matrix_test.o -o $@

sorted_string_map_test: pre $(OBJDIR)/sorted_string_map_test.o
	$(CXX) $(LDFLAGS) $(OBJDIR)/sorted_string_map_test.o -o $@

multi_type_vector_test: multi_type_vector_test_default multi_type_vector_test_custom multi_type_vector_test_perf

stlperf_test: pre ./$(SRCDIR)/stlperf_test.cpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) ./$(SRCDIR)/stlperf_test.cpp -o $@

$(OBJDIR)/template_test.o: ./$(SRCDIR)/template_test.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ ./$(SRCDIR)/template_test.cpp

test.fst: flat_segment_tree_test
	./flat_segment_tree_test func

test.fst.perf: flat_segment_tree_test
	./flat_segment_tree_test perf

test.fst.mem: flat_segment_tree_test
	valgrind --tool=memcheck --leak-check=full ./flat_segment_tree_test func

test.pqt: point_quad_tree_test
	./point_quad_tree_test

test.pqt.mem: point_quad_tree_test
	valgrind --tool=memcheck --leak-check=full ./point_quad_tree_test

test.recset: rectangle_set_test
	./rectangle_set_test func

test.recset.perf: rectangle_set_test
	./rectangle_set_test perf

test.recset.mem: rectangle_set_test
	valgrind --tool=memcheck --leak-check=full ./rectangle_set_test func

test.st: segment_tree_test
	./segment_tree_test func

test.st.perf: segment_tree_test
	./segment_tree_test perf

test.st.mem: segment_tree_test
	valgrind --tool=memcheck --leak-check=full ./segment_tree_test func

test.mtm: mixed_type_matrix_test
	./mixed_type_matrix_test func

test.mtm.perf: mixed_type_matrix_test
	./mixed_type_matrix_test perf

test.mtm.mem: mixed_type_matrix_test
	valgrind --tool=memcheck --leak-check=full ./mixed_type_matrix_test func

test.mtv: multi_type_vector_test_default multi_type_vector_test_custom
	./multi_type_vector_test_default
	./multi_type_vector_test_custom

test.mtv.perf: multi_type_vector_test_perf
	./multi_type_vector_test_perf

test.mtv.mem: multi_type_vector_test_default multi_type_vector_test_custom
	valgrind --tool=memcheck --leak-check=full ./multi_type_vector_test_default
	valgrind --tool=memcheck --leak-check=full ./multi_type_vector_test_custom

test.mtmatrix: multi_type_matrix_test
	./multi_type_matrix_test func

test.mtmatrix.perf: multi_type_matrix_test
	./multi_type_matrix_test perf

test.mtmatrix.mem: multi_type_matrix_test
	valgrind --tool=memcheck --leak-check=full ./multi_type_matrix_test func

test.ssmap: sorted_string_map_test
	./sorted_string_map_test func

test.ssmap.mem: sorted_string_map_test
	valgrind --tool=memcheck --leak-check=full ./sorted_string_map_test func

test.stl: stlperf_test
	./stlperf_test

$(QUICKCHECKDIR)/flat_segment_tree.o: $(QUICKCHECKDIR)/flat_segment_tree.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $<

$(QUICKCHECKDIR)/flat_segment_tree: $(QUICKCHECKDIR)/flat_segment_tree.o pre
	$(CXX) $(LDFLAGS) $< -o $@

install: $(HEADERS)
	install -d $(DESTDIR)${prefix}/include/mdds
	install -d $(DESTDIR)${prefix}/include/mdds/hash_container
	install -d $(DESTDIR)${prefix}/include/mdds/compat
	install -d $(DESTDIR)${datarootdir}/doc/${PACKAGE_TARNAME}
	install -d $(DESTDIR)${prefix}/share
	install -d $(DESTDIR)${prefix}/share/pkgconfig
	install -m 644 ./$(INCDIR)/mdds/*.hpp $(DESTDIR)${prefix}/include/mdds 
	install -m 644 ./$(INCDIR)/mdds/*.inl $(DESTDIR)${prefix}/include/mdds 
	install -m 644 ./$(INCDIR)/mdds/compat/*.hpp $(DESTDIR)${prefix}/include/mdds/compat 
	install -m 644 ./$(INCDIR)/mdds/hash_container/*.hpp $(DESTDIR)${prefix}/include/mdds/hash_container 
	install -m 644 $(MISCDIR)/mdds.pc $(DESTDIR)${prefix}/share/pkgconfig 
	install -m 644 ./AUTHORS ./COPYING ./NEWS ./README.md ./VERSION $(DESTDIR)${datarootdir}/doc/${PACKAGE_TARNAME}

check: $(ALL_TESTS)

quickcheck: $(QUICKCHECKDIR)/flat_segment_tree
	$(QUICKCHECKDIR)/flat_segment_tree

clean:
	rm -rf $(OBJDIR) 2>/dev/null || /bin/true
	rm $(EXECS) 2>/dev/null || /bin/true
	rm $(QUICKCHECKDIR)/flat_segment_tree $(QUICKCHECKDIR)/*.o 2>/dev/null || true
