// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// FILEIO.POV demonstrates basic use of fopen, read and write directives.  
// A string, float and vector are written to a file, read back in
// and displayed.

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,1,-16> direction 2*z look_at <0,1,0>}

#fopen MyFile "FILEIO.TXT" write 

#write (MyFile,"\"Testing 123\",",5,",",<1,2,3>,"\n")

#fclose MyFile

#fopen MyFile "FILEIO.TXT" read

#read (MyFile,MyString,MyFloat,MyVect)

#fclose MyFile

union{
 text{ttf "timrom.ttf" concat("MyString='",MyString,"'"),0.1,0 translate y}
 text{ttf "timrom.ttf" concat("MyFloat=",str(MyFloat,0,0)),0.1,0 }
 text{ttf "timrom.ttf" concat("MyVector=<",
                               str(MyVect.x,0,0),",",
                               str(MyVect.y,0,0),",",
                               str(MyVect.z,0,0),">"),0.1,0 translate -y}

 pigment{Red}
 translate -5*x
}

union { 
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker Cyan,Yellow}
}

