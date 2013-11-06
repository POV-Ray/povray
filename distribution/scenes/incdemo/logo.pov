// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer Scene Description File
// File: logo.pov
// Desc: logo.inc demo scene
// Date: September 2001
// Auth: Rune S. Johansen
// 
// This file is an example of how to use the official POV-Ray Logo
// in a scene. The original version as well as a prism version and
// a bevel version are available. Any texture or material can be
// applied to the logo, that way allowing for a great variety of looks.
//
// For more information on the POV-Ray logo, see logo.inc
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "logo.inc"

#declare Design = 0;
// 0 : All four designs
// 1 : Original logo object with 2-d appearance.
// 2 : Original logo object.
// 3 : Prism logo object.
// 4 : Beveled logo object.

#declare LogoPigment =
pigment {
   planar scale 2 translate y
   color_map {
      [0.0, color <1.0, 0.4, 0.4>*0.7] // Red
      [0.5, color <0.4, 0.9, 0.4>*0.7] // Green
      [1.0, color <0.4, 0.4, 1.0>*0.7] // Blue
   }
}

background {color rgb <1,0.98,0.93>*1} // White background

camera {
   // When using the logo for image_maps, 2-d designs etc.,
   // always use orthographic camera.
   orthographic angle 69
   location -2*z
   
   right   x*image_width/image_height
   look_at <0,0,0>
}

light_source {< 3, 4,-5>*1000, color 2.0}

#declare Variant1 =
object {
   Povray_Logo
   pigment {LogoPigment}
   // Remove all lighting to get a 2-d appearance:
   finish {ambient 1 diffuse 0}
}

#declare Variant2 =
object {
   Povray_Logo
   pigment {LogoPigment}
   rotate -10*y
}

#declare Variant3 =
object {
   Povray_Logo_Prism
   pigment {LogoPigment}
   rotate -10*y
}

#declare Variant4 =
difference{
   object {Povray_Logo_Bevel}
   // Adjust this plane to adjust the bevel:
   plane {z,-0.05}
   pigment {LogoPigment}
}

#switch (Design)
   #case(0)
      union {
         object {Variant1 translate <-1,+1>}
         object {Variant2 translate <+1,+1>}
         object {Variant3 translate <-1,-1>}
         object {Variant4 translate <+1,-1>}
         scale 0.5
      }
   #break
   #case(1) object {Variant1} #break
   #case(2) object {Variant2} #break
   #case(3) object {Variant3} #break
   #case(4) object {Variant4} #break
#end
