#version 3.7;
global_settings{assumed_gamma 1.0}
#include "colors.inc"

#default { pigment { rgb <1,.8,.6> } finish { specular .5 } }

#declare Obj = sphere {0,1 }
#declare Tes= tessel { 
                    original Obj 
                    accuracy 80 
                    offset 0.1 
                    smooth 
} 
#local i=1;
#while(i<6)
displace { original Tes 
           modulation { pigment { agate 
                               pigment_map {
                                            [0 Black]
                                            [0.9/(6-i) Black]
                                            [1 White]
                                           }
                                }
                      }
           amount 0.25*sqrt(i)/(i*i)
						 
						 translate (i*2.125/2)*x+(mod(i,2)-0.5)*2.2*y
         }
#local i=i+1;
#end
object { Tes translate -1.4*y }
camera { orthographic location < 2.5, 0, -13 > 
	up 2.4*y
		right 2.4*x*image_width/image_height
	angle 35 
}
light_source { <200, 100, -150 >, 1}
light_source { <-200, 100, -100 >, x * .5}


