
#global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"

camera {
  location <0, 8, -50>
  right <4/3, 0, 0>
  up <0, 1, 0>
  sky <0, 1, 0>
  angle 20
  look_at <0, 4, 0>
}

light_source { <20, 50, -100> color White }

background { color SkyBlue }

#declare P =
polygon {
  12,
  <0, 0>, <0, 6>, <4, 6>, <4, 3>, <1, 3>, <1, 0>, <0, 0>,
  <1, 4>, <1, 5>, <3, 5>, <3, 4>, <1, 4>
}

#declare O =
polygon {
  10,
  <0, 0>, <0, 6>, <4, 6>, <4, 0>, <0, 0>,
  <1, 1>, <1, 5>, <3, 5>, <3, 1>, <1, 1>
}

#declare V =
polygon {
  8,
  <1, 0>, <0, 6>, <1, 6>, <2, 1>, <3, 6>, <4, 6>, <3, 0>, <1, 0>
}

union {
  object { P translate -5*x }
  object { O translate 0*x }
  object { V translate 5*x }
  pigment { colour Red }
  finish { ambient .3 diffuse .6 }
  translate -3*x
}

plane { y, 0
  pigment { color Green }
}

