#version 3.8;
global_settings {
  assumed_gamma 1.0
  max_trace_level 10
}

#include "colors.inc"
#include "metals.inc"

#declare VEC=x;
#declare ScalePat = <2/11,1/13,1>;

#declare F1=function {pigment{cells}}
#declare F2=function(x,y,z){sin(x)*sin(y)}
#declare Yuck=pigment {
        function {
           F2(x*pi,y*pi,z)*F1(x,y,z).gray
        }
     }

     cone { -5,3,10,1
            uv_reference VEC
uv_mapping
          pigment {
               Yuck
               colour_map {
                  [0.1, rgb <0.6,0.5,0>]
                  [0.15, rgb <0.3,0.7,0>]
                  [0.25, rgb <0.5,0.7,0>]
                  [0.5, rgb <0.2,0.6,1>]
                  [0.8, rgb <.1,0,.1>]
               }
               scale ScalePat
            }
            scale 0.3
         }
camera {
     right x*image_width/image_height
     location  <0,0,-6>
     look_at   <0,0,0>
}
light_source {<500,500,-500> rgb 1}
light_source {<500,-100,-500> rgb <0.4,0.3,0.3> shadowless}
light_source {<-500,0,-500> rgb <0.3,0.3,0.4> shadowless}
sky_sphere {
pigment {
         planar
         color_map { [0.0 color blue 1] [1.0 color rgb <0.8,0.8,1.0>] }
      }
}
