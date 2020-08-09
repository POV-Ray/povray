#version 3.7;
global_settings{ assumed_gamma 1.0 }

#include "colors.inc"
background { Aquamarine }

#declare Tex= texture { binary 
{ 
#for(i,0,359,30)
sphere { 8*x, 10 rotate i*y }
#end
}
texture_map{
	[0.0 pigment { color Blue } ]
		[0.01 pigment { color Cyan } ]
		[0.05 pigment { color Green } ]
		[0.2 pigment { color Yellow } ]
		[1.0 pigment { color Red } ]
}
} 


plane { y,0
texture { Tex }
}
camera { location 100*y direction z up y right image_width/image_height*x look_at <0,0,0> angle 27 }


light_source { <-4,8,-2>*100, 1 area_light 40*x,40*z, 7,7 circular orient }
light_source { <4,8,-2>*100, 3/4 area_light 40*x,40*z, 7,7 circular orient }
light_source { <4,80,-2>*100, 1/2 area_light 40*x,40*z, 7,7 circular orient }

