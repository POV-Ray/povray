//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#declare a=2;
#declare F1= function(u,v){(a+cos(u/2)*sin(v)-sin(u/2)*sin(2*v))*cos(u)}
#declare F2= function(u,v){(a+cos(u/2)*sin(v)-sin(u/2)*sin(2*v))*sin(u)}
#declare F3= function(u,v){sin(u/2)*sin(v)+cos(u/2)*sin(2*v)}
//-------------------------------------------------------------------------------------------------
#include "meshmaker.inc"
//-------------------------------------------------------------------------------------------------
object{ // Parametric(Fx,Fy, Fz, <UVmin>, <UVmax>, Iter_U, Iter_V, FileName)
   Parametric( // Builds a parametric surface out of three given functions. 
               // The uv_coordinates for texturing the surface come from the square <0,0> - <1,1>. 
      F1, F2, F3, //three functions
     <-pi, -pi>, < pi, pi>,// range within to calculate surface: <u min, v min>,<u max, v max>. 
      100,100, // resolution of the mesh in the u range and v range.
      "" // FileName: ""= non, "NAME.obj'= Wavefront objectfile, "NAME.pcm" compressed mesh file 
         // "NAME.arr" = include file with arrays to build a mesh2 from, 
         //  others: includefile with a mesh2 object 
   ) //---------------------------------------------------------------------------------------------
  
   texture {  // inside texture
   //  uv_mapping
     pigment{ color rgb <1,1,1> }
    // pigment{ checker color rgb <0.5,0.0,0.1> rgb <1,1,1> scale <0.02,0.025,0.01>}
     finish { specular 0.25}
   } // 
   /*
   interior_texture{            //  outside (interior_texture)
     uv_mapping
     pigment{ checker color rgb <0.0,0,0.0> rgb <1,0.9,0.9>  scale <0.02 ,0.025,1>}
     finish { phong 0.5 }
   } // 
   */
   scale< 1,1,1>*0.52
   rotate<75,0,0>
   translate< 0, 0, 0 >
} // end of object 
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
