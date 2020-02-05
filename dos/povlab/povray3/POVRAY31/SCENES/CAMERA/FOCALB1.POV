// Persistence Of Vision raytracer version 3.1 sample file.
// Focal blur camera example
// File by Dan Farmer

#version 3.1;
global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"

#declare FB_Quality_Off  = 0;
#declare FB_Quality_Fast = 1;
#declare FB_Quality_Default = 2;
#declare FB_Quality_High = 3;

#declare FB_Quality= FB_Quality_High;

camera {
   location <2, 2, -11 >
   up y
   right <1.3333, 0, 0>
   direction z
   angle 37
   look_at <-1.75,0,0>

   aperture 2
   focal_point <0, 0, 0>

#switch(FB_Quality)
#case(FB_Quality_Off)
   aperture 0
   #warning "\nNo focal blur used...\n"
   #break
#case (FB_Quality_Fast)
   blur_samples 7
   confidence 0.5             // default is 0.9
   variance 1/64              // default is 1/128 (0.0078125)
   #warning "\nFast focal blur used...\n"
   #break
#case(FB_Quality_Default)
   blur_samples 19
   confidence 0.90            // default is 0.9
   variance 1/128             // default is 1/128 (0.0078125)
   #warning "\nDefault focal blur used...\n"
   #break
#case(FB_Quality_High)
   blur_samples 37
   confidence 0.975           // default is 0.9
   variance 1/255             // default is 1/128 (0.0078125)
   #warning "\nHigh Quality focal blur used...\n"
   #break
#else
   #warning "\nError! Switch condition skipped!...\n"
#end
}

background { color Black }

light_source { <30, 60, -100> color White }


#declare Obj = cylinder { -y*2, y*2, 0.2 finish { specular 0.75 roughness 0.005 metallic }}

union {
    object { Obj pigment{ Blue   } translate <-1,0,-8>}
    object { Obj pigment{ Green  } translate <-1,0,-6>}
    object { Obj pigment{ Yellow } translate <-1,0,-4>}
    object { Obj pigment{ Orange } translate <-1,0,-2>}
    object { Obj pigment{ Red    } translate <-1,0, 0>}
    object { Obj pigment{ Orange } translate <-1,0, 2>}
    object { Obj pigment{ Yellow } translate <-1,0, 4>}
    object { Obj pigment{ Green  } translate <-1,0, 6>}
    object { Obj pigment{ Blue   } translate <-1,0, 8>}
    translate y*1
      }

plane { y, 0 pigment { checker White, Gray90 }  scale 2}









