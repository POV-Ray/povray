// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Demo using the spiral1 texture ... by Dieter Bayer, May 1994
//
// 9 discs with different spiral1 textures.
//
// The scaling factor of the texture, i.e. the number of turns
// one "arm" of the spiral makes, decreases from left to right.
//
// The number of "arms" of the spiral,
// increases from top to bottom.
//
// -w320 -h240
// -w800 -h600 +a0.3
//

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "shapes.inc"

/* factors used for positioning the discs */

#declare X = 2.1;
#declare Y = 2.0;

/* arms used */

#declare O1 = 3;
#declare O2 = 6;
#declare O3 = 9;

/* scaling used */

#declare S1 = 100;
#declare S2 = 1;
#declare S3 = 0.2;

/* Spiral 1 */

#declare Spiral1 =
texture {
  pigment {
    spiral1 O1
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S1, S1, S1>
}

/* Spiral 2 */

#declare Spiral2 =
texture {
  pigment {
    spiral1 O1
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S2, S2, S2>
}

/* Spiral 3 */

#declare Spiral3 =
texture {
  pigment {
    spiral1 O1
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S3, S3, S3>
}

/* Spiral 4 */

#declare Spiral4 =
texture {
  pigment {
    spiral1 O2
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S1, S1, S1>
}

/* Spiral 5 */

#declare Spiral5 =
texture {
  pigment {
    spiral1 O2
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S2, S2, S2>
}

/* Spiral 6 */

#declare Spiral6 =
texture {
  pigment {
    spiral1 O2
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}

  }
  finish { ambient 1 }
  scale <S3, S3, S3>
}

/* Spiral 7 */

#declare Spiral7 =
texture {
  pigment {
    spiral1 O3
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S1, S1, S1>
}

/* Spiral 8 */

#declare Spiral8 =
texture {
  pigment {
    spiral1 O3
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S2, S2, S2>
}

/* Spiral 9 */

#declare Spiral9 =
texture {
  pigment {
    spiral1 O3
    color_map { [ 0.0 color rgb<1,1,1>]
                [ 1.0 color rgb<0.0,0.1,0.3>]}
  }
  finish { ambient 1 }
  scale <S3, S3, S3>
}

camera {
  location <0, 0, -6.5>
  right   x*image_width/image_height
  angle 65 
  look_at <0, 0, 0>
}

background { color rgb<1,1,1>*0.5 }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral1 } translate <-1.5*X, +1*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral2 } translate <-0.5*X, +1*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral3 } translate <+0.5*X, +1*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral4 } translate <-1*X,  0*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral5 } translate < 0*X,  0*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral6 } translate <+1*X,  0*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral7 } translate <-0.5*X, -1*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral8 } translate < 0.5*X, -1*Y, 0> }

disc { <0, 0, 0>, <0, 0, 1>, 1 hollow on texture { Spiral9 } translate <+1.5*X, -1*Y, 0> }

