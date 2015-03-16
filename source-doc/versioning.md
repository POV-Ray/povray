# Version Numbering {#versioning}

@section schm       Numbering Scheme

Official POV-Ray builds have version numbers of the form `X.Y.Z`[`-PRE.ID`], where:

  - `X` is the major version number, and is intended to indicate radical changes, such as the planned redesign of the
    scene description language (SDL).
  - `Y` is a minor version number, and is intended to indicate significant architectural or feature changes; for
    instance the addition of support for distributed rendering will certainly go along with an increase of the minor
    version number.
  - `Z` is a sub-minor version number, and is primarily intended to indicate addition of, or changes in, minor features,
    so that they can be tested for from the SDL.

  - `PRE` is a pre-release identifier, such as `alpha`, `beta` or `rc`, indicating that it is a forerunner to, but may
    still differ in functionality from, what will ultimately become the official version `X.Y.Z`.
  - `ID` is a numeric value uniquely distinguishing the pre-release in question from other pre-releases.

Unofficial builds should have version numbers of the form `X.Y.Z-`[`PRE.ID.`]`unofficial`.


@section chng       What To Change

When updating a version number, the following files need to be changed accordingly:

  - `unix/VERSION`
  - `source/backend/povray.h`


@section auto       Automatic Version Numbering

The directory `tools/git/hooks` contains scripts intended to help with version numbering when using Git; to benefit from
them, copy them to the `.git/hooks` directory in your local Git workspace. Whenever you perform a commit, they will
automatically care of the following:

  - Verify that `unix/VERSION` and the various version number variants in `source/backend/povray.h` match.
  - Update the `ID` portion of pre-release versions whenever any files in one of the following directory sub-trees have
    been staged for commit: `source`, `vfe`, `mac`, `unix`, `windows` or `libraries`.

