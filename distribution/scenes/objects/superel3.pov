// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// POV-Ray 3.0 sample data file by Dieter Bayer, Nov. 1994
// This scene shows a number of superellipsoids.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7;
global_settings { assumed_gamma 1.0 }
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
  orthographic
  location <0, 0, -10>
  right 25 * 4/3 * x
  up 25 * y
  look_at <0, 0, 0>
}

light_source { <50, 50, -100> color red 0.7 green 0.7 blue 0.7 }
light_source { <0, 0, -10000> color red 0.7 green 0.7 blue 0.7 }

/*
plane { <0, 0, 1>, 2
  hollow on
  pigment { checker color red 0 green 1 blue 0 color red 0 green 0 blue 1 }
  finish { ambient 0.1 diffuse 0.4 }
}
*/
background { color rgb<1,1,1>*0.35 } 


#declare Row1 = +10;
#declare Row2 =  +5;
#declare Row3 =   0;
#declare Row4 =  -5;
#declare Row5 = -10;
#declare Col1 = -10;
#declare Col2 =  -5;
#declare Col3 =   0;
#declare Col4 =  +5;
#declare Col5 = +10;

#declare Tex =
texture {
  pigment { color red 1 green 0 blue 0 }
  finish { ambient 0.2 diffuse 0.4 phong 0.5 phong_size 5 }
}

superellipsoid { <0.3, 0.3> scale 2 rotate <-15, 30, 0> translate <Col1, Row1, 0> texture { Tex } }

superellipsoid { <0.3, 0.5> scale 2  rotate <-15, 30, 0> translate <Col2, Row1, 0> texture { Tex } }

superellipsoid { <0.3, 1.0> scale 2  rotate <-15, 30, 0> translate <Col3, Row1, 0> texture { Tex } }

superellipsoid { <0.3, 2.0> scale 2  rotate <-15, 30, 0> translate <Col4, Row1, 0> texture { Tex } }

superellipsoid { <0.3, 3.0> scale 2  rotate <-15, 30, 0> translate <Col5, Row1, 0> texture { Tex } }

superellipsoid { <0.5, 0.3> scale 2  rotate <-15, 30, 0> translate <Col1, Row2, 0> texture { Tex } }

superellipsoid { <0.5, 0.5> scale 2  rotate <-15, 30, 0> translate <Col2, Row2, 0> texture { Tex } }

superellipsoid { <0.5, 1.0> scale 2  rotate <-15, 30, 0> translate <Col3, Row2, 0> texture { Tex } }

superellipsoid { <0.5, 2.0> scale 2  rotate <-15, 30, 0> translate <Col4, Row2, 0> texture { Tex } }

superellipsoid { <0.5, 3.0> scale 2  rotate <-15, 30, 0> translate <Col5, Row2, 0> texture { Tex } }

superellipsoid { <1.0, 0.3> scale 2  rotate <-15, 30, 0> translate <Col1, Row3, 0> texture { Tex } }

superellipsoid { <1.0, 0.5> scale 2  rotate <-15, 30, 0> translate <Col2, Row3, 0> texture { Tex } }

superellipsoid { <1.0, 1.0> scale 2  rotate <-15, 30, 0> translate <Col3, Row3, 0> texture { Tex } }

superellipsoid { <1.0, 2.0> scale 2  rotate <-15, 30, 0> translate <Col4, Row3, 0> texture { Tex } }

superellipsoid { <1.0, 3.0> scale 2  rotate <-15, 30, 0> translate <Col5, Row3, 0> texture { Tex } }

superellipsoid { <2.0, 0.3> scale 2  rotate <-15, 30, 0> translate <Col1, Row4, 0> texture { Tex } }

superellipsoid { <2.0, 0.5> scale 2  rotate <-15, 30, 0> translate <Col2, Row4, 0> texture { Tex } }

superellipsoid { <2.0, 1.0> scale 2  rotate <-15, 30, 0> translate <Col3, Row4, 0> texture { Tex } }

superellipsoid { <2.0, 2.0> scale 2  rotate <-15, 30, 0> translate <Col4, Row4, 0> texture { Tex } }

superellipsoid { <2.0, 3.0> scale 2  rotate <-15, 30, 0> translate <Col5, Row4, 0> texture { Tex } }

superellipsoid { <3.0, 0.3> scale 2  rotate <-15, 30, 0> translate <Col1, Row5, 0> texture { Tex } }

superellipsoid { <3.0, 0.5> scale 2  rotate <-15, 30, 0> translate <Col2, Row5, 0> texture { Tex } }

superellipsoid { <3.0, 1.0> scale 2  rotate <-15, 30, 0> translate <Col3, Row5, 0> texture { Tex } }

superellipsoid { <3.0, 2.0> scale 2  rotate <-15, 30, 0> translate <Col4, Row5, 0> texture { Tex } }

superellipsoid { <3.0, 3.0> scale 2  rotate <-15, 30, 0> translate <Col5, Row5, 0> texture { Tex } }

