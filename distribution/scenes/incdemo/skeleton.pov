#version 3.8;
global_settings { assumed_gamma 1.0 }

camera { location -20*z
 direction z
 up y
 right image_width*x/image_height
 angle 4
 look_at <0.5,0.5,0>
}

#include "colors.inc"

#declare Mesh =
mesh2 {
   vertex_vectors {
      24,
      <0,0,0>, <0.5,0,0>, <0.5,0.5,0>, //1
      <0.5,0,0>, <1,0,0>, <0.5,0.5,0>, //2
      <1,0,0>, <1,0.5,0>, <0.5,0.5,0>, //3
      <1,0.5,0>, <1,1,0>, <0.5,0.5,0>, //4
      <1,1,0>, <0.5,1,0>, <0.5,0.5,0>, //5
      <0.5,1,0>, <0,1,0>, <0.5,0.5,0>, //6
      <0,1,0>, <0,0.5,0>, <0.5,0.5,0>, //7
      <0,0.5,0>, <0,0,0>, <0.5,0.5,0>  //8
   }
   face_indices {
      8,
      <0,1,2>,    <3,4,5>,       //1 2
      <6,7,8>,    <9,10,11>,     //3 4
      <12,13,14>, <15,16,17>,    //5 6
      <18,19,20>, <21,22,23>     //7 8
   }
   pigment {rgb 1}
}


light_source { 10*<0,10,-20>, 0.9 }
#include "golds.inc"
#include "meshskeleton.inc"

MeshVertex ( Mesh, 0.04, texture { pigment { color rgb 0.75 } } )
MeshFace ( Mesh, 0.03, 0.01, T_Gold_3C )
MeshEdge( Mesh, 0.01, texture { pigment { color red 1 } } )
