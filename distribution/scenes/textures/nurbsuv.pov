#version 3.8;
global_settings {
  assumed_gamma 1.0
  max_trace_level 10
}

#include "colors.inc"
#include "metals.inc"

#declare VEC=x;
#declare ScalePat = <2/4,1/8,1>;

#declare F1=function {pigment{cells}}
#declare F2=function(x,y,z){sin(x)*sin(y)}
#declare Yuck=pigment {
        function {
           F2(x*pi,y*pi,z)*F1(x,y,z).gray
        }
     }

#declare Major=8;// Major radius of torus
#declare Minor=4;// Minor radius of torus
#declare PTI=18;// number of minor circles along the major radius
#declare PTJ=5;// number of points on each minor circle
#declare RESOLUTION=128;// resolution of the mesh

#macro P(I,J)
#local V=(mod(I,2)*Minor+Major)*x+(Minor)*<cos(pi*2*J/PTJ),sin(pi*2*J/PTJ),0>;
#local W=vrotate(V, 360*I/(PTI)*y);
W
#end

#macro NURBS(OrderU, OrderV)
  nurbs{
    OrderU, OrderV,
    #declare NBI=PTI+OrderU-1;
    #declare NBJ=PTJ+OrderV-1;
    NBI, NBJ,
    #for(I,1,NBI+OrderU)
    I,
  #end
  #for(J,1,NBJ+OrderV)
  J,
#end
#for(J,1,NBJ)
#for(I,1,NBI)
#declare W= P(I,J);
<W.x,W.y,W.z,1>
#end
#end
}
#end
#include "NurbsMesh.inc"
#declare Nurbs=NURBS(3,2);
mesh { UVMeshable( Nurbs, RESOLUTION, RESOLUTION )
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
            rotate -60*x
            scale 0.2
            translate y/3
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
