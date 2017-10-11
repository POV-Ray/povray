// ----------------------------------------------------------------------------
// ------------ STONE TEXTURE DESCRIPTIONS ------------------------------------
// ----------------------------------------------------------------------------

#declare Cloud = texture {
  pigment { colour rgbt <1, 1, 1, 1>  }
  halo {
    linear
    max_value 1
    spherical_mapping
    attenuating
    turbulence 0.7
    colour_map {
      [0.0 color rgbt <1, 1, 1, 1> ]
      [1.0 color rgbt <0.3,0.3,0.3,0> ]
    }
    scale 0.7
    samples 12
    aa_level 0
    aa_threshold 0.3
  }
}

#declare Cloud2 = texture {
  pigment { colour rgbt <1, 1, 1, 1> }
  halo {
    linear
    max_value 0.1
    spherical_mapping
    attenuating
    turbulence 0.75
    octaves 6
    omega (1/1.618)
    lambda 1.618
    colour_map {
      [0.0 color rgbt <1, 1, 1, 1>]
      [0.5 color rgbt <1, 1, 1, 0.65>]
      [1.0 color rgbt <0, 0, 0, 0>]
    }
    scale <0.9, 0.4, 0.5>
    samples 10
    aa_level 0
    aa_threshold 0.3
  }
}

#declare Red_Marble_1 = texture { 
pigment {
  marble
  turbulence 1
  color_map{
   [0.0, 0.8   color rgb <0.8, 0.8, 0.6>
               color rgb <0.8, 0.4, 0.4>]
   [0.8, 1.001 color rgb <0.8, 0.4, 0.4>
               color rgb <0.8, 0.2, 0.2>]
  }
}}

#declare Blue_Agate = texture { 
pigment {
  agate
  color_map {
    [0.0, 0.5   color rgb <0.30, 0.30, 0.50>
                color rgb <0.30, 0.30, 0.50>]
    [0.5, 0.55  color rgb <0.30, 0.30, 0.50>
                color rgb <0.20, 0.20, 0.30>]
    [0.55, 0.6  color rgb <0.20, 0.20, 0.30>
                color rgb <0.25, 0.25, 0.35>]
    [0.6, 0.7   color rgb <0.25, 0.25, 0.35>
                color rgb <0.15, 0.15, 0.26>]
    [0.7, 0.8   color rgb <0.15, 0.15, 0.26>
                color rgb <0.10, 0.10, 0.20>]
    [0.8, 0.9   color rgb <0.10, 0.10, 0.20>
                color rgb <0.30, 0.30, 0.50>]
    [0.9, 1.001 color rgb <0.30, 0.30, 0.50>
                color rgb <0.10, 0.10, 0.20>]
  }
}}

#declare White_Bleu_Agate = texture {
  pigment {
    agate
    color_map {
      [0.0, 0.5   color rgb <0.10, 0.10, 0.50>
                  color rgb <1.00, 1.00, 1.00>]
      [0.5, 1.001 color rgb <1.00, 1.00, 1.00>
                  color rgb <0.10, 0.10, 0.50>]
    }
  }
  finish {
    reflection 0.2
    phong 1.0
  }
  scale 2
}

#declare Saphirre_Agate = texture {pigment {
  agate
  color_map {
    [0.0, 0.3   color rgb <0, 0, 0.9>
                color rgb <0, 0, 0.8>]
    [0.3, 1.001 color rgb <0, 0, 0.8>
                color rgb <0, 0, 0.4>]
  }
}}

#declare White_Maroon_Agate_2 = texture {pigment {
  agate
  color_map {
    [0.0, 0.5   color rgb <1, 1, 1>
                color rgb <0.9, 0.7, 0.6>]
    [0.5, 0.6   color rgb <0.9, 0.7, 0.6>
                color rgb <0.9, 0.7, 0.4>]
    [0.6, 1.001 color rgb <0.9, 0.7, 0.4>
                color rgb <0.7, 0.4, 0.2>]
  }
}}

#declare Pink_Alabaster = texture {
  pigment {
    bozo
    turbulence 0.25
    color_map {
      [0,  1 color rgb <0.9, 0.75, 0.75>
             color rgb <0.6, 0.6, 0.6>]
    }
    scale 0.4
  }
  finish {
    ambient 0.25
  }
}
texture {
  pigment {
    granite
    color_map {
       [0,   0.9 color rgbf <0.52, 0.39, 0.39, 1>
                 color rgbf <0.52, 0.39, 0.39, 0.5>]
       [0.9, 1   color rgb <0.184314, 0.309804, 0.309804>
                 color rgb <0.184314, 0.309804, 0.309804>]
    }
    scale 0.5    // Twice as large as unit shape
  }
  finish {
    specular 1
    roughness 0.0001
    phong 0.25
    phong_size 75
    brilliance 4
  }
}

// ----------------------------------------------------------------------------
// ------------ SKIES TEXTURES ------------------------------------------------
// ----------------------------------------------------------------------------

#declare Blue_Sky = texture {
pigment {
  bozo
  turbulence 0.3
  color_map {
    [0.0, 0.5   color rgb <0.25, 0.25, 0.5>
                color rgb <0.25, 0.25, 0.5>]
    [0.5, 0.6   color rgb <0.25, 0.25, 0.5>
                color rgb <0.7,  0.7,  0.7>]
    [0.6, 1.001 color rgb <0.7,  0.7,  0.7>
                color rgb <0.3,  0.3,  0.3>]
  }
}}

#declare Bright_Blue_Sky = texture {
pigment {
  bozo
  turbulence 0.56
  color_map {
    [0.0, 0.5  color rgb <0.5, 0.5, 1.0>
               color rgb <0.5, 0.5, 1.0>]
    [0.5, 0.6  color rgb <0.5, 0.5, 1.0>
               color rgb <1.0, 1.0, 1.0>]
    [0.6, 1.01 color rgb <1.0, 1.0, 1.0>
               color rgb <0.5, 0.5, 0.5>]
  }
}}

#declare Blue_Sky_2 = texture {
pigment {
  agate
  color_map {
    [ 0, .3  color rgb <0, 0, 1>
             color rgb <0, 0, 1>]
    [.3,  1  color rgb <0, 0, 1>
             color rgb <1, 1, 1>]
  }
  scale 0.75
}}

#declare Blue_Sky_3 = texture {
pigment {
  granite
  turbulence 0.1
  color_map {
    [ 0, .3  color rgb <0, 0, 1>
             color rgb <0, 0, 1>]
    [.3,  1  color rgb <0, 0, 1>
             color rgb <1, 1, 1>]
  }
  scale 0.75
}}

#declare Apocalypse_Sky = texture {
pigment {
  bozo
  color_map {
    [0.0, 0.4  color rgb <0.8, 0, 0>
               color rgb <0.4, 0, 0.4>]
    [0.4, 0.6  color rgb <0.4, 0, 0.4>
               color rgb <0, 0, 0.2>]
    [0.6, 1.01 color rgb <0, 0, 0.2>
               color rgb <0, 0, 0>]
  }
}}

#declare Just_Clouds = texture {
pigment {
  bozo
  turbulence 0
  color_map {
    [0.0, 0.1   color rgb <0.8, 0.8, 0.8>
                color rgb <0.8, 0.8, 0.8>]
    [0.1, 0.5   color rgb <0.8, 0.8, 0.8>
                color rgbf <1.0, 1.0, 1.0, 1.0>]
    [0.5, 1.001 color rgbf <1.0, 1.0, 1.0, 1.0>
                color rgbf <1.0, 1.0, 1.0, 1.0>]
  }
}}

// ----------------------------------------------------------------------------
// ------------ WOODEN TEXTURES -----------------------------------------------
// ----------------------------------------------------------------------------

#declare Cherry_Wood = texture {
pigment {
  wood
  turbulence 0.3
  color_map {
    [0.0, 0.8  color rgb <0.666, 0.312, 0.2>
               color rgb <0.666, 0.312, 0.2>]
    [0.8, 1.01 color rgb <0.4, 0.133, 0.066>
               color rgb <0.2, 0.065, 0.033>]
  }
}}

#declare Pine_Wood = texture {
pigment {
  wood
  turbulence 0.2
  color_map {
    [0.0, 0.8  color rgb <1.0, 0.71875, 0.25>
               color rgb <1.0, 0.71875, 0.25>]
    [0.8, 1.01 color rgb <0.5, 0.5, 0.066>
               color rgb <0.4, 0.4, 0.033>]
  }
}}

#declare Dark_Wood = texture {
pigment {
  wood
  turbulence 0.2
  color_map {
    [0.0, 0.8  color rgb <0.42857, 0.23810, 0.04762>
               color rgb <0.42857, 0.23810, 0.04762>]
    [0.8, 1.01 color rgb <0.4, 0.333, 0.066>
               color rgb <0.2, 0.033, 0.033>]
  }
}}

#declare Tan_Wood = texture {
pigment {
  wood
  turbulence 0.1
  color_map {
    [0.0, 0.8  color rgb <0.888, 0.600, 0.3>
               color rgb <0.888, 0.600, 0.3>]
    [0.8, 1.01 color rgb <0.6, 0.4, 0.2>
               color rgb <0.4, 0.3, 0.2>]
  }
}}

#declare White_Wood = texture {
pigment {
  wood
  turbulence 0.6
  color_map {
    [0.0, 0.8  color rgb <0.93, 0.71, 0.532>
               color rgb <0.98, 0.81, 0.6>]
    [0.8, 1.01 color rgb <0.6, 0.333, 0.266>
               color rgb <0.7, 0.6, 0.23>]
  }
}}

#declare Tom_Wood = texture {
pigment {
  wood
  turbulence 0.31
  color_map {
    [0.0, 0.8   color rgb <0.7, 0.3, 0>
                color rgb <0.7, 0.3, 0>]
    [0.8, 1.01  color rgb <0.5, 0.2, 0>
                color rgb <0.4, 0.1, 0>]
  }
}}

#declare Yellow_Pine = texture {
 pigment {
   wood
   turbulence 0.02
   color_map {
      [0.000, 0.222  color rgbf <0.808, 0.671, 0.251, 0.000>
                     color rgbf <0.808, 0.671, 0.251, 0.000>]
      [0.222, 0.342  color rgbf <0.808, 0.671, 0.251, 0.000>
                     color rgbf <0.600, 0.349, 0.043, 0.000>]
      [0.342, 0.393  color rgbf <0.600, 0.349, 0.043, 0.000>
                     color rgbf <0.808, 0.671, 0.251, 0.000>]
      [0.393, 0.709  color rgbf <0.808, 0.671, 0.251, 0.000>
                     color rgbf <0.808, 0.671, 0.251, 0.000>]
      [0.709, 0.821  color rgbf <0.808, 0.671, 0.251, 0.000>
                     color rgbf <0.533, 0.298, 0.027, 0.000>]
      [0.821, 1      color rgbf <0.533, 0.298, 0.027, 0.000>
                     color rgbf <0.808, 0.671, 0.251, 0.000>]
   }
   scale 0.1
   translate <10, 0, 0>
   }
}
// Yellow_Pine layer 2
texture {
 pigment {
   wood
   turbulence 0.01
   color_map {
      [0.000, 0.120   color rgbf <1.000, 1.000, 1.000, 1.000>
                      color rgbf <0.702, 0.412, 0.118, 0.608>]
      [0.120, 0.231   color rgbf <0.702, 0.412, 0.118, 0.608>
                      color rgbf <0.702, 0.467, 0.118, 0.608>]
      [0.231, 0.496   color rgbf <0.702, 0.467, 0.118, 0.608>
                      color rgbf <1.000, 1.000, 1.000, 1.000>]
      [0.496, 0.701   color rgbf <1.000, 1.000, 1.000, 1.000>
                      color rgbf <1.000, 1.000, 1.000, 1.000>]
      [0.701, 0.829   color rgbf <1.000, 1.000, 1.000, 1.000>
                      color rgbf <0.702, 0.467, 0.118, 0.608>]
      [0.829, 1       color rgbf <0.702, 0.467, 0.118, 0.608>
                      color rgbf <1.000, 1.000, 1.000, 1.000>]
   }
   scale 0.5
   translate <10, 0, 0>
 }
}

//
//   Rosewood
//
#declare Rosewood = texture {
 pigment {
   bozo
   turbulence 0.04
   color_map {
      [0.000, 0.256   color rgbf <0.204, 0.110, 0.078, 0.000>
                      color rgbf <0.231, 0.125, 0.090, 0.000>]
      [0.256, 0.393   color rgbf <0.231, 0.125, 0.090, 0.000>
                      color rgbf <0.247, 0.133, 0.090, 0.000>]
      [0.393, 0.581   color rgbf <0.247, 0.133, 0.090, 0.000>
                      color rgbf <0.204, 0.110, 0.075, 0.000>]
      [0.581, 0.726   color rgbf <0.204, 0.110, 0.075, 0.000>
                      color rgbf <0.259, 0.122, 0.102, 0.000>]
      [0.726, 0.983   color rgbf <0.259, 0.122, 0.102, 0.000>
                      color rgbf <0.231, 0.125, 0.086, 0.000>]
      [0.983, 1       color rgbf <0.231, 0.125, 0.086, 0.000>
                      color rgbf <0.204, 0.110, 0.078, 0.000>]
   }
   scale <0.5, 0.5, 1>
   translate <10, 0, 0>
 }
 finish {
   ambient 0.5
   diffuse 0.8
 }
}
// Rosewood layer 2
texture {
 pigment {
   wood
   turbulence 0.04
   color_map {
      [0.000, 0.139   color rgbf <0.545, 0.349, 0.247, 1.000>
                      color rgbf <0.000, 0.000, 0.000, 0.004>]
      [0.139, 0.148   color rgbf <0.000, 0.000, 0.000, 0.004>
                      color rgbf <0.000, 0.000, 0.000, 0.004>]
      [0.148, 0.287   color rgbf <0.000, 0.000, 0.000, 0.004>
                      color rgbf <0.545, 0.349, 0.247, 1.000>]
      [0.287, 0.443   color rgbf <0.545, 0.349, 0.247, 1.000>
                      color rgbf <0.545, 0.349, 0.247, 1.000>]
      [0.443, 0.626   color rgbf <0.545, 0.349, 0.247, 1.000>
                      color rgbf <0.000, 0.000, 0.000, 0.004>]
      [0.626, 0.635   color rgbf <0.000, 0.000, 0.000, 0.004>
                      color rgbf <0.000, 0.000, 0.000, 0.004>]
      [0.635, 0.843   color rgbf <0.000, 0.000, 0.000, 0.004>
                      color rgbf <0.545, 0.349, 0.247, 1.000>]
      [0.843, 1       color rgbf <0.545, 0.349, 0.247, 1.000>
                      color rgbf <0.545, 0.349, 0.247, 1.000>]
   }
   scale <0.5, 0.5, 1>
   translate <10, 0, 0>
 }
 finish {
   ambient 0.5
   diffuse 0.8
 }
}

//
//   Sandalwood ( makes a great burled maple, too)
//
#declare Sandalwood = texture {
 pigment {
   bozo
   turbulence 0.2
   color_map {
      [0.000, 0.171   color rgbf <0.725, 0.659, 0.455, 0.000>
                      color rgbf <0.682, 0.549, 0.420, 0.000>]
      [0.171, 0.274   color rgbf <0.682, 0.549, 0.420, 0.000>
                      color rgbf <0.557, 0.451, 0.322, 0.000>]
      [0.274, 0.393   color rgbf <0.557, 0.451, 0.322, 0.000>
                      color rgbf <0.725, 0.659, 0.455, 0.000>]
      [0.393, 0.564   color rgbf <0.725, 0.659, 0.455, 0.000>
                      color rgbf <0.682, 0.549, 0.420, 0.000>]
      [0.564, 0.701   color rgbf <0.682, 0.549, 0.420, 0.000>
                      color rgbf <0.482, 0.392, 0.278, 0.000>]
      [0.701, 1       color rgbf <0.482, 0.392, 0.278, 0.000>
                      color rgbf <0.725, 0.659, 0.455, 0.000>]
   }
   scale <0.2, 0.2, 1>
   scale 2
 }
}
// Sandalwood layer 2
texture {
 pigment {
   bozo
   turbulence 0.8
   color_map {
      [0.000, 0.087   color rgbf <0.682, 0.604, 0.380, 1.000>
                      color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.087, 0.226   color rgbf <0.761, 0.694, 0.600, 0.020>
                      color rgbf <0.635, 0.553, 0.325, 1.000>]
      [0.226, 0.348   color rgbf <0.635, 0.553, 0.325, 1.000>
                      color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.348, 0.496   color rgbf <0.761, 0.694, 0.600, 0.020>
                      color rgbf <0.682, 0.604, 0.380, 1.000>]
      [0.496, 0.565   color rgbf <0.682, 0.604, 0.380, 1.000>
                      color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.565, 0.661   color rgbf <0.761, 0.694, 0.600, 0.020>
                      color rgbf <0.682, 0.604, 0.380, 1.000>]
      [0.661, 0.835   color rgbf <0.682, 0.604, 0.380, 1.000>
                      color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.835, 1       color rgbf <0.761, 0.694, 0.600, 0.020>
                      color rgbf <0.682, 0.604, 0.380, 1.000>]
   }
   scale 0.2
   scale 2
 }
}

// ----------------------------------------------------------------------------
// ------------ GLASS TEXTURES ------------------------------------------------
// ----------------------------------------------------------------------------

#declare Diamond = texture {
  pigment { color rgbf <1, 1, 1, 0.95> }
  finish {
    reflection 0.3
    refraction 1
    ior 2.471
    diffuse 0.000001
    metallic 1
    ambient 0
    fade_distance 12
    fade_power 1
    specular 1
    roughness 0.001
    caustics 1.0
    irid {
      0.1             // contribution to overall color
      thickness 0.8    // affects frequency, or "busy-ness"
      turbulence 0.1   // Variance in film thickness
    }
  }
}

#declare Plexiglas = texture {
  pigment { color rgbf <1, 1, 1, 1.0> }
  finish {
    ambient 0
    diffuse 0
    reflection 0.5
    refraction 0.85
    ior 1.5
    phong 0.3
    phong_size 60
  }
}

#declare Crystal = texture {
  pigment { color rgbf <0.98, 0.98, 0.98, 0.9> }
  finish {
    ambient 0.1
    diffuse 0.1
    specular 0.8
    reflection 0.1
    refraction 0.9
    ior 1.45
    roughness 0.0003
    phong 1
    phong_size 400
  }
}

#declare Green_Glass = texture {
  pigment { color rgbf <0.8, 1, 0.95, 0.9> }
  finish {
    ambient 0.1
    diffuse 0.1
    specular 0.8
    reflection 0.1
    refraction 0.9
    ior 1.45
    roughness 0.0003
    phong 1
    phong_size 400
  }
}

#declare Soap_Bubbles = texture {
  pigment {
    color rgbf <1,1,1,1.0>
  }
  finish {
    phong 1
    phong_size 100
    reflection .03
    ior 1.001
    refraction 1.0
    irid {
      0.25
      thickness 0.6
      turbulence 0.3
    }
  }
  scale <0.1,0.1,0.1>
}

// ----------------------------------------------------------------------------
// ------------ METAL TEXTURES ------------------------------------------------
// ----------------------------------------------------------------------------

#declare Metal_Chrome_1 = texture {
  pigment { rgb <0, 0, 0> }
  finish {
    ambient 0.3
    diffuse 0.7
    reflection 0.9
    phong 1.0
  }
}

#declare Brass_Metal = texture {
  pigment { color rgb <0.71, 0.65, 0.26> }
  finish {
    metallic
    ambient 0.2
    diffuse 0.7
    brilliance 6
    reflection 0.25
    phong 0.75
    phong_size 80
  }
}

#declare Gold_Metal = texture {
  pigment { color rgb <0.85, 0.85, 0.10> }
  finish {
    metallic
    ambient 0.2
    diffuse 0.7
    brilliance 6
    reflection 0.25
    phong 0.75
    phong_size 80
  }
}

#declare Bronze_Metal = texture {
  pigment { color rgb <0.55, 0.47, 0.14> }
  finish {
    metallic
    ambient 0.2
    diffuse 0.7
    brilliance 6
    reflection 0.25
    phong 0.75
    phong_size 80
  }
}

#declare Copper_Metal   = texture {
  pigment { color rgb <0.72, 0.45, 0.20> }
  finish {
    metallic
    ambient 0.2
    diffuse 0.7
    brilliance 6
    reflection 0.25
    phong 0.75
    phong_size 80
  }
}

#declare Silver_Metal_1 = texture {
  pigment { color rgb <0.90, 0.91, 0.98> }
  finish {
    metallic
    ambient 0.2
    diffuse 0.7
    brilliance 6
    reflection 0.25
    phong 0.75
    phong_size 80
  }
}

#declare Polish_Chrome_Metal = texture {
  pigment { color rgb <0.20, 0.20, 0.20> }
  finish {
    ambient 0.1
    diffuse 0.7
    brilliance 6.0
    reflection 0.6
    phong 0.8
    phong_size 120
  }
}

#declare Polish_Brass_Metal = texture {
  pigment { color rgb <0.578, 0.422, 0.195> }
  finish {
    metallic
    ambient 0.1
    diffuse 0.8
    brilliance 6.0
    reflection 0.4
    phong 0.8
    phong_size 120
  }
}

#declare Military_Brass_Metal = texture {
  pigment { color rgb <0.70, 0.56, 0.37> }
  finish {
    ambient 0.35
    diffuse 1.00
    brilliance 15
    phong 0.41
    phong_size 5
  }
}

#declare Cymbals_Metal = texture {
  pigment { color rgb <0.75, 0.65, 0.35> }
  normal { waves 0.02 scale 0.007 }
  finish {
    phong 1.0
    phong_size 45
    reflection 0.25
  }
}

#declare Silver_Metal_2 = texture {
  pigment { color rgb <0.94, 0.93, 0.83> }
  finish {
    metallic
    ambient 0.25
    diffuse 0.65
    reflection 0.45
    brilliance 6
    phong 1
    phong_size 100
  }
}

// ----------------------------------------------------------------------------
// ------------ FX TEXTURES ---------------------------------------------------
// ----------------------------------------------------------------------------

#declare Candy_Cane = texture {
pigment {
   gradient <1,1,0>
   color_map {
     [0.00, 0.25  color rgb <1, 0, 0>
                  color rgb <1, 0, 0>]
     [0.25, 0.75  color rgb <1, 1, 1>
                  color rgb <1, 1, 1>]
     [0.75, 1.001 color rgb <1, 0, 0>
                  color rgb <1, 0, 0>]
    }
  }}

#declare Peel = texture {
  pigment {
    gradient < 1, 1, 0 >
    color_map {
      [0.00, 0.25  color rgb <1, 0.5, 0.0>
                   color rgb <1, 0.5, 0.0>]
      [0.25, 0.75  color rgbf <1, 1, 1, 1>
                   color rgbf <1, 1, 1, 1>]
      [0.75, 1.001 color rgb <1, 0.5, 0.0>
                   color rgb <1, 0.5, 0.0>]
    }
  }
}

#declare Water = texture {
  pigment {
    color rgbf <0.9, 0.9, 1.0, 0.6>
  }
  normal { bumps 0.4 }
  finish {
    reflection 0.4
    refraction 0.5
    ior 1.33
    phong 1.0
    phong_size 100
  }
  scale <0.2,0.2,0.2>
}

#declare Quiet_Water = texture {
  pigment {
    color rgb <0.0, 0.0,0.3>
  }
  normal { bumps 0.04 }
  finish {
    reflection 0.4
    refraction 0.5
    ior 1.33
    phong 1.0
    phong_size 100
  }
  scale <0.2,0.2,0.2>
}

// ----------------------------------------------------------------------------
// ------------ MY OWN TEXTURES - DENIS OLIVIER -------------------------------
// ----------------------------------------------------------------------------

#declare StarShip1 = texture {
  pigment { color rgb <0.8,0.8,0.8> }
  normal { brick 0.1 }
  finish { reflection 0.4 }
  scale <1,1,1>/100
}

#declare Earth_Clouds = /*texture {
  pigment { // tres haute altitude
    granite
    turbulence 1.5
    color_map {
      [0.0 0.1 color rgbf <1, 1, 1,0.0>
               color rgbf <1, 1, 1,0.4>]
      [0.1 0.2 color rgbf <1, 1, 1,0.4>
               color rgbf <1, 1, 1,1.0>]
      [0.2 1.0 color rgbf <1, 1, 1,1.0>
               color rgbf <1, 1, 1,1.0>]
    }
  }
  scale <1,1,1>
  finish { ambient 0.3 diffuse 1 ambient 0.4 phong 0.3 phong_size 20 }
  rotate <15,45,80>
}*/
texture { // haute altitude
  pigment {
    granite
    color_map {
      [0.00   color rgbf <1.0, 1.0, 1.0, 1.0> ]
      [0.60   color rgbf <0.98, 0.98, 0.98, 1.0> ] // 0.50
      [0.90   color rgbf <0.98, 0.98, 0.98, 0.0> ]
    }
    turbulence 0.75
    octaves 5
    lambda 4
  }
  finish { ambient 0 diffuse 3 }
} 
texture { // moyenne altitude
  pigment {
    wrinkles
    turbulence 1.5
    color_map {
      [0.0 0.1 color rgbf <1, 1, 1,0.0>
               color rgbf <1, 1, 1,0.4>]
      [0.1 0.2 color rgbf <1, 1, 1,0.4>
               color rgbf <1, 1, 1,1.0>]
      [0.2 1.0 color rgbf <1, 1, 1,1.0>
               color rgbf <1, 1, 1,1.0>]
    }
  }
  scale <1,1,1>
  finish { ambient 0 diffuse 1 phong 0.3 phong_size 20 }
  rotate <15,45,80>
}
texture {
  pigment {
    wrinkles
    turbulence 0.5 octaves 4 lambda 2 omega 0.5
    color_map {
      // [0.00   color rgbf <1.0, 1.0, 1.0, 1.0> ]
      [0.40   color rgbf <0.98, 0.98, 0.98, 1.0> ]
      [0.70   color rgbf <0.98, 0.98, 0.98, 0.1> ]
    }
  }
  normal { granite 0.05 scale <1,1,1>/50 turbulence 0.8 }  scale <1,1,1>/2
  rotate <15,45,80>
  finish { ambient 0 diffuse 1.5 }
}

	

#declare Star_Field = texture {
 pigment {
    granite
    turbulence 0.8
    color_map {
      [ 0.00  0.72 color rgb <0,0,0>    color rgb <0,0,0> ]
      [ 0.72  0.75 color rgb <.2,.2,.2> color rgb <.4,.4,.4> ]
      [ 0.75  0.78 color rgb <.4,.4,.4> color rgb <.6,.6,.6> ]
      [ 0.78  0.81 color rgb <.4,.4,.4> color rgb <.4,.4,.4> ]
      [ 0.81  0.85 color rgb <.8,.8,.8> color rgb <.95,.95,.8> ]
      [ 0.85  0.91 color rgb <.95,.95,.95> color rgb <1,1,1>  ]
      [ 0.91  1.00 color rgb <0,0,0>  color rgb <0,0,0> ]
    }
  }
  finish { ambient 0 diffuse 0.8 }
  scale 0.01
}
texture {
  pigment {
    bozo
    turbulence 0.8
    color_map {
      [ 0.00  0.20 color rgbf <1.0,1.0,1.0,0.80>
                   color rgbf <1.0,1.0,1.0,0.84> ]
      [ 0.20  0.40 color rgbf <1.0,1.0,1.0,0.84>
                   color rgbf <1.0,1.0,1.0,0.88> ]
      [ 0.40  0.60 color rgbf <1.0,1.0,1.0,0.88>
                   color rgbf <1.0,1.0,1.0,0.92> ]
      [ 0.60  0.80 color rgbf <1.0,1.0,1.0,0.92>
                   color rgbf <1.0,1.0,1.0,1.00> ]
      [ 0.80  1.00 color rgbf <1.0,1.0,1.0,1.00>
                   color rgbf <1.0,1.0,1.0,1.00> ]
    }
  }
  finish { ambient 0 diffuse 1.0 }
  scale 0.06
}


#declare Atmosphere_Halo = texture {
  pigment { color rgbf <1, 1, 1, 1> }
    halo { 
      dust
      dust_type 1
      spherical_mapping
      linear
      max_value 1
      color_map {
        [ 0.000 color rgbft <0.0, 0.1, 0.80, 1.0 ,  1.0> ]
        [ 0.005 color rgbft <0.0, 0.2, 0.95, 1.0 ,  0.5> ]
        [ 0.010 color rgbft <0.3, 0.5, 1.0 , 0.7 ,  0.0> ]
        [ 0.025 color rgbft <0.3, 0.5, 1.0 , 0.50, -1.0> ]
        [ 0.040 color rgbft <0.3, 0.5, 1.0 , 0.0 , -2.0> ]
    }
  }
  finish { ambient 0 diffuse 2.2 }
}

#declare Earth_Land = texture { 
  pigment { color rgb <0.6,0.7,1> }
  finish { crand 0.05 ambient 0.0 diffuse 1 }
}
texture {
  pigment {
    bozo
    turbulence 0.6
    octaves 5
    lambda 7
    omega 0.5
    color_map {
      [ 0.0 0.2   color rgbf <0.8 ,0.75,0.75,0.0>
                  color rgbf <0.85,0.80,0.80,0.0>]
      [ 0.2 0.3   color rgbf <0.85,0.80,0.80,0.0>
                  color rgbf <0.9 ,0.9 ,0.9 ,0.1>]
      [ 0.3 0.41  color rgbf <0.9 ,0.85,0.7 ,0.1>
                  color rgbf <0.9 ,0.9 ,0.9 ,0.1>]
      [ 0.41 0.42 color rgbf <0.9 ,0.9 ,0.9 ,0.1>
                  color rgbf <0.9 ,0.9 ,0.9 ,1.0>]
      [ 0.42 1.0  color rgbf <0.9 ,0.9 ,0.9 ,1.0>
                  color rgbf <0.9 ,0.9 ,0.9 ,1.0>]
    }
    scale 1/1.2
  }
  // normal { granite 0.01 scale <1,1,1>/20 turbulence 0.8 }
  scale <1,1,1>/2
  rotate <0,-45,0>
  finish { ambient 0 diffuse 0.7 crand 0.01 }
}



#declare Rain = texture {
  pigment {
    bozo
    turbulence 0.1
    color_map {
      [0.00 0.90 color rgbf <1.00, 1.00, 1.00, 1.0>
                 color rgbf <1.00, 1.00, 1.00, 1.0>]
      [0.90 0.99 color rgbf <1.00, 1.00, 1.00, 1.0>
                 color rgbf <1.00, 1.00, 1.00, 0.9>]
      [0.99 1.00 color rgbf <1.00, 1.00, 1.00, 0.9>
                 color rgbf <1.00, 1.00, 1.00, 0.7>]
    }
  }
  finish { ambient 0.8 }
  scale <0.05 0.05 0.05>
  rotate <0 30 0>
}

#declare Clouds3D1 = texture {
  pigment { 
    bozo
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map {
      [0.0 0.1 color rgbt <1, 1, 1, 1>
               color rgbt <1, 1, 1, 1>]
      [0.1 0.5 color rgbt <1, 1, 1, 1>
               color rgbt <1, 1, 1, 1>]
      [0.5 1.0 color rgbt <1, 1, 1, 1>
               color rgbt <1, 1, 1, 1>]      
    }
    scale <0.2, 0.5, 0.2>
  } 
  halo {
    attenuating
    // 	eccentricity 1
    octaves 6
    omega 0.7
    lambda 2
    turbulence 0.7
    spherical_mapping
    poly
    max_value 0.2
    dust_type 3
    exponent 0.2
    jitter 0
    // aa_level 0.5
    // aa_threshold 0.5
    // phase 100
    color_map {
      [ 0   rgbt < 1,  1,  1, 1>]
      [.1   rgbt < 1,  1,  1, -.8>]
      [.2   rgbt < 1,  1,  1, -.8>]
      [.3   rgbt < 1,  1,  1, -.8>]
      [.4   rgbt < 1,  1,  1, -.8>]
      [.5   rgbt < 1,  1,  1, -.8>]
      [.6   rgbt < 1,  1,  1, -.8>]
      [.7   rgbt < 1,  1,  1, -.8>]
      [.8   rgbt < 1,  1,  1, -.8>]
      [.9   rgbt < 1,  1,  1, -.8>]
      [ 1   rgbt < 1,  1,  1, 1>]
    }
    frequency 10
    samples 20
    turbulence 0.7
    scale 0.8
  }
}

#declare Painted_W_Wood = texture {
  pigment { color rgb <0.9,0.9,0.9> }
  finish { crand 0.01 phong 0.8 reflection 0.1 }
  normal { bumps 0.02 scale <1/20,1/20,1/20> }
}

#declare Parquet_Wood = texture { // based on a Mike Miller texture
  pigment {
    marble
    color_map {
       [0.0 0.2  colour red 0.54 green 0.35 blue 0.25
                 colour red 0.54 green 0.35 blue 0.25]
       [0.2 0.4  colour red 0.54 green 0.35 blue 0.25
                 colour red 0.57 green 0.37 blue 0.24]
       [0.4 0.6  colour red 0.57 green 0.37 blue 0.24
                 colour red 0.56 green 0.31 blue 0.21]
       [0.6 0.8  colour red 0.56 green 0.31 blue 0.21
                 colour red 0.56 green 0.31 blue 0.21]
       [0.8 0.9  colour red 0.56 green 0.31 blue 0.21
                 colour red 0.57 green 0.37 blue 0.24]
       [0.9 1.01 colour red 0.57 green 0.37 blue 0.24
                 colour red 0.54 green 0.35 blue 0.25]
    }
    turbulence 0.25
    scale <9 9 1>
  }
  finish {
    ambient 0.3
    diffuse 1.2
  }
}
texture {
  pigment {
    wood
    color_map {
      [0.0 0.05 colour red 0.40 green 0.21 blue 0.15
                colour red 0.40 green 0.21 blue 0.15 filter 0.7]
      [0.05 0.2 colour red 0.40 green 0.21 blue 0.15 filter 0.7
                colour red 0.40 green 0.24 blue 0.14 filter 1.0]
      [0.2 0.7  colour red 0.40 green 0.24 blue 0.14 filter 1.0
                colour red 0.40 green 0.24 blue 0.14 filter 0.8]
      [0.7 0.8  colour red 0.30 green 0.14 blue 0.02 filter 0.7
                colour red 0.30 green 0.14 blue 0.02 filter 1.0]
      [0.8 0.9  colour red 0.30 green 0.14 blue 0.02 filter 1.0
                colour red 0.30 green 0.14 blue 0.02 filter 0.7]
      [0.9 1.01 colour red 0.30 green 0.14 blue 0.02 filter 1.0
                colour red 0.25 green 0.13 blue 0.09 filter 0.7]
    }
    turbulence 0.3
    scale <5, 10, 5>
  }
  finish {
    ambient 0.6
    diffuse 0.9
  }
}
texture {
  pigment {
    wood
    color_map {
      [0.0 0.05 colour red 0.40 green 0.21 blue 0.15
                colour red 0.40 green 0.21 blue 0.15 filter 0.4]
      [0.05 0.2 colour red 0.40 green 0.21 blue 0.15 filter 0.4
                colour red 0.40 green 0.24 blue 0.14 filter 0.8]
      [0.2 0.7  colour red 0.40 green 0.24 blue 0.14 filter 0.8
                colour red 0.40 green 0.24 blue 0.14 filter 0.9]
      [0.7 0.8  colour red 0.30 green 0.14 blue 0.02 filter 0.8
                colour red 0.30 green 0.14 blue 0.02 filter 0.6]
      [0.8 0.9  colour red 0.30 green 0.14 blue 0.02 filter 0.6
                colour red 0.30 green 0.14 blue 0.02 filter 0.4]
      [0.9 1.01 colour red 0.30 green 0.14 blue 0.02 filter 0.5
                colour red 0.25 green 0.13 blue 0.09 filter 0.8]
    }
    turbulence 0.2
    scale <4, 20, 4>
  }
  finish {
    diffuse 0.9
    ambient 0.3
    crand 0.1
    phong 0.5
    phong_size 20
  }
  normal {
    dents 0.2
    turbulence 0.85
    scale <1,1,0.3>
    scale 0.1
  }
}

#declare Red_Plastic = texture {
  pigment { color rgb <1,0,0> }
  finish { phong 1.0 }
}

#declare White_Plastic = texture {
  pigment { color rgb <1, 1, 1,> }
  finish { phong 1.0 }
}

#declare Green_Plastic = texture {
  pigment { color rgb <0, 1, 0> }
  finish { phong 1.0 }
}

#declare Blue_Plastic = texture {
  pigment { color rgb <0, 0, 1> }
  finish { phong 1.0 }
}

#declare Black_Plastic = texture {
  pigment { color rgb <0, 0, 0> }
  finish { phong 1.0 }
}

#declare Yellow_Plastic = texture {
  pigment { color rgb <1, 1, 0> }
  finish { phong 1.0 }
}

#declare Magenta_Plastic = texture {
  pigment { color rgb <1.0, 0, 1.0> }
  finish { phong 1.0 }
}

#declare Aquamarine_Plastic = texture {
  pigment { color rgb <0.439216, 0.858824, 0.576471> }
  finish { phong 1.0 }
}

#declare Brown_Plastic = texture {
  pigment { color rgb <0.647059, 0.164706, 0.164706> }
  finish { phong 1.0 }
}

#declare DarkOliveGreen_Plastic = texture {
  pigment { color rgb <0.309804, 0.309804, 0.184314> }
  finish { phong 1.0 }
}

#declare Firebrick_Plastic = texture {
  pigment { color rgb <0.556863, 0.137255, 0.137255> }
  finish { phong 1.0 }
}

#declare LightBlue_Plastic = texture {
  pigment { color rgb <0.74902, 0.847059, 0.847059> }
  finish { phong 1.0 }
}

#declare Maroon_Plastic = texture {
  pigment { color rgb <0.556863, 0.137255, 0.419608> }
  finish { phong 1.0 }
}

#declare MediumAquamarine_Plastic = texture {
  pigment { color rgb <0.196078, 0.8, 0.6> }
  finish { phong 1.0 }
}

#declare MidnightBlue_Plastic = texture {
  pigment { color rgb <0.184314, 0.184314, 0.309804> }
  finish { phong 1.0 }
}

#declare Navy_Plastic = texture {
  pigment { color rgb <0.137255, 0.137255, 0.556863> }
  finish { phong 1.0 }
}

#declare Orange_Plastic = texture {
  pigment { color rgb <1, 0.5, 0.0> }
  finish { phong 1.0 }
}

#declare Orchid_Plastic = texture {
  pigment { color rgb <0.858824, 0.439216, 0.858824> }
  finish { phong 1.0 }
}

#declare Pink_Plastic = texture {
  pigment { color rgb <0.737255, 0.560784, 0.560784> }
  finish { phong 1.0 } }

#declare Salmon_Plastic = texture {
  pigment { color rgb <0.435294, 0.258824, 0.258824> }
  finish { phong 1.0 }
}

#declare Tan_Plastic = texture {
  pigment { color rgb <0.858824, 0.576471, 0.439216> }
  finish { phong 1.0 }
}

#declare Skin_Plastic = texture {
  pigment { color rgb <1,0.84, 0.77> }
  finish { phong 1.0 }
}


#declare Turquoise_Plastic = texture {
  pigment { color rgb <0.678431, 0.917647, 0.917647> }
  finish { phong 1.0 }
}

#declare Violet_Plastic = texture {
  pigment { color rgb <0.309804, 0.184314, 0.309804> }
  finish { phong 1.0 }
}

#declare DarkBrown_Plastic = texture {
  pigment { color rgb <0.36, 0.25, 0.20> }
  finish { phong 1.0 }
}

#declare Red_RPlastic = texture {
  pigment { color rgb <1, 0, 0> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Skin_RPlastic = texture {
  pigment { color rgb <1,0.84, 0.77> }
  finish { phong 1.0 reflection 0.4 }
}

#declare White_RPlastic = texture {
  pigment { color rgb <1, 1, 1,> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Green_RPlastic = texture {
  pigment { color rgb <0, 1, 0>  }
  finish { phong 1.0 reflection 0.4 }
}

#declare Blue_RPlastic = texture {
  pigment { color rgb <0, 0, 1>  }
  finish { phong 1.0 reflection 0.4 }
}

#declare Black_RPlastic = texture {
  pigment { color rgb <0, 0, 0> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Yellow_RPlastic = texture {
  pigment { color rgb <1,1,0> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Magenta_RPlastic = texture {
  pigment { color rgb <1.0, 0, 1.0> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Aquamarine_RPlastic = texture {
  pigment { color rgb <0.439216, 0.858824, 0.576471> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Brown_RPlastic = texture {
  pigment { color rgb <0.647059, 0.164706, 0.164706> }
  finish { phong 1.0 reflection 0.4 }
}

#declare DarkOliveGreen_RPlastic = texture {
  pigment { color rgb <0.309804, 0.309804, 0.184314> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Firebrick_RPlastic = texture {
  pigment { color rgb <0.556863, 0.137255, 0.137255> }
  finish { phong 1.0 reflection 0.4 }
}

#declare LightBlue_RPlastic = texture {
  pigment { color rgb <0.74902, 0.847059, 0.847059> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Maroon_RPlastic = texture {
  pigment { color rgb <0.556863, 0.137255, 0.419608> }
  finish { phong 1.0 reflection 0.4 }
}

#declare MediumAquamarine_RPlastic = texture {
  pigment { color rgb <0.196078, 0.8, 0.6> }
  finish { phong 1.0 reflection 0.4 }
}

#declare MidnightBlue_RPlastic = texture {
  pigment { color rgb <0.184314, 0.184314, 0.309804> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Navy_RPlastic = texture {
  pigment { color rgb <0.137255, 0.137255, 0.556863> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Orange_RPlastic = texture {
  pigment { color rgb <1, 0.5, 0.0> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Orchid_RPlastic = texture {
  pigment { color rgb <0.858824, 0.439216, 0.858824> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Pink_RPlastic = texture {
  pigment { color rgb <0.737255, 0.560784, 0.560784> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Salmon_RPlastic = texture {
  pigment { color rgb <0.435294, 0.258824, 0.258824> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Tan_RPlastic = texture {
  pigment { color rgb <0.858824, 0.576471, 0.439216> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Turquoise_RPlastic = texture {
  pigment { color rgb <0.678431, 0.917647, 0.917647> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Violet_RPlastic = texture {
  pigment { color rgb <0.309804, 0.184314, 0.309804> }
  finish { phong 1.0 reflection 0.4 }
}

#declare DarkBrown_RPlastic = texture {
  pigment { color rgb <0.36, 0.25, 0.20> }
  finish { phong 1.0 reflection 0.4 }
}

#declare Red_Dull = texture {
  pigment { color rgb <1, 0, 0> }
  finish { phong 0.2 }
}

#declare White_Dull = texture {
  pigment { color rgb <1, 1, 1,> }
  finish { phong 0.2 }
}

#declare Green_Dull = texture {
  pigment { color rgb <0, 1, 0> }
  finish { phong 0.2 }
}

#declare Blue_Dull = texture {
  pigment { color rgb <0, 0, 1> }
  finish { phong 0.2 }
}

#declare Skin_Dull = texture {
  pigment { color rgb <1,0.84, 0.77> }
  finish { phong 0.2 }
}

#declare Black_Dull = texture {
  pigment { color rgb <0, 0, 0> }
  finish { phong 0.2 }
}

#declare Yellow_Dull = texture {
  pigment { color rgb <1,1,0> }
  finish { phong 0.2 }
}

#declare Magenta_Dull = texture {
  pigment { color rgb <1.0, 0, 1.0> }
  finish { phong 0.2 }
}

#declare Aquamarine_Dull = texture {
  pigment { color rgb <0.439216, 0.858824, 0.576471> }
  finish { phong 0.2 }
}

#declare Brown_Dull = texture {
  pigment { color rgb <0.647059, 0.164706, 0.164706> }
  finish { phong 0.2 }
}

#declare DarkOliveGreen_Dull = texture {
  pigment { color rgb <0.309804, 0.309804, 0.184314> }
  finish { phong 0.2 }
}

#declare Firebrick_Dull = texture {
  pigment { color rgb <0.556863, 0.137255, 0.137255> }
  finish { phong 0.2 }
}

#declare LightBlue_Dull = texture {
  pigment { color rgb <0.74902, 0.847059, 0.847059> }
  finish { phong 0.2 }
}

#declare Maroon_Dull = texture {
  pigment { color rgb <0.556863, 0.137255, 0.419608> }
  finish { phong 0.2 }
}

#declare MediumAquamarine_Dull = texture {
  pigment { color rgb <0.196078, 0.8, 0.6> }
  finish { phong 0.2 }
}

#declare MidnightBlue_Dull = texture {
  pigment { color rgb <0.184314, 0.184314, 0.309804> }
  finish { phong 0.2 }
}

#declare Navy_Dull = texture {
  pigment { color rgb <0.137255, 0.137255, 0.556863> }
  finish { phong 0.2 }
}

#declare Orange_Dull = texture {
  pigment { color rgb <1, 0.5, 0.0> }
  finish { phong 0.2 }
}

#declare Orchid_Dull = texture {
  pigment { color rgb <0.858824, 0.439216, 0.858824> }
  finish { phong 0.2 }
}

#declare Pink_Dull = texture {
  pigment { color rgb <0.737255, 0.560784, 0.560784> }
  finish { phong 0.2 }
}

#declare Salmon_Dull = texture {
  pigment { color rgb <0.435294, 0.258824, 0.258824> }
  finish { phong 0.2 }
}

#declare Tan_Dull = texture {
  pigment { color rgb <0.858824, 0.576471, 0.439216> }
  finish { phong 0.2 }
}

#declare Turquoise_Dull = texture {
  pigment { color rgb <0.678431, 0.917647, 0.917647> }
  finish { phong 0.2 }
}

#declare Violet_Dull = texture {
  pigment { color rgb <0.309804, 0.184314, 0.309804> }
  finish { phong 0.2 }
}

#declare DarkBrown_Dull = texture {
  pigment { color rgb <0.36, 0.25, 0.20> }
  finish { phong 0.2 }
}

#declare Old_Gold_Glass = texture {
  pigment {
    color rgbt <1, 0.9, 0.42, 0.99999>
  }
  finish {
    ambient 0.0
    diffuse 0.00001
    reflection 0.2
    refraction 1.0
    ior 1.5
    phong 0.3
    phong_size 90
    caustics 1.0
      irid {
         0.35
         thickness 0.8
         turbulence 0.1
      }
  }
}

#declare Black_White_Cow = texture {
  pigment {
    bozo
    turbulence 0.9
    color_map {
      [0   0.5  color rgb <1, 1, 1>
                color rgb <1, 1, 1> ]
      [0.5 1.0  color rgb <0, 0, 0>
                color rgb <0, 0, 0> ]
    }
  }
  normal {
    bumps 0.4
    scale <0.0001,0.0001,0.0001>
  }
  finish {
    phong 0.2
    ambient 0.4
  }
}

#declare Maroon_White_Agate_1 = texture {
  pigment {agate}
  finish {phong 0.3}
}

#declare Grey_Blue_Alabaster = texture {
  pigment {
    bozo
    turbulence 1.2
    color_map {
      [0   0.2   color rgb <1, 1, 1>
                 color rgb <0.9, 0.9, 1>]
      [0.2 0.4   color rgb <0.9, 0.9, 1>
                 color rgb <0.8, 0.8, 0.9>]
      [0.4 0.6   color rgb <0.8, 0.8, 0.9>
                 color rgb <0.7, 0.7, 0.8>]
      [0.6 0.8   color rgb <0.7, 0.7, 0.8>
                 color rgb <0.65, 0.65, 0.7>]
      [0.8 1.001 color rgb <0.65, 0.65, 0.7>
                 color rgb <0.7, 0.7, 0.8>]
    }
  }
  finish { phong 1.0 ambient 0.4 }
}

#declare Evening_Cloud_Sky = texture {
  pigment {
    bozo
    turbulence 0.5
    octaves 3
    lambda 6
    color_map {
      [0.0 0.1 color rgb < 0.8, 0.6,0.196078>
               color rgb < 0.8, 0.6,0.196078>]
      [1.0     color rgb <1, 1, 0.8,>]
    }
  }
  finish { ambient 0.9 diffuse 1 }
  scale <1,1,1>/6   /* please notice*/
}

#declare Light_Halo = texture {
  halo {
    spherical_mapping
    constant
    attenuating
    max_value 1
    color_map {
      [ 0.0 color rgbt <0  , 0 , 0, 1.00> ]
      [ 0.8 color rgbt <1  , 1 , 1, 0.99> ]
      [ 1.0 color rgbt <0.9,0.9, 1, 0.50> ]
    }
    turbulence 0
    samples 5
    aa_level 0
    aa_threshold 0.3
  }
  pigment { color rgbf <1,1,1,1> }
  finish { ambient 0 diffuse 0 }
}

#declare Anticyclone = texture {
  pigment {
    granite
    turbulence 0.7
    color_map {
      [0   0.22   color rgb <0.5945, 0.2855, 0.3515>
                  color rgb <0.5945, 0.2855, 0.3515>]
      [0.22 0.72  color rgb <0.3285, 0.8692, 0.8863>
                  color rgb <0.3917, 0.0324, 0.2404>]
      [0.72 1.001 color rgb <0.9442, 0.8700, 0.7092>
                  color rgb <0.4873, 0.4105, 0.2861>]
     }
  }
  finish {phong 1.0 ambient 0.4}
  rotate <128 329 120>
  scale  <3.6 3.6 1.9>
}

#declare SwimmingPool = texture {
  pigment {
    color rgbf <0.9,0.9,1.0,0.9>
  }
  normal { bumps 0.01 }
  finish {
    reflection 0.4
    refraction 1.0
    ior 1.33
    phong 1.0
    phong_size 100
  }
  scale <0.5,0.5,0.5>
}

#declare SwimBrick = texture {
  brick texture {
      pigment { color rgb <0.9,0.9,1.0> }
    },
    texture {
      pigment { color rgb <1.0,1.0,1.0> }
      finish { reflection 0.2 phong 1.0 phong_size 90 }
    }
  brick_size 0.15
  mortar 0.01
  scale <1,1.4,1>
}

#declare Azurite = texture {
  pigment {
    agate
     color_map {
        [0.0 0.3 color rgb <0.0, 0.0, 0.9>
                 color rgb <0.0, 0.0, 0.8> ]
        [0.3 1.0 color rgb <0.0, 0.0, 0.8>
                 color rgb <0.0, 0.0, 0.4> ]
     }
  }
  finish { phong 1.0 }
}

#declare Wood_0 = texture {
  pigment {
    wood
    turbulence 0.2
    color_map {
      [0.0 0.5 color rgb <0.866, 0.512, 0.4>
               color rgb <0.866, 0.512, 0.4> ]
      [0.5 1.0 color rgb <0.4, 0.133, 0.066>
               color rgb <0.2, 0.065, 0.033> ]
    }
  }
  finish {phong 0.2 ambient 0.4}
  scale <0.05 1 0.05>
  rotate <10 -45 0>
}

#declare Wood_2 = texture {
  pigment {
    wood
    turbulence 0.2
    color_map {
      [0.0 0.8 color rgb <1.0, 0.71875, 0.25>
               color rgb <1.0, 0.71875, 0.25>]
      [0.8 1   color rgb <0.5, 0.5, 0.066>
               color rgb <0.4, 0.4, 0.033>]
    }
  }
  scale <0.1 100 0.1>
  rotate <10 -45 0>
}

#declare Wood_3 = texture {
  pigment {
    wood
    turbulence 0.2
    color_map {
      [0.0 0.8 color rgb  <0.42857, 0.23810, 0.04762>
               color rgb  <0.42857, 0.23810, 0.04762>]
      [0.8 1.0 color rgb  <0.40000, 0.33301, 0.06601>
               color rgb  <0.20000, 0.03310, 0.03303>]
    }
  }
  finish {ambient 0.4 specular 0.1 roughness 1.0}
  scale <0.4 5 0.3>
  rotate <8 45 33>
}

#declare Wood_4 = texture {
  pigment {
    wood
    turbulence 0.04
    octaves 3
    color_map {
      [0.00 0.10 color rgb <0.60, 0.30, 0.18>
                 color rgb <0.60, 0.30, 0.18>]
      [0.10 0.90 color rgb <0.60, 0.30, 0.18>
                 color rgb <0.30, 0.15, 0.09>]
      [0.90 1    color rgb <0.30, 0.15, 0.09>
                 color rgb <0.30, 0.15, 0.09>]
    }
  }
  scale <0.2 0.2 1>
}

#declare Wood_5 = texture {
  pigment {
    wood
    turbulence 0.02
    color_map {
      [0.000 0.222 color rgb <0.808, 0.671, 0.251>
                   color rgb <0.808, 0.671, 0.251>]
      [0.222 0.342 color rgb <0.808, 0.671, 0.251>
                   color rgb <0.600, 0.349, 0.043>]
      [0.342 0.393 color rgb <0.600, 0.349, 0.043>
                   color rgb <0.808, 0.671, 0.251>]
      [0.393 0.709 color rgb <0.808, 0.671, 0.251>
                   color rgb <0.808, 0.671, 0.251>]
      [0.709 0.821 color rgb <0.808, 0.671, 0.251>
                   color rgb <0.533, 0.298, 0.027>]
      [0.821 1     color rgb <0.533, 0.298, 0.027>
                   color rgb <0.808, 0.671, 0.251>]
    }
  }
  scale <0.1 10 0.1>
  translate <10 0 0>
}

#declare Wood_6 = texture {
  pigment {
    bozo
    turbulence 0.2
    color_map {
      [0.000 0.171 color rgb <0.725, 0.659, 0.455>
                   color rgb <0.682, 0.549, 0.420>]
      [0.171 0.274 color rgb <0.682, 0.549, 0.420>
                   color rgb <0.557, 0.451, 0.322>]
      [0.274 0.393 color rgb <0.557, 0.451, 0.322>
                   color rgb <0.725, 0.659, 0.455>]
      [0.393 0.564 color rgb <0.725, 0.659, 0.455>
                   color rgb <0.682, 0.549, 0.420>]
      [0.564 0.701 color rgb <0.682, 0.549, 0.420>
                   color rgb <0.482, 0.392, 0.278>]
      [0.701 1.000 color rgb <0.482, 0.392, 0.278>
                   color rgb <0.725, 0.659, 0.455>]
    }
  }
  scale <0.04 0.04 0.2>
}

#declare Wood_7 = texture {
  pigment {
    wood
    turbulence 0.3
    color_map {
      [0.0 0.8 color rgb <0.666, 0.312, 0.2>
               color rgb <0.666, 0.312, 0.2>]
      [0.8 1.0 color rgb <0.4, 0.133, 0.066>
               color rgb <0.2, 0.065, 0.033>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  scale <5.0 0.3 0.3>
}

#declare Wood_8 = texture {
  pigment {
    wood
    turbulence 0.6
    color_map {
      [0.0 0.8  color rgb <0.789, 0.512, 0.200>
                color rgb <0.789, 0.512, 0.200>]
      [0.8 1.0  color rgb <0.400, 0.133, 0.066>
                color rgb <0.200, 0.065, 0.033>]
    }
  }
  scale <0.2 1.2 0.2>
  pigment {
    wood
    turbulence 0.2
  }
  finish {phong 0.52 phong_size 90 ambient 0.4}
  scale <1.0 1.4 1.3>
  rotate <95 121 341>
}

#declare Wood_9 = texture {
  pigment {
    wood
    turbulence 0.6
    color_map {
      [0.0 0.8  color rgb <0.789, 0.512, 0.200>
                color rgb <0.789, 0.512, 0.200>]
      [0.8 1.01 color rgb <0.400, 0.133, 0.066>
                color rgb <0.200, 0.065, 0.033>]
    }
  }
  pigment {
    gradient <1.0 3.6 3.9>
    turbulence 0.9
  }
  finish {phong 0.76 phong_size 63 ambient 0.4}
  scale <0.72 4.68 0.2>
  rotate <346 271 150>
}

#declare Wood_10 = texture {
  pigment {
    wood
    turbulence 0.04
    octaves 3
    color_map {
      [0.00 0.10 color rgb <0.888, 0.600, 0.300>
                 color rgb <0.888, 0.600, 0.300>]
      [0.10 0.90 color rgb <0.888, 0.600, 0.300>
                 color rgb <0.600, 0.400, 0.200>]
      [0.90 1.00 color rgb <0.600, 0.400, 0.200>
                 color rgb <0.600, 0.400, 0.200>]
    }
  }
  scale <0.05 .05 1>
}

#declare Wood_11 = texture {
  pigment {
    gradient <0.4 0.8 0.6>
    turbulence 0.5
    color_map {
      [0.0 0.8  color rgb <0.888, 0.600, 0.3>
                color rgb <0.888, 0.600, 0.3> ]
      [0.8 1.01 color rgb <0.6, 0.4, 0.2>
                color rgb <0.4, 0.3, 0.2> ]
    }
  }
  finish {refraction 0.3 phong 0.84 phong_size 72 ambient 0.4}
  scale <0.8 0.6 0.4>
  rotate <315 124 175>
}

#declare Island_Wood = texture {
  pigment {
    bozo
    color_map {
       [0.00 0.05 color rgb <0.647059, 0.164706, 0.164706>
                  color rgb <0.647059, 0.164706, 0.164706>]
       [0.05 0.25 color rgb <0.647059, 0.164706, 0.164706>
                  color rgb <0.858824, 0.576471, 0.439216>]
       [0.25 0.75 color rgb <0.858824, 0.576471, 0.439216>
                  color rgb <0.858824, 0.576471, 0.439216>]
       [0.75 0.95 color rgb <0.858824, 0.576471, 0.439216>
                  color rgb <0.647059, 0.164706, 0.164706>]
       [0.95 1.01 color rgb <0.647059, 0.164706, 0.164706>
                  color rgb <0.647059, 0.164706, 0.164706>]
    }
  }
  finish {
    ambient 0.15
    diffuse 0.7
    specular 1
    roughness 0.005
    phong 1
    phong_size 100
  }
  scale <0.01 0.01 1>
  rotate <0 90 0>
}
texture {
  pigment {
    wood
    turbulence 0.05
    color_map {
       [0.00 0.05 color rgbf <1.0, 1.0, 1.0, 1.0>
                  color rgbf <1.0, 1.0, 1.0, 1.0>]
       [0.05 0.30 color rgbf <1.0, 1.0, 1.0, 1.0>
                  color rgb <0.36, 0.25, 0.20>]
       [0.30 0.70 color rgb <0.36, 0.25, 0.20>
                  color rgb <0.36, 0.25, 0.20>]
       [0.70 0.95 color rgb <0.36, 0.25, 0.20>
                  color rgbf <1.0, 1.0, 1.0, 1.0>]
       [0.95 1.01 color rgbf <1.0, 1.0, 1.0, 1.0>
                  color rgbf <1.0, 1.0, 1.0, 1.0>]
    }
  }
  finish {ambient 0.15 diffuse 0.7}
  scale <0.1 0.1 1>
  translate <0 -0.5 0>
  rotate <14 90 34>
}

#declare Rocher = texture {
  pigment { rgb <0.75,0.75,0.75> }
  normal {
    gradient y
    frequency 3
    turbulence .5
    normal_map {
      [0.00 granite]
      [0.25 spotted turbulence .35]
      [0.50 marble turbulence .5]
      [0.75 bozo turbulence .25]
      [1.00 granite]
    }
  }
}

#declare Rocks = texture {
  pigment { rgb <0.45,0.42,0.42> }
  normal {
    crackle 2.0
    turbulence .5
    lambda 2
    scale <2,1,2>
  }
}

#declare Alien_Skin_1 = texture {
  pigment { rgb <0.42,0.52,0.42> }
  normal {
    crackle 2.0
    lambda 2
  }
  finish {
    phong 0.6
    reflection 0.06
  }
}

#declare Stormy_Water = texture {
  pigment { rgb <0.42,0.32,0.32> }
  normal {
    crackle 2.0
    turbulence 0.3
    lambda 2
  }
  finish {
    phong 1.0
    reflection 0.4
  }
}

#declare Alien_Skin_2 = texture {
  pigment { color rgb <0.02,0.72,0.62> }
  normal {
    hexagon 0.1
    turbulence 0.1
    lambda 2
    scale <0.1,0.1,0.1>
  }
  finish {
    phong 0.5
    reflection 0.1
  }
}

#declare Alien_Skin_3 = texture {
  pigment { color rgb <0.02,0.72,0.62> }
  normal {
    leopard 1
    turbulence 0.4
    lambda 2
    scale <0.1,0.1,0.1>
  }
  finish {
    phong 0.5
    reflection 0.1
  }
}

#declare Alien_Skin_4 = texture {
  pigment { color rgb <0.02,0.72,0.62> }
  normal {
    spiral1 0.4
    turbulence 0.2
    lambda 2
  }
  finish {
    phong 0.5
    reflection 0.1
  }
}

#declare Alien_Skin_5 = texture {
  pigment { color rgb <0.02,0.72,0.62> }
  normal {
    spiral2 0.4
    turbulence 0.2
    lambda 2
    scale 0.5
  }
  finish {
    phong 0.5
    reflection 0.1
  }
}

#declare Brain_Stomac = texture {
  pigment { color rgb <1.0,0.72,0.62> }
  normal {
    spotted 0.7
    turbulence 0.2
    lambda 2
    scale 0.1
  }
  finish {
    phong 0.5
    reflection 0.1
  }
}

#declare Ebony_Wood = texture {
  pigment {
    agate
    color_map {
      [0.0 0.5   color rgb <0.21, 0.1, 0.1>
                 color rgb <0.25, 0.25, 0.01>]
      [0.5 0.6   color rgb <0.25, 0.25, 0.01>
                 color rgb <0.3, 0.1, 0.1>]
      [0.6 1.001 color rgb <0.15, 0.1, 0.1>
                 color rgb <0.15, 0.1, 0.1>]
    }
  }
  finish {ambient 0.6 phong 1.0}
}

#declare Texture_Systo = texture {
  pigment { color rgb <0,0,0.98039> }
  finish { phong 0.2 }
}

#declare Henna_Wood = texture {
  pigment {
    wood
    turbulence 0.2
  }
  finish {phong 0.3 ambient 0.4}
  scale <0.4 14 0.4>
  rotate <0 45 0>
}

#declare Cork_Wood_1 = texture {
  pigment {
    granite
    color_map {
      [0.0 0.6  color rgb <0.93, 0.71, 0.532>
                color rgb <0.98, 0.81, 0.60>]
      [0.6 0.65 color rgb <0.50, 0.30, 0.20>
                color rgb <0.50, 0.30, 0.20>]
      [0.65 1.0 color rgb <0.80, 0.53, 0.46>
                color rgb <0.85, 0.75, 0.35>]
    }
  }
  finish {specular 0.1 roughness 0.5}
  scale <0.25 0.25 0.25>
}

#declare Cork_Wood_2 = texture {
  pigment {
    granite
    color_map {
      [0.00 0.60 color rgb <0.93, 0.71, 0.53>
                 color rgb <0.98, 0.81, 0.60>]
      [0.60 0.65 color rgb <0.50, 0.30, 0.20>
                 color rgb <0.50, 0.30, 0.20>]
      [0.65 1.00 color rgb <0.80, 0.53, 0.46>
                 color rgb <0.85, 0.75, 0.35>]
    }
  }
  finish {specular 0.1 roughness 0.5}
  scale <0.25 1.00 0.25>
}

#declare High_Sky = texture {
  pigment {
    bozo
    turbulence 0.5
    color_map {
        [0.0 0.5   color rgb <0.5, 0.5, 1.0>
                   color rgb <0.5, 0.5, 1.0>]
        [0.5 0.6   color rgb <0.5, 0.5, 1.0>
                   color rgb <1.0, 1.0, 1.0>]
        [0.6 1.001 color rgb <1.0, 1.0, 1.0>
                   color rgb <0.5, 0.5, 0.5>]
    }
  }
  finish {phong 1.0}
  scale <0.5 0.5 0.5>
}

#declare Midnight_Sky = texture {
  pigment {
    bozo
    turbulence 0.3
    color_map {
       [0.00 0.50 color rgb <0.25, 0.25, 0.50>
                  color rgb <0.25, 0.25, 0.50>]
       [0.50 0.60 color rgb <0.25, 0.25, 0.50>
                  color rgb <0.70, 0.70, 0.70>]
       [0.60 1.00 color rgb <0.70, 0.70, 0.70>
                  color rgb <0.30, 0.30, 0.30>]
    }
  }
  finish {ambient 0.4}
}

#declare Bright_Sky_2 = texture {
  pigment {
    bozo
    turbulence 0.56
    color_map {
       [0.0 0.5 color rgb <0.5, 0.5, 1.0>
                color rgb <0.5, 0.5, 1.0>]
       [0.5 0.6 color rgb <0.5, 0.5, 1.0>
                color rgb <1.0, 1.0, 1.0>]
       [0.6 1.0 color rgb <1.0, 1.0, 1.0>
                color rgb <0.5, 0.5, 0.5>]
    }
  }
  finish {ambient 0.4}
}

#declare Red_Blue_Checker = texture {
  pigment {
    checker
    color rgb <1, 0, 0>
    color rgb <0, 0, 1>
  }
  finish {phong 1.0}
}

#declare White_Black_Checker = texture {
  pigment {
    checker
    color rgb <0, 0, 0>
    color rgb <1, 1, 1>
  }
  finish {phong 1.0}
}

#declare White_Blue_Checker = texture {
  pigment {
    checker
    color rgb <0.8, 0.8, 1>
    color rgb <1, 1, 1>
  }
  finish {phong 1.0 reflection 0.3}
}

#declare Dendrite = texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.00 0.07 color rgb <1.000000, 1.000000, 1.000000>
                 color rgb <1.000000, 1.000000, 1.000000>]
      [0.07 0.20 color rgb <1.000000, 1.000000, 1.000000>
                 color rgb <0.329412, 0.329412, 0.329412>]
      [0.20 0.30 color rgb <0.329412, 0.329412, 0.329412>
                 color rgbf <1.000000, 1.000000, 1.000000, 1.0>]
      [0.30 0.70 color rgbf <1.000000, 1.000000, 1.000000, 1.0>
                 color rgbf <1.000000, 1.000000, 1.000000, 1.0>]
      [0.70 1.00 color rgbf <1.000000, 1.000000, 1.000000, 1.0>
                 color rgbf <0.329412, 0.329412, 0.329412>]
    }
  }
  finish {phong 0.867}
}

#declare Funny_Pattern = texture {
  pigment {
    wood
    turbulence 0.6
    color_map {
      [0   0.23   color rgb <0.3017, 0.2697, 0.3231>
                  color rgb <0.3017, 0.2697, 0.3231>]
      [0.23 0.88  color rgb <0.9167, 0.4900, 0.0287>
                  color rgb <0.1358, 0.4347, 0.9851>]
      [0.88 0.00  color rgb <0.1404, 0.6290, 0.6517>
                  color rgb <0.8902, 0.4708, 0.3765>]
      [0.00 1.001 color rgb <0.8902, 0.4708, 0.3765>
                  color rgb <0.4432, 0.8593, 0.0434>]
    }
  }
  finish {phong 0.45 phong_size 72 ambient 0.4}
  scale <0.4 0.9 0.4>
  rotate < 35 355 137>
}

#declare Pink_Flock = texture {
  pigment {
    leopard
    color_map {
      [0.0 0.3  color rgbf <1.00, 1.00, 1.000, 1.0>
                color rgbf <1.00, 1.00, 1.000, 1.0>]
      [0.3  1.0 color rgb <0.80, 0.53, 0.846>
                color rgb <0.85, 0.75, 0.835>]
    }
  }
  scale <0.02 0.02 0.02>
  rotate <0 30 0>
}

#declare Snow_Flock = texture {
  pigment {
    bozo
    turbulence 0.2
    color_map {
      [0.00 0.90 color rgbf <1.00, 1.00, 1.00, 1.0>
                 color rgbf <1.00, 1.00, 1.00, 1.0>]
      [0.90 0.99 color rgbf <1.00, 1.00, 1.00, 1.0>
                 color rgbf <1.00, 1.00, 1.00, 0.1>]
      [0.99 1.00 color rgbf <1.00, 1.00, 1.00, 0.1>
                 color rgbf <1.00, 1.00, 1.00, 0.0>]
    }
  }
  finish {ambient 0.8}
  scale <0.05 0.05 0.05>
  rotate <0 30 0>
}

#declare Ice = texture {
  pigment {
    bozo
    turbulence 0.2
    color_map {
      [0.00 0.90 color rgbf <1.00, 1.00, 1.00, 0.0>
                 color rgbf <1.00, 1.00, 1.00, 0.2>]
      [0.90 0.99 color rgbf <1.00, 1.00, 1.00, 0.2>
                 color rgbf <0.90, 0.90, 1.00, 0.1>]
      [0.99 1.00 color rgbf <0.90, 0.90, 1.00, 0.1>
                 color rgbf <1.00, 1.00, 1.00, 0.0>]
    }
  }
  finish {
    ambient 0.7
    refraction 1.0
    ior 1.5
    reflection 0.2
  }
}

#declare Bounding_Box = texture {
  pigment {rgbf <1,1,1,0.98>}
  finish {phong 1.0}
}

#declare Mountains = texture {
  pigment {
    bozo
    turbulence 0.5
    color_map {
      [0.00 0.29 color rgb <0.79, 0.71, 0.66>
                 color rgb <0.83, 0.69, 0.62>]
      [0.29 0.31 color rgb <0.83, 0.69, 0.62>
                 color rgb <0.76, 0.66, 0.61>]
      [0.31 0.43 color rgb <0.76, 0.66, 0.61>
                 color rgb <0.88, 0.76, 0.73>]
      [0.43 0.64 color rgb <0.88, 0.76, 0.73>
                 color rgb <0.70, 0.60, 0.55>]
      [0.64 0.69 color rgb <0.70, 0.60, 0.55>
                 color rgb <0.87, 0.71, 0.66>]
      [0.69 1.00 color rgb <0.87, 0.71, 0.66>
                 color rgb <0.79, 0.71, 0.66>]
    }
  }
  normal {bumps 0.5 scale <1,0.2,1>}
}
texture {
  pigment {
    marble
    turbulence 1.0
    color_map {
      [0.00 0.20 color rgbf <0.79, 0.71, 0.66, 0.5>
                 color rgbf <0.53, 0.39, 0.32, 0.5>]
      [0.20 0.40 color rgbf <0.53, 0.39, 0.32, 0.5>
                 color rgbf <0.76, 0.66, 0.61, 0.5>]
      [0.40 0.60 color rgbf <0.76, 0.66, 0.61, 0.5>
                 color rgbf <0.88, 0.76, 0.73, 0.5>]
      [0.60 0.80 color rgbf <0.88, 0.76, 0.73, 0.5>
                 color rgbf <0.50, 0.40, 0.25, 0.5>]
      [0.80 1.00 color rgbf <0.50, 0.40, 0.25, 0.5>
                 color rgbf <0.79, 0.71, 0.66, 0.5>]
    }
  }
  finish {ambient 0.02}
  normal {wrinkles 0.9}
  scale <0.1,0.5,0.5>
  rotate <0,0,90>
}

#declare Mountains_Snow = texture {
  pigment {
    bozo
    turbulence 0.5
    color_map {
      [0.00 0.29 color rgb <0.79, 0.71, 0.66>
                 color rgb <0.83, 0.69, 0.62>]
      [0.29 0.31 color rgb <0.83, 0.69, 0.62>
                 color rgb <0.76, 0.66, 0.61>]
      [0.31 0.43 color rgb <0.76, 0.66, 0.61>
                 color rgb <0.88, 0.76, 0.73>]
      [0.43 0.64 color rgb <0.88, 0.76, 0.73>
                 color rgb <0.70, 0.60, 0.55>]
      [0.64 0.69 color rgb <0.70, 0.60, 0.55>
                 color rgb <0.87, 0.71, 0.66>]
      [0.69 1.00 color rgb <0.87, 0.71, 0.66>
                 color rgb <0.79, 0.71, 0.66>]
    }
  }
  normal {bumps 0.5 scale <1, 0.2, 1>}
}
texture {
  pigment {
    marble
    turbulence 1.0
    color_map {
      [0.00 0.20 color rgbf <0.79, 0.71, 0.66, 0.5>
                 color rgbf <0.53, 0.39, 0.32, 0.5>]
      [0.20 0.40 color rgbf <0.53, 0.39, 0.32, 0.5>
                 color rgbf <0.76, 0.66, 0.61, 0.5>]
      [0.40 0.60 color rgbf <0.76, 0.66, 0.61, 0.5>
                 color rgbf <0.88, 0.76, 0.73, 0.5>]
      [0.60 0.80 color rgbf <0.88, 0.76, 0.73, 0.5>
                 color rgbf <0.50, 0.40, 0.25, 0.5>]
      [0.80 1.00 color rgbf <0.50, 0.40, 0.25, 0.5>
                 color rgbf <0.79, 0.71, 0.66, 0.5>]
    }
  }
  finish {ambient 0.02}
  normal {wrinkles 0.9}
  scale <0.1,0.5,0.5>
  rotate <0,0,90>
}
texture {
  pigment {
    bozo
    turbulence 0.7
    color_map {
      [0.0 0.5 color rgbf <1.00, 1.00, 1.00, 1.0>
               color rgbf <1.00, 1.00, 1.00, 1.0>]
      [0.5 0.6 color rgbf <1.00, 1.00, 1.00, 0.2>
               color rgbf <1.00, 1.00, 1.00, 0.2>]
      [0.6 1.0 color rgbf <1.00, 1.00, 1.00, 0.0>
               color rgbf <1.00, 1.00, 1.00, 0.0>]
    }
  }
  finish {ambient 0.8}
  scale <0.3 0.3 0.3>

}

#declare Salmon_Cock = texture {
  pigment {
    granite
    color_map {
       [0.0 0.9 color rgbf <1.00, 1.00, 1.0, 1.0>
                color rgb <0.80, 0.53, 0.46>]
       [0.9 1.0 color rgb <0.85, 0.75, 0.35>
                color rgbf <1.00, 1.00, 1.0, 1.0>]
    }
  }
  finish {ambient 0.8}
  scale <0.5 0.5 0.5>
  rotate <0 30 0>
}

#declare Granit_DO = texture {
  pigment {granite}
  finish {phong 1.0}
  normal {dents 0.8}
}

#declare Red_Gray_Granit = texture {
  pigment {
    granite
    turbulence 0.7
    color_map {
      [0   0.09   color rgbf <0.2168, 0.6793, 0.4631, 0>
                  color rgbf <0.2058, 0.7014, 0.2960, 0>]
      [0.09 0.16  color rgbf <0.2058, 0.7014, 0.2960, 0>
                  color rgbf <0.6494, 0.1976, 0.2262, 0>]
      [0.16 0.23  color rgbf <0.6494, 0.1976, 0.2262, 0>
                  color rgbf <0.7978, 0.1631, 0.2152, 0>]
      [0.23 0.43  color rgbf <0.7978, 0.1631, 0.2152, 0>
                  color rgbf <0.6321, 0.4497, 0.6554, 0>]
      [0.43 0.68  color rgbf <0.6321, 0.4497, 0.6554, 0>
                  color rgbf <0.7079, 0.6002, 0.6073, 0>]
      [0.68 1.00  color rgbf <0.7079, 0.6002, 0.6073, 0>
                  color rgbf <0.0770, 0.2991, 0.7439, 0>]
    }
  }
  finish {
    phong 0.68
    phong_size 63
    ambient 0.4
    refraction 0.2
    ior 1.1
  }
  scale <0.29 0.31 0.61>
  rotate <209 157 283>
}

#declare Pink_Granit = texture {
  pigment {
    granite
    color_map {
      [0.000 0.241 color rgbf <0.973, 0.973, 0.976, 0.0>
                   color rgbf <0.973, 0.973, 0.976, 0.0>]
      [0.241 0.284 color rgbf <0.973, 0.973, 0.976, 0.0>
                   color rgbf <0.600, 0.741, 0.608, 0.0>]
      [0.284 0.336 color rgbf <0.600, 0.741, 0.608, 0.0>
                   color rgbf <0.820, 0.643, 0.537, 0.0>]
      [0.336 0.474 color rgbf <0.820, 0.643, 0.537, 0.0>
                   color rgbf <0.886, 0.780, 0.714, 0.0>]
      [0.474 0.810 color rgbf <0.886, 0.780, 0.714, 0.0>
                   color rgbf <0.996, 0.643, 0.537, 0.0>]
      [0.810 0.836 color rgbf <0.996, 0.643, 0.537, 0.0>
                   color rgbf <0.973, 0.973, 0.976, 0.0>]
      [0.836 1.001 color rgbf <0.973, 0.973, 0.976, 0.0>
                   color rgbf <0.973, 0.973, 0.976, 0.0>]
    }
  }
  finish {phong 0.789}
}

#declare Gray_Vein_Granit = texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0 0.3 color rgb <1.000000, 1.000000, 1.000000>
               color rgb <0.329412, 0.329412, 0.329412>]
      [0.2 0.4 color rgb <0.329412, 0.329412, 0.329412>
               color rgb <0.329412, 0.329412, 0.329412>]
      [0.4 0.6 color rgb <0.329412, 0.329412, 0.329412>
               color rgb <0.329412, 0.329412, 0.329412>]
      [0.6 1.0 color rgb <0.329412, 0.329412, 0.329412>
               color rgb <0.329412, 0.329412, 0.329412>]
    }
  }
  finish {phong 0.7}
}

#declare Black_Vein_Granit = texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0 0.3 color rgb <0.0, 0.0, 0.0>
               color rgb <1.0, 1.0, 1.0>]
      [0.2 0.4 color rgb <1.0, 1.0, 1.0>
               color rgb <1.0, 1.0, 1.0>]
      [0.4 0.6 color rgb <1.0, 1.0, 1.0>
               color rgb <1.0, 1.0, 1.0>]
      [0.6 1.0 color rgb <1.0, 1.0, 1.0>
               color rgb <1.0, 1.0, 1.0>]
    }
  }
  finish {phong 0.77}
}

#declare Purple_Granit = texture {
  pigment {
    bozo
    turbulence 0.9
    color_map {
      [0.00 0.09 color rgbf <0.2172, 0.1237, 0.7066, 0.56>
                 color rgbf <0.6871, 0.4201, 0.0434, 0.92>]
      [0.09 0.15 color rgbf <0.6871, 0.4201, 0.0434, 0.92>
                 color rgbf <0.3897, 0.3777, 0.0352, 0.59>]
      [0.15 0.22 color rgbf <0.3897, 0.3777, 0.0352, 0.59>
                 color rgbf <0.0973, 0.7580, 0.9859, 0.56>]
      [0.22 0.31 color rgbf <0.0973, 0.7580, 0.9859, 0.56>
                 color rgbf <0.0145, 0.9400, 0.2154, 0.60>]
      [0.31 0.33 color rgbf <0.0145, 0.9400, 0.2154, 0.60>
                 color rgbf <0.7688, 0.1050, 0.5760, 0.07>]
      [0.33 1.00 color rgbf <0.7688, 0.1050, 0.5760, 0.07>
                 color rgbf <0.9939, 0.7191, 0.8677, 0.39>]
    }
  }
  finish {phong 0.90 phong_size 72}
  scale <0.08 0.13 0.34>
}

#declare Grass = texture {
  pigment {
    granite
    turbulence 0.7
    color_map {
      [0.00 0.07 color rgbf <0.7743, 0.3353, 0.5701, 0.98>
                 color rgbf <0.5210, 0.1044, 0.0333, 0.45>]
      [0.07 0.13 color rgbf <0.5210, 0.1044, 0.0333, 0.45>
                 color rgbf <0.3663, 0.5682, 0.0687, 0.82>]
      [0.13 0.31 color rgbf <0.3663, 0.5682, 0.0687, 0.82>
                 color rgbf <0.0887, 0.9886, 0.4438, 0.01>]
      [0.31 0.39 color rgbf <0.0887, 0.9886, 0.4438, 0.01>
                 color rgbf <0.1308, 0.9083, 0.5524, 0.74>]
      [0.39 0.55 color rgbf <0.1308, 0.9083, 0.5524, 0.74>
                 color rgbf <0.4840, 0.3945, 0.7343, 0.69>]
      [0.55 1.00 color rgbf <0.4840, 0.3945, 0.7343, 0.69>
                 color rgbf <0.1755, 0.5095, 0.4480, 0.43>]
    }
  }
  finish {phong 0.66 phong_size 81}
  scale <0.98 0.87 0.81>
}

#declare Hollywood = texture {
  pigment {
    onion
    turbulence 0.6
    color_map {
      [0   0.29   color rgb <0.9136, 0.8418, 0.8764>
                  color rgb <0.9136, 0.8418, 0.8764>]
      [0.29 0.93  color rgb <0.4478, 0.7337, 0.4993>
                  color rgb <0.2489, 0.3407, 0.2393>]
      [0.93 1.001 color rgb <0.4917, 0.9572, 0.0833>
                  color rgb <0.0667, 0.4001, 0.3527>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {ripples 1.0}
  rotate <340  72 287>
  scale  <0.1 0.9 4.0>
}

#declare Inferno_1 = texture {
  pigment {
    wood
    turbulence 0.3
    color_map {
      [0   0.58   color rgb <0.3501, 0.4402, 0.0798>
                  color rgb <0.3501, 0.4402, 0.0798>]
      [0.58 0.44  color rgb <0.3623, 0.1044, 0.5842>
                  color rgb <0.9410, 0.9179, 0.3361>]
      [0.44 0.00  color rgb <0.5917, 0.8572, 0.5896>
                  color rgb <0.7324, 0.2038, 0.2956>]
      [0.00 1.001 color rgb <0.7324, 0.2038, 0.2956>
                  color rgb <0.6273, 0.9007, 0.1178>]
    }
  }
  finish {phong 0.33 phong_size 90 ambient 0.4}
  normal {wrinkles 0.2}
  scale <0.5 0.4 0.7>
  rotate <3 340 91>
}

#declare Inferno_2 = texture {
  pigment {
    wood
    turbulence 0.2
    color_map {
      [0.00 0.06 color rgbf <0.2259, 0.2300, 0.2794, 0.53>
                 color rgbf <0.6536, 0.8927, 0.1314, 0.49>]
      [0.06 0.14 color rgbf <0.6536, 0.8927, 0.1314, 0.49>
                 color rgbf <0.6191, 0.0502, 0.1610, 0.09>]
      [0.14 0.27 color rgbf <0.6191, 0.0502, 0.1610, 0.09>
                 color rgbf <0.3962, 0.9469, 0.1328, 0.71>]
      [0.27 0.28 color rgbf <0.3962, 0.9469, 0.1328, 0.71>
                 color rgbf <0.3543, 0.6589, 0.2293, 0.98>]
      [0.28 0.85 color rgbf <0.3543, 0.6589, 0.2293, 0.98>
                 color rgbf <0.3748, 0.0369, 0.7473, 0.69>]
      [0.85 1.00 color rgbf <0.3748, 0.0369, 0.7473, 0.69>
                 color rgbf <0.2141, 0.6359, 0.9982, 0.45>]
    }
  }
  finish {phong 0.63 phong_size 90 ambient 0.4 metallic 0.0}
  normal {wrinkles 0.2}
  scale <0.99 0.96 0.39>
  rotate <349 208 121>
}

#declare Jade_1 = texture {
  pigment {
    marble
    turbulence 1.8
    color_map {
      [0.0 0.8 color rgb <0.1, 0.6, 0.1>
               color rgb <0.0, 0.3, 0.0>]
      [0.8 1.0 color rgb <0.1, 0.6, 0.1>
               color rgb <0.0, 0.2, 0.0>]
    }
  }
}

#declare Jade_2 = texture {
  pigment {
    marble
    turbulence 2.3
    color_map {
      [0.0 0.8 color rgb <0, 0, 0>
               color rgb <0.0, 0.6, 0.6>]
      [0.8 0.9 color rgb <0.0, 0.6, 0.6>
               color rgb <0.6, 0.0, 0.0>]
      [0.9 1   color rgb <0.6, 0.0, 0.0>
               color rgb <0, 0, 0>]
    }
  }
  finish {ambient 0.4}
}

#declare Leopard = texture {
  pigment {
    leopard
    turbulence 0.4
    color_map {
      [0   0.07   color rgbf <0.8944, 0.8251, 0.1203, .31>
                  color rgbf <0.8944, 0.8251, 0.1203, .31>]
      [0.07 0.19  color rgbf <0.6406, 0.1485, 0.1700, .02>
                  color rgbf <0.6406, 0.1485, 0.1700, .02>]
      [0.19 0.37  color rgbf <0.3889, 0.0074, 0.3002, .9>
                  color rgbf <0.3889, 0.0074, 0.3002, .9>]
      [0.37 0.57  color rgbf <0.2357, 0.2495, 0.1793, .62>
                  color rgbf <0.2357, 0.2495, 0.1793, .62>]
      [0.57 0.84  color rgbf <0.3302, 0.1342, 0.0686, .89>
                  color rgbf <0.3302, 0.1342, 0.0686, .89>]
      [0.84 1.00  color rgbf <0.0260, 0.8683, 0.9984, .93>
                  color rgbf <0.0260, 0.8683, 0.9984, .93>]
    }
  }
  finish {phong 0.57 phong_size 90 ambient 0.4}
  normal {bumps 0.1}
  scale <0.06 0.09 0.06>
  rotate <318 279 203>
}

#declare Pink_Grey_Floor = texture {
  pigment {
    bozo
    turbulence 0.25
    color_map {
      [0.0 1.0 color rgb <0.90, 0.75, 0.75>
               color rgb <0.60, 0.60, 0.60>]
    }
  }
  finish {ambient 0.25 phong 0.9 phong_size 85}
  scale <0.1 0.1 0.1>
}

#declare May_68 = texture {
  pigment {
    marble
    turbulence 0.2
    color_map {
      [0   0.17   color rgb <0.2170, 0.4924, 0.8205>
                  color rgb <0.2170, 0.4924, 0.8205>]
      [0.17 0.79  color rgb <0.4739, 0.8551, 0.0012>
                  color rgb <0.0304, 0.1953, 0.6764>]
      [0.79 0.00  color rgb <0.4252, 0.7422, 0.5126>
                  color rgb <0.9843, 0.0119, 0.2422>]
      [0.00 1.001 color rgb <0.9843, 0.0119, 0.2422>
                  color rgb <0.5564, 0.8157, 0.1293>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {bumps 1.0}
  scale <0.7 3.5 3.6>
  rotate < 93 206 312>
}

#declare White_Vein_Black_Marble = texture {
  pigment {
    marble
    turbulence 0.9
    color_map {
      [0   0.8   color rgb <1.0, 1.0, 1.0>
                 color rgb <0.6, 0.6, 0.6>]
      [0.8 1.001 color rgb <0.6, 0.6, 0.6>
                 color rgb <0.1, 0.1, 0.1>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  scale  <1 1 1>
}

#declare Grey_Vein_Black_Marble = texture {
  pigment {
    marble
    turbulence 1.0
    color_map {
      [0.0 0.8 color rgb <0.9, 0.9, 0.9>
               color rgb <0.5, 0.5, 0.5>]
      [0.8 1.0 color rgb <0.5, 0.5, 0.5>
               color rgb <0.2, 0.2, 0.2>]
    }
  }
}

#declare Red_Marble_2 = texture {
  pigment {
    marble
    turbulence 1.0
    color_map {
      [0.0 0.8 color rgb <0.8, 0.8, 0.6>
               color rgb <0.8, 0.4, 0.4>]
      [0.8 1.0 color rgb <0.8, 0.4, 0.4>
               color rgb <0.8, 0.2, 0.2>]
    }
  }
  scale <0.2 0.2 0.2>
}

#declare Dark_Red_Marble = texture {
  pigment {
    marble
    turbulence 1.0
    color_map {
      [0.0 0.8 color rgb <0.8, 0.8, 0.6>
               color rgb <0.8, 0.4, 0.4>]
      [0.8 1.0 color rgb <0.8, 0.4, 0.4>
               color rgb <0.8, 0.2, 0.2>]
    }
  }
  finish {phong 1.0 phong_size 85}
}

#declare Chrome_Metal_2 = texture {
  pigment {color rgb <0, 0, 0>}
  finish {
    ambient 0.3
    diffuse 0.0
    reflection 1.0
    brilliance 20.0
    specular 1.0
    roughness 0.001
    phong 1.0
    phong_size 90
  }
}

#declare Black_Galaxy = texture {
  pigment {
    granite
    turbulence 0.7
    color_map {
      [0   0.29   color rgb <0.0240, 0.1189, 0.1877>
                  color rgb <0.0240, 0.1189, 0.1877>]
      [0.29 0.30  color rgb <0.1991, 0.4928, 0.4967>
                  color rgb <0.5606, 0.4679, 0.0630>]
      [0.30 1.001 color rgb <0.1194, 0.0568, 0.0426>
                  color rgb <0.5284, 0.7513, 0.1368>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {wrinkles 0.0}
  rotate < 52 293 142>
  scale <1.6 1.8 1.2>
}

#declare Easter_Egg = texture {
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0   0.51   color rgb <0.4026, 0.9165, 0.7932>
                  color rgb <0.4026, 0.9165, 0.7932>]
      [0.51 0.17  color rgb <0.0099, 0.1125, 0.6465>
                  color rgb <0.3074, 0.1857, 0.9227>]
      [0.17 1.001 color rgb <0.0880, 0.2992, 0.6395>
                  color rgb <0.6386, 0.2604, 0.5928>]
    }
  }
  finish {phong 1.0 ambient 0.4 refraction 1.0}
  rotate <207 222  48>
  scale  <0.7 1.0 1.0>
}

#declare Pacific = texture {
  pigment {
    granite
    turbulence 0.4
    color_map {
      [0   0.12   color rgb <0.6602, 0.8021, 0.8680>
                  color rgb <0.6602, 0.8021, 0.8680>]
      [0.12 0.74  color rgb <0.6211, 0.5580, 0.7332>
                  color rgb <0.1284, 0.1723, 0.9133>]
      [0.74 0.00  color rgb <0.2577, 0.3911, 0.1019>
                  color rgb <0.5084, 0.3372, 0.8250>]
      [0.00 1.001 color rgb <0.5084, 0.3372, 0.8250>
                  color rgb <0.2430, 0.1515, 0.2866>]
    }
  }
  finish {phong 0.63 phong_size 81 ambient 0.4}
  scale <0.7 0.2 0.8>
  rotate <1 78 236>
}

#declare Copper_Shavings = texture {
  pigment {
    agate
    color_map {
      [0.0 0.5  color rgbf <1.00, 1.00, 1.00, 1.0>
                color rgbf <1.00, 1.00, 1.00, 1.0>]
      [0.5  1.0 color rgb <0.80, 0.53, 0.46>
                color rgb <0.85, 0.75, 0.35>]
    }
  }
  finish {specular 1.0}
  scale <0.02 200 0.02>
  rotate <0 30 0>
}

#declare Iron_Shavings = texture {
  pigment {
    marble
    color_map {
      [0.0 0.5 color rgbf <1.0, 1.0, 1.0, 1.0>
               color rgbf <1.0, 1.0, 1.0, 1.0>]
      [0.5 1.0 color rgb <1.0, 1.0, 1.0>
               color rgb <1.0, 1.0, 1.0>]
    }
  }
  finish {specular 1.0}
  scale <0.02 200 0.02>
  rotate <0 30 0>
}

#declare Paint_Cream = texture {
  pigment {
    wood
    turbulence 1.0
    color_map {
      [0   0.87   color rgb <0.6234, 0.8872, 0.7855>
                  color rgb <0.6234, 0.8872, 0.7855>]
      [0.87 0.74  color rgb <0.1126, 0.2688, 0.4823>
                  color rgb <0.1270, 0.0232, 0.2029>]
      [0.74 0.00  color rgb <0.7802, 0.4708, 0.9490>
                  color rgb <0.8964, 0.5643, 0.4392>]
      [0.00 1.001 color rgb <0.8964, 0.5643, 0.4392>
                  color rgb <0.6899, 0.9496, 0.1711>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {ripples 1.0}
  scale <4.1 0.1 2.4>
  rotate <328 127 140>
}

#declare Particles_1 = texture {
  pigment {
    bozo
    turbulence 0.8
    color_map {
      [0.000 0.087 color rgbf <0.682, 0.604, 0.380, 1.000>
                   color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.087 0.226 color rgbf <0.761, 0.694, 0.600, 0.020>
                   color rgbf <0.635, 0.553, 0.325, 1.000>]
      [0.226 0.348 color rgbf <0.635, 0.553, 0.325, 1.000>
                   color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.348 0.496 color rgbf <0.761, 0.694, 0.600, 0.020>
                   color rgbf <0.682, 0.604, 0.380, 1.000>]
      [0.496 0.565 color rgbf <0.682, 0.604, 0.380, 1.000>
                   color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.565 0.661 color rgbf <0.761, 0.694, 0.600, 0.020>
                   color rgbf <0.682, 0.604, 0.380, 1.000>]
      [0.661 0.835 color rgbf <0.682, 0.604, 0.380, 1.000>
                   color rgbf <0.761, 0.694, 0.600, 0.020>]
      [0.835 1.000 color rgbf <0.761, 0.694, 0.600, 0.020>
                   color rgbf <0.682, 0.604, 0.380, 1.000>]
    }
  }
  scale <0.2 0.2 2.0>
}

#declare Particles_2 = texture {
  pigment {
    agate
    turbulence 0.4
    color_map {
      [0.00 0.09 color rgbf <0.6514, 0.4223, 0.8026, 0.39>
                 color rgbf <0.6514, 0.4223, 0.8026, 0.39>]
      [0.09 0.13 color rgbf <0.5378, 0.4306, 0.9972, 0.05>
                 color rgbf <0.5378, 0.4306, 0.9972, 0.05>]
      [0.13 0.28 color rgbf <0.3555, 0.2631, 0.1179, 0.86>
                 color rgbf <0.3555, 0.2631, 0.1179, 0.86>]
      [0.28 0.47 color rgbf <0.0664, 0.5731, 0.6892, 0.67>
                 color rgbf <0.0664, 0.5731, 0.6892, 0.67>]
      [0.47 0.85 color rgbf <0.4443, 0.0841, 0.7888, 0.45>
                 color rgbf <0.4443, 0.0841, 0.7888, 0.45>]
      [0.85 1.00 color rgbf <0.2672, 0.8101, 0.7697, 0.43>
                 color rgbf <0.2672, 0.8101, 0.7697, 0.43>]
    }
  }
  finish {phong 0.82 phong_size 72 roughness 0.3 ambient 0.4}
  normal {waves 0.9}
  scale <0.37 0.20 0.18>
  rotate <116  70 307>
}

#declare Particles_3 = texture {
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0   0.09   color rgbf <0.4313, 0.8570, 0.8943, 0.42>
                  color rgbf <0.4313, 0.8570, 0.8943, 0.42>]
      [0.09 0.18  color rgbf <0.6569, 0.0018, 0.5253, 0.35>
                  color rgbf <0.6569, 0.0018, 0.5253, 0.35>]
      [0.18 0.26  color rgbf <0.4516, 0.3755, 0.9183, 0.87>
                  color rgbf <0.4516, 0.3755, 0.9183, 0.87>]
      [0.26 0.46  color rgbf <0.0548, 0.7616, 0.5482, 0.61>
                  color rgbf <0.0548, 0.7616, 0.5482, 0.61>]
      [0.46 0.74  color rgbf <0.9618, 0.8725, 0.6565, 0.42>
                  color rgbf <0.9618, 0.8725, 0.6565, 0.42>]
      [0.74 1.00  color rgbf <0.9024, 0.5490, 0.4140, 1>
                  color rgbf <0.9024, 0.5490, 0.4140, 1>]
    }
  }
  finish {phong 0.08 phong_size 63 ambient 0.4}
  scale <0.86 0.26 0.25>
  rotate <252  98 195>
}

#declare Blue_Particles = texture {
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0   0.05   color rgbf <0.1904, 0.4554, 0.6735, .08>
                  color rgbf <0.1904, 0.4554, 0.6735, .08>]
      [0.05 0.05  color rgbf <0.9138, 0.8481, 0.1662, .65>
                  color rgbf <0.9138, 0.8481, 0.1662, .65>]
      [0.05 0.15  color rgbf <0.1833, 0.7413, 0.2904, .93>
                  color rgbf <0.1833, 0.7413, 0.2904, .93>]
      [0.15 0.33  color rgbf <0.9108, 0.8256, 0.7252, .73>
                  color rgbf <0.9108, 0.8256, 0.7252, .73>]
      [0.33 0.82  color rgbf <0.2749, 0.4311, 0.7643, .01>
                  color rgbf <0.2749, 0.4311, 0.7643, .01>]
      [0.82 1.00  color rgbf <0.1158, 0.1045, 0.5100, .84>
                  color rgbf <0.1158, 0.1045, 0.5100, .84>]
    }
  }
  finish {phong 0.40 phong_size 63 ambient 0.4}
  scale <0.43 0.88 0.58>
  rotate <305 201 15>
}

#declare Onion_Peel = texture {
  pigment {
    gradient <1.2 1.2 0.3>
    turbulence 0.3
    color_map {
      [0   0.36   color rgb <0.3439, 0.7016, 0.4760>
                  color rgb <0.3439, 0.7016, 0.4760>]
      [0.36 0.17  color rgb <0.4653, 0.4809, 0.2126>
                  color rgb <0.8636, 0.6378, 0.7462>]
      [0.17 1.001 color rgb <0.4276, 0.0905, 0.3921>
                  color rgb <0.4470, 0.7727, 0.3792>]
    }
  }
  finish {phong 1.0 ambient 0.4 refraction 1.0}
  rotate <328 247 136>
  scale  <1.2 0.3 1.2>
}

#declare Stone_1 = texture {
  pigment {
    granite
    turbulence 0.8
    color_map {
      [0.00 0.09 color rgbf <0.1400, 0.0571, 0.2182, 0>
                 color rgbf <0.0618, 0.2772, 0.1627, 0>]
      [0.09 0.19 color rgbf <0.0618, 0.2772, 0.1627, 0>
                 color rgbf <0.0443, 0.7271, 0.7939, 0>]
      [0.19 0.38 color rgbf <0.0443, 0.7271, 0.7939, 0>
                 color rgbf <0.4215, 0.6412, 0.6240, 0>]
      [0.38 0.48 color rgbf <0.4215, 0.6412, 0.6240, 0>
                 color rgbf <0.8587, 0.3760, 0.0453, 0>]
      [0.48 0.83 color rgbf <0.8587, 0.3760, 0.0453, 0>
                 color rgbf <0.2012, 0.3254, 0.2589, 0>]
      [0.83 1.00 color rgbf <0.2012, 0.3254, 0.2589, 0>
                 color rgbf <0.9386, 0.9777, 0.8777, 0>]
    }
  }
  finish {phong 0.10 phong_size 72}
  scale <0.63 0.57 0.44>
}

#declare Stone_2 = texture {
  pigment {
    granite
    turbulence 1.1
    color_map {
      [0.00 0.09 color rgbf <0.7420, 0.1957, 0.1646, 0.70>
                 color rgbf <0.3277, 0.4617, 0.4008, 0.62>]
      [0.09 0.13 color rgbf <0.3277, 0.4617, 0.4008, 0.62>
                 color rgbf <0.8833, 0.8066, 0.8869, 0.36>]
      [0.13 0.17 color rgbf <0.8833, 0.8066, 0.8869, 0.36>
                 color rgbf <0.3138, 0.3189, 0.4586, 0.74>]
      [0.17 0.42 color rgbf <0.3138, 0.3189, 0.4586, 0.74>
                 color rgbf <0.5701, 0.6466, 0.3433, 0.23>]
      [0.42 0.75 color rgbf <0.5701, 0.6466, 0.3433, 0.23>
                 color rgbf <0.8167, 0.4925, 0.7607, 0.97>]
      [0.75 1.00 color rgbf <0.8167, 0.4925, 0.7607, 0.97>
                 color rgbf <0.3506, 0.8950, 0.3793, 0.92>]
    }
  }
  finish {phong 0.55 phong_size 81}
  scale <0.55 0.54 0.48>
}

#declare Pink_Granite = texture {
  pigment {
    granite
    turbulence 0
    color_map {
       [0.00 0.40 color rgb <0.000, 0.000, 0.000>
                  color rgb <0.000, 0.000, 0.000>]
       [0.40 0.45 color rgb <0.850, 0.850, 0.950>
                  color rgb <0.850, 0.850, 0.950>]
       [0.45 0.50 color rgb <0.850, 0.850, 0.950>
                  color rgb <0.753, 0.753, 0.753>]
       [0.50 0.55 color rgb <0.753, 0.753, 0.753>
                  color rgb <0.820, 0.570, 0.460>]
       [0.55 0.80 color rgb <0.820, 0.570, 0.460>
                  color rgb <0.820, 0.570, 0.460>]
       [0.80 1.00 color rgb <0.820, 0.570, 0.460>
                  color rgb <1.000, 0.500, 0.000>]
    }
  }
  finish {phong 1.0 phong_size 85}
}

#declare Pistachio_Nut = texture {
  pigment {
    bozo
    turbulence 0.3
    color_map {
      [0   0.06   color rgbf <0.6297, 0.3571, 0.6976, 0>
                  color rgbf <0.1408, 0.9514, 0.5200, 0>]
      [0.06 0.14  color rgbf <0.1408, 0.9514, 0.5200, 0>
                  color rgbf <0.6599, 0.8747, 0.2528, 0>]
      [0.14 0.34  color rgbf <0.6599, 0.8747, 0.2528, 0>
                  color rgbf <0.9483, 0.7884, 0.6313, 0>]
      [0.34 0.56  color rgbf <0.9483, 0.7884, 0.6313, 0>
                  color rgbf <0.4711, 0.7890, 0.3997, 0>]
      [0.56 0.70  color rgbf <0.4711, 0.7890, 0.3997, 0>
                  color rgbf <0.8220, 0.9360, 0.3860, 0>]
      [0.70 1.00  color rgbf <0.8220, 0.9360, 0.3860, 0>
                  color rgbf <0.1711, 0.5548, 0.1139, 0>]
    }
  }
  finish {phong 0.08 phong_size 63 ambient 0.4}
  scale <0.13 0.10 0.50>
  rotate < 49  18 306>
}

#declare Planet_1 = texture {
  pigment {
    granite
    turbulence 0.1
    color_map {
      [0   0.18   color rgb <0.5586, 0.7248, 0.2998>
                  color rgb <0.5586, 0.7248, 0.2998>]
      [0.18 0.76  color rgb <0.5461, 0.6625, 0.4053>
                  color rgb <0.4134, 0.5366, 0.9395>]
      [0.76 1.001 color rgb <0.7083, 0.1053, 0.0655>
                  color rgb <0.7737, 0.5325, 0.3206>]
    }
  }
  finish {phong 1.0 metallic}
  normal {bumps 1.0}
  rotate <291  82 198>
  scale  <2.2 2.7 1.3>
}

#declare Blue_Vein_Pink_Plastic = texture {
  pigment {
    wood
    turbulence 0.8
    color_map {
      [0   0.89   color rgb <0.0468, 0.0862, 0.6746>
                  color rgb <0.0468, 0.0862, 0.6746>]
      [0.89 0.23  color rgb <0.8405, 0.9888, 0.9298>
                  color rgb <0.0406, 0.8997, 0.3637>]
      [0.23 0.00  color rgb <0.5662, 0.2521, 0.6819>
                  color rgb <0.9011, 0.2936, 0.0707>]
      [0.00 1.001 color rgb <0.9011, 0.2936, 0.0707>
                  color rgb <0.7730, 0.5941, 0.5171>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {ripples 0.0}
  scale <1.1 0.7 3.1>
  rotate <106 255  43>
}

#declare Mauve_Marble_Marine_Plastic = texture {
  pigment {
    gradient <0.2 1.1 2.0>
    turbulence 0.9
    color_map {
      [0   0.66   color rgb <0.7928, 0.6860, 0.9390>
                  color rgb <0.7928, 0.6860, 0.9390>]
      [0.66 0.58  color rgb <0.9091, 0.3747, 0.7370>
                  color rgb <0.6217, 0.6140, 0.5999>]
      [0.58 0.00  color rgb <0.8846, 0.4936, 0.2392>
                  color rgb <0.1685, 0.0771, 0.1102>]
      [0.00 1.001 color rgb <0.1685, 0.0771, 0.1102>
                  color rgb <0.0406, 0.0322, 0.3444>]
    }
  }
  finish {phong 1.0 ambient 0.4 roughness 1.0}
  scale <1.1 2.0 0.2>
  rotate <284 251  67>
}

#declare Pistachio_Marble_Black_Plastic = texture {
  pigment {
    agate
    turbulence 0.9
    color_map {
      [0   0.71   color rgb <0.0226, 0.1489, 0.0900>
                  color rgb <0.0226, 0.1489, 0.0900>]
      [0.71 0.52  color rgb <0.6521, 0.8775, 0.5787>
                  color rgb <0.1011, 0.1854, 0.9406>]
      [0.52 1.001 color rgb <0.3205, 0.9668, 0.0943>
                  color rgb <0.8730, 0.9234, 0.6004>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {dents 0.0}
  rotate <148 323 164>
  scale <2.3 0.2 3.5>
}

#declare Red_Vein_Grey_Plastic = texture {
  pigment {
    marble
    turbulence 1.0
    color_map {
      [0.0 0.7 color rgb <1.0, 0, 0>
               color rgb <1.0, 0, 0>]
      [0.7 0.9 color rgb <1.0, 0, 0>
               color rgb <1.0, 1.0, 1.0>]
      [0.9 1.0 color rgb <0.9, 0.8, 0.8>
               color rgb <0.7, 0.5, 0.5>]
    }
  }
  finish {phong 1.0}
  rotate <0 -30 0>
}

#declare Green_Vein_Pistachio_Plastic = texture {
  pigment {
    wood
    turbulence 0.4
    color_map {
      [0   0.81   color rgb <0.1686, 0.3219, 0.0639>
                  color rgb <0.1686, 0.3219, 0.0639>]
      [0.81 0.23  color rgb <0.0283, 0.2406, 0.4457>
                  color rgb <0.2252, 0.7120, 0.3254>]
      [0.23 0.00  color rgb <0.9740, 0.5141, 0.5747>
                  color rgb <0.1422, 0.2789, 0.3231>]
      [0.00 1.001 color rgb <0.1422, 0.2789, 0.3231>
                  color rgb <0.3723, 0.7744, 0.0549>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  normal {ripples 0.0}
  scale <0.5 1.2 2.6>
  rotate <124 222 358>
}

#declare Psychedelik_1 = texture {
  pigment {
    wood
    turbulence 1.0
    color_map {
      [0   0.09   color rgb <0.9138, 0.3142, 0.3359>
                  color rgb <0.9138, 0.3142, 0.3359>]
      [0.09 0.86  color rgb <0.2600, 0.8661, 0.4363>
                  color rgb <0.3114, 0.7216, 0.6458>]
      [0.86 0.00  color rgb <0.5377, 0.2882, 0.5914>
                  color rgb <0.5984, 0.8569, 0.8621>]
      [0.00 1.001 color rgb <0.5984, 0.8569, 0.8621>
                  color rgb <0.2072, 0.0735, 0.4433>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  scale <2.0 1.5 1.0>
  rotate <272 31 39>
}

#declare Psychedelik_2 = texture {
  pigment {
    wood
    turbulence 0.8
    color_map {
      [0   0.12   color rgb <0.3372, 0.5133, 0.2250>
                  color rgb <0.3372, 0.5133, 0.2250>]
      [0.12 0.36  color rgb <0.9879, 0.5860, 0.8839>
                  color rgb <0.2251, 0.5980, 0.8066>]
      [0.36 0.00  color rgb <0.3237, 0.0695, 0.3244>
                  color rgb <0.6031, 0.5862, 0.4936>]
      [0.00 1.001 color rgb <0.6031, 0.5862, 0.4936>
                  color rgb <0.2903, 0.7180, 0.7893>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  scale <3.0 2.1 1.7>
  rotate < 51 159 302>
}

#declare Psychedelik_3 = texture {
  pigment {
    granite
    turbulence 1.1
    color_map {
      [0   0.17   color rgb <0.7616, 0.8549, 0.5759>
                  color rgb <0.7616, 0.8549, 0.5759>]
      [0.17 0.52  color rgb <0.4968, 0.0588, 0.1270>
                  color rgb <0.5049, 0.5687, 0.2050>]
      [0.52 1.001 color rgb <0.4033, 0.3751, 0.6748>
                  color rgb <0.4208, 0.9663, 0.0133>]
    }
  }
  finish {phong 1.0 ambient 0.4}
  rotate <123 249 306>
  scale  <1.7 2.3 3.8>
}

#declare Riviera_1 = texture {
  pigment {
    marble
    turbulence 0.9
    color_map {
      [0.0 0.7   color rgb <0.184, 0.184, 0.309>
                 color rgb <0.184, 0.184, 0.309>]
      [0.7 1.001 color rgb <1.000, 1.000, 1.000>
                 color rgb <0.000, 0.000, 1.000>]
    }
  }
  finish {phong 1.0}
  scale <0.4 0.4 0.4>
}

#declare Riviera_2 = texture {
  pigment {
    marble
    turbulence 0.5
    color_map {
      [0   0.44   color rgb <0.3872, 0.8987, 0.9664>
                  color rgb <0.3872, 0.8987, 0.9664>]
      [0.44 0.35  color rgb <0.1073, 0.7214, 0.0725>
                  color rgb <0.0201, 0.4910, 0.9891>]
      [0.35 0.00  color rgb <0.2843, 0.2324, 0.3433>
                  color rgb <0.0675, 0.7303, 0.4137>]
      [0.00 1.001 color rgb <0.0675, 0.7303, 0.4137>
                  color rgb <0.3399, 0.0372, 0.7414>]
    }
  }
  finish {phong 1.0 ambient 0.4 refraction 1.0}
  scale <0.2 2.3 0.2>
  rotate < 79 157 220>
}

#declare Scanner = texture {
  pigment {
    marble
    turbulence 1.1
    color_map {
      [0   0.05  color rgb <0.2183, 0.6550, 0.3040>
                 color rgb <0.4810, 0.5457, 0.0340>]
      [0.05 0.16 color rgb <0.4810, 0.5457, 0.0340>
                 color rgb <0.2517, 0.2356, 0.6608>]
      [0.16 0.20 color rgb <0.2517, 0.2356, 0.6608>
                 color rgb <0.1233, 0.1173, 0.2549>]
      [0.20 0.50 color rgb <0.1233, 0.1173, 0.2549>
                 color rgb <0.4906, 0.1921, 0.6600>]
      [0.50 0.81 color rgb <0.4906, 0.1921, 0.6600>
                 color rgb <0.2150, 0.6789, 0.7684>]
      [0.81 1.00 color rgb <0.2150, 0.6789, 0.7684>
                 color rgb <0.7305, 0.8288, 0.5263>]
    }
  }
  finish {phong 0.82 phong_size 81 ambient 0.4}
  normal {bumps 0.9}
  scale <0.79 0.53 0.60>
  rotate < 62  59 126>
}

#declare Tempest = texture {
  pigment {
    agate
    turbulence 0.7
    color_map {
      [0   0.34   color rgb <0.4389, 0.1428, 0.4260>
                  color rgb <0.4389, 0.1428, 0.4260>]
      [0.34 0.22  color rgb <0.7746, 0.8381, 0.5612>
                  color rgb <0.7973, 0.5913, 0.0370>]
      [0.22 1.001 color rgb <0.3723, 0.0032, 0.6093>
                  color rgb <0.5034, 0.8444, 0.6547>]
    }
  }
  finish {phong 1.0 ambient 0.4 refraction 0.0}
  rotate <355 267 317>
  scale  <1.0 2.0 4.0>
}

#declare Blob_Paint_1 = texture {
  pigment {
    agate
    turbulence 0.2
    color_map {
      [ 0.0 0.96 color rgbf <1.0, 1.0, 1.0, 0.9>
                 color rgbf <1.0, 1.0, 1.0, 0.9>]
      [ 0.96 1.0 color rgb <1.0, 1.0, 1.0>
                 color rgb <1.0, 1.0, 1.0>]
    }
  }
  finish {
    phong 0.93
    phong_size 90
    ambient 0.4
    reflection 0.3
    refraction 1.0
    ior 1.0
    roughness 0.0001
  }
  scale <1.2 2.6 3.9>
  rotate <276 282 252>
}

#declare Blob_Paint_2 = texture {
  pigment {
    gradient <0.8 0.6 2.9>
    turbulence 0.2
    color_map {
      [ 0.0 0.96 color rgbf <1.0, 1.0, 1.0, 0.9>
                 color rgbf <1.0, 1.0, 1.0, 0.9>]
      [ 0.96 1.0 color rgb <1.0, 1.0, 1.0>
                 color rgb <1.0, 1.0, 1.0>]
    }
  }
  finish {
    phong 0.93
    phong_size 90
    ambient 0.4
    reflection 0.3
    refraction 1.0
    ior 1.0
    roughness 0.0001
  }
  scale <0.6 2.9 0.8>
  rotate <221 340 99>
}

#declare Glass_Colored_Blue = texture {
  pigment {
    gradient <0.50 0.46 0.65>
    turbulence 1.0
    color_map {
      [0   0.10   color rgbf <0.6958, 0.1164, 0.8589, .83>
                  color rgbf <0.6958, 0.1164, 0.8589, .83>]
      [0.10 0.19  color rgbf <0.9704, 0.3980, 0.9414, .59>
                  color rgbf <0.9704, 0.3980, 0.9414, .59>]
      [0.19 0.24  color rgbf <0.1775, 0.5148, 0.8690, .13>
                  color rgbf <0.1775, 0.5148, 0.8690, .13>]
      [0.24 0.27  color rgbf <0.2514, 0.3192, 0.9290, .32>
                  color rgbf <0.2514, 0.3192, 0.9290, .32>]
      [0.27 0.30  color rgbf <0.4005, 0.0619, 0.8237, .99>
                  color rgbf <0.4005, 0.0619, 0.8237, .99>]
      [0.30 1.00  color rgbf <0.2303, 0.7319, 0.9427, .68>
                  color rgbf <0.2303, 0.7319, 0.9427, .68>]
    }
  }
  finish {
    phong 0.47
    phong_size 90
    ambient 0.4
  }
  scale <0.46 0.65 0.50>
  rotate <157 86 318>
}

#declare Thin_Glass = texture {
  pigment {color rgbf <1.0, 1.0, 1.0, 0.99999>}
  finish {
    ambient 0.0
    diffuse 0.0
    reflection 0.1
    refraction 1.0
    specular 1.0
    roughness 0.001
    phong 1.0
  }
}

#declare Blue_Volute = texture {
  pigment {
    onion
    turbulence 0.2
    color_map {
      [0   0.77   color rgb <0.4965, 0.2714, 0.6892>
                  color rgb <0.4965, 0.2714, 0.6892>]
      [0.77 0.08  color rgb <0.5210, 0.8875, 0.4673>
                  color rgb <0.9060, 0.2643, 0.3944>]
      [0.08 0.00  color rgb <0.9637, 0.7192, 0.9106>
                  color rgb <0.2672, 0.5501, 0.6322>]
      [0.00 1.001 color rgb <0.2672, 0.5501, 0.6322>
                  color rgb <0.8755, 0.2391, 0.8814>]
    }
  }
  finish {
    phong 0.83
    phong_size 90
    ambient 0.4
    brilliance 0.9
  }
  scale <0.9 0.1 0.2>
  rotate <132 263 334>
}

// --------------------------------------------------------------------------
// ---------------- STONE TEXTURES BY MIKE MILLER 1992 ----------------------
// --------------------------------------------------------------------------
// ------------- Scaled to cover nicely over a 2 unit Cube ------------------

//------------- Deep Rose & Green Marble with large White Swirls
#declare Stone1 = texture {
  pigment {
    granite
    turbulence 0.5
    color_map {
      [0.000, 0.153   color rgbf <0.439, 0.310, 0.282, 0.000>
                      color rgbf <0.463, 0.369, 0.259, 0.000>]
      [0.153, 0.398   color rgbf <0.463, 0.369, 0.259, 0.000>
                      color rgbf <0.541, 0.369, 0.298, 0.000>]
      [0.398, 0.559   color rgbf <0.541, 0.369, 0.298, 0.000>
                      color rgbf <0.573, 0.424, 0.286, 0.000>]
      [0.559, 0.729   color rgbf <0.573, 0.424, 0.286, 0.000>
                      color rgbf <0.494, 0.396, 0.306, 0.000>]
      [0.729, 1.001   color rgbf <0.494, 0.396, 0.306, 0.000>
                      color rgbf <0.439, 0.310, 0.282, 0.000>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.153   color rgbf <0.729, 0.502, 0.451, 0.306>
                      color rgbf <0.769, 0.686, 0.592, 0.792>]
      [0.153, 0.398   color rgbf <0.769, 0.686, 0.592, 0.792>
                      color rgbf <0.843, 0.753, 0.718, 0.396>]
      [0.398, 0.559   color rgbf <0.843, 0.753, 0.718, 0.396>
                      color rgbf <0.780, 0.667, 0.561, 0.976>]
      [0.559, 0.729   color rgbf <0.780, 0.667, 0.561, 0.976>
                      color rgbf <0.741, 0.659, 0.576, 0.820>]
      [0.729, 1.001   color rgbf <0.741, 0.659, 0.576, 0.820>
                      color rgbf <0.729, 0.502, 0.451, 0.306>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0,-30>
  finish{phong 1 phong_size 90}
}

//------------- Light Greenish Tan Marble with Agate style veining
#declare Stone2 = texture {
  pigment {
    granite
    turbulence 0.4
    color_map {
      [0.000, 0.153   color rgbf <0.729, 0.502, 0.451, 0.000>
                      color rgbf <0.769, 0.686, 0.592, 0.000>]
      [0.153, 0.398   color rgbf <0.769, 0.686, 0.592, 0.000>
                      color rgbf <0.843, 0.753, 0.718, 0.000>]
      [0.398, 0.559   color rgbf <0.843, 0.753, 0.718, 0.000>
                      color rgbf <0.780, 0.667, 0.561, 0.000>]
      [0.559, 0.729   color rgbf <0.780, 0.667, 0.561, 0.000>
                      color rgbf <0.741, 0.659, 0.576, 0.000>]
      [0.729, 1.001   color rgbf <0.741, 0.659, 0.576, 0.000>
                      color rgbf <0.729, 0.502, 0.451, 0.000>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.119   color rgbf <0.439, 0.310, 0.282, 0.631>
                      color rgbf <0.463, 0.369, 0.259, 0.847>]
      [0.119, 0.322   color rgbf <0.463, 0.369, 0.259, 0.847>
                      color rgbf <0.541, 0.369, 0.298, 0.549>]
      [0.322, 0.449   color rgbf <0.541, 0.369, 0.298, 0.549>
                      color rgbf <0.573, 0.424, 0.286, 0.965>]
      [0.449, 0.729   color rgbf <0.573, 0.424, 0.286, 0.965>
                      color rgbf <0.494, 0.396, 0.306, 0.741>]
      [0.729, 1.001   color rgbf <0.494, 0.396, 0.306, 0.741>
                      color rgbf <0.439, 0.310, 0.282, 0.631>]
    }
  }
  scale <1.3, 2, 1.3>
  rotate <0, 0, -30>
  rotate <40, 0, 0>
  finish{phong 1.0 phong_size 90 ambient 0.2 }
}

//------------- Rose & Yellow Marble with fog white veining
#declare Stone3 = texture {
  pigment {
    granite
    turbulence 0.5
    color_map {
      [0.000, 0.178   color rgbf <0.804, 0.569, 0.494, 0.000>
                      color rgbf <0.855, 0.729, 0.584, 0.000>]
      [0.178, 0.356   color rgbf <0.855, 0.729, 0.584, 0.000>
                      color rgbf <0.667, 0.502, 0.478, 0.000>]
      [0.356, 0.525   color rgbf <0.667, 0.502, 0.478, 0.000>
                      color rgbf <0.859, 0.624, 0.545, 0.000>]
      [0.525, 0.729   color rgbf <0.859, 0.624, 0.545, 0.000>
                      color rgbf <0.855, 0.729, 0.584, 0.000>]
      [0.729, 1.001   color rgbf <0.855, 0.729, 0.584, 0.000>
                      color rgbf <0.804, 0.569, 0.494, 0.000>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.153   color rgbf <0.729, 0.502, 0.451, 0.306>
                      color rgbf <0.769, 0.686, 0.592, 0.792>]
      [0.153, 0.398   color rgbf <0.769, 0.686, 0.592, 0.792>
                      color rgbf <0.843, 0.753, 0.718, 0.396>]
      [0.398, 0.559   color rgbf <0.843, 0.753, 0.718, 0.396>
                      color rgbf <0.780, 0.667, 0.561, 0.976>]
      [0.559, 0.729   color rgbf <0.780, 0.667, 0.561, 0.976>
                      color rgbf <0.741, 0.659, 0.576, 0.820>]
      [0.729, 1.001   color rgbf <0.741, 0.659, 0.576, 0.820>
                      color rgbf <0.729, 0.502, 0.451, 0.306>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, -30>
  finish{phong 1.0 phong_size 90}
}

//------------- Tan Marble with Rose patches
#declare Stone4 = texture {
  pigment {
    granite
    turbulence 0.5
    color_map {
      [0.000, 0.153   color rgbf <0.545, 0.380, 0.345, 0.000>
                      color rgbf <0.588, 0.475, 0.333, 0.000>]
      [0.153, 0.398   color rgbf <0.588, 0.475, 0.333, 0.000>
                      color rgbf <0.675, 0.478, 0.404, 0.000>]
      [0.398, 0.559   color rgbf <0.675, 0.478, 0.404, 0.000>
                      color rgbf <0.757, 0.635, 0.522, 0.000>]
      [0.559, 0.729   color rgbf <0.757, 0.635, 0.522, 0.000>
                      color rgbf <0.659, 0.549, 0.443, 0.000>]
      [0.729, 1.001   color rgbf <0.659, 0.549, 0.443, 0.000>
                      color rgbf <0.545, 0.380, 0.345, 0.000>]
    }
  }
  scale <1.5, 3, 2>
  rotate <0, 0, 40>
  finish{diffuse 0.5}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.179   color rgbf <0.843, 0.655, 0.655, 0.455>
                      color rgbf <0.886, 0.769, 0.627, 0.608>]
      [0.179, 0.368   color rgbf <0.886, 0.769, 0.627, 0.608>
                      color rgbf <0.906, 0.820, 0.714, 0.392>]
      [0.368, 0.538   color rgbf <0.906, 0.820, 0.714, 0.392>
                      color rgbf <0.851, 0.671, 0.671, 0.659>]
      [0.538, 0.744   color rgbf <0.851, 0.671, 0.671, 0.659>
                      color rgbf <0.890, 0.792, 0.675, 0.392>]
      [0.744, 0.983   color rgbf <0.890, 0.792, 0.675, 0.392>
                      color rgbf <0.827, 0.612, 0.612, 0.706>]
      [0.983, 1.001   color rgbf <0.827, 0.612, 0.612, 0.706>
                      color rgbf <0.843, 0.655, 0.655, 0.455>]
    }
  }
  scale <1, 3, 2>
  rotate <0, 0, -30>
  finish{phong 1.0 phong_size 90}
}

//------------- White Cream Marble with Pink veining
#declare Stone5 = texture {
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.996, 0.969, 0.800, 0.000>
                      color rgbf <0.996, 0.682, 0.604, 0.000>]
      [0.154, 0.308   color rgbf <0.996, 0.682, 0.604, 0.000>
                      color rgbf <0.906, 0.820, 0.714, 0.000>]
      [0.308, 0.444   color rgbf <0.906, 0.820, 0.714, 0.000>
                      color rgbf <0.816, 0.631, 0.537, 0.000>]
      [0.444, 0.615   color rgbf <0.816, 0.631, 0.537, 0.000>
                      color rgbf <0.890, 0.792, 0.675, 0.000>]
      [0.615, 0.812   color rgbf <0.890, 0.792, 0.675, 0.000>
                      color rgbf <0.973, 0.627, 0.627, 0.000>]
      [0.812, 1.001   color rgbf <0.973, 0.627, 0.627, 0.000>
                      color rgbf <0.996, 0.969, 0.800, 0.000>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.034   color rgbf <0.027, 0.012, 0.012, 0.000>
                      color rgbf <0.851, 0.812, 0.741, 0.235>]
      [0.034, 0.342   color rgbf <0.851, 0.812, 0.741, 0.235>
                      color rgbf <0.792, 0.694, 0.690, 0.839>]
      [0.342, 0.462   color rgbf <0.792, 0.694, 0.690, 0.839>
                      color rgbf <0.631, 0.506, 0.471, 0.608>]
      [0.462, 0.632   color rgbf <0.631, 0.506, 0.471, 0.608>
                      color rgbf <0.851, 0.812, 0.741, 0.922>]
      [0.632, 0.983   color rgbf <0.851, 0.812, 0.741, 0.922>
                      color rgbf <0.647, 0.655, 0.655, 0.282>]
      [0.983, 1.001   color rgbf <0.647, 0.655, 0.655, 0.282>
                      color rgbf <0.027, 0.012, 0.012, 0.000>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, -30>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.55, 0.09, 0.09, 0.80>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 2, 1.4>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90 ambient 0.2}
}

//------------- Rose & Yellow Cream Marble
#declare Stone6 = texture {
  pigment {
    granite
    turbulence 0.4
    color_map {
      [0.000, 0.202   color rgbf <1.000, 0.718, 0.541, 0.000>
                      color rgbf <0.890, 0.651, 0.612, 0.000>]
      [0.202, 0.298   color rgbf <0.890, 0.651, 0.612, 0.000>
                      color rgbf <1.000, 0.820, 0.675, 0.000>]
      [0.298, 0.377   color rgbf <1.000, 0.820, 0.675, 0.000>
                      color rgbf <0.890, 0.643, 0.612, 0.000>]
      [0.377, 0.465   color rgbf <0.890, 0.643, 0.612, 0.000>
                      color rgbf <0.937, 0.729, 0.561, 0.000>]
      [0.465, 0.544   color rgbf <0.937, 0.729, 0.561, 0.000>
                      color rgbf <0.878, 0.604, 0.565, 0.000>]
      [0.544, 0.640   color rgbf <0.878, 0.604, 0.565, 0.000>
                      color rgbf <0.984, 0.780, 0.655, 0.000>]
      [0.640, 0.860   color rgbf <0.984, 0.780, 0.655, 0.000>
                      color rgbf <1.000, 0.863, 0.635, 0.000>]
      [0.860, 0.982   color rgbf <1.000, 0.863, 0.635, 0.000>
                      color rgbf <1.000, 0.765, 0.620, 0.000>]
      [0.982, 1.001   color rgbf <1.000, 0.765, 0.620, 0.000>
                      color rgbf <1.000, 0.718, 0.541, 0.000>]
    }
  }
  scale <1.5, 3, 3>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.3 color rgb <0, 0, 0>
                color rgb <1, 1, 1>]
      [0.3, 0.4 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.4, 0.6 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.6, 1.0 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <2, 4, 1.3>
  rotate <0, 0, 30>
  finish{phong 1.0 phong_size 90}
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.55, 0.09, 0.09, 0.80>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 2, 1.4>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90 ambient 0.2}
}

//------------- Light Coffee Marble with darker patches
#declare Stone7 = texture {
  pigment {
    color rgb <0.435294, 0.258824, 0.258824>
  }
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.263   color rgbf <0.545, 0.380, 0.345, 0.733>
                      color rgbf <0.588, 0.475, 0.333, 0.741>]
      [0.263, 0.432   color rgbf <0.588, 0.475, 0.333, 0.741>
                      color rgbf <0.675, 0.478, 0.404, 0.545>]
      [0.432, 0.551   color rgbf <0.675, 0.478, 0.404, 0.545>
                      color rgbf <0.757, 0.635, 0.522, 0.384>]
      [0.551, 0.720   color rgbf <0.757, 0.635, 0.522, 0.384>
                      color rgbf <0.659, 0.549, 0.443, 0.675>]
      [0.720, 1.001   color rgbf <0.659, 0.549, 0.443, 0.675>
                      color rgbf <0.545, 0.380, 0.345, 0.733>]
    }
  }
  scale <1, 3, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.216   color rgbf <0.812, 0.812, 0.812, 0.835>
                      color rgbf <0.745, 0.843, 0.835, 0.847>]
      [0.216, 0.241   color rgbf <0.745, 0.843, 0.835, 0.847>
                      color rgbf <0.404, 0.337, 0.337, 0.463>]
      [0.241, 0.267   color rgbf <0.404, 0.337, 0.337, 0.463>
                      color rgbf <0.773, 0.729, 0.745, 0.622>]
      [0.267, 0.759   color rgbf <0.773, 0.729, 0.745, 0.622>
                      color rgbf <0.914, 0.843, 0.725, 0.651>]
      [0.759, 0.784   color rgbf <0.914, 0.843, 0.725, 0.651>
                      color rgbf <0.153, 0.133, 0.208, 0.437>]
      [0.784, 0.810   color rgbf <0.153, 0.133, 0.208, 0.437>
                      color rgbf <0.812, 0.812, 0.812, 0.835>]
      [0.810, 1.001   color rgbf <0.812, 0.812, 0.812, 0.835>
                      color rgbf <0.812, 0.812, 0.812, 0.835>]
    }
  }
  scale <3.5, 5, 4>
  rotate <0, 0, 60>
  finish{phong 1.0 phong_size 90}
}

//------------- Gray Granite with white patches
#declare Stone8 = texture {
  pigment{
    color rgb <1, 1, 1>
  }
  finish{ambient 0.4 crand 0.06 diffuse 0.7}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.153   color rgbf <0.729, 0.502, 0.451, 0.306>
                      color rgbf <0.769, 0.686, 0.592, 0.792>]
      [0.153, 0.398   color rgbf <0.769, 0.686, 0.592, 0.792>
                      color rgbf <0.843, 0.753, 0.718, 0.396>]
      [0.398, 0.559   color rgbf <0.843, 0.753, 0.718, 0.396>
                      color rgbf <0.780, 0.667, 0.561, 0.976>]
      [0.559, 0.729   color rgbf <0.780, 0.667, 0.561, 0.976>
                      color rgbf <0.741, 0.659, 0.576, 0.820>]
      [0.729, 1.001   color rgbf <0.741, 0.659, 0.576, 0.820>
                      color rgbf <0.729, 0.502, 0.451, 0.306>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, -30>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.216   color rgbf <0.812, 0.812, 0.812, 0.835>
                      color rgbf <0.745, 0.843, 0.835, 0.847>]
      [0.216, 0.241   color rgbf <0.745, 0.843, 0.835, 0.847>
                      color rgbf <0.404, 0.337, 0.337, 0.463>]
      [0.241, 0.267   color rgbf <0.404, 0.337, 0.337, 0.463>
                      color rgbf <0.773, 0.729, 0.745, 0.622>]
      [0.267, 0.759   color rgbf <0.773, 0.729, 0.745, 0.622>
                      color rgbf <0.914, 0.843, 0.725, 0.651>]
      [0.759, 0.784   color rgbf <0.914, 0.843, 0.725, 0.651>
                      color rgbf <0.153, 0.133, 0.208, 0.437>]
      [0.784, 0.810   color rgbf <0.153, 0.133, 0.208, 0.437>
                      color rgbf <0.812, 0.812, 0.812, 0.835>]
      [0.810, 1.001   color rgbf <0.812, 0.812, 0.812, 0.835>
                      color rgbf <0.812, 0.812, 0.812, 0.835>]
    }
  }
  scale <5, 3, 4>
  rotate <0, 0, 40>
  finish{phong 1.0 ambient 0.2 diffuse 0.5 phong_size 90}
}

//------------- White & Light Blue Marble with light violets
#declare Stone9 = texture {
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.894, 0.886, 0.886, 0.000>
                      color rgbf <0.745, 0.745, 0.753, 0.000>]
      [0.154, 0.308   color rgbf <0.745, 0.745, 0.753, 0.000>
                      color rgbf <0.902, 0.902, 0.859, 0.000>]
      [0.308, 0.444   color rgbf <0.902, 0.902, 0.859, 0.000>
                      color rgbf <0.729, 0.706, 0.694, 0.000>]
      [0.444, 0.615   color rgbf <0.729, 0.706, 0.694, 0.000>
                      color rgbf <0.588, 0.592, 0.635, 0.000>]
      [0.615, 0.803   color rgbf <0.588, 0.592, 0.635, 0.000>
                      color rgbf <0.608, 0.616, 0.659, 0.000>]
      [0.803, 1.001   color rgbf <0.608, 0.616, 0.659, 0.000>
                      color rgbf <0.894, 0.886, 0.886, 0.000>]
    }
  }
  scale <1.2, 2.4, 1.2>
  rotate <0, 0, -30>
  rotate <40, 0, 0>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.55, 0.09, 0.09, 0.80>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 2, 1.4>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90 ambient 0.2}
}

//------------- Dark Brown & Tan swirl Granite with gray undertones
#declare Stone10 = texture {
  pigment {
    color rgb <0, 0, 0>
  }
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.034   color rgbf <0.027, 0.012, 0.012, 0.000>
                      color rgbf <0.851, 0.812, 0.741, 0.235>]
      [0.034, 0.342   color rgbf <0.851, 0.812, 0.741, 0.235>
                      color rgbf <0.792, 0.694, 0.690, 0.839>]
      [0.342, 0.462   color rgbf <0.792, 0.694, 0.690, 0.839>
                      color rgbf <0.631, 0.506, 0.471, 0.608>]
      [0.462, 0.632   color rgbf <0.631, 0.506, 0.471, 0.608>
                      color rgbf <0.851, 0.812, 0.741, 0.922>]
      [0.632, 0.983   color rgbf <0.851, 0.812, 0.741, 0.922>
                      color rgbf <0.647, 0.655, 0.655, 0.282>]
      [0.983, 1.001   color rgbf <0.647, 0.655, 0.655, 0.282>
                      color rgbf <0.027, 0.012, 0.012, 0.000>]
    }
  }
  scale <3, 6, 2>
  rotate <0, 0, 50>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.153   color rgbf <0.831, 0.631, 0.569, 0.447>
                      color rgbf <0.925, 0.831, 0.714, 0.678>]
      [0.153, 0.297   color rgbf <0.925, 0.831, 0.714, 0.678>
                      color rgbf <0.871, 0.702, 0.659, 0.475>]
      [0.297, 0.441   color rgbf <0.871, 0.702, 0.659, 0.475>
                      color rgbf <0.831, 0.631, 0.569, 0.918>]
      [0.441, 0.763   color rgbf <0.831, 0.631, 0.569, 0.918>
                      color rgbf <0.937, 0.882, 0.820, 0.655>]
      [0.763, 1.001   color rgbf <0.937, 0.882, 0.820, 0.655>
                      color rgbf <0.831, 0.631, 0.569, 0.447>]
    }
  }
  scale <1, 2, 1>
  rotate <0, 0, -50>
  finish{phong 1.0 phong_size 90}
}

//------------- Rose & White Marble with dark tan swirl
#declare Stone11 = texture {
  pigment {
    color rgb <0, 0, 0>
  }
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.996, 0.824, 0.780, 0.412>
                      color rgbf <0.996, 0.698, 0.624, 0.412>]
      [0.154, 0.308   color rgbf <0.996, 0.698, 0.624, 0.412>
                      color rgbf <0.906, 0.675, 0.553, 0.616>]
      [0.308, 0.444   color rgbf <0.906, 0.675, 0.553, 0.616>
                      color rgbf <0.996, 0.682, 0.604, 0.412>]
      [0.444, 0.615   color rgbf <0.996, 0.682, 0.604, 0.412>
                      color rgbf <0.996, 0.824, 0.780, 0.412>]
      [0.615, 0.812   color rgbf <0.996, 0.824, 0.780, 0.412>
                      color rgbf <0.973, 0.627, 0.627, 0.600>]
      [0.812, 1.001   color rgbf <0.973, 0.627, 0.627, 0.600>
                      color rgbf <0.996, 0.824, 0.780, 0.412>]
    }
  }
  scale <1.2, 3, 1.5>
  rotate <70, 0, 30>
  finish{crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.144   color rgbf <0.863, 0.757, 0.596, 0.596>
                      color rgbf <0.925, 0.792, 0.714, 0.349>]
      [0.144, 0.288   color rgbf <0.925, 0.792, 0.714, 0.349>
                      color rgbf <0.871, 0.702, 0.659, 0.784>]
      [0.288, 0.644   color rgbf <0.871, 0.702, 0.659, 0.784>
                      color rgbf <0.992, 0.922, 0.659, 0.498>]
      [0.644, 0.983   color rgbf <0.992, 0.922, 0.659, 0.498>
                      color rgbf <0.902, 0.812, 0.714, 0.722>]
      [0.983, 1.001   color rgbf <0.902, 0.812, 0.714, 0.722>
                      color rgbf <0.863, 0.757, 0.596, 0.596>]
    }
  }
  scale <3, 3, 4>
  rotate <0, 0, 40>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.55, 0.09, 0.09, 0.80>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 2, 1.4>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90}
}

//------------- White & Pinkish Tan Marble
#declare Stone12 = texture {
  pigment {
    marble
    turbulence 0.5
    color_map {
      [0.000, 0.178   color rgbf <0.831, 0.631, 0.569, 0.000>
                      color rgbf <0.925, 0.831, 0.714, 0.000>]
      [0.178, 0.356   color rgbf <0.925, 0.831, 0.714, 0.000>
                      color rgbf <0.871, 0.702, 0.659, 0.000>]
      [0.356, 0.525   color rgbf <0.871, 0.702, 0.659, 0.000>
                      color rgbf <0.831, 0.631, 0.569, 0.000>]
      [0.525, 0.729   color rgbf <0.831, 0.631, 0.569, 0.000>
                      color rgbf <0.937, 0.882, 0.820, 0.000>]
      [0.729, 1.001   color rgbf <0.937, 0.882, 0.820, 0.000>
                      color rgbf <0.831, 0.631, 0.569, 0.000>]
    }
  }
  scale <1, 5, 1>
  rotate <0, 0, 50>
  finish{ambient 0.2 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.153   color rgbf <0.729, 0.502, 0.451, 0.306>
                      color rgbf <0.769, 0.686, 0.592, 0.792>]
      [0.153, 0.398   color rgbf <0.769, 0.686, 0.592, 0.792>
                      color rgbf <0.843, 0.753, 0.718, 0.396>]
      [0.398, 0.559   color rgbf <0.843, 0.753, 0.718, 0.396>
                      color rgbf <0.780, 0.667, 0.561, 0.976>]
      [0.559, 0.729   color rgbf <0.780, 0.667, 0.561, 0.976>
                      color rgbf <0.741, 0.659, 0.576, 0.820>]
      [0.729, 1.001   color rgbf <0.741, 0.659, 0.576, 0.820>
                      color rgbf <0.729, 0.502, 0.451, 0.306>]
    }
  }
  scale <1, 3, 2>
  rotate <0, 0, -30>
  finish{phong 1.0 phong_size 90}
}

//------------- Medium Gray Blue Marble
#declare Stone13 = texture {
  pigment {
    marble
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.894, 0.886, 0.886, 0.000>
                      color rgbf <0.745, 0.745, 0.753, 0.000>]
      [0.154, 0.308   color rgbf <0.745, 0.745, 0.753, 0.000>
                      color rgbf <0.902, 0.902, 0.859, 0.000>]
      [0.308, 0.444   color rgbf <0.902, 0.902, 0.859, 0.000>
                      color rgbf <0.729, 0.706, 0.694, 0.000>]
      [0.444, 0.615   color rgbf <0.729, 0.706, 0.694, 0.000>
                      color rgbf <0.588, 0.592, 0.635, 0.000>]
      [0.615, 0.803   color rgbf <0.588, 0.592, 0.635, 0.000>
                      color rgbf <0.608, 0.616, 0.659, 0.000>]
      [0.803, 1.001   color rgbf <0.608, 0.616, 0.659, 0.000>
                      color rgbf <0.894, 0.886, 0.886, 0.000>]
    }
  }
  scale <2, 5, 2>
  rotate <0, 0, 50>
  finish{ambient 0.2 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.119   color rgbf <0.310, 0.384, 0.420, 0.631>
                      color rgbf <0.322, 0.369, 0.416, 0.847>]
      [0.119, 0.322   color rgbf <0.322, 0.369, 0.416, 0.847>
                      color rgbf <0.424, 0.369, 0.420, 0.549>]
      [0.322, 0.449   color rgbf <0.424, 0.369, 0.420, 0.549>
                      color rgbf <0.373, 0.424, 0.518, 0.965>]
      [0.449, 0.729   color rgbf <0.373, 0.424, 0.518, 0.965>
                      color rgbf <0.482, 0.573, 0.533, 0.741>]
      [0.729, 1.001   color rgbf <0.482, 0.573, 0.533, 0.741>
                      color rgbf <0.310, 0.384, 0.420, 0.631>]
    }
  }
  scale <1, 3, 2>
  rotate <0, 0, -30>
  finish{phong 1.0 phong_size 90}
}

//------------- Tan & Olive Marble with gray white veins
#declare Stone14 = texture {
  pigment {
    granite
    turbulence 0.5
    color_map {
      [0.000, 0.153   color rgbf <0.545, 0.380, 0.345, 0.000>
                      color rgbf <0.588, 0.475, 0.333, 0.000>]
      [0.153, 0.398   color rgbf <0.588, 0.475, 0.333, 0.000>
                      color rgbf <0.675, 0.478, 0.404, 0.000>]
      [0.398, 0.559   color rgbf <0.675, 0.478, 0.404, 0.000>
                      color rgbf <0.757, 0.635, 0.522, 0.000>]
      [0.559, 0.729   color rgbf <0.757, 0.635, 0.522, 0.000>
                      color rgbf <0.659, 0.549, 0.443, 0.000>]
      [0.729, 1.001   color rgbf <0.659, 0.549, 0.443, 0.000>
                      color rgbf <0.545, 0.380, 0.345, 0.000>]
    }
  }
  scale <2, 3, 2>
  rotate <0, 0, -30>
  finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.3 color rgbf <1, 1, 1, 0.0>
                color rgbf <0.329412, 0.329412, 0.329412, 0.5>]
      [0.3, 0.4 color rgbf <0.329412, 0.329412, 0.329412, 0.5>
                color rgbf <0.329412, 0.329412, 0.329412, 0.8>]
      [0.4, 1.0 color rgbf <0.329412, 0.329412, 0.329412, 0.8>
                color rgbf <0.329412, 0.329412, 0.329412, 0.9>]
    }
  }
  finish {crand 0.02}
  scale <1, 3, 1>
  rotate <0, 0, 40>
  finish{phong 1.0 phong_size 90}
}

//------------- Deep Gray Marble with white veining
#declare Stone15 = texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.3 color rgb <0, 0, 0>
                color rgb <1, 1, 1>]
      [0.3, 0.4 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.4, 0.6 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.6, 1.0 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <1, 2, 2>
  rotate <0, 0, -30>
  finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.119   color rgbf <0.310, 0.384, 0.420, 0.631>
                      color rgbf <0.322, 0.369, 0.416, 0.847>]
      [0.119, 0.322   color rgbf <0.322, 0.369, 0.416, 0.847>
                      color rgbf <0.424, 0.369, 0.420, 0.549>]
      [0.322, 0.449   color rgbf <0.424, 0.369, 0.420, 0.549>
                      color rgbf <0.373, 0.424, 0.518, 0.965>]
      [0.449, 0.729   color rgbf <0.373, 0.424, 0.518, 0.965>
                      color rgbf <0.482, 0.573, 0.533, 0.741>]
      [0.729, 1.001   color rgbf <0.482, 0.573, 0.533, 0.741>
                      color rgbf <0.310, 0.384, 0.420, 0.631>]
    }
  }
  scale <1, 2, 1>
  rotate <0, 0, 40>
  finish{phong 1.0 phong_size 90}
}

//------------- Peach & Yellow Marble with white veining
#declare Stone16 = texture {
  pigment {
    granite
    turbulence 0.4
    color_map {
      [0.000, 0.202   color rgbf <1.000, 0.718, 0.541, 0.000>
                      color rgbf <0.890, 0.651, 0.612, 0.000>]
      [0.202, 0.298   color rgbf <0.890, 0.651, 0.612, 0.000>
                      color rgbf <1.000, 0.820, 0.675, 0.000>]
      [0.298, 0.377   color rgbf <1.000, 0.820, 0.675, 0.000>
                      color rgbf <0.890, 0.643, 0.612, 0.000>]
      [0.377, 0.465   color rgbf <0.890, 0.643, 0.612, 0.000>
                      color rgbf <0.937, 0.729, 0.561, 0.000>]
      [0.465, 0.544   color rgbf <0.937, 0.729, 0.561, 0.000>
                      color rgbf <0.878, 0.604, 0.565, 0.000>]
      [0.544, 0.640   color rgbf <0.878, 0.604, 0.565, 0.000>
                      color rgbf <0.984, 0.780, 0.655, 0.000>]
      [0.640, 0.860   color rgbf <0.984, 0.780, 0.655, 0.000>
                      color rgbf <1.000, 0.863, 0.635, 0.000>]
      [0.860, 0.982   color rgbf <1.000, 0.863, 0.635, 0.000>
                      color rgbf <1.000, 0.765, 0.620, 0.000>]
      [0.982, 1.001   color rgbf <1.000, 0.765, 0.620, 0.000>
                      color rgbf <1.000, 0.718, 0.541, 0.000>]
    }
  }
  scale <1.3, 2, 2>
  rotate <0, 0, -30>
  finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.3 color rgb <0, 0, 0>
                color rgb <1, 1, 1>]
      [0.3, 0.4 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.4, 0.6 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.6, 1.0 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <2, 4, 2>
  rotate <0, 0, -30>
  finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.2 color rgbf <1, 1, 1, 0.0>
                color rgbf <1, 1, 1, 0.7>]
      [0.2, 0.3 color rgbf <1, 1, 1, 0.7>
                color rgbf <1, 1, 1, 1>]
      [0.3, 1.0 color rgbf <1, 1, 1, 1>
                color rgbf <1, 1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <1, 2, 1>
  rotate <0, 0, 40>
  finish{phong 1.0 phong_size 90}
}

//------------- White Marble with gray veining
#declare Stone17 = texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.3 color rgb <0, 0, 0>
                color rgb <1, 1, 1>]
      [0.3, 0.4 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.4, 0.6 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
      [0.6, 1.0 color rgb <1, 1, 1>
                color rgb <1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <1, 2, 2>
  rotate <0, 0, -30>
  finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.184314, 0.309804, 0.184314, 0.80>
                   color rgbf <0.184314, 0.309804, 0.184314, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 2, 1.4>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90}
}

//------------- Green Jade with white veining
#declare Stone18 = texture {
  pigment {
    color rgb <0.137255, 0.556863, 0.419608>
  }
  finish{ambient 0.3 diffuse 0.6 crand 0.03}
}
texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.07 color rgb <1, 1, 1>
                 color rgb <1, 1, 1>]
      [0.07, 0.2 color rgb <1, 1, 1>
                 color rgb <0.329412, 0.329412, 0.329412>]
      [0.2, 0.3  color rgb <0.329412, 0.329412, 0.329412>
                 color rgbf <1, 1, 1, 1>]
      [0.3, 0.7  color rgbf <1, 1, 1, 1>
                 color rgbf <1, 1, 1, 1>]
      [0.7, 1.0  color rgbf <1, 1, 1, 1>
                 color rgb <0.329412, 0.329412, 0.329412>]
    }
  }
  finish {crand 0.02}
  scale <1.5, 0.7, 0.5>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.2 color rgbf <1, 1, 1, 0.0>
                color rgbf <1, 1, 1, 0.7>]
      [0.2, 0.3 color rgbf <1, 1, 1, 0.7>
                color rgbf <1, 1, 1, 1>]
      [0.3, 1.0 color rgbf <1, 1, 1, 1>
                color rgbf <1, 1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <2.5, 2, 0.5>
  rotate <0, 0, -50>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.03   color rgbf <1, 1, 1, 0.3>
                   color rgbf <1, 1, 1, 1.0>]
      [0.03, 0.97  color rgbf <0, 0, 0, 0.70>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <1, 1, 1, 0.9>
                   color rgbf <1, 1, 1, 1.0>]
    }
  }
  scale <0.7, 1, 1>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90}
}

//------------- Peach Granite with white patches & green trim
#declare Stone19 = texture{
  pigment {
    granite
    color_map {
      [0.000, 0.241   color rgbf <0.973, 0.973, 0.976, 0.000>
                      color rgbf <0.973, 0.973, 0.976, 0.000>]
      [0.241, 0.284   color rgbf <0.973, 0.973, 0.976, 0.000>
                      color rgbf <0.600, 0.741, 0.608, 0.000>]
      [0.284, 0.336   color rgbf <0.600, 0.741, 0.608, 0.000>
                      color rgbf <0.820, 0.643, 0.537, 0.000>]
      [0.336, 0.474   color rgbf <0.820, 0.643, 0.537, 0.000>
                      color rgbf <0.886, 0.780, 0.714, 0.000>]
      [0.474, 0.810   color rgbf <0.886, 0.780, 0.714, 0.000>
                      color rgbf <0.996, 0.643, 0.537, 0.000>]
      [0.810, 0.836   color rgbf <0.996, 0.643, 0.537, 0.000>
                      color rgbf <0.973, 0.973, 0.976, 0.000>]
      [0.836, 1.001   color rgbf <0.973, 0.973, 0.976, 0.000>
                      color rgbf <0.973, 0.973, 0.976, 0.000>]
    }
  }
  finish {crand 0.02}
  scale <1, 0.7, 0.5>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.2 color rgbf <1, 1, 1, 0.0>
                color rgbf <1, 1, 1, 0.7>]
      [0.2, 0.3 color rgbf <1, 1, 1, 0.7>
                color rgbf <1, 1, 1, 1>]
      [0.3, 1.0 color rgbf <1, 1, 1, 1>
                color rgbf <1, 1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <2, 3, 1>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90}
}

//------------- Brown & Olive Marble with white veining
#declare Stone20 = texture {
  pigment {
    granite
    color_map {
      [0.000, 0.043   color rgbf <0.773, 0.647, 0.569, 0.000>
                      color rgbf <0.431, 0.322, 0.227, 0.000>]
      [0.043, 0.113   color rgbf <0.431, 0.322, 0.227, 0.000>
                      color rgbf <0.278, 0.282, 0.216, 0.000>]
      [0.113, 0.304   color rgbf <0.278, 0.282, 0.216, 0.000>
                      color rgbf <0.278, 0.282, 0.216, 0.000>]
      [0.304, 0.426   color rgbf <0.278, 0.282, 0.216, 0.000>
                      color rgbf <0.459, 0.341, 0.243, 0.000>]
      [0.426, 0.843   color rgbf <0.459, 0.341, 0.243, 0.000>
                      color rgbf <0.459, 0.341, 0.243, 0.000>]
      [0.843, 0.878   color rgbf <0.459, 0.341, 0.243, 0.000>
                      color rgbf <0.459, 0.341, 0.243, 0.000>]
      [0.878, 0.983   color rgbf <0.459, 0.341, 0.243, 0.000>
                      color rgbf <0.278, 0.282, 0.216, 0.000>]
      [0.983, 1.001   color rgbf <0.278, 0.282, 0.216, 0.000>
                      color rgbf <0.773, 0.647, 0.569, 0.000>]
    }
  }
  scale <0.7, 0.99, 0.7>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.890, 0.690, 0.690, 0.659>
                      color rgbf <0.996, 0.835, 0.737, 0.659>]
      [0.154, 0.308   color rgbf <0.996, 0.835, 0.737, 0.659>
                      color rgbf <0.745, 0.635, 0.651, 0.780>]
      [0.308, 0.444   color rgbf <0.745, 0.635, 0.651, 0.780>
                      color rgbf <0.733, 0.596, 0.557, 0.686>]
      [0.444, 0.615   color rgbf <0.733, 0.596, 0.557, 0.686>
                      color rgbf <0.996, 0.835, 0.737, 0.659>]
      [0.615, 0.803   color rgbf <0.996, 0.835, 0.737, 0.659>
                      color rgbf <0.765, 0.616, 0.659, 0.761>]
      [0.803, 1.001   color rgbf <0.765, 0.616, 0.659, 0.761>
                      color rgbf <0.890, 0.690, 0.690, 0.659>]
    }
  }
  scale <1, 1.3, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.2 color rgbf <1, 1, 1, 0.0>
                color rgbf <1, 1, 1, 0.7>]
      [0.2, 0.3 color rgbf <1, 1, 1, 0.7>
                color rgbf <1, 1, 1, 1>]
      [0.3, 1.0 color rgbf <1, 1, 1, 1>
                color rgbf <1, 1, 1, 1>]
    }
  }
  finish {crand 0.02}
  scale <1.9, 3, 0.5>
  rotate <0, 0, -50>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.55, 0.09, 0.09, 0.80>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 0.6, 1>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90}
}

//------------- Red Marble with gray & white veining
#declare Stone21 = texture {
  pigment {
    marble
    turbulence 0.7
    color_map {
      [0.000, 0.155   color rgbf <0.686, 0.235, 0.282, 0.000>
                      color rgbf <0.686, 0.235, 0.282, 0.000>]
      [0.155, 0.328   color rgbf <0.686, 0.235, 0.282, 0.000>
                      color rgbf <0.494, 0.243, 0.294, 0.000>]
      [0.328, 0.474   color rgbf <0.494, 0.243, 0.294, 0.000>
                      color rgbf <0.769, 0.329, 0.373, 0.000>]
      [0.474, 0.647   color rgbf <0.769, 0.329, 0.373, 0.000>
                      color rgbf <0.769, 0.329, 0.373, 0.000>]
      [0.647, 0.810   color rgbf <0.769, 0.329, 0.373, 0.000>
                      color rgbf <0.686, 0.235, 0.282, 0.000>]
      [0.810, 0.922   color rgbf <0.686, 0.235, 0.282, 0.000>
                      color rgbf <0.792, 0.388, 0.427, 0.000>]
      [0.922, 1.001   color rgbf <0.792, 0.388, 0.427, 0.000>
                      color rgbf <0.686, 0.235, 0.282, 0.000>]
    }
  }
  finish {crand 0.03}
  scale <1.3, 2.5, 1.7>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.07 color rgb <1, 1, 1>
                 color rgb <1, 1, 1>]
      [0.07, 0.2 color rgb <1, 1, 1>
                 color rgb <0.329412, 0.329412, 0.329412>]
      [0.2, 0.3  color rgb <0.329412, 0.329412, 0.329412>
                 color rgbf <1, 1, 1, 1>]
      [0.3, 0.7  color rgbf <1, 1, 1, 1>
                 color rgbf <1, 1, 1, 1>]
      [0.7, 1.0  color rgbf <1, 1, 1, 1>
                 color rgb <0.329412, 0.329412, 0.329412>]
    }
  }
  finish {crand 0.02}
  scale <1, 2, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.03   color rgbf <1, 1, 1, 0.3>
                   color rgbf <1, 1, 1, 1.0>]
      [0.03, 0.97  color rgbf <0, 0, 0, 0.70>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <1, 1, 1, 0.9>
                   color rgbf <1, 1, 1, 1.0>]
    }
  }
  scale <1, 0.6, 1>
  rotate <10, 0, -20>
  finish{phong 1.0 phong_size 90}
}

//------------- Dark Tan Marble with gray & white veining
#declare Stone22 = texture {
  pigment {
    color rgb <0.82, 0.57, 0.46>
  }
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.119   color rgbf <0.310, 0.384, 0.420, 0.631>
                      color rgbf <0.322, 0.369, 0.416, 0.847>]
      [0.119, 0.322   color rgbf <0.322, 0.369, 0.416, 0.847>
                      color rgbf <0.424, 0.369, 0.420, 0.549>]
      [0.322, 0.449   color rgbf <0.424, 0.369, 0.420, 0.549>
                      color rgbf <0.373, 0.424, 0.518, 0.965>]
      [0.449, 0.729   color rgbf <0.373, 0.424, 0.518, 0.965>
                      color rgbf <0.482, 0.573, 0.533, 0.741>]
      [0.729, 1.001   color rgbf <0.482, 0.573, 0.533, 0.741>
                      color rgbf <0.310, 0.384, 0.420, 0.631>]
    }
  }
  scale <1, 2, 2>
  rotate <0, 0, 40>
}
texture{
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0, 0.07 color rgb <1, 1, 1>
                 color rgb <1, 1, 1>]
      [0.07, 0.2 color rgb <1, 1, 1>
                 color rgb <0.329412, 0.329412, 0.329412>]
      [0.2, 0.3  color rgb <0.329412, 0.329412, 0.329412>
                 color rgbf <1, 1, 1, 1>]
      [0.3, 0.7  color rgbf <1, 1, 1, 1>
                 color rgbf <1, 1, 1, 1>]
      [0.7, 1.0  color rgbf <1, 1, 1, 1>
                 color rgb <0.329412, 0.329412, 0.329412>]
    }
  }
  finish {crand 0.02}
  scale <2, 4, 1.5>
  rotate <0, 0, -50>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.03   color rgbf <1, 1, 1, 0.3>
                   color rgbf <1, 1, 1, 1.0>]
      [0.03, 0.97  color rgbf <0, 0, 0, 0.70>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <1, 1, 1, 0.9>
                   color rgbf <1, 1, 1, 1.0>]
    }
  }
  scale <1, 1, 1>
  rotate <10, 0, -40>
  finish{phong 1.0 phong_size 90}
}

//------------- Peach & Cream Marble with orange veining
#declare Stone23 = texture {
  pigment {
    marble
    turbulence 0.5
    color_map {
      [0.000, 0.053   color rgbf <0.784, 0.627, 0.522, 0.000>
                      color rgbf <0.784, 0.627, 0.624, 0.000>]
      [0.053, 0.263   color rgbf <0.784, 0.627, 0.624, 0.000>
                      color rgbf <0.824, 0.557, 0.376, 0.000>]
      [0.263, 0.281   color rgbf <0.824, 0.557, 0.376, 0.000>
                      color rgbf <0.643, 0.380, 0.376, 0.000>]
      [0.281, 0.325   color rgbf <0.643, 0.380, 0.376, 0.000>
                      color rgbf <0.839, 0.722, 0.722, 0.000>]
      [0.325, 0.711   color rgbf <0.839, 0.722, 0.722, 0.000>
                      color rgbf <0.784, 0.627, 0.522, 0.000>]
      [0.711, 0.798   color rgbf <0.784, 0.627, 0.522, 0.000>
                      color rgbf <0.769, 0.380, 0.376, 0.000>]
      [0.798, 0.895   color rgbf <0.769, 0.380, 0.376, 0.000>
                      color rgbf <0.824, 0.557, 0.376, 0.000>]
      [0.895, 0.982   color rgbf <0.824, 0.557, 0.376, 0.000>
                      color rgbf <0.784, 0.627, 0.522, 0.000>]
      [0.982, 1.001   color rgbf <0.784, 0.627, 0.522, 0.000>
                      color rgbf <0.784, 0.627, 0.522, 0.000>]
    }
  }
  scale <1, 1, 2>
  rotate <40, 0, 0>
  rotate <0, 0, 30>
}
texture{
  pigment {
    granite
    turbulence 0.5
    color_map {
      [0.000, 0.053   color rgbf <0.784, 0.627, 0.522, 0.500>
                      color rgbf <0.784, 0.627, 0.624, 0.500>]
      [0.053, 0.263   color rgbf <0.784, 0.627, 0.624, 0.500>
                      color rgbf <0.824, 0.557, 0.376, 0.500>]
      [0.263, 0.281   color rgbf <0.824, 0.557, 0.376, 0.500>
                      color rgbf <0.643, 0.380, 0.376, 0.500>]
      [0.281, 0.325   color rgbf <0.643, 0.380, 0.376, 0.500>
                      color rgbf <0.839, 0.722, 0.722, 0.500>]
      [0.325, 0.711   color rgbf <0.839, 0.722, 0.722, 0.500>
                      color rgbf <0.784, 0.627, 0.522, 0.500>]
      [0.711, 0.798   color rgbf <0.784, 0.627, 0.522, 0.500>
                      color rgbf <0.769, 0.380, 0.376, 0.500>]
      [0.798, 0.895   color rgbf <0.769, 0.380, 0.376, 0.500>
                      color rgbf <0.824, 0.557, 0.376, 0.500>]
      [0.895, 0.982   color rgbf <0.824, 0.557, 0.376, 0.500>
                      color rgbf <0.784, 0.627, 0.522, 0.500>]
      [0.982, 1.001   color rgbf <0.784, 0.627, 0.522, 0.500>
                      color rgbf <0.784, 0.627, 0.522, 0.500>]
    }
  }
  scale <2, 1, 2>
  rotate <40, 0, 0>
  rotate <0, 0, 30>
}
texture{
  pigment {
    marble
    turbulence 0.85
    color_map {
      [0.0, 0.04   color rgbf <0, 0, 0, 0.6>
                   color rgbf <0, 0, 0, 1.0>]
      [0.04, 0.97  color rgbf <0.55, 0.09, 0.09, 0.80>
                   color rgbf <0.329412, 0.329412, 0.329412, 0.90>]
      [0.97, 1.001 color rgbf <0, 0, 0, 0.9>
                   color rgbf <0, 0, 0, 1.0>]
    }
  }
  scale <1, 2, 1.5>
  rotate <0, 0, 40>
}
texture {
  pigment {
    color rgbf <1, 1, 0, 0.9>
  }
  finish{phong 1.0 phong_size 90}  // tint to liking
}

//------------- Green & Tan Moss Marble
#declare Stone24 = texture {
  pigment {
    marble
    turbulence 0.7
    color_map {
      [0.000, 0.168   color rgbf <0.824, 0.725, 0.584, 0.000>
                      color rgbf <0.514, 0.584, 0.533, 0.000>]
      [0.168, 0.301   color rgbf <0.514, 0.584, 0.533, 0.000>
                      color rgbf <0.298, 0.376, 0.318, 0.000>]
      [0.301, 0.398   color rgbf <0.298, 0.376, 0.318, 0.000>
                      color rgbf <0.263, 0.337, 0.282, 0.000>]
      [0.398, 0.558   color rgbf <0.263, 0.337, 0.282, 0.000>
                      color rgbf <0.431, 0.506, 0.451, 0.000>]
      [0.558, 0.655   color rgbf <0.431, 0.506, 0.451, 0.000>
                      color rgbf <0.529, 0.631, 0.471, 0.000>]
      [0.655, 0.735   color rgbf <0.529, 0.631, 0.471, 0.000>
                      color rgbf <0.333, 0.376, 0.318, 0.000>]
      [0.735, 0.823   color rgbf <0.333, 0.376, 0.318, 0.000>
                      color rgbf <0.298, 0.376, 0.318, 0.000>]
      [0.823, 0.876   color rgbf <0.298, 0.376, 0.318, 0.000>
                      color rgbf <0.416, 0.376, 0.318, 0.000>]
      [0.876, 0.929   color rgbf <0.416, 0.376, 0.318, 0.000>
                      color rgbf <0.416, 0.376, 0.318, 0.000>]
      [0.929, 1.001   color rgbf <0.416, 0.376, 0.318, 0.000>
                      color rgbf <0.824, 0.725, 0.584, 0.000>]
    }
  }
  scale <1, 1, 2>
  rotate <0, 0, 50>
  rotate <20, 0, 30>
}
texture{
  pigment {
    granite
    color_map {
      [0.000, 0.168   color rgbf <0.824, 0.725, 0.584, 0.600>
                      color rgbf <0.514, 0.584, 0.533, 0.600>]
      [0.168, 0.301   color rgbf <0.514, 0.584, 0.533, 0.600>
                      color rgbf <0.298, 0.376, 0.318, 0.600>]
      [0.301, 0.398   color rgbf <0.298, 0.376, 0.318, 0.600>
                      color rgbf <0.263, 0.337, 0.282, 0.700>]
      [0.398, 0.558   color rgbf <0.263, 0.337, 0.282, 0.700>
                      color rgbf <0.431, 0.506, 0.451, 0.600>]
      [0.558, 0.655   color rgbf <0.431, 0.506, 0.451, 0.600>
                      color rgbf <0.529, 0.631, 0.471, 0.500>]
      [0.655, 0.735   color rgbf <0.529, 0.631, 0.471, 0.500>
                      color rgbf <0.333, 0.376, 0.318, 0.700>]
      [0.735, 0.823   color rgbf <0.333, 0.376, 0.318, 0.700>
                      color rgbf <0.298, 0.376, 0.318, 0.600>]
      [0.823, 0.876   color rgbf <0.298, 0.376, 0.318, 0.600>
                      color rgbf <0.416, 0.376, 0.318, 0.500>]
      [0.876, 0.929   color rgbf <0.416, 0.376, 0.318, 0.500>
                      color rgbf <0.416, 0.376, 0.318, 0.600>]
      [0.929, 1.001   color rgbf <0.416, 0.376, 0.318, 0.600>
                      color rgbf <0.824, 0.725, 0.584, 0.700>]
    }
  }
  scale <2, 1, 2>
  rotate <40, 0, 0>
  rotate <0, 0, -30>
}
texture {
  pigment {
    color rgbf <0.752941, 0.752941, 0.752941, 0.8>
  }
  finish{phong 1.0 phong_size 90} //   tint to liking
}

// ----------------------------------------------------------------------------
// -- TEXTURES FOUND IN MIKE MILLER'S SCENERY FILES. --------------------------
// -- COMPILATION BY ALAIN LIORET ---------------------------------------------
// -- MODIFIED 12/02/96 BY DENIS OLIVIER --------------------------------------
// ----------------------------------------------------------------------------


#declare Explosion01 = texture {
   pigment { color rgbt 1 }
   halo {
      attenuating
      spherical_mapping
      linear
      turbulence 5.2824
      color_map {
         [0.00 0.20  color rgbt <0.95, 0.64, 0.14,  1.00>
                     color rgbt <0.95, 0.64, 0.14,  0.95>]
         [0.20 0.75  color rgbt <0.95, 0.63, 0.14,  0.52>
                     color rgbt <1.00, 0.60, 0.00,  0.89>]
         [0.75 1.00  color rgbt <0.95, 0.62, 0.15,  0.99>
                     color rgbt <0.00, 0.05, 0.05,  0.99>]
      }
      frequency 1.65
      scale 0.1
   }
   halo {
      emitting
      spherical_mapping
      linear
      turbulence 0.7785
      color_map {
         [0.00 0.20  color rgbt <0.00, 0.00, 0.00,  1.00>
                     color rgbt <0.75, 0.14, 0.10,  0.39>]
         [0.20 1.00  color rgbt <0.70, 0.14, 0.10,  0.37>
                     color rgbt <1.00, 0.50, 0.00,  0.48>]
      }
      frequency 3
      samples 20
      scale 0.95
   }
   finish  {
      ambient 0.0
      diffuse 1.0
   }
}


//---- Dragon Textures
#declare ChromeTexture = texture {
  pigment {
    color rgb <0.658824, 0.658824, 0.658824>
  }
  finish {
    ambient .3
    diffuse .6
    phong 1
    phong_size 20
    brilliance 10
    reflection .6
  }
}

#declare MessTexture = texture {
  pigment {
    color rgb <0.752941, 0.752941, 0.752941>
  }
  finish {
    ambient .2
    diffuse .5
    phong 1
    phong_size 50
    brilliance 9
    reflection .5
  }
  normal {waves .9}
  rotate <90 0 0>
  rotate <0 90 0>
  scale <1 1 .1>
}

//---  BodyFrame Texture

#declare Glass_1 = texture {
  pigment {
    color rgbf <1, 1, 1, 1>
  }
  finish {
    ambient 0.1
    diffuse 0.0
    reflection 0.2
    refraction 1.0
    ior 1.5
    phong 0.3
    phong_size 90
  }
}

// Canon AE-1 35MM Camera ...............10/15/92 M.Miller

//----Brush Silver
#declare Tex2 = texture {
  pigment {
    color rgb <0.658824, 0.658824, 0.658824>
  }
  finish {
    ambient .3
    diffuse .6
    phong 1
    phong_size 10
    brilliance 8
    reflection .2
    metallic
  }
}

//----Brush Silver
#declare B_Silver = texture {
  pigment {
    color rgb <0.658824, 0.658824, 0.658824>
  }
  finish {
    ambient .3
    diffuse .6
    phong 1
    phong_size 10
    brilliance 8
    reflection .2
    metallic
  }
}

//-------------- planet 7             .....miller 9/24/92
#declare nimrod = texture {
  pigment {
    gradient <0 1 0>
    colour_map {
      [0.000 0.308  colour rgbf <0.722, 0.722, 0.992, 0.000>
                    colour rgbf <0.333, 0.333, 0.584, 0.000>]
      [0.308 0.992  colour rgbf <0.333, 0.333, 0.584, 0.000>
                    colour rgbf <0.016, 0.016, 0.231, 0.000>]
      [0.992 1.001  colour rgbf <0.016, 0.016, 0.231, 0.000>
                    colour rgbf <0.722, 0.722, 0.992, 0.000>]
    }
  }
  finish {
    ambient 1
    diffuse 0
  }
  scale <1 2 1>
  rotate <0 0 180>
  translate <0 0 0>
}
texture {
  pigment {
    bozo
    turbulence 0.8
    colour_map {
      [0.000 0.252 colour rgbf <0.722, 0.722, 0.992, 0.890>
                   colour rgbf <0.333, 0.333, 0.584, 0.859>]
      [0.252 0.798 colour rgbf <0.333, 0.333, 0.584, 0.859>
                   colour rgbf <0.918, 0.922, 0.929, 0.000>]
      [0.798 0.992 colour rgbf <0.918, 0.922, 0.929, 0.000>
                   colour rgbf <0.529, 0.522, 0.733, 0.243>]
      [0.992 1.001 colour rgbf <0.529, 0.522, 0.733, 0.243>
                   colour rgbf <0.722, 0.722, 0.992, 0.890>]
    }
  }
  finish {
    ambient 1
    diffuse 0
  }
  translate <1 0 0>
}
texture {
  pigment {
    gradient <0 1 0>
    color_map {
      [0.0 0.3  color rgb <0, 0, 0>
                color rgb <0.309804, 0.184314, 0.309804>]
      [0.3 0.5  color rgb <0.309804,  0.184314, 0.309804>
                color rgbf <0.309804, 0.184314, 0.309804, 0.4>]
      [0.5 1.01 color rgbf <0.309804, 0.184314, 0.309804, 0.4>
                color rgbf <0.309804, 0.184314, 0.309804, 1.0>]
    }
  }
  finish {
    ambient 1
    diffuse 0
  }
  scale <1 2 1>
  translate <0 1.6 0>
}
texture {
  pigment {
    bozo
    turbulence 0.8
    colour_map {
      [0.000 0.252 colour rgbf < 0.722, 0.722, 0.992, 0.890>
                   colour rgbf < 0.333, 0.333, 0.584, 0.859>]
      [0.252 0.798 colour rgbf < 0.333, 0.333, 0.584, 0.859>
                   colour rgbf < 0.918, 0.922, 0.929, 0.000>]
      [0.798 0.992 colour rgbf < 0.918, 0.922, 0.929, 0.000>
                   colour rgbf < 0.529, 0.522, 0.733, 0.243>]
      [0.992 1.001 colour rgbf < 0.529, 0.522, 0.733, 0.243>
                   colour rgbf < 0.722, 0.722, 0.992, 0.890>]
    }
  }
  finish {
    ambient 1
    diffuse 0
  }
  scale <2 0.5 1>
  translate <0 -1 0>
}

#declare pop_wood = texture {
  pigment {
    marble
    turbulence 0.4
    color_map {
      [0.0 0.2  color rgb <0.54, 0.35, 0.25>
                color rgb <0.54, 0.35, 0.25>]
      [0.2 0.4  color rgb <0.54, 0.35, 0.25>
                color rgb <0.57, 0.37, 0.24>]
      [0.4 0.6  color rgb <0.57, 0.37, 0.24>
                color rgb <0.56, 0.31, 0.21>]
      [0.6 0.8  color rgb <0.56, 0.31, 0.21>
                color rgb <0.56, 0.31, 0.21>]
      [0.8 0.9  color rgb <0.56, 0.31, 0.21>
                color rgb <0.57, 0.37, 0.24>]
      [0.9 1.01 color rgb <0.57, 0.37, 0.24>
                color rgb <0.54, 0.35, 0.25>]
    }
  }
  finish {ambient 0.8 phong 0.5}
  scale <1,1,(1/13)>
}
texture {
  pigment {
    wood
    turbulence 0.5
    color_map {
      [0.0 0.05 color rgb <0.40, 0.21, 0.15>
                color rgbf <0.40, 0.21, 0.15, 0.5>]
      [0.05 0.2 color rgbf <0.40, 0.21, 0.15, 0.5>
                color rgbf <0.40, 0.24, 0.14, 1.0>]
      [0.2 0.7  color rgbf <0.40, 0.24, 0.14, 1.0>
                color rgbf <0.40, 0.24, 0.14, 0.8>]
      [0.7 0.8  color rgbf <0.30, 0.14, 0.02, 0.7>
                color rgbf <0.30, 0.14, 0.02, 1.0>]
      [0.8 0.9  color rgbf <0.30, 0.14, 0.02, 1.0>
                color rgbf <0.30, 0.14, 0.02, 0.7>]
      [0.9 1.01 color rgbf <0.30, 0.14, 0.02, 1.0>
                color rgbf <0.25, 0.13, 0.09, 0.5>]
    }
  }
  finish {ambient 0.9 phong 1.0}
  scale <(1/3),1,(1/15)>
}

// ------------------ MAKE HARDWOOD FLOOR ----------------------------------

#declare WT = texture {
  pigment {
    marble
    turbulence 0.25
    color_map {
      [0.0 0.2  color rgb <0.54, 0.35, 0.25>
                color rgb <0.54, 0.35, 0.25>]
      [0.2 0.4  color rgb <0.54, 0.35, 0.25>
                color rgb <0.57, 0.37, 0.24>]
      [0.4 0.6  color rgb <0.57, 0.37, 0.24>
                color rgb <0.56, 0.31, 0.21>]
      [0.6 0.8  color rgb <0.56, 0.31, 0.21>
                color rgb <0.56, 0.31, 0.21>]
      [0.8 0.9  color rgb <0.56, 0.31, 0.21>
                color rgb <0.57, 0.37, 0.24>]
      [0.9 1.01 color rgb <0.57, 0.37, 0.24>
                color rgb <0.54, 0.35, 0.25>]
    }
  }
  finish { ambient 0.2 diffuse .8}
  scale <1,1,(1/13)>
}
texture {
  pigment {
    wood
    turbulence 0.3
    color_map {
      [0.0 0.05 color rgb <0.40, 0.21, 0.15>
               color rgbf <0.40, 0.21, 0.15, 0.7>]
      [0.05 0.2 color rgbf <0.40, 0.21, 0.15, 0.7>
                color rgbf <0.40, 0.24, 0.14, 1.0>]
      [0.2 0.7  color rgbf <0.40, 0.24, 0.14, 1.0>
                color rgbf <0.40, 0.24, 0.14, 0.8>]
      [0.7 0.8  color rgbf <0.30, 0.14, 0.02, 0.7>
                color rgbf <0.30, 0.14, 0.02, 1.0>]
      [0.8 0.9  color rgbf <0.30, 0.14, 0.02, 1.0>
                color rgbf <0.30, 0.14, 0.02, 0.7>]
      [0.9 1.01 color rgbf <0.30, 0.14, 0.02, 1.0>
                color rgbf <0.25, 0.13, 0.09, 0.7>]
    }
  }
  finish {ambient 0.2 diffuse 0.7}
  scale <1,(1/3),1>
}
texture {
  pigment {
    wood
    turbulence 0.2
    color_map {
      [0.0 0.05 color rgb <0.40, 0.21, 0.15>
                color rgbf <0.40, 0.21, 0.15, 0.4>]
      [0.05 0.2 color rgbf <0.40, 0.21, 0.15, 0.4>
                color rgbf <0.40, 0.24, 0.14, 0.8>]
      [0.2 0.7  color rgbf <0.40, 0.24, 0.14, 0.8>
                color rgbf <0.40, 0.24, 0.14, 0.9>]
      [0.7 0.8  color rgbf <0.30, 0.14, 0.02, 0.8>
                color rgbf <0.30, 0.14, 0.02, 0.6>]
      [0.8 0.9  color rgbf <0.30, 0.14, 0.02, 0.6>
                color rgbf <0.30, 0.14, 0.02, 0.4>]
      [0.9 1.01 color rgbf <0.30, 0.14, 0.02, 0.5>
                color rgbf <0.25, 0.13, 0.09, 0.8>]
    }
  }
  finish {
    diffuse 0.7
    ambient 0.2
    phong 0.25
    phong_size 50
    reflection 0.3
  }
  scale <1 5 1>
}


// ----------------------------------- Truck Textures

#declare Truck_Glass = texture {
  pigment {
    color rgbf <0.95, 0.95, 0.95, .95>
  }
  finish {
    ambient .8
    diffuse .1
    phong 1
    phong_size 100
    reflection .2
    refraction .9
  }
}

#declare Bumble = texture {
  pigment {
    gradient <1 0 0>
    color_map {
      [0 .5 color rgb <0, 0, 0> color rgb <0, 0, 0>]
      [.5 1 color rgb <.9, .56, .1> color rgb <.9, .56, .1>]
    }
  }
  finish {
    ambient .1
    phong 1
    phong_size 70
  }
  translate <0 0 0>
  rotate <0 0 10>
}

// ---- Head Texture

#declare HeadCheese = texture {
  pigment {
    wood
    color_map {
      [0 .3  color rgb <0, 0, 0>
             color rgb <0.309804, 0.184314, 0.309804>]
      [.3 .6 color rgb <0.309804, 0.184314, 0.309804>
             color rgb <0.55, 0.09, 0.09>]
      [.6 .8 color rgb <0.55, 0.09, 0.09>
             color rgb <0.8, 0.498039, 0.196078>]
      [.8 1  color rgb <0.8, 0.498039, 0.196078>
             color rgb <.9, .85, .8>]
    }
  }
  finish {ambient .2 phong 1}
  scale <1,1,(1/8)>
}
texture {
  pigment {
    color rgbf <1, 1, 1, 1>
  }
  finish {
    ambient .1
    diffuse .5
    phong .7
    phong_size 80
  }
  normal {bumps 1 frequency .010}
  scale <(1/80),(1/80),(1/80)>
}

// ---- Alternative Head Pulpt

#declare HeadTexture = texture {
  pigment {
    gradient <0 0 1>
    color_map {
      [0 .5 color rgb <.9, .85, .8>
            color rgbf <1, 1, 1, 1>]
      [.5 1 color rgbf <1, 1, 1, 1>
            color rgbf <1, 1, 1, 1>]
    }
  }
  finish {
    ambient .1
    diffuse .4
    phong 1
  }
  scale <(1/2),(1/2),1>
  translate <0 0 -2>
}
texture {
  pigment {
    color rgbf <1, 1, 1, 1>
  }
  normal {bumps 1 frequency .010}
  scale <(1/20),(1/20),(1/20)>
}

// --- Bright Metallic Gold

#declare Or2 = texture {
  pigment {
    colour rgb <0.64, 0.51, 0.05>
  }
  finish {
    ambient 0.3
    diffuse 0.5
    reflection 0.6
    brilliance 8
    specular 0.5
  }
}

// ---- Textures For Pursian Pool

// --------- White Marble with Gray feather Viens

#declare Grnt20 = texture {
  pigment {
    granite
    turbulence 0.0
    color_map {
      [0.0 0.3 color rgb <0, 0, 0> color rgb <1, 1, 1>]
      [0.2 0.4 color rgb <1, 1, 1> color rgb <1, 1, 1>]
      [0.4 0.6 color rgb <1, 1, 1> color rgb <1, 1, 1>]
      [0.6 1.0 color rgb <1, 1, 1> color rgb <1, 1, 1>]
    }
  }
}

#declare ColumnTexture = texture {
  pigment {
    marble
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.894, 0.886, 0.886, 0.000>
                      color rgbf <0.745, 0.745, 0.753, 0.000>]
      [0.154, 0.308   color rgbf <0.745, 0.745, 0.753, 0.000>
                      color rgbf <0.902, 0.902, 0.859, 0.000>]
      [0.308, 0.444   color rgbf <0.902, 0.902, 0.859, 0.000>
                      color rgbf <0.729, 0.706, 0.694, 0.000>]
      [0.444, 0.615   color rgbf <0.729, 0.706, 0.694, 0.000>
                      color rgbf <0.588, 0.592, 0.635, 0.000>]
      [0.615, 0.803   color rgbf <0.588, 0.592, 0.635, 0.000>
                      color rgbf <0.608, 0.616, 0.659, 0.000>]
      [0.803, 1.001   color rgbf <0.608, 0.616, 0.659, 0.000>
                      color rgbf <0.894, 0.886, 0.886, 0.000>]
    }
  }
  scale <2, 5, 2>
  rotate <0, 0, 50>
  finish{ambient 0.2 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.119   color rgbf <0.310, 0.384, 0.420, 0.631>
                      color rgbf <0.322, 0.369, 0.416, 0.847>]
      [0.119, 0.322   color rgbf <0.322, 0.369, 0.416, 0.847>
                      color rgbf <0.424, 0.369, 0.420, 0.549>]
      [0.322, 0.449   color rgbf <0.424, 0.369, 0.420, 0.549>
                      color rgbf <0.373, 0.424, 0.518, 0.965>]
      [0.449, 0.729   color rgbf <0.373, 0.424, 0.518, 0.965>
                      color rgbf <0.482, 0.573, 0.533, 0.741>]
      [0.729, 1.001   color rgbf <0.482, 0.573, 0.533, 0.741>
                      color rgbf <0.310, 0.384, 0.420, 0.631>]
    }
  }
  scale <1, 3, 2>
  rotate <0, 0, -30>
}
texture {
  pigment {
    color rgbf <0.752941, 0.752941, 0.752941, .5>
  }
  finish {
    phong .5
    phong_size 150
    ambient .15
    diffuse .6
  }
}

#declare PoolTexture = texture {
  pigment {
    marble
    turbulence 0.6
    color_map {
      [0.000, 0.154   color rgbf <0.894, 0.886, 0.886, 0.000>
                      color rgbf <0.745, 0.745, 0.753, 0.000>]
      [0.154, 0.308   color rgbf <0.745, 0.745, 0.753, 0.000>
                      color rgbf <0.902, 0.902, 0.859, 0.000>]
      [0.308, 0.444   color rgbf <0.902, 0.902, 0.859, 0.000>
                      color rgbf <0.729, 0.706, 0.694, 0.000>]
      [0.444, 0.615   color rgbf <0.729, 0.706, 0.694, 0.000>
                      color rgbf <0.588, 0.592, 0.635, 0.000>]
      [0.615, 0.803   color rgbf <0.588, 0.592, 0.635, 0.000>
                      color rgbf <0.608, 0.616, 0.659, 0.000>]
      [0.803, 1.001   color rgbf <0.608, 0.616, 0.659, 0.000>
                      color rgbf <0.894, 0.886, 0.886, 0.000>]
    }
  }
  scale <2, 5, 2>
  rotate <0, 0, 50>
  finish{ambient 0.2 crand 0.03}
}
texture{
  pigment {
    granite
    turbulence 0.6
    color_map {
      [0.000, 0.119   color rgbf <0.310, 0.384, 0.420, 0.631>
                      color rgbf <0.322, 0.369, 0.416, 0.847>]
      [0.119, 0.322   color rgbf <0.322, 0.369, 0.416, 0.847>
                      color rgbf <0.424, 0.369, 0.420, 0.549>]
      [0.322, 0.449   color rgbf <0.424, 0.369, 0.420, 0.549>
                      color rgbf <0.373, 0.424, 0.518, 0.965>]
      [0.449, 0.729   color rgbf <0.373, 0.424, 0.518, 0.965>
                      color rgbf <0.482, 0.573, 0.533, 0.741>]
      [0.729, 1.001   color rgbf <0.482, 0.573, 0.533, 0.741>
                      color rgbf <0.310, 0.384, 0.420, 0.631>]
    }
  }
  scale <1, 3, 2>
  rotate <0, 0, -30>
}
texture {
  pigment {
    color rgbf <0.752941, 0.752941, 0.752941, .3>
  }
  finish {
    phong 1
    phong_size 100
    ambient .25
    diffuse .5
    reflection .2
  }
}

//----- The Atrium Textures

#declare GlassColor = texture {
  pigment {
    color rgbf <.82, .97, .82, .82>
  }
  finish {
    reflection .2
    refraction .9
    ambient .1
    diffuse 0
    phong .3
    phong_size 50
  }
}

#declare WaterColor1 = texture {
  pigment {
    color rgb <0.372549, 0.623529, 0.623529>
  }
  finish {
    ambient .2
    diffuse .7
    phong .3
    phong_size 20
    reflection .4
  }
}

//-------Gray Wrinkle Texture

#declare WrinkleGray = texture {
  pigment {
    color rgb <0.45, 0.4, 0.4>
  }
  finish {
    roughness 0.9
    ambient 0.3
    diffuse 0.7
    phong 0.5
    phong_size 10
  }
  normal {wrinkles 0.5}
  scale <0.1 0.1 0.1>
}

//-------DarkGray Wrinkle Texture

#declare DGray = texture {
  pigment {
    color rgb <0.25, 0.2, 0.2>
  }
  finish {
    roughness 0.9
    ambient 0.3
    diffuse 0.6
    phong 0.5
    phong_size 10
  }
  normal {wrinkles 0.5}
  scale <0.1 0.1 0.1>
}

//-------Dark Red Cloth Texture

#declare BELL = texture {
  pigment {
    color rgb <0.44, 0.0, 0.17>
  }
  finish {
    ambient 0.3
    diffuse 0.7
    phong 0.5
    phong_size 10
  }
  normal {wrinkles 0.25}
  scale <0.1 0.1 0.1>
}

//------- Metallic Chrome

#declare Chrome = texture {
  pigment {
    color rgb <0.84, 0.84, 0.84>
  }
  finish {
    ambient 0.3
    diffuse 0.3
    roughness 0.9
    metallic
    brilliance 7.5
    reflection 0.3
    phong 0.7
    phong_size 10
  }
  scale <0.1 0.1 0.1>
}

//------- Metallic Gold

#declare  GOLD = texture {
  pigment {
    color rgb <0.94, 0.64, 0.20>
  }
  finish {
    ambient 0.3
    diffuse 0.3
    roughness 0.9
    metallic
    brilliance 7.5
    reflection 0.3
    phong 0.7
    phong_size 10
  }
  scale <0.1 0.1 0.1>
}

#declare WOOD11 = texture {
  pigment {
    wood
    turbulence 0.04
    octaves 3
    color_map {
      [0.00, 0.10 color rgb <0.888, 0.600, 0.3>
                  color rgb <0.888, 0.600, 0.3>]
      [0.10, 0.90 color rgb <0.888, 0.600, 0.3>
                  color rgb <0.6, 0.4, 0.2>]
      [0.90, 1.0  color rgb <0.6, 0.4, 0.2>
                  color rgb <0.6, 0.4, 0.2>]
    }
  }
  finish {
    ambient 0.3
    diffuse 0.7
    phong 0.7
    phong_size 60
  }
  scale <(1/32),(5/100),(3/8)>
  rotate <0 0 0>
}

#declare gold2 = texture {
  pigment {
    colour rgb <0.64, 0.51, 0.05>
  }
  finish {
    ambient 0.3
    diffuse 0.5
    reflection 0.6
    brilliance 8
    specular 0.5
  }
}

//------ Back Sky

#declare Clouds1 = texture {
  pigment {
    bozo
    turbulence 0.8
    colour_map {
      [0.0 0.6   colour rgbf <1.0, 1.0, 1.0, 1.0>
                 colour rgbf <1.0, 1.0, 1.0, 1.0>]
      [0.6 0.8   colour rgbf <0.96, 0.59, 0.66, 1.0>
                 colour rgb <0.96, 0.59, 0.66>]
      [0.8 1.001 colour rgb <0.96, 0.59, 0.66>
                 colour rgb <0.329412, 0.329412, 0.329412> ]
    }
  }
  scale <1  0.5  1>
}

#declare C1b = texture {
  pigment {
    color rgb <0.91, 0.76, 0.65>
  }
  finish {
    ambient .5
    diffuse .6
    phong 1
  }
}

// ----------------------------------------------------------------------------
// ------------ JUSSI KANTOLA TEXTURES ----------------------------------------
// ----------------------------------------------------------------------------

#declare Sunny_Cloud_Sky = texture {
  pigment {
    bozo
    turbulence 0.5
    octaves 3   /* doesn't have much effect       */
    lambda 6    /* bigger values are worth trying*/
    color_map {
      [0.0 0.1 color rgb <2/3, 2/3, 1>
               color rgb <2/3, 2/3, 1> ]
      [1.0     color rgb <1, 1, 1,> ]
    }
  }
  finish { ambient 1 diffuse 0 }
  scale <0.1666,0.1666,0.1666>   /* please notice*/
}

#declare Tropics_Cloud_Sky = texture {
  pigment {
    bozo
    turbulence 0.5
    octaves 3   /* doesn't have much effect       */
    lambda 6    /* bigger values are worth trying*/
    color_map {
      [0.0 0.1 color rgb <0.196078, 0.6, 0.8>
               color rgb <0.196078, 0.6, 0.8>]
      [1.0     color rgb <1, 1, 1,>]
    }
  }
  finish { ambient 1 diffuse 0 }
  scale <1,1,1>/6   /* please notice*/
}

#declare Carribean_Cloud_Sky = texture {
  pigment {
    bozo
    turbulence 0.5
    octaves 3   /* doesn't have much effect       */
    lambda 6    /* bigger values are worth trying*/
    color_map {
      [0.0 0.1 color rgb <0.196078, 0.6, 0.8>
               color rgb <0.196078, 0.6, 0.8>]
      [0.70    color rgb <4/5, 4/5, 5/6>]
      [1.0     color rgb <1, 1, 1,>]
    }
  }
  finish { ambient 1 diffuse 0 }
  scale <1,1,1>/6   /* please notice*/
}

#declare Stormy_Cloud_Sky = texture {
  pigment {
    bozo
    turbulence 0.5
    octaves 3   /* doesn't have much effect       */
    lambda 6    /* bigger values are worth trying*/
    color_map {
      [0.0 0.1 color rgbf <1, 1, 1, 1>
               color rgbf <1, 1, 1, 1>]
      [0.70    color rgbf <4/5, 4/5, 5/6, 1/2>]
      [1.0     color rgb <1, 1, 1>]
    }
  }
  finish { ambient 1 diffuse 0 }
  scale <1,1,1>/6   /* please notice*/
}

// ----------------------------------------------------------------------------
// ------------ J.PETERSON TEXTURES -------------------------------------------
// ----------------------------------------------------------------------------

#declare Streaky_Steel_Metal = texture {
  pigment { color rgb <.75, .75, .75> }
  finish {
    specular .7
    diffuse .4
    brilliance 6
    roughness .0082
    reflection .32
  }
  normal {
    wrinkles .6
    scale <.051, 30, .051>
    rotate <0, 0, 45>
  }
}

// ----------------------------------------------------------------------------
// ------------ ERIC WEBER TEXTURES -------------------------------------------
// ----------------------------------------------------------------------------

#declare EW_Stone = texture {
  pigment {
    crackle
    turbulence 0.3
    lambda 2.5
    omega 0.75
    octaves 5
    scale 0.45
    rotate <0, 5, 60>
    color_map {
      [0.02 color rgbf <0.30, 0.60, 0.45, 0.0> ]
      [0.06 color rgbf <0.35, 0.55, 0.40, 0.0> ]
      [0.10 color rgbf <0.25, 0.25, 0.20, 0.0> ]
      [1.00 color rgbf <0.75, 0.75, 0.60, 0.0> ]
    }
  }
  finish {
    ambient 0.0
    diffuse 1.0
    // brilliance 12.0
    // metallic
    specular 2.0
    roughness 0.001
    reflection 0.75
    irid {
      0.25
      thickness 0.2
      turbulence 0.4
    }
  }
  normal {
    crackle
    scale <0.5, 0.5, 0.5>
    turbulence 0.4
  }
}

#declare Solar = texture {
  pigment {
    bozo
    turbulence -3
    octaves 2
    lambda 6
    color_map {
      [0.0 0.2 color rgb <0.556863, 0.137255, 0.137255>
               color rgb <0.556863, 0.137255, 0.137255>]
      [0.2 1.0 color rgb <0.556863, 0.137255, 0.137255>
               color rgb <1, 0.5, 0.0>]
    }
  }
  finish {
    ambient 1.0
    diffuse 0.0
  }
}

#declare Solarized = texture {
  pigment {
    bozo
    turbulence -3
    octaves 2
    lambda 6
    color_map {
      [0.0 0.2 color rgb <0.556863, 0.137255, 0.137255>
               color rgb <0.556863, 0.137255, 0.137255>]
      [0.2 1.0 color rgb <0.556863, 0.137255, 0.137255>
               color rgb <1, 0.5, 0.0>]
    }
  }
  finish {
    ambient 0.0
    diffuse 1.0
    metallic
    reflection 0.1
    brilliance 2
    specular 0.45
    roughness 0.1
  }
  normal {
    dents 1.0
    scale <.25, .25, .25>
  }
}

#declare Plasmoid = texture {
  pigment {
    agate
    turbulence -3
    octaves 6
    omega .001
    lambda 1
    color_map {
      [0.0, 0.3 color rgbf <0.30, 0.30, 1.00, 0.9>
                color rgbf <0.00, 0.00, 0.61, 0.99>]
      [0.3, 0.5 color rgbf <0.00, 0.00, 0.61, 0.99>
                color rgbf <0.30, 0.30, 1.00, 0.9>]
      [0.5, 0.7 color rgbf <0.30, 0.30, 1.00, 0.9>
                color rgbf <0.00, 0.00, 0.61, 0.99>]
      [0.7, 1.0 color rgbf <0.00, 0.00, 0.61, 0.99>
                color rgbf <0.30, 0.30, 1.00, 0.9>]
    }
    scale <2, 2, 2>
  }
  finish  {
    ambient 1.0
    diffuse 0.0
    refraction 1.0
    ior 2.0
  }
}

#declare EWBlues = texture {
  pigment {
    marble
    turbulence .6
    octaves 3
    omega .2
    lambda 10
    color_map {
      [0.0, 0.7 color rgb <0, 0, 0>
                color rgb <0.00, 0.00, 0.61>]
      [0.7, 1.0 color rgb <0.00, 0.00, 0.61>
                color rgb <0, 0, 0>]
    }
  }
  finish  {
    ambient 0.0
    diffuse 1.0
    reflection 0.1
    specular 1.0
    roughness 0.001
  }
}

#declare EWReds = texture {
  pigment {
    marble
    turbulence .6
    octaves 3
    omega .2
    lambda 10
    color_map {
      [0.0, 0.7 color rgb <0, 0, 0>
                color rgb <0.55, 0.09, 0.09>]
      [0.7, 1.0 color rgb <0.55, 0.09, 0.09>
                color rgb <0, 0, 0>]
    }
  }
  finish {
    ambient 0.0
    diffuse 1.0
    reflection 0.1
    specular 1.0
    roughness 0.001
  }
}

#declare EWGreens = texture {
  pigment { marble
    turbulence .6
    octaves 3
    omega .2
    lambda 10
    color_map {
      [0.0, 0.7 color rgb <0, 0, 0>
                color rgb <0.137255, 0.556863, 0.419608>]
      [0.7, 1.0 color rgb <0.137255, 0.556863, 0.419608>
                color rgb <0, 0, 0>]
    }
  }
  finish {
    ambient 0.0
    diffuse 1.0
    reflection 0.1
    specular 1.0
    roughness 0.001
  }
}

#declare Trailers = texture {
  pigment {
    bozo
    turbulence .4
    octaves 3
    omega .2
    lambda 10
    color_map {
      [0.0, 0.4 color rgb <0.137255, 0.419608, 0.556863>
                color rgb <1, 1, 1,> ]
      [0.6, 1.0 color rgb <1, 1, 1,>
                color rgb <0.137255, 0.419608, 0.556863> ]
    }
    scale <2, .5, .5>
  }
  finish {
    ambient 1.0
    diffuse 0.0
  }
}

// ----------------------------------------------------------------------------
// ------------ LEE BRYARS TEXTURES -------------------------------------------
// ----------------------------------------------------------------------------

// Aberdeen Pink Granite Rough
#declare AberGranitePR = texture {
  pigment {
    granite
    turbulence 0.70
    color_map {
      [0.00 color rgb<0.20, 0.20, 0.20>]
      [0.31 color rgb<0.57, 0.53, 0.51>]
      [1.00 color rgb<0.77, 0.66, 0.64>]
    }
  }
  normal {
    bumps 0.10
    scale <0.20,0.20,0.20>
  }
  finish {
    ambient 0.50
    diffuse 1.00
    crand 0.250
  }
}

// Aberdeen Grey Granite Rough
#declare AberGraniteGR = texture {
  pigment {
    granite
    turbulence 0.70
    color_map {
      [0.00 color rgb<0.20, 0.20, 0.20>]
      [0.31 color rgb<0.57, 0.56, 0.56>]
      [1.00 color rgb<0.77, 0.77, 0.76>]
    }
  }
  normal {
    bumps 0.10
    scale <0.20,0.20,0.20>
  }
  finish {
    ambient 0.50
    diffuse 1.00
    crand 0.250
  }
}

//Aberdeen Grey Granite Polished
#declare AberGrGP = texture {
  pigment {
    granite
    turbulence 0.70
    color_map {
      [0.00 color rgb<0.20, 0.20, 0.20>]
      [0.31 color rgb<0.57, 0.56, 0.56>]
      [1.00 color rgb<0.77, 0.77, 0.76>]
    }
  }
  normal {
    bumps 0.10
    scale <0.20, 0.20, 0.20>
  }
  finish {
    ambient 0.50
    diffuse 1.00
    specular 0.50
    roughness 0.01
    metallic
    phong 0.08
    reflection 0.12
    crand 0.250
  }
}
texture {
  pigment {
    colour rgbf <1.00, 1.00, 1.00, 1.00>
  }
  finish {
    ambient 0.50
    brilliance 1.50
    phong 0.90
    phong_size 80
  }
}

// Aberdeen Pink Granite Polished
#declare AberGrPP = texture {
  pigment {
    granite
    turbulence 0.70
    color_map {
      [0.00 color rgb<0.20, 0.20, 0.20>]
      [0.31 color rgb<0.57, 0.53, 0.51>]
      [1.00 color rgb<0.77, 0.66, 0.64>]
    }
  }
  normal {
    bumps 0.10
    scale <0.20, 0.20, 0.20>
  }
  finish {
    ambient 0.50
    diffuse 1.00
    specular 0.50
    roughness 0.01
    metallic
    reflection 0.11
    crand 0.250
  }
}
texture {
  pigment {
    colour rgbf<1.00, 1.00, 1.00, 1.00>
  }
  finish {
    ambient 0.50
    brilliance 1.50
    phong 0.90
    phong_size 80
  }
}

// Wierd Texture!
#declare Cool = texture {
  pigment {
    agate
    color_map {
      [0.00 color rgbf <0.00, 0.00, 0.00, 1.00>]
      [0.30 color rgbf <0.00, 0.00, 0.30, 1.00>]
      [0.30 color rgbf <0.00, 0.00, 0.29, 1.00>]
      [1.00 color rgb <0.00, 0.00, 1.00>]
    }
  }
  finish {
    ambient 0.20
    diffuse 0.80
    specular 0.60
    metallic
  }
}
texture {
  pigment {
    leopard
    color_map {
      [0.00 color rgbf <1.00, 0.00, 0.00, 0.40>]
      [0.04 color rgbf <1.00, 0.00, 0.00, 1.00>]
      [0.40 color rgb <0.00, 0.98, 0.00>]
      [1.00 color rgb <1.00, 0.00, 0.00>]
    }
  }
  normal {
    ripples 1.00
    frequency 5
  }
  finish {
    diffuse 0.65
    specular 1.00
    reflection 0.35
  }
}

#declare Electricity = texture {
  pigment {
    gradient y
    turbulence 0.60
    lambda 2.25
    color_map {
      [0.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.30 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.30 color rgb <0.15, 0.00, 1.00>]
      [0.50 color rgb <1.00, 1.00, 1.00>]
      [0.70 color rgb <0.15, 0.00, 1.00>]
      [0.70 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [1.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
    }
    scale <1.25,1.00,1.00>
  }
  finish {
    ambient 0.40
    diffuse 0.80
  }
}

// This texture is rather difficult to use since you should
// scale it to fit your object exactly.
#declare Fire = texture {
  pigment {
    gradient y
    turbulence 0.20
    omega 0.80
    color_map {
      [0.00 color rgb <1.00, 1.00, 1.00>]
      [0.08 color rgbf <0.98, 1.00, 0.02, 0.08>]
      [0.29 color rgbf <1.00, 0.74, 0.01, 0.40>]
      [0.53 color rgbf <1.00, 0.16, 0.01, 0.67>]
      [0.79 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [1.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
    }
    scale <3.00, 7.50, 3.00>
  }
  finish {
    ambient 1.00
  }
  translate <0.00, -3.25, 0.00>
}

// Water with the reflection (simulated) of clouds!
#declare Cloud_Water = texture {
  pigment {
    bozo
    turbulence 0.80
    color_map {
      [0.00 color rgb <0.00, 0.00, 1.00>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
  }
  scale <6.00, 1.70, 2.00>
}
texture {
  pigment {
    colour rgbf <1.00,1.00,1.00,1.00>
  }
  normal {
    waves 1.00
    frequency 5
    scale <1.00, 1.00, 1.50>
  }
  finish {
    specular 0.80
  }
}

// Looks like one of those balls that you play with as a kid.
#declare Bouncy_Ball = texture {
  pigment {
    gradient y
    turbulence 0.75
    color_map {
      [0.00 color rgb <1.00, 0.00, 0.00>]
      [0.39 color rgb <1.00, 1.00, 0.00>]
      [0.78 color rgb<1.00, 1.00, 1.00>]
      [1.00 color rgb<0.00, 0.00, 0.00>]
    }
    rotate <-45.00, 0.00, 0.00>
    scale <1.00, 3.00, 1.00>
  }
  normal {
    ripples 0.60
    turbulence 0.20
    lambda 1.50
    frequency 10
  }
  finish {
    ambient 0.15
    diffuse 0.80
    specular 0.15
  }
}

#declare Funky_Metal = texture {
  pigment {
    wood
    turbulence 0.02
    octaves 4
    omega 1.00
    lambda 1.00
    color_map {
      [0.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.36 color rgbf <0.00, 1.00, 1.00, 0.13>]
      [1.00 color rgb <0.00, 0.00, 1.00>]
    }
  }
  finish {
    ambient 0.15
    diffuse 0.80
    specular 0.90
    roughness 0.10
    metallic
  }
}

// Purple and red rings.  Nice special effect!
#declare Wierd_Rings = texture {
  pigment {
    leopard
    turbulence 0.10
    lambda 1.00
    frequency 5
    color_map {
      [0.00 color rgbf <0.50, 0.00, 1.00, 0.38>]
      [0.16 color rgb <0.00, 0.50, 0.50>]
      [0.16 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.37 color rgb <1.00, 0.00, 0.00>]
      [0.53 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.73 color rgbf <0.00, 0.00, 0.50, 1.00>]
      [1.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
    }
    rotate <30.00, 0.00, 0.00>
  }
  normal {
    ripples 0.80
    turbulence 0.20
    lambda 2.50
    frequency 10
  }
  finish {
    ambient 0.25
    diffuse 0.70
    specular 0.71
    metallic
  }
}

#declare Orb = texture {
  pigment {
    bozo
    turbulence 0.40
    color_map {
      [0.00 color rgbf <0.38, 0.68, 0.84, 0.78>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
  }
  normal {
    waves 1.00
    turbulence 0.40
    octaves 5
    omega 0.70
    lambda 1.00
    frequency 10
    scale <4.00, 4.00, 7.00>
  }
  finish {
    ambient 0.30
    diffuse 0.80
    specular 0.60
  }
}

#declare Jelly_Fish = texture {
  pigment {
    leopard
    color_map {
      [0.00 color rgbf <0.94, 0.93, 0.77, 0.39>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
    frequency 5
  }
  normal
  {
    ripples 1.00
    frequency 10
  }
  finish {
    ambient 0.30
    diffuse 0.80
    specular 0.70
  }
}

// This is a good asteroid texture.
#declare Asteroid = texture {
  pigment {
    bozo
    turbulence 0.60
    omega 0.80
    lambda 3.00
    color_map {
      [0.00 color rgb <0.51, 0.51, 0.51>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
  }
  normal {
    bumps 1.00
    turbulence 0.60
    octaves 5
    omega 0.65
    lambda 3.00
    scale <0.75, 0.75, 1.00>
  }
  finish {
    ambient 0.15
    specular 0.20
  }
}

// This is a good asteroid texture.
// Modified 18.07.96 by DO
#declare Asteroid2 = texture {
  pigment {
    bozo
    turbulence 0.60
    omega 0.80
    lambda 3.00
    color_map {
      [0.00 color rgb <0.51, 0.51, 0.1>]
      [1.00 color rgb <1.00, 1.00, 0.6>]
    }
  }
  normal {
    bumps 1.00
    turbulence 0.60
    octaves 5
    omega 0.65
    lambda 3.00
    scale <0.75, 0.75, 1.00>
  }
  finish {
    ambient 0.15
    specular 0.20
  }
}

#declare Shiny_Orb = texture {
  pigment {
    bozo
    turbulence 0.40
    color_map {
      [0.00 color rgbf <0.38, 0.68, 0.84, 0.78>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
  }
  normal {
    waves 1.00
    turbulence 0.40
    octaves 5
    omega 0.70
    lambda 1.00
    frequency 10
    scale <4.00, 4.00, 7.00>
  }
  finish {
    diffuse 0.75
    specular 0.60
    reflection 0.20
  }
}

// This texture is rather difficult to use since you should
// scale it to fit your object exactly.
#declare Fire_Map = texture {
  pigment {
    wood
    turbulence 0.20
    omega 0.80
    color_map {
      [0.00 color rgb <1.00, 1.00, 1.00>]
      [0.08 color rgbf <0.98, 1.00, 0.02, 0.08>]
      [0.29 color rgbf <1.00, 0.74, 0.01, 0.40>]
      [0.53 color rgbf <1.00, 0.16, 0.01, 0.67>]
      [0.79 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [1.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
    }
    scale <3.00, 7.50, 3.00>
  }
  finish {
    ambient 1.00
  }
  translate <0.00, -3.25, 0.00>
}

// Blue Plasma texture by Iain Young
#declare Plasma = texture {
  pigment {
    marble
    turbulence 0.50
    color_map {
      [0.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.40 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.40 color rgbf <0.40, 0.40, 1.00, 1.00>]
      [0.50 color rgb <0.90, 0.90, 1.00>]
      [0.60 color rgbf <0.40, 0.40, 1.00, 1.00>]
      [0.60 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [1.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
    }
    scale <2.00, 2.00, 2.00>
  }
  finish {
    ambient 1.00
  }
}

// By Bradley Gawthrop-Like the inner walls of some kinds of
// geods really nice, PLAY WITH THE TURBULENCE you'd be amazed
// what neat new effects can be gained by doing so.
#declare Geod = texture {
  pigment {
    onion
    turbulence 0.60
    color_map {
      [0.00 color rgb <0.00, 0.00, 0.00>]
      [0.10 color rgb <0.83, 0.79, 1.00>]
      [0.20 color rgb <0.63, 0.62, 0.83>]
      [0.30 color rgbf <0.46, 0.44, 0.65, 1.00>]
      [0.40 color rgb <0.29, 0.27, 0.48>]
      [0.50 color rgbf <0.11, 0.11, 0.30, 1.00>]
      [0.60 color rgb <0.25, 0.24, 0.38>]
      [0.70 color rgbf <0.41, 0.37, 0.46, 1.00>]
      [0.80 color rgb <0.57, 0.49, 0.54>]
      [0.90 color rgbf <0.73, 0.62, 0.62, 1.00>]
      [1.00 color rgb <0.89, 0.75, 0.70>]
    }
  }
  finish {
    specular 0.20
  }
}

// Looks neat with a light source in the center of it and
// appropriately rotated 'round. By David Anjo.
#declare Russ_Bright = texture {
  pigment {
    marble
    turbulence 0.20
    color_map {
      [0.00 color rgbf <1.00, 0.00, 0.49, 0.80>]
      [0.20 color rgbf <1.00, 0.50, 0.00, 0.80>]
      [0.20 color rgbf <1.00, 1.00, 0.00, 0.80>]
      [0.40 color rgbf <1.00, 0.00, 0.50, 0.80>]
      [0.40 color rgbf <1.00, 1.00, 0.00, 0.80>]
      [0.50 color rgbf <0.80, 0.50, 0.20, 0.80>]
      [0.60 color rgbf <1.00, 0.50, 0.00, 0.80>]
      [0.60 color rgbf <1.00, 1.00, 0.00, 1.00>]
      [1.00 color rgb <1.00, 0.50, 0.00>]
    }
  }
  finish {
    specular 0.85
    reflection 0.55
  }
}

// Nice amber by David Anjo
#declare Amber = texture {
  pigment {
    marble
    turbulence 0.11
    color_map {
      [0.00 color rgb <0.95, 0.72, 0.05>]
      [0.80 color rgbf <1.00, 0.50, 0.50, 0.90>]
      [0.80 color rgb <1.00, 0.80, 0.80>]
      [1.00 color rgbf <1.00, 0.65, 0.65, 0.90>]
    }
  }
}

#declare Light_Clouds = texture {
  pigment {
    bozo
    turbulence 0.80
    color_map {
      [0.00 color rgb <0.00, 0.00, 1.00>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
  }
  finish {
    ambient 1.00
    diffuse 0.00
  }
  scale <6000.00, 1700.00, 2000.00>
}
#declare Late_Sunset = texture {
  pigment {
    bozo
    color_map {
      [0.00 color rgb <0.87, 0.40, 0.28>]
      [0.60 color rgb <0.39, 0.44, 0.78>]
      [1.00 color rgb <0.10, 0.40, 0.60>]
    }
    translate <-1200.00, 220.00, 0.00>
    scale <6000.00, 1700.00, 4000.00>
  }
}
texture {
  pigment {
    bozo
    turbulence 0.50
    color_map {
      [0.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.50 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.80 color rgb <1.00, 0.91, 0.91>]
      [1.00 color rgb <0.96, 1.00, 1.00>]
    }
    scale <1000.00, 200.00, 800.00>
  }
  finish {
    ambient 0.85
    diffuse 0.00
  }
}


#declare Overcast = texture {
  pigment {
    bozo
    turbulence 0.50
    lambda 6.00
    color_map {
      [0.00 color rgb <0.85, 0.85, 0.85>]
      [1.00 color rgb <1.00, 1.00, 1.00>]
    }
    scale <3000.00, 2000.00, 1000.00>
  }
  finish {
    ambient 1.00
    diffuse 0.00
  }
}
texture {
  pigment {
    bozo
    turbulence 0.60
    lambda 2.20
    color_map {
      [0.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.48 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [0.71 color rgbf <0.83, 0.83, 0.83, 0.40>]
      [1.00 color rgbf <0.85, 0.85, 0.85, 0.10>]
    }
    translate <150.00, 100.00, 0.00>
    scale <800.00, 750.00, 500.00>
  }
  finish {
    ambient 0.80
    diffuse 0.00
  }
}


// Looks like a city is on fire!
#declare Fire_Sky = texture {
  pigment {
    gradient y
    turbulence 0.20
    omega 0.80
    phase 0.85
    color_map {
      [0.00 color rgb <1.00, 1.00, 1.00>]
      [0.08 color rgbf <0.98, 1.00, 0.02, 0.08>]
      [0.29 color rgbf <1.00, 0.74, 0.01, 0.40>]
      [0.53 color rgbf <1.00, 0.16, 0.01, 0.67>]
      [0.79 color rgbf <1.00, 1.00, 1.00, 1.00>]
      [1.00 color rgbf <1.00, 1.00, 1.00, 1.00>]
    }
    scale <3.00, 7.50, 3.00>
  }
  finish {
    ambient 1.00
  }
  translate <0.00, -3.25, 0.00>
  scale <400.00, 600.00, 400.00>
}

#declare Stars = texture {
  pigment {
    granite
    color_map {
      [0.00 color rgb <0.00, 0.00, 0.00>]
      [0.84 color rgb <0.00, 0.00, 0.00>]
      [0.85 color rgb <0.80, 0.80, 0.80>]
      [0.86 color rgb <0.20, 0.20, 0.20>]
      [0.87 color rgb <0.95, 0.95, 0.95>]
      [0.88 color rgb <0.00, 0.00, 0.00>]
      [0.89 color rgb <1.00, 1.00, 1.00>]
      [0.90 color rgb <0.95, 0.95, 0.95>]
      [0.91 color rgb <0.00, 0.00, 0.00>]
      [1.00 color rgb <0.00, 0.00, 0.00>]
    }
  }
}

// Sandstone Polished

#declare SandstP = texture {
  pigment {
    granite
    turbulence 0.70
    color_map {
      [0.00 color rgb <0.86, 0.82, 0.67>]
      [0.50 color rgb <0.91, 0.87, 0.71>]
      [1.00 color rgb <0.86, 0.84, 0.59>]
    }
  }
  normal {
    bumps 0.10
    scale <0.20, 0.20, 0.20>
  }
  finish {
    ambient 0.50
    diffuse 1.00
    specular 0.50
    roughness 0.01
    metallic
    reflection 0.11
    crand 0.250
  }
}
texture {
  pigment {
    colour rgbf <1.00, 1.00, 1.00, 1.00>
  }
  finish {
    ambient 0.50
    brilliance 1.50
    phong 0.90
    phong_size 80
  }
}


//Bumpy Metallic Stone Texture

#declare BumpMetal = texture {
  pigment {
    spotted
    turbulence 0.40
    color_map {
      [0.00 color rgb<0.00, 0.00, 0.00>]
      [1.00 color rgb<1.00, 1.00, 1.00>]
    }
    scale <0.20,0.20,0.20>
  }
  normal {
    bumps 0.16
    scale <0.20,0.20,0.20>
  }
  finish {
    specular 0.72    
    metallic
    reflection 0.24
    crand 0.050
  }
}

// ----------------------------------------------------------------------------
// ------------ BACKGROUND TEXTURE --------------------------------------------
// ----------------------------------------------------------------------------

#declare BackGround01 = texture {
  pigment {
    gradient y
    color_map {
      [0.00 0.25 color rgb <1,1,1>
                 color rgb <0.2980,0.1921,0.8392>]
      [0.25 0.50 color rgb <0.2980,0.1921,0.8392>
                 color rgb <0.02352,0.07843,0.2156>]
      [0.50 0.95 color rgb <0.02352,0.07843,0.2156>
                 color rgb <1,1,1>]
      [0.95 1.00 color rgb <1,1,1>
                 color rgb <0.4,0.3,0.9>]
    }
  }
  finish {
    ambient 0.30
    phong 0.0
    diffuse 0.0
  }
}

//*****************************************************************************
//                           SKY PIGMENTS
//*****************************************************************************

// Red sky w/ yellow clouds -- very surreal.
#declare Blood_Sky = texture {
pigment {
    bozo
    turbulence 0.5
    color_map {
        [0.0, 0.5 color rgb <0.9, 0.700, 0.0>
                  color rgb <0.3, 0.200, 0.0>]
        [0.5, 0.6 color rgb <0.6, 0.025, 0.0>
                  color rgb <0.9, 0.700, 0.0>]
        [0.6, 1.0 color  rgb <0.6, 0.025, 0.0>
                  color rgb <0.6, 0.025, 0.0>]
    }
}}

// White clouds w/ transparent sky.
#declare Clouds = texture {
pigment {
    bozo
    turbulence 0
    color_map {
        [0.0, 0.1 color rgbf <0.8, 0.8, 0.8, 0.0>
                  color rgbf <0.8, 0.8, 0.8, 0.0>]
        [0.1, 0.5 color rgbf <0.8, 0.8, 0.8, 0.0>
                  color rgbf <1.0, 1.0, 1.0, 1.0>] 
        [0.5, 1.0 color rgbf <1.0, 1.0, 1.0, 1.0>
                  color rgbf <1.0, 1.0, 1.0, 1.0>]
    }
}}

#declare FBM_Clouds = texture {
pigment {
    bozo
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map { 
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85 
                    color red 0.75 green 0.75 blue 0.75] 
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75 
                    color rgbf <1,1,1,1>]
        [0.5, 1.001 color rgbf <1,1,1,1> color rgbf <1,1,1,1>]
    } 
scale <6, 1, 6>
}}

#declare Shadow_Clouds = texture {  // The blue sky background for the clouds
    pigment { color rgb <0.196078,0.6,0.8> }
    finish { ambient 0.7 diffuse 0 }
}
texture {                          // The upper part of the clouds
    pigment { 
    bozo
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map { 
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85 
                    color red 0.75 green 0.75 blue 0.75] 
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75 
                    color rgbf <1,1,1,1>]
        [0.5, 1.001 color rgbf <1,1,1,1> color rgbf <1,1,1,1>]
    } 
scale <6, 1, 6>
} 
    finish { ambient 1.0 diffuse 0 }
}
texture {                          // The darker underside of the clouds
    pigment { 
    bozo
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map { 
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85 
                    color red 0.75 green 0.75 blue 0.75] 
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75 
                    color rgbf <1,1,1,1>]
        [0.5, 1.001 color rgbf <1,1,1,1> color rgbf <1,1,1,1>]
    } 
scale <6, 1, 6>
  translate -0.15 }
    finish { ambient 0.6 diffuse 0 }
}

//*****************************************************************************
//                           WOODEN PIGMENTS
//*****************************************************************************

#declare DMFWood2 = texture {
pigment {
    wood
    turbulence 0.03
    octaves 4
    scale <0.05, .05, 1>
    color_map {
        [0.0, 0.1 color rgb <0.52, 0.37, 0.26>
                  color rgb <0.52, 0.37, 0.26>]
        [0.1, 0.9 color rgb <0.52, 0.37, 0.26>
                  color rgb <0.42, 0.26, 0.15>]
        [0.9, 1.0 color rgb <0.42, 0.26, 0.15>
                  color rgb <0.42, 0.26, 0.15>]
    }
}}

#declare DMFWood3 = texture {
pigment {
    wood
    turbulence 0.05
    octaves 2
    scale <0.05, .05, 1>
    color_map {
        [0.0, 0.1 color rgb <0.4, 0.133, 0.066>
                  color rgb <0.4, 0.133, 0.066>]
        [0.1, 0.9 color rgb <0.4, 0.133, 0.066>
                  color rgb <0.2, 0.065, 0.033>]
        [0.9, 1.0 color rgb <0.2, 0.065, 0.033>
                  color rgb <0.2, 0.065, 0.033>]
    }
}}

#declare DMFWood4 = texture {
pigment {
    wood
    turbulence 0.04
    octaves 3
    scale <0.05, .05, 1>
    color_map {
        [0.0, 0.1 color rgb <0.888, 0.600, 0.3>
                  color rgb <0.888, 0.600, 0.3>]
        [0.1, 0.9 color rgb <0.888, 0.600, 0.3>
                  color rgb <0.600, 0.400, 0.2>]
        [0.9, 1.0 color rgb <0.600, 0.400, 0.2>
                  color rgb <0.600, 0.400, 0.2>]
    }
}}

#declare DMFWood5 = texture {
pigment {
    wood
    turbulence 0.05
    octaves 6
    scale <0.075, .075, 1>
    color_map {
        [0.0, 0.1 color rgb <0.30, 0.10, 0.050>
                  color rgb <0.30, 0.10, 0.050>]
        [0.1, 0.9 color rgb <0.30, 0.10, 0.050>
                  color rgb <0.25, 0.07, 0.038>]
        [0.9, 1.0 color rgb <0.25, 0.07, 0.038>
                  color rgb <0.25, 0.07, 0.038>]
    }
}}

// This is a three-layer wood texture.  Renders rather slowly because of
// the transparent layers and the two layers of turbulence, but it looks
// great.  Try other colors of "varnish" for simple variations.  
#declare DMFWood6 = texture {
    pigment {
        wood  turbulence 0.04
        octaves 3
        scale <0.05, .05, 1>
        color_map { 
            [0.0, 0.1 color rgb <0.88, 0.60, 0.4>
                      color rgb <0.88, 0.60, 0.4>]
            [0.1, 0.9 color rgb <0.88, 0.60, 0.4>
                      color rgb <0.60, 0.40, 0.3>]
            [0.9, 1.0 color rgb <0.60, 0.40, 0.3>
                      color rgb <0.60, 0.40, 0.3>]
        }
    }
    finish { 
        specular 0.25
        roughness 0.05
        ambient 0.45 
        diffuse 0.33
        reflection 0.15
    }
}
texture {
    pigment {
        wood  turbulence <0.1, 0.5, 1> 
        octaves 5
        lambda 3.25
        scale <0.15, .5, 1>
        color_map { 
            [0.0, 0.1 color rgbf <0.7, 0.6, 0.4, 0.100> 
                      color rgbf <0.8, 0.6, 0.3, 0.500>]
            [0.1, 0.9 color rgbf <0.8, 0.6, 0.3, 0.650>
                      color rgbf <0.6, 0.4, 0.2, 0.975>]
            [0.9, 1.0 color rgbf <0.6, 0.4, 0.2, 0.975>
                      color rgbf <0.6, 0.4, 0.2, 1.000>]
        }
    rotate <5, 10, 5>
    translate -x*2
    }
    finish { 
        specular 0.25 
        roughness 0.0005
        ambient .1 
        diffuse 0.33
    } 
}
// A "coat of varnish" to modify the overall color of the wood
texture {
    pigment { rgbf <0.75, 0.15, 0.0, 0.95> }
    finish { 
        specular 0.25
        roughness 0.01
        ambient 0
        diffuse 0.33
    }
}

// Is this really oak?  I dunno.  Quite light, maybe more like spruce.
#declare DMFLightOak = texture {
pigment {
    wood
    turbulence 0.05            // For best results,  keep this low!
    scale <0.2, 0.2, 1>        // Scaled for a unit object
    color_map {
        [0.0, 0.1 color rgb <0.42, 0.26, 0.15>
                  color rgb <0.42, 0.26, 0.15>]
        [0.1, 0.9 color rgb <0.42, 0.26, 0.15>
                  color rgb <0.52, 0.37, 0.26>]
        [0.9, 1.0 color rgb <0.52, 0.37, 0.26>
                  color rgb <0.52, 0.37, 0.26>]
    }
}}

// Looks like old desk oak if used correctly.
#declare DMFDarkOak = texture {
pigment {
    wood
    turbulence 0.04            // For best results,  keep this low!
    octaves 3
    scale <0.2, 0.2, 1>        // Scaled for a unit object
    color_map {
        [0.0, 0.1 color rgb <0.60, 0.30, 0.18>
                  color rgb <0.60, 0.30, 0.18>]
        [0.1, 0.9 color rgb <0.60, 0.30, 0.18>
                  color rgb <0.30, 0.15, 0.09>]
        [0.9, 1.0 color rgb <0.30, 0.15, 0.09>
                  color rgb <0.30, 0.15, 0.09>]
    }
}}

// Wood by Eric Barish
#declare EMBWood1 = texture {  /* Bottom wood-grain layer */
    pigment {
        wood
        turbulence 0.05
        colour_map {
            [0.00 0.34  color rgb <0.58, 0.45, 0.23>
                        color rgb <0.65, 0.45, 0.25>]
            [0.34 0.40  color rgb <0.65, 0.45, 0.25>
                        color rgb <0.33, 0.23, 0.13>]
            [0.40 0.47  color rgb <0.33, 0.23, 0.13>
                        color rgb <0.60, 0.40, 0.20>]
            [0.47 1.00  color rgb <0.60, 0.40, 0.20>
                        color rgb <0.25, 0.15, 0.05>]
        }
    }
    finish {
        crand 0.02
        ambient 0.32
        diffuse 0.63
        phong 0.2
        phong_size 10
    }
    normal { bumps 0.05 }
}
texture {     /* top layer, adds small dark spots */
    pigment {
        bozo
        color_map {
            [0.0 0.8 color rgbf <1.00, 1.00, 1.00, 1.00>
                     color rgbf <1.00, 0.90, 0.80, 0.80>]
            [0.8 1.0 color rgbf <1.00, 0.90, 0.80, 0.80>
                     color rgbf <0.30, 0.20, 0.10, 0.40>]
        }
    scale 0.25
    }
}

//*****************************************************************************
//                           GLASS TEXTURES
//*****************************************************************************

#declare Glass = texture {
   pigment { color rgbf<1.0, 1.0, 1.0, 0.7> }
   finish  {
     specular 1
     roughness 0.001
     ambient 0
     diffuse 0
     reflection 0.1
     refraction 1
     ior 1.5
   }
}

// Probably more of a "Plexiglas" than glass
#declare Glass2 = texture {
    pigment { color rgbf <1,1,1,1> }
    finish {
        ambient 0
        diffuse 0
        reflection 0.5
        refraction 0.85
        ior 1.5
        phong 0.3
        phong_size 60
    }
}

// An excellent lead crystal glass!
#declare Glass3 = texture {
    pigment { color rgbf <0.98, 0.98, 0.98, 0.9> }
    finish  {
        ambient 0.1
        diffuse 0.1
        reflection 0.1
        refraction 0.9
        ior 1.45
        specular 0.8
        roughness 0.0003
        phong 1 
        phong_size 400
     }
}

// Glass textures contributed by Norm Bowler, of Richland WA 
#declare NBglass = texture {
    pigment { color rgbf <0.98, 1.0, 0.99, 0.75> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

#declare NBoldglass = texture {
    pigment { color rgbf <0.8, 0.9, 0.85, 0.85> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

#declare NBwinebottle = texture {
    pigment { color rgbf <0.4, 0.72, 0.4, 0.6> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

#declare NBbeerbottle = texture {
    pigment { color rgbf <0.7, 0.5, 0.1, 0.6> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

// A few color variations on Norm's glass
// Ruby glass
#declare Ruby_Glass = texture {
    pigment { color rgbf <0.9, 0.1, 0.2, 0.8> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

// Dark, glass
#declare Dark_Green_Glass = texture {
    pigment { color rgbf <0.1, 0.7, 0.8, 0.8> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

// Yellow glass
#declare Yellow_Glass = texture {
  pigment { color rgbf <0.8, 0.8, 0.2, 0.8> }
  finish {
    ambient 0.1
    diffuse 0.1
    reflection .25
    refraction 1
    ior 1.5
    specular 1
    roughness .001
  }
}

// Orange/Amber glass
#declare Orange_Glass = texture {
    pigment { rgbf <1.0, 0.5, 0.0, 0.8> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

// Vicks bottle, glass
#declare Vicks_Bottle_Glass = texture {
    pigment { color rgbf <0.1, 0.15, 0.5, 0.9> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        refraction 1
        ior 1.5
        specular 1
        roughness .001
    }
}

//*****************************************************************************
//                           METAL TEXTURES
//*****************************************************************************

#declare Chrome_Texture = texture {
    pigment { color  rgb <0.658824,0.658824,0.658824> }
    finish {
        ambient 0.3
        diffuse 0.7
        reflection 0.15
        brilliance 8
        specular 0.8
        roughness 0.1
    }
}

#declare New_Brass = texture {
    pigment { color rgb <0.70, 0.56, 0.37> }
    finish {
        ambient 0.35
        diffuse 1.0
        brilliance 15
        phong 0.41
        phong_size 5
    }
}


// Spun Brass texture for cymbals & such
#declare Spun_Brass = texture {
    pigment { color rgb <0.70, 0.56, 0.37> }
    finish {
        ambient 0.35
        diffuse 1.0
        brilliance 15
        phong 0.41
        phong_size 5
    }
    normal { waves 0.35 frequency 2 scale 0.01 }
}

// Brushed aluminum (brushed along X axis)
#declare Brushed_Aluminum = texture {
  pigment { rgb <0, 0, 0> }
  finish {
    ambient 0.3
    diffuse 0.7
    reflection 0.9
    phong 1.0
  }
  normal {
    bumps -0.5
    scale <1, 0.001, 0.001>
  }
}

// Interesting texture -- Give it a try.
// Sort of a "rgb <0.0, 0.0, 0.0> Hills Gold", black, white, and orange specks or splotches.

#declare Brass_Valley = texture {
    pigment {
        granite
        color_map {
            [0.0, 0.3 color rgb <0.82, 0.57, 0.46>  
                      color rgb <0.82, 0.57, 0.46>]
            [0.3, 0.6 color rgb <0.0, 0.0, 0.0>
                      color rgb <0.85, 0.85, 0.95>]
            [0.6, 1.0 color rgb <0.82, 0.57, 0.46>  
                      color rgb <0.85, 0.85, 0.95>]
        }
    }
    finish {
        metallic
        brilliance 6.0
        reflection 0.75
        phong 0.75
    }
}

#declare Rust = texture {
    pigment {
        granite
        color_map {
            [0.0, 0.4 color rgb <0.89, 0.51, 0.28>
                      color rgb <0.70, 0.13, 0.00>]
            [0.4, 0.5 color rgb <0.70, 0.13, 0.00>
                      color rgb <0.69, 0.41, 0.08>]
            [0.5, 0.6 color rgb <0.69, 0.41, 0.08>
                      color rgb <0.49, 0.31, 0.28>]
            [0.6, 1.0 color rgb <0.49, 0.31, 0.28>
                      color rgb <0.89, 0.51, 0.28>]
        }
    }
    finish { ambient 0.2 diffuse 0.4 }
}

#declare Rusty_Iron = texture {
    pigment {
        granite
        color_map {
            [0.0, 0.5 color rgb <0.21, 0.10, 0.10> * 2
                      color rgb <0.25, 0.25, 0.01> * 2]
            [0.5, 0.6 color rgb <0.25, 0.25, 0.01> * 2
                      color rgb <0.30, 0.10, 0.10> * 2]
            [0.6, 1.0 color rgb <0.15, 0.10, 0.10> * 2
                      color rgb <0.15, 0.10, 0.10> * 2]
        }
    }
    finish { ambient 0.2 diffuse 0.6 }
    normal { wrinkles 1 scale 0.1 }
}

#declare Soft_Silver = texture {
    pigment { color rgb <0.94, 0.93, 0.83> }
    finish {
        metallic
        ambient 0.2
        diffuse 0.35
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

#declare New_Penny = texture {
    pigment { color rgb <0.6, 0.45, 0.4> }
    finish {
        metallic
        ambient 0.1
        diffuse 0.65
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

#declare Tinny_Brass = texture {
    pigment { color rgb <0.70, 0.56, 0.37> }
    finish {
        metallic
        ambient 0.1
        diffuse 0.65
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

#declare Gold_Nugget = texture {
    pigment { color rgb <0.5, 0.35, 0.25> }
    finish {
        metallic
        ambient 0.1
        diffuse 0.65
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

#declare Aluminum  = texture {
    pigment { color rgb <0.55, 0.5, 0.45> }
    finish {
        metallic
        ambient 0.1
        diffuse 0.65
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

#declare Bright_Bronze = texture {
    pigment { color rgb <0.36, 0.28, 0.20> }
    finish {
        metallic
        ambient 0.1
        diffuse 0.65
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

//*****************************************************************************
//                    SPECIAL EFFECTS
//*****************************************************************************

#declare Y_Gradient = 
pigment {
    gradient <0,1,0>
    color_map {
        [0.00, 0.33 color rgb <1,0,0>   color rgb <0,0,1>]
        [0.33, 0.66 color rgb <0,0,1>  color rgb <0,1,0>]
        [0.66, 1.00 color rgb <0,1,0> color rgb <1,0,0>]
    }
}

#declare X_Gradient = 
pigment {
    gradient <1,0,0>
    color_map {
        [0.00, 0.33 color rgb <1,0,0>   color rgb <0,0,1>]
        [0.33, 0.66 color rgb <0,0,1>  color rgb <1,1,1>]
        [0.66, 1.00 color rgb <1,1,1> color rgb <1,1,1>]
    }
}

// Wavy water 
// Requires a sub-plane, and may require scaling to fit your scene.
#declare Water2 =
texture {
    pigment{ color rgbf <0.0, 0.0, 1.0, 0.9> }
    normal {
        ripples 0.75
        frequency 10
    }
    finish {
        reflection 0.3
        refraction 0.5
        ior 1.33
    }
}

#declare Lightening1 = texture {
    pigment {
      marble
      color_map {
          [0.00 0.15 color rgb <1,1,1>
                     color red 0.94 green 0.81 blue 0.99 filter 0.65]
          [0.15 0.25 color red 0.94 green 0.81 blue 0.99 filter 0.65
                     color red 0.94 green 0.81 blue 0.99 filter 0.65]
          [0.25 0.30 color red 0.94 green 0.81 blue 0.99 filter 0.65
                     color red 0.87 green 0.58 blue 0.98 filter 0.85]
          [0.30 0.40 color red 0.87 green 0.58 blue 0.98 filter 0.85
                     color red 0.87 green 0.58 blue 0.98 filter 0.85]
          [0.40 0.45 color red 0.87 green 0.58 blue 0.98 filter 0.85
                     color red 0.73 green 0.16 blue 0.96 filter 0.95]
          [0.50 1.00 color red 0.73 green 0.16 blue 0.96 filter 0.95
                     color red 0.73 green 0.16 blue 0.96 filter 0.95]
       }
       turbulence 0.5
    }
    finish { ambient 1 }
}

#declare Lightening2 = texture {
    pigment {
        granite
        color_map {
          [0.00 0.10 color rgb <1,1,1>
               color red 0.94 green 0.81 blue 0.99 filter 0.65]
          [0.10 0.20 color red 0.94 green 0.81 blue 0.99 filter 0.65
               color red 0.94 green 0.81 blue 0.99 filter 0.65]
          [0.20 0.30 color red 0.94 green 0.81 blue 0.99 filter 0.65
               color red 0.87 green 0.58 blue 0.98 filter 0.85]
          [0.30 0.45 color red 0.87 green 0.58 blue 0.98 filter 0.85
               color red 0.87 green 0.58 blue 0.98 filter 0.85]
          [0.45 0.65 color red 0.87 green 0.58 blue 0.98 filter 0.85
               color red 0.73 green 0.16 blue 0.96 filter 0.95]
          [0.65 1.00 color red 0.73 green 0.16 blue 0.96 filter 0.95
               color red 0.73 green 0.16 blue 0.96 filter 0.95]
        }
        turbulence 0.5
    }
    finish { ambient 1 }
}

// field, by Jim Burton
#declare Starfield = texture {
    pigment {
        granite  
        color_map {
            [ 0.00  0.72 color rgb <0,0,0>    color rgb <0,0,0> ]  // No Stars in this area
            [ 0.72  0.75 color rgb <.2,.2,.2> color rgb <.4,.4,.4> ] // Very Very Faint Stars
            [ 0.75  0.78 color rgb <.4,.4,.4> color rgb <.6,.6,.6> ] // Very Faint Stars                [ 0.78  0.81 color Gray60 color Gray80 ] // Faint Stars
            [ 0.81  0.85 color rgb <.8,.8,.8> color rgb <.95,.95,.95> ] // Medium White Stars
            [ 0.85  0.91 color rgb <.95,.95,.95> color rgb <1,1,1>  ] // White Stars
            [ 0.91  1.00 color rgb <0,0,0>  color rgb <0,0,0> ]  // No Stars in this area
        }
    scale .015
    }
    finish { ambient 1 }
}

// Glass Textures

// Simple clear glass
#declare T_Glass1 = texture {
    pigment { color rgbf<1.0, 1.0, 1.0, 0.7> }
    finish  {
      specular 1
      roughness 0.001
      ambient 0
      diffuse 0
      refraction 1
      reflection 0.1
      ior 1.5
    }
}

// More like an acrylic plastic
#declare T_Glass2 = texture {
    pigment { color rgbf<1.0, 1.0, 1.0, 1.0> }
    finish  {
      ambient 0
      diffuse 0
      reflection 0.5
      refraction 1
      ior 1.5
      phong 0.3
      phong_size 60
    }
}


// An excellent lead crystal glass!
#declare T_Glass3 = texture {
    pigment { color rgbf <0.98, 0.98, 0.98, 0.9> }
    finish {
      ambient 0.1
      diffuse 0.1
      reflection 0.1
      refraction 1
      ior 1.45
      specular 0.8
      roughness 0.003
      phong 1
      phong_size 400
    }
}

#declare T_Glass4 = texture {
    pigment { color rgbf <0.98, 1.0, 0.99, 0.75> }
    finish {
      ambient 0.1
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
}


#declare T_Old_Glass = texture {
    finish {
      ambient 0.1
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.8, 0.9, 0.85, 0.85> }
}

#declare T_Winebottle_Glass = texture {
    finish {
      ambient 0.1
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.4, 0.72, 0.4, 0.6> }
}

#declare T_Beerbottle_Glass = texture {
    finish {
      ambient 0.1
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.7, 0.5, 0.1, 0.6> }
}

// A few color variations on Norm's glass
// Ruby glass
#declare T_Ruby_Glass = texture {
    finish {
      ambient 0.1
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.9, 0.1, 0.2, 0.8> }
}

#declare T_Green_Glass = texture {
    pigment { color rgbf <0.8, 1, 0.95, 0.9> }
    finish {
      ambient 0.1
      diffuse 0.1
      reflection 0.1
      refraction 1
      ior 1.45
      specular 0.8
      roughness 0.003
      phong 1
      phong_size 400
    }
}

#declare T_Dark_Green_Glass = texture {
    finish {
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.1, 0.7, 0.8, 0.8> }
}

#declare T_Yellow_Glass = texture {
    finish {
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.8, 0.8, 0.2, 0.8> }
}

// Orange/Amber glass
#declare T_Orange_Glass = texture {
    finish {
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { rgbf <1.0, 0.5, 0.0, 0.8> }
}

// Vicks bottle, glass
#declare T_Vicksbottle_Glass = texture {
    finish {
      diffuse 0.1
      reflection .25
      refraction 1
      ior 1.5
      specular 1
      roughness 0.001
    }
    pigment { color rgbf <0.1, 0.15, 0.5, 0.9> }
}

// ***********************************
//  Bill Pulver's FBM Clouds
// A three-layer cloud texture.
// ***********************************
#declare T_Cloud1 = texture {                              // The upper part of the clouds
    pigment {
      bozo
      turbulence 0.65
      octaves 6
      omega 0.7
      lambda 2
      color_map {
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85
                    color red 0.75 green 0.75 blue 0.75]
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75
                    color rgbf <1,1,1,1>]
        [0.5, 1.001 color rgbf <1,1,1,1>
                    color rgbf <1,1,1,1>]
      }
      scale <6, 1, 6>
    }
    finish {
        ambient 1.0
        diffuse 0
    }
}
texture {                              // The darker underside of the clouds
    pigment {
      bozo
      turbulence 0.65
      octaves 6
      omega 0.7
      lambda 2
      color_map {
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85
                    color red 0.75 green 0.75 blue 0.75]
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75
                    color rgbf <1,1,1,1>]
        [0.5, 1.001 color rgbf <1,1,1,1>
                    color rgbf <1,1,1,1>]
      }
      scale <6, 1, 6>
      translate -0.15*y
    }
    finish {
        ambient 0.6
        diffuse 0
    }
}

// ***********************************
//  Darin Dugger's "Kite" clouds
// ***********************************
#declare T_Cloud2 = texture {
    pigment {
        bozo
        turbulence 1.5
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.5 color rgbf<.75, .75, .75, 0>
                      color rgbf<.9, .9, .9, .9> ]
            [0.5, 0.7 color rgbf<.9, .9, .9, .9>
                      color rgbf<1, 1, 1, 1> ]
            [0.7, 1.0 color rgbf<1, 1, 1, 1>
                      color rgbf<1, 1, 1, 1> ]
        }
    }
    finish {
        ambient 0.9
        diffuse 0.1
    }
}

#declare T_Cloud3 = texture {
    pigment {
        bozo
        turbulence 0.8 //0.6
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.4 color rgbf<.75, .75, .75, 0>
                      color rgbf<.9, .9, .9, .9> ]
            [0.4, 0.7 color rgbf<.9, .9, .9, .9>
                      color rgbf<1, 1, 1, 1> ]
            [0.7, 1.0 color rgbf<1, 1, 1, 1>
                      color rgbf<1, 1, 1, 1> ]
        }
    }
    finish {
        ambient 1.0
        diffuse 0.0
    }
}
texture {
    pigment {
        bozo
        turbulence 0.8 //0.6
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.4 color rgbf<.75, .75, .75, 0>
                      color rgbf<.9, .9, .9, .9> ]
            [0.4, 0.6 color rgbf<.9, .9, .9, .9>
                      color rgbf<1, 1, 1, 1> ]
            [0.6, 1.0 color rgbf<1, 1, 1, 1>
                      color rgbf<1, 1, 1, 1> ]
        }
    }
    finish {
        ambient 0.95
        diffuse 0.0
    }
  scale .9
  translate y*-0.15
}

#declare Stone25 = texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map {
            [0.02 color rgbf <0.30, 0.60, 0.45, 0.0> ]
            [0.06 color rgbf <0.35, 0.55, 0.40, 0.0> ]
            [0.10 color rgbf <0.25, 0.25, 0.20, 0.0> ]
            [1.00 color rgbf <0.75, 0.75, 0.60, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map { 
            [0.02 color rgbf <0.30, 0.60, 0.45, 0.95> ]
            [0.06 color rgbf <0.35, 0.55, 0.40, 0.75> ]
            [0.10 color rgbf <0.25, 0.25, 0.20, 0.65> ]
            [1.00 color rgbf <0.75, 0.75, 0.60, 0.60> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.15
        rotate <30, 30, 30>
        color_map {
            [0.00 color rgbf <0.95, 0.95, 0.95, 0.30> ]
            [0.03 color rgbf <0.85, 0.85, 0.85, 0.60> ]
            [0.06 color rgbf <0.50, 0.50, 0.50, 0.90> ]
            [0.10 color rgbf <1.00, 1.00, 1.00, 1.00> ]
        }
    }
}

#declare Stone26 = texture {
    pigment {
        granite
        rotate y*90
        color_map { 
            [0.02 color rgbf <0.25, 0.45, 0.60, 0.0> ]
            [0.06 color rgbf <0.20, 0.40, 0.55, 0.0> ]
            [0.10 color rgbf <0.15, 0.20, 0.25, 0.0> ]
            [1.00 color rgbf <0.40, 0.60, 0.75, 0.0> ]
        }
        frequency 6
        rotate <0, 10, 0>
        turbulence <0.05, 0.05, 0.05>
        octaves 6
        omega 0.7
        lambda 2
    }
}
texture {
    pigment {
        gradient x
        color_map {
            [0.00 color rgbf <1,1,1,1> ]
            [0.10 color rgbf <1.00, 1.00, 1.00, 0.75> ]
            [0.15 color rgbf <0.15, 0.25, 0.40, 0.50> ]
            [0.20 color rgbf <0.00, 0.05, 0.10, 0.25> ]
            [0.25 color rgbf <0.15, 0.25, 0.40, 0.25> ]
            [0.30 color rgbf <0.35, 0.55, 0.40, 0.50> ]
            [0.35 color rgbf <0.25, 0.25, 0.20, 0.75> ]
            [0.35 color rgbf <1,1,1,1> ]
        }
        frequency 3
        rotate <1, 10, 45>
        rotate x*60
        turbulence <0.30, 0.30, 0.30>
        octaves 6
        omega 0.7
        lambda 2.2
    }
}
texture {
    pigment {
        agate
        color_map {
            [0.00 color rgbf <1,1,1,1> ]
            [0.10 color rgbf <1.00, 1.00, 1.00, 0.75> ]
            [0.15 color rgbf <0.35, 0.55, 0.40, 0.50> ]
            [0.20 color rgbf <0.25, 0.25, 0.20, 0.25> ]
            [0.25 color rgbf <0.30, 0.60, 0.45, 0.25> ]
            [0.30 color rgbf <0.35, 0.55, 0.40, 0.50> ]
            [0.35 color rgbf <0.25, 0.25, 0.20, 0.75> ]
            [0.35 color rgbf <1,1,1,1> ]
        }
        frequency 3
        rotate <2, 10, 75>
        rotate x*120
        turbulence <0.05, 0.05, 0.05>
        octaves 6
        omega 0.7
        lambda 2
    }
}

#declare Stone27 = texture {
    pigment {
        crackle
        turbulence 0.35
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.5
        frequency 3
        rotate <0, -45, 30>
        color_map { 
            [0.02 color rgbf <0.25, 0.45, 0.60, 0.0> ]
            [0.06 color rgbf <0.20, 0.40, 0.55, 0.0> ]
            [0.10 color rgbf <0.05, 0.10, 0.15, 0.0> ]
            [1.00 color rgbf <0.40, 0.60, 0.75, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.20 color rgbf <1.00, 1.00, 1.00, 0.75> ]
            [0.25 color rgbf <0.25, 0.45, 0.40, 0.50> ]
            [0.30 color rgbf <0.00, 0.00, 0.00, 0.00> ]
            [0.35 color rgbf <0.15, 0.75, 0.60, 0.50> ]
            [0.40 color rgbf <0.35, 0.85, 0.40, 0.60> ]
            [0.45 color rgbf <0.35, 0.45, 0.20, 0.75> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.35
        lambda 2.5
        omega 0.75
        octaves 5
        scale 2
        frequency 2
        rotate <0, 90, 0>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.20 color rgbf <1.00, 1.00, 1.00, 0.75> ]
            [0.25 color rgbf <0.35, 0.55, 0.40, 0.50> ]
            [0.30 color rgbf <0.00, 0.00, 0.00, 0.00> ]
            [0.45 color rgbf <0.30, 0.60, 0.45, 0.25> ]
            [0.50 color rgbf <0.35, 0.55, 0.40, 0.50> ]
            [0.65 color rgbf <0.25, 0.25, 0.20, 0.75> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}

#declare Stone28 = texture {
    pigment {
        agate
        agate_turb 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 5>
        color_map { 
            [0.02 color rgbf <0.65, 0.45, 0.25, 0.0> ]
            [0.06 color rgbf <0.55, 0.40, 0.20, 0.0> ]
            [0.10 color rgbf <0.15, 0.10, 0.05, 0.0> ]
            [1.00 color rgbf <0.75, 0.60, 0.40, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.75
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.25 color rgbf <0.40, 0.25, 0.15, 0.50> ]
            [0.30 color rgbf <0.00, 0.00, 0.00, 0.00> ]
            [0.35 color rgbf <0.40, 0.25, 0.15, 0.50> ]
            [0.40 color rgbf <0.45, 0.55, 0.35, 0.60> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.25 color rgbf <0.65, 0.25, 0.40, 0.50> ]
            [0.30 color rgbf <0.00, 0.00, 0.00, 0.00> ]
            [0.45 color rgbf <0.50, 0.60, 0.35, 0.25> ]
            [0.50 color rgbf <0.65, 0.25, 0.40, 0.50> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}


#declare Stone29 = texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map {
            [0.02 color rgb <0.65, 0.35, 0.25> ]
            [0.06 color rgb <0.55, 0.25, 0.10> ]
            [0.10 color rgb <0.45, 0.30, 0.25> ]
            [1.00 color rgb <0.65, 0.35, 0.25> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.6
        lambda 1.75
        omega 0.55
        octaves 4
        scale 0.45
        rotate <0, 5, 60>
        color_map { 
            [0.01 color rgbf <0.00, 0.00, 0.00, 0.00> ]
            [0.02 color rgbf <0.40, 0.25, 0.15, 0.20> ]
            [0.05 color rgbf <0.40, 0.35, 0.20, 0.40> ]
            [0.10 color rgbf <0.45, 0.55, 0.25, 0.60> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}
texture {
    pigment {
        granite
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.75
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map {
            [0.15 color rgbf <0.40, 0.25, 0.15, 0.40> ]
            [0.20 color rgbf <0.20, 0.10, 0.00, 0.00> ]
            [0.25 color rgbf <0.20, 0.15, 0.15, 0.40> ]
            [0.30 color rgbf <0.40, 0.25, 0.15, 0.70> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}

#declare Stone30 = texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map { 
            [0.02 color rgbf <0.85, 0.85, 0.45, 0.0> ]
            [0.06 color rgbf <0.65, 0.65, 0.40, 0.0> ]
            [0.10 color rgbf <0.90, 0.90, 0.65, 0.0> ]
            [1.00 color rgbf <0.85, 0.85, 0.45, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.25 color rgbf <0.40, 0.40, 0.15, 0.50> ]
            [0.30 color rgbf <0.00, 0.00, 0.00, 0.00> ]
            [0.35 color rgbf <0.40, 0.40, 0.20, 0.50> ]
            [0.40 color rgbf <0.65, 0.65, 0.30, 0.60> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.75
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.15 color rgbf <0.40, 0.15, 0.45, 0.60> ]
            [0.18 color rgbf <0.20, 0.10, 0.00, 0.00> ]
            [0.25 color rgbf <0.20, 0.15, 0.45, 0.60> ]
            [0.30 color rgbf <0.40, 0.15, 0.45, 0.80> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}

#declare Stone31 = texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map { 
            [0.00 color rgb <.9,.9,.9> ]
            [0.02 color rgb <0.95, 0.90, 0.85> ]
            [0.03 color rgb <0.75, 0.85, 0.80> ]
            [0.04 color rgb <0.45, 0.40, 0.50> ]
            [0.10 color rgb <.15,.15,.15> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map { 
            [0.00 color rgbf <1,1,1,1> ]
            [0.02 color rgb <0.95, 0.90, 0.85> ]
            [0.03 color rgb <0.75, 0.85, 0.80> ]
            [0.04 color rgb <0.45, 0.40, 0.50> ]
            [0.10 color rgbf <1,1,1,1> ]
        }
    }
}

#declare Stone32 = texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map { 
            [0, 1 color rgb <0.9, 0.75, 0.75>
                  color rgb <0.6, 0.6, 0.6> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map { 
            [0.0, 0.9 color rgbf <0.52, 0.39, 0.39, 1.0> 
                      color rgbf <0.52, 0.39, 0.39, 0.5>]
            [0.9, 1.0 color rgbf <0.42, 0.14, 0.55, 0.0>
                      color rgbf <0.42, 0.14, 0.55, 0.0>]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.15
        rotate <30, 30, 30>
        color_map {
            [0.00 color rgbf <0.95, 0.95, 0.95, 0.30> ]
            [0.03 color rgbf <0.85, 0.85, 0.85, 0.60> ]
            [0.06 color rgbf <0.50, 0.50, 0.50, 0.90> ]
            [0.10 color rgbf <1.00, 1.00, 1.00, 1.00> ]
        }
    }
}

#declare Stone33 = texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map {
            [0.0, 0.1 color rgb <0.0, 0.0, 0.0>
                      color rgb <0.9, 0.7, 0.6>]
            [0.1, 0.3 color rgb <0.9, 0.7, 0.6>
                      color rgb <0.9, 0.7, 0.4>]
            [0.3, 1.0 color rgb <0.9, 0.7, 0.4>
                      color rgb <0.7, 0.4, 0.2>]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map {
            [0.0, 0.2 color rgbf <0.7, 0.4, 0.2, 0.20>
                      color rgbf <0.9, 0.7, 0.6, 0.30>]
            [0.2, 0.3 color rgbf <0.9, 0.7, 0.6, 0.50>
                      color rgbf <0.9, 0.7, 0.4, 0.70>]
            [0.3, 1.0 color rgbf <0.9, 0.7, 0.4, 0.80>
                      color rgbf <0.7, 0.4, 0.2, 0.80>]
        }
    }
}

#declare Stone34 = texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map {
            [0.02 color rgbf <0.85, 0.85, 0.85, 0.0> ]
            [0.10 color rgbf <1.00, 1.00, 1.00, 0.0> ]
            [1.00 color rgbf <0.85, 0.85, 0.85, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map {
            [0.00 color rgbf <1,1,1,1> ]
            [0.15 color rgbf <0.40, 0.30, 0.30, 0.50> ]
            [0.30 color rgbf <0.15, 0.08, 0.02, 0.00> ]
            [0.35 color rgbf <0.60, 0.40, 0.35, 0.50> ]
            [0.45 color rgbf <0.40, 0.35, 0.30, 0.60> ]
            [0.65 color rgbf <1,1,1,1> ]
        }
    }
}

// Creamy coffee w/greenish-grey veins & faint avacado swirls
#declare Stone36 = texture  {
    pigment {
        granite
        turbulence 0.815
        colour_map {
            [0.10 colour rgbf <0.43529, 0.49804, 0.32941,0.0>]
            [0.20 colour rgbf <0.19608, 0.27843, 0.19608,0.0>]
            [0.35 colour rgbf <0.43529, 0.49804, 0.32941,0.0>]
            [0.55 colour rgbf <0.45490, 0.44706, 0.20784,0.0>]
            [0.75 colour rgbf <0.43529, 0.49804, 0.32941,0.0>]
            [0.90 colour rgbf <0.19608, 0.22745, 0.00000,0.0>]
            [1.00 colour rgbf <0.43529, 0.49804, 0.32941,0.0>]
        }
    scale <0.85, 1.5, 0.5>
    rotate <10, 0, 40>
    }
    finish { brilliance 1.825 }
}
texture  {
    pigment {
        granite
        turbulence 0.825
        colour_map {
            [0.18 colour rgbf <0.20784, 0.13333, 0.00000, 0.427>]
            [0.35 colour rgbf <0.20784, 0.13333, 0.00000, 0.735>]
            [0.45 colour rgbf <0.27059, 0.11373, 0.00000, 0.625>]
            [0.90 colour rgbf <0.27059, 0.11373, 0.00000, 0.875>]
            [1.00 colour rgbf <0.20784, 0.13333, 0.00000, 0.890>]
        }
     scale <0.65, 1.18, 0.34>
     rotate <5, 10, -55>
    }
    finish { ambient 0.275 diffuse 0.775 crand 0.01975 }
}

// Olive greens w/lighter swirls & hints of salmon
#declare Stone37 = texture  {
    pigment {
        granite
        turbulence 0.9
        colour_map  {
            [0.18 colour rgbf <0.29020, 0.24706, 0.00000,0>]
            [0.35 colour rgbf <0.29020, 0.24706, 0.00000,0>]
            [0.45 colour rgbf <0.16471, 0.15294, 0.00000,0>]
            [0.90 colour rgbf <0.16471, 0.15294, 0.00000,0>]
            [1.00 colour rgbf <0.32941, 0.25882, 0.00000,0>]
        }
        scale <0.825, 1.7, 0.25>
        rotate <10,5,40>
    }
    finish { brilliance 2.5 }
}
texture  {
    pigment {
        granite
        turbulence 0.85
        colour_map   {
            [0.05 colour rgbf <0.86275, 0.65490, 0.40392, 0.6250>]
            [0.35 colour rgbf <0.48627, 0.16471, 0.00000, 0.8250>]
            [0.60 colour rgbf <0.86275, 0.65490, 0.40392, 0.7150>]
            [0.85 colour rgbf <0.48627, 0.16471, 0.00000, 0.5450>]
            [1.00 colour rgbf <0.52941, 0.34118, 0.17647, 0.8975>]
        }
        scale <0.85, 1.6, 0.45>
        rotate <0,5,-50>
    }
    finish { ambient 0.275 diffuse 0.6925 crand 0.01975}
}

// Deep rich coffee w/darker veins & lots of creamy swirl
#declare Stone38 = texture  {
    pigment {
        granite
        turbulence 1.25
        colour_map  {
            [0.15 colour rgbf<0.72549, 0.73725, 0.54118, 0.0>]
            [0.25 colour rgbf<0.22745, 0.14510, 0.00000, 0.0>]
            [0.45 colour rgbf<0.72549, 0.73725, 0.54118, 0.0>]
            [0.60 colour rgbf<0.22745, 0.14510, 0.00000, 0.0>]
            [0.80 colour rgbf<0.22745, 0.14510, 0.00000, 0.0>]
            [1.00 colour rgbf<0.60392, 0.61176, 0.46667, 0.0>]
        }
        scale <0.95, 1.7, 0.39>
        rotate <10, 5, 40>
    }
    finish { brilliance 2.125 }
}
texture  {
    pigment {
        granite
        turbulence 0.825
        colour_map {
            [0.35 colour rgbf <0.56078, 0.54902, 0.42353,0.65>]
            [0.65 colour rgbf <0.10196, 0.05882, 0.00000,0.85>]
            [1.00 colour rgbf <0.56078, 0.54902, 0.42353,0.90>]
        }
        scale <0.76, 1.4, 0.4>
        rotate <0, 10, -55>
    }
    finish { ambient 0.4 diffuse 0.575 crand 0.021 }
}

// Light mauve w/large plum swirls
#declare Stone39 =  texture  {
    pigment {
        granite
        turbulence 0.825
        colour_map  {
             [0.12 colour rgbf <0.78039, 0.54902, 0.46667, 0.0>]
             [0.35 colour rgbf <0.78039, 0.54902, 0.46667, 0.0>]
             [0.65 colour rgbf <0.49804, 0.30980, 0.30196, 0.0>]
             [0.90 colour rgbf <0.49804, 0.30980, 0.30196, 0.0>]
             [1.00 colour rgbf <0.35294, 0.09020, 0.00000, 0.0>]
        }
    }
    scale <0.78,1.45, 0.4>
    rotate <5,10,-55>
    finish { brilliance 1.9275 ambient .3 diffuse .575 crand .0215}
}

// Creamy aqua w/green hi-lites & subtle hints of grey
#declare Stone40 = texture  {
    pigment {
        agate
        agate_turb 1.1
        colour_map  {
            [0.10 colour rgbf <0.56078, 0.67451, 0.62353, 0.0>]
            [0.35 colour rgbf <0.27843, 0.41569, 0.30196, 0.0>]
            [0.55 colour rgbf <0.56078, 0.67451, 0.62353, 0.0>]
            [0.85 colour rgbf <0.56078, 0.67451, 0.62353, 0.0>]
            [0.95 colour rgbf <0.27843, 0.41569, 0.30196, 0.0>]
            [1.00 colour rgbf <0.54118, 0.58039, 0.56078, 0.0>]
        }
        scale <0.97, 0.97, 0.5>
        rotate <0, 5, 40>
    }
    finish {brilliance 1.25}
}
texture  {
    pigment {
        granite
        turbulence 1.25
        colour_map  {
            [0.15 colour rgbf <0.46667, 0.59216, 0.54118, 0.005>]
            [0.65 colour rgbf <0.35294, 0.67451, 0.54902, 0.750>]
            [0.80 colour rgbf <0.46667, 0.59216, 0.54118, 0.660>]
            [1.00 colour rgbf <0.35294, 0.67451, 0.54902, 0.879>]
        }
        scale <0.85, 1.5, 0.5>
        rotate <10, 5, -75>
    }
    finish { ambient 0.175 diffuse 0.6975 crand 0.025 }
}

// Dark powder blue w/steel blue & grey swirls
#declare Stone41 = texture  {
    pigment {
        marble
        turbulence 1.5
        omega 0.6235
        lambda 2.25
        colour_map  {
            [0.15 colour rgbf <0.03922, 0.20784, 0.52941, 0.0>]
            [0.45 colour rgbf <0.03922, 0.20784, 0.52941, 0.0>]
            [0.55 colour rgbf <0.22745, 0.23922, 0.42353, 0.0>]
            [0.75 colour rgbf <0.20784, 0.30980, 0.54118, 0.0>]
            [1.00 colour rgbf <0.20784, 0.30980, 0.54118, 0.0>]
        }
        rotate <0, 0, 85>
        scale <0.75, 1.33, 0.35>
        rotate <10, 5, 45>
    }
    finish { brilliance 1.825 }
}
texture  {
    pigment {
        granite
        turbulence  1.15
        colour_map  {
            [0.25 colour rgbf <0.30196, 0.35294, 0.48627, 0.600>]
            [0.45 colour rgbf <0.30196, 0.35294, 0.48627, 0.475>]
            [0.85 colour rgbf <0.17647, 0.24706, 0.39216, 0.715>]
            [1.00 colour rgbf <0.17647, 0.24706, 0.39216, 0.750>]
        }
        rotate <0,0,45>
       scale <0.78, 1.0, 0.4>
       rotate <5, 10, -55>
    }
    finish { ambient 0.225 diffuse 0.75 crand 0.01975}
}

//Brick red w/yellow-green swirls
#declare Stone42 = texture  {
    pigment {
        onion
        turbulence  2.1
        omega 0.575
        colour_map  {
           [0.15 colour rgbf<0.29020, 0.08235, 0.00000, 0.0>]
           [0.35 colour rgbf<0.34118, 0.29020, 0.09020, 0.0>]
           [0.65 colour rgbf<0.29020, 0.08235, 0.00000, 0.0>]
           [0.85 colour rgbf<0.34118, 0.29020, 0.09020, 0.0>]
           [1.00 colour rgbf<0.29020, 0.08235, 0.00000, 0.0>]
        }
        scale <0.9, 1.6, .45>
        rotate <10, 5, 45>
    }
    finish { brilliance 1.825 }
}
texture  {
    pigment {
        granite
        turbulence 0.925
        colour_map  {
            [0.25 colour rgbf<0.42353, 0.37255, 0.05882, 0.45>]
            [0.50 colour rgbf<0.18431, 0.01569, 0.00000, 0.55>]
            [0.72 colour rgbf<0.42353, 0.37255, 0.05882, 0.55>]
            [1.00 colour rgbf<0.18431, 0.01569, 0.00000, 0.65>]
        }
        rotate <5, 15, 90>
        scale <0.78, 1.05, 0.4>
        rotate <5, 10, -55>
    }
    finish { ambient 0.225 diffuse 0.75 crand 0.01975 }
}

// Rusty red w/cream swirls and duck overtones
#declare Stone43 = texture  {
    pigment {
        granite
        turbulence 0.815
        colour_map {
            [0.15 colour rgbf<0.86275, 0.75686, 0.61176, 0.0>]
            [0.45 colour rgbf<0.49804, 0.21569, 0.03922, 0.0>]
            [0.65 colour rgbf<0.84314, 0.72157, 0.63529, 0.0>]
            [0.85 colour rgbf<0.46667, 0.23922, 0.05098, 0.0>]
            [1.00 colour rgbf<0.87059, 0.73725, 0.62745, 0.0>]
        }
        scale <0.87, 1.235, 0.47>
        rotate <15, 25, 45>
    }
    finish {brilliance 2.585}
}
texture  {
    pigment {
        agate
        agate_turb 1.275
        colour_map {
            [0.35 colour rgbf<0.42353, 0.12157, 0.01176, 0.650>]
            [0.65 colour rgbf<0.50196, 0.35294, 0.21176, 0.750>]
            [1.00 colour rgbf<0.41569, 0.20784, 0.02745, 0.550>]
        }
        scale <0.69,1.27, 0.35>
        rotate <10, 5, -60>
    }
    finish { ambient 0.225 diffuse 0.6975 crand 0.02175 }
}

// This one is quite different from the rest.
// Its a dark, dull, bumpy rock texture.
#declare Stone44 = texture {
    pigment {
        granite
        color_map {
            [0.0 rgb 0.3 ]
            [1.0 rgb 0.7 ]
        }
        scale 0.075
    }
    normal  { granite 0.75 scale 0.075 }
}
texture {
    pigment {
        wrinkles
        turbulence 0.3
        scale 0.3
        color_map {
            [0.0 rgbf< 0.50, 0.25, 0.10, 0.85>]
            [1.0 rgbf< 0.65, 0.40, 0.00, 0.65>]
        }
    }
}

// A light tan wood - heavily grained (variable coloration)
#declare T_Wood31=
texture {
    pigment {
        wood
        turbulence 0.045
        scale 0.125
        color_map {
          [0.10 color rgbf <0.49020, 0.22353, 0.00000, 0.00>]
          [0.16 color rgbf <0.71863, 0.69412, 0.46275, 0.00>]
          [0.20 color rgbf <0.49020, 0.22353, 0.00000, 0.00>]
          [0.25 color rgbf <0.76863, 0.69412, 0.46275, 0.00>]
          [0.32 color rgbf <0.44020, 0.22353, 0.00000, 0.00>]
          [0.37 color rgbf <0.74863, 0.69412, 0.46275, 0.00>]
          [0.40 color rgbf <0.49020, 0.20353, 0.00000, 0.00>]
          [0.45 color rgbf <0.76863, 0.66412, 0.48275, 0.00>]
          [0.51 color rgbf <0.49020, 0.22353, 0.00000, 0.00>]
          [0.55 color rgbf <0.76863, 0.67059, 0.37255, 0.00>]
          [0.61 color rgbf <0.49020, 0.25353, 0.00000, 0.00>]
          [0.65 color rgbf <0.76863, 0.67059, 0.37255, 0.00>]
          [0.70 color rgbf <0.49020, 0.22353, 0.00000, 0.00>]
          [0.75 color rgbf <0.76863, 0.63059, 0.37255, 0.00>]
          [0.82 color rgbf <0.49020, 0.22353, 0.00000, 0.00>]
          [0.88 color rgbf <0.77863, 0.67059, 0.37255, 0.00>]
          [0.91 color rgbf <0.49020, 0.22353, 0.00000, 0.00>]
          [0.95 color rgbf <0.79863, 0.67059, 0.39550, 0.00>]
          [1.00 color rgbf <0.48020, 0.22353, 0.00000, 0.00>]
        }
    }
}

#declare Old_Wood = texture {
   pigment {
    bozo
    turbulence 0.97
    color_map {         
      [ 0.00 0.25 color rgb <0.138039, 0.104902, 0.05>
                  color rgb <0.203539, 0.114902, 0.02> ]
      [ 0.25 0.50 color rgb <0.203539, 0.114902, 0.02>
                  color rgb <0.203549, 0.100000, 0.05> ]
      [ 0.50 0.75 color rgb <0.203549, 0.100000, 0.05>
                  color rgb <0.118139, 0.124902, 0.01> ]
      [ 0.75 1.00 color rgb <0.118039, 0.124932, 0.01>
                  color rgb <0.138039, 0.104902, 0.05> ]
    }
  }
  normal {
    bumps 0.1
    scale <0.001,2,1>
  }
  finish { phong 0.4 }
}

//A rich dark reddish wood, like rosewood, with smooth-flowing grain
#declare T_Wood32=
texture {
    pigment {
        wood turbulence 0.04
        scale 0.15
        color_map {
            [0.15 color rgbf <0.38039, 0.14902, 0.0, 0.0>]
            [0.25 color rgbf <0.23539, 0.00000, 0.0, 0.0>]
            [0.35 color rgbf <0.38139, 0.14912, 0.0, 0.0>]
            [0.45 color rgbf <0.23549, 0.00000, 0.0, 0.0>]
            [0.55 color rgbf <0.38139, 0.14902, 0.0, 0.0>]
            [0.65 color rgbf <0.23559, 0.00000, 0.0, 0.0>]
            [0.75 color rgbf <0.38139, 0.14922, 0.0, 0.0>]
            [0.85 color rgbf <0.23549, 0.00000, 0.0, 0.0>]
            [0.95 color rgbf <0.38039, 0.14902, 0.0, 0.0>]
            [0.90 color rgbf <0.23539, 0.00000, 0.0, 0.0>]
            [1.00 color rgbf <0.38039, 0.14932, 0.0, 0.0>]
        }
    }
}

// Similar to T_WoodB, but brighter
#declare T_Wood33=
texture {
    pigment {
        wood turbulence 0.0425
        scale 0.2
        color_map {
            [0.05 color rgbf <0.55294, 0.21176, 0.00000, 0.0>]
            [0.15 color rgbf <0.32549, 0.13725, 0.00000, 0.0>]
            [0.25 color rgbf <0.55294, 0.21176, 0.00000, 0.0>]
            [0.35 color rgbf <0.32549, 0.11765, 0.00000, 0.0>]
            [0.48 color rgbf <0.55294, 0.21176, 0.00000, 0.0>]
            [0.55 color rgbf <0.29412, 0.13725, 0.01176, 0.0>]
            [0.65 color rgbf <0.55294, 0.21176, 0.00000, 0.0>]
            [0.78 color rgbf <0.32549, 0.13725, 0.00000, 0.0>]
            [0.85 color rgbf <0.55294, 0.21176, 0.00000, 0.0>]
            [0.96 color rgbf <0.28627, 0.13725, 0.00000, 0.0>]
            [1.00 color rgbf <0.54510, 0.17647, 0.03529, 0.0>]
        }
        translate <.015, 0, 0>
    }
}

// Reddish-orange, large, smooth grain.
#declare T_Wood34 =
texture {
    pigment {
        wood turbulence 0.04
        scale 0.15
        color_map {
            [0.15 color rgbf <0.38039, 0.14902, 0.0, 0.0>]
            [0.25 color rgbf <0.23539, 0.00000, 0.0, 0.0>]
            [0.35 color rgbf <0.38139, 0.14912, 0.0, 0.0>]
            [0.45 color rgbf <0.23549, 0.00000, 0.0, 0.0>]
            [0.55 color rgbf <0.38139, 0.14902, 0.0, 0.0>]
            [0.65 color rgbf <0.23559, 0.00000, 0.0, 0.0>]
            [0.75 color rgbf <0.38139, 0.14922, 0.0, 0.0>]
            [0.85 color rgbf <0.23549, 0.00000, 0.0, 0.0>]
            [0.95 color rgbf <0.38039, 0.14902, 0.0, 0.0>]
            [0.90 color rgbf <0.23539, 0.00000, 0.0, 0.0>]
            [1.00 color rgbf <0.38039, 0.14932, 0.0, 0.0>]
        }
    }
}
texture  {
    pigment {
        onion
        turbulence 0.2125
        colour_map {
            [0.225 colour rgbf <1.000000, 0.53333, 0.11767, 0.4875>]
            [0.350 colour rgbf <0.662750, 0.28617, 0.00001, 0.7250>]
            [0.500 colour rgbf <1.000100, 0.53333, 0.11765, 0.5745>]
            [0.625 colour rgbf <0.662775, 0.28627, 0.00005, 0.6875>]
            [0.750 colour rgbf <1.000200, 0.53333, 0.11755, 0.5275>]
            [0.875 colour rgbf <0.662755, 0.28629, 0.00001, 0.3795>]
            [1.000 colour rgbf <1.000000, 0.53333, 0.11665, 0.6165>]
        }
        scale <0.225, 0.20, 1.15>
    }
}


// Orangish, with a grain more like a veneer than a plank
#declare T_Wood35=
texture {
    pigment {
        wood
        turbulence 0.03725
        omega 0.65725
        lambda 2.425
        color_map {
            [0.250 color rgbf <1.00000, 0.53373, 0.11665, 0.000>]
            [0.350 color rgbf <0.66275, 0.28607, 0.00000, 0.000>]
            [0.525 color rgbf <1.00000, 0.53363, 0.11715, 0.000>]
            [0.600 color rgbf <0.66475, 0.28647, 0.00000, 0.000>]
            [0.750 color rgbf <1.00000, 0.53353, 0.11565, 0.000>]
            [0.850 color rgbf <0.66275, 0.28667, 0.00000, 0.000>]
            [1.000 color rgbf <1.00000, 0.53143, 0.11795, 0.000>]
        }
        scale <0.25, 0.225, 1.0>
    }
}
texture {
    pigment {
        wood
        scale 1.01275
        turbulence 0.0435
        omega 0.65
        lambda 3.15
        color_map {
            [0.200 color rgbf <0.56695, 0.17347, 0.00000, 0.8250>]
            [0.350 color rgbf <0.96471, 0.54510, 0.22753, 0.7710>]
            [0.400 color rgbf <0.56341, 0.17547, 0.00000, 0.9150>]
            [0.615 color rgbf <0.96472, 0.54510, 0.22553, 0.7590>]
            [0.700 color rgbf <0.56671, 0.17687, 0.00000, 0.7920>]
            [0.850 color rgbf <0.96485, 0.54510, 0.22453, 0.8975>]
            [1.000 color rgbf <0.56478, 0.17247, 0.00000, 0.9750>]
        }
        scale <0.225, 0.2725, 1.0>
        translate <-0.35, 0.095, 1.25>
    }
}

// ----------------------------------------------------------------------------
// ---- KLAUS LEPSCHI TEXTURES ------------------------------------------------
// ----------------------------------------------------------------------------

#declare Fog_Light_Halo = texture {
   pigment  { color rgbt <1,1,1,1> }
   halo {
      emitting
      spherical_mapping
      linear
      color_map   {
         [0.00 color rgbt <0.75,0.75 ,0.75 ,  1.00>]   // A
         [0.87 color rgbt <0.80, 0.80, 0.80, -0.10>]   // B
         [0.88 color rgbt <0.98, 0.98, 0.98, -1.00>]   // C
         [1.00 color rgbt <1.00, 1.00, 1.00, -1.00>]   // D
      }
      samples 20
   }
   finish {
      ambient 0
      diffuse 0
   }
}

#declare Halo___Bulb01 = texture {
   pigment  { color rgbt <1,1,1,1> }
   halo {
      emitting
      spherical_mapping
      linear
      color_map   {
         [0.00 color rgbt <0.75, 0.05, 0.05,  0.60>]
         [0.03 color rgbt <0.95, 0.85, 0.70,  0.10>]
         [1.00 color rgbt <0.98, 0.92, 0.92, -1.00>]
      }
      samples 20
   }
   finish {
      ambient 0
      diffuse 0
   }
}

//------------------------------------------------------------------
// TEXTURE........: FX__ExplosionNovae01
// SUGGESTED GROUP: Special effects
//------------------------------------------------------------------
// play with the turbulence of both halos, to make the explosion
// more massive or finer (lower turb. = more massive)
//------------------------------------------------------------------
#declare FX__ExplosionNovae01 = texture {
   pigment { color rgbt 1 }
   //---------------------------------------------------------------
   // layer one: <ze light sucker>
   //---------------------------------------------------------------
   halo {
      attenuating
      spherical_mapping
      linear
      turbulence 5.2824
      color_map {
         [0.00 0.20  color rgbt <0.55, 0.14, 0.14,  1.00>
                     color rgbt <0.55, 0.14, 0.14,  0.95>]
         [0.20 0.75  color rgbt <0.55, 0.13, 0.14,  0.52>
                     color rgbt <1.00, 0.50, 0.00,  0.89>]
         [0.75 1.00  color rgbt <0.55, 0.52, 0.45,  0.99>
                     color rgbt <0.00, 0.05, 0.05,  0.99>]
      }
      frequency 1.65
      scale <0.45, 0.71, 0.61>
   }
   //---------------------------------------------------------------
   // layer two: the inner section
   //--------------------------------------------------------------- 
   halo {
      emitting
      spherical_mapping
      linear
      turbulence 0.7785
      color_map {
         [0.00 0.20  color rgbt <0.00, 0.00, 0.00,  1.00>
                     color rgbt <0.75, 0.14, 0.15, -0.29>]
         [0.20 1.00  color rgbt <0.55, 0.14, 0.14, -0.27>
                     color rgbt <1.00, 0.50, 0.00,  0.48>]
      }
      frequency 3
      samples 20
      scale 0.80
   }
   finish  {
      ambient 0.0
      diffuse 1.0
   }
}

//------------------------------------------------------------------
// TEXTURE .......: Ceramic__WhiteBump
// SUGGESTED GROUP: Ceramic, Stone, Tiles etc.
//------------------------------------------------------------------
// ceramic-like texture with smooth bumps in two layers and a glassy
// finish for easy reflection (well suited for tiles etc.)
//------------------------------------------------------------------
#declare Ceramic___WhiteBump = texture {
   pigment { color rgb 1 }
   normal {
      bumps 0.096
      scale 0.092
   }
   finish {
      ambient 0.2
      diffuse 0.65
      reflection 0.0025
      specular 0.2
      roughness 0.125
      phong 0.25
   }
}
texture {
  pigment {color rgbf <1.0, 1.0, 1.0, 0.99999>}
  finish {
    ambient 0.0
    diffuse 0.0
    reflection 0.1    
    refraction 1.0
    specular 1.0
    roughness 0.01
    phong 1.0
  }
}

// --------------------------------------------------------------------------
// -- Texture file - Kevin Ison -  Sept. 1996 -------------------------------
// --------------------------------------------------------------------------
#declare Tx_1 = 
texture {
  pigment
  {
    marble
    turbulence 0.16  octaves 4  omega 0.61  lambda 1.9
    color_map
    {
      [0.00 color rgbf<0.400, 0.000, 0.129, 0.137>]
      [1.00 color rgbf<0.573, 0.804, 0.773, 0.525>]
    }
    phase 0.12
  }
  normal
  {
    ripples 0.2
    turbulence 0.4
    phase 0.43
  }
  finish {
    specular 0.01
  }
}

#declare Tx_2 =
texture {
  pigment
  {
    gradient x
    turbulence 0.19  octaves 4  omega 0.71
    color_map
    {
      [0.00 color rgbf<0.137, 0.000, 0.278, 0.110>]
      [0.60 color rgbf<0.624, 0.812, 0.961, 0.149>]
      [1.00 color rgbf<0.624, 0.812, 0.584, 0.318>]
    }
    phase 0.08
  }
  normal
  {
    dents 0.25
    phase 0.12
  }
  finish {
    specular 0.01    roughness 0.08    metallic
    reflection 0.01
    crand 0.06
  }
}
texture {
  pigment
  {
    bozo
    color_map
    {
      [0.00 color rgb<0.239, 0.098, 0.090>]
      [0.38 color rgb<0.682, 0.624, 1.000>]
      [0.49 color rgbf<1.000, 0.761, 0.694, 0.200>]
      [0.84 color rgbf<0.239, 0.318, 0.286, 0.098>]
      [1.00 color rgb<0.843, 0.843, 0.824>]
    }
  }
}


// wierd texture
#declare Tx_3 = 
texture {
  pigment
  {
    radial
    turbulence 0.76  octaves 4  omega 0.36  lambda 3.6
    color_map
    {
      [0.00 color rgbf<0.227, 0.306, 0.129, 0.071>]
      [0.05 color rgb<0.196, 0.149, 0.259>]
      [0.20 color rgbf<0.149, 0.239, 0.247, 0.039>]
      [1.00 color rgb<0.643, 0.902, 0.733>]
    }
    phase 0.1
    frequency 5
  }
  finish {
    reflection 0.04
    refraction 1.0
    crand 0.05
  }
}

#declare Wierd_3 =
texture {
  pigment
  {
    agate
    agate_turb 0.97  octaves 2  omega 0.69  lambda 0.9
    color_map
    {
      [0.00 color rgbf<0.345, 0.220, 0.000, 0.090>]
      [0.24 color rgbf<0.000, 0.400, 0.408, 0.435>]
      [0.40 color rgbf<0.200, 0.098, 0.000, 0.110>]
      [0.60 color rgb<0.384, 0.220, 0.286>]
      [0.81 color rgbf<0.467, 0.318, 0.494, 0.675>]
      [1.00 color rgbf<1.000, 1.000, 1.000, 0.200>]
    }
    frequency 2
  }
  finish {
    ambient 0.15
    reflection 0.12
  }
}
texture {
  pigment
  {
    colour rgb<0.651,0.435,0.435>
  }
  normal
  {
    ripples 0.51
    turbulence 0.43  octaves 2  omega 0.24  lambda 2.9
    phase 0.18
    frequency 4
  }
  finish {
    specular 0.01    roughness 0.08
    reflection 0.18
    refraction 1.0
  }
}


#declare Tx_4 = 
texture {
  pigment
  {
    bozo
    turbulence 0.43  octaves 3  omega 0.51  lambda 4.4
    color_map
    {
      [0.00 color rgbf<0.169, 0.176, 0.227, 0.780>]
      [0.34 color rgbf<0.286, 0.259, 0.298, 0.780>]
      [1.00 color rgbf<0.929, 0.961, 1.000, 0.427>]
    }
    phase 0.08
    scale <2.0, 2.0, 2.0>
  }
  normal
  {
    ripples 0.49
    turbulence 1.11  octaves 3  omega 0.42  lambda 0.1
    phase 0.27
    frequency 6
  }
  finish {
    brilliance 1.1
    refraction 1.0
  }
}

#declare Tx_5 = 
texture {
  pigment
  {
    gradient x
    turbulence 0.86  octaves 2  omega 0.12  lambda 4.5
    color_map
    {
      [0.00 color rgbf<0.545, 0.365, 0.000, 0.090>]
      [1.00 color rgb<0.702, 0.733, 0.761>]
    }
    phase 0.06
    frequency 3
  }
  finish {
    specular 0.12    metallic
    reflection 0.16
    refraction 1.0
    crand 0.08
  }
}

#declare ki_Gold = 
texture {
  pigment
  {
    colour rgb<0.988,0.584,0.024>
  }
  normal
  {
    waves 0.04
    turbulence 1.14
    phase 0.37
    frequency 4.3
  }
  finish {
    diffuse 0.8
    reflection 0.19
  }
}

#declare Tx_6 =
texture {
  pigment
  {
    leopard
    turbulence 0.4  octaves 4  omega 0.69
    color_map
    {
      [0.00 color rgb<0.584, 0.592, 0.592>]
      [0.10 color rgbf<0.318, 0.090, 0.200, 0.098>]
      [1.00 color rgbf<0.761, 0.682, 0.592, 0.137>]
    }
    phase 0.2
  }
  finish {
    ambient 0.39
    diffuse 0.8
  }
}
texture {
  pigment
  {
    colour rgb<0.980,0.808,0.055>
  }
  normal
  {
    ripples 0.6
    turbulence 0.05  octaves 2  omega 0.13  lambda 1.1
    phase 0.27
    frequency -7
  }
  finish {
    ambient 0.06
    refraction 1.0    ior 2.0
  }
}


#declare Tx_7 =
texture {
  pigment
  {
    colour rgb<0.973,0.796,0.027>
  }
  finish {
    diffuse 0.81
    reflection 0.49
  }
}
texture {
  pigment
  {
    agate
    agate_turb 1.76  omega 0.12  lambda 2.4
    color_map
    {
      [0.00 color rgbf<0.000, 0.000, 0.000, 1.000>]
      [0.18 color rgbf<0.075, 0.467, 0.196, 0.031>]
      [1.00 color rgb<0.792, 1.000, 1.000>]
    }
  }
  finish {
    refraction 1.0    ior 2.0
  }
}

// ----------------------------------------------------------------------------
// --  (C) 1996 Correia Emmanuel-AXEM - bois jaune ----------------------------
// ----------------------------------------------------------------------------

#declare BoisJaune = texture {
  pigment
  {
    wood
    turbulence 0.02
    color_map
    {
      [0.00 color rgb<0.808, 0.671, 0.251>]
      [0.16 color rgb<0.808, 0.671, 0.251>]
      [0.34 color rgb<0.600, 0.345, 0.039>]
      [0.48 color rgb<0.808, 0.671, 0.251>]
      [0.65 color rgb<0.808, 0.671, 0.251>]
      [0.82 color rgb<0.529, 0.294, 0.024>]
      [1.00 color rgb<0.808, 0.671, 0.251>]
    }
    translate <1.0, 0.0, 0.0>
    rotate <75.0, 0.0, 0.0>
    scale <1.0, 20.0, 1.0>
  }
  finish {
    ambient 0.32
    diffuse 0.63
    phong 0.2    phong_size 10
  }
}
texture {
  pigment
  {
    granite
    turbulence 1.0
    color_map
    {
      [0.00 color rgbf<0.498, 0.349, 0.000, 1.000>]
      [0.39 color rgbf<0.478, 0.329, 0.000, 1.000>]
      [0.60 color rgbf<0.478, 0.329, 0.000, 0.686>]
      [0.84 color rgbf<0.486, 0.337, 0.000, 0.486>]
      [0.94 color rgb<0.486, 0.337, 0.000>]
      [1.00 color rgb<0.498, 0.349, 0.000>]
    }
    scale <1.0, 5.0, 1.0>
  }
  finish {
    ambient 0.2
    specular 0.2
  }
}
texture {
  pigment
  {
    wood
    turbulence 0.01
    color_map
    {
      [0.00 color rgbf<1.000, 1.000, 1.000, 1.000>]
      [0.12 color rgbf<0.702, 0.412, 0.118, 0.608>]
      [0.23 color rgbf<0.702, 0.467, 0.118, 0.608>]
      [0.50 color rgbf<1.000, 1.000, 1.000, 1.000>]
      [0.70 color rgbf<1.000, 1.000, 1.000, 1.000>]
      [0.83 color rgbf<0.702, 0.467, 0.118, 0.608>]
      [1.00 color rgbf<1.000, 1.000, 1.000, 1.000>]
    }
    frequency 4
    translate <1.0, 0.0, 0.0>
    rotate <75.0, 0.0, 0.0>
    scale <5.0, 5.0, 5.0>
  }
}

// -------------------------------------------------------------------------
// -- SPECIAL WOODEN TEXTURE -----------------------------------------------
// -------------------------------------------------------------------------


// ----- WOODS001-------------


#declare T_Wood1 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.880, 0.600, 0.400 > ]
            [ 0.100 rgb < 0.880, 0.600, 0.400 > ]
            [ 0.100 rgb < 0.880, 0.600, 0.400 > ]
            [ 0.900 rgb < 0.600, 0.300, 0.200 > ]
            [ 0.900 rgb < 0.600, 0.300, 0.200 > ]
            [ 1.000 rgb < 0.600, 0.300, 0.200 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.550, 0.320, 0.200, 0.100 > ]
            [ 0.100 rgbf < 0.550, 0.320, 0.200, 0.500 > ]
            [ 0.100 rgbf < 0.550, 0.350, 0.200, 0.650 > ]
            [ 0.200 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 0.200 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 0.300 rgbf < 0.600, 0.300, 0.200, 1.000 > ]
            [ 0.300 rgbf < 0.600, 0.300, 0.200, 0.100 > ]
            [ 0.400 rgbf < 0.600, 0.300, 0.200, 0.500 > ]
            [ 0.400 rgbf < 0.600, 0.300, 0.200, 0.650 > ]
            [ 0.900 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 0.900 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 1.000 rgbf < 0.550, 0.320, 0.200, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood10 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 1.000, 0.850, 0.500 > ]
            [ 0.500 rgb < 0.900, 0.700, 0.460 > ]
            [ 0.500 rgb < 0.900, 0.700, 0.460 > ]
            [ 0.700 rgb < 0.900, 0.700, 0.460 > ]
            [ 0.700 rgb < 0.900, 0.700, 0.460 > ]
            [ 1.000 rgb < 1.000, 0.850, 0.500 > ]
        }
        scale < 0.175, 0.175, 0.175 >
        lambda 3
        octaves 4
        turbulence < 0.020, 0.020, 0.020 >
        rotate < 2.000, 2.000, 0.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 1.000, 0.450, 0.100, 0.800 > ]
            [ 0.500 rgbf < 0.850, 0.650, 0.400, 0.400 > ]
            [ 0.500 rgbf < 0.850, 0.650, 0.400, 0.400 > ]
            [ 0.700 rgbf < 0.850, 0.650, 0.400, 0.400 > ]
            [ 0.700 rgbf < 0.850, 0.650, 0.400, 0.400 > ]
            [ 1.000 rgbf < 1.000, 0.450, 0.100, 0.800 > ]
        }
        rotate < 2.000, 2.000, 0.000 >
        scale < 0.200, 0.200, 0.200 >
        lambda 2.8
        octaves 4
        turbulence < 0.020, 0.020, 0.020 >
        translate < 0.018, 0.018, 0.018 >
    }
}


#declare T_Wood11 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.222 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.222 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.342 rgb < 0.600, 0.340, 0.040 > ]
            [ 0.342 rgb < 0.600, 0.340, 0.040 > ]
            [ 0.393 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.393 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.709 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.709 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.821 rgb < 0.530, 0.290, 0.020 > ]
            [ 0.821 rgb < 0.530, 0.290, 0.020 > ]
            [ 1.000 rgb < 0.800, 0.670, 0.250 > ]
        }
        scale < 0.100, 0.100, 0.100 >
        turbulence < 0.020, 0.020, 0.020 >
        translate < 10.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.120 rgbf < 0.700, 0.410, 0.110, 0.600 > ]
            [ 0.120 rgbf < 0.700, 0.410, 0.110, 0.600 > ]
            [ 0.231 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 0.231 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 0.496 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.496 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.701 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.701 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.829 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 0.829 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        scale < 0.500, 0.500, 0.500 >
        turbulence < 0.010, 0.010, 0.010 >
        translate < 10.000, 0.000, 0.000 >
    }
}


#declare T_Wood12 =
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgb < 2.916, 1.600, 1.024 > ]
            [ 3.724 rgb < 2.681, 1.401, 1.050 > ]
            [ 0.256 rgb < 2.145, 1.121, 0.840 > ]
            [ 3.663 rgb < 1.789, 0.969, 0.672 > ]
            [ 0.393 rgb < 1.431, 0.775, 0.538 > ]
            [ 3.465 rgb < 0.955, 0.524, 0.335 > ]
            [ 0.581 rgb < 0.764, 0.419, 0.268 > ]
            [ 2.772 rgb < 0.764, 0.367, 0.305 > ]
            [ 0.726 rgb < 0.611, 0.294, 0.244 > ]
            [ 2.400 rgb < 0.450, 0.235, 0.156 > ]
            [ 0.983 rgb < 0.360, 0.188, 0.125 > ]
            [ 1.563 rgb < 0.250, 0.137, 0.088 > ]
        }
        octaves 101.868
        omega 1.454
        lambda 36.375
        turbulence < 0.594, 0.594, 0.594 >
        scale < 7.274, 0.742, 0.742 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.139 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.139 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.148 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.148 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.287 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.287 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.443 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.443 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.626 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.626 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.635 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.635 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.843 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.843 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 1.000 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
        }
        scale < 0.150, 0.500, 1.000 >
        turbulence < 0.100, 0.040, 1.000 >
        rotate < 2.000, 0.000, 0.000 >
    }
}


#declare T_Wood13 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.100 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.100 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.900 rgb < 0.900, 0.650, 0.300 > ]
            [ 0.900 rgb < 0.900, 0.650, 0.300 > ]
            [ 1.000 rgb < 0.600, 0.350, 0.200 > ]
        }
        scale < 0.100, 0.100, 0.100 >
        turbulence < 0.020, 0.020, 0.020 >
        translate < 10.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.400 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 0.400 rgbf < 0.900, 0.650, 0.300, 0.000 > ]
            [ 0.500 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 0.500 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.010, 0.010, 0.010 >
        scale < 0.050, 0.050, 0.050 >
    }
}


#declare T_Wood2 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.005, 0.002, 0.002 > ]
            [ 0.002 rgb < 0.011, 0.005, 0.004 > ]
            [ 0.100 rgb < 0.022, 0.013, 0.010 > ]
            [ 0.056 rgb < 0.058, 0.032, 0.023 > ]
            [ 0.900 rgb < 0.115, 0.065, 0.045 > ]
            [ 0.250 rgb < 0.175, 0.080, 0.060 > ]
        }
        octaves 0.047
        turbulence < 0.001, 0.001, 0.001 >
        scale < 0.002, 0.002, 0.016 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.500, 0.300, 0.200, 0.100 > ]
            [ 0.100 rgbf < 0.500, 0.300, 0.200, 0.500 > ]
            [ 0.100 rgbf < 0.500, 0.300, 0.200, 0.650 > ]
            [ 0.900 rgbf < 0.500, 0.250, 0.150, 0.975 > ]
            [ 0.900 rgbf < 0.500, 0.250, 0.150, 0.975 > ]
            [ 1.000 rgbf < 0.500, 0.300, 0.200, 0.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.250, 0.250, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.350, 0.100 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood21 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.222 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.222 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.342 rgb < 0.600, 0.340, 0.040 > ]
            [ 0.342 rgb < 0.600, 0.340, 0.040 > ]
            [ 0.393 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.393 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.709 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.709 rgb < 0.800, 0.670, 0.250 > ]
            [ 0.821 rgb < 0.530, 0.290, 0.020 > ]
            [ 0.821 rgb < 0.530, 0.290, 0.020 > ]
            [ 1.000 rgb < 0.800, 0.670, 0.250 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.120 rgbf < 0.700, 0.410, 0.110, 0.600 > ]
            [ 0.120 rgbf < 0.700, 0.410, 0.110, 0.600 > ]
            [ 0.231 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 0.231 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 0.496 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.496 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.701 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.701 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.829 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 0.829 rgbf < 0.700, 0.460, 0.110, 0.600 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood22 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 2.916, 1.600, 1.024 > ]
            [ 3.724 rgb < 2.681, 1.401, 1.050 > ]
            [ 0.256 rgb < 2.145, 1.121, 0.840 > ]
            [ 3.663 rgb < 1.789, 0.969, 0.672 > ]
            [ 0.393 rgb < 1.431, 0.775, 0.538 > ]
            [ 3.465 rgb < 0.955, 0.524, 0.335 > ]
            [ 0.581 rgb < 0.764, 0.419, 0.268 > ]
            [ 2.772 rgb < 0.764, 0.367, 0.305 > ]
            [ 0.726 rgb < 0.611, 0.294, 0.244 > ]
            [ 2.400 rgb < 0.450, 0.235, 0.156 > ]
            [ 0.983 rgb < 0.360, 0.188, 0.125 > ]
            [ 1.563 rgb < 0.250, 0.137, 0.088 > ]
        }
        octaves 43.656
        turbulence < 0.594, 0.594, 0.594 >
        scale < 0.742, 0.742, 14.554 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.139 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.139 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.148 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.148 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.287 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.287 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.443 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.443 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.626 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.626 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.635 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.635 rgbf < 0.450, 0.350, 0.320, 0.800 > ]
            [ 0.843 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 0.843 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
            [ 1.000 rgbf < 0.545, 0.349, 0.247, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood23 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.100 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.100 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.900 rgb < 0.900, 0.650, 0.300 > ]
            [ 0.900 rgb < 0.900, 0.650, 0.300 > ]
            [ 1.000 rgb < 0.600, 0.350, 0.200 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.400 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 0.400 rgbf < 0.900, 0.650, 0.300, 0.000 > ]
            [ 0.500 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 0.500 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood24 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.100 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.100 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.900 rgb < 0.450, 0.115, 0.060 > ]
            [ 0.900 rgb < 0.450, 0.115, 0.060 > ]
            [ 1.000 rgb < 0.450, 0.115, 0.060 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.700, 0.232, 0.115, 0.500 > ]
            [ 0.100 rgbf < 0.700, 0.232, 0.115, 0.700 > ]
            [ 0.100 rgbf < 0.700, 0.232, 0.115, 0.700 > ]
            [ 0.150 rgbf < 0.350, 0.115, 0.060, 0.900 > ]
            [ 0.150 rgbf < 0.700, 0.232, 0.115, 0.900 > ]
            [ 0.200 rgbf < 0.350, 0.115, 0.060, 0.700 > ]
            [ 0.200 rgbf < 0.350, 0.115, 0.060, 0.700 > ]
            [ 1.000 rgbf < 0.350, 0.115, 0.060, 0.500 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood25 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.400 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.003, 0.002, 0.001 > ]
            [ 0.500 rgb < 0.006, 0.003, 0.001 > ]
            [ 0.008 rgb < 0.010, 0.006, 0.001 > ]
            [ 0.700 rgb < 0.025, 0.015, 0.004 > ]
            [ 0.044 rgb < 0.060, 0.036, 0.010 > ]
            [ 0.980 rgb < 0.148, 0.091, 0.024 > ]
            [ 0.280 rgb < 0.353, 0.217, 0.055 > ]
        }
        octaves 0.002
        turbulence < 0.000, 0.000, 0.000 >
        scale < 1.000, 1.000, 0.001 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.404, 0.210, 0.078, 0.200 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.800 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.800 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.200 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.200 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.600 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.600 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.400 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.400 > ]
            [ 1.000 rgbf < 0.404, 0.210, 0.078, 0.100 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood26 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.400 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.003, 0.002, 0.001 > ]
            [ 0.500 rgb < 0.006, 0.003, 0.001 > ]
            [ 0.008 rgb < 0.010, 0.006, 0.001 > ]
            [ 0.700 rgb < 0.025, 0.015, 0.004 > ]
            [ 0.044 rgb < 0.060, 0.036, 0.010 > ]
            [ 0.980 rgb < 0.148, 0.091, 0.024 > ]
            [ 0.280 rgb < 0.353, 0.217, 0.055 > ]
        }
        octaves 0.002
        turbulence < 0.000, 0.000, 0.000 >
        scale < 1.000, 1.000, 0.001 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.404, 0.210, 0.078, 0.300 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.600 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.600 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.300 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.300 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.400 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.400 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.500 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.500 > ]
            [ 1.000 rgbf < 0.404, 0.210, 0.078, 0.300 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood27 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.001 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.001 > ]
            [ 0.400 rgb < 0.002, 0.001, 0.001 > ]
            [ 0.001 rgb < 0.004, 0.002, 0.001 > ]
            [ 0.500 rgb < 0.011, 0.006, 0.002 > ]
            [ 0.011 rgb < 0.033, 0.023, 0.005 > ]
            [ 0.700 rgb < 0.083, 0.059, 0.013 > ]
            [ 0.127 rgb < 0.211, 0.136, 0.065 > ]
            [ 0.980 rgb < 0.351, 0.227, 0.108 > ]
            [ 0.540 rgb < 0.540, 0.360, 0.135 > ]
        }
        octaves 0.002
        turbulence < 0.000, 0.000, 0.000 >
        scale < 1.000, 1.000, 0.001 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.400, 0.200, 0.080, 0.100 > ]
            [ 0.250 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.250 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.400 rgbf < 0.450, 0.250, 0.090, 0.300 > ]
            [ 0.400 rgbf < 0.450, 0.250, 0.090, 0.300 > ]
            [ 0.500 rgbf < 0.400, 0.200, 0.080, 0.600 > ]
            [ 0.500 rgbf < 0.400, 0.200, 0.080, 0.600 > ]
            [ 0.700 rgbf < 0.460, 0.230, 0.100, 0.300 > ]
            [ 0.700 rgbf < 0.460, 0.230, 0.100, 0.300 > ]
            [ 0.980 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.980 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 1.000 rgbf < 0.400, 0.200, 0.080, 0.100 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood28 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.150 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.280 rgb < 0.002, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.007, 0.002, 0.000 > ]
            [ 0.400 rgb < 0.018, 0.007, 0.000 > ]
            [ 0.009 rgb < 0.044, 0.018, 0.000 > ]
            [ 0.500 rgb < 0.073, 0.036, 0.000 > ]
            [ 0.052 rgb < 0.123, 0.062, 0.000 > ]
            [ 0.700 rgb < 0.175, 0.079, 0.000 > ]
            [ 0.171 rgb < 0.250, 0.112, 0.000 > ]
            [ 0.980 rgb < 0.500, 0.225, 0.000 > ]
            [ 0.500 rgb < 1.000, 0.500, 0.000 > ]
        }
        octaves 0.001
        turbulence < 0.000, 0.000, 0.000 >
        scale < 1.000, 1.000, 0.001 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.500, 0.260, 0.120, 0.300 > ]
            [ 0.250 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.250 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.400 rgbf < 0.550, 0.280, 0.100, 0.600 > ]
            [ 0.400 rgbf < 0.550, 0.280, 0.100, 0.600 > ]
            [ 0.500 rgbf < 0.500, 0.230, 0.150, 1.000 > ]
            [ 0.500 rgbf < 0.500, 0.230, 0.150, 1.000 > ]
            [ 0.700 rgbf < 0.560, 0.290, 0.170, 0.600 > ]
            [ 0.700 rgbf < 0.560, 0.290, 0.170, 0.600 > ]
            [ 0.980 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.980 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 1.000 rgbf < 0.500, 0.260, 0.120, 0.300 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood29 =
texture {
    pigment {
        wood
        color_map {
            [ 0.00 rgb < 0.500, 0.250, 0.125 > ]
            [ 0.40 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.60 rgb < 1.000, 0.500, 0.250 > ]
            [ 1.00 rgb < 0.500, 0.250, 0.125 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.350, 0.175, 0.087 > ]
            [ 0.300 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood3 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.750, 0.650, 0.450 > ]
            [ 0.100 rgb < 0.750, 0.650, 0.450 > ]
            [ 0.100 rgb < 0.750, 0.650, 0.450 > ]
            [ 0.900 rgb < 0.700, 0.550, 0.400 > ]
            [ 0.900 rgb < 0.700, 0.550, 0.400 > ]
            [ 1.000 rgb < 0.750, 0.650, 0.450 > ]
        }
        scale < 0.050, 0.050, 1.000 >
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        rotate < 2.000, 2.000, 0.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.700, 0.600, 0.400, 0.100 > ]
            [ 0.100 rgbf < 0.700, 0.600, 0.400, 0.500 > ]
            [ 0.100 rgbf < 0.700, 0.600, 0.400, 0.650 > ]
            [ 0.900 rgbf < 0.750, 0.700, 0.600, 0.975 > ]
            [ 0.900 rgbf < 0.750, 0.700, 0.600, 0.975 > ]
            [ 1.000 rgbf < 0.700, 0.600, 0.400, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood30 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.880, 0.600, 0.400 > ]
            [ 0.100 rgb < 0.880, 0.600, 0.400 > ]
            [ 0.100 rgb < 0.880, 0.600, 0.400 > ]
            [ 0.900 rgb < 0.600, 0.300, 0.200 > ]
            [ 0.900 rgb < 0.600, 0.300, 0.200 > ]
            [ 1.000 rgb < 0.600, 0.300, 0.200 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.550, 0.320, 0.200, 0.100 > ]
            [ 0.100 rgbf < 0.550, 0.320, 0.200, 0.500 > ]
            [ 0.100 rgbf < 0.550, 0.350, 0.200, 0.650 > ]
            [ 0.200 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 0.200 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 0.300 rgbf < 0.600, 0.300, 0.200, 1.000 > ]
            [ 0.300 rgbf < 0.600, 0.300, 0.200, 0.100 > ]
            [ 0.400 rgbf < 0.600, 0.300, 0.200, 0.500 > ]
            [ 0.400 rgbf < 0.600, 0.300, 0.200, 0.650 > ]
            [ 0.900 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 0.900 rgbf < 0.880, 0.600, 0.400, 0.975 > ]
            [ 1.000 rgbf < 0.550, 0.320, 0.200, 1.000 > ]
        }
        rotate < 5.000, 10.000, 5.000 >
        scale < 0.150, 0.500, 1.000 >
        lambda 3.25
        octaves 5
        turbulence < 0.100, 0.500, 1.000 >
        translate < 0.000, 2.000, 0.000 >
    }
}


#declare T_Wood4 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.300, 0.100, 0.150 > ]
            [ 0.300 rgb < 0.300, 0.150, 0.150 > ]
            [ 0.300 rgb < 0.300, 0.150, 0.150 > ]
            [ 0.600 rgb < 0.500, 0.200, 0.150 > ]
            [ 0.600 rgb < 0.500, 0.200, 0.150 > ]
            [ 1.000 rgb < 0.300, 0.100, 0.150 > ]
        }
        omega 1.25
        octaves 4
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.150, 0.150, 1.000 >
    }
}
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgbf < 0.320, 0.150, 0.170, 0.000 > ]
            [ 0.300 rgbf < 0.320, 0.130, 0.150, 0.200 > ]
            [ 0.300 rgbf < 0.320, 0.130, 0.130, 0.350 > ]
            [ 0.400 rgbf < 0.520, 0.250, 0.230, 0.400 > ]
            [ 0.400 rgbf < 0.520, 0.250, 0.230, 0.200 > ]
            [ 0.600 rgbf < 0.320, 0.130, 0.130, 0.350 > ]
            [ 0.600 rgbf < 0.320, 0.130, 0.150, 0.500 > ]
            [ 1.000 rgbf < 0.320, 0.150, 0.200, 0.000 > ]
        }
        lambda 3.25
        octaves 5
        turbulence < 0.030, 0.030, 0.100 >
        scale < 0.025, 0.025, 1.000 >
    }
}


#declare T_Wood5 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.500, 0.350, 0.100 > ]
            [ 0.300 rgb < 0.500, 0.350, 0.100 > ]
            [ 0.300 rgb < 0.500, 0.350, 0.100 > ]
            [ 0.600 rgb < 0.600, 0.450, 0.100 > ]
            [ 0.600 rgb < 0.600, 0.450, 0.100 > ]
            [ 1.000 rgb < 0.500, 0.350, 0.100 > ]
        }
        omega 1.25
        octaves 4
        turbulence < 0.040, 0.040, 0.100 >
        scale < 0.150, 0.150, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.400, 0.350, 0.100, 1.000 > ]
            [ 0.100 rgbf < 0.400, 0.000, 0.100, 0.600 > ]
            [ 0.100 rgbf < 0.400, 0.000, 0.100, 0.750 > ]
            [ 0.900 rgbf < 0.350, 0.400, 0.150, 0.900 > ]
            [ 0.900 rgbf < 0.350, 0.400, 0.150, 0.900 > ]
            [ 1.000 rgbf < 0.400, 0.350, 0.100, 1.000 > ]
        }
        scale < 0.300, 0.200, 1.000 >
        omega 1.3
        octaves 4
        turbulence < 0.025, 0.025, 0.100 >
        translate < 0.100, 0.100, 0.200 >
    }
}


#declare T_Wood6 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.250, 0.100, 0.100 > ]
            [ 0.300 rgb < 0.250, 0.100, 0.100 > ]
            [ 0.300 rgb < 0.250, 0.100, 0.100 > ]
            [ 0.600 rgb < 0.600, 0.150, 0.100 > ]
            [ 0.600 rgb < 0.600, 0.150, 0.100 > ]
            [ 1.000 rgb < 0.250, 0.100, 0.100 > ]
        }
        scale < 0.150, 0.150, 1.000 >
        octaves 4
        turbulence < 0.050, 0.080, 1.000 >
        translate < 0.000, 100.000, 0.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.250, 0.100, 0.100, 0.000 > ]
            [ 0.300 rgbf < 0.250, 0.100, 0.100, 0.400 > ]
            [ 0.300 rgbf < 0.250, 0.100, 0.100, 0.400 > ]
            [ 0.500 rgbf < 0.600, 0.150, 0.100, 1.000 > ]
            [ 0.500 rgbf < 0.600, 0.150, 0.100, 1.000 > ]
            [ 0.700 rgbf < 0.250, 0.100, 0.100, 0.400 > ]
            [ 0.700 rgbf < 0.600, 0.150, 0.100, 0.400 > ]
            [ 1.000 rgbf < 0.250, 0.100, 0.100, 0.000 > ]
        }
        scale < 0.200, 0.200, 1.000 >
        omega 0.95
        octaves 4
        turbulence < 0.050, 0.050, 0.000 >
        rotate < 20.000, 0.000, 0.000 >
    }
}


#declare T_Wood7 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.100 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.100 rgb < 0.600, 0.350, 0.200 > ]
            [ 0.900 rgb < 0.900, 0.650, 0.300 > ]
            [ 0.900 rgb < 0.900, 0.650, 0.300 > ]
            [ 1.000 rgb < 0.600, 0.350, 0.200 > ]
        }
        octaves 4
        turbulence < 0.050, 0.080, 1000.000 >
        scale < 0.150, 0.150, 1.000 >
    }
}
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgbf < 0.900, 0.650, 0.300, 0.000 > ]
            [ 0.100 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 0.100 rgbf < 0.900, 0.650, 0.300, 0.300 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        scale < 0.010, 0.010, 100000.000 >
    }
}


#declare T_Wood8 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.450, 0.300, 0.150 > ]
            [ 0.100 rgb < 0.450, 0.300, 0.150 > ]
            [ 0.100 rgb < 0.550, 0.300, 0.150 > ]
            [ 0.900 rgb < 0.200, 0.150, 0.050 > ]
            [ 0.900 rgb < 0.200, 0.150, 0.050 > ]
            [ 1.000 rgb < 0.450, 0.300, 0.150 > ]
        }
        octaves 3
        turbulence < 0.040, 0.040, 0.040 >
        scale < 0.050, 0.050, 1.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 0.600, 0.350, 0.200, 0.300 > ]
            [ 0.500 rgbf < 0.600, 0.350, 0.200, 0.800 > ]
            [ 0.500 rgbf < 0.600, 0.350, 0.200, 0.800 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        octaves 2.640
        turbulence < 0.026, 0.026, 0.660 >
        scale < 0.033, 0.033, 0.660 >
    }
}


#declare T_Wood9 =
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 1.000, 0.850, 0.500 > ]
            [ 0.500 rgb < 0.600, 0.400, 0.160 > ]
            [ 0.500 rgb < 0.600, 0.400, 0.160 > ]
            [ 0.700 rgb < 0.600, 0.400, 0.160 > ]
            [ 0.700 rgb < 0.600, 0.400, 0.160 > ]
            [ 1.000 rgb < 1.000, 0.850, 0.500 > ]
        }
        scale < 0.200, 0.200, 0.200 >
        lambda 3
        octaves 4
        turbulence < 0.100, 0.100, 0.100 >
        rotate < 0.125, 0.125, 0.000 >
    }
}
texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgbf < 1.000, 0.700, 0.250, 1.000 > ]
            [ 0.500 rgbf < 0.500, 0.300, 0.060, 0.400 > ]
            [ 0.500 rgbf < 0.500, 0.300, 0.060, 0.400 > ]
            [ 0.700 rgbf < 0.500, 0.300, 0.060, 0.300 > ]
            [ 0.700 rgbf < 0.500, 0.300, 0.060, 0.400 > ]
            [ 1.000 rgbf < 1.000, 0.700, 0.250, 1.000 > ]
        }
        rotate < 0.125, 0.125, 0.000 >
        scale < 0.200, 0.200, 0.200 >
        lambda 3.5
        octaves 4
        turbulence < 0.100, 0.100, 0.100 >
        translate < 0.015, 0.015, 0.015 >
    }
}


#declare T_Wood14 =
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.100 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.100 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.900 rgb < 0.450, 0.115, 0.060 > ]
            [ 0.900 rgb < 0.450, 0.115, 0.060 > ]
            [ 1.000 rgb < 0.450, 0.115, 0.060 > ]
        }
        turbulence < 0.065, 0.065, 0.065 >
        octaves 2
        scale < 0.150, 0.150, 1.000 >
        translate < -1.000, 0.000, 0.000 >
        rotate < -3.000, -3.000, 0.000 >
        color_map {
            [ 0.000 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.100 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.100 rgb < 0.800, 0.232, 0.115 > ]
            [ 0.900 rgb < 0.450, 0.115, 0.060 > ]
            [ 0.900 rgb < 0.450, 0.115, 0.060 > ]
            [ 1.000 rgb < 0.450, 0.115, 0.060 > ]
        }
    }
}
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgbf < 0.700, 0.232, 0.115, 0.500 > ]
            [ 0.100 rgbf < 0.700, 0.232, 0.115, 0.700 > ]
            [ 0.100 rgbf < 0.700, 0.232, 0.115, 0.700 > ]
            [ 0.150 rgbf < 0.350, 0.115, 0.060, 0.900 > ]
            [ 0.150 rgbf < 0.700, 0.232, 0.115, 0.900 > ]
            [ 0.200 rgbf < 0.350, 0.115, 0.060, 0.700 > ]
            [ 0.200 rgbf < 0.350, 0.115, 0.060, 0.700 > ]
            [ 1.000 rgbf < 0.350, 0.115, 0.060, 0.500 > ]
        }
        turbulence < 0.000, 0.100, 0.000 >
        lambda 2.75
        omega 1.15
        octaves 4
        scale < 5.000, 0.075, 1.000 >
        rotate < 90.000, 0.000, 0.000 >
        color_map {
            [ 0.000 rgbf < 0.700, 0.232, 0.115, 0.500 > ]
            [ 0.100 rgbf < 0.700, 0.232, 0.115, 0.700 > ]
            [ 0.100 rgbf < 0.700, 0.232, 0.115, 0.700 > ]
            [ 0.150 rgbf < 0.350, 0.115, 0.060, 0.900 > ]
            [ 0.150 rgbf < 0.700, 0.232, 0.115, 0.900 > ]
            [ 0.200 rgbf < 0.350, 0.115, 0.060, 0.700 > ]
            [ 0.200 rgbf < 0.350, 0.115, 0.060, 0.700 > ]
            [ 1.000 rgbf < 0.350, 0.115, 0.060, 0.500 > ]
        }
    }
}


#declare T_Wood15 =
texture {
    pigment {
        bozo
        colour_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.400 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.500 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.980 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
        scale < 1.000, 1.000, 0.001 >
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.400 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.003, 0.002, 0.001 > ]
            [ 0.500 rgb < 0.006, 0.003, 0.001 > ]
            [ 0.008 rgb < 0.010, 0.006, 0.001 > ]
            [ 0.700 rgb < 0.025, 0.015, 0.004 > ]
            [ 0.044 rgb < 0.060, 0.036, 0.010 > ]
            [ 0.980 rgb < 0.148, 0.091, 0.024 > ]
            [ 0.280 rgb < 0.353, 0.217, 0.055 > ]
        }
    }
}
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgbf < 0.404, 0.210, 0.078, 0.200 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.800 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.800 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.200 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.200 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.600 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.600 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.400 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.400 > ]
            [ 1.000 rgbf < 0.404, 0.210, 0.078, 0.100 > ]
        }
        scale < 0.200, 0.200, 1.000 >
        turbulence < 0.040, 0.040, 0.040 >
        rotate < -2.000, 2.000, 0.000 >
        color_map {
            [ 0.000 rgbf < 0.404, 0.210, 0.078, 0.200 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.800 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.800 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.200 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.200 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.600 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.600 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.400 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.400 > ]
            [ 1.000 rgbf < 0.404, 0.210, 0.078, 0.100 > ]
        }
    }
}


#declare T_Wood16 =
texture {
    pigment {
        bozo
        colour_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.400 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.500 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.980 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
        scale < 1.000, 1.000, 0.001 >
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.400 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.003, 0.002, 0.001 > ]
            [ 0.500 rgb < 0.006, 0.003, 0.001 > ]
            [ 0.008 rgb < 0.010, 0.006, 0.001 > ]
            [ 0.700 rgb < 0.025, 0.015, 0.004 > ]
            [ 0.044 rgb < 0.060, 0.036, 0.010 > ]
            [ 0.980 rgb < 0.148, 0.091, 0.024 > ]
            [ 0.280 rgb < 0.353, 0.217, 0.055 > ]
        }
    }
}
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgbf < 0.404, 0.210, 0.078, 0.300 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.600 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.600 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.300 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.300 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.400 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.400 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.500 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.500 > ]
            [ 1.000 rgbf < 0.404, 0.210, 0.078, 0.300 > ]
        }
        scale < 0.075, 0.075, 1.000 >
        turbulence < 0.035, 0.035, 0.035 >
        rotate < -2.000, 2.000, 0.000 >
        color_map {
            [ 0.000 rgbf < 0.404, 0.210, 0.078, 0.300 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.600 > ]
            [ 0.250 rgbf < 0.431, 0.225, 0.090, 0.600 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.300 > ]
            [ 0.400 rgbf < 0.447, 0.233, 0.090, 0.300 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.400 > ]
            [ 0.500 rgbf < 0.404, 0.210, 0.075, 0.400 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.700 rgbf < 0.459, 0.222, 0.102, 0.200 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.500 > ]
            [ 0.980 rgbf < 0.431, 0.225, 0.086, 0.500 > ]
            [ 1.000 rgbf < 0.404, 0.210, 0.078, 0.300 > ]
        }
    }
}


#declare T_Wood17 =
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.400 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.500 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.980 rgb < 0.001, 0.000, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
        omega 0.001
        scale < 1.000, 1.000, 0.001 >
        rotate < 0.000, 0.002, 0.000 >
        color_map {
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.001 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.001 > ]
            [ 0.400 rgb < 0.002, 0.001, 0.001 > ]
            [ 0.001 rgb < 0.004, 0.002, 0.001 > ]
            [ 0.500 rgb < 0.011, 0.006, 0.002 > ]
            [ 0.011 rgb < 0.033, 0.023, 0.005 > ]
            [ 0.700 rgb < 0.083, 0.059, 0.013 > ]
            [ 0.127 rgb < 0.211, 0.136, 0.065 > ]
            [ 0.980 rgb < 0.351, 0.227, 0.108 > ]
            [ 0.540 rgb < 0.540, 0.360, 0.135 > ]
        }
    }
}
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgbf < 0.400, 0.200, 0.080, 0.100 > ]
            [ 0.250 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.250 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.400 rgbf < 0.450, 0.250, 0.090, 0.300 > ]
            [ 0.400 rgbf < 0.450, 0.250, 0.090, 0.300 > ]
            [ 0.500 rgbf < 0.400, 0.200, 0.080, 0.600 > ]
            [ 0.500 rgbf < 0.400, 0.200, 0.080, 0.600 > ]
            [ 0.700 rgbf < 0.460, 0.230, 0.100, 0.300 > ]
            [ 0.700 rgbf < 0.460, 0.230, 0.100, 0.300 > ]
            [ 0.980 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.980 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 1.000 rgbf < 0.400, 0.200, 0.080, 0.100 > ]
        }
        turbulence < 0.050, 0.050, 0.050 >
        omega 0.65
        scale < 0.200, 0.200, 1.000 >
        rotate < 0.000, 2.000, 0.000 >
        color_map {
            [ 0.000 rgbf < 0.400, 0.200, 0.080, 0.100 > ]
            [ 0.250 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.250 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.400 rgbf < 0.450, 0.250, 0.090, 0.300 > ]
            [ 0.400 rgbf < 0.450, 0.250, 0.090, 0.300 > ]
            [ 0.500 rgbf < 0.400, 0.200, 0.080, 0.600 > ]
            [ 0.500 rgbf < 0.400, 0.200, 0.080, 0.600 > ]
            [ 0.700 rgbf < 0.460, 0.230, 0.100, 0.300 > ]
            [ 0.700 rgbf < 0.460, 0.230, 0.100, 0.300 > ]
            [ 0.980 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 0.980 rgbf < 0.440, 0.230, 0.090, 0.200 > ]
            [ 1.000 rgbf < 0.400, 0.200, 0.080, 0.100 > ]
        }
    }
}


#declare T_Wood18 =
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.150 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.250 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.280 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.400 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.500 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.980 rgb < 0.001, 0.000, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
        octaves 0.001
        lambda 0.001
        scale < 1.000, 1.000, 1.000 >
        rotate < 0.001, 0.000, 0.000 >
        color_map {
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.000 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.150 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.250 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.001, 0.001, 0.000 > ]
            [ 0.280 rgb < 0.002, 0.001, 0.000 > ]
            [ 0.001 rgb < 0.007, 0.002, 0.000 > ]
            [ 0.400 rgb < 0.018, 0.007, 0.000 > ]
            [ 0.009 rgb < 0.044, 0.018, 0.000 > ]
            [ 0.500 rgb < 0.073, 0.036, 0.000 > ]
            [ 0.052 rgb < 0.123, 0.062, 0.000 > ]
            [ 0.700 rgb < 0.175, 0.079, 0.000 > ]
            [ 0.171 rgb < 0.250, 0.112, 0.000 > ]
            [ 0.980 rgb < 0.500, 0.225, 0.000 > ]
            [ 0.500 rgb < 1.000, 0.500, 0.000 > ]
        }
    }
}
texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgbf < 0.500, 0.260, 0.120, 0.300 > ]
            [ 0.250 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.250 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.400 rgbf < 0.550, 0.280, 0.100, 0.600 > ]
            [ 0.400 rgbf < 0.550, 0.280, 0.100, 0.600 > ]
            [ 0.500 rgbf < 0.500, 0.230, 0.150, 1.000 > ]
            [ 0.500 rgbf < 0.500, 0.230, 0.150, 1.000 > ]
            [ 0.700 rgbf < 0.560, 0.290, 0.170, 0.600 > ]
            [ 0.700 rgbf < 0.560, 0.290, 0.170, 0.600 > ]
            [ 0.980 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.980 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 1.000 rgbf < 0.500, 0.260, 0.120, 0.300 > ]
        }
        turbulence < 0.020, 0.020, 0.020 >
        octaves 6
        lambda 2.8
        omega 0.6
        scale < 0.050, 0.050, 0.050 >
        rotate < 2.000, 0.000, 0.000 >
        color_map {
            [ 0.000 rgbf < 0.500, 0.260, 0.120, 0.300 > ]
            [ 0.250 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.250 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.400 rgbf < 0.550, 0.280, 0.100, 0.600 > ]
            [ 0.400 rgbf < 0.550, 0.280, 0.100, 0.600 > ]
            [ 0.500 rgbf < 0.500, 0.230, 0.150, 1.000 > ]
            [ 0.500 rgbf < 0.500, 0.230, 0.150, 1.000 > ]
            [ 0.700 rgbf < 0.560, 0.290, 0.170, 0.600 > ]
            [ 0.700 rgbf < 0.560, 0.290, 0.170, 0.600 > ]
            [ 0.980 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 0.980 rgbf < 0.540, 0.290, 0.130, 0.400 > ]
            [ 1.000 rgbf < 0.500, 0.260, 0.120, 0.300 > ]
        }
    }
}


#declare T_Wood19 =
texture {
    pigment {
        wood
        color_map {
            [ 0.00 rgb < 0.500, 0.250, 0.125 > ]
            [ 0.40 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.60 rgb < 1.000, 0.500, 0.250 > ]
            [ 1.00 rgb < 0.500, 0.250, 0.125 > ]
        }
        color_map {
            [ 0.00 rgb < 0.500, 0.250, 0.125 > ]
            [ 0.40 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.60 rgb < 1.000, 0.500, 0.250 > ]
            [ 1.00 rgb < 0.500, 0.250, 0.125 > ]
        }
        rotate < 4.000, 0.000, 0.000 >
        omega 0.45
        turbulence < 0.065, 0.065, 0.065 >
        scale < 0.075, 0.075, 1.000 >
        lambda 2.3
    }
}
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgb < 0.350, 0.175, 0.087 > ]
            [ 0.300 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        color_map {
            [ 0.000 rgb < 0.350, 0.175, 0.087 > ]
            [ 0.300 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        scale < 0.013, 0.013, 0.750 >
    }
}


#declare T_Wood20 =
texture {
    pigment {
        wood
        color_map {
            [ 0.00 rgb < 0.500, 0.250, 0.125 > ]
            [ 0.40 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.60 rgb < 1.000, 0.500, 0.250 > ]
            [ 1.00 rgb < 0.500, 0.250, 0.125 > ]
        }
        color_map {
            [ 0.00 rgb < 0.500, 0.250, 0.125 > ]
            [ 0.40 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.60 rgb < 1.000, 0.500, 0.250 > ]
            [ 1.00 rgb < 0.500, 0.250, 0.125 > ]
        }
        rotate < 4.000, 0.000, 0.000 >
        omega 0.45
        turbulence < 0.065, 0.065, 0.065 >
        scale < 0.075, 0.075, 1.000 >
        lambda 2.3
    }
}
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgb < 0.350, 0.175, 0.087 > ]
            [ 0.300 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        color_map {
            [ 0.000 rgb < 0.350, 0.175, 0.087 > ]
            [ 0.300 rgb < 1.000, 0.500, 0.250 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        scale < 0.013, 0.013, 0.750 >
    }
}


// ----- STONES1001-------------


#declare Grnt17a = texture {
    pigment
    {
     granite
     turbulence < 0.600, 0.600, 0.600 >
           color_map
        {
            [ 0.000 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 1.001 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
        }
    }
}


#declare Grnt3a = texture {
    pigment
    {
     granite
    turbulence < 0.600, 0.600, 0.600 >
             color_map
      {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
        }
    }
}


#declare T_Crack1 = texture {
    pigment
    {
      marble
    turbulence < 0.850, 0.850, 0.850 >
             color_map
     {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
    }
}


#declare T_Crack2 = texture {
    pigment
    {
     granite
    turbulence < 0.800, 0.800, 0.800 >
              color_map
     {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.500 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.500 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.540 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.540 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
    }
}


#declare T_Crack3 = texture {
    pigment
    {
      marble
    turbulence < 0.850, 0.850, 0.850 >
             color_map
     {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.184, 0.310, 0.184, 0.800 > ]
            [ 0.970 rgbf < 0.184, 0.310, 0.184, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
    }
}


#declare T_Crack4 = texture {
    pigment
    {
      marble
    turbulence < 0.850, 0.850, 0.850 >
            color_map
      {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 0.300 > ]
            [ 0.030 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.030 rgbf < 0.000, 0.000, 0.000, 0.700 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 1.000, 1.000, 1.000, 0.900 > ]
            [ 1.001 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
    }
}


#declare T_Grnt0 = texture {
    pigment
    {
       granite
    turbulence < 0.400, 0.400, 0.400 >
             color_map
    {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.000 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.000 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.000 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.000 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.000 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.000 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.000 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.000 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.000 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.000 > ]
        }
    }
}


#declare T_Grnt0a = texture {
    pigment
    {
      granite
    turbulence < 0.600, 0.600, 0.600 >
             color_map
     {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
        }
    }
}


#declare T_Grnt1 = texture {
    pigment
    {
       granite
    turbulence < 0.600, 0.600, 0.600 >
             color_map
    {
            [ 0.000 rgbf < 0.898, 0.898, 0.851, 0.000 > ]
            [ 0.212 rgbf < 0.969, 0.980, 0.875, 0.000 > ]
            [ 0.212 rgbf < 0.969, 0.980, 0.875, 0.000 > ]
            [ 0.424 rgbf < 0.859, 0.859, 0.859, 0.000 > ]
            [ 0.424 rgbf < 0.859, 0.859, 0.859, 0.000 > ]
            [ 0.627 rgbf < 0.992, 0.922, 0.659, 0.000 > ]
            [ 0.627 rgbf < 0.992, 0.922, 0.659, 0.000 > ]
            [ 0.881 rgbf < 0.937, 0.965, 0.902, 0.000 > ]
            [ 0.881 rgbf < 0.937, 0.965, 0.902, 0.000 > ]
            [ 1.001 rgbf < 0.898, 0.898, 0.851, 0.000 > ]
        }
    }
}


#declare T_Grnt10 = texture {
    pigment
    {
        granite
    turbulence < 0.600, 0.600, 0.600 >
              color_map
  {
            [ 0.000 rgbf < 0.890, 0.690, 0.690, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.000 > ]
            [ 0.308 rgbf < 0.745, 0.635, 0.651, 0.004 > ]
            [ 0.308 rgbf < 0.745, 0.635, 0.651, 0.004 > ]
            [ 0.444 rgbf < 0.733, 0.596, 0.557, 0.004 > ]
            [ 0.444 rgbf < 0.733, 0.596, 0.557, 0.004 > ]
            [ 0.615 rgbf < 0.996, 0.835, 0.737, 0.000 > ]
            [ 0.615 rgbf < 0.996, 0.835, 0.737, 0.000 > ]
            [ 0.803 rgbf < 0.765, 0.616, 0.659, 0.000 > ]
            [ 0.803 rgbf < 0.765, 0.616, 0.659, 0.000 > ]
            [ 1.001 rgbf < 0.890, 0.690, 0.690, 0.000 > ]
        }
    }
}


#declare T_Grnt10a = texture {
    pigment
    {
      granite
    turbulence < 0.600, 0.600, 0.600 >
              color_map
    {
            [ 0.000 rgbf < 0.843, 0.655, 0.655, 0.455 > ]
            [ 0.179 rgbf < 0.886, 0.769, 0.627, 0.608 > ]
            [ 0.179 rgbf < 0.886, 0.769, 0.627, 0.608 > ]
            [ 0.368 rgbf < 0.906, 0.820, 0.714, 0.392 > ]
            [ 0.368 rgbf < 0.906, 0.820, 0.714, 0.392 > ]
            [ 0.538 rgbf < 0.851, 0.671, 0.671, 0.659 > ]
            [ 0.538 rgbf < 0.851, 0.671, 0.671, 0.659 > ]
            [ 0.744 rgbf < 0.890, 0.792, 0.675, 0.392 > ]
            [ 0.744 rgbf < 0.890, 0.792, 0.675, 0.392 > ]
            [ 0.983 rgbf < 0.827, 0.612, 0.612, 0.706 > ]
            [ 0.983 rgbf < 0.827, 0.612, 0.612, 0.706 > ]
            [ 1.001 rgbf < 0.843, 0.655, 0.655, 0.455 > ]
        }
    }
}


#declare T_Grnt11 = texture {
    pigment
    {
      granite
    turbulence < 0.600, 0.600, 0.600 >
             color_map
     {
            [ 0.000 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.000 > ]
            [ 0.308 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
            [ 0.308 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
            [ 0.444 rgbf < 0.694, 0.624, 0.604, 0.004 > ]
            [ 0.444 rgbf < 0.694, 0.624, 0.604, 0.004 > ]
            [ 0.615 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
            [ 0.615 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
            [ 0.812 rgbf < 0.725, 0.655, 0.651, 0.000 > ]
            [ 0.812 rgbf < 0.725, 0.655, 0.651, 0.000 > ]
            [ 1.001 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
        }
    }
}


#declare T_Grnt11a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.894, 0.886, 0.886, 0.659 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.584 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.584 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.780 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.780 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.686 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.686 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.424 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.424 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.761 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.761 > ]
            [ 1.001 rgbf < 0.894, 0.886, 0.886, 0.659 > ]
        }
    }
}


#declare T_Grnt12 = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {

            [ 0.000 rgbf < 0.996, 0.969, 0.800, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.682, 0.604, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.682, 0.604, 0.000 > ]
            [ 0.308 rgbf < 0.906, 0.820, 0.714, 0.000 > ]
            [ 0.308 rgbf < 0.906, 0.820, 0.714, 0.000 > ]
            [ 0.444 rgbf < 0.816, 0.631, 0.537, 0.000 > ]
            [ 0.444 rgbf < 0.816, 0.631, 0.537, 0.000 > ]
            [ 0.615 rgbf < 0.890, 0.792, 0.675, 0.000 > ]
            [ 0.615 rgbf < 0.890, 0.792, 0.675, 0.000 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.000 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.000 > ]
            [ 1.001 rgbf < 0.996, 0.969, 0.800, 0.000 > ]
        }
    }
}


#declare T_Grnt12a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {

            [ 0.000 rgbf < 0.890, 0.690, 0.690, 0.659 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.308 rgbf < 0.745, 0.635, 0.651, 0.780 > ]
            [ 0.308 rgbf < 0.745, 0.635, 0.651, 0.780 > ]
            [ 0.444 rgbf < 0.733, 0.596, 0.557, 0.686 > ]
            [ 0.444 rgbf < 0.733, 0.596, 0.557, 0.686 > ]
            [ 0.615 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.615 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.803 rgbf < 0.765, 0.616, 0.659, 0.761 > ]
            [ 0.803 rgbf < 0.765, 0.616, 0.659, 0.761 > ]
            [ 1.001 rgbf < 0.890, 0.690, 0.690, 0.659 > ]
        }
    }
}


#declare T_Grnt13 = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.996, 0.824, 0.780, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.698, 0.624, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.698, 0.624, 0.000 > ]
            [ 0.308 rgbf < 0.906, 0.675, 0.553, 0.000 > ]
            [ 0.308 rgbf < 0.906, 0.675, 0.553, 0.000 > ]
            [ 0.444 rgbf < 0.996, 0.682, 0.604, 0.000 > ]
            [ 0.444 rgbf < 0.996, 0.682, 0.604, 0.000 > ]
            [ 0.615 rgbf < 0.996, 0.824, 0.780, 0.000 > ]
            [ 0.615 rgbf < 0.996, 0.824, 0.780, 0.000 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.000 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.000 > ]
            [ 1.001 rgbf < 0.996, 0.824, 0.780, 0.000 > ]
        }
    }
}


#declare T_Grnt13a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.608 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.608 > ]
            [ 0.308 rgbf < 0.800, 0.651, 0.557, 0.635 > ]
            [ 0.308 rgbf < 0.800, 0.651, 0.557, 0.635 > ]
            [ 0.444 rgbf < 0.694, 0.624, 0.604, 0.294 > ]
            [ 0.444 rgbf < 0.694, 0.624, 0.604, 0.294 > ]
            [ 0.615 rgbf < 0.800, 0.651, 0.557, 0.816 > ]
            [ 0.615 rgbf < 0.800, 0.651, 0.557, 0.816 > ]
            [ 0.812 rgbf < 0.725, 0.655, 0.651, 0.957 > ]
            [ 0.812 rgbf < 0.725, 0.655, 0.651, 0.957 > ]
            [ 1.001 rgbf < 0.800, 0.651, 0.557, 0.000 > ]
        }
    }
}


#declare T_Grnt14 = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.690, 0.612, 0.569, 0.000 > ]
            [ 0.154 rgbf < 0.737, 0.596, 0.522, 0.000 > ]
            [ 0.154 rgbf < 0.737, 0.596, 0.522, 0.000 > ]
            [ 0.368 rgbf < 0.776, 0.702, 0.624, 0.000 > ]
            [ 0.368 rgbf < 0.776, 0.702, 0.624, 0.000 > ]
            [ 0.538 rgbf < 0.796, 0.678, 0.643, 0.000 > ]
            [ 0.538 rgbf < 0.796, 0.678, 0.643, 0.000 > ]
            [ 0.846 rgbf < 0.690, 0.612, 0.569, 0.000 > ]
            [ 0.846 rgbf < 0.690, 0.612, 0.569, 0.000 > ]
            [ 0.932 rgbf < 0.773, 0.612, 0.569, 0.000 > ]
            [ 0.932 rgbf < 0.773, 0.612, 0.569, 0.000 > ]
            [ 1.001 rgbf < 0.690, 0.612, 0.569, 0.000 > ]
        }
    }
}


#declare T_Grnt14a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.996, 0.969, 0.800, 0.373 > ]
            [ 0.154 rgbf < 0.996, 0.682, 0.604, 0.412 > ]
            [ 0.154 rgbf < 0.996, 0.682, 0.604, 0.412 > ]
            [ 0.308 rgbf < 0.906, 0.820, 0.714, 0.616 > ]
            [ 0.308 rgbf < 0.906, 0.820, 0.714, 0.616 > ]
            [ 0.444 rgbf < 0.816, 0.631, 0.537, 0.443 > ]
            [ 0.444 rgbf < 0.816, 0.631, 0.537, 0.443 > ]
            [ 0.615 rgbf < 0.890, 0.792, 0.675, 0.745 > ]
            [ 0.615 rgbf < 0.890, 0.792, 0.675, 0.745 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.600 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.600 > ]
            [ 1.001 rgbf < 0.996, 0.969, 0.800, 0.373 > ]
        }
    }
}


#declare T_Grnt15 = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.161, 0.133, 0.118, 0.000 > ]
            [ 0.104 rgbf < 0.110, 0.082, 0.071, 0.000 > ]
            [ 0.104 rgbf < 0.110, 0.082, 0.071, 0.000 > ]
            [ 0.252 rgbf < 0.161, 0.133, 0.118, 0.000 > ]
            [ 0.252 rgbf < 0.161, 0.133, 0.118, 0.000 > ]
            [ 0.383 rgbf < 0.000, 0.000, 0.000, 0.000 > ]
            [ 0.383 rgbf < 0.000, 0.000, 0.000, 0.000 > ]
            [ 0.643 rgbf < 0.161, 0.133, 0.118, 0.000 > ]
            [ 0.643 rgbf < 0.161, 0.133, 0.118, 0.000 > ]
            [ 0.783 rgbf < 0.220, 0.149, 0.137, 0.000 > ]
            [ 0.783 rgbf < 0.220, 0.149, 0.137, 0.000 > ]
            [ 0.922 rgbf < 0.000, 0.000, 0.000, 0.000 > ]
            [ 0.922 rgbf < 0.000, 0.000, 0.000, 0.000 > ]
            [ 0.983 rgbf < 0.220, 0.149, 0.137, 0.000 > ]
            [ 0.983 rgbf < 0.220, 0.149, 0.137, 0.000 > ]
            [ 1.001 rgbf < 0.161, 0.133, 0.118, 0.000 > ]
        }
    }
}


#declare T_Grnt15a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
            [ 0.154 rgbf < 0.996, 0.698, 0.624, 0.412 > ]
            [ 0.154 rgbf < 0.996, 0.698, 0.624, 0.412 > ]
            [ 0.308 rgbf < 0.906, 0.675, 0.553, 0.616 > ]
            [ 0.308 rgbf < 0.906, 0.675, 0.553, 0.616 > ]
            [ 0.444 rgbf < 0.996, 0.682, 0.604, 0.412 > ]
            [ 0.444 rgbf < 0.996, 0.682, 0.604, 0.412 > ]
            [ 0.615 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
            [ 0.615 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.600 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.600 > ]
            [ 1.001 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
        }
    }
}


#declare T_Grnt16 = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.910, 0.788, 0.788, 0.000 > ]
            [ 0.316 rgbf < 0.922, 0.914, 0.871, 0.000 > ]
            [ 0.316 rgbf < 0.922, 0.914, 0.871, 0.000 > ]
            [ 0.453 rgbf < 0.894, 0.867, 0.780, 0.000 > ]
            [ 0.453 rgbf < 0.894, 0.867, 0.780, 0.000 > ]
            [ 0.624 rgbf < 0.784, 0.788, 0.788, 0.000 > ]
            [ 0.624 rgbf < 0.784, 0.788, 0.788, 0.000 > ]
            [ 0.726 rgbf < 0.851, 0.812, 0.741, 0.000 > ]
            [ 0.726 rgbf < 0.851, 0.812, 0.741, 0.000 > ]
            [ 0.863 rgbf < 0.647, 0.655, 0.655, 0.000 > ]
            [ 0.863 rgbf < 0.647, 0.655, 0.655, 0.000 > ]
            [ 1.001 rgbf < 0.910, 0.788, 0.788, 0.000 > ]
        }
    }
}


#declare T_Grnt16a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.769, 0.722, 0.690, 0.180 > ]
            [ 0.078 rgbf < 0.745, 0.690, 0.655, 1.000 > ]
            [ 0.078 rgbf < 0.745, 0.690, 0.655, 1.000 > ]
            [ 0.960 rgbf < 0.839, 0.804, 0.780, 1.000 > ]
            [ 0.960 rgbf < 0.839, 0.804, 0.780, 0.278 > ]
            [ 1.001 rgbf < 0.769, 0.722, 0.690, 0.180 > ]
        }
    }
}


#declare T_Grnt17 = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.000, 0.239, 0.000, 0.000 > ]
            [ 0.303 rgbf < 0.333, 0.294, 0.000, 0.000 > ]
            [ 0.303 rgbf < 0.333, 0.294, 0.000, 0.000 > ]
            [ 0.588 rgbf < 0.000, 0.239, 0.341, 0.000 > ]
            [ 0.588 rgbf < 0.000, 0.239, 0.341, 0.000 > ]
            [ 0.790 rgbf < 0.000, 0.020, 0.000, 0.000 > ]
            [ 0.790 rgbf < 0.000, 0.020, 0.000, 0.000 > ]
            [ 1.001 rgbf < 0.000, 0.239, 0.000, 0.000 > ]
        }
    }
}


#declare T_Grnt17a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 1.001 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
        }
    }
}


#declare T_Grnt18 = texture {
    pigment
    {
    granite
    turbulence < 0.400, 0.400, 0.400 >
            color_map
        {
            [ 0.000 rgbf < 1.000, 0.718, 0.541, 0.000 > ]
            [ 0.202 rgbf < 0.890, 0.651, 0.612, 0.000 > ]
            [ 0.202 rgbf < 0.890, 0.651, 0.612, 0.000 > ]
            [ 0.298 rgbf < 1.000, 0.820, 0.675, 0.000 > ]
            [ 0.298 rgbf < 1.000, 0.820, 0.675, 0.000 > ]
            [ 0.377 rgbf < 0.890, 0.643, 0.612, 0.000 > ]
            [ 0.377 rgbf < 0.890, 0.643, 0.612, 0.000 > ]
            [ 0.465 rgbf < 0.937, 0.729, 0.561, 0.000 > ]
            [ 0.465 rgbf < 0.937, 0.729, 0.561, 0.000 > ]
            [ 0.544 rgbf < 0.878, 0.604, 0.565, 0.000 > ]
            [ 0.544 rgbf < 0.878, 0.604, 0.565, 0.000 > ]
            [ 0.640 rgbf < 0.984, 0.780, 0.655, 0.000 > ]
            [ 0.640 rgbf < 0.984, 0.780, 0.655, 0.000 > ]
            [ 0.860 rgbf < 1.000, 0.863, 0.635, 0.000 > ]
            [ 0.860 rgbf < 1.000, 0.863, 0.635, 0.000 > ]
            [ 0.982 rgbf < 1.000, 0.765, 0.620, 0.000 > ]
            [ 0.982 rgbf < 1.000, 0.765, 0.620, 0.000 > ]
            [ 1.001 rgbf < 1.000, 0.718, 0.541, 0.000 > ]
        }
    }
}


#declare T_Grnt18a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.820, 0.580, 0.580, 0.000 > ]
            [ 0.128 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.128 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.282 rgbf < 0.792, 0.694, 0.690, 0.282 > ]
            [ 0.282 rgbf < 0.792, 0.694, 0.690, 0.282 > ]
            [ 0.393 rgbf < 0.647, 0.655, 0.655, 0.133 > ]
            [ 0.393 rgbf < 0.647, 0.655, 0.655, 0.133 > ]
            [ 0.590 rgbf < 0.851, 0.812, 0.741, 0.333 > ]
            [ 0.590 rgbf < 0.851, 0.812, 0.741, 0.333 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 1.001 rgbf < 0.820, 0.580, 0.580, 0.000 > ]
        }
    }
}


#declare T_Grnt19 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.300 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.600 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.600 rgb < 0.329, 0.329, 0.329 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >

    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt19a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.300 rgbf < 0.329, 0.329, 0.329, 0.500 > ]
            [ 0.300 rgbf < 0.329, 0.329, 0.329, 0.500 > ]
            [ 0.400 rgbf < 0.329, 0.329, 0.329, 0.800 > ]
            [ 0.400 rgbf < 0.329, 0.329, 0.329, 0.800 > ]
            [ 1.000 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
        }
    turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt1a = texture {
    pigment
    {
    granite
    turbulence < 0.600, 0.600, 0.600 >
            color_map
        {
            [ 0.000 rgbf < 0.898, 0.898, 0.851, 0.306 > ]
            [ 0.212 rgbf < 0.969, 0.980, 0.875, 0.792 > ]
            [ 0.212 rgbf < 0.969, 0.980, 0.875, 0.792 > ]
            [ 0.424 rgbf < 0.859, 0.859, 0.859, 0.396 > ]
            [ 0.424 rgbf < 0.859, 0.859, 0.859, 0.396 > ]
            [ 0.627 rgbf < 0.992, 0.922, 0.659, 0.976 > ]
            [ 0.627 rgbf < 0.992, 0.922, 0.659, 0.976 > ]
            [ 0.881 rgbf < 0.937, 0.965, 0.902, 0.820 > ]
            [ 0.881 rgbf < 0.937, 0.965, 0.902, 0.820 > ]
            [ 1.001 rgbf < 0.898, 0.898, 0.851, 0.306 > ]
        }
    }
}


#declare T_Grnt2 = texture {
    pigment
    {
    granite
    turbulence < 0.500, 0.500, 0.500 >
            color_map
        {
            [ 0.000 rgbf < 0.863, 0.757, 0.596, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.792, 0.714, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.792, 0.714, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.525 rgbf < 0.992, 0.922, 0.659, 0.000 > ]
            [ 0.525 rgbf < 0.992, 0.922, 0.659, 0.000 > ]
            [ 0.729 rgbf < 0.902, 0.812, 0.714, 0.000 > ]
            [ 0.729 rgbf < 0.902, 0.812, 0.714, 0.000 > ]
            [ 1.001 rgbf < 0.863, 0.757, 0.596, 0.000 > ]
        }
    }
}


#declare T_Grnt20 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.400 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.400 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.600 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.600 rgb < 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 1.000, 1.000, 1.000 > ]
        }
    turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt20a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
    turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt21 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 0.300 > ]
            [ 0.100 rgbf < 0.137, 0.557, 0.420, 0.400 > ]
            [ 0.100 rgbf < 0.137, 0.557, 0.420, 0.400 > ]
            [ 0.300 rgbf < 0.270, 0.340, 0.260, 0.700 > ]
            [ 0.300 rgbf < 0.270, 0.340, 0.260, 0.700 > ]
            [ 0.500 rgbf < 0.184, 0.310, 0.184, 0.700 > ]
            [ 0.500 rgbf < 0.184, 0.310, 0.184, 0.700 > ]
            [ 0.700 rgbf < 0.030, 0.180, 0.080, 0.700 > ]
            [ 0.700 rgbf < 0.030, 0.180, 0.080, 0.700 > ]
            [ 0.800 rgbf < 0.184, 0.310, 0.184, 0.700 > ]
            [ 0.800 rgbf < 0.184, 0.310, 0.184, 0.700 > ]
            [ 1.000 rgbf < 0.184, 0.310, 0.184, 0.700 > ]
        }
    turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt21a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 0.400 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.800 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.800 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
    turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt22 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.200 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.200 rgbf < 0.329, 0.329, 0.329, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000 , 1.000 , 1.000 , 1.000 > ]
            [ 0.700 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]
        }
    turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt22a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 1.000, 0.718, 0.541, 0.890 > ]
            [ 0.175 rgbf < 0.843, 0.678, 0.655, 0.753 > ]
            [ 0.175 rgbf < 0.843, 0.678, 0.655, 0.753 > ]
            [ 0.228 rgbf < 0.906, 0.831, 0.773, 0.980 > ]
            [ 0.228 rgbf < 0.906, 0.831, 0.773, 0.698 > ]
            [ 0.386 rgbf < 0.992, 0.718, 0.545, 0.794 > ]
            [ 0.386 rgbf < 0.992, 0.718, 0.545, 0.794 > ]
            [ 0.412 rgbf < 0.333, 0.188, 0.067, 0.784 > ]
            [ 0.412 rgbf < 0.333, 0.188, 0.067, 0.784 > ]
            [ 0.439 rgbf < 0.925, 0.557, 0.514, 0.778 > ]
            [ 0.439 rgbf < 0.925, 0.557, 0.514, 0.678 > ]
            [ 0.684 rgbf < 0.984, 0.780, 0.655, 0.696 > ]
            [ 0.684 rgbf < 0.984, 0.780, 0.655, 0.696 > ]
            [ 0.781 rgbf < 0.965, 0.847, 0.675, 0.880 > ]
            [ 0.781 rgbf < 0.965, 0.847, 0.675, 0.880 > ]
            [ 0.982 rgbf < 1.000, 0.718, 0.541, 0.990 > ]
            [ 0.982 rgbf < 1.000, 0.718, 0.541, 0.890 > ]
            [ 1.001 rgbf < 1.000, 0.718, 0.541, 0.890 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt23 = texture {
    pigment
    {
    marble
            color_map
        {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.831, 0.714, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.831, 0.714, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.525 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.525 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.729 rgbf < 0.937, 0.882, 0.820, 0.000 > ]
            [ 0.729 rgbf < 0.937, 0.882, 0.820, 0.000 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt23a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.824, 0.725, 0.584, 0.600 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.600 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.600 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.700 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.700 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.600 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.600 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.500 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.500 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.700 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.700 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.500 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.500 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.600 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.600 > ]
            [ 1.001 rgbf < 0.824, 0.725, 0.584, 0.700 > ]
        }
    }
}


#declare T_Grnt24 = texture {
    pigment
    {
    marble
            color_map
        {
            [ 0.000 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 1.001 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt24a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.500 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.500 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.500 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.500 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.500 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.500 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.500 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.500 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 1.001 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt25 = texture {
    pigment
    {
    marble
            color_map
        {
            [ 0.000 rgbf < 0.824, 0.725, 0.584, 0.000 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.000 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.000 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.000 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.000 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.000 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.000 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.000 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.000 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.000 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.000 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 1.001 rgbf < 0.824, 0.725, 0.584, 0.000 > ]
        }
    turbulence < 0.700, 0.700, 0.700 >
    }
}


#declare T_Grnt26 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.241 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.241 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.284 rgbf < 0.600, 0.741, 0.608, 0.000 > ]
            [ 0.284 rgbf < 0.600, 0.741, 0.608, 0.000 > ]
            [ 0.336 rgbf < 0.820, 0.643, 0.537, 0.000 > ]
            [ 0.336 rgbf < 0.820, 0.643, 0.537, 0.000 > ]
            [ 0.474 rgbf < 0.886, 0.780, 0.714, 0.000 > ]
            [ 0.474 rgbf < 0.886, 0.780, 0.714, 0.000 > ]
            [ 0.810 rgbf < 0.996, 0.643, 0.537, 0.000 > ]
            [ 0.810 rgbf < 0.996, 0.643, 0.537, 0.000 > ]
            [ 0.836 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.836 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 1.001 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
        }
    }
    finish {
        crand 0.02
    }
}


#declare T_Grnt27 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.773, 0.647, 0.569, 0.000 > ]
            [ 0.043 rgbf < 0.431, 0.322, 0.227, 0.000 > ]
            [ 0.043 rgbf < 0.431, 0.322, 0.227, 0.000 > ]
            [ 0.113 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.113 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.304 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.304 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.426 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.426 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.843 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.843 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.878 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.878 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.983 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.983 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 1.001 rgbf < 0.773, 0.647, 0.569, 0.000 > ]
        }
    }
}


#declare T_Grnt28 = texture {
    pigment
    {
     marble
            color_map
       {
            [ 0.000 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.155 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.155 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.328 rgbf < 0.494, 0.243, 0.294, 0.000 > ]
            [ 0.328 rgbf < 0.494, 0.243, 0.294, 0.000 > ]
            [ 0.474 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.474 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.647 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.647 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.810 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.810 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.922 rgbf < 0.792, 0.388, 0.427, 0.000 > ]
            [ 0.922 rgbf < 0.792, 0.388, 0.427, 0.000 > ]
            [ 1.001 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
        }
    turbulence < 0.700, 0.700, 0.700 >
    }
    finish {
        crand 0.03
    }
}


#declare T_Grnt29 = texture {
    pigment
    {
    marble
            color_map
        {
            [ 0.000 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.000 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.000 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.000 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.000 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.000 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.000 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.000 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.000 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 1.001 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt2a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.863, 0.757, 0.596, 0.596 > ]
            [ 0.144 rgbf < 0.925, 0.792, 0.714, 0.349 > ]
            [ 0.144 rgbf < 0.925, 0.792, 0.714, 0.349 > ]
            [ 0.288 rgbf < 0.871, 0.702, 0.659, 0.784 > ]
            [ 0.288 rgbf < 0.871, 0.702, 0.659, 0.784 > ]
            [ 0.644 rgbf < 0.992, 0.922, 0.659, 0.498 > ]
            [ 0.644 rgbf < 0.992, 0.922, 0.659, 0.498 > ]
            [ 0.983 rgbf < 0.902, 0.812, 0.714, 0.722 > ]
            [ 0.983 rgbf < 0.902, 0.812, 0.714, 0.722 > ]
            [ 1.001 rgbf < 0.863, 0.757, 0.596, 0.596 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt3 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.831, 0.714, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.831, 0.714, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.525 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.525 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.729 rgbf < 0.937, 0.882, 0.820, 0.000 > ]
            [ 0.729 rgbf < 0.937, 0.882, 0.820, 0.000 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt3a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt4 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.804, 0.569, 0.494, 0.000 > ]
            [ 0.178 rgbf < 0.816, 0.725, 0.537, 0.000 > ]
            [ 0.178 rgbf < 0.816, 0.725, 0.537, 0.000 > ]
            [ 0.356 rgbf < 0.820, 0.580, 0.522, 0.000 > ]
            [ 0.356 rgbf < 0.820, 0.580, 0.522, 0.000 > ]
            [ 0.525 rgbf < 0.882, 0.725, 0.537, 0.000 > ]
            [ 0.525 rgbf < 0.882, 0.725, 0.537, 0.000 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 1.001 rgbf < 0.804, 0.569, 0.494, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt4a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.804, 0.569, 0.494, 0.569 > ]
            [ 0.144 rgbf < 0.816, 0.725, 0.537, 0.467 > ]
            [ 0.144 rgbf < 0.816, 0.725, 0.537, 0.467 > ]
            [ 0.449 rgbf < 0.820, 0.580, 0.522, 0.584 > ]
            [ 0.449 rgbf < 0.820, 0.580, 0.522, 0.584 > ]
            [ 0.568 rgbf < 0.882, 0.725, 0.537, 0.871 > ]
            [ 0.568 rgbf < 0.882, 0.725, 0.537, 0.871 > ]
            [ 0.754 rgbf < 0.855, 0.729, 0.584, 0.816 > ]
            [ 0.754 rgbf < 0.855, 0.729, 0.584, 0.816 > ]
            [ 1.001 rgbf < 0.804, 0.569, 0.494, 0.569 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt5 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.804, 0.569, 0.494, 0.000 > ]
            [ 0.178 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.178 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.356 rgbf < 0.667, 0.502, 0.478, 0.000 > ]
            [ 0.356 rgbf < 0.667, 0.502, 0.478, 0.000 > ]
            [ 0.525 rgbf < 0.859, 0.624, 0.545, 0.000 > ]
            [ 0.525 rgbf < 0.859, 0.624, 0.545, 0.000 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 1.001 rgbf < 0.804, 0.569, 0.494, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt5a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.804, 0.569, 0.494, 0.569 > ]
            [ 0.178 rgbf < 0.855, 0.729, 0.584, 0.467 > ]
            [ 0.178 rgbf < 0.855, 0.729, 0.584, 0.467 > ]
            [ 0.356 rgbf < 0.667, 0.502, 0.478, 0.584 > ]
            [ 0.356 rgbf < 0.667, 0.502, 0.478, 0.584 > ]
            [ 0.525 rgbf < 0.859, 0.624, 0.545, 0.871 > ]
            [ 0.525 rgbf < 0.859, 0.624, 0.545, 0.871 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.816 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.816 > ]
            [ 1.001 rgbf < 0.804, 0.569, 0.494, 0.569 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt6 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.545, 0.380, 0.345, 0.000 > ]
            [ 0.153 rgbf < 0.588, 0.475, 0.333, 0.000 > ]
            [ 0.153 rgbf < 0.588, 0.475, 0.333, 0.000 > ]
            [ 0.398 rgbf < 0.675, 0.478, 0.404, 0.000 > ]
            [ 0.398 rgbf < 0.675, 0.478, 0.404, 0.000 > ]
            [ 0.559 rgbf < 0.757, 0.635, 0.522, 0.000 > ]
            [ 0.559 rgbf < 0.757, 0.635, 0.522, 0.000 > ]
            [ 0.729 rgbf < 0.659, 0.549, 0.443, 0.000 > ]
            [ 0.729 rgbf < 0.659, 0.549, 0.443, 0.000 > ]
            [ 1.001 rgbf < 0.545, 0.380, 0.345, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt6a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.545, 0.380, 0.345, 0.733 > ]
            [ 0.263 rgbf < 0.588, 0.475, 0.333, 0.741 > ]
            [ 0.263 rgbf < 0.588, 0.475, 0.333, 0.741 > ]
            [ 0.432 rgbf < 0.675, 0.478, 0.404, 0.545 > ]
            [ 0.432 rgbf < 0.675, 0.478, 0.404, 0.545 > ]
            [ 0.551 rgbf < 0.757, 0.635, 0.522, 0.384 > ]
            [ 0.551 rgbf < 0.757, 0.635, 0.522, 0.384 > ]
            [ 0.720 rgbf < 0.659, 0.549, 0.443, 0.675 > ]
            [ 0.720 rgbf < 0.659, 0.549, 0.443, 0.675 > ]
            [ 1.001 rgbf < 0.545, 0.380, 0.345, 0.733 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt7 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.439, 0.310, 0.282, 0.000 > ]
            [ 0.153 rgbf < 0.463, 0.369, 0.259, 0.000 > ]
            [ 0.153 rgbf < 0.463, 0.369, 0.259, 0.000 > ]
            [ 0.398 rgbf < 0.541, 0.369, 0.298, 0.000 > ]
            [ 0.398 rgbf < 0.541, 0.369, 0.298, 0.000 > ]
            [ 0.559 rgbf < 0.573, 0.424, 0.286, 0.000 > ]
            [ 0.559 rgbf < 0.573, 0.424, 0.286, 0.000 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.000 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.000 > ]
            [ 1.001 rgbf < 0.439, 0.310, 0.282, 0.000 > ]
        }
    turbulence < 0.500, 0.500, 0.500 >
    }
}


#declare T_Grnt7a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.439, 0.310, 0.282, 0.631 > ]
            [ 0.119 rgbf < 0.463, 0.369, 0.259, 0.847 > ]
            [ 0.119 rgbf < 0.463, 0.369, 0.259, 0.847 > ]
            [ 0.322 rgbf < 0.541, 0.369, 0.298, 0.549 > ]
            [ 0.322 rgbf < 0.541, 0.369, 0.298, 0.549 > ]
            [ 0.449 rgbf < 0.573, 0.424, 0.286, 0.965 > ]
            [ 0.449 rgbf < 0.573, 0.424, 0.286, 0.965 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.741 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.741 > ]
            [ 1.001 rgbf < 0.439, 0.310, 0.282, 0.631 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt8 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.843, 0.655, 0.655, 0.000 > ]
            [ 0.179 rgbf < 0.886, 0.769, 0.627, 0.000 > ]
            [ 0.179 rgbf < 0.886, 0.769, 0.627, 0.000 > ]
            [ 0.368 rgbf < 0.906, 0.820, 0.714, 0.000 > ]
            [ 0.368 rgbf < 0.906, 0.820, 0.714, 0.000 > ]
            [ 0.538 rgbf < 0.851, 0.671, 0.671, 0.000 > ]
            [ 0.538 rgbf < 0.851, 0.671, 0.671, 0.000 > ]
            [ 0.846 rgbf < 0.890, 0.792, 0.675, 0.000 > ]
            [ 0.846 rgbf < 0.890, 0.792, 0.675, 0.000 > ]
            [ 0.983 rgbf < 0.827, 0.612, 0.612, 0.000 > ]
            [ 0.983 rgbf < 0.827, 0.612, 0.612, 0.000 > ]
            [ 1.001 rgbf < 0.843, 0.655, 0.655, 0.000 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt8a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 1.001 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt9 = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 1.001 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare T_Grnt9a = texture {
    pigment
    {
    granite
            color_map
        {
            [ 0.000 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 0.216 rgbf < 0.745, 0.843, 0.835, 0.847 > ]
            [ 0.216 rgbf < 0.745, 0.843, 0.835, 0.847 > ]
            [ 0.241 rgbf < 0.404, 0.337, 0.337, 0.463 > ]
            [ 0.241 rgbf < 0.404, 0.337, 0.337, 0.463 > ]
            [ 0.267 rgbf < 0.773, 0.729, 0.745, 0.622 > ]
            [ 0.267 rgbf < 0.773, 0.729, 0.745, 0.622 > ]
            [ 0.759 rgbf < 0.914, 0.843, 0.725, 0.651 > ]
            [ 0.759 rgbf < 0.914, 0.843, 0.725, 0.651 > ]
            [ 0.784 rgbf < 0.153, 0.133, 0.208, 0.437 > ]
            [ 0.784 rgbf < 0.153, 0.133, 0.208, 0.437 > ]
            [ 0.810 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 0.810 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 1.001 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
        }
    turbulence < 0.600, 0.600, 0.600 >
    }
}


#declare Stone1 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.439, 0.310, 0.282, 0.000 > ]
            [ 0.153 rgbf < 0.463, 0.369, 0.259, 0.000 > ]
            [ 0.153 rgbf < 0.463, 0.369, 0.259, 0.000 > ]
            [ 0.398 rgbf < 0.541, 0.369, 0.298, 0.000 > ]
            [ 0.398 rgbf < 0.541, 0.369, 0.298, 0.000 > ]
            [ 0.559 rgbf < 0.573, 0.424, 0.286, 0.000 > ]
            [ 0.559 rgbf < 0.573, 0.424, 0.286, 0.000 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.000 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.000 > ]
            [ 1.001 rgbf < 0.439, 0.310, 0.282, 0.000 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1
        phong_size 90
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 2.000, 3.000, 2.000 >
}


#declare Stone10 = texture {
    pigment {
        color rgb < 0.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 1.001 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 50.000 >
    scale < 3.000, 6.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, -50.000 >
    scale < 1.000, 2.000, 1.000 >
}


#declare Stone11 = texture {
    pigment {
        color rgb < 0.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
            [ 0.154 rgbf < 0.996, 0.698, 0.624, 0.412 > ]
            [ 0.154 rgbf < 0.996, 0.698, 0.624, 0.412 > ]
            [ 0.308 rgbf < 0.906, 0.675, 0.553, 0.616 > ]
            [ 0.308 rgbf < 0.906, 0.675, 0.553, 0.616 > ]
            [ 0.444 rgbf < 0.996, 0.682, 0.604, 0.412 > ]
            [ 0.444 rgbf < 0.996, 0.682, 0.604, 0.412 > ]
            [ 0.615 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
            [ 0.615 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.600 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.600 > ]
            [ 1.001 rgbf < 0.996, 0.824, 0.780, 0.412 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        crand 0.03
    }
    rotate < 70.000, 0.000, 30.000 >
    scale < 1.200, 3.000, 1.500 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.863, 0.757, 0.596, 0.596 > ]
            [ 0.144 rgbf < 0.925, 0.792, 0.714, 0.349 > ]
            [ 0.144 rgbf < 0.925, 0.792, 0.714, 0.349 > ]
            [ 0.288 rgbf < 0.871, 0.702, 0.659, 0.784 > ]
            [ 0.288 rgbf < 0.871, 0.702, 0.659, 0.784 > ]
            [ 0.644 rgbf < 0.992, 0.922, 0.659, 0.498 > ]
            [ 0.644 rgbf < 0.992, 0.922, 0.659, 0.498 > ]
            [ 0.983 rgbf < 0.902, 0.812, 0.714, 0.722 > ]
            [ 0.983 rgbf < 0.902, 0.812, 0.714, 0.722 > ]
            [ 1.001 rgbf < 0.863, 0.757, 0.596, 0.596 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 3.000, 3.000, 4.000 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 2.000, 1.400 >
}


#declare Stone12 = texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.831, 0.714, 0.000 > ]
            [ 0.178 rgbf < 0.925, 0.831, 0.714, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.356 rgbf < 0.871, 0.702, 0.659, 0.000 > ]
            [ 0.525 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.525 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
            [ 0.729 rgbf < 0.937, 0.882, 0.820, 0.000 > ]
            [ 0.729 rgbf < 0.937, 0.882, 0.820, 0.000 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.000 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        crand 0.03
    }
    rotate < 0.000, 0.000, 50.000 >
    scale < 1.000, 5.000, 1.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 1.000, 3.000, 2.000 >
}


#declare Stone13 = texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 1.001 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        crand 0.03
    }
    rotate < 0.000, 0.000, 50.000 >
    scale < 2.000, 5.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 1.001 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 1.000, 3.000, 2.000 >
}


#declare Stone14 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.545, 0.380, 0.345, 0.000 > ]
            [ 0.153 rgbf < 0.588, 0.475, 0.333, 0.000 > ]
            [ 0.153 rgbf < 0.588, 0.475, 0.333, 0.000 > ]
            [ 0.398 rgbf < 0.675, 0.478, 0.404, 0.000 > ]
            [ 0.398 rgbf < 0.675, 0.478, 0.404, 0.000 > ]
            [ 0.559 rgbf < 0.757, 0.635, 0.522, 0.000 > ]
            [ 0.559 rgbf < 0.757, 0.635, 0.522, 0.000 > ]
            [ 0.729 rgbf < 0.659, 0.549, 0.443, 0.000 > ]
            [ 0.729 rgbf < 0.659, 0.549, 0.443, 0.000 > ]
            [ 1.001 rgbf < 0.545, 0.380, 0.345, 0.000 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.9
        crand 0.03
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.300 rgbf < 0.329, 0.329, 0.329, 0.500 > ]
            [ 0.300 rgbf < 0.329, 0.329, 0.329, 0.500 > ]
            [ 0.400 rgbf < 0.329, 0.329, 0.329, 0.800 > ]
            [ 0.400 rgbf < 0.329, 0.329, 0.329, 0.800 > ]
            [ 1.000 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 3.000, 1.000 >
}


#declare Stone15 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.400 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.400 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.600 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.600 rgb < 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.9
        crand 0.03
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 1.000, 2.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 1.001 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 2.000, 1.000 >
}


#declare Stone16 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 1.000, 0.718, 0.541, 0.000 > ]
            [ 0.202 rgbf < 0.890, 0.651, 0.612, 0.000 > ]
            [ 0.202 rgbf < 0.890, 0.651, 0.612, 0.000 > ]
            [ 0.298 rgbf < 1.000, 0.820, 0.675, 0.000 > ]
            [ 0.298 rgbf < 1.000, 0.820, 0.675, 0.000 > ]
            [ 0.377 rgbf < 0.890, 0.643, 0.612, 0.000 > ]
            [ 0.377 rgbf < 0.890, 0.643, 0.612, 0.000 > ]
            [ 0.465 rgbf < 0.937, 0.729, 0.561, 0.000 > ]
            [ 0.465 rgbf < 0.937, 0.729, 0.561, 0.000 > ]
            [ 0.544 rgbf < 0.878, 0.604, 0.565, 0.000 > ]
            [ 0.544 rgbf < 0.878, 0.604, 0.565, 0.000 > ]
            [ 0.640 rgbf < 0.984, 0.780, 0.655, 0.000 > ]
            [ 0.640 rgbf < 0.984, 0.780, 0.655, 0.000 > ]
            [ 0.860 rgbf < 1.000, 0.863, 0.635, 0.000 > ]
            [ 0.860 rgbf < 1.000, 0.863, 0.635, 0.000 > ]
            [ 0.982 rgbf < 1.000, 0.765, 0.620, 0.000 > ]
            [ 0.982 rgbf < 1.000, 0.765, 0.620, 0.000 > ]
            [ 1.001 rgbf < 1.000, 0.718, 0.541, 0.000 > ]
        }
        turbulence < 0.400, 0.400, 0.400 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.9
        crand 0.03
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 1.300, 2.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.300 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.600 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.600 rgb < 0.329, 0.329, 0.329 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.9
        crand 0.03
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 2.000, 4.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 2.000, 1.000 >
}


#declare Stone17 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.400 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.400 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.600 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.600 rgb < 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.9
        crand 0.03
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 1.000, 2.000, 2.000 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.184, 0.310, 0.184, 0.800 > ]
            [ 0.970 rgbf < 0.184, 0.310, 0.184, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 2.000, 1.400 >
}


#declare Stone18 = texture {
    pigment {
        color rgb < 0.137, 0.557, 0.420 >
    }
    finish {
        ambient < 0.300, 0.300, 0.300 >
        diffuse 0.6
        crand 0.03
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.200 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.200 rgbf < 0.329, 0.329, 0.329, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000 , 1.000 , 1.000 , 1.000 > ]
            [ 0.700 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.500, 0.700, 0.500 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    rotate < 0.000, 0.000, -50.000 >
    scale < 2.500, 2.000, 0.500 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 0.300 > ]
            [ 0.030 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.030 rgbf < 0.000, 0.000, 0.000, 0.700 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 1.000, 1.000, 1.000, 0.900 > ]
            [ 1.001 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 0.700, 1.000, 1.000 >
}


#declare Stone19 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.241 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.241 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.284 rgbf < 0.600, 0.741, 0.608, 0.000 > ]
            [ 0.284 rgbf < 0.600, 0.741, 0.608, 0.000 > ]
            [ 0.336 rgbf < 0.820, 0.643, 0.537, 0.000 > ]
            [ 0.336 rgbf < 0.820, 0.643, 0.537, 0.000 > ]
            [ 0.474 rgbf < 0.886, 0.780, 0.714, 0.000 > ]
            [ 0.474 rgbf < 0.886, 0.780, 0.714, 0.000 > ]
            [ 0.810 rgbf < 0.996, 0.643, 0.537, 0.000 > ]
            [ 0.810 rgbf < 0.996, 0.643, 0.537, 0.000 > ]
            [ 0.836 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 0.836 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
            [ 1.001 rgbf < 0.973, 0.973, 0.976, 0.000 > ]
        }
    }
    finish {
        crand 0.02
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 0.700, 0.500 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 2.000, 3.000, 1.000 >
}


#declare Stone2 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.000 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.000 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.000 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.000 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.000 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.000 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.000 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.000 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.000 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.000 > ]
        }
        turbulence < 0.400, 0.400, 0.400 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.439, 0.310, 0.282, 0.631 > ]
            [ 0.119 rgbf < 0.463, 0.369, 0.259, 0.847 > ]
            [ 0.119 rgbf < 0.463, 0.369, 0.259, 0.847 > ]
            [ 0.322 rgbf < 0.541, 0.369, 0.298, 0.549 > ]
            [ 0.322 rgbf < 0.541, 0.369, 0.298, 0.549 > ]
            [ 0.449 rgbf < 0.573, 0.424, 0.286, 0.965 > ]
            [ 0.449 rgbf < 0.573, 0.424, 0.286, 0.965 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.741 > ]
            [ 0.729 rgbf < 0.494, 0.396, 0.306, 0.741 > ]
            [ 1.001 rgbf < 0.439, 0.310, 0.282, 0.631 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
        ambient < 0.200, 0.200, 0.200 >
    }
    rotate< 40.000, 0.000, -30.000 >
    scale < 1.300, 2.000, 1.300 >
}


#declare Stone20 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.773, 0.647, 0.569, 0.000 > ]
            [ 0.043 rgbf < 0.431, 0.322, 0.227, 0.000 > ]
            [ 0.043 rgbf < 0.431, 0.322, 0.227, 0.000 > ]
            [ 0.113 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.113 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.304 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.304 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.426 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.426 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.843 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.843 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.878 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.878 rgbf < 0.459, 0.341, 0.243, 0.000 > ]
            [ 0.983 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 0.983 rgbf < 0.278, 0.282, 0.216, 0.000 > ]
            [ 1.001 rgbf < 0.773, 0.647, 0.569, 0.000 > ]
        }
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 0.700, 0.990, 0.700 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.890, 0.690, 0.690, 0.659 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.154 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.308 rgbf < 0.745, 0.635, 0.651, 0.780 > ]
            [ 0.308 rgbf < 0.745, 0.635, 0.651, 0.780 > ]
            [ 0.444 rgbf < 0.733, 0.596, 0.557, 0.686 > ]
            [ 0.444 rgbf < 0.733, 0.596, 0.557, 0.686 > ]
            [ 0.615 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.615 rgbf < 0.996, 0.835, 0.737, 0.659 > ]
            [ 0.803 rgbf < 0.765, 0.616, 0.659, 0.761 > ]
            [ 0.803 rgbf < 0.765, 0.616, 0.659, 0.761 > ]
            [ 1.001 rgbf < 0.890, 0.690, 0.690, 0.659 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 1.300, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.200 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    rotate < 0.000, 0.000, -50.000 >
    scale < 1.900, 3.000, 0.500 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 0.600, 1.000 >
}


#declare Stone21 = texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.155 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.155 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.328 rgbf < 0.494, 0.243, 0.294, 0.000 > ]
            [ 0.328 rgbf < 0.494, 0.243, 0.294, 0.000 > ]
            [ 0.474 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.474 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.647 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.647 rgbf < 0.769, 0.329, 0.373, 0.000 > ]
            [ 0.810 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.810 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
            [ 0.922 rgbf < 0.792, 0.388, 0.427, 0.000 > ]
            [ 0.922 rgbf < 0.792, 0.388, 0.427, 0.000 > ]
            [ 1.001 rgbf < 0.686, 0.235, 0.282, 0.000 > ]
        }
        turbulence < 0.700, 0.700, 0.700 >
    }
    finish {
        crand 0.03
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.300, 2.500, 1.700 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.200 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.200 rgbf < 0.329, 0.329, 0.329, 1.000 > ] 
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.700 rgbf < 1.000 , 1.000 , 1.000 , 1.000 > ]
            [ 0.700 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 2.000, 2.000 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 0.300 > ]
            [ 0.030 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.030 rgbf < 0.000, 0.000, 0.000, 0.700 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 1.000, 1.000, 1.000, 0.900 > ]
            [ 1.001 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 0.600, 1.000 >
}


#declare Stone22 = texture {
    pigment {
        color rgb < 0.820, 0.570, 0.460 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.119 rgbf < 0.322, 0.369, 0.416, 0.847 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.322 rgbf < 0.424, 0.369, 0.420, 0.549 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.449 rgbf < 0.373, 0.424, 0.518, 0.965 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 0.729 rgbf < 0.482, 0.573, 0.533, 0.741 > ]
            [ 1.001 rgbf < 0.310, 0.384, 0.420, 0.631 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 2.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.070 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.200 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.200 rgbf < 0.329, 0.329, 0.329, 1.000 > ] 
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.300 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.700 rgbf < 1.000 , 1.000 , 1.000 , 1.000 > ]
            [ 0.700 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]        
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    rotate < 0.000, 0.000, -50.000 >
    scale < 2.000, 4.000, 1.500 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 1.000, 1.000, 1.000, 0.300 > ]
            [ 0.030 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.030 rgbf < 0.000, 0.000, 0.000, 0.700 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 1.000, 1.000, 1.000, 0.900 > ]
            [ 1.001 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 10.000, 0.000, -40.000 >
    scale < 1.000, 1.000, 1.000 >
}


#declare Stone23 = texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.000 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.000 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.000 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.000 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.000 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.000 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.000 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.000 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.000 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
            [ 1.001 rgbf < 0.784, 0.627, 0.522, 0.000 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    rotate< 40.000, 0.000, 30.000 >
    scale < 1.000, 1.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.500 > ]
            [ 0.053 rgbf < 0.784, 0.627, 0.624, 0.500 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.263 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.500 > ]
            [ 0.281 rgbf < 0.643, 0.380, 0.376, 0.500 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.500 > ]
            [ 0.325 rgbf < 0.839, 0.722, 0.722, 0.500 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.711 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.500 > ]
            [ 0.798 rgbf < 0.769, 0.380, 0.376, 0.500 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.895 rgbf < 0.824, 0.557, 0.376, 0.500 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 0.982 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
            [ 1.001 rgbf < 0.784, 0.627, 0.522, 0.500 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    rotate< 40.000, 0.000, 30.000 >
    scale < 2.000, 1.000, 2.000 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 2.000, 1.500 >
}
texture {
    pigment {
        color rgbf < 1.000, 1.000, 0.000, 0.900 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
}


#declare Stone24 = texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.824, 0.725, 0.584, 0.000 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.000 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.000 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.000 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.000 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.000 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.000 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.000 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.000 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.000 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.000 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.000 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.000 > ]
            [ 1.001 rgbf < 0.824, 0.725, 0.584, 0.000 > ]
        }
        turbulence < 0.700, 0.700, 0.700 >
    }
    rotate< 20.000, 0.000, 80.000 >
    scale < 1.000, 1.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.824, 0.725, 0.584, 0.600 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.600 > ]
            [ 0.168 rgbf < 0.514, 0.584, 0.533, 0.600 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.301 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.700 > ]
            [ 0.398 rgbf < 0.263, 0.337, 0.282, 0.700 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.600 > ]
            [ 0.558 rgbf < 0.431, 0.506, 0.451, 0.600 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.500 > ]
            [ 0.655 rgbf < 0.529, 0.631, 0.471, 0.500 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.700 > ]
            [ 0.735 rgbf < 0.333, 0.376, 0.318, 0.700 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.823 rgbf < 0.298, 0.376, 0.318, 0.600 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.500 > ]
            [ 0.876 rgbf < 0.416, 0.376, 0.318, 0.500 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.600 > ]
            [ 0.929 rgbf < 0.416, 0.376, 0.318, 0.600 > ]
            [ 1.001 rgbf < 0.824, 0.725, 0.584, 0.700 > ]
        }
    }
    rotate< 40.000, 0.000, -30.000 >
    scale < 2.000, 1.000, 2.000 >
}
texture {
    pigment {
        color rgbf < 0.753, 0.753, 0.753, 0.800 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
}


#declare Stone3 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.804, 0.569, 0.494, 0.000 > ]
            [ 0.178 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.178 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.356 rgbf < 0.667, 0.502, 0.478, 0.000 > ]
            [ 0.356 rgbf < 0.667, 0.502, 0.478, 0.000 > ]
            [ 0.525 rgbf < 0.859, 0.624, 0.545, 0.000 > ]
            [ 0.525 rgbf < 0.859, 0.624, 0.545, 0.000 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 0.729 rgbf < 0.855, 0.729, 0.584, 0.000 > ]
            [ 1.001 rgbf < 0.804, 0.569, 0.494, 0.000 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 2.000, 3.000, 2.000 >
}


#declare Stone4 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.545, 0.380, 0.345, 0.000 > ]
            [ 0.153 rgbf < 0.588, 0.475, 0.333, 0.000 > ]
            [ 0.153 rgbf < 0.588, 0.475, 0.333, 0.000 > ]
            [ 0.398 rgbf < 0.675, 0.478, 0.404, 0.000 > ]
            [ 0.398 rgbf < 0.675, 0.478, 0.404, 0.000 > ]
            [ 0.559 rgbf < 0.757, 0.635, 0.522, 0.000 > ]
            [ 0.559 rgbf < 0.757, 0.635, 0.522, 0.000 > ]
            [ 0.729 rgbf < 0.659, 0.549, 0.443, 0.000 > ]
            [ 0.729 rgbf < 0.659, 0.549, 0.443, 0.000 > ]
            [ 1.001 rgbf < 0.545, 0.380, 0.345, 0.000 > ]
        }
        turbulence < 0.500, 0.500, 0.500 >
    }
    finish {
        diffuse 0.5
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.500, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.843, 0.655, 0.655, 0.455 > ]
            [ 0.179 rgbf < 0.886, 0.769, 0.627, 0.608 > ]
            [ 0.179 rgbf < 0.886, 0.769, 0.627, 0.608 > ]
            [ 0.368 rgbf < 0.906, 0.820, 0.714, 0.392 > ]
            [ 0.368 rgbf < 0.906, 0.820, 0.714, 0.392 > ]
            [ 0.538 rgbf < 0.851, 0.671, 0.671, 0.659 > ]
            [ 0.538 rgbf < 0.851, 0.671, 0.671, 0.659 > ]
            [ 0.744 rgbf < 0.890, 0.792, 0.675, 0.392 > ]
            [ 0.744 rgbf < 0.890, 0.792, 0.675, 0.392 > ]
            [ 0.983 rgbf < 0.827, 0.612, 0.612, 0.706 > ]
            [ 0.983 rgbf < 0.827, 0.612, 0.612, 0.706 > ]
            [ 1.001 rgbf < 0.843, 0.655, 0.655, 0.455 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 1.000, 3.000, 2.000 >
}


#declare Stone5 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.996, 0.969, 0.800, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.682, 0.604, 0.000 > ]
            [ 0.154 rgbf < 0.996, 0.682, 0.604, 0.000 > ]
            [ 0.308 rgbf < 0.906, 0.820, 0.714, 0.000 > ]
            [ 0.308 rgbf < 0.906, 0.820, 0.714, 0.000 > ]
            [ 0.444 rgbf < 0.816, 0.631, 0.537, 0.000 > ]
            [ 0.444 rgbf < 0.816, 0.631, 0.537, 0.000 > ]
            [ 0.615 rgbf < 0.890, 0.792, 0.675, 0.000 > ]
            [ 0.615 rgbf < 0.890, 0.792, 0.675, 0.000 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.000 > ]
            [ 0.812 rgbf < 0.973, 0.627, 0.627, 0.000 > ]
            [ 1.001 rgbf < 0.996, 0.969, 0.800, 0.000 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 1.001 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
        ambient < 0.200, 0.200, 0.200 >
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 2.000, 1.400 >
}


#declare Stone6 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 1.000, 0.718, 0.541, 0.000 > ]
            [ 0.202 rgbf < 0.890, 0.651, 0.612, 0.000 > ]
            [ 0.202 rgbf < 0.890, 0.651, 0.612, 0.000 > ]
            [ 0.298 rgbf < 1.000, 0.820, 0.675, 0.000 > ]
            [ 0.298 rgbf < 1.000, 0.820, 0.675, 0.000 > ]
            [ 0.377 rgbf < 0.890, 0.643, 0.612, 0.000 > ]
            [ 0.377 rgbf < 0.890, 0.643, 0.612, 0.000 > ]
            [ 0.465 rgbf < 0.937, 0.729, 0.561, 0.000 > ]
            [ 0.465 rgbf < 0.937, 0.729, 0.561, 0.000 > ]
            [ 0.544 rgbf < 0.878, 0.604, 0.565, 0.000 > ]
            [ 0.544 rgbf < 0.878, 0.604, 0.565, 0.000 > ]
            [ 0.640 rgbf < 0.984, 0.780, 0.655, 0.000 > ]
            [ 0.640 rgbf < 0.984, 0.780, 0.655, 0.000 > ]
            [ 0.860 rgbf < 1.000, 0.863, 0.635, 0.000 > ]
            [ 0.860 rgbf < 1.000, 0.863, 0.635, 0.000 > ]
            [ 0.982 rgbf < 1.000, 0.765, 0.620, 0.000 > ]
            [ 0.982 rgbf < 1.000, 0.765, 0.620, 0.000 > ]
            [ 1.001 rgbf < 1.000, 0.718, 0.541, 0.000 > ]
        }
        turbulence < 0.400, 0.400, 0.400 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.500, 3.000, 3.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.300 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.600 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.600 rgb < 0.329, 0.329, 0.329 > ]
            [ 1.000 rgb < 0.329, 0.329, 0.329 > ]
        }
        turbulence < 0.000, 0.000, 0.000 >
    }
    finish {
        crand 0.02
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, 30.000 >
    scale < 2.000, 4.000, 1.300 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
        ambient < 0.200, 0.200, 0.200 >
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 2.000, 1.400 >
}


#declare Stone7 = texture {
    pigment {
        color rgb < 0.435, 0.259, 0.259 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.545, 0.380, 0.345, 0.733 > ]
            [ 0.263 rgbf < 0.588, 0.475, 0.333, 0.741 > ]
            [ 0.263 rgbf < 0.588, 0.475, 0.333, 0.741 > ]
            [ 0.432 rgbf < 0.675, 0.478, 0.404, 0.545 > ]
            [ 0.432 rgbf < 0.675, 0.478, 0.404, 0.545 > ]
            [ 0.551 rgbf < 0.757, 0.635, 0.522, 0.384 > ]
            [ 0.551 rgbf < 0.757, 0.635, 0.522, 0.384 > ]
            [ 0.720 rgbf < 0.659, 0.549, 0.443, 0.675 > ]
            [ 0.720 rgbf < 0.659, 0.549, 0.443, 0.675 > ]
            [ 1.001 rgbf < 0.545, 0.380, 0.345, 0.733 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 1.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 0.216 rgbf < 0.745, 0.843, 0.835, 0.847 > ]
            [ 0.216 rgbf < 0.745, 0.843, 0.835, 0.847 > ]
            [ 0.241 rgbf < 0.404, 0.337, 0.337, 0.463 > ]
            [ 0.241 rgbf < 0.404, 0.337, 0.337, 0.463 > ]
            [ 0.267 rgbf < 0.773, 0.729, 0.745, 0.622 > ]
            [ 0.267 rgbf < 0.773, 0.729, 0.745, 0.622 > ]
            [ 0.759 rgbf < 0.914, 0.843, 0.725, 0.651 > ]
            [ 0.759 rgbf < 0.914, 0.843, 0.725, 0.651 > ]
            [ 0.784 rgbf < 0.153, 0.133, 0.208, 0.437 > ]
            [ 0.784 rgbf < 0.153, 0.133, 0.208, 0.437 > ]
            [ 0.810 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 0.810 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 1.001 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, 60.000 >
    scale < 3.500, 5.000, 4.000 >
}


#declare Stone8 = texture {
    pigment {
        color rgb < 1.000, 1.000, 1.000 >
    }
    finish {
        ambient < 0.400, 0.400, 0.400 >
        crand 0.06
        diffuse 0.7
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.153 rgbf < 0.769, 0.686, 0.592, 0.792 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.398 rgbf < 0.843, 0.753, 0.718, 0.396 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.559 rgbf < 0.780, 0.667, 0.561, 0.976 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 0.729 rgbf < 0.741, 0.659, 0.576, 0.820 > ]
            [ 1.001 rgbf < 0.729, 0.502, 0.451, 0.306 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, -30.000 >
    scale < 2.000, 3.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 0.216 rgbf < 0.745, 0.843, 0.835, 0.847 > ]
            [ 0.216 rgbf < 0.745, 0.843, 0.835, 0.847 > ]
            [ 0.241 rgbf < 0.404, 0.337, 0.337, 0.463 > ]
            [ 0.241 rgbf < 0.404, 0.337, 0.337, 0.463 > ]
            [ 0.267 rgbf < 0.773, 0.729, 0.745, 0.622 > ]
            [ 0.267 rgbf < 0.773, 0.729, 0.745, 0.622 > ]
            [ 0.759 rgbf < 0.914, 0.843, 0.725, 0.651 > ]
            [ 0.759 rgbf < 0.914, 0.843, 0.725, 0.651 > ]
            [ 0.784 rgbf < 0.153, 0.133, 0.208, 0.437 > ]
            [ 0.784 rgbf < 0.153, 0.133, 0.208, 0.437 > ]
            [ 0.810 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 0.810 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
            [ 1.001 rgbf < 0.812, 0.812, 0.812, 0.835 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        ambient
        phong_size 90
    }
    rotate < 0.000, 0.000, 40.000 >
    scale < 5.000, 3.000, 4.000 >
}

#declare Stone9 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.154 rgbf < 0.745, 0.745, 0.753, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.308 rgbf < 0.902, 0.902, 0.859, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.444 rgbf < 0.729, 0.706, 0.694, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.615 rgbf < 0.588, 0.592, 0.635, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 0.803 rgbf < 0.608, 0.616, 0.659, 0.000 > ]
            [ 1.001 rgbf < 0.894, 0.886, 0.886, 0.000 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate< 40.000, 0.000, -30.000 >
    scale < 1.200, 2.400, 1.200 >
}
texture {
    pigment {
        marble
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 0.600 > ]
            [ 0.040 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.040 rgbf < 0.550, 0.090, 0.090, 0.800 > ]
            [ 0.970 rgbf < 0.329, 0.329, 0.329, 0.900 > ]
            [ 0.970 rgbf < 0.000, 0.000, 0.000, 0.900 > ]
            [ 1.001 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
        }
        turbulence < 0.850, 0.850, 0.850 >
    }
    finish {
        phong 1.0
        phong_size 90
        ambient < 0.200, 0.200, 0.200 >
    }
    rotate < 10.000, 0.000, -20.000 >
    scale < 1.000, 2.000, 1.400 >
}


#declare Aqua_Glass = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        refraction 1
        ior 1.2
    }
    pigment {
        color rgbf < 0.439, 0.859, 0.576, 0.960 >
    }
}


#declare Red_Glass = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        refraction 1
        ior 1.2
    }
    pigment {
        color rgb < 1.000, 0.000, 0.000 > filter 0.96
    }
}


#declare White_Metal = texture {
    finish {
        metallic
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.8
        brilliance 6.0
        phong 1.0
        phong_size 80
    }
    pigment {
        color rgb < 1.000, 1.000, 1.000 >
    }
}


#declare Black_Box = texture {
    finish {
        metallic
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.8
        brilliance 6.0
        reflection < 0.600, 0.600, 0.600 >
        phong 1.0
        phong_size 80
    }
    pigment {
        color rgb < 0.000, 0.000, 0.000 >
    }
}


#declare Blue_Engine = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        reflection < 0.200, 0.200, 0.200 >
    }
    pigment {
        color rgb < 0.000, 0.000, 1.000 >
    }
}


#declare Blue_Glass = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        refraction 1
        ior 1.2
    }
    pigment {
        color rgb < 0.000, 0.000, 1.000 > filter 0.96
    }
}


#declare Blue_Metal = texture {
    finish {
        metallic
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.8
        brilliance 6.0
        reflection < 0.700, 0.700, 0.700 >
        phong 1.0
        phong_size 80
    }
    pigment {
        color rgb < 0.000, 0.000, 0.610 >
    }
}


#declare Brass_Gifmap = texture {
    finish {
        metallic
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.8
        brilliance 6.0
        reflection < 0.400, 0.400, 0.400 >
        phong 0.8
        phong_size 120
    }
    pigment {
        color rgb < 0.578, 0.422, 0.195 >
    }
}


#declare Brown_Brick = texture {
    pigment {
        color rgb < 0.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.034 rgbf < 0.851, 0.812, 0.741, 0.235 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.342 rgbf < 0.792, 0.694, 0.690, 0.839 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.462 rgbf < 0.631, 0.506, 0.471, 0.608 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.632 rgbf < 0.851, 0.812, 0.741, 0.922 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 0.983 rgbf < 0.647, 0.655, 0.655, 0.282 > ]
            [ 1.001 rgbf < 0.027, 0.012, 0.012, 0.000 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    rotate < 0.000, 0.000, 50.000 >
    scale < 3.000, 6.000, 2.000 >
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.153 rgbf < 0.925, 0.831, 0.714, 0.678 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.297 rgbf < 0.871, 0.702, 0.659, 0.475 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.441 rgbf < 0.831, 0.631, 0.569, 0.918 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 0.763 rgbf < 0.937, 0.882, 0.820, 0.655 > ]
            [ 1.001 rgbf < 0.831, 0.631, 0.569, 0.447 > ]
        }
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        phong 1.0
        phong_size 90
    }
    rotate < 0.000, 0.000, -50.000 >
    scale < 1.000, 2.000, 1.000 >
}


#declare BWood = texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgb < 0.450, 0.250, 0.000 > ]
            [ 0.550 colour red 0.45 green 0.25 ]
            [ 0.550 rgb < 0.300, 0.160, 0.000 > ]
            [ 1.000 colour red 0.30 green 0.16 ]
            }
        turbulence < 0.100, 0.100, 0.100 >
        scale < 0.600, 1000.000, 0.600 >
        translate < 100.000, 0.000, -200.000 >
    }
    finish {
        specular 1
        roughness 0.02
    }
}

#declare Chrome_Gifmap = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        brilliance 6.0
        reflection < 0.600, 0.600, 0.600 >
        phong 0.8
        phong_size 120
    }
    pigment {
        color rgb < 0.200, 0.200, 0.200 >
    }
}


#declare Cream_Plastic = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
    }
    pigment {
        color rgb < 0.859, 0.576, 0.439 >
    }
}


#declare Cyan_Metalic = texture {
    finish {
        metallic
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.8
        brilliance 6.0
        reflection < 0.600, 0.600, 0.600 >
        phong 1.0
        phong_size 80
    }
    pigment {
        agate
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.900 > ]
            [ 0.300 rgb < 0.000, 0.000, 0.800 > ]
            [ 0.300 rgb < 0.000, 0.000, 0.800 > ]
            [ 1.000 rgb < 0.000, 0.000, 0.400 > ]
        }
    }
}



#declare Gray_SemiGloss = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        reflection < 0.300, 0.300, 0.300 >
    }
    pigment {
        color rgb < 0.400, 0.400, 0.400 >
    }
}


#declare Green_Metal = texture {
    finish
    {
        metallic
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.7
        brilliance 6
        reflection < 0.250, 0.250, 0.250 >
        phong 0.75
        phong_size 80
    }
    pigment
    {
        color rgb < 0.800, 1.000, 0.950 >
    }
}


#declare Jade = texture {
    finish {
        reflection < 0.600, 0.600, 0.600 >
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.8
        brilliance 6.0
    }
    pigment
    {
    marble
            color_map
        {
            [ 0.000 rgb < 0.100, 0.600, 0.100 > ]
            [ 0.800 rgb < 0.000, 0.300, 0.000 > ]
            [ 0.800 rgb < 0.100, 0.600, 0.100 > ]
            [ 1.001 rgb < 0.000, 0.200, 0.000 > ]
        }
    turbulence < 1.800, 1.800, 1.800 >
    }
}


#declare Maroon_Glass = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        refraction 1
        ior 1.2
    }
    pigment {
        color rgbf < 0.557, 0.137, 0.420, 0.960 >
    }
}


#declare Mirror = texture {
    finish
    {
        reflection < 0.600, 0.600, 0.600 >
    }
    pigment
    {
    marble
            color_map
        {
            [ 0.000 rgb < 0.100, 0.600, 0.100 > ]
            [ 0.800 rgb < 0.000, 0.300, 0.000 > ]
            [ 0.800 rgb < 0.100, 0.600, 0.100 > ]
            [ 1.001 rgb < 0.000, 0.200, 0.000 > ]
        }
    turbulence < 1.800, 1.800, 1.800 >
    }
}


#declare Old_Metal = texture {
    finish {
        metallic
        ambient < 0.250, 0.250, 0.250 >
        diffuse 0.65
        reflection < 0.100, 0.100, 0.100 >
        brilliance 6
        phong 1
        phong_size 100
    }
    pigment {
        color rgb < 0.200, 0.200, 0.200 >
    }
}


#declare Orange_Glass = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        refraction 1
        ior 1.2
    }
    pigment {
        color rgbf < 1.000, 0.500, 0.000, 0.960 >
    }
}


#declare Polished_Chrome = texture {
    pigment {
        color rgb < 0.200, 0.200, 0.200 >
    }
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        brilliance 6.0
        reflection < 0.600, 0.600, 0.600 >
        phong 0.8
        phong_size 120
    }
}


#declare Purple_Glass = texture {
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.7
        phong 1.0
        phong_size 70.0
        refraction 1
        ior 1.2
    }
    pigment {
        color rgbf < 0.918, 0.678, 0.918, 0.960 >
    }
}


#declare Sat1Ball = texture {
    pigment {
        radial
        color_map {
            [ 0 rgb < 0.000, 0.000, 1.000 > ]
            [ 0.167 rgb < 1.000, 0.000, 1.000 > ]
            [ 0.333 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.500 rgb < 1.000, 1.000, 0.000 > ]
            [ 0.667 rgb < 0.000, 1.000, 0.000 > ]
            [ 0.833 rgb < 0.000, 1.000, 1.000 > ]
            [ 1 rgb < 0.000, 0.000, 1.000 > ]
        }
        phase .5
    }
    finish {
        phong .8
        phong_size 30
    }
}
texture {
    pigment {
        radial
        color_map {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.500 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.500 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        frequency 12
    }
}


#declare Warbird_Engine = texture {
    pigment {
        color rgb < 0.700, 0.560, 0.370 >
    }
    finish {
        ambient < 0.350, 0.350, 0.350 >
        diffuse 1.00
        brilliance 15
        phong 0.41
        phong_size 5
    }
}


#declare WWood = texture {
    pigment {
        wood
        colour_map {
            [ 0.000 rgb < 0.700, 0.400, 0.000 > ]
            [ 0.350 colour red 0.7 green 0.4 ]
            [ 0.350 rgb < 0.950, 0.620, 0.000 > ]
            [ 1.000 colour red 0.95 green 0.62 ]
        }
        turbulence < 0.100, 0.100, 0.100 >
        scale < 0.600, 1000.000, 0.600 >
        translate < 200.000, 0.000, 100.000 >
    }
    finish {
        specular 1
        roughness 0.02
    }
}


#declare Vase_Wood = texture {
    finish
    {
    ambient < 0.100, 0.100, 0.100 >
    diffuse 0.50
    brilliance 2.00
    phong 3.00
    phong_size 20.00
    metallic
    reflection < 1.500, 1.500, 1.500 >
    }
    pigment{
    wood
    color_map {
        [ 0.000 rgb < 0.888, 0.600, 0.300 > ]
        [ 0.800 rgb < 0.888, 0.600, 0.300 > ]
        [ 0.800 rgb < 0.600, 0.400, 0.200 > ]
        [ 1.010 rgb < 0.400, 0.300, 0.200 > ]
    }
    turbulence < 0.040, 0.040, 0.040 >
}
}

#declare CJFAlabaster = texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgb < 0.900, 0.750, 0.750 > ]
            [ 1.000 rgb < 0.600, 0.600, 0.600 > ]
        }
        scale < 1.000, 1.000, 1.000 >
        turbulence < 0.250, 0.250, 0.250 >
    }
    finish {
        ambient < 0.250, 0.250, 0.250 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.600, 0.196, 0.800, 1.000 > ]
            [ 0.900 rgbf < 0.600, 0.196, 0.800, 0.500 > ]
            [ 0.900 rgb < 0.420, 0.137, 0.557 > ]
            [ 1.000 rgb < 0.420, 0.137, 0.557 > ]
        }
        scale < 5.000, 5.000, 5.000 >
    }
    finish {
        specular 1
        roughness 0.0001
        phong 0.25
        phong_size 75
        brilliance 4
    }
}


#declare Flame_Tex = texture {
    pigment {
        gradient < 0.0 0.0 1.0 >
            turbulence < 0.150, 0.150, 0.150 >
        colour_map {
            [ 0.000 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.200 colour red .8 green .4 blue 0 ]
            [ 0.200 rgb < 0.800, 0.400, 0.000 > ]
            [ 0.600 colour red 1.0 green 1.0 blue 1.0 filter 1 ]
            [ 0.600 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
    }
    finish {
        ambient < 1.000, 1.000, 1.000 >
        phong 0
    }
}


#declare FloorWood = texture {
    pigment
    {
    wood
    scale < 0.050, 0.750, 1.000 >
    turbulence < 0.100, 0.100, 0.100 >
    octaves 3
            color_map
        {
            [ 0.000 rgb < 0.200, 0.050, 0.100 > ]
            [ 0.100 rgb < 0.200, 0.050, 0.100 > ]
            [ 0.100 rgb < 0.200, 0.050, 0.100 > ]
            [ 0.900 rgb < 0.400, 0.050, 0.100 > ]
            [ 0.900 rgb < 0.400, 0.050, 0.100 > ]
            [ 1.000 rgb < 0.400, 0.050, 0.100 > ]
        }
    }
}


#declare LimogesOpakGlossTex = texture {
    pigment {
        color rgb < 0.978, 0.980, 1.000 >
    }
    finish {
        ambient < 0.550, 0.550, 0.550 >
        diffuse 0.45
        specular 1
        brilliance 3
        reflection < 0.125, 0.125, 0.125 >
        roughness 0.001
    }
}


#declare LimogesOpakSemiTex = texture {
    pigment {
        color rgb < 0.978, 0.980, 1.000 >
    }
    finish {
        specular 1
        roughness 0.001
        reflection < 0.061, 0.061, 0.061 >
        roughness 0.003
    }
    normal {
        bumps 0.7
    }
    scale < 2.000, 2.000, 2.000 >
}


#declare LimogesOpakTex = texture {
    pigment {
        color rgb < 0.978, 0.980, 1.000 >
    }
    finish {
        specular 0.5
        roughness 0.15
        ambient < 0.520, 0.520, 0.520 >
        diffuse 0.48
    }
}


#declare LimogesTranspTex = texture {
    pigment {
        color rgbf < 0.978, 0.980, 1.000, 0.250 >
    }
    finish {
        ambient < 0.500, 0.500, 0.500 >
        diffuse 0.5
        phong 1
        phong_size 20
    }
}


#declare Pearl = texture {
    pigment
    {
        agate
        color_map {
            [ 0.000 rgb < 0.900, 0.980, 1.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.300 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.800 rgb < 1.000, 1.000, 1.000 > ]
            [ 0.800 rgb < 1.000, 0.980, 0.970 > ]
            [ 1.000 rgb < 1.000, 1.000, 1.000 > ]
        }
    }
    scale < 0.750, 0.150, 0.750 >
}

#declare BaseTexture = texture {
    pigment {
        spotted
        color_map {
            [ 0.000 rgb < 0.647, 0.165, 0.165 > ]
            [ 0.150 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.150 rgb < 0.376, 0.482, 0.545 > ]
            [ 1.000 rgb < 0.376, 0.482, 0.545 > ]
        }
        turbulence < 0.250, 0.250, 0.250 >
    }
    scale < 0.050, 0.050, 0.050 >
}
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgb < 0.871, 0.722, 0.529 > ]
            [ 0.400 rgb < 0.647, 0.165, 0.165 > ]
            [ 0.400 rgbf < 0.400, 0.400, 0.400, 1.000 > ]
            [ 0.400 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.150, 0.150, 0.150 >
    }
    translate < 0.000, 0.250, 0.000 >
}
texture {
    pigment {
        gradient z
        color_map {
            [ 0.000 rgbf < 0.000, 0.000, 0.000, 1.000 > ]
            [ 0.000 rgbf < 0.100, 0.100, 0.100, 1.000 > ]
            [ 0.100 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.120 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.120 rgbf < 0.120, 0.120, 0.120, 1.000 > ]
            [ 0.120 rgbf < 0.300, 0.300, 0.300, 1.000 > ]
            [ 0.300 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.320 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.320 rgbf < 0.320, 0.320, 0.320, 1.000 > ]
            [ 0.320 rgbf < 0.600, 0.600, 0.600, 1.000 > ]
            [ 0.600 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.620 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.620 rgbf < 0.620, 0.620, 0.620, 1.000 > ]
            [ 0.620 rgbf < 0.700, 0.700, 0.700, 1.000 > ]
            [ 0.700 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.720 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.720 rgbf < 0.720, 0.720, 0.720, 1.000 > ]
            [ 0.720 rgbf < 0.900, 0.900, 0.900, 1.000 > ]
            [ 0.900 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.920 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.920 rgbf < 0.920, 0.920, 0.920, 1.000 > ]
            [ 0.920 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.250, 0.250, 0.250 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.6
        specular 1.0
        reflection < 0.150, 0.150, 0.150 >
        roughness 0.0001
    }
}


#declare BaseTex = texture {
    pigment {
        marble
        color_map {
            [ 0 rgb < 0.900, 0.900, 0.900 > ]
            [ 0.45 rgb < 0.750, 0.750, 0.750 > ]
            [ 0.55 rgb < 0.300, 0.150, 0.070 > ]
            [ 0.65 rgb < 0.750, 0.750, 0.750 > ]
            [ 1 rgb < 0.900, 0.900, 0.900 > ]
        }
        turbulence < 0.300, 0.300, 0.300 >
        scale < 1.300, 1.300, 1.300 >
        omega 0.4
    }
    finish {
        roughness 0.02
        specular 0.75
        reflection < 0.125, 0.125, 0.125 >
    }
}

#declare DD_Cloud2 = texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgbf < 0.750, 0.750, 0.750, 0.000 > ]
            [ 0.400 rgbf < 0.900, 0.900, 0.900, 0.900 > ]
            [ 0.400 rgbf < 0.900, 0.900, 0.900, 0.900 > ]
            [ 0.700 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.700 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        omega .5
        octaves 10
        turbulence < 0.800, 0.800, 0.800 >
        lambda 2.5
    }
    finish {
        ambient < 1.000, 1.000, 1.000 >
        diffuse 0.0
    }
}
texture {
    pigment {
        bozo
        color_map {
            [ 0.000 rgbf < 0.750, 0.750, 0.750, 0.000 > ]
            [ 0.400 rgbf < 0.900, 0.900, 0.900, 0.900 > ]
            [ 0.400 rgbf < 0.900, 0.900, 0.900, 0.900 > ]
            [ 0.600 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.600 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.000 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        omega .5
        octaves 10
        turbulence < 0.800, 0.800, 0.800 >
        lambda 2.5
    }
    finish {
        ambient < 0.950, 0.950, 0.950 >
        diffuse 0.0
    }
    translate < 0.000, -0.150, 0.000 >
    scale < 0.900, 0.900, 0.900 >
}


#declare FinTex = texture {
    pigment {
        color rgb < 1.000, 0.000, 0.000 > filter 0.5
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.6
        specular 1.0
        reflection < 0.150, 0.150, 0.150 >
        refraction 1.0
        ior 1.33
        roughness 0.0001
    }
}


#declare FishBodyTex = texture {
    pigment {
        gradient x
        color_map {
            [ 0.000 rgb < 0.376, 0.482, 0.545 > ]
            [ 0.200 rgb < 0.376, 0.482, 0.545 > ]
            [ 0.200 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.330 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.330 rgb < 0.376, 0.482, 0.545 > ]
            [ 0.530 rgb < 0.376, 0.482, 0.545 > ]
            [ 0.530 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.660 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.660 rgb < 0.376, 0.482, 0.545 > ]
            [ 0.860 rgb < 0.376, 0.482, 0.545 > ]
            [ 0.860 rgb < 1.000, 0.000, 0.000 > ]
            [ 1.000 rgb < 0.000, 0.000, 0.000 > ]
        }
        translate < 1.000, 0.000, 0.000 >
        turbulence < 0.075, 0.075, 0.075 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.6
        specular 1.0
        reflection < 0.150, 0.150, 0.150 >
        roughness 0.0001
    }
    rotate< -90.000, 25.000, 0.000 >
    scale < 6.000, 1.000, 1.000 >
}


#declare GlassFrit = texture {
    pigment {
        spotted
        color_map {
            [ 0.000 rgb < 0.647, 0.165, 0.165 > ]
            [ 0.100 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.100 rgbf < 0.100, 0.100, 0.100, 1.000 > ]
            [ 0.100 rgbf < 0.500, 0.500, 0.500, 1.000 > ]
            [ 0.500 rgb < 0.647, 0.165, 0.165 > ]
            [ 0.510 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.510 rgbf < 0.510, 0.510, 0.510, 1.000 > ]
            [ 0.510 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        turbulence < 0.250, 0.250, 0.250 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.5
        specular 1.0
        reflection < 0.150, 0.150, 0.150 >
        refraction 1.0
        ior 1.33
        roughness 0.0001
    }
    scale < 0.100, 0.100, 0.100 >
}


#declare RedEyeTex = texture {
    pigment {
        color rgb < 1.000, 0.000, 0.000 >
    }
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse 0.50
        phong 1.00
        phong_size 100.00
    }
}


#declare StemBeadTex = texture {
    pigment {
        radial
        color_map {
            [ 0.5 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.5 rgb < 0.376, 0.482, 0.545 > ]
        }
        rotate < 0.000, 90.000, 0.000 >
        frequency 12
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.6
        specular 1.0
        reflection < 0.150, 0.150, 0.150 >
        roughness 0.0001
    }
}


#declare StoneLayer1 = texture {
    pigment {
        bozo
        color_map {
            [ 0.0 rgb < 0.100, 0.100, 0.500 > ]
            [ 0.3 rgb < 0.200, 0.200, 0.900 > ]
            [ 0.4 rgb < 0.400, 0.400, 0.600 > ]
            [ 0.8 rgb < 0.200, 0.000, 0.700 > ]
        }
    octaves 2
    omega .6
    turbulence < 0.900, 0.900, 0.900 >
    }
    finish {
        ambient.25
    }
}


#declare StoneLayer2 = texture {
    pigment {
        marble
    octaves 4
    omega .7
    turbulence < 0.700, 0.700, 0.700 >
        color_map {
            [ 0.0 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.75 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 0.85 rgbf < 0.500, 0.500, 0.500, 0.500 > ]
            [ 0.92 rgbf < 0.150, 0.180, 0.100, 0.000 > ]
            [ 0.9 rgbf < 0.100, 0.100, 0.000, 0.000 > ]
        }
    }
    finish {
        ambient.25
    }
}


#declare CaseTex = texture {
    pigment {
        color rgb < 0.890, 0.867, 0.505 >
    }
    finish {
        reflection < 0.600, 0.600, 0.600 >
        brilliance 2.500
        phong 0.77
        phong_size 15.00
        metallic
    }
}


#declare PaneTex = texture {
    pigment {
        color rgbf < 1.000, 1.000, 1.000, 0.999 >
    }
    finish {
        ambient < 0.000, 0.000, 0.000 >
        diffuse 0.0
        refraction 1
        ior 1.55
        brilliance 1.25
        phong 0.80
        phong_size 18.00
    }
}


#declare Post_Tex = texture {
    pigment {
        color rgb < 0.900, 0.700, 0.350 >
    }
    finish {
        reflection < 0.600, 0.600, 0.600 >
        brilliance 2.5
        phong 0.80
        phong_size 15.00
        metallic
    }
}


#declare PupilTex = texture {
    pigment {
        color rgb < 0.000, 0.000, 0.000 >
    }
    finish {
        ambient < 0.000, 0.000, 0.000 >
        diffuse 0.00
    }
}


#declare ShelfTex = texture {
    pigment {
        granite
        turbulence < 0.250, 0.250, 0.250 >
        lambda 1
        omega 0.4
        scale < 1.200, 1.200, 1.200 >
        color_map {
            [ 0 rgb < 0.300, 0.300, 0.300 > ]
            [ 0.4 rgb < 0.450, 0.350, 0.450 > ]
            [ 0.5 rgb < 0.500, 0.370, 0.500 > ]
            [ 0.6 rgb < 0.450, 0.370, 0.380 > ]
            [ 1 rgb < 0.300, 0.300, 0.300 > ]
        }
    }
    finish {
        roughness 0.23
        specular 0.5
        reflection < 0.120, 0.120, 0.120 >
    }
}


#declare Version1_Surface1 = texture { 
pigment {
  hexagon
  color red 0.5 green 0.5 blue 0.0    /* dull yellow */
  color red 0.0 green 0.5 blue 0.5    /* dull cyan */
  color red 0.5 green 0.0 blue 0.5    /* dull magenta */
  rotate <90, 0, 0>
}}

#declare Version1_Surface2 = texture { 
pigment {
	gradient x
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/2] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/2] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/2] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/2] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/2] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/2] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/2] /* red */
	}
}}

#declare Version1_Surface3 = texture { 
pigment {
	gradient x
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/3] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/3] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/3] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/3] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/3] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/3] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/3] /* red */
	}
}}

#declare Version2_Surface1 = texture { 
  pigment {
	hexagon
	color red 1.0 green 0.0 blue 0.0	/* red */
	color red 0.0 green 1.0 blue 0.0	/* green */
	color red 0.0 green 0.0 blue 1.0	/* blue */
	rotate <90, 0, 0>
}}

#declare Version2_Surface2 = texture { 
  pigment {
	hexagon
	color red 1.0 green 1.0 blue 0.0 filter 1 - 1/2	/* yellow */
	color red 0.0 green 1.0 blue 1.0 filter 1 - 1/2	/* cyan */
	color red 1.0 green 0.0 blue 1.0 filter 1 - 1/2	/* magenta */
	rotate <90, 0, 0>
}}

#declare Version2_Surface3 = texture { 
  pigment {
	hexagon
	color red 1.0 green 1.0 blue 0.0 filter 1 - 1/3 /* yellow */
	color red 0.0 green 1.0 blue 1.0 filter 1 - 1/3 /* cyan */
	color red 1.0 green 0.0 blue 1.0 filter 1 - 1/3 /* magenta */
	rotate <90, 0, 0>
}}

#declare Version2_Surface4 = texture { 
  pigment {
	hexagon
	color red 1.0 green 1.0 blue 1.0 filter 0.5 	/* white 50% transmission */
	color red 1.0 green 1.0 blue 1.0 filter 0.75	/* white 75% transmission */
	color red 1.0 green 1.0 blue 1.0 filter 1.0 	/* white 100% transmission */
	rotate <90, 0, 0>
}}



#declare Version3_Surface2 = texture { 
  pigment {
	radial
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/2] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/2] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/2] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/2] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/2] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/2] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/2] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/2] /* red */
	}
	frequency 2
	phase 2/7
	rotate <90, 0, 0>
}}

#declare Version3_Surface3 = texture { 
  pigment {
	radial
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/3] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/3] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/3] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/3] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/3] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/3] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/3] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/3] /* red */
	}
	frequency 2
	phase 3/7
	rotate <90, 0, 0>
}}

#declare Version3_Surface4 = texture { 
  pigment {
	radial
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/4] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/4] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/4] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/4] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/4] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/4] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/4] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/4] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/4] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/4] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/4] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/4] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/4] /* red */
	}
	frequency 2
	phase 4/7
	rotate <90, 0, 0>
}}

#declare Version3_Surface5 = texture { 
  pigment {
	radial
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/5] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/5] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/5] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/5] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/5] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/5] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/5] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/5] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/5] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/5] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/5] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/5] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/5] /* red */
	}
	frequency 2
	phase 5/7
	rotate <90, 0, 0>
}}

#declare Version3_Surface6 = texture { 
  pigment {
	radial
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/6] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/6] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/6] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/6] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/6] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/6] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/6] /* red */
	}
	frequency 2
	phase 6/7
	rotate <90, 0, 0>
}}

#declare Version3_Surface7 = texture { 
  pigment {
	radial
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/7] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/7] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/7] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/7] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/7] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/7] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/7] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/7] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/7] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/7] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/7] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/7] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/7] /* red */
	}
	frequency 2
	rotate <90, 0, 0>
}}


#declare Version4_Surface6 = texture { 
  pigment {
	onion
	color_map {
		[ 0/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/6] /* red */
		[ 1/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 2/12 color red 1.0 green 1.0 blue 0.0 filter 1 - 1/6] /* yellow */
		[ 3/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 4/12 color red 0.0 green 1.0 blue 0.0 filter 1 - 1/6] /* green */
		[ 5/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 6/12 color red 0.0 green 1.0 blue 1.0 filter 1 - 1/6] /* cyan */
		[ 7/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[ 8/12 color red 0.0 green 0.0 blue 1.0 filter 1 - 1/6] /* blue */
		[ 9/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[10/12 color red 1.0 green 0.0 blue 1.0 filter 1 - 1/6] /* magenta */
		[11/12 color red 0.0 green 0.0 blue 0.0 filter 1 - 1/6] /* - */
		[12/12 color red 1.0 green 0.0 blue 0.0 filter 1 - 1/6] /* red */
	}
}}


#declare Negativ1 = texture {
    pigment
    {
         bozo
    lambda 6
    turbulence < -3.000, -3.000, -3.000 >
    octaves 2
         color_map
        {
            [ 0.000 rgb < 0.557, 0.137, 0.137 > ]
            [ 0.200 rgb < 0.557, 0.137, 0.137 > ]
            [ 0.200 rgb < 0.557, 0.137, 0.137 > ]
            [ 1.000 rgb < 1.000, 0.500, 0.000 > ]
        }
    }
    finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Negativ2 = texture {
    pigment
    {
    agate
        agate_turb 0
    lambda -5
    octaves -1
    omega -10
            color_map
        {
            [ 0.000 rgb < 0.753, 0.753, 0.753 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.400 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.500 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.500 rgb < 0.329, 0.329, 0.329 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.000 > ]
            [ 1.000 rgb < 0.753, 0.753, 0.753 > ]
        }
        rotate < 15.000, 15.000, 0.000 >
    }
    finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Negativ3 = texture {
    pigment
    {
    agate
    lambda 1
    turbulence < -3.000, -3.000, -3.000 >
    octaves 6
    omega .001
            color_map
        {
            [ 0.000 rgb < 0.300, 0.300, 1.000 > ]
            [ 0.300 rgb < 0.000, 0.000, 0.610 > ]
            [ 0.300 rgb < 0.000, 0.000, 0.610 > ]
            [ 0.500 rgb < 0.300, 0.300, 1.000 > ]
            [ 0.500 rgb < 0.300, 0.300, 1.000 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.610 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.610 > ]
            [ 1.000 rgb < 0.300, 0.300, 1.000 > ]
        }
        scale < 2.000, 2.000, 2.000 >
    }
    finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Negativ4 = texture {
    pigment
    {
    bozo
    lambda -10
    turbulence < 1.000, 1.000, 1.000 >
    octaves 3
            color_map
        {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.100 rgb < 0.550, 0.090, 0.090 > ]
            [ 0.100 rgb < 0.550, 0.090, 0.090 > ]
            [ 0.900 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.900 rgb < 0.000, 0.000, 0.000 > ]
            [ 1.000 rgb < 0.000, 0.000, 0.000 > ]
        }
    }
    finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Negativ5 = texture {
    pigment
    {
    marble
    lambda -4
    turbulence < -3.000, -3.000, -3.000 >
    octaves 3
    omega .2
            color_map
        {
            [ 0.000 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.700 rgb < 0.290, 0.460, 0.430 > ]
            [ 0.700 rgb < 0.290, 0.460, 0.430 > ]
            [ 1.000 rgb < 0.000, 0.000, 0.000 > ]
        }
        scale < 2.000, 2.000, 2.000 >
    }
    finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Negativ6 = texture {
    pigment
    {
    spotted
    lambda 6
    turbulence < -3.000, -3.000, -3.000 >
    octaves 2
           color_map
        {
            [ 0.000 rgb < 0.557, 0.137, 0.137 > ]
            [ 0.200 rgb < 0.557, 0.137, 0.137 > ]
            [ 0.200 rgb < 0.557, 0.137, 0.137 > ]
            [ 1.000 rgb < 1.000, 0.500, 0.000 > ]
        }
    }
     finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Negativ7 = texture {
    pigment
    {
    agate
        agate_turb 0
    lambda -5
    octaves -1
    omega 0
            color_map
        {
            [ 0.000 rgb < 0.196, 0.196, 0.800 > ]
            [ 0.400 rgb < 0.300, 0.300, 1.000 > ]
            [ 0.400 rgb < 0.300, 0.300, 1.000 > ]
            [ 0.500 rgb < 0.196, 0.196, 0.800 > ]
            [ 0.500 rgb < 0.196, 0.196, 0.800 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.610 > ]
            [ 0.700 rgb < 0.000, 0.000, 0.610 > ]
            [ 1.000 rgb < 0.196, 0.196, 0.800 > ]
        }
    }
    finish {
        diffuse 1
        ambient < 0.000, 0.000, 0.000 >
    }
}


#declare Pierre1 = texture {
    pigment {
        agate
        color_map {
            [ 0.10 rgb < 0.520, 0.780, 0.520 > ]
            [ 0.20 rgb < 0.520, 0.520, 0.400 > ]
            [ 0.30 rgb < 0.270, 0.400, 0.020 > ]
            [ 0.40 rgb < 0.270, 0.650, 0.520 > ]
        }
        agate_turb 0.3
    }
    finish {
        diffuse 0.60
        phong 0.20
        phong_size 5
    }
}


#declare Pierre10 = texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.130, 0.150, 0.180 > ]
            [ 0.80 rgb < 0.250, 0.280, 0.200 > ]
            [ 0.85 rgb < 0.150, 0.200, 0.220 > ]
        }
        scale < 0.600, 2.000, 0.500 >
        rotate < 0.000, 0.000, -60.000 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .4
        reflection < 0.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.550, 0.600, 0.620 > ]
            [ 0.10 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < -30.000, 0.000, 50.000 >
        scale < 1.500, 4.400, 1.400 >
    }
    finish {
        phong .4
        phong_size 30
    }
}


#declare Pierre11 = texture {
    pigment {
        granite
        color_map {
            [ .00 rgb < 0.400, 0.520, 0.450 > ]
            [ .70 rgb < 0.320, 0.520, 0.620 > ]
            [ .75 rgb < 0.320, 0.300, 0.280 > ]
            [ .95 rgb < 0.420, 0.500, 0.580 > ]
        }
        scale< 0.240, 0.240, 0.320 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .7
        reflection < 0.200, 0.200, 0.200 >
        phong .6
        phong_size 120
    }
    normal {
        bumps .05
        scale < 0.012, 0.012, 0.012 >
    }
}


#declare Pierre12 = texture {
    pigment {
        granite
        color_map {
            [ .00 rgb < 0.120, 0.120, 0.120 > ]
            [ .90 rgb < 0.120, 0.220, 0.120 > ]
            [ .95 rgb < 0.450, 0.580, 0.480 > ]
        }
        rotate < 30.000, 0.000, 50.000 >
        scale < 2.000, 4.000, 2.000 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .90
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.400, 0.480, 0.420 > ]
            [ 0.30 rgbf < 1.000, 1.000, 1.000, 0.700 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < -30.000, 0.000, 50.000 >
        scale < 1.500, 2.200, 1.400 >
    }
    finish {
        phong .9
        phong_size 90
    }
    normal {
        wrinkles .3
        scale < 0.200, 0.200, 0.200 >
    }
    scale < 0.800, 0.800, 0.800 >
}


#declare Pierre13 = texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgb < 0.098, 0.098, 0.196 > ]
            [ 0.228 rgb < 0.255, 0.255, 0.349 > ]
            [ 0.325 rgb < 0.302, 0.302, 0.416 > ]
            [ 0.447 rgb < 0.255, 0.255, 0.349 > ]
            [ 0.535 rgb < 0.149, 0.149, 0.259 > ]
            [ 0.623 rgb < 0.094, 0.094, 0.196 > ]
            [ 0.816 rgb < 0.294, 0.294, 0.408 > ]
            [ 1.000 rgb < 0.098, 0.098, 0.196 > ]
        }
        rotate < 0.000, 0.000, -40.000 >
        scale < 2.000, 4.000, 2.000 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .90
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.788, 0.761, 0.676 > ]
            [ 0.15 rgbf < 0.788, 0.761, 0.676, 0.700 > ]
            [ 0.30 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 0.000, 0.000, 30.000 >
        scale < 0.800, 1.600, 0.800 >
    }
    finish {
        phong .9
        phong_size 90
    }
}


#declare Pierre14 = texture {
    pigment {
        bozo
        color_map {
            [ 0.00 rgb < 0.235, 0.035, 0.027 > ]
            [ 0.12 rgb < 0.545, 0.255, 0.184 > ]
            [ 0.43 rgb < 0.816, 0.537, 0.027 > ]
            [ 0.63 rgb < 1.000, 1.000, 0.588 > ]
            [ 0.91 rgb < 0.839, 0.604, 0.071 > ]
        }
        rotate < -25.000, 0.000, 0.000 >
        scale < 3.800, 1.800, 2.100 >
        lambda 6
        octaves 4
        turbulence < 0.500, 0.500, 0.500 >
        omega .55
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .90
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgbf < 0.235, 0.035, 0.027, 0.000 > ]
            [ 0.19 rgbf < 0.465, 0.275, 0.184, 0.400 > ]
            [ 0.25 rgbf < 0.716, 0.537, 0.027, 0.700 > ]
            [ 0.65 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 25.000, 0.000, 0.000 >
        scale < 4.200, 2.200, 2.300 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .90
        phong .9
        phong_size 90
    }
}


#declare Pierre15 = texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.300, 0.645, 0.715 > ]
            [ 0.35 rgb < 0.239, 0.235, 0.239 > ]
            [ 1.00 rgb < 0.239, 0.235, 0.239 > ]
        }
        rotate < 0.000, 0.000, -30.000 >
        scale < 3.100, 4.800, 2.800 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .90
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgbf < 0.452, 0.940, 0.960, 0.100 > ]
            [ 0.20 rgbf < 0.165, 0.745, 0.867, 0.800 > ]
            [ 0.30 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.60 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.85 rgbf < 0.330, 0.330, 0.330, 0.400 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 0.000, 0.000, 50.000 >
        scale < 2.400, 4.800, 1.800 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .60
        phong .9
        phong_size 90
    }
}


#declare Pierre2 = texture {
    pigment {
        agate
        color_map {
            [ 0.10 rgb < 0.780, 0.780, 0.780 > ]
            [ 0.60 rgb < 0.890, 0.520, 0.270 > ]
            [ 0.60 rgb < 0.600, 0.470, 0.320 > ]
            [ 0.68 rgb < 0.270, 0.270, 0.300 > ]
            [ 0.72 rgb < 0.560, 0.400, 0.440 > ]
            [ 1.00 rgb < 0.520, 0.520, 0.560 > ]
        }
        agate_turb 0.80
        lambda 7
        omega 0.6
    }
    normal {
        bumps 0.30
        turbulence < 0.400, 0.400, 0.400 >
        scale < 0.200, 0.200, 0.200 >
    }
    finish {
        diffuse 0.40
        ambient < 0.200, 0.200, 0.200 >
        reflection < 0.300, 0.300, 0.300 >
    }
    scale < 0.800, 0.800, 0.800 >
}


#declare Pierre3 = texture {
    pigment {
        onion
        color_map {
            [ 0.10 rgb < 0.400, 0.400, 0.650 > ]
            [ 0.40 rgb < 0.400, 0.520, 0.400 > ]
            [ 0.50 rgb < 0.270, 0.400, 0.780 > ]
            [ 1.00 rgb < 0.520, 0.520, 1.000 > ]
        }
        lambda 3
        omega 0.80
        turbulence < 0.800, 0.800, 0.800 >
    }
    normal {
        dents 0.80
        turbulence < 0.700, 0.700, 0.700 >
        scale < 0.050, 0.050, 0.050 >
    }
    finish {
        diffuse 0.60
        ambient < 0.200, 0.200, 0.200 >
        reflection < 0.300, 0.300, 0.300 >
        phong 0.20
        phong_size 2.0
        specular 0.10
        roughness 0.08
    }
    scale < 0.200, 0.200, 0.200 >
}


#declare Pierre4 = texture {
    pigment {
        marble
        color_map {
            [ 0.10 rgb < 0.520, 0.520, 0.520 > ]
            [ 1.00 rgb < 0.520, 0.400, 0.780 > ]
        }
        rotate < 10.000, 0.000, 40.000 >
        scale < 1.000, 3.000, 2.000 >
        lambda 8
        omega 0.80
        turbulence < 0.800, 0.800, 0.800 >
    }
    normal {
        wrinkles 0.40
        turbulence < 0.800, 0.800, 0.800 >
        scale < 0.080, 0.080, 0.080 >
    }
    finish {
        diffuse 0.60
        reflection < 0.100, 0.100, 0.100 >
        phong 0.80
        phong_size 2
    }
}


#declare Pierre5 = texture {
    pigment {
        bozo
        color_map {
            [ 0.00 rgbf < 0.300, 0.950, 0.500, 0.500 > ]
            [ 0.20 rgbf < 0.200, 0.780, 0.300, 0.650 > ]
            [ 0.80 rgbf < 0.180, 0.620, 0.100, 0.700 > ]
        }
        frequency 5.0
        turbulence < 0.200, 0.200, 0.200 >
    }
    finish {
        diffuse 0.40
        ambient < 0.100, 0.100, 0.100 >
        reflection < 0.400, 0.400, 0.400 >
        refraction 1.0
        phong 0.80
        phong_size 5
        specular 0.20
    }
}


#declare Pierre8 = texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.700, 0.600, 0.300 > ]
            [ 0.20 rgb < 0.400, 0.400, 0.400 > ]
            [ 0.90 rgb < 0.270, 0.270, 0.270 > ]
            [ 1.00 rgb < 0.050, 0.050, 0.050 > ]
        }
        scale < 0.060, 0.060, 0.060 >
    }
    finish {
        diffuse 0.50
        ambient < 0.200, 0.200, 0.200 >
        reflection < 0.000, 0.000, 0.000 >
        phong 0.30
        phong_size 30
    }
    normal {
        bumps .6
        scale < 0.050, 0.050, 0.050 >
    }
}


#declare Pierre9 = texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.630, 0.610, 0.650 > ]
            [ 0.70 rgb < 0.550, 0.580, 0.630 > ]
            [ 0.85 rgb < 0.230, 0.210, 0.280 > ]
        }
        rotate < 0.000, 0.000, -45.000 >
        scale < 2.500, 0.800, 0.700 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .4
        reflection < 0.000, 0.000, 0.000 >
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.00 rgb < 0.170, 0.290, 0.280 > ]
            [ 0.10 rgbf < 1.000, 1.000, 1.000, 0.800 > ]
            [ 0.40 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 0.65 rgbf < 1.000, 1.000, 1.000, 0.600 > ]
            [ 0.73 rgb < 0.160, 0.200, 0.280 > ]
            [ 0.80 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 0.000, 0.000, 40.000 >
        scale < 3.500, 2.800, 3.200 >
    }
    finish {
        phong .4
        phong_size 30
    }
    normal {
        bumps .3
        scale < 0.200, 0.200, 0.200 >
    }
}

#declare Black_Sky = texture {
    pigment {
        bozo
        color_map {
            [ 0.00 rgb < 0.000, 0.000, 0.000 > ]
            [ 0.65 rgb < 0.020, 0.020, 0.050 > ]
            [ 0.80 rgb < 0.100, 0.100, 0.100 > ]
            [ 1.00 rgb < 0.030, 0.030, 0.030 > ]
        }
        scale < 1.000, 6.000, 6.000 >
        turbulence < 0.400, 0.400, 0.400 >
    }
    finish {
        ambient < 1.000, 1.000, 1.000 >
        diffuse 0.0
    }
    normal {
        bumps .2
        scale < 0.300, 0.300, 0.300 >
    }
}
texture {
    pigment {
        gradient x
        color_map {
            [ 0.0 rgbf < 0.100, 0.100, 0.100, 0.500 > ]
            [ 0.5 rgbf < 0.600, 0.600, 0.600, 1.000 > ]
        }
        scale < 8.000, 8.000, 8.000 >
        turbulence < 0.500, 0.500, 0.500 >
    }
    finish {
        ambient < 1.000, 1.000, 1.000 >
        diffuse 0.0
    }
}


#declare Book_Texture = texture {
    pigment {
        marble
        color_map {
            [ .00 rgb < 0.000, 0.250, 0.250 > ]
            [ .90 rgb < 0.000, 0.250, 0.250 > ]
            [ .00 rgb < 0.400, 0.300, 0.100 > ]
        }
        scale < 0.120, 0.120, 0.120 >
        turbulence < 0.600, 0.600, 0.600 >
    }
    normal {
        bumps 0.6
        scale < 0.050, 0.050, 0.050 >
        turbulence < 0.600, 0.600, 0.600 >
    }
    finish {
        ambient < 0.100, 0.100, 0.100 >
        diffuse .3
        reflection < 0.000, 0.000, 0.000 >
        phong .3
        phong_size 8
    }
}


#declare Candelar_Texture = texture {
    pigment {
        color rgb < 1.000, 0.800, 0.460 >
    }
    finish {
        ambient < 0.110, 0.110, 0.110 >
        diffuse 0.43
        brilliance 2
        reflection < 0.300, 0.300, 0.300 >
        specular 0.2
        roughness 0.02
        phong 0.53
        phong_size 5
        metallic
    }
    normal {
        dents .9
        scale < 0.030, 0.030, 0.030 >
    }
}


#declare Cloth_Texture = texture {
    pigment {
        color rgb < 0.900, 0.300, 0.200 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.6
        phong .40
        phong_size 15
    }
}
texture {
    pigment {
        marble
        color_map {
            [ 0.00 rgb < 0.900, 0.680, 0.400 > ]
            [ 0.30 rgb < 0.900, 0.500, 0.500 > ]
            [ 0.30 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < 45.000, 0.000, 0.000 >
        scale < 0.100, 0.100, 5.000 >
        turbulence < 0.200, 0.200, 0.200 >
    }
    normal {
        bumps .6
        scale < 0.050, 0.050, 0.050 >
    }
}
texture {
    pigment {
        marble
        color_map {
            [ 0.00 rgb < 0.900, 0.680, 0.400 > ]
            [ 0.30 rgb < 0.900, 0.500, 0.500 > ]
            [ 0.30 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
            [ 1.00 rgbf < 1.000, 1.000, 1.000, 1.000 > ]
        }
        rotate < -45.000, 0.000, 0.000 >
        scale < 0.100, 0.100, 5.000 >
        turbulence < 0.200, 0.200, 0.200 >
    }
    normal {
        bumps .6
        scale < 0.050, 0.050, 0.050 >
    }
}


#declare Flare_Texture = texture {
    pigment {
        color rgbf < 1.000, 1.000, 1.000, 1.000 >
    }
    finish {
        ambient < 0.000, 0.000, 0.000 >
        reflection < 0.000, 0.000, 0.000 >
        refraction 1.0
        specular .8
        roughness 0.025
    }
}


#declare Page_Texture = texture {
    pigment {
        bozo
        color_map {
            [ .00 rgb < 0.900, 0.900, 0.800 > ]
            [ .90 rgb < 0.800, 0.700, 0.400 > ]
            [ .90 rgb < 0.550, 0.500, 0.400 > ]
        }
        scale< 1.600, 1.600, 0.039 >
        turbulence < 0.050, 0.050, 0.050 >
    }
    finish {
        ambient < 0.150, 0.150, 0.150 >
        diffuse .4
    }
}


#declare Parafin = texture {
    pigment {
        color rgb < 0.850, 0.100, 0.100 >
    }
    finish {
        ambient < 0.300, 0.300, 0.300 >
        diffuse 0.60
        phong 0.30
        phong_size 2
        brilliance 2
    }
    normal {
        bumps .5
        scale < 0.050, 0.050, 0.300 >
    }
}


#declare S_Wood1 = texture {
    pigment {
        wood
        color_map {
            [ 0.00 rgb < 0.890, 0.600, 0.300 > ]
            [ 0.10 rgb < 0.890, 0.600, 0.300 > ]
            [ 0.90 rgb < 0.600, 0.400, 0.200 > ]
            [ 1.00 rgb < 0.600, 0.400, 0.200 > ]
        }
        rotate < 2.000, 90.000, 1.000 >
        scale < 0.200, 0.200, 1.100 >
        turbulence < 0.090, 0.090, 0.090 >
        octaves 3
    }
    finish {
        ambient < 0.300, 0.300, 0.300 >
        diffuse .50
        reflection < 0.000, 0.000, 0.000 >
        specular .10
        roughness 0.002
        phong .40
        phong_size 16
    }
}


#declare S_Wood2 = texture {
    pigment {
        wood
        color_map {
            [ 0.00 rgb < 0.270, 0.220, 0.000 > ]
            [ 0.80 rgb < 0.350, 0.240, 0.130 > ]
            [ 1.00 rgb < 0.650, 0.400, 0.140 > ]
        }
        rotate < -1.000, 90.000, 1.000 >
        scale < 0.200, 0.200, 1.000 >
        octaves 4
        turbulence < 0.080, 0.080, 0.080 >
        lambda 0.60
    }
    finish {
        ambient < 0.050, 0.050, 0.050 >
        diffuse 0.40
        reflection < 0.050, 0.050, 0.050 >
        specular 0.20
        roughness .04
        phong 0.30
        phong_size 20
        brilliance 2
    }
}


#declare S_Wood3 = texture {
    pigment {
        wood
        color_map {
            [ 0.10 rgb < 0.270, 0.020, 0.000 > ]
            [ 0.80 rgb < 0.350, 0.140, 0.130 > ]
            [ 1.00 rgb < 0.650, 0.400, 0.140 > ]
        }
        rotate < -1.000, 90.000, 2.000 >
        scale < 0.200, 0.200, 1.000 >
        octaves 4
        turbulence < 0.080, 0.080, 0.080 >
        lambda 0.60
    }
    finish {
        ambient < 0.050, 0.050, 0.050 >
        diffuse 0.40
        specular 0.10
        roughness .4
    }
}


#declare S_Wood4 = texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.561, 0.416, 0.282 > ]
            [ 0.050 rgb < 0.788, 0.588, 0.369 > ]
            [ 0.983 rgb < 0.647, 0.502, 0.396 > ]
        }
        rotate < -1.000, 90.000, 0.000 >
        scale < 0.200, 0.200, 1.000 >
        octaves 4
        turbulence < 0.060, 0.060, 0.060 >
        lambda 0.60
    }
    finish {
        ambient < 0.050, 0.050, 0.050 >
        diffuse 0.40
        reflection < 0.000, 0.000, 0.000 >
        specular 0.10
        roughness .4
    }
}


#declare S_Wood5 = texture {
    pigment {
        wood
        color_map {
            [ 0.000 rgb < 0.561, 0.416, 0.282 > ]
            [ 0.050 rgb < 0.788, 0.588, 0.369 > ]
            [ 0.983 rgb < 0.647, 0.502, 0.396 > ]
        }
        rotate < 2.000, 90.000, 2.000 >
        scale < 0.200, 0.200, 1.000 >
        octaves 4
        turbulence < 0.090, 0.090, 0.090 >
        lambda 0.60
    }
    finish {
        ambient < 0.050, 0.050, 0.050 >
        diffuse 0.40
        reflection < 0.200, 0.200, 0.200 >
        specular 0.10
        roughness .4
    }
}
texture {
    pigment {
        granite
        color_map {
            [ 0.000 rgbf < 0.204, 0.055, 0.000, 0.000 > ]
            [ 0.059 rgbf < 0.412, 0.227, 0.020, 0.200 > ]
            [ 0.378 rgbf < 0.667, 0.467, 0.325, 0.880 > ]
            [ 0.748 rgbf < 0.208, 0.212, 0.200, 0.920 > ]
            [ 1.000 rgbf < 0.204, 0.055, 0.000, 0.950 > ]
        }
        rotate < 0.000, 20.000, 50.000 >
        scale < 1.400, 1.400, 1.600 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse .60
        phong .3
        phong_size 45
    }
}


#declare Shiny_Gold = texture {
    pigment {
        color rgb < 0.900, 0.680, 0.300 >
    }
    finish {
        metallic
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.6
        brilliance 4
        reflection < 0.250, 0.250, 0.250 >
        phong 0.75
        phong_size 80
    }
}


#declare FZhulltex1 = texture {
    pigment {
        gradient z
        color_map {
            [ 0.000 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.330 rgb < 0.000, 0.000, 1.000 > ]
            [ 0.330 rgb < 0.000, 0.000, 1.000 > ]
            [ 0.660 rgb < 0.900, 0.900, 0.900 > ]
            [ 0.660 rgb < 0.900, 0.900, 0.900 > ]
            [ 1.001 rgb < 0.900, 0.900, 0.900 > ]
        }
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.50
        brilliance 1.00
        phong 0.60
        phong_size 20.00
        crand 0.001
    }
    scale < 40.000, 40.000, 40.000 >
    translate < 0.000, 0.000, 0.500 >
}


#declare FZhulltex2 = texture {
    pigment
    {
    gradient z
            color_map
        {
            [ 0.000 rgb < 1.000, 0.000, 0.000 > ]
            [ 0.330 rgb < 0.000, 0.000, 1.000 > ]
            [ 0.330 rgb < 0.000, 0.000, 1.000 > ]
            [ 0.660 rgb < 0.900, 0.900, 0.900 > ]
            [ 0.660 rgb < 0.900, 0.900, 0.900 > ]
            [ 1.001 rgb < 0.900, 0.900, 0.900 > ]
        }
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.50
        brilliance 1.00
        phong 0.60
        phong_size 20.00
        crand 0.001
    }
}


#declare FZhulltex3 = texture {
    pigment {
        color rgb < 0.900, 0.900, 0.900 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.50
        brilliance 1.00
        phong 0.60
        phong_size 20.00
        crand 0.001
    }
}


#declare FZhulltex4 = texture {
    pigment {
        color rgb < 0.900, 0.900, 0.900 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.50
        brilliance 1.00
        phong 0.60
        phong_size 20.00
        crand 0.001
    }
}


#declare Leather = texture {
    pigment {
        color rgb < 0.360, 0.250, 0.200 >
    }
    finish {
        ambient < 0.200, 0.200, 0.200 >
        diffuse 0.50
        brilliance 1.00
        phong 0.60
        phong_size 50.00
        crand 0.100
    }
}




#declare Wthr_Wood = texture {
  pigment {
    marble
    color_map {
        [ 0.000 rgb < 0.380, 0.350, 0.370 > ]
        [ 0.200 rgb < 0.380, 0.350, 0.370 > ]
        [ 0.200 rgb < 0.380, 0.350, 0.370 > ]
        [ 0.400 rgb < 0.400, 0.370, 0.390 > ]
        [ 0.400 rgb < 0.400, 0.370, 0.390 > ]
        [ 0.600 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.600 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.800 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.800 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.900 rgb < 0.400, 0.370, 0.390 > ]
        [ 0.900 rgb < 0.400, 0.370, 0.390 > ]
        [ 1.010 rgb < 0.380, 0.350, 0.370 > ]
    }
    turbulence < 0.250, 0.250, 0.250 >
  }
  finish {
    diffuse 0.3
  }
  scale < 13.000, 13.000, 1.000 >
}
texture { 
  pigment {
    wood
    color_map {
        [ 0.000 rgb < 0.290, 0.210, 0.270 > ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.7 ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.7 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 1.0 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 1.0 ]
        [ 0.700 rgb < 0.300, 0.240, 0.290 > filter 0.8 ]
        [ 0.700 rgb < 0.200, 0.140, 0.190 > filter 0.7 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 1.0 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 1.0 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 0.7 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 1.0 ]
        [ 1.010 rgb < 0.150, 0.110, 0.160 > filter 0.7 ]
    }
    turbulence < 0.300, 0.300, 0.300 > 
  }
  finish {
    diffuse 0.3
  }
  scale < 5.000, 15.000, 5.000 >
}
texture { 
    pigment {
    wood
    color_map {
        [ 0.000 rgb < 0.290, 0.210, 0.270 > ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.4 ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.4 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 0.8 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 0.8 ]
        [ 0.700 rgb < 0.300, 0.240, 0.290 > filter 0.9 ]
        [ 0.700 rgb < 0.200, 0.140, 0.190 > filter 0.8 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 0.6 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 0.6 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 0.4 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 0.5 ]
        [ 1.010 rgb < 0.150, 0.110, 0.160 > filter 0.8 ]
    }
    turbulence < 0.200, 0.200, 0.200 >
  }
  finish { 
    diffuse 0.2
  }
  scale < 4.000, 20.000, 4.000 >
}


#declare Zeb_Wood = texture {
  pigment {
    marble
    color_map {
        [ 0.000 rgb < 0.380, 0.350, 0.370 > ]
        [ 0.200 rgb < 0.380, 0.350, 0.370 > ]
        [ 0.200 rgb < 0.380, 0.350, 0.370 > ]
        [ 0.400 rgb < 0.400, 0.370, 0.390 > ]
        [ 0.400 rgb < 0.400, 0.370, 0.390 > ]
        [ 0.600 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.600 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.800 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.800 rgb < 0.360, 0.310, 0.340 > ]
        [ 0.900 rgb < 0.400, 0.370, 0.390 > ]
        [ 0.900 rgb < 0.400, 0.370, 0.390 > ]
        [ 1.010 rgb < 0.380, 0.350, 0.370 > ]
    }
    turbulence < 0.250, 0.250, 0.250 >
  }
  finish {
    ambient 0.300
    diffuse 1.2
  }
  scale < 13.000, 13.000, 1.000 >
}
texture {
  pigment {
    wood
    color_map {
        [ 0.000 rgb < 0.290, 0.210, 0.270 > ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.7 ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.7 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 1.0 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 1.0 ]
        [ 0.700 rgb < 0.300, 0.240, 0.290 > filter 0.8 ]
        [ 0.700 rgb < 0.200, 0.140, 0.190 > filter 0.7 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 1.0 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 1.0 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 0.7 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 1.0 ]
        [ 1.010 rgb < 0.150, 0.110, 0.160 > filter 0.7 ]
    }
    turbulence < 0.300, 0.300, 0.300 >
  }
  finish {
    ambient < 0.600, 0.600, 0.600 >
    diffuse 0.9
  }
  scale < 5.000, 15.000, 5.000 >
}
texture {
  pigment {
    wood
    color_map {
        [ 0.000 rgb < 0.290, 0.210, 0.270 > ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.4 ]
        [ 0.050 rgb < 0.290, 0.210, 0.270 > filter 0.4 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 0.8 ]
        [ 0.200 rgb < 0.300, 0.240, 0.290 > filter 0.8 ]
        [ 0.700 rgb < 0.300, 0.240, 0.290 > filter 0.9 ]
        [ 0.700 rgb < 0.200, 0.140, 0.190 > filter 0.8 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 0.6 ]
        [ 0.800 rgb < 0.200, 0.140, 0.190 > filter 0.6 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 0.4 ]
        [ 0.900 rgb < 0.200, 0.140, 0.190 > filter 0.5 ]
        [ 1.010 rgb < 0.150, 0.110, 0.160 > filter 0.8 ]
    }
    turbulence < 0.200, 0.200, 0.200 >
  }
  finish {
    diffuse 0.9
    ambient < 0.300, 0.300, 0.300 >
  }
  scale < 4.000, 20.000, 4.000 >
}

//***************************************************************************
// Nebulaes.  Hazy looking, and based on a Veil nebula effect.
// Created by Gautam N. Lad.  July 1997.
// CONTACTING GAUTAM N. LAD
//   E-Mail  - gautam@interlog.com
//   Website - http://www.interlog.com/~gautam
//***************************************************************************

#declare NebulaTex1 = texture {
  pigment { bozo
    color_map {
      [0.8 rgb 1.5 transmit 1]
      [1.0 rgb 0.9 transmit 1.1]
      }
      turbulence .8
  } 
  halo {
    emitting
    spherical_mapping
    linear
    color_map {
      [ 0   rgbt < 1,  0,  1,  1>]
      [.888 rgbt < 0,  1,  1, .4882>]
      [ 1   rgbt < 1,  0,  1,-.2882>]
      }
    turbulence .96
    samples 4
  }
}

#declare NebulaTex2 = texture {
  pigment { bozo
    color_map {
      [0.1 rgb 1.1 transmit 1]
      [0.8 rgb 1.5 transmit 1]
      [1.0 rgb 0.9 transmit 1.1]
      }
      ramp_wave
    }

  halo {
    emitting
    spherical_mapping
    linear
    color_map {
      [ 0   rgbt <.5,  0, .5,  1>]
      [.288 rgbt < 1,  0,  0, .8>]
      [.888 rgbt < 2,  1,  1, .4>]
      [ 1   rgbt < 2,  1,  1,-.3>]
      }
    turbulence .8
    samples 4
    }
}


#declare NebulaTex3 = texture {
  pigment { bozo
    color_map {
      [0.8 rgb 1.5 transmit 1]
      [1.0 rgb 0.9 transmit 1.1]
      }
    } 
  halo {
    emitting
    spherical_mapping
    linear
    color_map {
      [ 0   rgbt < 1,  0, .5,  1>]
      [.1   rgbt < 1,  0,  0, .688>]
      [.2   rgbt < 1,  0, .3, .788>]
      [.3   rgbt < 1,  0, .3,  1>]
      [.4   rgbt < 1,  0, .3, .788>]
      [.7   rgbt < 2,  0,  1, .688>]
      [.9   rgbt < 0,  1, .3,  1>]
      [ 1   rgbt < 0,  1, .1, .7882>]
      }
    turbulence .81
    samples 4
    }
}


// End of file
