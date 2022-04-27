# Makefile for creating libTclMagick and libTkMagick

# Copyright David N. Welton <davidw@dedasys.com>, 2004, with many
# thanks to Bob Friesenhahn for all the help.

# AM_CFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

# Put this in its own package directory.
lib_magickdir = @libdir@/TclMagick@VERSION@
lib_magick_LTLIBRARIES =  libttkcommon.la libTclMagick.la libTkMagick.la

libttkcommon_la_SOURCES = libttkcommon.c libttkcommon.h
libttkcommon_la_LDFLAGS = @GM_LDFLAGS@ -no-undefined
libttkcommon_la_LIBADD = @TCL_STUB_LIB_SPEC@ @TCL_LIBS@ @TCL_LIBS@ @GM_LIBS@
libttkcommon_la_CPPFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

libTclMagick_la_SOURCES = TclMagick.c TclMagick.h libttkcommon.h
libTclMagick_la_LDFLAGS = @GM_LDFLAGS@ -L$(lib_magickdir) -no-undefined -module
libTclMagick_la_LIBADD = @TCL_STUB_LIB_SPEC@ @TCL_LIBS@ @TCL_LIBS@ @GM_LIBS@ libttkcommon.la
libTclMagick_la_CPPFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@

libTkMagick_la_SOURCES = TkMagick.c TclMagick.h libttkcommon.h
libTkMagick_la_LDFLAGS = @GM_LDFLAGS@ -L$(lib_magickdir) -no-undefined -module
libTkMagick_la_LIBADD = @TCL_STUB_LIB_SPEC@ @TK_STUB_LIB_SPEC@ @TCL_LIBS@ @TK_LIBS@ @GM_LIBS@ libttkcommon.la
libTkMagick_la_CPPFLAGS = @TCL_INCLUDES@ @GM_CPPFLAGS@ @TK_INCLUDES@ @TK_XINCLUDES@
