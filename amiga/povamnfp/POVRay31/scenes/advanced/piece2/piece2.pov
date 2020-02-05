// Persistence Of Vision raytracer version 3.1 sample file.

// piece 2:
//          by Truman Brown 11/91

//          Close-up of a museum piece in the Woild Museum

// NOTE: The following comment by Truman is no longer relevant.  The
// gamma has been modified by the assumed_gamma setting below.  -dmf
//
//   The lighting is intentionally dim when the image is rendered.
//   You can enhance the image using PicLab's gamma, brightness, and
//   contrast command when you post-process the image.


global_settings { assumed_gamma 0.8 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

#include "light.inc"
#include "ttexture.inc"
#include "tori2.inc"
#include "spural1.inc"
#include "spural2.inc"

object { light3 translate < -43,  14,  -80>  }
object { light3 translate < 134, 223,  -20>  }
object { light4 translate <  28,  88, -259>  }

camera {
   location  < 15.0, 23.0, -55.0>
   direction <  0.0,  0.0,   2.0  >
   up        <  0.0,  1.0,   0.0  >
   right     <  4/3,  0.0,   0.0  >
   look_at   <  0.0, -0.5,   0.0  >
}

#declare orb =
union {
   object { torus7 translate  9.510565*y  texture { oak  }  }
   object { torus5 translate  8.090170*y  texture { oak  }  }
   object { torus3 translate  5.877853*y  texture { oak  }  }
   object { torus1 translate  3.090170*y  texture { oak  }  }
   object { torusx translate  0.000000*y  texture { oak  }  }
   object { torus1 translate -3.090170*y  texture { oak  }  }
   object { torus3 translate -5.877853*y  texture { oak  }  }
   object { torus5 translate -8.090170*y  texture { oak  }  }
   object { torus7 translate -9.510565*y  texture { oak  }  }
   sphere { <0, 0, 0> 4.3  texture { pigment { White } finish { ambient 0.0 diffuse 0.1 reflection 0.98 specular 1.0 roughness 0.00001 } } }

   object { torus9 rotate <90,  60, 0> texture { brace_texture } }
   object { torus9 rotate <90, 120, 0> texture { brace_texture } }
   object { torus9 rotate <90, 180, 0> texture { brace_texture } }

   rotate 25*z
}

#declare plate =
intersection {
   object { Cylinder_Y scale <15, 1, 15> }
   plane { y, 0 }
   plane { -y, 1 }

   texture { gilt_texture }
}

#declare plate_border =
intersection {
   object { Cylinder_Y scale <17, 1, 17> }
   object { Cylinder_Y scale <15, 1, 15> inverse }
   plane { y, 0  }
   plane { -y, 1  }

   texture { oak  }
}

object { orb translate 3.0*y }
object { spural1 rotate -28*y translate < 8.9, -12.5, -0.5>  }
object { spural2 rotate 212*y translate <-8.9, -12.5, -0.5>  }

object { plate translate -12.7*y }
object { plate_border translate -12.7*y }
