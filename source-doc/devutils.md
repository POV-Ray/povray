@page devutils  Development Utilities


Aside from the necessary material to re-build POV-Ray from scratch, the GitHub repository (and source package) also
includes a few utilities that may make life a little easier for developers. These usually reside in the platform or
build tool specific sections of the directory tree.


Git Utilities
=============

  - `tools/git/hooks/pre-commit`: Copy this to your local repository's `.git/hooks` directory for some automatic
    housekeeping at each commit. Most notably, this will auto-increment the version number for official pre-releases,
    if you commit any changes to the source code. It will also clean up whitespace and do a sanity check of the
    file header comments.

  - `tools/git/revision.sh`: This shell script will re-generate `revision.txt` from the Git history.
    @note
        The newly generated `revision.txt` is _not_ intended to be committed as-is, but rather merged with the
        previous version manually.


Visual Studio 2010 Utilities
============================

@todo
    This section is outdated.

  - `windows/vs10/autoexp.dat`: For easier viewing of common POV-Ray data structures, merge the snippets in this file
    into `C:\Program Files\Microsoft Visual Studio 10.0\Common7\Packages\Debugger\autoexp.dat` (or wherever you happen
    to have installed Visual Studio).

  - `windows/vs10/StepOver.reg`: Import this to your registry to step over some library code during debugging.
    Currently, this steps over any code in std::shared_ptr and std::vector.
    @warning
        If you are already using the step over feature, check that the registry keys in `StepOver.reg` don't
        conflict with any already in use.
