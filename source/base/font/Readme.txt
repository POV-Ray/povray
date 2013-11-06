reswrap is a program provided by libfox-1.6-dev (package of Ubuntu 12.04, version might vary)

To generate the files for a font foo.ttf:

 $ reswrap -o foo.cpp -e -n pov_base -p font_  -z foo.ttf
 $ reswrap -o foo.h -e -i -n pov_base -p font_  -z foo.ttf

