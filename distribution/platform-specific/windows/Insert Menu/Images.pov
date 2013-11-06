// Persistence of Vision Ray Tracer Scene Description File
// File: Images.pov
//
// Desc: wrapping file for rendering all Insert menu pictures
// Date: August 2001
// Auth: Christoph Hormann
// Updated for POV-Ray 3.7 and extended by Friedrich A. Lohmueller, Feb-2012
// Updated and extended by Shapes3.pov, Normal.pov, Woods.pov, Meshmaker.pov
// by Friedrich A. Lohmueller, March-2013
//
// Modified numbering for easier customizing. August 30th, Bob Hughes
// When adding to the menu previews simply append to the existing scene files
// and use next number in series.  Or add new file and insert a new range.
// Be sure to also add entries into Insert Menu\Image.ini
// Aspect ratios are not standardized, 
// for correct options see file 'image.ini'


#version 3.7;

#switch (Switch)
 
  #range (101,138)     // --- patterns ---  
    #declare Typ=Switch-100;
    #include "Patterns.pov"
    #break
       
  #range (401,599)     // --- patterns --- new tiling, pavement
    #declare Typ=Switch-300;
    #include "Patterns.pov"
    #break
 
  #range (201,208)     // --- pattern texture attributes ---
    #declare Typ=Switch-200;
    #include "Attributes.pov"
    #break

  #range (301,304)     // --- transformations ---
    #declare Typ=Switch-300;
    #include "Transform.pov"
    #break

  #range (701,702)     // --- material ---
    #declare Typ=Switch-700;
    #include "Material.pov"
    #break

  #range (801,807)     // --- no_shadow, no_reflection, no_image, open ---
    #declare Typ=Switch-800;
    #include "No_XXX.pov"
  #break

  #range (901,910)     // --- csg ---
    #declare Typ=Switch-900;
    #include "CSG_Material.pov"
  #break

  #range (1001,1009)     // --- lights ---  
    #declare Typ=Switch-1000;
    #include "Lights.pov"
  #break
 
  #range (1101,1106)     // --- include --- overview
    #declare Typ=Switch-1100; 
    #include "Include.pov"     
  #break
  
  #range (1301,1311)     // --- camera ---  
    #declare Typ=Switch-1300;
    #include "Camera.pov"
  #break

//;---------------------------------- new 2012


  #range (51,69)     // --- basic scenes Lo---
    #declare Typ=Switch-50;
    #include "Ready made scenes.pov"
  #break

  #range(3001,3160)     // --- basic shapes Lo---
    #declare Typ=Switch-3000;
    #include "Basic shapes.pov"
  #break
   
  #range(4001,4077)     // --- shapes2 Lo---
    #declare Typ=Switch-4000;
    #include "Shapes2.pov"
  #break

  #range(3501,3572)     // --- shapes3 Lo---
    #declare Typ=Switch-3500;
    #include "Shapes3.pov"
  #break
 
  #range(5110,5955)     // --- special shapes Lo -- all!
    #declare Typ=Switch-5000;
    #include "Special shapes.pov"
  #break
 
  #range(6010,6089)     // --- colors Lo---
    #declare Typ=Switch-6000;
    #include "Colors.pov"
  #break

  #range(7100,7255)     // --- Loops + sphere_sweep + spline curves Lo---
    #declare Typ=Switch-7000;
    #include "Loops_sweep_spline.pov"
  #break

  #range(8010,8027)     // --- Shear_Transform Lo---
    #declare Typ=Switch-8000;
    #include "Shear_Transform.pov"
  #break

  #range(8030,8075)     // --- random Lo---
    #declare Typ=Switch-8000;
    #include "Random.pov"
  #break
 
  #range(8110,8121)     // --- animation Lo---
    #declare Typ=Switch-8100;
    #include "Animation.pov"
  #break

  #range(8530,8590)     // --- photons and radiosity Lo---
    #declare Typ=Switch-8500;
    #include "Photons_and_Radiosity.pov"
  #break

  #range(8710,8780)     // --- sky fog rainbow Lo---
    #declare Typ=Switch-8700;
    #include "Sky.pov"
  #break

  #range(9001,9092)     // --- texture samples + mirrors & glasses Lo---
    #declare Typ=Switch-9000;
    #include "Tex_Mat.pov"
  #break

  #range(9110,9160)     // --- normal samples Lo---
    #declare Typ=Switch-9100;
    #include "Normal.pov"
  #break

  #range(9200,9369)     // --- wood samples  Lo---
    #declare Typ=Switch-9200;
    #include "Woods.pov"
  #break

  #range(9400,9443)     // --- metals  Lo---
    #declare Typ=Switch-9400;
    #include "Metals.pov"
  #break
 
  #range(9501,9511)     // --- Include files own Lo---
    #declare Typ=Switch-9500;
    #include "IncludeFiles_own.pov"
  #break
 
  #range(9601,9611)     // --- math function samples Lo---
    #declare Typ=Switch-9600;
    #include "Math functions.pov"
  #break
 
  #range(9700,9784)     // --- stones and granites  Lo---
    #declare Typ=Switch-9700;
    #include "Stones_and_Granites.pov"
  #break
 
  #range(10010,10163)     // --- metals  Lo---
    #declare Typ=Switch-10000;
    #include "Meshmaker.pov"
  #break



#end
