LIBS = $(GFEED_LIBS)

AM_CPPFLAGS = $(GFEED_CFLAGS)

lib_LTLIBRARIES = libgfeed.la

libgfeed_la_SOURCES = rss.c \
                      rsschannel.c \
                      rsschannelitem.c \
                      rsscategory.c \
                      rsscloud.c \
                      rssimage.c \
                      rsstextinput.c \
                      rssenclosure.c \
                      rssguid.c \
                      rsssource.c \
                      atom.c \
                      atomcommon.c \
                      atomtext.c \
                      atomperson.c \
                      atomdate.c \
                      atomid.c \
                      atomcategory.c \
                      atomgenerator.c \
                      atomlink.c \
                      atomsource.c \
                      atomentry.c \
                      atomcontent.c

libgfeed_la_LDFLAGS = -no-undefined

libgfeed_include_HEADERS = gfeed.h \
                  rss.h \
                  rsschannel.h \
                  rsschannelitem.h \
                  rsscategory.h \
                  rsscloud.h \
                  rssimage.h \
                  rsstextinput.h \
                  rssenclosure.h \
                  rssguid.h \
                  rsssource.h \
                  atom.h \
                  atomcommon.h \
                  atomtext.h \
                  atomperson.h \
                  atomdate.h \
                  atomid.h \
                  atomcategory.h \
                  atomgenerator.h \
                  atomlink.h \
                  atomsource.h \
                  atomentry.h \
                  atomcontent.h

libgfeed_includedir = $(includedir)/libgfeed
