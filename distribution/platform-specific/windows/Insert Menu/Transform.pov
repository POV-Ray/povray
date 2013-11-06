// Insert menu illustration scene
// Created June-August 2001 by Christoph Hormann
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012.

// ----- transformations submenu -----

// -w120 -h48 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#include "colors.inc"


//#declare Typ=1;     // translate
//#declare Typ=2;     // rotate
//#declare Typ=3;     // scale
//#declare Typ=4;     // matrix


global_settings {
  //assumed_gamma 1
  max_trace_level 5
}

light_source {
  <1.5, 0.35, 1.0>*10000
  color rgb 1.0
}

camera {
  location    <10, 20, 7.5>
  direction   y
  sky         z
  up          z
  right       (120/48)*x
  look_at     <0, 0, -0.2>
  angle       28
}

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [1.0 rgb <0.2,0.2,0.8>]
    }
  }
}

// ----------------------------------------

plane
{
  z, -1
  texture
  {
    pigment { color rgb 1 }
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

//---------------------------------------------------

#declare Tex1=
  texture {

    #switch (Typ)
      #case (1)
        pigment { color NeonBlue }
        #break
      #case (2)
        pigment { color ForestGreen }
        #break
      #case (3)
        pigment { color Coral }
        #break
      #case (4)
        pigment { color SpicyPink }
    #end

    finish { specular 0.4 roughness 0.02 }
  }


// =============================================

#declare Obj=
box {
  #if (Typ=1)
    <-0.4, -0.8, -0.8> <0.4, 0.8, 0.8>
  #else
    <-0.3, -0.8, -0.8> <0.3, 0.8, 0.8>
  #end
  texture { Tex1 }
}

// =============================================

#if (Typ=1)

#declare Cnt=0;

#while (Cnt < 5)
  object {
    Obj
    translate <-4.0+Cnt*2, 0.8, -1>
    translate Cnt*0.5*z
  }
  #declare Cnt=Cnt+1;
#end

#end

// =============================================

#if (Typ=2)

#declare Cnt=0;

#while (Cnt < 5)
  object {
    Obj
    rotate -Cnt*18*z
    translate <-4.2+Cnt*2, 0, 0>
  }
  #declare Cnt=Cnt+1;
#end

#end

// =============================================

#if (Typ=3)

#declare Cnt=0;

#while (Cnt < 5)
  object {
    Obj
    scale (Cnt+1)*0.3
    translate <-4.2+Cnt*2, 0, 0>
  }
  #declare Cnt=Cnt+1;
#end

#end

// =============================================

#if (Typ=4)

#declare Cnt=0;

#while (Cnt < 5)
  object {
    Obj
    matrix < 1, 0, 0,
             0, 1, 0,
	     Cnt*0.3, 0, 1,
	     0, 0, 0>

    translate <-4.2+Cnt*2, 0, 0>
  }
  #declare Cnt=Cnt+1;
#end

#end

