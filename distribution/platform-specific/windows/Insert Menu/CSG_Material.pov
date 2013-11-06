// Persistence of Vision Ray Tracer Scene Description File
// File: im_CSG_Material.pov
// Vers: 3.6
// Desc: CSG operations: cutaway_textures, difference, intersection,
// inverse, merge, union
// Also Material: interior, media
// Date: August 2001
// Auth: Bob Hughes
//       modified Aug. 25 by Christoph Hormann
// Adapted and extended variation for version 3.7
// by Friedrich A.  Lohmueller
//
// +w120 +h48 +a0.1 +r3 +am2

#version 3.7;
#if ( Typ != 4 )
global_settings{ assumed_gamma 1.0 } 
#end 

#include "textures.inc" 
#include "glass.inc" 
#include "shapes.inc" 
#include "shapes2.inc" 

//#declare Typ=1;  // CSG union
//#declare Typ=2;  // CSG difference
//#declare Typ=3;  // CSG intersection
//#declare Typ=4;  // CSG merge
//#declare Typ=5;  // CSG inverse
//#declare Typ=6;  // CSG cutaway texture
//#declare Typ=7;  // interior
//#declare Typ=8;  // atmospheric media
//#declare Typ=9;  // clipped_by


#declare Inverse1=no; // inverses for CSG's
#declare Inverse2=no;
#declare Inverse3=no;

#if ((Typ=1) | (Typ=2) | (Typ=3) | (Typ=4) )
  #declare MenuPreview=1;  // which one? 1=CSG, 2=interior, 3=media, 4=better media example

  #declare CA=no; // cutaway texture used or not (type 1 CSG)

  #declare CSG=Typ-1; // type of CSG: 0=union, 1=difference, 2=intersection, 3=merge

  #declare C=.8;  // color (for interior example objects, not fade color)
  #declare F=0.0; // filter
  #declare T=.4;  // transmit
#end

#if (Typ=5)
  #declare MenuPreview=1;  // which one? 1=CSG, 2=interior, 3=media, 4=better media example

  #declare CA=no; // cutaway texture used or not (type 1 CSG)

  #declare CSG=2; // type of CSG: 0=union, 1=difference, 2=intersection, 3=merge

  #declare Inverse1=yes; // inverses for CSG's
  #declare Inverse2=no;
  #declare Inverse3=no;

  #declare C=.8;  // color (for interior example objects, not fade color)
  #declare F=0.0; // filter
  #declare T=.4;  // transmit
#end

#if (Typ=6)
  #declare MenuPreview=1;  // which one? 1=CSG, 2=interior, 3=media, 4=better media example

  #declare CA=yes; // cutaway texture used or not (type 1 CSG)

  #declare CSG=1; // type of CSG: 0=union, 1=difference, 2=intersection, 3=merge

  #declare C=.8;  // color (for interior example objects, not fade color)
  #declare F=0.0; // filter
  #declare T=.4;  // transmit
#end


#if (Typ=7)
  #declare MenuPreview=2;  // which one? 1=CSG, 2=interior, 3=media, 4=better media example

  #declare C=1;//.9; // color (for interior example objects, not fade color)
  #declare F=1;//.91; // filter
  #declare T=0;//.09; // transmit
#end

#if (Typ=8)
  #declare MenuPreview=4;  // which one? 1=CSG, 2=interior, 3=media, 4=better media example

  #declare C=1;//.9; // color (for interior example objects, not fade color)
  #declare F=1;//.91; // filter
  #declare T=0;//.09; // transmit
#end

#if (Typ=9)
  #declare MenuPreview=5;  // which one? 1=CSG, 2=interior, 3=media, 4=better media example
#end

//#include "colors.inc"

global_settings {
 
 // assumed_gamma 1.0
  ambient_light 1
  max_trace_level 15
}

// ----------------------------------------
#if (Typ!=8)
light_source {

  0,
  #if (MenuPreview=4)
    color rgb 0.26  // light's color (1.5 unless doing MenuPreview 4, then 0.5)
  #else
    color rgb 1
  #end
 
  #if(Typ=9)
  translate <-250, 250,  100>
  #else
  translate <-250, 250, -300>
  #end
  media_interaction off
}

camera {  

  #if(Typ=9)
  location  <-6,9.5, -13>
  rotate 16*y
  look_at   <-0.0,0,0.3>
  angle 26

  #else
  location  <-6, 7.5, -15>
  rotate 16*y
  look_at   <-0.3,0,0>
  angle 26

  #end 
  right x*image_width/image_height  
}


sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [0.7 rgb <0.0,0.1,0.8>]
    }
  }
}
#end // #if (Typ!=8)

plane {<0,1,0>, -1.001
  texture {
    pigment { color rgb 1 } //checker color rgb 1 color rgb <1,1,1>*0.5 scale 0.1}
    finish {
      diffuse 0.7
      specular 0.4
      roughness 0.01

      reflection { 0.5 , 1.0
        fresnel on
        metallic 0.8
      }
      conserve_energy

    }
  }
}
#declare Base_Color = color rgb<1,0,0>; 
#declare Cut_Color  = color rgb<1,0.85,0>; 

//////////////////////////////////////////////////////////////////////////////////

#switch (MenuPreview)

#case (1) /* CSG's */

#switch (CSG)
#case (0)
union
#break
#case (1)
difference
#break
#case (2)
intersection
#break
#case (3)
merge
#break
#end
 {
sphere {
  z/2, 1 
  #if (Inverse1=yes) inverse #end
  texture {
     pigment {
      Base_Color transmit T
    }
    finish{
      diffuse 0.6
      ambient 0.1
      specular 0.2
    }
  }
}
sphere {
   z/2, 1 translate<0,0.6,0>
  #if (Inverse2=yes) inverse #end
#if (CA=no)
  texture {
    pigment {
      Cut_Color transmit T
    }
    finish{
      diffuse 0.6
      ambient 0.1
      specular 0.2
    }
  }
#end
} rotate -15*y
// scale <1,1,-1>
no_shadow
#if (CA=yes)
  cutaway_textures
#end
  #if (Inverse3=yes) inverse #end
}

#switch (CSG)
#case (0)
union
#break
#case (1)
difference
#break
#case (2)
intersection
#break
#case (3)
merge
#break
#end
 {
cone {
  -y,1,y,0
  #if (Inverse1=yes) inverse #end
  texture {
    pigment {
      Base_Color transmit T
    }
    finish{
      diffuse 0.6
      ambient 0.1
      specular 0.2
    }
  } translate z/2
}
cone {
  -y*1.01,1,y,0
  #if (Inverse2=yes) inverse #end
#if (CA=no)
  texture {
    pigment {
      Cut_Color transmit T
    }
    finish{
      diffuse 0.6
      ambient 0.1
      specular 0.2
    }
  }
#end
  translate -z/2
}
 rotate -15*y translate -x*3
// scale <1,1,-1>
no_shadow
#if (CA=yes)
  cutaway_textures
#end
  #if (Inverse3=yes) inverse #end
 }

#switch (CSG)
#case (0)
union
#break
#case (1)
difference
#break
#case (2)
intersection
#break
#case (3)
merge
#break
#end
 {
cylinder {
  -y,y, 1
  #if (Inverse1=yes) inverse #end
  texture {
    pigment {
      Base_Color transmit T
    }
    finish{
      diffuse 0.6
      ambient 0.1
      specular 0.2
    }
  } translate z/2
}
cylinder {
  -y*1.01,y*1.01, 1
  #if (Inverse2=yes) inverse #end
#if (CA=no)
  texture {
    pigment {
      Cut_Color transmit T
    }
    finish{
      diffuse 0.6
      ambient 0.1
      specular 0.2
    }
  }
#end
   translate -z/2
}
 rotate -15*y translate x*3
// scale <1,1,-1>
no_shadow
#if (CA=yes)
  cutaway_textures
#end
  #if (Inverse3=yes) inverse #end
}

#break

//////////////////////////////////////////////////////////////////////////////////

#case (2) /* Interior's */

sphere {
  0, 1 //inverse
  texture {
    pigment {
      color rgb C filter F transmit T
    }
    finish{
      diffuse 0.1
      ambient 0.1
      specular 0.9
      roughness 0.01
    }
  }
  interior {
    ior 1.3               // index of refraction (1 = none, use > 1)
    dispersion 1.25        // spreads light/background colors into a spectrum (1..1.01..1.1 >)
    dispersion_samples 13 // accuracy of calculation (2..100) [7]
    caustics 1.1          // faked caustics (0..1 >)
    fade_power 1234       // values larger than 1000 give realistic exponential attenuation
    fade_distance 1       // distance where light reaches half intensity
    fade_color <0, 1, 0> // color for fading
   // media {..media items..}
  }
 scale 1
}

cone {
  -y,1,y,0 //inverse
  texture {
    pigment {
      color rgb C filter F transmit T
    }
    finish{
      diffuse 0.1
      ambient 0.1
      specular 0.9
      roughness 0.01
    }
  }
// describes inside of a shape (similar to texture, but for inside, not surface)
  interior {
    ior 1.1               // index of refraction (1 = none, use > 1)
    dispersion 1.5        // spreads light/background colors into a spectrum (1..1.01..1.1 >)
    dispersion_samples 13 // accuracy of calculation (2..100) [7]
    caustics 1.2          // faked caustics (0..1 >)
    fade_power 1234       // values larger than 1000 give realistic exponential attenuation
    fade_distance 5       // distance where light reaches half intensity
    fade_color <1, 0, 0> // color for fading
   // media {..media items..}
  }
 scale 1 translate -3*x
}

cylinder {
  -y,y, 1 //inverse
  texture {
    pigment {
      color rgb C filter F transmit T
    }
   finish{
      diffuse 0.1
      ambient 0.1
      specular 0.9
      roughness 0.01
    }
  }
  interior {
    ior 1.5               // index of refraction (1 = none, use > 1)
    dispersion 1.0        // spreads light/background colors into a spectrum (1..1.01..1.1 >)
    dispersion_samples 13 // accuracy of calculation (2..100) [7]
    caustics 1.0          // faked caustics (0..1 >)
    fade_power 1234       // values larger than 1000 give realistic exponential attenuation
    fade_distance .5       // distance where light reaches half intensity
    fade_color <0, 0, 1> // color for fading
   // media {..media items..}
  }
 scale 1 translate 3*x
}

#break

//////////////////////////////////////////////////////////////////////////////////
// media 1 (thanks goes out to Nathan Kopp here for double media suggestion)

#case (3) /* media's */

union {

sphere {
  0, 1 //inverse
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media {method 3 samples 1,10 emission 1 absorption 0
         scattering {1,0.1 extinction 1}
     density {spherical
    density_map {[0 rgb <0,1,0>][1 rgb <0,0,0>]} frequency -1
    }
    }
    media {method 3 samples 1,10 emission 0 absorption 1
         scattering {1,0.1 extinction 1}
     density {spherical
    density_map {[0 rgb <1,0,1>][1 rgb <0,0,0>]} frequency -1
    }
    }
  }
 }
 scale 1
 hollow
}

cone {
  -y,1,y,0 //inverse
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media {method 3 samples 1,10 emission 1 absorption 0
         scattering {1,0.1 extinction 1}
     density {gradient y
    density_map {[0 rgb <1,0,0>][1 rgb <0,0,0>]} frequency 1 scale 2 translate -y
    }
    }
    media {method 3 samples 1,10 emission 0 absorption 1
         scattering {1,0.1 extinction 1}
     density {gradient y
    density_map {[0 rgb <0,1,1>][1 rgb <0,0,0>]} frequency 1 scale 2 translate -y
    }
    }
  }
 }
 scale 1 translate -3.5*x
 hollow
}

cone {
  -y,1,y,0 //inverse
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media {method 3 samples 1,10 emission 1 absorption 0
         scattering {1,0.1 extinction 1}
     density {gradient y
    density_map {[0 rgb <1,1,0>][1 rgb <0,0,0>]} frequency 1 turbulence 1 scale 2 translate -y
    }
    }
    media {method 3 samples 1,10 emission 0 absorption 1
         scattering {1,0.1 extinction 1}
     density {gradient y
    density_map {[0 rgb <0,0,1>][1 rgb <0,0,0>]} frequency 1 turbulence 1 scale 2 translate -y
    }
    }
  }
 }
 scale 1 translate <-2,0,3>
 hollow
}

cylinder {
  -y,y, 1 // inverse
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media {method 3 samples 1,10 emission 1 absorption 0
         scattering {1,0.1 extinction 1}
     density {cylindrical
    density_map {[0 rgb <0,0,1>][1 rgb <0,0,0>]} frequency -1 // turbulence 1
    }
    }
    media {method 3 samples 1,10 emission 0 absorption 1
         scattering {1,0.1 extinction 1}
     density {cylindrical
    density_map {[0 rgb <1,1,0>][1 rgb <0,0,0>]} frequency -1 // turbulence 1
    }
    }
  }
 }
 scale 1 translate 3.5*x
  hollow
}

cylinder {
  -y,y, 1 // inverse
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media {method 3 samples 1,10 emission 1 absorption 0
         scattering {1,0.1 extinction 1}
     density {cylindrical
    density_map {[0 rgb <0,1,1>][1 rgb <0,0,0>]} frequency -1  turbulence 1
    }
    }
    media {method 3 samples 1,10 emission 0 absorption 1
         scattering {1,0.1 extinction 1}
     density {cylindrical
    density_map {[0 rgb <1,0,0>][1 rgb <0,0,0>]} frequency -1  turbulence 1
    }
    }
  }
 }
 scale 1 translate <1.25,0,-2.5>
  hollow
}
}

#break

//////////////////////////////////////////////////////////////////////////////////
 
#case (4) /* atmospheric media example, light beams through window/door */

camera { angle 75 location<0.0, 2.5,-3.0> look_at   <-0.2 , 1.5 , 0.0> right x*image_width/image_height }
#include concat("H0 - Sky, fog, rainbow/","20 - Blue Sky by sky_sphere.txt")
light_source {<-125,50,100>,<5,5,2.5>*0.6  spotlight radius 2 falloff 3 point_at 0 }

/* atmospheric media example, light beams through window/door */
union { 
// window/door
difference {
box {-1,1 scale <1,1,.1>}
box {-1,1 scale <.4,.4,.11> translate <-.5,-.5,0>}
box {-1,1 scale <.4,.4,.11> translate <.5,-.5,0>}
box {-1,1 scale <.4,.4,.11> translate <.5,.5,0>}
box {-1,1 scale <.4,.4,.11> translate <-.5,.5,0>}
 pigment {rgb <1,1,1>} finish {ambient .2 diffuse .5 specular .3 roughness .1}
 scale 2 translate <2,1,-2>
}
// room wall
box {  -1,1 scale <100,100,0.01> translate -2.01*z
       clipped_by {box {-1,1 scale <2,2,0.1> translate <2,1,-2> inverse}}
       pigment {rgb 1}}
// room air
box {
  -1,1
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media { method 3 intervals 1 samples 50,50 emission 0 absorption 0.01
            scattering {1,0.1 extinction 0.3}
    }
  }
 }
 scale <3,2,1>*3 translate z
 hollow
}
rotate 180*y
}// end of room

#end

#if(Typ=9)
union{
sphere { <0,0,0>, 1 
         texture{ pigment{ color rgb<1,0,0> }
                  finish{ diffuse 0.9 ambient 0.1 specular 0.2 }
                }
         clipped_by { box{<-1,-1,-1>,<1,0.25,1>}}
       } //-----------------------
object { // Wire_Box(A, B, WireRadius, UseMerge)
         Wire_Box(<-1,-1,-1>,<1,0.25,1>, 0.01   , 0)  
         
         texture{ pigment{ color rgb<1,1,1>}
                }
         scale<1,1,1>  rotate<0, 0,0> translate<0,0.1,0>
       } // --------------------------------------------- 
 scale 2
 translate<0,1,0>
} 



#end //