#version 3.7;
global_settings{assumed_gamma 1.0}
#default { pigment { rgb <1,.8,.6> } finish { specular .5 } }

#declare Objiii = 
mesh { 
   cubicle { original box { <-4,1,0.5>,<4,2,1> } 
             accuracy <120,20,20> 
           }
   tessel { original cone { <-4,3,0.75>,0.5,<4,3,0.75>,0 } 
            accuracy <120,20,20> 
            offset 0.1 
          }
   tessel { original cylinder { <-4,4.5,0.75>,<4,4.5,0.75>,0.5 }  
            accuracy <120,20,20> 
            offset 0.1 
          }
     }

bend  { original Objiii 
        origin 0  
        amount 30 
        fixed z 
        direction y
        minimal -0.0
        maximal 2.0 
				rotate 30*x
                translate -4*y
      } 

bend  { original Objiii 
        origin 0  
        amount 30 
        fixed y 
        direction z
        minimal -50.0
        maximal 2.0 
				translate 10*x-4*y
				translate 1.25*y
      } 
object { Objiii rotate -19*y translate 10*x+4*y }
object { Objiii rotate -99*y translate 4*y }
camera { //orthographic
	location <5.0,2.5,150> //look_at <0,2.5,0> 
		up 8*y
		right 8*x*image_width/image_height
		direction -z
		angle 10
		}
light_source { <200,100,-150>, z }
light_source { <00,10,150>, 1 }
light_source { <200,100,100>, x }
light_source { 0, y }
#include "colors.inc"
cylinder { -10*y,10*y,0.1 
	texture { pigment { Black } }finish { ambient 0 diffuse 0 specular 0}}

sphere { 20*x+1.25*y,0.1 
	texture { pigment { Black } }finish { ambient 0 diffuse 0 specular 0}}
