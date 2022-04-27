#!/bin/sh
# Create a PerlMagick MANIFEST file.  This needs to be run in a clean source tree.
rm -f MANIFEST
perl -MExtUtils::Manifest=mkmanifest -e 'mkmanifest()'

#find . -type f -print | egrep -v '(.#.*)|(CVS)|(core)|(.*_tmp\.*)|(perlmain\.*)|(^PerlMagick)|(~$)|(.*\.orig)|(.*\.cvsignore)|(.*\.gdbinit)|(demo/demo.jpg)|(Makefile.in)|(Makefile.old)' | sed -e 's:\./::' | sort > MANIFEST
