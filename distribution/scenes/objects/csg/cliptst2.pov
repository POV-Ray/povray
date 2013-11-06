// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Clipped_by example
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7;
global_settings { assumed_gamma 1.7 }

#include "colors.inc"           // Standard colors library
#include "textures.inc"         // LOTS of neat textures.  Lots of NEW textures.

camera {
   location <2.0, 4, -4>
   angle 50
   right     x*image_width/image_height
   look_at <0, 0, 0>
}

background{ color rgb<1,1,1>*0.1 } 


// Light source
#declare Grayscale = 0.25;
#declare AmbientLight = color red Grayscale green Grayscale blue Grayscale;

light_source { <-20, 30, -100> color White }

light_source { <50, 50 ,15> color AmbientLight }


// A hollow sphere using a clipping plane.  This sphere has no "thickness"
// to its walls,  no matter what the scaling.
sphere { <0, 0, 0>, 1
   clipped_by { plane { <0, 1, 0>, 0.25 } }

   finish {
      Phong_Glossy
      ambient 0.2
   }

   pigment {
      gradient <1, 1, 1>
      // Notice the -1 to +1 color range.  This is just to demonstrate
      // that this is a valid form for a color map.  It's not really
      // neccessary in this case to do it this way, but thought I'd
      // use it for instructional value.
      color_map {
         [-1.0 0.0 color Yellow color Cyan ]
         [ 0.0 1.0 color Cyan color Magenta]
      }
      scale <0.1, 0.1, 0.1>
   }

   translate <-2, 1, 0>
}

// Hollow sphere done with intersection.
intersection {
   sphere { <0, 0, 0>, 1 }                // outer wall
   sphere { <0, 0, 0>, 0.85 inverse }     // inner wall
   plane { <0, 1, 0>, 0.25 }              // top surface

   finish {
      Phong_Glossy
      ambient 0.2
   }
   pigment {
      leopard
      color_map {
         [0.0   0.10 color Yellow color Red ]
         [0.10  0.98 color Red color Blue ]
         [0.98  1.00 color Magenta color Yellow  ]
      }
      //        scale <0.05, 1.0, 0.05>
      scale <0.025, 1.0, 0.025>
   }

   translate <2, 1, 0>
}

// Flat-topped sphere/plane intersection
intersection {
   sphere { <0, 0, 0>, 1 }               // outer wall
   plane { <0, 1, 0>, 0.25 }             // top surface

   finish {
      Phong_Glossy
      ambient 0.2
   }
   pigment {
      onion
      turbulence 10                      // try with 0 turb, too!
      octaves 2
      color_map {
         [0.0   0.30 color Yellow color Orange ]
         [0.30  0.90 color Orange color Magenta ]
         [0.90  1.00 color Blue color Green    ]
      }
   }

   translate <0, 1, -1>
}
