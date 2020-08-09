#version 3.7;
global_settings{assumed_gamma 1.0 }
#include "colors.inc"
#declare T1= texture { pigment { rgb <1,.8,.6> } finish { specular .5 } };
#declare T2= texture { pigment { rgb <.8,.6,1> } finish { specular .5 } };
#declare T3= texture { pigment { rgb <.6,1,.8> } finish { specular .5 } };
#default { texture { T1 }}
#declare Obj = sphere { 0,1 texture { T3} }
#declare Spacing = 2.1;
#declare Tes= tessel { original Obj accuracy 5 offset 0.1 albinos texture { T2 } }
object { Obj translate x*2.5*Spacing-1.1*y }

object { Tes translate 0.5*x*Spacing-1.1*y }
#for(i,0,3,1)
smooth { original Tes method i translate Spacing*x*i+1.1*y }
#end

camera { orthographic location < 0, 4, -18 > *2.1 up y right image_width/image_height*x
look_at < 0, 0, 0 > angle 37 / 3 translate Spacing*x*3.0/2 }
light_source { <200, 100, -150 >, 1}
light_source { <-200, 100, -100 >, x * .5}
