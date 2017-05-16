#!/bin/sh

# Run this script from time to time to update revision.txt.

outfile="revision.txt~"
tempfile="${outfile}~"

# first, generate a list of version tags
releases=`git log --decorate --date=short --pretty=format:"%d" HEAD | sed -n 's/^.*tag: \(v3[.][^,)]*\).*$/\1/p'`
# strip any experimental versions
releases=`echo "${releases}" | sed 's/^v[0-9.]*-x.*$//g'`

# generate list of relevant commits
echo -n "Fetching revision data from Git repo"
current="HEAD"
rm "${outfile}"
for prev in $releases
do
  echo -n "."
  git log --decorate --date=short --topo-order --pretty=tformat:"%d%n%nCommit %h on %cd by %an %n%n%n%w(0,4,4)%B%n" "$current" "^$prev" >> "${outfile}"
  current="$prev"
done
git log --decorate --date=short --topo-order --pretty=tformat:"%d%n%nCommit %h on %cd by %an %n%n%n%w(0,4,4)%B%n" "$current" >> "${outfile}"
echo "Done."

# split up ref name lists into single lines
echo -n "Isolating ref names"
while grep -e '^ ([^,)]*,' "${outfile}" >/dev/null
do
  echo -n "."
  sed 's/^ (\([^,)]*\),\s/ (\1)\n (/g' "${outfile}" > "${tempfile}"
  mv -f "${tempfile}" "${outfile}"
done
echo "Done."

# eliminate any tags indicating experimental versions
sed 's/^ (tag:\sv[0-9.]*-x[^)]*)$//g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# prettify the tag ref names
divider='------------------------------------------------------------------------------'
sed 's/^ (tag:\s\(v3\.[^)+]*\)[^)]*)/'$divider'\nPOV-Ray \1\n'$divider'/g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"
sed 's/^ (tag:\s\([^)+]*\)[^)]*)/'$divider'\n??? \1\n'$divider'/g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# eliminate any other ref names
sed 's/^ ([^)]*)$//g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# collapse consecutive empty lines
sed -r ':a; /^\s*$/ {N;ba}; s/( *\n){2,}/\n/' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# remove trailing whitespace
sed 's/\s*$//g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# identify known authors
sed 's/by chris20$/by Chris Cason (chris20)/g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"
sed 's/by c-lipka$/by Christoph Lipka (c-lipka)/g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"
sed 's/by wfpokorny$/by William F. Pokorny (wfpokorny)/g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"
sed 's/by BentSm$/by Ben Small (BentSm)/g' "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# word wrap after 79 characters
fmt -w79 -s "${outfile}" > "${tempfile}"
mv -f "${tempfile}" "${outfile}"

# add header
(
  echo $divider
  echo "POV-Ray Revision History"
  echo $divider
  echo "NOTE: This auto-generated revision history needs manual postprocessing."
  echo "DO NOT COMMIT THIS FILE TO THE REPOSITORY!"
  echo $divider
  echo ""
  echo $divider
  echo "POV-Ray v???"
  echo $divider
  cat "${outfile}"
) > "${tempfile}"
mv -f "${tempfile}" "${outfile}"
