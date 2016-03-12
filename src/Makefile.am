INCLUDES = -I$(top_srcdir)
CFLAGS = @CFLAGS@ -std=gnu99

h_sources = itzam.h

cpp_sources = itzam_util.c itzam_data.c itzam_btree.c

lib_LTLIBRARIES = libitzam.la

libitzam_la_SOURCES = $(h_sources) $(cpp_sources)
libitzam_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

DEFS = -I. -I$(srcdir)
