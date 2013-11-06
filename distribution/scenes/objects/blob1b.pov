// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// File by Alexander Enzmann (modified by Dieter Bayer)
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 } 

camera {
  location  <0, 8, -10>
  right     x*image_width/image_height
  look_at   <0, 0, -1>
  angle 46
}

light_source { <20, 30, -20> color rgb< 1,1,1> }

blob {
  threshold 0.5
  cylinder { <-7, 0, -3>, <7, 0, -3>, 0.8, 1 }
  cylinder { <-7, 0, -2>, <7, 0, -2>, 0.8, 1 }
  cylinder { <-7, 0, -1>, <7, 0, -1>, 0.8, 1 }
  cylinder { <-7, 0,  0>, <7, 0,  0>, 0.8, 1 }
  cylinder { <-7, 0,  1>, <7, 0,  1>, 0.8, 1 }
  cylinder { <-7, 0,  2>, <7, 0,  2>, 0.8, 1 }
  cylinder { <-7, 0,  3>, <7, 0,  3>, 0.8, 1 }

  sphere { <-1, 1,  1>, 1.5, 1 }
  sphere { < 1, 1,  1>, 1.5, 1 }
  sphere { <-1, 1, -1>, 1.5, 1 }
  sphere { < 1, 1, -1>, 1.5, 1 }
  
  scale<1,1,1.5>
  pigment { color red 1 green 0 blue 0 }
  finish { ambient 0.1 diffuse 0.7 phong 1 }
}
