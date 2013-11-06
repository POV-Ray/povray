// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: rad_def_test.pov
// Desc: rad_def.inc demo scene
// Date: May 2001
// Auth: Christoph Hormann
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

#include "rad_def.inc"
global_settings {
  assumed_gamma 1.0
  radiosity {
    Rad_Settings(Radiosity_Default, off, off)
    //Rad_Settings(Radiosity_Debug, off, off)
    //Rad_Settings(Radiosity_Fast, off, off)
    //Rad_Settings(Radiosity_Normal, off, off)
    //Rad_Settings(Radiosity_2Bounce, off, off)
    //Rad_Settings(Radiosity_Final, off, off)

    //Rad_Settings(Radiosity_OutdoorLQ, off, off)
    //Rad_Settings(Radiosity_OutdoorHQ, off, off)
    //Rad_Settings(Radiosity_OutdoorLight, off, off)
    //Rad_Settings(Radiosity_IndoorLQ, off, off)
    //Rad_Settings(Radiosity_IndoorHQ, off, off)

  }
}
//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
  location    <7, -10, 4>
  direction   y
  sky         z
  up          z
  right       x*image_width/image_height
  look_at     <0.0, 0.0, 0.7>
  angle       23
}

 
light_source {
  <3.2, -2.3, 1.8>*1
  color rgb 1.0
}
   

sphere {
  <0, 0, 0>, 1
  texture {
   pigment {
     gradient z
     color_map {
       [0.0 color rgb < 0.900, 0.910, 1.000 >]
       [0.2 color rgb < 0.700, 0.705, 1.000 >]
     }
   }
   finish { diffuse 0  #if (version < 3.7) ambient 1 #else emission 0.25 #end  }
  }
  hollow on
  no_shadow
  scale 3000
}



plane {
  z,0
  texture{
    pigment { color rgb 1 }
    finish  { ambient 0.0 diffuse 0.65 }
  }
}

#declare Rim=
union {
  #declare Cnt=0.0;

  #while (Cnt<=1.8)
    box { <0,     Cnt, 0>, <0.2,        Cnt+0.16, 0.2> }
    box { <2-Cnt, 0,   0>, <2-Cnt-0.16, 0.2,      0.2> }
    box { <2,     Cnt, 0>, <1.8,        Cnt+0.16, 0.2> }
    box { <Cnt,   2,   0>, <Cnt+0.16,   1.8,      0.2> }

    #declare Cnt=Cnt+0.36;
  #end
}

union {

  #declare Cnt=0.12;

  #while (Cnt<1.8)
    union {
      cylinder { <0, 0, 0>, <0, 0, 1.6>, 0.08 }
      torus { 0.1, 0.04 rotate 90*x translate 0.04*z }
      translate <2, Cnt, 0>
    }
    #declare Cnt=Cnt+0.37;
  #end

  difference {
    union {
      box { <0, 0, 0>, <0.1, 2, 1.6> }
      box { <0.05, 0, 0>, <-0.3, 0.9, 1.0> }
    }
    box { <0.2, 0.1, -0.1>, <-1.0, 0.8, 0.9> }
  }

  cylinder { <-0.1, 0, 0>, <-0.1, 0, 1.0>, 0.2 }
  sphere { <-0.1, 0, 1.0>, 0.2 }

  box { <0, 1.9, 0>, <2, 2, 1.6> }

  difference {
    union {
      box { <0, 0, 1.6>, <2, 2, 1.8> }
      box { <-0.03, -0.03, 1.75>, <2.03, 2.03, 1.8> }
    }
    cylinder { <1, 1, 1.5>, <1, 1, 2>, 0.4 }
  }

  object { Rim translate <0, 0, 1.8> }

  translate <-1, -1, 0>

  texture{
    pigment { color rgb 1 }
    finish  { ambient 0.0 diffuse 0.65 }
  }
}



