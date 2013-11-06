// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: splinefollow.pov
// Desc: Spline demo animation that shows how to make an object or
//       the camera fly along a spline. This is a cyclic animation.
// Date: August 30 2001
// Auth: Rune S. Johansen 

// Use these command line settings to view the animation
// in REGULAR MODE:
// 
// +kf0.1666 +kff20 +kc declare=fp=0

// Use these command line settings to view the animation
// in FIRST PERSON MODE:
// 
// +kff120 +kc declare=fp=1

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "math.inc"
#include "transforms.inc"

// #declare FirstPerson = yes;

#ifndef(FirstPerson)
   #ifdef(fp)
      #declare FirstPerson = fp;
   #else
      #declare FirstPerson = no;
   #end 
#end

// Overview camera
#if (FirstPerson=no)
   camera {
      location <2,12-2,-10+2>
      right     x*image_width/image_height
      look_at  <0,2,3>
   }
#end

sky_sphere {
   pigment {
      planar poly_wave 2
      color_map {
         [0.0, color <0.2,0.5,1.0>]
         [1.0, color <0.8,0.9,1.0>]
      }
   }
}

light_source {< 1,2,-2>*1000, color 1.0}
light_source {<-1,2, 1>*1000, color 0.7 shadowless}

plane { // checkered plane
   y, 0
   pigment {checker color rgb 1.0, color rgb 0.9 scale 2}
}
cylinder { // start/stop location
   0, y, 2
   pigment {color rgb 0.7}
}
torus { // yellow ring
   1.3, 0.3
   pigment {color <1,1,0>}
   rotate 90*x rotate 45*y
   translate <5,3,0>
}
cylinder { // green pole
   0, 7*y, 0.4
   pigment {color <0,1,0>}
   translate 7*z
}
torus { // blue ring
   1.3, 0.3
   pigment {color <0,0,1>}
   rotate 90*x rotate -45*y
   translate <-5,3,0>
}

// The spline that the aircracfts fly along
#declare MySpline =
spline {
   cubic_spline
   
   -2, <-5, 3, 0>, // control point
   -1, <-2, 2, 0>, // control point
   
   00, < 0, 2, 0>, // start
   01, < 2, 2, 0>,
   02, < 5, 3, 0>, // through yellow ring
   03, < 5, 4, 4>,
   04, < 0, 5, 5>, // around
   05, <-2, 4, 9>, // the
   06, < 2, 3, 9>, // green
   07, < 0, 2, 5>, // pole
   08, <-5, 2, 4>,
   09, <-5, 3, 0>, // through blue ring
   10, <-2, 2, 0>,
   11, < 0, 2, 0>, // stop
   
   12, < 2, 2, 0>, // control point
   13, < 5, 3, 0>, // control point
}

// The aircraft object
#declare Aircraft =
union {
   #declare Part =
   union {
      cone {-1.0*z, 0.7, -0.7*z, 1.0}
      cone {-0.7*z, 1.0,  2.0*z, 0.3}
   }
   object {Part scale <0.5,0.4,0.7>}
   object {Part scale <+0.25,0.3,0.3> translate <+0.6,-0.1,-0.3>}
   object {Part scale <-0.25,0.3,0.3> translate <-0.6,-0.1,-0.3>}
   sphere {
      0, 1 scale <0.3,0.25,0.5> rotate 12*x translate <0,0.22,0.15>
      pigment {color <0,1,1>}
      finish {phong 0.3 phong_size 10}
   }
   pigment {color <1.0,0.3,0.3>}
   finish {brilliance 2 phong 0.3}
}

// The Spline_Trans macro has the following parameters:
// Spline_Trans (Spline, Time, SkyVector, ForeSight, Banking)

// Make 6 aircrafts fly along the spline.
// the mod() function is used for the Time value to make it cycle
// through the spline. The time is then multiplied with 11 to make
// it match the time values specified in the spline.
object { Aircraft Spline_Trans (MySpline, mod( (clock+0/6) ,1)*11, y, 0.5, 0.5) }
object { Aircraft Spline_Trans (MySpline, mod( (clock+1/6) ,1)*11, y, 0.5, 0.5) }
object { Aircraft Spline_Trans (MySpline, mod( (clock+2/6) ,1)*11, y, 0.5, 0.5) }
object { Aircraft Spline_Trans (MySpline, mod( (clock+3/6) ,1)*11, y, 0.5, 0.5) }
object { Aircraft Spline_Trans (MySpline, mod( (clock+4/6) ,1)*11, y, 0.5, 0.5) }
object { Aircraft Spline_Trans (MySpline, mod( (clock+5/6) ,1)*11, y, 0.5, 0.5) }

// First-person-view camera
// Follows the same path as the first aircraft
#if (FirstPerson=yes)
   camera {
      location 0
      look_at z
      translate <0,0.4,0.4>
      Spline_Trans (MySpline, clock*11, y, 0.5, 0.5)
   }
#end

// The yellow wire that shows the spline path.
union {
   #declare C = 0;
   #declare Cmax= 50;
   #while (C<=Cmax)
      #declare Value1 = C/Cmax*11;
      #declare Value2 = (C+1)/Cmax*11;
      #declare Point1 = -0.5*y+MySpline(Value1);
      #declare Point2 = -0.5*y+MySpline(Value2);
      sphere {Point1, 0.1}
      cylinder {Point1, Point2, 0.1}
      #declare C = C+1;
   #end
   pigment {color <1,1,0>}
}
