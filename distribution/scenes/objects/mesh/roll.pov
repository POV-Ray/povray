#version 3.7;
global_settings{assumed_gamma 1.0}
#default { pigment { rgb <1,.8,.6> } finish { specular .5 } }

#declare Objiii = cristal { original
difference { box { -1,1 } 
             cylinder { <-2,0,0>,<2,0,0>,0.8 } 
             cylinder { <0,-2,0>,<0,2,0>,0.6 } 
             cylinder { <0,0,-2>,<0,0,2>,0.3 } 
						 scale 2.5
           }
accuracy 90
}


roll  { original Objiii 
        origin 0  
        direction 30*y
				rotate 40*y
				rotate 30*x
      } 

roll  { original Objiii 
        origin 0  
        direction -70*(y+x+z)
        minimal 135.0
        maximal 255.0 
				rotate 30*x-20*y
				translate 20*x
      } 
object { Objiii rotate -19*y+30*x translate 10*x }
camera { orthographic
	location <10.0,0.0,150> //look_at <0,2.5,0> 
		up 8*y
		right 8*x*image_width/image_height
		direction -z
		angle 12
		}
light_source { <200,100,-150>, z }
light_source { <00,10,150>, 1 }
light_source { <200,100,100>, x }
light_source { 0, y }
light_source { 20*x, y }
