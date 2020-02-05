// Persistence Of Vision raytracer version 3.1 sample file.
// Normal_map example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

 camera {
     location <0,3,-31>
     direction 3*z
 }

#declare Amt=<0,0,0>;

 light_source { <200, 200, -500> color White}

 #default {
     pigment { White }
     normal { bump_size 1.0 }
     finish { phong 0.8 phong_size 200 }
 }

box{<-2,-2,0>,<2,2,1>
  normal {
    gradient x
    normal_map{
      [0.3 marble turbulence 0.5]
      [0.7 gradient y scallop_wave scale .3]
    }
    translate -3*x
    scale 2
  }
  translate <-3,5.50>
}

box{<-2,-2,0>,<2,2,1>
  normal {
    wood
    normal_map{
      [0.5 marble turbulence 0.5]
      [0.5 radial sine_wave frequency 10 rotate x*90]
    }
  }
  translate <3,5.50>
}
box{<-2,-2,0>,<2,2,1>
  normal {
    checker
      normal { marble turbulence 0.5 }
      normal { radial sine_wave frequency 10 rotate x*90}
  }
  translate <-3,1,0>
}
box{<-2,-2,0>,<2,2,1>
  normal {
    radial frequency 10 
    normal_map{
      [0.5 gradient x triangle_wave scale .3]
      [0.5 gradient z scallop_wave scale .3]
    }
    rotate x*90
  }
  translate <3,1,0>
}
