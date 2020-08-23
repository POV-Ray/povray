#version 3.8;
global_settings {assumed_gamma 1.0}
#include "colors.inc"
#default{ finish{ ambient 0.1 diffuse 0.9 }} 


light_source { <100,1000,-1000>, White}

camera { location <0,-1,-16> 
         right    x*image_width/image_height
         angle 35 
         look_at <0,-1,0>
       }

plane{-z,-10  pigment{checker color rgb<1,1,1>*0.8 color rgb<1,1,1>} }

#declare String1=concat("parsed_tokens =", str(parsed_tokens,0,0));


text{ internal 1, "Parser info", 0.3, 0 pigment{Blue*0.7} translate -4.5*x+1.5*y }
text{ internal 1, String1, 0.3, 0 pigment{Red*0.7} translate -4.5*x }
#declare String2=concat("parsed_tokens =", str(parsed_tokens,0,0));
#declare String3=concat("parsed_tokens =", str(parsed_tokens,0,0));
text{ internal 1, String2, 0.3, 0 pigment{Red*0.7} translate -4.5*x -1.5*y }
text{ internal 1, String3, 0.3, 0 pigment{Red*0.7} translate -4.5*x -3*y }
