// Persistence Of Vision raytracer version 3.1 sample file.
// Demonstrates quilted normals


global_settings { assumed_gamma 2.2 }

 #include "colors.inc"
 #include "textures.inc"
 #include "shapes.inc"

 camera {
   location <-5, 6, -13>
   direction <0, 0, 2>
   right <1.3333, 0, 0>
   look_at <0, 0, 0>
 }

 light_source { < -10, 0.5, -4> colour SkyBlue }
 light_source { < 10, 20, -2> colour Gray90
     area_light <4 0 0> <0 0 4> 10 10
     adaptive 1
     jitter

 }

 // Sky
 sphere {
   <0, 0, 0>, 1
   hollow on
   texture {
     pigment {
       gradient y
       color_map {[0, 1 color White color SkyBlue]}
     }
     finish {ambient 1 diffuse 0}
   }
   scale 100000
 }

 #declare X_Scale = 100;
 union {
     plane { z, 0 hollow on}
     plane { y, 0 inverse }
     cylinder { <-2*X_Scale, 1, -1>,<2*X_Scale, 1, -1>, 1
         clipped_by { box { <-X_Scale, 0, -1> <X_Scale, 1, 0> }}
     }
     texture {
         normal { quilted 1 }
         pigment { checker color SkyBlue color Wheat }
         finish {  Shiny }
     scale <0.5,0.5,0.5>
     translate y*100
     translate .1
     }
     clipped_by { box { <-X_Scale+0.1, -0.1, -10000> <X_Scale+0.1, 1000, 1> }}
     rotate -y*2
 }

 sphere { <0, 1, 0>, 2
     texture {
         pigment { White }
         finish { Mirror }
         normal { quilted 1  control0 0.5  control1 1
             scale 0.45
             turbulence 0.5
             lambda 0.5
             omega 0.707
             octaves 4
         }
     }
     translate <-2, 1, -4>
 }

