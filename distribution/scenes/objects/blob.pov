// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// File by Alexander Enzmann
//
// -w320 -h240
// -w800 -h600 +a0.3
//
#version 3.7;
global_settings { assumed_gamma 1.0 }

camera {
   location  <0, 0, -5>
   right     x*image_width/image_height
   direction <0, 0, 1.7>
   look_at   <0, 0, 0>
}

background { color rgb<1,1,1>*0.02 } 

light_source { <-15, 30, -25> color red 1 green 1 blue 1 }
light_source { < 15, 30, -25> color red 1 green 1 blue 1 }

blob {
   threshold 0.6
   component 1.0, 1.0, <0.75, 0, 0>
   component 1.0, 1.0, <-0.375, 0.64952, 0>
   component 1.0, 1.0, <-0.375, -0.64952, 0>

   pigment { color rgb< 0.7,0,0.05> }
   finish {
      ambient 0.1
      diffuse 0.7
      phong 1
   }
   rotate 30*y
}
