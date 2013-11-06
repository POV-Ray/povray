// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: slopemt_dat.pov
// Date: August 30 2001
// Auth: Rune S. Johansen
// Desc: Render this file and then render SLOPEMT.POV.
// 
// +w400 +h400

#version 3.7;

global_settings {
  assumed_gamma 2.2
  noise_generator 1
  hf_gray_16
}

camera {
   location 22*y
   up y
   right x
   look_at 0
}

light_source {y, color 1}

plane {
   y, 0
   texture {
      pigment {
         wrinkles
         scale 0.8
         color_map {
            [0.0, color rgb 0.0]
            [1.0, color rgb 1.6]
         }
      }
      finish {ambient 0 diffuse 1}
   }
   texture {
      pigment {
         spherical translate -0.2*x
         color_map {
            [0.0, color rgb 1 transmit 1.0]
            [0.3, color rgb 1 transmit 0.3]
            [1.0, color rgb 1 transmit 0.0]
         }
      }
      finish {ambient 0 diffuse 1}
   }
}
