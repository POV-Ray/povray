// Persistence Of Vision raytracer version 3.1 sample file.
// Render this file to create CRAT_DAT.TGA and then render CRATER.POV
// Use special 16-bit gray output

global_settings { assumed_gamma 2.2 hf_gray_16 }

#include "colors.inc"

// a wrinkle colored plane

plane {z,10 
 hollow on
 pigment{wrinkles
  color_map{
   [0 White*0.3]
   [1 White]
  }
 }
}

// Main spotlight creates crater mountain
light_source {0 color 1  spotlight point_at z*10
  radius 7 falloff 11
}

// Dim spotlight softens outer edges further
light_source {0 color .25  spotlight point_at z*10
  radius 2 falloff 15
}

// Narrow spotlight creates central peak
light_source {0 color .1  spotlight point_at z*10
  radius 0 falloff 1.3
}

// Negative spotlight cuts out crater insides
light_source {0 color -0.9  spotlight point_at z*10
  radius 5 falloff 9.5
}

// Dim negative spotlight counteracts dim positive light in center
light_source {0 color -.25  spotlight point_at z*10
  radius 3 falloff 8
}

