AUTOMAKE_OPTIONS = 1.11 dist-zip dist-bzip2 foreign
ACLOCAL_AMFLAGS = -I unix/m4
AM_DISTCHECK_CONFIGURE_FLAGS = --with-tcl=${TCL_BIN_DIR} --with-tk=${TK_BIN_DIR} --with-magick=${wand_config} --exec-prefix=${prefix}

EXTRA_DIST=LICENSE tests images doc pkgIndex.tcl
SUBDIRS = generic

install-data-local:
	$(mkinstalldirs) $(DESTDIR)@libdir@/TclMagick@VERSION@
	$(INSTALL_DATA) $(top_srcdir)/pkgIndex.tcl $(DESTDIR)@libdir@/TclMagick@VERSION@

uninstall-local:
	rm -v $(DESTDIR)@libdir@/TclMagick@VERSION@/pkgIndex.tcl
