#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { pigment { rgb <1,.8,.6> } finish { specular .5 } }
#declare Objiii = cubicle { original box { <-0.5,-3.5,-0.5>,<0.5,3.5,0.5>  } accuracy <20,120,20> }
#local i=0;
#while (i<15)
screw  { original Objiii origin 0  direction i*20*y
#if (mod(i,3)=1) minimal -30.0 maximal i*45.0 #end
#if (mod(i,4)=0) right #end
				rotate 30*x*mod(i+1,2)  translate (mod(i,2)-0.5)*8*y+i*1.5*x } 
#local i=i+1;
#end
camera { location <10.0,0,150> up 8*y right 8*x*image_width/image_height
		direction -z angle 10 }
light_source { <200,100,-150>, z }
light_source { <00,10,150>, 1 }
light_source { <200,100,100>, x }
light_source { 0, y }
