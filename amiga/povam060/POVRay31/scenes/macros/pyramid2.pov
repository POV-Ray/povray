// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
//    Based on a POV-Ray 3.0 file by
//    Sven Hilscher * 3D-Max usergroup germany
//    email: sven@rufus.central.de
// PYRAMID2.POV demonstrates basic use of macros and local
// identifiers in recursive calls.  Creates a fractal
// pyramid from spheres.
//
// This version is more complicated but it eliminates
// some reduntant spheres.

// Define the macro.  Parameters are:
//   X:  position of sphere
//   Y:  position of sphere
//   Z:  position of sphere
//   R:  radius of sphere
//   L:  level of recursion
#macro Pyramid(X,Y,Z,R,L,D)

  sphere { <X,Y,Z>,R}

  #if (L > 0)
    #local New_L = L - 1;
    #local New_R = R / 2;
    #local Pos   = New_R * 3;

    #if (D!=2)
       Pyramid(X+Pos,Y,Z,New_R,New_L,1)
    #end       
    #if (D!=1)
       Pyramid(X-Pos,Y,Z,New_R,New_L,2)
    #end       
    #if (D!=4)
       Pyramid(X,Y+Pos,Z,New_R,New_L,3)
    #end       
    #if (D!=3)
       Pyramid(X,Y-Pos,Z,New_R,New_L,4)
    #end       
    #if (D!=6)
       Pyramid(X,Y,Z+Pos,New_R,New_L,5)
    #end       
    #if (D!=5)
       Pyramid(X,Y,Z-Pos,New_R,New_L,6)
    #end       
  #end       
#end




union {
  Pyramid(0,0,0,4,6,0)

  pigment { color rgb <1,1,0> } 
}

light_source { <2,20,10> color rgb <1,1,1> }

background { color rgb <.4, .3, .2> }

camera { location <5,17,19>
         look_at  <0,0,0>
}
