// Insert menu illustration scene
// Created June-August 2001 by Christoph Hormann
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012.

// ----- material -----

// -w120 -h48 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#include "colors.inc"


//#declare Typ=1;     // material
//#declare Typ=2;     // media


global_settings {
  // assumed_gamma 1
  max_trace_level 5
}

light_source {
  <1.5, 0.35, 1.0>*10000
  color rgb 1.0
}

camera {
  location    <22, 20, 7.5>
  direction   y
  sky         z
  up          z
 right    x*image_width/image_height// right       (120/48)*x
  look_at     <0, 0, 0>
  angle       17
}

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [1.0 rgb <0.2,0.2,0.8>]
    }
  }
}

// ----------------------------------------

plane
{
  z, -1
  texture
  {
    pigment {color rgb 1} //checker color rgb 1 color rgb <0,0,0.1> scale 1.1}
    finish {
      diffuse 0.7
      specular 0.4
      roughness 0.01

      reflection { 0.5 , 1.0
        fresnel on
        metallic 0.8
      }
      conserve_energy

    }
  }
}

// =============================================

#if(Typ=1)

sphere {
  <0, 0, 0>, 1.2
  hollow on
  texture { pigment { color rgbt 1 } }
  interior {
    ior 1.6
    dispersion 1.2
  }
  translate 0.3*z
}

sphere {
  <0, 0, 0>, 1.2
  hollow on
  texture {
    pigment {
      agate
      color_map {
        [0.2 rgb 0][0.8 rgb 1]
      }
    }
    finish {
      diffuse 0
      ambient 1
    }
  }
  translate <-3.8, 0, 0.3>
}

sphere {
  <0, 0, 0>, 1.2
  hollow on
  texture {
    pigment {
      color Wheat
    }
    finish { specular 0.5 }
    normal { granite 0.75 scale 1.4 }
  }

  translate <3.6, 0, 0.3>
}

#end

// =============================================

#if(Typ=2)

sphere {
  <0, 0, 0>, 1.2
  hollow on
  texture { pigment { color rgbt 1 } }
  interior {
      media {
         scattering {1,<0.8, 0.3, 0.0> }
         method 3

         density {
           agate
           color_map{
             [0.3 rgb 0 ]
             [0.5 rgb 1 ]
           }
           scale 1
         }
      }
  }
  translate 0.3*z
}

sphere {
  <0, 0, 0>, 1.2
  hollow on
  texture { pigment { color rgbt 1 } }
  interior {
      media {
              absorption rgb 1.0
              method 3

              density {
                agate
                color_map{
                  [0.3 rgb 0 ]
                  [0.5 rgb 1 ]
                }
                scale 1
              }
      }
  }

  translate <-3.8, 0, 0.3>
}

sphere {
  <0, 0, 0>, 1.2
  hollow on
  texture { pigment { color rgbt 1 } }
  interior {
      media {
              emission rgb 1.0
              method 3

              density {
                agate
                color_map{
                  [0.4 rgb 0 ]
                  [0.8 rgb <0.4, 0.4, 0.0> ]
                  [1.0 rgb 1 ]
                }
                scale 1
              }
      }
  }

  translate <3.6, 0, 0.3>
}

#end

// =============================================

