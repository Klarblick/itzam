CFLAGS = @CFLAGS@ -std=gnu11 -O3

h_sources = itzam_errors.h

bin_PROGRAMS = insert stress threads strvar befaft cursor overwrite

insert_SOURCES = insert.c
stress_SOURCES = stress.c
threads_SOURCES = threads.c
strvar_SOURCES = strvar.c
befaft_SOURCES = befaft.c
cursor_SOURCES = cursor.c
overwrite_SOURCES = overwrite.c

LIBS = -L../src -litzam -lpthread

EXTRA_DIST = itzam_errors.h
