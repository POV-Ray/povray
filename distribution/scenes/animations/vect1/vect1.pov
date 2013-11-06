// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Chris Young
// Demonstrates various new vector math functions.
// Animate this scene with clock values +k0.0 to +k1.0

#version 3.7;

global_settings {
  assumed_gamma 1.0
  }

#include "colors.inc"

#declare Font="cyrvetic.ttf"

// Basic clock runs from 0.0 to 1.0 but we want to move more
//  than that.  Define a scaled version.

#declare Clock360 = 360*clock;
#declare ClockRot = Clock360*z;


// An object that rotates one full circle of 360 degrees
#declare Arm =
 union{
   cylinder{0,3*x,.3}
   sphere{0,.5}
   sphere{3*x,.5}
   pigment{color rgb<1,0.0,0>}
   rotate ClockRot
 }

// A point on the object that is rotating
#declare Attach_Point=vrotate(x*3,ClockRot);

// A point where we will anchor the push rod
#declare Fixed_Point =x*8;

// This rod runs from the Attach_Point to the Fixed_Point.
// It varies in length as the Arm rotates.
#declare Long_Rod=
 union{
   sphere{Attach_Point,.2}
   cylinder {Attach_Point,Fixed_Point,0.2 }
   pigment{color rgb<0.3,0.9,0>}
 }

// Use the vlength function to compute the length.
#declare Long_Length=vlength(Attach_Point - Fixed_Point);

// We want a fixed length short, fat rod that follows the same angle
// as the long rod.  Compute a unit vector that is parallel to
// the long rod.

#declare Normalized_Point = vnormalize(Attach_Point-Fixed_Point);

#declare Short_Length=4;

#declare Short_Rod=
  union{
    sphere{0,.5}
    cylinder {0,Short_Length*Normalized_Point,0.5}
    translate Fixed_Point  // move into place
    pigment{color rgb<0.15,0.1,0.6>}
  }

union {
  object{Arm}
  union {
    object{Long_Rod}
    object{Short_Rod}
    translate -z/2
  }
  translate -x*4
}

text{ttf Font concat("Angle=",str(Clock360,1,1)),0.1,0 scale 2 pigment{color rgb<1,0.1,0>} translate <-3.5,3.5,0>}
text{ttf Font concat("Length=",str(Long_Length,1,1)),0.1,0 scale 2 pigment{color rgb<0.3,0.9,0>} translate <-3.5,-5,0>}

camera {
   location  <0, 0, -120>
  right     x*image_width/image_height
   direction <0, 0.5,  10>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { -z, -1/3 pigment {checker color rgb <1,1,1>*1.2 color rgb <1,1,.8>} }

