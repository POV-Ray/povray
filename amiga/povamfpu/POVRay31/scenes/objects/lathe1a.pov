// Persistence Of Vision raytracer version 3.1 sample file.
// Demo showing a lathe with quadratic interpolation ... Dieter Bayer, June 1994


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

background { color MidnightBlue }

camera {
  location <0, 7, -10>
  right <4/3, 0, 0>
  up <0, 1, 0>
  sky <0, 1, 0>
  direction <0, 0, 1.21>
  look_at <0, 0, 0>
}

light_source { <5, 20, -10> colour White }

plane { y, -6
   pigment {
      checker colour Blue colour Green 
      scale 5
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

difference {
  lathe {
    linear_spline
  
    12,
  
    <2, 1>,
    <2, -1>, <3, -1>, <3.4, -2>, <4, -1.1>, <3.6, -0.9>,
    <2.6, 0>,
    <3.6.9>, <4, 1.1>, <3.4, 2>, <3, 1>, <2, 1>
  
    pigment {
      color Red
    }
    finish {
      ambient 0.1
      diffuse 0.6
      phong 0.6
      phong_size 7
//      reflection 0.3
    }
  }
  box { 
    <0, -5, 0>, <5, 5, -5> 
    rotate <0, 30, 0> 
    pigment {
      color Green
    }
    finish {
      ambient 0.1
      diffuse 0.6
      phong 0.6
      phong_size 7
    }
  }
}
