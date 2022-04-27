.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=======================
GraphicsMagick Security
=======================

Background
----------

.. _`SSRF` : https://cwe.mitre.org/data/definitions/918.html

Although GraphicsMagick is image processing software, security is a
very important consideration for GraphicsMagick.  GraphicsMagick may
be used to open files and URLs produced by an untrusted party.  Given
a suitable weakness (which we make every effort to prevent), an
intentionally constructed file might be able to cause the software to
crash, leak memory, request huge amounts memory, run forever, or in
the worst case execute arbitrary code, including shell code.
GraphicsMagick is very powerful and complex software supporting many
capabilities and so untrusted parties should never be allowed to
submit arbitrary requests into it.

GraphicsMagick includes the ability to access arbitrary http and ftp
URLs as well as local image, font, and SVG files.  The SVG renderer
supports read access to http and ftp URLs as well as local files
according to the SVG specification.  Since URLs and local file paths
may be included in SVG files, untrusted SVG files may create a Server
Side Request Forgery (`SSRF`_) vulnerability since URL requests are
done by the computer executing the SVG, which may be in a realm of
trust (e.g. inside the firewall and able to access "localhost"
addresses).

The GraphicsMagick project is continually striving to eliminate
conditions in the software which might pose a risk for its users while
not constraining what its users may do with the software.

Reporting Issues
----------------

.. _`GraphicsMagick Security`: mailto:graphicsmagick-security@graphicsmagick.org

.. _`GraphicsMagick Bug Tracker`: https://sourceforge.net/p/graphicsmagick/bugs/

If you become aware of a serious security issue with GraphicsMagick,
then it may be addressed by email directly to the GraphicsMagick
maintainers or to the `GraphicsMagick Security`_ mail address.  More
minor issues are best addressed via the `GraphicsMagick Bug Tracker`_
at SourceForge.

Reporting an issue will allow us to fix it so that future releases of
the software won't suffer from the problem.


Safe Use Of The Software
------------------------

.. _`graphicsmagick-announce` : https://lists.sourceforge.net/lists/listinfo/graphicsmagick-announce
.. _`Container` : https://en.wikipedia.org/wiki/Operating-system-level_virtualization
.. _`FreeBSD Jail` : https://www.freebsd.org/doc/en_US.ISO8859-1/books/handbook/jails.html
.. _`Solaris Zone` : https://illumos.org/man/5/zones
.. _`chrooted` : https://en.wikipedia.org/wiki/Chroot

You are the first line of defense when it comes to GraphicsMagick
security!

If you are operating a server which supports file uploads from
untrusted users, or delivered via a network protocol such as http,
ftp, or email, then you should take steps to assure that a problem
with opening/processing the file does not place the whole server at
risk.  These are steps which can be taken:

1. Subscribe to the `graphicsmagick-announce`_ mailing list so that
   you are informed about new GraphicsMagick releases or special
   security bulletins.

2. Make sure that GraphicsMagick is up to date as reported on the
   GraphicsMagick web site.  Don't simply trust that packages from
   your operating system vendor are kept up to date or are updated to
   include security fixes.  Keeping GraphicsMagick up to date might
   require that you compile GraphicsMagick yourself from source code.

3. Execute the software in a `Container`_, `FreeBSD Jail`_, `Solaris
   Zone`_, or `chrooted`_ environment such that it can not cause harm
   to the system running it.

4. Execute the software as a least-privileged user (e.g. 'nobody').

5. Normalize input file names or any other external inputs so that
   they are under your control and not controlled by an untrusted
   party.  This should include any file name specifications, which may
   include arbitrary 'glob' patterns (wildcards), requiring hours or
   days to complete if sufficiently close long file names exist.

6. Enforce that uploaded files are passed to the expected reader.  For
   example, the uploaded file "file.jpg" is forced to be read by the
   JPEG reader (rather than a reader selected based on header magic
   testing) by using the file name "jpg:file.jpg".  If the file is not
   really what was indicated, then an error is reported.

   GraphicsMagick supports a great many file types and auto-detects
   many file types based on their content rather than file extension.
   The file which pretends to be an ordinary PNG or JPEG file might be
   something else entirely.  Note that even using independent file
   header testing may not be sufficient since it is possible to
   construct valid files with a header which appears to be several
   different types, but the first type which matches while testing the
   header will be selected.

7. Apply resource limits via the `-limit` option or the
   `MAGICK_LIMIT_*` environment variables (e.g. `export
   MAGICK_LIMIT_PIXELS=30Mp`, `export MAGICK_LIMIT_MEMORY=500Mb`).
   Also consider setting resource limits using the `ulimit` command.

8. Consider using the `MAGICK_CODER_STABILITY` environment variable to
   constrain the supported file formats to the subsets selected by
   `PRIMARY` or `STABLE`.  After setting this environment variable
   (e.g. `export MAGICK_CODER_STABILITY=PRIMARY`), use `gm
   convert -list format` and verify that the format support you need
   is enabled.  Selecting the `PRIMARY` or `STABLE` options blocks
   access of http and ftp URLs (`SSRF`_ vulnerability), but does not
   block SVG renderer access to read local image files.


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2017
