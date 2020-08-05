#version 3.8;
global_settings {
  assumed_gamma 1.0
  max_trace_level 10
}

#include "colors.inc"
#include "metals.inc"

#declare VEC=x;
#declare ScalePat = <2/8,1/16,1>;

#declare F1=function {pigment{cells}}
#declare F2=function(x,y,z){sin(x)*sin(y)}
#declare Yuck=pigment {
        function {
           F2(x*pi,y*pi,z)*F1(x,y,z).gray
        }
     }
#declare RA = seed(122);
#declare R1=2;
#declare R2=3;
#declare R3=4;
#declare R4=5;
#declare R5=6;
#declare S=1/5;
#declare N=8/S;
rational_bezier_patch{ 5,N+1 
#for(i,-4,4,S)
#local In=rand(RA);
<(R1+In)*cos(i),i,(R1+In)*sin(i),1>
<(R2+In/2)*cos(i),i+2*rand(RA),(R2+In/2)*sin(i),0.707>
<R3*cos(i),i,R3*sin(i),1>
<(R4-In/2)*cos(i),i-2*rand(RA),(R4-In/2)*sin(i),0.707>
<(R5-In)*cos(i),i,(R5-In)*sin(i),1>
#end
texture {
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
          }
            scale 0.5
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
