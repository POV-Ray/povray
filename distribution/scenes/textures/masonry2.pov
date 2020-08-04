#version 3.7;
global_settings{ assumed_gamma 1.0 }

#declare seeds=seed(33);
#include "colors.inc"
background { Aquamarine }
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

#declare Tex=
 texture {
masonry <0.05,1,0.999> { 
#for(ty,-10,10,1)
#local k=rand(seeds)*0.98;
#local k2=rand(seeds)*0.98;
#local k3=rand(seeds)*0.98;
#for(tx,0,359,7.5)
#local zy=ty;
#local zx= 4.5*cos(radians(tx+7.5*ty))-12;
#local zz= 4.5*sin(radians(tx+7.5*ty));

#local ax= 4.5*cos(radians(tx+7.5*ty))+7;
#local ay= 4.5*sin(radians(tx+7.5*ty));
#local az= ty;
#local bx= 3.5*cos(radians(tx+7.5+7.5*ty))+7;
#local by= 3.5*sin(radians(tx+7.5+7.5*ty));
#local bz= ty;
#if (0=mod(tx,15))
#local k=rand(seeds)*0.98;
#local k2=rand(seeds)*0.98;
#local k3=rand(seeds)*0.98;
#end
<zx, zy, zz ,k>,
<-zx,zy,zz,k2>,
//#if (abs(ty)>=3)
<ax,ay,az,k3>,
<bx,by,bz,k>,
//#end
#end
#end
}
texture_map{
[0.0 pigment { color Red } ]
[0.99 pigment { color IndianRed } ]
[0.990 pigment { color White } ]
[0.994 pigment { color White } ]
[0.999 pigment { color Black } ]
[1.0 pigment { color White } ]
}
} 
difference{
union {
cylinder { <-12,-10,0>,<-12,10,0>,5}
cylinder { <12,-10,0>,<12,10,0>,5}
cylinder { <7,0,-10>,<7,0,10>,5}
}
cylinder { <-12,-11,0>,<-12,11,0>,3}
cylinder { <12,-11,0>,<12,11,0>,3}
cylinder { <7,0,-11>,<7,0,11>,3}
texture { Tex }
}
