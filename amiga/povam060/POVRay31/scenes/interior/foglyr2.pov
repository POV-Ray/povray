// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dieter Bayer.
// This scene shows fog with transmittance used.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

camera {
   location  <0, 20, -100>
   direction <0,  0,    1>
   up        <0,  1,    0>
   right   <4/3,  0,    0>
}

background { colour SkyBlue }

fog{
    color Blue
    fog_type 2
    fog_alt 0.35
    fog_offset 0
    distance 1.5
    turbulence <.15, .15, .15>
    omega 0.35
    lambda 1.25
    octaves 5
}
fog{
    color Orange
    fog_type 2
    fog_alt 1.45
    fog_offset 0
    distance 1.5
    turbulence <.2, .2, .2>
    omega 0.40
    lambda 1.25
    octaves 5
}

// Put down the beloved famous raytrace green/yellow checkered floor
plane { y, -10
   pigment {
      checker colour Yellow colour Green
      scale 20
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

sphere { <0, 25, 0>, 40
   pigment {Red}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

sphere { <-100, 150, 200>,  20
   pigment {Green}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

sphere { <100, 25, 100>, 30
   pigment {Blue}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

light_source {<100, 120, 40> colour White}
