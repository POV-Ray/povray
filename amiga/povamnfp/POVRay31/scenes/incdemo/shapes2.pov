// Persistence Of Vision raytracer version 3.1 sample file.
// By Chris Young
// This image contains an example of every shape from SHAPES2.INC

#version 3.1;
global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"
#include "shapes2.inc"

camera {
   location <10, 10, -20>
   direction <0, 0, 1.5>
   up  <0, 1, 0>
   right <4/3, 0, 0>
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
   bounded_by {sphere{<0, 0, 0>, 3}}
   translate <Col1, 2, Row1>
}

object { Octahedron
   pigment {Green}
   bounded_by {sphere{<0, 0, 0>, 1.8}}
   translate <Col2, 1.8, Row1>
}

object { Dodecahedron
   pigment {Blue}
   bounded_by {sphere{<0, 0, 0>, 1.3}}
   translate <Col3, 1.3, Row1>
}

object { Icosahedron
   pigment {Magenta}
   bounded_by {sphere{<0, 0, 0>, 1.3}}
   translate <Col4, 1.3, Row1>
}

object { HalfCone_Y
   pigment {Yellow}
   bounded_by {sphere{<0, 0, 0>, 1.5}}
   translate <Col1, 1.5, Row2>
}

object { Hexagon
   pigment {Cyan}
   bounded_by {sphere{<0, 0, 0>, 1.5}}
   translate <Col2, 1.5, Row2>
}

object { Rhomboid
   pigment {Tan}
   bounded_by {sphere{<0, 0, 0>, 2.3}}
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
