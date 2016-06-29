//----------------------- user_defined.pov -------------------------
// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
//----------------- Inward facing cylinderical ---------------------
//
// +w900 +h450 +a0.1

#version 3.71; // user_defined introduced with 3.71.
global_settings { assumed_gamma 1 }

#include "shapes.inc"
#declare Grey50 = srgbft <0.5,0.5,0.5,0,0>;
background { color Grey50 }
#declare Black = srgbft <0,0,0,0,0>;
#declare OurText = "Round and Round and Round and Round and Round old Y we go."
#declare Text00 = object {
    Circle_Text_Valigned("crystal.ttf",OurText,
         0.10,0.001,0.02,0.5,0,Align_Left,-5,90)
    pigment { color Black }
    rotate x*-90
}
#declare CameraP = camera {
    perspective
    location <2.3,2.3,-2.301>
    sky <0,1,0>
    angle 35
    right x*(image_width/image_height)
    look_at <0,0,0>
}
#declare White = srgbft <1,1,1,0,0>;
#declare Light00 = light_source { <50,150,-250>, White }
#declare CylY = cylinder { <0,-1,0>, <0,1,0>, 0.01
    texture { pigment { color White }
              finish { ambient 0 diffuse 0 emission 1 }
    }
}
//------- Set up functional camera
#declare FnXrad = function (x) { (x+0.5)*tau }
#declare FnX    = function (x) { cos(FnXrad(x)) }
#declare FnZ    = function (x) { sin(FnXrad(x)) }
#declare Camera00 = camera {
    user_defined
    location {
      function { FnX(x) }
      function { y }
      function { FnZ(x) }
    }
    direction {
      function { -FnX(x) }
      function { 0 }
      function { -FnZ(x) }
    }
}
//---
  camera { Camera00 }
//camera { CameraP  } // Use to see perspective view
light_source { Light00 }
object { CylY }
object { Text00 }

