#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,-1,-16> 
         right    x*image_width/image_height
         angle 35 
         look_at <0,-1,0>
       }

plane{-z,-10  pigment{checker color rgb<1,1,1>*0.8 color rgb<1,1,1>} }

#declare String=concat("Type : ",camera_type,"\n\nUp   : <",
vstr(3,camera_up,",",0,3),">\nright: <",
vstr(3,camera_right,",",0,3),">\ndir. : <",
vstr(3,camera_direction,",",0,3),">\n\nloca.: <",
vstr(3,camera_location,",",0,2),">");

 galley{internal  3
    thickness 0.3
    spacing 1
    wrap false
    width 40
    leading 1
    indentation 0
    String
 pigment{Red*0.7} scale 2/3
 translate -4.5*x
}
text{ internal 1, "Camera Info", 0.3, 0
 pigment{Blue*0.7} 
 translate -4.5*x+1.5*y
}
