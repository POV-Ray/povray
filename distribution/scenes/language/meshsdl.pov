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

#declare Mesh= mesh2{
  vertex_vectors{
    14
    <-1, -1, -1>, < 1, -1, -1>,
    <-1,  1, -1>, < 1,  1, -1>,
    <-1, -1,  1>, < 1, -1,  1>,
    <-1,  1,  1>, < 1,  1,  1>,
    
    -1.5*x, 1.5*x,
    -1.5*y, 1.5*y,
    -1.5*z, 1.5*z,
  }
  face_indices{
    24,
    <8,0,2>,
    <8,0,4>,
    <8,6,2>,
    <8,6,4>,
    
    <9,1,3>,
    <9,1,5>,
    <9,7,3>,
    <9,7,5>,
    
    <10,0,1>,
    <10,0,4>,
    <10,5,1>,
    <10,5,4>,
    
    <11,2,3>,
    <11,2,6>,
    <11,7,3>,
    <11,7,6>,
    
    <12,0,1>,
    <12,0,2>,
    <12,3,1>,
    <12,3,2>,
    
    <13,4,5>,
    <13,4,6>,
    <13,7,5>,
    <13,7,6>
  }
}

object { Mesh texture { pigment { color rgb <0.8, 0.8, 0.2> } }
 translate 3*x +y
 }

#declare String=concat("#Vertex    : ",str(get_vertex_count(Mesh),3,0),
"\n#Normal    : ",str(get_normal_count(Mesh),3,0),
"\n#triangles : ",str(get_triangle_count(Mesh),3,0),
"\n\nFourth Triangle\nindex of vertex #",vstr(3,get_vertex_indices(Mesh,3),", #",1,0),
"\nindex of normal #",vstr(3,get_normal_indices(Mesh,3),", #",2,0),
"\nsmooth (0 for no) : ",str(is_smooth_triangle(Mesh,3),3,0),
"\n\nTenth vertex  = <",vstr(3,get_vertex(Mesh,9),",",2,3),
">\n\nTenth normal  = <",vstr(3,get_normal(Mesh,9),",",2,3),
">\n");

 galley{internal  3
       thickness 0.3
    spacing 1
    wrap false
    width 40
    leading 1
    indentation 0

    String
 pigment{Red*0.7} scale 1/2
 translate -4.5*x+1*y
}
text{ internal 1, "Mesh Info", 0.3, 0
 pigment{Blue*0.7} 
 translate -4.5*x+1.5*y
}
