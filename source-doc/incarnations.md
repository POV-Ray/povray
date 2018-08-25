@page incarnations  POV-Ray Incarnations

POV-Ray comes in different _incarnations_, that is, particular applications that
differ in the user interface and/or the operating system they are designed for;
for example, _POV-Ray for Windows_ is one such incarnation of POV-Ray.


Creating a New POV-Ray Incarnation
==================================

If you want to develop a new incarnation of POV-Ray, here are a few things you
should look out for:

  - Make sure your build process automatically picks up on any newly added or
    removed source code files in the @ref source directory (including new
    sub-directories). Developers working on the incarnation-neutral modules of
    POV-Ray _will_ add or remove such files comparatively frequently without
    warning.

  - In the same vein, make sure your distribution process automatically picks up
    on any newly added or removed files in existing sub-directories of the
    @ref distribution directory.
