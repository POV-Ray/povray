// Persistence Of Vision raytracer version 3.1 sample file.



global_settings { assumed_gamma 2.2 }

#include "colors.inc"

camera {
   location  <0, 10,-20>
   direction <0, 0,  1>
   up        <0,  1,  0>
   right     <4/3, 0,  0>
   look_at   <0, 0, 0>
   }

background { color rgb <0.5, 0.5, 0.5> }

text { ttf "crystal.ttf", "POV-Ray", 2, 0
   translate <-2, 0, -7>
   pigment { color rgb <1, 0.2, 0.2> }
   finish {
      ambient 0.2
      diffuse 0.6
      phong 0.3
      phong_size 100
      }
   scale <4, 4, 1>
   rotate <0, 10, 0>
   }

text { ttf "crystal.ttf", "Version:3.1", 2, 0
   translate <-2, 0, 8>
   pigment { color rgb <1, 0.2, 0.2> }
   finish {
      ambient 0.2
      diffuse 0.6
      phong 0.3
      phong_size 100
      }
   scale <4, 4, 1>
   rotate <0, -10, 0>
   }

light_source {<20, 30, -100> colour White}

disc { <0, -1, 0>, <0, 1, 0>, 5000
   pigment { checker color rgb <0.2, 1, 0.2> color rgb <1, 1, 1> scale 10 }
   finish { ambient 0.2 diffuse 0.6 }
   }
