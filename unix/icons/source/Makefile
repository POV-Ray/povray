### Makefile --- 
#   Makefile for POV-Ray v3.6 for UNIX icons

## Author: chris_hormann@gmx.de
## Version: $Id: Makefile,v 0.0 2004/05/12 16:21:28 chris Exp $
## Keywords: 
## X-URL: 

default: all

all: file_logos

# --------- logos ---------

pov_logos:
	povray -iicon.pov -p -w16 -h16 +a0.0 -j +r8 +ua DECLARE=Image=2 -ofile_pov_16.png
	povray -iicon.pov -p -w32 -h32 +a0.0 -j +r6 +ua DECLARE=Image=2 -ofile_pov_32.png
	povray -iicon.pov -p -w48 -h48 +a0.0 -j +r4 +ua DECLARE=Image=2 -ofile_pov_48.png
	povray -iicon.pov -p -w64 -h64 +a0.0 -j +r4 +ua DECLARE=Image=2 -ofile_pov_64.png
	povray -iicon.pov -p -w16 -h16 +a0.0 -j +r8 +ua DECLARE=Image=3 -ofile_inc_16.png
	povray -iicon.pov -p -w32 -h32 +a0.0 -j +r6 +ua DECLARE=Image=3 -ofile_inc_32.png
	povray -iicon.pov -p -w48 -h48 +a0.0 -j +r4 +ua DECLARE=Image=3 -ofile_inc_48.png
	povray -iicon.pov -p -w64 -h64 +a0.0 -j +r4 +ua DECLARE=Image=3 -ofile_inc_64.png
	povray -iicon.pov -p -w16 -h16 +a0.0 -j +r8 +ua DECLARE=Image=5 -oshadow_16.png
	povray -iicon.pov -p -w32 -h32 +a0.0 -j +r6 +ua DECLARE=Image=5 -oshadow_32.png
	povray -iicon.pov -p -w48 -h48 +a0.0 -j +r4 +ua DECLARE=Image=5 -oshadow_48.png
	povray -iicon.pov -p -w64 -h64 +a0.0 -j +r4 +ua DECLARE=Image=5 -oshadow_64.png
	povray -iicon.pov -p -w16 -h16 +a0.0 -j +r8 +ua DECLARE=Image=1 -opovray_16.png
	povray -iicon.pov -p -w32 -h32 +a0.0 -j +r6 +ua DECLARE=Image=1 -opovray_32.png
	povray -iicon.pov -p -w48 -h48 +a0.0 -j +r4 +ua DECLARE=Image=1 -opovray_48.png
	povray -iicon.pov -p -w64 -h64 +a0.0 -j +r4 +ua DECLARE=Image=1 -opovray_64.png

file_logos_crystal:
	composite -compose Multiply shadow_16.png crystal_16.png crystal_shadow1_16.png
	composite -compose Multiply shadow_32.png crystal_32.png crystal_shadow1_32.png
	composite -compose Multiply shadow_48.png crystal_48.png crystal_shadow1_48.png
	composite -compose Multiply shadow_64.png crystal_64.png crystal_shadow1_64.png
	composite -compose CopyOpacity crystal_16.png crystal_shadow1_16.png crystal_shadow_16.png
	composite -compose CopyOpacity crystal_32.png crystal_shadow1_32.png crystal_shadow_32.png
	composite -compose CopyOpacity crystal_48.png crystal_shadow1_48.png crystal_shadow_48.png
	composite -compose CopyOpacity crystal_64.png crystal_shadow1_64.png crystal_shadow_64.png
	rm -f crystal_shadow1_16.png crystal_shadow1_32.png crystal_shadow1_48.png crystal_shadow1_64.png
	composite -compose Over file_pov_16.png crystal_shadow_16.png file_pov_crystal_16.png
	composite -compose Over file_pov_32.png crystal_shadow_32.png file_pov_crystal_32.png
	composite -compose Over file_pov_48.png crystal_shadow_48.png file_pov_crystal_48.png
	composite -compose Over file_pov_64.png crystal_shadow_64.png file_pov_crystal_64.png
	composite -compose Over file_inc_16.png crystal_shadow_16.png file_inc_crystal_16.png
	composite -compose Over file_inc_32.png crystal_shadow_32.png file_inc_crystal_32.png
	composite -compose Over file_inc_48.png crystal_shadow_48.png file_inc_crystal_48.png
	composite -compose Over file_inc_64.png crystal_shadow_64.png file_inc_crystal_64.png

file_logos_classic:
	composite -compose Multiply shadow_16.png classic_16.png classic_shadow1_16.png
	composite -compose Multiply shadow_32.png classic_32.png classic_shadow1_32.png
	composite -compose Multiply shadow_48.png classic_48.png classic_shadow1_48.png
	composite -compose Multiply shadow_64.png classic_64.png classic_shadow1_64.png
	composite -compose CopyOpacity classic_16.png classic_shadow1_16.png classic_shadow_16.png
	composite -compose CopyOpacity classic_32.png classic_shadow1_32.png classic_shadow_32.png
	composite -compose CopyOpacity classic_48.png classic_shadow1_48.png classic_shadow_48.png
	composite -compose CopyOpacity classic_64.png classic_shadow1_64.png classic_shadow_64.png
	rm -f classic_shadow1_16.png classic_shadow1_32.png classic_shadow1_48.png classic_shadow1_64.png
	composite -compose Over file_pov_16.png classic_shadow_16.png file_pov_classic_16.png
	composite -compose Over file_pov_32.png classic_shadow_32.png file_pov_classic_32.png
	composite -compose Over file_pov_48.png classic_shadow_48.png file_pov_classic_48.png
	composite -compose Over file_pov_64.png classic_shadow_64.png file_pov_classic_64.png
	composite -compose Over file_inc_16.png classic_shadow_16.png file_inc_classic_16.png
	composite -compose Over file_inc_32.png classic_shadow_32.png file_inc_classic_32.png
	composite -compose Over file_inc_48.png classic_shadow_48.png file_inc_classic_48.png
	composite -compose Over file_inc_64.png classic_shadow_64.png file_inc_classic_64.png

file_logos_slick:
	composite -compose Multiply shadow_16.png slick_16.png slick_shadow1_16.png
	composite -compose Multiply shadow_32.png slick_32.png slick_shadow1_32.png
	composite -compose Multiply shadow_48.png slick_48.png slick_shadow1_48.png
	composite -compose Multiply shadow_64.png slick_64.png slick_shadow1_64.png
	composite -compose CopyOpacity slick_16.png slick_shadow1_16.png slick_shadow_16.png
	composite -compose CopyOpacity slick_32.png slick_shadow1_32.png slick_shadow_32.png
	composite -compose CopyOpacity slick_48.png slick_shadow1_48.png slick_shadow_48.png
	composite -compose CopyOpacity slick_64.png slick_shadow1_64.png slick_shadow_64.png
	rm -f slick_shadow1_16.png slick_shadow1_32.png slick_shadow1_48.png slick_shadow1_64.png
	composite -compose Over file_pov_16.png slick_shadow_16.png file_pov_slick_16.png
	composite -compose Over file_pov_32.png slick_shadow_32.png file_pov_slick_32.png
	composite -compose Over file_pov_48.png slick_shadow_48.png file_pov_slick_48.png
	composite -compose Over file_pov_64.png slick_shadow_64.png file_pov_slick_64.png
	composite -compose Over file_inc_16.png slick_shadow_16.png file_inc_slick_16.png
	composite -compose Over file_inc_32.png slick_shadow_32.png file_inc_slick_32.png
	composite -compose Over file_inc_48.png slick_shadow_48.png file_inc_slick_48.png
	composite -compose Over file_inc_64.png slick_shadow_64.png file_inc_slick_64.png

file_logos: pov_logos
	$(MAKE) file_logos_crystal
	$(MAKE) file_logos_classic
	$(MAKE) file_logos_slick


### Makefile ends here
