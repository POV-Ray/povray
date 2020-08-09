#version 3.7;
global_settings{ assumed_gamma 1.0 }

#declare seeds=seed(33);
#include "colors.inc"
background { Aquamarine }
#declare Tex=
 texture {
masonry <0.2,1,0.99> { 
#for(zx,-15,15,1)
#for(zy,-15,15,1)
#for(zz,-15,15,1)
#if (rand(seeds)< 0.01250)
<zx, zy, zz ,(rand(seeds)*0.98)>,
#end
#end
#end
#end
}
texture_map{
[0.0 pigment { color Blue } ]
[0.3 pigment { color Cyan } ]
[0.5 pigment { color Green } ]
[0.7 pigment { color Yellow } ]
[0.99 pigment { color Magenta } ]
[0.990 pigment { color White } ]
[0.994 pigment { color White } ]
[0.998 pigment { color Red } ]
[1.0 pigment { color Red } ]
}
} 
difference {
superellipsoid { <0.025,0.025> scale 10
} 
box { <-3,-3,-11>,<3,3,11>  rotate -10*z}
box { <-11,-4,-4>,<11,4,4>  rotate -20*x}
box { <-5,-11,-5>,<5,11,5> rotate 30*y}

texture { Tex scale 6/11}
}
camera { location 50*y+060*x-100*z
direction z
up y
right image_width/image_height*x
look_at <0,0,0>
angle 20
}


light_source { <-4,8,-2>*100, 1 
area_light 40*x,40*z, 7,7 circular orient
}
light_source { <4,8,-2>*100, 3/4 
area_light 40*x,40*z, 7,7 circular orient 
}
light_source { <4,80,-2>*100, 1/2 
area_light 40*x,40*z, 7,7 circular orient 
}
