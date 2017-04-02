@page github    GitHub Procedures and Practices


POV-Ray's official source code repository is hosted on GitHub. Besides version
management, we're also making use of other features of that platform, most
notably GitHub issues.


Issues
======

Labels
------

This section describes the current (2017-03-25) intent of the various GitHub
issue labels we're currently using (or have available for use):

### Triage

The following labels are used to categorize unexpected behaviour:
    
  - **bug**: The behaviour has been identified as a genuine bug, i.e. neither
    intentional nor deliberately accepted, and affecting all platforms on which
    the code is intended to run.
    
  - **compatibility**: The behaviour has been identified as unintentional,
    but affecting only a subset of the relevant build or runtime environments.

  - **feature**: The reported unexpected behaviour (if any) has been
    identified as intentional or deliberately accepted in the past, but is taken
    as an incentive for possible improvement; or the original author suggested
    such an improvement straight away.

  - **minor**: The behaviour has been identified as unintentional, but deemed
    more of a small nuisance than a genuine problem.
 
  - **not a bug**: The behaviour has been identified as fully intentional.

  - **wontfix**: The behaviour has been identified as sub-optimal but
    deliberately accepted.

Alternatively, the following labels are used to flag special cases:

  - **bulk**: The issue reports multiple independent items; it should be
    split up into multiple new issues, and closed with reference to the new
    issue IDs.
    
  - **duplicate**: The issue has already been reported with an earlier
    GitHub issue, and should be closed with reference to the earlier issue ID.
    
  - **invalid**: The issue has been identified as blatant misuse of the
    GitHub issues feature.
    
  - **support**: The issue does not constitute a report of unexpected behaviour,
    but rather a request for support.

### Symptoms

The following labels are used to identify particular classes of symptoms:

  - **artefact**: The issue pertains to visible render artefacts.

  - **performance**: The issue pertains to sub-optimal performance.

### Components

The following labels are used to identify particular components affected:

  - **Windows Editor**: The issue affects the Windows editor DLLs.

  - **Windows GUI**: The issue affects the Windows GUI.

### Platforms

Labels starting with "OS:" are used to indicate that the reported issue or the
suggested course of action involve platform-specific behaviour:

  - **OS: BSD**: BSD-style Unix

  - **OS: Linux**: GNU/Linux

  - **OS: Mac OS X**: Linux-based Apple Macintosh OS

  - **OS: Windows**: Microsoft Windows

  - **OS: Unix**: Generic Unix-style platforms

Only the most generic applicable labels should be used (e.g. just _Unix_ if
all Unix-style platforms are affected alike, instead of _BSD_, _Linux_ and
_Max OS X_). If all platforms are affected alike, none of the labels should be
used.

In a similar vein, these labels should not be used if the platform is already
unambiguously identified by a component label (e.g. _Windows Editor_,
implying the Windows platform).

### Action

Labels starting with "ToDo:" are used to identify how to proceed with the issue:

  - **ToDo: discuss**: Comments are called for, to discuss how to proceed with
    the issue.

  - **ToDo: implement**: A good solution has already been identified, and just
    needs to be implemented.

  - **ToDo: refactor**: Reasonable ideas to address the issue already exist,
    but require further refactoring of the codebase.

  - **ToDo: reproduce**: The issue still needs to be reproduced in order to
    identify the root cause.

  - **ToDo: research**: Some research is needed before determining how to
    proceed with the issue.

  - **ToDo: test**: An attempt to address the issue has been made, and needs
    to be tested now.

  - **ToDo: user feedback**: Some clarification is needed from the original
    author of the issue report.

Alternatively, the following labels also imply a particular action (or absence
thereof):

  - **informational**: It has been decided to not take any action with
    respect to the issue, but leave it open as a source of information for
    other users.

### Label Colouring

The label colours have been chosen somewhat arbitrarily, except for the
following:

  - **White** (`#FFFFFF`) is reserved for platform labels.

  - **Silver** (`#CCCCCC`) is reserved for miscellaneous labels indicating that
    we're essentially ignoring the issue for one reason or another.

  - **Dim Grey** (`#666666`) is reserved for labels indicating components that
    are considered obsolete.

  - **Black** (`#000000`) is reserved for labels indicating that we're
    deliberately rejecting the issue for one reason or another.

  - **Scarlet** (`#FC2929`) is reserved for labels indicating behaviour that
    needs fixing.

  - **Crimson** (`#B60205`) is reserved for labels indicating behaviour that
    warrants improvement.

  - **Pastel** colours are used for _ToDo:_ labels.
 