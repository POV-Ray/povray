#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera {location -30*z
direction z
up y
right x*image_width/image_height
angle 25
rotate 40*x
}

#include "colors.inc"
#include "woods.inc"
cylinder { -6*y, 4*y, 1 open texture { pigment { White } } }

#declare RA = seed(122);

light_source { -100*z, 1 }
light_source { 100*y, 1 }
light_source { 100*y-100*z, 1 }
light_source { 100*y-100*x, 1 }

plane { y, -6
texture { checker texture { pigment { Yellow } }  texture { pigment { Green } } }
}
#declare Texture = texture { T_Wood5 rotate 90*x };
#declare R1=2;
#declare R2=3;
#declare R3=4;
#declare R4=5;
#declare R5=6;
#declare S=1/5;
#declare N=8/S;
rational_bezier_patch{ 5,N+1 // accuracy 0.0000001
#for(i,-4,4,S)
#local In=rand(RA);
<(R1+In)*cos(i),i,(R1+In)*sin(i),1>
<(R2+In/2)*cos(i),i+2*rand(RA),(R2+In/2)*sin(i),0.707>
<R3*cos(i),i,R3*sin(i),1>
<(R4-In/2)*cos(i),i-2*rand(RA),(R4-In/2)*sin(i),0.707>
<(R5-In)*cos(i),i,(R5-In)*sin(i),1>
#end
texture { uv_mapping Texture scale <1/4, 1/34,1> }
}
