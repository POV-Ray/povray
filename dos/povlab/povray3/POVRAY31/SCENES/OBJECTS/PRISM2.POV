// Persistence Of Vision raytracer version 3.1 sample file.
// Demo showing several conic prisms ... Dieter Bayer, June 1994
//
// back to front: linear, quadratic, cubic interpolation
// left to right: decreasing "slope" of the conic sweeping
//

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

camera {
  location <80, 80, -160>
  right <4/3, 0, 0>
  up <0, 1, 0>
  sky <0, 1, 0>
  direction <0, 0, 4.5>
  look_at <0, 5, 0>
}

light_source { <40, 40, -80> colour Gray40 }

light_source { <0, 50, 20> colour Gray40 }

light_source { <50, 50, -50> colour Gray40 }

light_source { <-50, 50, -50> colour Gray40 }

background { color SkyBlue }

plane { y, 0
   pigment {
      checker colour Yellow colour Green
      scale 5
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

// Prism with linear interpolation and conic sweeping

#declare Prism1 =
prism {
  linear_spline
  conic_sweep
  0.25,
  1.0,
  11,

  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>

  texture { Brass_Metal }

  translate <0, -0.625, 0>
  scale <6, 6, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with quadratic interpolation and conic sweeping

#declare Prism2 =
prism {
  quadratic_spline
  conic_sweep
  0.25,
  1.0,
  12,

  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>,
  < 0.2,  0.2>

  texture { Brass_Metal }

  translate <0, -0.625, 0>
  scale <6, 6, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with cubic interpolation and conic sweeping

#declare Prism3 =
prism {
  cubic_spline
  conic_sweep
  0.25,
  1.0,
  13,

  <-0.2, -1.0>,
  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>,
  < 0.2,  0.2>

  texture { Brass_Metal }

  translate <0, -0.625, 0>
  scale <6, 6, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with linear interpolation and conic sweeping

#declare Prism4 =
prism {
  linear_spline
  conic_sweep
  0.5,
  1.0,
  11,

  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>

  texture { Brass_Metal }

  translate <0, -0.75, 0>
  scale <6, 9, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with quadratic interpolation and conic sweeping

#declare Prism5 =
prism {
  quadratic_spline
  conic_sweep
  0.5,
  1.0,
  12,

  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>,
  < 0.2,  0.2>

  texture { Brass_Metal }

  translate <0, -0.75, 0>
  scale <6, 9, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with cubic interpolation and conic sweeping

#declare Prism6 =
prism {
  cubic_spline
  conic_sweep
  0.5,
  1.0,
  13,

  <-0.2, -1.0>,
  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>,
  < 0.2,  0.2>

  texture { Brass_Metal }

  translate <0, -0.75, 0>
  scale <6, 9, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with linear interpolation and conic sweeping

#declare Prism7 =
prism {
  linear_spline
  conic_sweep
  0.75,
  1.0,
  11,

  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>

  texture { Brass_Metal }

  translate <0, -0.875, 0>
  scale <6, 18, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with quadratic interpolation and conic sweeping

#declare Prism8 =
prism {
  quadratic_spline
  conic_sweep
  0.75,
  1.0,
  12,

  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>,
  < 0.2,  0.2>

  texture { Brass_Metal }

  translate <0, -0.875, 0>
  scale <6, 18, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

// Prism with cubic interpolation and conic sweeping

#declare Prism9 =
prism {
  cubic_spline
  conic_sweep
  0.75,
  1.0,
  13,

  <-0.2, -1.0>,
  < 0.2, -1.0>, < 0.2,  0.2>, < 1.0, -0.2>, < 1.0,  0.2>, < 0.2,  1.0>, 
  <-0.2,  1.0>, <-1.0,  0.2>, <-1.0, -0.2>, <-0.2,  0.2>, <-0.2, -1.0>,
  < 0.2, -1.0>,
  < 0.2,  0.2>

  texture { Brass_Metal }

  translate <0, -0.875, 0>
  scale <6, 18, 6>
  rotate <90, 0, 180>
  translate <0, 4.5, 0>
}

object { Prism1 translate <-15, 4, 15> }

object { Prism2 translate <-15, 4, 0> }

object { Prism3 translate <-15, 4, -15> }

object { Prism4 translate <0, 4, 15> }

object { Prism5 translate <0, 4, 0> }

object { Prism6 translate <0, 4, -15> }

object { Prism7 translate <15, 4, 15> }

object { Prism8 translate <15, 4, 0> }

object { Prism9 translate <15, 4, -15> }


