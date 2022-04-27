.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

======================
GraphicsMagick Thanks
======================

We would like to thank the following organizations or individuals who
may have not contributed code but contributed substantially in other
ways:

.. contents::
  :local:

Fuzz Testing
------------

  * Gustavo Grieco

  * Agostino Sarubbo

  * Jodie Cunningham

  * Gynvael Coldwind and Mateusz Jurczyk of the Google Security Team

  * Hanno Böck

  * Tobias Ospelt

  * Michal Zalewski

  * Moshe Kaplan

Security Analysis/Reports
-------------------------

  * John Lightsey

  * David Chan

  * Federico Larumbe

Static Code Analysis
--------------------

.. _`Coverity` : http://www.synopsys.com/software/coverity/

  * `Coverity`_ was used (free of charge) to perform static code
    analysis of the software.  After resolving many issues, there are
    now zero reported defects per 1000 lines of code.

Software Analysis Tools
-----------------------

.. _`AddressSanitizer` : https://code.google.com/p/address-sanitizer/

.. _`Valgrind` : http://www.valgrind.org/

.. _`American fuzzy lop` : http://lcamtuf.coredump.cx/afl/

.. _`QuickFuzz` : http://quickfuzz.org/

  * `American fuzzy lop`_ was used (by many) to produce and discover
    many of the files which caused problems for the software.

  * `QuickFuzz`_ was used (by Gustavo Grieco) to discover issues in the
    SVG renderer.  `QuickFuzz`_ provides specific support for a set of
    file formats and is very good at chasing down and isolating
    weaknesses.

  * `AddressSanitizer`_ (ASan) was used to detect and isolate memory
    access issues.

  * `Valgrind`_ was used to detect and isolate memory access issues as
    well as memory leaks.

Additional Acknowledgements
---------------------------

David Pensak, E. I. du Pont de Nemours and Company
		For providing the computing environment that made
		developing ImageMagick possible.

Alexander Zimmermann
		Responsible for the ImageMagick Linux binary
		distributions for many years. His efforts are very much
		appreciated.

Paul Heckbert, Carnegie Mellon University
		Image resizing is based on Paul Heckbert's Zoom program.

Paul Raveling, USC Information Sciences Institute
		The spatial subdivision color reduction algorithm is
		based on his Img software.		

Michael Halle, Spatial Imaging Group at MIT
		For the initial implementation of Alan Paeth's image
		rotation algorithm.

Peder Langlo, Hewlett Packard
		Made hundreds of suggestions and bug reports. Without
		Peder, this software would not be nearly as useful as it
		is today.

Rod Bogart and John W. Peterson, University of Utah
		Image compositing is loosely based on rlecomp of the
		Utah Raster Toolkit.

Alvy Ray Smith and Eric Ray Lyons
		HWB color transform and algorithm.

Thomas R Crimmins
		Inventor of the eight hull algorithm used for speckle
		reduction.


---------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

| Copyright |copy| GraphicsMagick Group 2002 - 2017

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/Copyright.html.

