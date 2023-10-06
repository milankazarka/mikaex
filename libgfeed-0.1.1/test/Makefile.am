LIBS = $(GFEED_LIBS)

AM_CPPFLAGS = $(GFEED_CFLAGS) \
              -I$(top_srcdir)/src

noinst_PROGRAMS = test_rss \
                  test_rss_load \
                  test_rss_load_buffer \
                  test_atom \
                  test_atom_load \
                  test_atom_load_buffer

LDADD = $(top_srcdir)/src/libgfeed.la
