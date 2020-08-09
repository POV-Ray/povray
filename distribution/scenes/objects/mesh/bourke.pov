#declare Num=6;
#declare Height=4.4;
camera { orthographic 
	location <2,1,-5>
		up Height*y
		right Height*x*image_width/image_height
		direction z
}
light_source { <-40,100,-15>,1}
light_source { <-10,00,-25>,1/2}
light_source { <10,00,-25>,1/4}
#include "colors.inc"
#declare Ob= sphere { 0,0.9 };
#local i=0;
#while (i< Num)
bourke 
{
	original Ob
		accuracy pow(2,i+1)
		precision 4
		offset 0.01
	texture { pigment { colour Aquamarine }}
	translate 2*(mod(i,3))*x+(int(i/3)*2*y)
	}
#local i=i+1;
#end
