#!/bin/sh

###############################################################################
# prebuild.sh script (maintainers only) 
# Written by Nicolas Calimet and Christoph Hormann
#
# This prebuild.sh script prepares the source tree for building the
# Unix/Linux version of POV-Ray.  Unlike the former versions, the
# prebuild procedure does not change the directory structure, so that
# the overall layout of the UNIX source distribution remains consistent
# with the other supported architectures (namely Windows and Macintosh).
# Yet, some "standard" files such as configure, README, INSTALL, etc.
# are placed in the root directory to give the expected GNUish look to
# the distribution.
#
# The purpose of this script is to create the 'configure' and various
# 'Makefile.am' shell scripts, as well as modify/update some generic files
# (e.g. doc).
#
# Running prebuild.sh requires:
#   1) GNU autoconf >= 2.68 and GNU automake >= 1.9
#   2) perl and m4 (should be on any system, at least Linux is okay)
#   3) Run from the unix/ directory where the script is located.
#
# Prepare all but the doc/ directory using:
#   % ./prebuild.sh
#
# Clean up all files and folders created by this script (but docs):
#   % ./prebuild.sh clean
#
# The unix-specific documentation is created seperately since it requires
# lots of processing time as well as a bunch of other programs such as PHP.
# See ../documentation/povdocgen.doc for details.
# The "all" option builds all docs, otherwise only html docs are created.
# Any other option (e.g. "skip ta") replaces the default.
#   % ./prebuild.sh doc(s) [option]
#
# Clean up the docs:
#   % ./prebuild.sh doc(s)clean
#
# Note that the 'clean' and 'doc(s)(clean)' options are mutually exclusive.
#
###############################################################################

# Change to the directory this script resides in
scriptname=`basename $0`
olddir=`pwd`
cd `dirname $0`

umask 022

# Extract version information from `source/base/version.h`
eval `../tools/unix/get-source-version.sh ../source/base/version.h`
pov_copyright="$POV_RAY_COPYRIGHT"
pov_version_base="$POV_RAY_GENERATION"
pov_version="$POV_RAY_FULL_VERSION"

pov_config_bugreport="POV-Ray issue tracker at https://github.com/POV-Ray/povray/issues"

# documentation
timestamp=`date +%Y-%m-%d`
build="./docs_$timestamp"
builddoc="$build/documentation"

required_autoconf="2.68"
required_automake="1.9"


###############################################################################
# Setup
###############################################################################

# Check optional argument.
case "$1" in
  ""|clean|doc|docs|docclean|docsclean) ;;
  *) echo "$scriptname: error: unrecognized option '$1'"; cd "$olddir" ; exit ;;
esac

# Check whether 'cp -u' is supported.
if test x"`cp -u ./prebuild.sh /dev/null 2>&1`" = x""; then
  cp_u='cp -u'
else
  cp_u='cp'
fi

# Check for autoconf/automake presence and version.
if test x"$1" = x""; then
  if autoconf --version > /dev/null 2>&1; then
    autoconf=`autoconf --version | grep autoconf | sed s,[^0-9.]*,,g`
    echo "Detected autoconf $autoconf"
    autoconf=`echo $autoconf | sed -e 's,\([0-9]*\),Z\1Z,g' -e 's,Z\([0-9]\)Z,Z0\1Z,g' -e 's,[^0-9],,g'`
    required=`echo $required_autoconf | sed -e 's,\([0-9]*\),Z\1Z,g' -e 's,Z\([0-9]\)Z,Z0\1Z,g' -e 's,[^0-9],,g'`
    expr $autoconf \>= $required > /dev/null || autoconf=""
  fi
  if test x"$autoconf" = x""; then
    echo "$scriptname: error: requires autoconf $required_autoconf or above"
    cd "$olddir"
    exit 1
  fi

  if automake --version > /dev/null 2>&1; then
    automake=`automake --version | grep automake | sed s,[^0-9.]*,,g`
    echo "Detected automake $automake"
    automake=`echo $automake | sed -e 's,\([0-9]*\),Z\1Z,g' -e 's,Z\([0-9]\)Z,Z0\1Z,g' -e 's,[^0-9],,g'`
    required=`echo $required_automake | sed -e 's,\([0-9]*\),Z\1Z,g' -e 's,Z\([0-9]\)Z,Z0\1Z,g' -e 's,[^0-9],,g'`
    expr $automake \>= $required > /dev/null || automake=""
  fi
  if test x"$automake" = x""; then
    echo "$scriptname: error: requires automake $required_automake or above"
    cd "$olddir"
    exit 1
  fi
fi


###############################################################################
# Copying and generating standard/additional files
###############################################################################

case "$1" in

  # Cleanup all files not in the repository
  clean)
  if test -f ../Makefile; then
    makeclean=`\
cd .. ; \
echo "make clean" 1>&2  &&  make clean 1>&2 ; \
echo "make maintainer-clean" 1>&2  &&  make maintainer-clean 1>&2 ; \
` 2>&1
  fi

  # backward-compatible cleanup
  for file in \
    acinclude.m4 acx_pthread.m4 AUTHORS ChangeLog config/ configure.ac \
    COPYING INSTALL NEWS README icons/ include/ ini/ povray.1 povray.conf \
    povray.ini.in scenes/ scripts/ VERSION
  do
    rm -r ../$file 2> /dev/null  &&  echo "Cleanup ../$file"
  done
  # cleanup stuff added by automake
  for file in config.guess config.sub compile depcomp install-sh missing
  do
    rm config/$file 2> /dev/null  &&  echo "Cleanup config/$file"
  done
  ;;


  # Cleanup documentation
  doc*clean)
  for file in ../doc/ $build; do
    rm -r $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;


  # Generate the documentation (adapted from C.H. custom scripts)
  doc|docs)
  echo "Generate docs"
  log_file="makedocs.log"
  cat /dev/null > $log_file

  # cleanup or create the ../doc folder.
  if ! test -d ../doc; then
    echo "Create ../doc" | tee -a $log_file
    mkdir ../doc
    echo "Create ../doc/html" | tee -a $log_file
    mkdir ../doc/html
  else
    echo "Cleanup ../doc" | tee -a $log_file
    rm -f -r ../doc/*
    mkdir ../doc/html
  fi

  # create build folder and documentation.
  if ! test -d $build; then
    echo "Create $build" | tee -a $log_file
    mkdir $build
    echo "Create $build/distribution" | tee -a $log_file
    echo "Copy distribution" | tee -a $log_file
    $cp_u -f -R ../distribution $build/
  fi
  if ! test -d $builddoc; then
    echo "Create $builddoc" | tee -a $log_file
    mkdir $builddoc
  fi
  if test x"../documentation" != x"$builddoc"; then
    echo "Copy documentation" | tee -a $log_file
    $cp_u -f -R ../documentation/* $builddoc/
    chmod -f -R u+rw $builddoc/
  fi
  chmod -R u+rw $build/*

  # run makedocs script.
  # The default "skip ta" does not build latex nor archive files.
  # Yet some GIF images from the output/final/tex/images directories are needed;
  # for simplicity this directory is a symlink to output/final/machelp/images
  # (do not symlink with output/final/unixhelp/images).
  echo "Run makedocs" | tee -a $log_file
  rootdir=`pwd`
  cd $builddoc
  docopts="skip ta"
  case "$2" in
    all) docopts=;;
    .*)  docopts="$2";;
  esac
  test -d ./output/final/tex/  ||  mkdir -p ./output/final/tex/
  skipt=`echo "$docopts" | grep 'skip.*t'`
  test x"$skipt" != x""  &&  test -d ./output/final/tex/images  ||  ln -s ../machelp/images ./output/final/tex/images
  sh makedocs.script $docopts | tee -a $rootdir/$log_file
  test x"$skipt" != x""  &&  test -d ./output/final/tex/images  &&  rm -f ./output/final/tex/images
  cd $rootdir

  # post-process HTML files in several steps.
  echo "Process unixhelp HTML files" | tee -a $log_file
  files=`find $builddoc/output/final/unixhelp/ -name "*.html"`

  # add document type
  # replace &trade; characters
  # remove (often misplaced) optional </p> tags
  # remove empty strong's
  # reorganise section link and add 'id' attribute
  for htmlfile in $files ; do
    mv -f $htmlfile $htmlfile.temp
    echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">' > $htmlfile
    cat $htmlfile.temp | sed \
      -e 's,&trade;,<sup>TM</sup>,g' \
      -e 's,&amp;trade;,<sup>TM</sup>,g' \
      -e 's,</p>,,g' \
      -e 's,<strong>[[:space:]]*</strong>,,g' \
      -e 's,<a name="\([^"]*\)">\([0-9.]* \)</a>,<a id="\1" name="\1"></a>\2,g' \
      >> $htmlfile
  done

  # add link targets for index keywords
  idx="$builddoc/output/final/unixhelp/idx.html"
  mv -f $idx $idx.temp
  cat $idx.temp | sed \
    '/<!-- keyword -->/ N; s,<!-- keyword -->\(<dt>\)\n\s*\(.*\),\1<a id="\2" name="\2"></a>\2,' \
    > $idx

  # replace invalid caracters in 'id' and 'name' attributes; needs seperate steps.
  for htmlfile in $files ; do  # comma+space -> dot
    mv -f $htmlfile $htmlfile.temp
    cat $htmlfile.temp | sed \
      ':BEGIN; s/id="\([^,"]*\), \([^"]*\)" name="\([^,"]*\), \([^"]*\)"/id="\1.\2" name="\3.\4"/g; tBEGIN' \
      > $htmlfile
  done
  for htmlfile in $files ; do  # slash -> dot
    mv -f $htmlfile $htmlfile.temp
    cat $htmlfile.temp | sed \
      ':BEGIN; s/id="\([^\/"]*\)\/\([^"]*\)" name="\([^\/"]*\)\/\([^"]*\)"/id="\1.\2" name="\3.\4"/g; tBEGIN' \
      > $htmlfile
  done
  for htmlfile in $files ; do  # spaces -> dots
    mv -f $htmlfile $htmlfile.temp
    cat $htmlfile.temp | sed \
      ':BEGIN; s/id="\([^ "]*\) \([^"]*\)" name="\([^ "]*\) \([^"]*\)"/id="\1.\2" name="\3.\4"/g; tBEGIN' \
      > $htmlfile
  done
  for htmlfile in $files ; do  # hash character -> H
    mv -f $htmlfile $htmlfile.temp
    cat $htmlfile.temp | sed \
      ':BEGIN; s/id="\([^#"]*\)#\([^"]*\)" name="\([^#"]*\)#\([^"]*\)"/id="\1H\2" name="\3H\4"/g; tBEGIN' \
      > $htmlfile
  done
  for htmlfile in $files ; do  # first plus sign -> P
    mv -f $htmlfile $htmlfile.temp
    cat $htmlfile.temp | sed \
      ':BEGIN; s/id="\([^+"]*\)+\([^"]*\)" name="\([^+"]*\)+\([^"]*\)"/id="\1P\2" name="\3P\4"/; tBEGIN' \
      > $htmlfile
  done
  for htmlfile in $files ; do  # first minus sign -> M
    mv -f $htmlfile $htmlfile.temp
    cat $htmlfile.temp | sed \
      ':BEGIN; s/id="\([^"-]*\)-\([^"]*\)" name="\([^"-]*\)-\([^"]*\)"/id="\1M\2" name="\3M\4"/; tBEGIN' \
      > $htmlfile
  done

  # add keyword list on top of the index, using alphabetical folded sublists
  idx="$builddoc/output/final/unixhelp/idx.html"
  rm -f $idx.list
  echo "<br><div style=\"text-align:left;\">" > $idx.list
  for car in \# + - A B C D E F G H I J K L M N O P Q R S T U V W X Y Z; do
    firsttarget=`grep -i "</a>$car" $idx | head -n 1 | sed "s,\s*<dt><a id=\"\(.*\)\" name=.*></a>\(.*\),<a href=\"#\1\"><strong>$car</strong></a>,"`
    echo "&nbsp;&nbsp;&nbsp;$firsttarget&nbsp;<span class=\"menuEntry\" onclick=\"MenuToggle(event);\">&raquo;</span><div class=\"menuSection\" style=\"display:none;\"><blockquote class=\"Note\">" >> $idx.list
    grep -i "</a>$car" $idx | sed \
      's,\s*<dt><a id="\(.*\)" name=.*></a>\(.*\),<a href="#\1">\2</a>\&nbsp;\&nbsp;,' \
      >> $idx.list
    echo "</blockquote></div>" >> $idx.list
  done
  echo "</div><hr>" >> $idx.list
  mv -f $idx $idx.temp
  cat $idx.temp | sed "/<!-- list -->/ r $idx.list" > $idx
  rm -f $idx.list

  # improve navigation from the keyword index by placing all link targets
  # directly in the heading section on the previous line
  for htmlfile in $files ; do
    mv -f $htmlfile $htmlfile.temp 
    cat $htmlfile.temp | sed \
      '/<h[1-5]><a/ { N; s,\(<h[1-5]><a.*</a>\)\(.*\)\(</h[1-5]>\)\n\(<a id=.* name=.*></a>\)*,<!-- \2 -->\n\1\4\2\3,g }' \
      > $htmlfile
  done

  # unfold the chapter entries in menu.html.
  menu="$builddoc/output/final/unixhelp/menu.html"
  mv -f $menu $menu.temp
  cat $menu.temp | sed \
    -e 's,^  \(<span.*>\)\(<ul.*>\),  <div>\2,' \
    -e 's,^    \(<span.*\)raquo\(.*display:\)none\(.*\),    \1laquo\2block\3,' \
    > $menu

  # finally make a clean copy of the documentation.
  echo "Cleanup" | tee -a $log_file
  rm -f -r $builddoc/output/final/unixhelp/*.html.temp
  rm -f    $builddoc/output/final/unixhelp/template*
  rm -f    $builddoc/output/final/unixhelp/README
  rm -f    $builddoc/output/final/unixhelp/images/README
  rm -f    $builddoc/output/final/unixhelp/images/reference/README
  rm -f    $builddoc/output/final/unixhelp/images/reference/colors/README
  rm -f    $builddoc/output/final/unixhelp/images/tutorial/README
  rm -f -r $builddoc/output/final/unixhelp/images/unix/
  rm -f    $builddoc/output/final/unixhelp/unix_splash.html
  rm -f    $builddoc/output/final/unixhelp/unix_frame.html

  echo "Copy documentation in ../doc/" | tee -a $log_file
  $cp_u -f -R   $builddoc/output/final/unixhelp/* ../doc/html/
  chmod -R u+rw ../doc/html/
  mv -f         ../doc/html/*.txt ../doc/
  mv -f         ../doc/html/*.doc ../doc/
  $cp_u -f      README.unix ../doc/
  chmod -R u+rw ../doc/

  # [C.H.]
  # povlegal.doc contains 0x99, which is a TM character under Windows.
  # Converting to "[tm]".
  echo "Copy licence files in ../doc/"
  perl -e 'while (<>) {s/\x99/[tm]/g; print;}' ../distribution/povlegal.doc \
    > ../doc/povlegal.doc || echo "povlegal.doc not created !"
  $cp_u -f ../distribution/agpl-3.0.txt ../doc/ \
    || echo "agpl-3.0.txt not copied !"

  # log tracing.
  $cp_u -f   $log_file docs_$timestamp.log
  chmod u+rw docs_$timestamp.log
  $cp_u -f   $builddoc/output/log.txt docs_internal_$timestamp.log
  chmod u+rw docs_internal_$timestamp.log
  rm -f      $log_file

  cd "$olddir"
  exit
  ;;


  # Copy files
  *)
  # some shells seem unable to expand properly wildcards in the list entries
  # (e.g. ../distribution/in*/).
  for file in \
    AUTHORS ChangeLog configure.ac COPYING NEWS README \
    povray.1 povray.conf \
    scripts \
    ../distribution/ini ../distribution/include ../distribution/scenes
  do
    out=`basename $file`
    echo "Create ../$out`test -d $file && echo /`"
    $cp_u -f -R $file ../  ||  echo "$file not copied !"
    chmod -f -R u+rw ../$out
  done

  # special cases:

  # INSTALL
  echo "Create ../INSTALL"
  $cp_u -f install.txt ../INSTALL  ||  echo "INSTALL not copied !"
  chmod -f u+rw ../INSTALL

  # VERSION
  echo "Create ../VERSION"
  echo "$pov_version" > ../VERSION  ||  echo "VERSION not created !"

  # icons/
  # don't copy the icons/source directory
  mkdir -p ../icons
  files=`find icons -maxdepth 1 -name \*.png`
  for file in $files ; do
    echo "Create ../$file"
    $cp_u -f $file ../$file  ||  echo "$file not copied !"
    chmod -f -R u+rw ../$file
  done

  echo "Create ../doc/html"
  mkdir -p ../doc/html  # required to build without existing docs
  ;;

esac


###############################################################################
# Creation of supporting unix-specific files
###############################################################################


###
### ./Makefile.am
###

makefile="./Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  echo "Create $makefile.am"
  cat Makefile.header > $makefile.am
  cat << pbEOF >> $makefile.am

# Makefile.am for the source distribution of POV-Ray $pov_version_base for UNIX
# Please report bugs to $pov_config_bugreport

# Programs to build.
bin_PROGRAMS = povray

# Source files.
povray_SOURCES = \\
  disp.h \\
  disp_sdl.cpp disp_sdl.h \\
  disp_sdl2.cpp disp_sdl2.h \\
  disp_x11.cpp disp_x11.h \\
  disp_text.cpp disp_text.h

cppflags_platformcpu =
ldadd_platformcpu =
if BUILD_x86
cppflags_platformcpu += -I\$(top_srcdir)/platform/x86
ldadd_platformcpu += \$(top_builddir)/platform/libx86.a
endif
if BUILD_x86avx
ldadd_platformcpu += \$(top_builddir)/platform/libx86avx.a
endif
if BUILD_x86avxfma4
ldadd_platformcpu += \$(top_builddir)/platform/libx86avxfma4.a
endif
if BUILD_x86avx2fma3
ldadd_platformcpu += \$(top_builddir)/platform/libx86avx2fma3.a
endif

# Include paths for headers.
AM_CPPFLAGS = \\
  -I\$(top_srcdir)/unix/povconfig \\
  -I\$(top_srcdir) \\
  -I\$(top_srcdir)/source \\
  -I\$(top_builddir)/source \\
  -I\$(top_srcdir)/platform/unix \\
  \$(cppflags_platformcpu) \\
  -I\$(top_srcdir)/vfe \\
  -I\$(top_srcdir)/vfe/unix

# Libraries to link with.
# Beware: order does matter!
# TODO - Having vfe/libvfe.a twice in this list is a bit of a hackish way to cope with cyclic dependencies.
LDADD = \\
  \$(top_builddir)/vfe/libvfe.a \\
  \$(top_builddir)/source/libpovray.a \\
  \$(top_builddir)/vfe/libvfe.a \\
  \$(top_builddir)/platform/libplatform.a \\
  \$(ldadd_platformcpu)
pbEOF
  ;;
esac



##### Root directory ##########################################################


###
### ../kde_install.sh
###

file="../kde_install.sh"

case "$1" in
  clean)
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  ;;

  doc*)
  ;;

  *)
    echo "Create $file"
    echo "#!/bin/sh
# ==================================================================
# POV-Ray $pov_version_base - Unix source version - KDE install script
# ==================================================================
# written July 2003 - March 2004 by Christoph Hormann
# Based on parts of the Linux binary version install script
# This file is part of POV-Ray and subject to the POV-Ray licence
# see POVLEGAL.DOC for details.
# ==================================================================

" > "$file"

    grep -A 1000 -E "^#.*@@KDE_BEGIN@@" "./install" | grep -B 1000 -E "^#.*@@KDE_END@@" >> "$file"

    echo "

kde_install
"  >> "$file"

  chmod +x $file
  ;;
esac


###
### ../povray.ini.in (template for ../povray.ini)
###

ini="../povray.ini"

case "$1" in
  clean)
  for file in $ini $ini.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  # __POVLIBDIR__ will be updated at make time.
  echo "Create $ini.in"
  cat ../distribution/ini/povray.ini | sed \
    's/C:.POVRAY3 drive and/__POVLIBDIR__/' > $ini.in
  cat << pbEOF >> $ini.in

;; Search path for #include source files or command line ini files not
;; found in the current directory.  New directories are added to the
;; search path, up to a maximum of 25.

Library_Path="__POVLIBDIR__"
Library_Path="__POVLIBDIR__/ini"
Library_Path="__POVLIBDIR__/include"

;; File output type control.
;;     T    Uncompressed Targa-24
;;     C    Compressed Targa-24
;;     P    UNIX PPM
;;     N    PNG (8-bits per colour RGB)
;;     Nc   PNG ('c' bit per colour RGB where 5 <= c <= 16)

Output_to_File=true
Output_File_Type=N8             ;; (+/-Ftype)
pbEOF
  ;;
esac


###
### ../Makefile.am
###

makefile="../Makefile"

case "$1" in
  clean)
  for file in $makefile.am; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  scriptfiles=`find scripts -type f`

  echo "Create $makefile.am"
  cat Makefile.header > $makefile.am
  cat << pbEOF >> $makefile.am

# Makefile.am for the source distribution of POV-Ray $pov_version_base for UNIX
# Please report bugs to $pov_config_bugreport

# Directories.
povlibdir = @datadir@/@PACKAGE@-@VERSION_BASE@
povdocdir = @datadir@/doc/@PACKAGE@-@VERSION_BASE@
povconfdir = @sysconfdir@/@PACKAGE@/@VERSION_BASE@
povuser = \$(HOME)/.@PACKAGE@
povconfuser = \$(povuser)/@VERSION_BASE@
povinstall = \$(top_builddir)/install.log
povowner = @povowner@
povgroup = @povgroup@

# Directories to build.
SUBDIRS = source vfe platform unix

# Additional files to distribute.
EXTRA_DIST = \\
  bootstrap kde_install.sh \\
  doc icons include ini scenes scripts \\
  povray.ini.in changes.txt revision.txt

# Additional files to clean with 'make distclean'.
DISTCLEANFILES = \$(top_builddir)/povray.ini
CONFIG_CLEAN_FILES = 

# Render a test scene for 'make check'.
# This is meant to run before 'make install'.
check: all
	\$(top_builddir)/unix/povray +i\$(top_srcdir)/scenes/advanced/biscuit.pov -f +d +p +v +w320 +h240 +a0.3 +L\$(top_srcdir)/include

# Install scripts in povlibdir.
nobase_povlib_SCRIPTS = `echo $scriptfiles`

# Install documentation in povdocdir.
povdoc_DATA = AUTHORS ChangeLog NEWS

# Install configuration and INI files in povconfdir.
dist_povconf_DATA = povray.conf
povray.conf:

povconf_DATA = povray.ini
povray.ini:
	cat \$(top_srcdir)/povray.ini.in | sed "s,__POVLIBDIR__,\$(povlibdir),g" > \$(top_builddir)/povray.ini

# Install man page.
dist_man_MANS = povray.1

# Remove all unwanted files for 'make dist(check)'.
# Make all files user read-writeable.
dist-hook:
	chmod -R u+rw \$(distdir)
	chmod 755 \$(distdir)/scripts/*
	rm -f    \`find \$(distdir) -name "*.h.in~"\`
	rm -f -r \`find \$(distdir) -name autom4te.cache\`
	rm -f -r \`find \$(distdir) -name .libs\`

# Manage various data files for 'make install'.
# Creates an install.log file to record created folders and files.
# Folder paths are prepended (using POSIX printf) to ease later removal in 'make uninstall'.
# Don't be too verbose so as to easily spot possible problems.
install-data-local:
	cat /dev/null > \$(povinstall);
	@echo "Creating data directories..."; \\
	list='\$(top_srcdir)/icons \$(top_srcdir)/include \$(top_srcdir)/ini \$(top_srcdir)/scenes'; \\
	dirlist=\`find \$\$list -type d | sed s,\$(top_srcdir)/,,\`; \\
	for p in "" \$\$dirlist ; do \\
	  \$(mkdir_p) \$(DESTDIR)\$(povlibdir)/\$\$p && printf "%s\\n" "\$(DESTDIR)\$(povlibdir)/\$\$p" "\`cat \$(povinstall)\`" > \$(povinstall); \\
	done; \\
	echo "Copying data files..."; \\
	filelist=\`find \$\$list -type f | sed s,\$(top_srcdir)/,,\`; \\
	for f in \$\$filelist ; do \\
	  \$(INSTALL_DATA) \$(top_srcdir)/\$\$f \$(DESTDIR)\$(povlibdir)/\$\$f && echo "\$(DESTDIR)\$(povlibdir)/\$\$f" >> \$(povinstall); \\
	done
	@echo "Creating documentation directories..."; \\
	dirlist=\`find \$(top_srcdir)/doc/ -type d | sed s,\$(top_srcdir)/doc/,,\`; \\
	for p in "" \$\$dirlist ; do \\
	  \$(mkdir_p) \$(DESTDIR)\$(povdocdir)/\$\$p && printf "%s\\n" "\$(DESTDIR)\$(povdocdir)/\$\$p" "\`cat \$(povinstall)\`" > \$(povinstall); \\
	done
	@echo "Copying documentation files..."; \\
	filelist=\`find \$(top_srcdir)/doc/ -type f | sed s,\$(top_srcdir)/doc/,,\`; \\
	for f in \$\$filelist ; do \\
	  \$(INSTALL_DATA) \$(top_srcdir)/doc/\$\$f \$(DESTDIR)\$(povdocdir)/\$\$f && echo "\$(DESTDIR)\$(povdocdir)/\$\$f" >> \$(povinstall); \\
	done
	@echo "Creating user directories..."; \\
	for p in \$(povuser) \$(povconfuser) ; do \\
	  \$(mkdir_p) \$\$p && chown \$(povowner) \$\$p && chgrp \$(povgroup) \$\$p && printf "%s\\n" "\$\$p" "\`cat \$(povinstall)\`" > \$(povinstall); \\
	done
	@echo "Copying user configuration and INI files..."; \\
	for f in povray.conf povray.ini ; do \\
	  if test -f \$(povconfuser)/\$\$f; then \\
	    echo "Creating backup of \$(povconfuser)/\$\$f"; \\
	    mv -f \$(povconfuser)/\$\$f \$(povconfuser)/\$\$f.bak; \\
	  fi; \\
	done; \\
	\$(INSTALL_DATA) \$(top_srcdir)/povray.conf \$(povconfuser)/povray.conf && chown \$(povowner) \$(povconfuser)/povray.conf && chgrp \$(povgroup) \$(povconfuser)/povray.conf  && echo "\$(povconfuser)/povray.conf" >> \$(povinstall); \\
	\$(INSTALL_DATA) \$(top_builddir)/povray.ini \$(povconfuser)/povray.ini && chown \$(povowner) \$(povconfuser)/povray.ini && chgrp \$(povgroup) \$(povconfuser)/povray.ini  && echo "\$(povconfuser)/povray.ini" >> \$(povinstall)

# Remove data, config, and empty folders for 'make uninstall'.
# Use 'hook' instead of 'local' so as to properly remove *empty* folders (e.g. scripts).
# The last echo prevents getting error from failed rmdir command.
uninstall-hook:
	@if test -f \$(top_builddir)/install.log ; then \\
	  rmdir \$(DESTDIR)\$(povlibdir)/scripts; \\
	  echo "Using install info from \$(top_builddir)/install.log"; \\
	  echo "Removing data, documentation, and configuration files..."; \\
	  for f in \`cat \$(top_builddir)/install.log\` ; do \\
	    test -f \$\$f && rm -f \$\$f ; \\
	  done; \\
	  echo "Removing empty directories..."; \\
	  for f in \`cat \$(top_builddir)/install.log\` ; do \\
	    test -d \$\$f && rmdir \$\$f ; \\
	  done; \\
	  echo "Removing \$(top_builddir)/install.log" && rm -f \$(top_builddir)/install.log ; \\
	else \\
	  "Removing all data unconditionally"; \\
	  rm -f -r \$(DESTDIR)\$(povlibdir); \\
	  rm -f -r \$(DESTDIR)\$(povdocdir); \\
	  rm -f    \$(DESTDIR)\$(povconfdir)/povray.ini; \\
	  rmdir    \$(DESTDIR)\$(povconfdir); \\
	  rmdir    \$(DESTDIR)\$(sysconfdir)/@PACKAGE@; \\
	  rm -f    \$(povconfuser)/povray.conf; \\
	  rm -f    \$(povconfuser)/povray.ini; \\
	  rmdir    \$(povconfuser); \\
	  rmdir    \$(povuser); \\
	fi; \\
	echo "Uninstall finished"
pbEOF
  ;;
esac


###
### ../bootstrap
###

bootstrap="../bootstrap"

case "$1" in
  clean)
  for file in $bootstrap; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  echo "Create $bootstrap"
  cat << pbEOF > $bootstrap
#!/bin/sh -x

# bootstrap for the source distribution of POV-Ray $pov_version_base for UNIX
# Please report bugs to $pov_config_bugreport
# Run this script if configure.ac or any Makefile.am has changed

rm -f config.log config.status

# Create aclocal.m4 for extra automake and autoconf macros
aclocal -I .

# Create config.h.in
autoheader --warnings=all

# Create all Makefile.in's from Makefile.am's
automake --add-missing --warnings=all

# Create configure from configure.ac
autoconf --warnings=all

# Small post-fixes to 'configure'
#   add --srcdir when using --help=recursive
#   protect \$ac_(pop)dir with double quotes in cd commands
#   protect \$am_aux_dir with double quotes when looking for 'missing'
cat ./configure | sed \\
  -e 's,configure.gnu  --help=recursive,& --srcdir=\$ac_srcdir,g' \\
  -e 's,\(cd \)\(\$ac_\)\(pop\)*\(dir\),\1"\2\3\4",g' \\
  -e 's,\$am_aux_dir/missing,\\\\"\$am_aux_dir\\\\"/missing,g' \\
  > ./configure.tmp
mv -f ./configure.tmp ./configure
chmod +x ./configure

# Remove cache directory
rm -f -r ./autom4te.cache
pbEOF

  chmod 755 $bootstrap
  ;;
esac



##### Source directory ########################################################


###
### ../source/Makefile.am
###

dir="../source"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  files=`find $dir -name "*.cpp" -or -name "*.h" | sed s,"$dir/",,g | sort`

  echo "Create $makefile.am"
  cat Makefile.header > $makefile.am
  cat << pbEOF >> $makefile.am

# Makefile.am for the source distribution of POV-Ray $pov_version_base for UNIX
# Please report bugs to $pov_config_bugreport

# Libraries to build.
noinst_LIBRARIES = libpovray.a

# Source files.
libpovray_a_SOURCES = \\
  `echo $files`

cppflags_platformcpu = 
if BUILD_x86
cppflags_platformcpu += -I\$(top_srcdir)/platform/x86
endif

# Include paths for headers.
AM_CPPFLAGS = \\
  -I\$(top_srcdir)/unix/povconfig \\
  -I\$(top_srcdir) \\
  -I\$(top_srcdir)/platform/unix \\
  \$(cppflags_platformcpu) \\
  -I\$(top_srcdir)/unix \\
  -I\$(top_srcdir)/vfe \\
  -I\$(top_srcdir)/vfe/unix
pbEOF
  ;;
esac


###
### ../source/base/Makefile.am
###

dir="../source/base"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../source/frontend/Makefile.am
###

dir="../source/frontend"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac

###
### ../source/backend/Makefile.am
###

dir="../source/backend"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac



##### Supporting libraries ####################################################

###
### ../libraries/Makefile.am
###

makefile="../libraries/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac



##### Supporting libraries: PNG ###############################################

###
### ../libraries/png/configure.ac and configure.gnu
###

configure="../libraries/png/configure"

case "$1" in
  clean)
  for file in $configure.ac $configure.gnu; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  if test -f $configure.orig; then
    echo "Restore $configure"
    mv -f $configure.orig $configure
  fi
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/png/Makefile.am
###

dir="../libraries/png"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/png/bootstrap
###

bootstrap="../libraries/png/bootstrap"

case "$1" in
  clean)
  for file in $bootstrap; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac




##### Supporting libraries: ZLIB ##############################################

###
### ../libraries/zlib/configure.ac and configure.gnu
###

configure="../libraries/zlib/configure"
makefile="../libraries/zlib/Makefile"

case "$1" in
  clean)
  for file in $configure.ac $configure.gnu; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  if test -f $configure.orig; then
    echo "Restore $configure"
    mv -f $configure.orig $configure
  fi
  if test -f $makefile.orig; then
    echo "Restore $makefile"
    mv -f $makefile.orig $makefile
  fi
  if test -f $makefile.in.orig; then
    echo "Restore $makefile.in"
    mv -f $makefile.in.orig $makefile.in
  fi
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/zlib/Makefile.am
###

makefile="../libraries/zlib/Makefile"

case "$1" in
  clean)
  for file in $makefile.am; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  if test -f $makefile.in.orig; then
    echo "Restore $makefile.in"
    mv -f $makefile.in.orig $makefile.in
  fi
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/zlib/bootstrap
###

bootstrap="../libraries/zlib/bootstrap"

case "$1" in
  clean)
  for file in $bootstrap; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/zlib/ config scripts
###

dir="../libraries/zlib"

# no longer distributed
rm -f $dir/mkinstalldirs

case "$1" in
  clean)
  for file in config.guess config.sub install-sh missing ; do
    rm $dir/$file 2> /dev/null  &&  echo "Cleanup $dir/$file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac




##### Supporting libraries: JPEG ##############################################

###
### ../libraries/jpeg/configure.gnu
###

configure="../libraries/jpeg/configure"

case "$1" in
  clean)
  for file in $configure.gnu; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac




##### Supporting libraries: TIFF ##############################################

###
### ../libraries/tiff/libtiff/configure.ac and configure.gnu
###

configure="../libraries/tiff/libtiff/configure"

# remove old configure.gnu
rm -f ../libraries/tiff/configure.gnu

case "$1" in
  clean)
  for file in $configure.ac $configure.gnu; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/tiff/libtiff/Makefile.am
###

makefile="../libraries/tiff/libtiff/Makefile"

case "$1" in
  clean)
  for file in $makefile.am; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  if test -f $makefile.in.orig; then
    echo "Restore $makefile.in"
    mv -f $makefile.in.orig $makefile.in
  fi
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/tiff/libtiff/bootstrap
###

bootstrap="../libraries/tiff/libtiff/bootstrap"

case "$1" in
  clean)
  for file in $bootstrap; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/tiff/libtiff/ config scripts
###

dir="../libraries/tiff/libtiff"

case "$1" in
  clean)
  for file in config.guess config.sub install-sh missing ; do
    rm $dir/$file 2> /dev/null  &&  echo "Cleanup $dir/$file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac



##### Supporting libraries: Boost #############################################

###
### ../libraries/boost/configure.ac and configure.gnu
###

configure="../libraries/boost/configure"

case "$1" in
  clean)
  for file in $configure.ac $configure.gnu; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  if test -f $configure.orig; then
    echo "Restore $configure"
    mv -f $configure.orig $configure
  fi
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/boost/Makefile.am
###

dir="../libraries/boost"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac

###
### ../libraries/boost/bootstrap
###

bootstrap="../libraries/boost/bootstrap"

case "$1" in
  clean)
  for file in $bootstrap; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac


###
### ../libraries/boost/ config scripts
###

dir="../libraries/boost"

case "$1" in
  clean)
  for file in config.guess config.sub install-sh missing configure ; do
    rm $dir/$file 2> /dev/null  &&  echo "Cleanup $dir/$file"
  done
  test -d $dir  &&  rm -rf $dir  &&  echo "Cleanup $dir"
  ;;

  doc*)
  ;;

  *)
  ;;
esac




##### VFE #####################################################################

###
### ../vfe/Makefile.am
###

dir="../vfe"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  # includes the vfe/unix/ files to avoid circular dependencies when linking
  files=`find $dir $dir/unix -maxdepth 1 -name \*.cpp -or -name \*.h | sed s,"$dir/",,g | sort`

  echo "Create $makefile.am"
  cat Makefile.header > $makefile.am
  cat << pbEOF >> $makefile.am

# Makefile.am for the source distribution of POV-Ray $pov_version_base for UNIX
# Please report bugs to $pov_config_bugreport

# Libraries to build.
noinst_LIBRARIES = libvfe.a

# Source files.
libvfe_a_SOURCES = \\
  `echo $files`

cppflags_platformcpu = 
if BUILD_x86
cppflags_platformcpu += -I\$(top_srcdir)/platform/x86
endif

# Include paths for headers.
AM_CPPFLAGS = \\
  -I\$(top_srcdir)/unix/povconfig \\
  -I\$(top_srcdir)/platform/unix \\
  \$(cppflags_platformcpu) \\
  -I\$(top_srcdir)/vfe/unix \\
  -I\$(top_srcdir)/unix \\
  -I\$(top_srcdir)/source

# Extra definitions for compiling.
# They cannot be placed in config.h since they indirectly rely on \$prefix.
DEFS = \\
  @DEFS@ \\
  -DPOVLIBDIR=\"@datadir@/@PACKAGE@-@VERSION_BASE@\" \\
  -DPOVCONFDIR=\"@sysconfdir@/@PACKAGE@/@VERSION_BASE@\" \\
  -DPOVCONFDIR_BACKWARD=\"@sysconfdir@\"
pbEOF
  ;;
esac




##### Platform ################################################################

###
### ../platform/Makefile.am
###

dir="../platform"
makefile="$dir/Makefile"

case "$1" in
  clean)
  for file in $makefile.am $makefile.in; do
    rm $file 2> /dev/null  &&  echo "Cleanup $file"
  done
  ;;

  doc*)
  ;;

  *)
  files=`find $dir/unix -name "*.cpp" -or -name "*.h" | sed s,"$dir/",,g | sort`
  files_x86=`find $dir/x86 -maxdepth 1 -name "*.cpp" -or -name "*.h" | sed s,"$dir/",,g | sort`
  for ext in avx avxfma4 avx2fma3; do
    files_ext=`find $dir/x86/$ext -name "*.cpp" -or -name "*.h" | sed s,"$dir/",,g | sort`
    eval files_x86$ext='$files_ext'
  done

  echo "Create $makefile.am"
  cat Makefile.header > $makefile.am
  cat << pbEOF >> $makefile.am

# Makefile.am for the source distribution of POV-Ray $pov_version_base for UNIX
# Please report bugs to $pov_config_bugreport

# Platform-specifics.
cppflags_platformcpu =
libraries_platformcpu =
if BUILD_x86
cppflags_platformcpu += -I\$(top_srcdir)/platform/x86
libraries_platformcpu += libx86.a
libx86_a_SOURCES = `echo $files_x86`
libx86_a_CXXFLAGS = \$(CXXFLAGS)
endif
if BUILD_x86avx
libraries_platformcpu += libx86avx.a
libx86avx_a_SOURCES = `echo $files_x86avx`
libx86avx_a_CXXFLAGS = \$(CXXFLAGS) -mavx
endif
if BUILD_x86avxfma4
libraries_platformcpu += libx86avxfma4.a
libx86avxfma4_a_SOURCES = `echo $files_x86avxfma4`
libx86avxfma4_a_CXXFLAGS = \$(CXXFLAGS) -mavx -mfma4
endif
if BUILD_x86avx2fma3
libraries_platformcpu += libx86avx2fma3.a
libx86avx2fma3_a_SOURCES =  `echo $files_x86avx2fma3`
libx86avx2fma3_a_CXXFLAGS = \$(CXXFLAGS) -mavx2 -mfma
endif

# Libraries to build.
noinst_LIBRARIES = \\
  libplatform.a \\
  \$(libraries_platformcpu)

# Source files.
libplatform_a_SOURCES = \\
  `echo $files`

# Include paths for headers.
AM_CPPFLAGS = \\
  -I\$(top_srcdir)/unix/povconfig \\
  -I\$(top_srcdir)/platform/unix \\
  \$(cppflags_platformcpu) \\
  -I\$(top_srcdir)/vfe \\
  -I\$(top_srcdir)/vfe/unix \\
  -I\$(top_srcdir)/unix \\
  -I\$(top_srcdir)/source

# Extra definitions for compiling.
# They cannot be placed in config.h since they indirectly rely on \$prefix.
DEFS = \\
  @DEFS@ \\
  -DPOVLIBDIR=\"@datadir@/@PACKAGE@-@VERSION_BASE@\" \\
  -DPOVCONFDIR=\"@sysconfdir@/@PACKAGE@/@VERSION_BASE@\" \\
  -DPOVCONFDIR_BACKWARD=\"@sysconfdir@\"
pbEOF
  ;;
esac




###############################################################################
# Bootstrapping
###############################################################################

dir=".."
case "$1" in
  clean)
  # conf.h* is for backward compatibility
  for file in aclocal.m4 autom4te.cache conf.h conf.h.in conf.h.in~ config.h config.h.in config.h.in~ configure configure.ac Makefile Makefile.am Makefile.in stamp-h1; do
    rm -r $dir/$file 2> /dev/null  &&  echo "Cleanup $dir/$file"
  done
  ;;

  doc*)
  ;;

  *)
  echo "Run $dir/bootstrap"
  ok=`cd $dir/; ./bootstrap`
  # post-process DIST_COMMON in unix/Makefile.in
  for file in AUTHORS COPYING NEWS README configure.ac ChangeLog; do
    sed "s,$file,,g" ./Makefile.in > ./Makefile.in.tmp
    mv -f ./Makefile.in.tmp ./Makefile.in
  done
  ;;
esac


dir="../libraries/zlib"
case "$1" in
  clean)
  # don't remove Makefile, Makefile.in
  for file in aclocal.m4 autom4te.cache config.h config.h.in Makefile.am stamp-h1; do
    rm -r $dir/$file 2> /dev/null  &&  echo "Cleanup $dir/$file"
  done
  ;;

  doc*)
  ;;

  *)
  ;;
esac  # zlib


dir="../libraries/boost"
case "$1" in
  clean)
  for file in aclocal.m4 autom4te.cache config.h config.h.in Makefile Makefile.am Makefile.in stamp-h1; do
    rm -r $dir/$file 2> /dev/null  &&  echo "Cleanup $dir/$file"
  done
  ;;
esac  # boost


cd "$olddir"
