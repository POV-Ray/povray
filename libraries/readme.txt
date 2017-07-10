This directory contains the source code (or subsets thereof) of all the
3rd party libraries required or recommended to build POV-Ray from scratch.

-------------------------------------------------------------------------------
NOTE: The presence of this directory should _by no means_ be understood as a
recommendation to use these particular versions of the libraries. To the
contrary, on most platforms it is highly recommended to use up-to-date versions
of the libraries instead.
-------------------------------------------------------------------------------

Originally dating back to pre-Internet times when obtaining 3rd party libraries
was non-trivial for everyone, the only reason for this directory's continued
existence are platforms that still have no well-established integrated
mechanism for obtaining popular 3rd party libraries (yes, Microsoft Windows,
we're talking about you here), making it difficult not only to obtain these
libraries in pre-built format (let alone in a variant ideal for use in
POV-Ray's build process), but also to obtain some of the toolsets normally
required to build these libraries from source code. Our workaround for this
problem was to set up our own custom build processes for these libraries, using
the same tools as POV-Ray's own build process. However, this approach requires
the use of well-defined versions of those libraries, which is exactly what this
directory continues to provide.
