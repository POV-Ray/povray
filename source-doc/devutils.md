# Development Utilities {#devutils}

Aside from the necessary material to re-build POV-Ray from scratch, the GitHub repository (and source package) also
includes a few utilities that may make life a little easier for developers. These usually reside in the platform or
build tool specific sections of the directory tree.

@section vs10       Visual Studio 2010 utilities

  - `windows/vs10/autoexp.dat`: For easier viewing of common POV-Ray data structures, merge the snippets in this file
    into `C:\Program Files\Microsoft Visual Studio 10.0\Common7\Packages\Debugger\autoexp.dat` (or wherever you happen
    to have installed Visual Studio).

  - `windows/vs10/StepOver.reg`: Import this to your registry to step over some library code during debugging.
    Currently, this steps over any code in std::tr1::shared_ptr and std::vector.
    @warning    If you are already using the step over feature, check that the registry keys in `StepOver.reg` don't
                conflict with any already in use.
