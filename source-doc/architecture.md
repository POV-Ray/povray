# Architecture {#architecture}


@section modules    Modules

The original architectural design of POV-Ray 3.7.0 identified POV-Ray as being comprised of two main entities, called
the _back-end_ and _front-end_. In a nutshell, the front-end would identify what scene file to render, start up one or
more back-end instances, make sure those have access to the input files, and let them parse the scene and subsequently
compute raw output image data from it. The front-end would then wait for the image data to trickle in, and ultimately
write the collected data to an actual image file. The communication between the front- and back-end would use a
proprietary asynchronous message-passing protocol called _POVMS_.

Each of these two entities resided in a separate sub-tree within the source tree, named @ref backend/ and
@ref frontend/. A third sub-tree, @ref base/, contained code shared between back- and front-end.

@remark
    The ability to run and manage multiple back-end instances is a prerequisite for distributed rendering in a networked
    environment; however, to this date (2015-09-07), no official version of POV-Ray exists that implements this ability.
    As far as multi-core rendering is concerned, current implementations exclusively rely on multithreading support
    built into the back-end itself.

Current platform-specific implementations of POV-Ray also make use of another entity called the _virtual front-end_, or
_VFE_, the code of which resides outside the main source tree despite being shared across platforms. The purpose of this
entity is to abstract away various details of the _POVMS_ interface exposed by the official _front-end_.

Since the official release of POV-Ray 3.7.0, ongoing work has been put into further refining the architectural design,
currently distinguishing the following _modules_, each of which again resides in its own separate source sub-tree:


@subsection backend     Back-End

The responsibility of the _back-end_ module, residing in the @ref backend/ source sub-tree, is being trimmed down to
coordinating multi-threaded execution of the _core_ and _parser_ modules, as well as interfacing them to a -- possibly
remote -- _front-end_ instance via the _POVMS_ protocol.


@subsection base        Base

The _base_ module, still residing in the @ref base/ source sub-tree, continues to be a loose collection of miscellaneous
utility code that happens to be shared across modules that should not interface directly, such as the _core_ and
_front-end_.

The base module is intended to be mostly unaware of multithreading, except that it makes provisions for sharing a few of
its data structures -- mostly related to performance optimizations -- among multiple threads.


@subsection core        Core

The _core_ module, residing in the @ref core/ source sub-tree, is designated to contain the actual render engine code,
the job of which is to take an internal representation of the scene and camera, and trace individual rays for given
pixel coordinates.

The core module is intended to be mostly unaware of multithreading, except that it makes provisions for sharing a few of
its data structures -- mostly related to performance optimizations -- among multiple core threads.


@subsection frontend    Front-End

The _front-end_ module, residing in the @ref frontend/ source sub-tree, currently retains its dual role of providing
high-level control of the rendering process as well as assembling the computed image data into an actual result image
file.


@subsection parser      Parser

The _parser_, residing in the @ref parser/ source sub-tree, is responsible for parsing a scene file, reading in any
auxiliary files such as include files and input images, and generating an internal representation of the scene and
camera in the format required by the _core_ module.

The parser module is entirely unaware of multithreading: It does not seem to make sense to run multiple parser instances
to process the same file, nor to share data between multiple parser instances processing different files.


@subsection povms       POVMS

The _POVMS_ module, residing in the @ref povms/ source sub-tree, implements the message-passing interface used between
the _front-end_ and _back-end_ modules.


@subsection vfe         Virtual Front-End

The _virtual front-end_ module, or _VFE_, which resides outside the main source tree, contains code shared between most
platform-specific versions of POV-Ray to abstract away various details of the _POVMS_ interface exposed by the official
_front-end_.


@section units      Units

The POV-Ray code is comprised of hundreds of code _units_ - that is, individual `.cpp` files and their respective
associated header files. Frequently, one unit corresponds to one C++ class, but this is not always the case. See the
file list for details on each individual file.
