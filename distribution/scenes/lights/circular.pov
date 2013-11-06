// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
//Demonstration of the area_light "circular" keyword
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

camera { location <50,42,0> 
         right     x*image_width/image_height
         angle 65 // direction z*1 
         look_at <0,2,0>
       }

//lights
light_source {<10,15,15> White*1.5
              area_light x*1.5,z*1.5,4,4 adaptive 1
              circular //that's here !
              }

light_source {<10,15,-15> White*1.5
              area_light x*1.5,z*1.5,4,4 adaptive 1
              }

//objects
#declare With =
union {
 text {ttf "cyrvetic","circular",.05,0}
 text {ttf "cyrvetic","with",.05,0 translate y*1}
 pigment {SteelBlue}
 rotate <0,-90,0>
 scale 3
 }

#declare Without =
union {
 text {ttf "cyrvetic","circular",.05,0}
 text {ttf "cyrvetic","without",.05,0 translate y*1}
 pigment {SteelBlue}
 rotate <0,-90,0>
 scale 3
 }

#declare Object_4 = union {
 disc {<0,0,0>,y,1,0 pigment {OrangeRed}}
 disc {<0,0,0>,y,4,2 pigment {OrangeRed}}
 disc {<0,0,0>,y,7,5 pigment {OrangeRed}}
 }

object {With translate <-5,12,5+3> no_shadow}
object {Without translate <-5,12,5-30> no_shadow}
object {Object_4 translate <10,10,15>}
object {Object_4 translate <10,10,-15>}

//markers to show where the light_sources are
sphere {<10,15,15>,.4 pigment {Green*2} finish {diffuse 0 ambient 1} no_shadow}
sphere {<10,15,-15>,.4 pigment {Green*2} finish {diffuse 0 ambient 1} no_shadow}

//context
fog {distance 200 Wheat*.5}
plane {y,0 pigment {rgb <.9,.9,1>*1.2}}
plane {x,-10 pigment {rgb <.9,.9,1>*1.2}}
box {<45,0,-.1>,<-150,40,.1> pigment {SteelBlue}}
