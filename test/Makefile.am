CFLAGS = @CFLAGS@ -std=gnu99

h_sources = itzam_errors.h

bin_PROGRAMS = itzam_btree_test_insert itzam_btree_test_stress itzam_btree_test_threads itzam_btree_test_strvar itzam_btree_test_befaft itzam_btree_test_cursor

itzam_btree_test_insert_SOURCES = itzam_btree_test_insert.c
itzam_btree_test_stress_SOURCES = itzam_btree_test_stress.c
itzam_btree_test_threads_SOURCES = itzam_btree_test_threads.c
itzam_btree_test_strvar_SOURCES = itzam_btree_test_strvar.c
itzam_btree_test_befaft_SOURCES = itzam_btree_test_befaft.c
itzam_btree_test_cursor_SOURCES = itzam_btree_test_cursor.c

LIBS = -L../src -litzam -lpthread

EXTRA_DIST = itzam_errors.h
