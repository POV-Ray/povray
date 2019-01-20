To build the HTML Help file for POV-Ray for Windows, you need the following pieces of software:

  - **Perl**: ActivePerl 5.24.0 has successfully been tested; other implementations and
    versions may work just as fine.
  - **Microsoft HTML Help Workshop**: Version 4.7.4.8702.0 has successfully been tested; other
    versions may work just as fine.

Perl is presumed to be associated with the `.pl` file extension; HTML Help Workshop is presumed to be
found in the command search path.

To build the POV-Ray for Windows help files, proceed as follows:

  - Clear the `input` subfolder (or create it if it doesn't exist yet).
  - Extract the HTML documentation generated from the Wiki (Windows variant) to the `input`
    subfolder.
  - Run `makedocs.bat`.

The generated HTML Help file will be placed in `distribution/platform-specific/windows/Help/`
as `povray.chm`, ready for inclusion in the installer package.

(**Note:** Don't forget to also update the Unix HTML help files residing in
`doc/html`.)
