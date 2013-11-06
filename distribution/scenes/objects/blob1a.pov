// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// File by Alexander Enzmann (modified by Dieter Bayer)
//
// -w320 -h240
// -w800 -h600 +a0.3
//
#version 3.7;
global_settings { assumed_gamma 1.0 }

camera {
  location  <0, 8, -15>
  right     x*image_width/image_height
  look_at   <0, 0, 0>
  angle 46
}

light_source { <10, 30, -20> color red 1 green 1 blue 1 }

background { color rgb<1,1,1>*0.02 } 

blob {
  threshold 0.5
  cylinder { <-7, 0, 0>, <7, 0, 0>, 4, 2 }
  cylinder { <0, 0, -7>, <0, 0, 7>, 4, 2 }
  sphere { <0, 3, 0>, 2.5, -4 }

  pigment { color red 1 green 0 blue 0 }
  finish { ambient 0.1 diffuse 0.7 phong 1 }

  rotate <-30, 0, 0>
}

