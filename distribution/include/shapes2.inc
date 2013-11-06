// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

#ifndef(Shapes2_Inc_Temp)
#declare Shapes2_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
#   debug "including shapes2.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1
         Implements additional, useful, but seldom used shapes

                    Contents:
                    =========
            Tetrahedron       HalfCone_Y
            Octahedron        Pyramid
            Dodecahedron      Pyramid2
            Icosahedron       Square_X
            Rhomboid          Square_Y
            Hexagon           Square_Z
*/


// Shapes by Tom Price [75300,620]:
#declare Tetrahedron =
 intersection 
  {plane {-y,1}
   plane {-z,1 rotate <19.47,    0, 0>}
   plane {-z,1 rotate <19.47, -120, 0>}
   plane {-z,1 rotate <19.47,  120, 0>}
   
   bounded_by {sphere {0, 3}}
  }
   
#declare Octahedron = 
 intersection 
  {plane { z, 1 rotate < 35.26438968275, 0, 0>}
   plane { z, 1 rotate <-35.26438968275, 0, 0>}
   plane {-z, 1 rotate < 35.26438968275, 0, 0>}
   plane {-z, 1 rotate <-35.26438968275, 0, 0>}
   
   plane { x, 1 rotate <0, 0, -35.26438968275>}
   plane { x, 1 rotate <0, 0,  35.26438968275>}
   plane {-x, 1 rotate <0, 0, -35.26438968275>}
   plane {-x, 1 rotate <0, 0,  35.26438968275>}
   
   bounded_by {sphere {0, 1.7321}}
  }
   
#declare Dodecahedron = 
 intersection 
  {plane {-z, 1 rotate <-26.56505117708,    0, 0>}
   plane {-z, 1 rotate <-26.56505117708,  -72, 0>}
   plane {-z, 1 rotate <-26.56505117708, -144, 0>}
   plane {-z, 1 rotate <-26.56505117708, -216, 0>}
   plane {-z, 1 rotate <-26.56505117708, -288, 0>}
   
   plane {-z, 1 rotate <26.56505117708,  -36, 0>}
   plane {-z, 1 rotate <26.56505117708, -108, 0>}
   plane {-z, 1 rotate <26.56505117708, -180, 0>}
   plane {-z, 1 rotate <26.56505117708, -252, 0>}
   plane {-z, 1 rotate <26.56505117708, -324, 0>}
   
   plane { y, 1}
   plane {-y, 1}
   
   bounded_by {sphere {0, 1.2585}}
  }
   
#declare Icosahedron = 
 intersection 
  {plane {-z, 1 rotate <52.6625,    0, 0>}
   plane {-z, 1 rotate <52.6625,  -72, 0>}
   plane {-z, 1 rotate <52.6625, -144, 0>}
   plane {-z, 1 rotate <52.6625, -216, 0>}
   plane {-z, 1 rotate <52.6625, -288, 0>}
   
   plane {-z, 1 rotate <10.8125,    0, 0>}
   plane {-z, 1 rotate <10.8125,  -72, 0>}
   plane {-z, 1 rotate <10.8125, -144, 0>}
   plane {-z, 1 rotate <10.8125, -216, 0>}
   plane {-z, 1 rotate <10.8125, -288, 0>}
   
   plane {-z, 1 rotate <-52.6625,  -36, 0>}
   plane {-z, 1 rotate <-52.6625, -108, 0>}
   plane {-z, 1 rotate <-52.6625, -180, 0>}
   plane {-z, 1 rotate <-52.6625, -252, 0>}
   plane {-z, 1 rotate <-52.6625, -324, 0>}
   
   plane {-z, 1 rotate <-10.8125,  -36, 0>}
   plane {-z, 1 rotate <-10.8125, -108, 0>}
   plane {-z, 1 rotate <-10.8125, -180, 0>}
   plane {-z, 1 rotate <-10.8125, -252, 0>}
   plane {-z, 1 rotate <-10.8125, -324, 0>}
   
   bounded_by {sphere {0, 1.2585}}
  }

// Shapes by others
// Convenient  finite cone primitive, pointing up in the Y axis
#declare HalfCone_Y = 
 intersection 
  {object {Cone_Y}
   plane  { y, 0}
   plane  {-y, 2}
   translate <0, 1, 0>
   scale <0.5, 1, 0.5>
   
   bounded_by {sphere{0, 1.5}}
  }

/* Hexagonal Solid, axis along x */
#declare  Hexagon = 
 intersection
  {plane {z, 1}  /* Rotate 90 in z axis to stand up */
   plane {z, 1 rotate < 60, 0, 0>}
   plane {z, 1 rotate <120, 0, 0>}
   plane {z, 1 rotate <180, 0, 0>}
   plane {z, 1 rotate <240, 0, 0>}
   plane {z, 1 rotate <300, 0, 0>}
   plane { x, 1}
   plane {-x, 1}
   
   bounded_by {sphere{0, 1.5}}
  }

/* Three Dimensional 4-Sided Diamond */
#declare Rhomboid = 
 intersection 
  {plane {-x, 1 rotate <0, 0, -30>}
   plane { x, 1 rotate <0, 0, -30>}
   plane { z, 1}
   plane {-z, 1}
   plane { y, 1}
   plane {-y, 1}
   
   bounded_by {sphere{0, 2.3}}
  }

// Classic four-sided pyramids.
// The first can't be used correctly in CSG, the second can.
#declare Pyramid = 
   union { // This isn't true CSG, it's just used for convenience
      triangle { <-1, 0, -1>, <+1, 0, -1>, <0, 1, 0>  }
      triangle { <+1, 0, -1>, <+1, 0, +1>, <0, 1, 0>  }
      triangle { <-1, 0, +1>, <+1, 0, +1>, <0, 1, 0>  }
      triangle { <-1, 0, +1>, <-1, 0, -1>, <0, 1, 0>  }

      triangle { <-1, 0, -1>, <-1, 0, +1>, <1, 0, +1>  }
      triangle { <-1, 0, -1>, <+1, 0, -1>, <1, 0, +1>  }

   scale <1, 2, 1>
   translate -y
}
#declare Pyramid2 = intersection {
   plane { < 1, 0,  0>, 1  rotate <  0, 0,  40>}
   plane { <-1, 0,  0>, 1  rotate <  0, 0, -40>}
   plane { < 0, 0,  1>, 1  rotate <-40, 0,   0>}
   plane { < 0, 0, -1>, 1  rotate < 40, 0,   0>}
   plane { <0, -1, 0>, 0 }
   translate <0 ,-1, 0>
   
   bounded_by {box {<-1,0,-1>, <1,1,1>}}
}            
             
// These next three are finite planes.
#declare Square_X = /* Scale-able plane in x */
  union 
   {triangle {<0, 1, -1>, <0, -1, 1>, <0,  1,  1>}
    triangle {<0, 1, -1>, <0, -1, 1>, <0, -1, -1>}
   }

#declare Square_Y =  /* Scale-able plane in y */
  union 
   {triangle {<-1, 0, 1>, <1, 0, -1>, < 1, 0,  1>}
    triangle {<-1, 0, 1>, <1, 0, -1>, <-1, 0, -1>}
   }

#declare Square_Z =  /* Scale-able plane in z */
  union 
   {triangle {<-1, 1, 0>, <1, -1, 0>, <-1, -1, 0>}
    triangle {<-1, 1, 0>, <1, -1, 0>, < 1,  1, 0>}
   }

#version Shapes2_Inc_Temp;
#end
