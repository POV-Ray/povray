// Persistence Of Vision raytracer version 3.1 sample file.
// Omimax camera example
// File by Dieter Bayer

global_settings { assumed_gamma 2.2 }

#include "colors.inc"

// camera used for omnimax

camera {
  omnimax 
  location <0, 2, -20>
  right <4/3, 0, 0>  
  up <0, 1, 0>         
  direction <0, 0, 1> 
  look_at <0, 2, 10>
}

background { color red 0.078 green 0.361 blue 0.753 }

light_source { <100, 100, -100> color Gray60 }

light_source { <-100, 100, -100> color Gray60 }

#declare My_Texture_1 =
texture {
  pigment {
    color red 1 green 0.75 blue 0.33
  }
  finish {
    diffuse 1
    phong 0
    phong_size 0
    reflection 0
  }
}

triangle { <50, -4, 50> <-50, -4, 50> <-50, -4, -50> texture { My_Texture_1 } }
triangle { <50, -4, 50> <-50, -4, -50> <50, -4, -50> texture { My_Texture_1 } }

#declare My_Texture_2 =
texture {
  pigment {
    color red 1 green 0.9 blue 0.7
  }
  finish {
    diffuse 0.5
    phong 0.5
    phong_size 3
    reflection 0.5
  }
}

/* red */

#declare My_Texture_3 =
texture {
  pigment {
    color red 1 green 0 blue 0
  }
  finish {
    diffuse 0.5
    phong 0.5
    phong_size 3
    reflection 0.5
  }
}

/* green */

#declare My_Texture_4 =
texture {
  pigment {
    color red 0 green 1 blue 0
  }
  finish {
    diffuse 0.5
    phong 0.5
    phong_size 3
    reflection 0.5
  }
}

/* blue */

#declare My_Texture_5 =
texture {
  pigment {
    color red 0 green 0 blue 1
  }
  finish {
    diffuse 0.5
    phong 0.5
    phong_size 3
    reflection 0.5
  }
}

/* yellow */

#declare My_Texture_6 =
texture {
  pigment {
    color red 1 green 1 blue 0
  }
  finish {
    diffuse 0.5
    phong 0.5
    phong_size 3
    reflection 0.5
  }
}

sphere { <+10, 0, +10>, 4 texture { My_Texture_3 } }

sphere { <-10, 0, -10>, 4 texture { My_Texture_6 } }

sphere { <+10, 0, -10>, 4 texture { My_Texture_5 } }

sphere { <-10, 0, +10>, 4 texture { My_Texture_4 } }


sphere { <-10, 20, -10>, 4 texture { My_Texture_6 } }

sphere { <+10, 20, -10>, 4 texture { My_Texture_6 } }

sphere { <-10, 20, +10>, 4 texture { My_Texture_6 } }

sphere { <+10, 20, +10>, 4 texture { My_Texture_6 } }

cylinder { <-10, 0, -10>, <+10, 0, -10>, 2 texture { My_Texture_2 } }

cylinder { <+10, 0, -10>, <+10, 0, +10>, 2 texture { My_Texture_2 } }

cylinder { <+10, 0, +10>, <-10, 0, +10>, 2 texture { My_Texture_2 } }

cylinder { <-10, 0, +10>, <-10, 0, -10>, 2 texture { My_Texture_2 } }

cylinder { <-10, 20, -10>, <+10, 20, -10>, 2 texture { My_Texture_2 } }

cylinder { <+10, 20, -10>, <+10, 20, +10>, 2 texture { My_Texture_2 } }

cylinder { <+10, 20, +10>, <-10, 20, +10>, 2 texture { My_Texture_2 } }

cylinder { <-10, 20, +10>, <-10, 20, -10>, 2 texture { My_Texture_2 } }

cylinder { <-10, 0, -10>, <-10, 20, -10>, 2 texture { My_Texture_2 } }

cylinder { <-10, 0, +10>, <-10, 20, +10>, 2 texture { My_Texture_2 } }

cylinder { <+10, 0, +10>, <+10, 20, +10>, 2 texture { My_Texture_2 } }

cylinder { <+10, 0, -10>, <+10, 20, -10>, 2 texture { My_Texture_2 } }

