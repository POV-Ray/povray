// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: radiosity.pov
// Desc: radiosity demo scene
// Date: August 2001
// Auth: Christoph Hormann

// -w320 -h240
// -w512 -h384 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
  
  radiosity {             // --- Settings 1 (fast) ---
    pretrace_start 0.08
    pretrace_end   0.02
    count 50
    error_bound 0.5
    recursion_limit 1
  }
  /*
  radiosity {             // --- Settings 2 (medium quality) ---
    pretrace_start 0.08
    pretrace_end   0.01
    count 120
    error_bound 0.25
    recursion_limit 1
  }*/
  /*
  radiosity {             // --- Settings 3 (high quality) ---
    pretrace_start 0.08
    pretrace_end   0.005
    count 400
    error_bound 0.1
    recursion_limit 1
  }*/
  /*
  radiosity {             // --- Settings 4 (medium quality, recursion_limit 2) ---
    pretrace_start 0.08
    pretrace_end   0.005
    count 350
    error_bound 0.15
    recursion_limit 2
  }*/
}

fog {
  fog_type 2
  fog_alt 1.3
  fog_offset 0
  color rgb <0.7, 0.8, 0.9>
  distance 800
}

// ===================================================


camera {                  // --- Camera 1 ---
  location    <7.5, 1.3, 6.5>
  right     x*image_width/image_height
  look_at     <0, 1.3, 0>
  angle       45
}

/*
camera {                  // --- Camera 2 ---
  ultra_wide_angle
  location    <7.5, 1.3, 6.5>
  right       (4/3)*x
  look_at     <0, 1.3, 0>
  angle       126
}*/

// ===================================================

// --- Three alternative light sources,
// --- also try without light


light_source {            // --- Light 1 ---
  <-5000, 14000, 15000>
  color rgb <1.0, 0.9, 0.78>*2.3
}

/*
light_source {            // --- Light 2 ---
  <-5000, 4000, 15000>
  color rgb <1.1, 0.93, 0.78>*2.1
}*/

/*
light_source {            // --- Light 3 ---
  <5000, 14000, -15000>
  color rgb <1.0, 0.9, 0.78>*2.3
}*/

// ===================================================

sphere {                  // --- Sky ---
  <0, 0, 0>, 1
  texture {
   pigment {
     gradient y
     color_map {
       [0.0 color rgb < 1.0, 1.0, 1.0 >]
       [0.3 color rgb < 0.5, 0.6, 1.0 >]
     }
   }
   finish { diffuse 0 ambient 1 }
  }
  scale 10000
  hollow on
  no_shadow
}

// ===================================================

#declare Tex_1=
texture {
  pigment {
    color rgb 0.8
  }
  finish {
    ambient 0.0
    diffuse 0.6
    specular 0.1
  }
}

#declare Tex_2=
texture {
  pigment {
    color rgb <0.9, 0.65, 0.3>
  }
  finish {
    ambient 0.0
    diffuse 0.6
  }
}

// ===================================================

plane {
  y, 0
  texture { Tex_2 }
}

#declare Column_Block=
union {
  #declare Cnt=0;
  #while (Cnt < 8.8)
    cylinder { <4.4, 0.2, 4.4-Cnt>, <4.4, 3.6, 4.4-Cnt>, 0.2 }
    #declare Cnt=Cnt+(8.8/3);
  #end
}

#declare Walls=
difference {
  box { <-10, -0.1, -10>, <10, 4, 10> }
  box { < -9,  0.3,  -9>, < 9, 3.5,  9> }
  box { < -4,  0.1,  -4>, < 4, 5,  4> }

  intersection {
    merge {
      cylinder { <0, 0.3, 0>, <0, 0.3, 12>, 2 }
      cylinder { <0, 0.3, 0>, <0, 0.3, 12>, 2 rotate 180*y }
      cylinder { <0, 0.3, 0>, <0, 0.3, 12>, 2 rotate  90*y }
      cylinder { <0, 0.3, 0>, <0, 0.3, 12>, 2 rotate -90*y }
    }
    plane { y, 0.3 inverse }
  }
}

#declare Col_Base=
difference {
  box { <-4.7,    0, -4.7>, <4.7, 3.6, 4.7> }
  box { <-4.1, -0.1, -4.1>, <4.1,   4, 4.1> }
  box { < -5,  0.5,  -5>, < 5, 3.3,  5> }
}

union {
  object { Walls }
  object { Column_Block }
  object { Column_Block rotate 180*y }
  object { Column_Block rotate  90*y }
  object { Column_Block rotate -90*y }
  object { Col_Base }
  texture { Tex_1 }
}
