#version 3.8;
global_settings{ assumed_gamma 1.0 }

#include "colors.inc"
#include "metals.inc"
#include "skies.inc"

#declare Unzoom = 3;
#declare Radius = 18.5;

camera { orthographic
location 6*Radius*<-6,2,-9>
direction Radius*z
up Unzoom*Radius*y
right Unzoom*Radius*x*image_width/image_height
look_at 0
}

light_source { Radius*<-2,50,-30>*100, 1 area_light 400*x*Radius,Radius*400*z, 10, 10  }

interunion {
#for(i,-1, 1, 2)
#for(j,-1, 1, 2)
#for(k,-1, 1, 2)
sphere { <i,j,k>, Radius texture { T_Chrome_3A} }
#end
#end
#end
sphere { 0, Radius }
  range{ 5 }
  texture { T_Chrome_3B}
}

plane { y, -Radius
  texture { pigment { tiling 2
      color_map{
        [1/3 color (Salmon+IndianRed)/2]
        [2/3 color Salmon]
        [0.95 color IndianRed]
        [1 color Black]
      }
        scale Radius/2} }
}

sky_sphere { S_Cloud2 }
