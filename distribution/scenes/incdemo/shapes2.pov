// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// By Chris Young
// This image contains an example of every shape from SHAPES2.INC
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"
#include "shapes2.inc"

camera {
   location <10, 10, -20>
   direction <0, 0, 1.5>
   up  <0, 1, 0>
   right     x*image_width/image_height
   look_at <2, 0, 0>
}

light_source {<0, 1000, -1000> color LightGray}

light_source {<150, 100, -100> color LightGray}

#declare Col1 =-9;
#declare Col2 =-3;
#declare Col3 =3;
#declare Col4 =8;

#declare Row1 =6;
#declare Row2 =0;
#declare Row3 =-6;

object { Tetrahedron
   pigment {Red}
   translate <Col1, 2, Row1>
}

object { Octahedron
   pigment {Green}
   translate <Col2, 1.8, Row1>
}

object { Dodecahedron
   pigment {Blue}
   translate <Col3, 1.3, Row1>
}

object { Icosahedron
   pigment {Magenta}
   translate <Col4, 1.3, Row1>
}

object { HalfCone_Y
   pigment {Yellow}
   translate <Col1, 1.5, Row2>
}

object { Hexagon
   pigment {Cyan}
   translate <Col2, 1.5, Row2>
}

object { Rhomboid
   pigment {Tan}
   translate <Col3, 1.5, Row2>
}

object { Pyramid
   pigment {Orange}
   translate <Col4, 1.5, Row2>
}

object { Square_X
   pigment {NeonPink}
   translate <Col2, 1, Row3>
}

object { Square_Y
   pigment {Scarlet}
   translate <Col3, 1, Row3>
}

object { Square_Z
   pigment {NeonBlue}
   translate <Col4, 1, Row3>
}

object {
   plane {y, 0}
   pigment {White}
}
