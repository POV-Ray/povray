// Persistence Of Vision raytracer version 3.1 sample file.
// Density_file pattern example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

#declare T1=
 texture{
   pigment{
     density_file df3 "spiral.df3" interpolate 1
     translate <-0.5,-0.5,-0.5>
     scale <1,1,0.1>
     translate <0,0,-0.75>
     color_map{[0.0 Black][1.0 White]}
   }
 }

#declare T2=
 texture{
   pigment{White}
   normal{
     density_file df3 "spiral.df3" ,0.6 
     interpolate 1
     translate <-0.5,-0.5,-0.5>
     scale <1,1,0.1>
     translate <0,0,-0.75>
   }
   finish{phong 0.8 phong_size 200}
 }

#include "pignorm.inc"
