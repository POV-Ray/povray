// Persistence Of Vision raytracer version 3.1 sample file.

#declare Panther_Inc_Temp = version;
#version 1.0;

/*                PANTHER         M.Miller 11/18/91 - with help from Truman*/

#declare DCone = /*---------CONE POINT DOWN /POINT at 000--------*/
intersection {
   quadric { QCone_Y scale <1 1 1>  }
   plane { <0 -1 0> 0  } plane { <0 1 0> 1   }  }

#declare UCone = /*---------CONE POINT UP /POINT at 000--------*/
intersection {
   quadric { QCone_Y scale <1 1 1>  }
   plane { <0 1 0> 0    } plane { <0 1 0> -1 inverse  }
}


#declare pan1 = texture {
   /* image_map { <1.0 -1.0 0> gif "frac3.gif"  }       */
   scale <40 40 4> rotate <0 0 180>
   color green 0.3 red 0.16 blue 0.16
   ambient 0.4 diffuse 0.7 phong 0.8 phong_size 25  }



/***********************************************************************/
/*------------------------------THE HEAD-------------------------------*/

// #declare Dot =    quadric {
//    < 1.0  1.0  1.0>
//    < 0.0  0.0  0.0>
//    < 0.0  0.0  0.0>
//    -1.0
// }
#declare Dot = sphere { 0, 1 }

// #declare Connector_1a =
// object {
//    quadric {
//       < 0.0  1.0  1.0>
//       < 0.0  0.0  0.0>
//       < 0.0  0.0  0.0>
//       -1.0
//    }
//    clipped_by {
//       plane { < 1.0  0.0  0.0> 1.0  }
//       plane { <-1.0  0.0  0.0> 0.0  }
//    }
// }
#declare Connector_1a = cylinder { -x, x, 1 }


// Infinite cone in Y axis, rotated to X axis
#declare Connector_2a =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}


/*************************************************************************/
/*------------------THE NECK---------------------------------------------*/
#declare neck =

union {
   quadric { Dot scale < 6.000000  6.000000  6.000000>
   }      

   quadric { Dot scale < 6.600000  6.600000  6.600000>
      translate < 7.000000 -1.500000  0.000000>
   }      

   quadric { Dot scale     < 7.000000  7.000000  7.000000>
      translate < 16.500000 -5.000000  0.000000>
   }      
}


#declare c1 = intersection { 
   quadric { Connector_2a  } 
   plane {     < 1.000000  0.000000  0.000000>  6.576779  }
   plane {     < 1.000000  0.000000  0.000000>  5.978890 inverse  }

   translate <-5.978890  0.000000  0.000000>
   scale     < 11.889538  1.000000  1.000000>
   translate < 0.502870  0.000000  0.000000>
   rotate    < 0.000000  0.000000 -12.094757>
} 

#declare c2 = intersection {         
   quadric { Connector_2a  } 
   plane {     < 1.000000  0.000000  0.000000>  6.994534  }
   plane {     < 1.000000  0.000000  0.000000>  6.594847 inverse  }
   translate <-6.594847  0.000000  0.000000>
   scale     < 25.290809  1.000000  1.000000>
   translate < 0.260761  0.000000  0.000000>
   rotate    < 0.000000  0.000000 -20.224859>
   translate < 7.000000 -1.500000  0.000000>
}

/*---------------------THE NOSE---------------------------------------*/

#declare Connector_1b = 
object {
   quadric {
      < 0.0  1.0  1.0>
      < 0.0  0.0  0.0>
      < 0.0  0.0  0.0>
      -1.0
   }      
   clipped_by {
      plane { < 1.0  0.0  0.0> 1.0  }
      plane { <-1.0  0.0  0.0> 0.0  }
   }     
}


#declare Connector_2b =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}


#declare c3 = intersection { 
   quadric { Connector_2b  } 
   plane {     < 1.000000  0.000000  0.000000>  4.898979  }
   plane {     < 1.000000  0.000000  0.000000>  3.919184 inverse  }

   translate <-3.919184  0.000000  0.000000>
   scale     < 4.898979  1.010000  1.010000>
   translate < 0.800000  0.000000  0.000000>
}


#declare nose = 

union {   
   difference {
      union {

         quadric { Dot scale     < 2.300000  4.000000  4.000000>  }
         quadric { Ellipsoid scale <2.5 3.98 3.98> translate <0.3 0 0>  }
         intersection { c3  }
         quadric { Dot scale     < 4.000000  5.000000  5.000000>
            translate < 5.000000  0.000000  0.000000>
         }                  

         /*-------------Cleff Palette------------------------------------------*/
         quadric { Ellipsoid scale <3 2 3> translate <1 1.7 -2>  }
         quadric { Ellipsoid scale <3 3 3> translate <1 0.7 -2>  }

      }            

      quadric { Ellipsoid scale <2.2 1.7 7> translate <-0.5 -0.5 0>  }

   }         /*----------MOUTH OPENING--------------*/
}   


/*-------------------GUMS------------------------------*/
#declare gum = union {
   cylinder { <0,0,0>, y, 1 scale <3 0.7 3.5> translate <0 1.2 0>  }
   cylinder { <0,0,0>, y, 1 scale <3 0.7 3.5> translate <0 -1 0>  }
}

/*-----------Poodle Chew - Poodle Bite-----------------------------------*/
#declare choppers = object {
   union {
      cylinder { <0,0,0>, y, 1 scale <2.8 0.7 3.3> translate <0 0.9 0>  }
      cylinder { <0,0,0>, y, 1 scale <2.8 0.7 3.3> translate <0 -0.7 0>  }
      intersection { DCone scale <0.4 1.5 0.4> translate <-2 -0.2 -2>  }
      intersection { DCone scale <0.4 1.5 0.4> translate <-2 -0.2 2>  }
   }       
   texture { color White phong 1.0  }
}


/*----------------THE SKULL---------------------------------------------*/



#declare Connector_1c = 
object {
   quadric {
      < 0.0  1.0  1.0>
      < 0.0  0.0  0.0>
      < 0.0  0.0  0.0>
      -1.0
   }      
   clipped_by {
      plane { < 1.0  0.0  0.0> 1.0  }
      plane { <-1.0  0.0  0.0> 0.0  }
   }     
}


#declare Connector_2c =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

#declare skull = 

union {
   difference {
      union {
         quadric { Dot scale < 3.200000  6.200000  6.200000>  }

         quadric { Dot scale < 6.500000  6.500000  6.500000>
            translate < 3.000000 -0.100000  0.000000>
         }                  

         /*------SOCKET MOUND-----------------------------------------*/
         quadric { Ellipsoid scale <2 2 2> translate <1 3.2 -4>  }
         /*
{--------------JAW----------------------------------------------------}
quadric { Ellipsoid scale <4 4 2> translate <2.3 -0.5 -4.7> }
*/
      }            


      /*----------------Eye Sockets------------------------------------------*/
      quadric { Cylinder_X scale <1 1.3 1.3> translate <0 3.1 3.9>  }
      quadric { Cylinder_X scale <1 1.3 1.3> translate <0 3.1 -3.9>  }
   }      
}



#declare c4 = intersection {
   quadric { Connector_2c  } 
   plane {     < 1.000000  0.000000  0.000000>  6.467455  }
   plane {     < 1.000000  0.000000  0.000000>  6.168957 inverse  }

   translate <-6.168957  0.000000  0.000000>
   scale     < 9.955456  1.000000  1.000000>
   translate < 0.619656  0.000000  0.000000>
   rotate    < 0.000000  0.000000 -1.909152>
}


#declare eye = /*---------------EYE BALL----------------*/
object {
   quadric { Ellipsoid scale <1.2 1 1.2>  }
   texture {
      /*
image_map { <1.0 -1.0 0> gif "pantheye.gif" }
*/
      scale <2 1.8 1> translate <-1 -0.9 -1> color Black phong 1.0

   }      
   rotate <0 50 0>
}

#declare ear = difference {   /*-----EAR------*/
   quadric { Ellipsoid scale <1.75 3.5 1.75>  }
   quadric { Ellipsoid scale <1.25 3.5 1.25> translate <-0.7 -0.5 0>  }
   rotate <0 -65 0>
}

/*---------------------------A Matching set---------------------------------*/

#declare ears = 
union {
   difference { ear rotate <0 0 -30> rotate <30 0 0> translate <0 0 6>  }
   difference { ear rotate <0 0 -30> rotate <-30 0 0> translate <0 0 -6>  }
}


#declare bridge =  /*----------NOSE & BRIDGE------*/
union {
   cylinder { <0,0,0>, x, 1 scale <6.5 1.2 1.2>  }
   quadric { Ellipsoid scale <1 1.5 1.5>  }
   quadric { Ellipsoid scale <1.3 1.3 1.3> translate <0.2 0 0>  }
}



#declare ed = 
object {
   union {
      union { nose scale <1 1 1.4> rotate <0 0 7> translate <-42.5 24.3 0>  }
      union { skull scale <1.2 1 1.2> rotate <0 0 4> translate <-38 26 0>  }
      union { neck scale <1 1 0.8> translate <-33 26.5 0>  }
      intersection { c1 translate <-33 26.5 0>  }
      intersection { c2 translate <-33 26.5 0>  }
      intersection { c4 scale <1.2 1 1.2> rotate <0 0 4> translate <-38 26 0>  }
      union { ears  translate <-33 29 0>  }
      union { bridge scale <1 1 3> rotate <0 0 16> translate <-44 27.4 0>  }
      union { gum translate <-41 23.2 0>  }


   }      
   texture { 
      /* image_map { <1.0 -1.0 0> gif "frac3.gif"  } */ 
      color green 0.4 red 0 blue 0
      ambient 0.4 diffuse 0.7 phong 0.8 phong_size 25
      scale <36 22 2> rotate <0 0 180> translate <-18 -11 -2>
   }
}

#declare head =
composite {
   object { ed  }
   object { eye translate <-39 28.3 -6.3>  }
   object { eye translate <-39 28.3 6.3>  }

   object { choppers translate <-41 23.2 0>  }
}


/*************************************************************************/
/*---------------------------- LEG1 -------------------------------------*/


#declare Connector_1 = 
object {
   quadric {
      < 0.0  1.0  1.0>
      < 0.0  0.0  0.0>
      < 0.0  0.0  0.0>
      -1.0
   }      
   clipped_by {
      plane { < 1.0  0.0  0.0> 1.0  }
      plane { <-1.0  0.0  0.0> 0.0  }
   }     
}


#declare Connector_2 =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

#declare leg011 =
composite {
   object {
      sphere { < 1.000000  3.000000  0.000000>  6.000000  }
      texture {
         pan1
      }            
   }      
   object {
      Connector_1
      scale     < 3.162278  6.000000  6.000000>
      rotate    < 0.000000  0.000000 -71.565051>
      rotate    < 0.000000 -180.000000  0.000000>
      translate < 1.000000  3.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 0.000000  0.000000  0.000000>  6.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.541289  }
         plane {     < 1.000000  0.000000  0.000000>  3.232419 inverse  }
      }           
      translate <-5.541289  0.000000  0.000000>
      scale     < 2.408319  1.000000  1.000000>
      translate <-2.300895  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -57.528808>
      rotate    < 0.000000 -180.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-3.500000 -5.500000  0.000000>  3.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  3.481912  }
         plane {     < 1.000000  0.000000  0.000000>  2.984496 inverse  }
      }           
      translate <-3.481912  0.000000  0.000000>
      scale     < 9.797959  1.000000  1.000000>
      translate <-0.355371  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -23.962489>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-3.500000 -5.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-8.000000 -7.500000  0.000000>  3.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.986864  }
         plane {     < 1.000000  0.000000  0.000000>  2.489053 inverse  }
      }           
      translate <-2.986864  0.000000  0.000000>
      scale     < 10.650822  1.000000  1.000000>
      translate <-0.280435  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -20.806791>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-8.000000 -7.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-13.000000 -9.400000  0.000000>  2.500000  }
      texture {
         pan1
      }            
   }      
   object {
      Connector_1
      scale     < 3.848376  2.500000  2.500000>
      rotate    < 0.000000  0.000000 -24.567171>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-13.000000 -9.400000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-16.500000 -11.000000  0.000000>  2.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2 
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  3.357082  }
         plane {     < 1.000000  0.000000  0.000000>  2.397916 inverse  }
      }           
      translate <-2.397916  0.000000  0.000000>
      scale     < 3.391165  1.000000  1.000000>
      translate < 0.707107  0.000000  0.000000>
      rotate    < 0.000000  0.000000 -8.130102>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-16.500000 -11.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-20.000000 -11.500000  0.000000>  3.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  3.300744  }
         plane {     < 1.000000  0.000000  0.000000>  2.357674 inverse  }
      }           
      translate <-3.300744  0.000000  0.000000>
      scale     < 2.835489  1.000000  1.000000>
      translate <-1.164083  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -3.814075>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-20.000000 -11.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-23.000000 -11.700000  0.000000>  2.500000  }
      texture {
         pan1
      }            
   }      

   /*-----------------------PLACE CLAWS------------------------------------*/



}

#declare leg01 = 
composite {
   composite { leg011  }

}


/************************************************************************/
/*-----------------------------  LEG 2 ---------------------------------*/

#declare Connector_1 = 
object {
   quadric {
      < 0.0  1.0  1.0>
      < 0.0  0.0  0.0>
      < 0.0  0.0  0.0>
      -1.0
   }      
   clipped_by {
      plane { < 1.0  0.0  0.0> 1.0  }
      plane { <-1.0  0.0  0.0> 0.0  }
   }     
}


#declare Connector_2 =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

#declare leg02 =
composite {
   object {
      sphere { < 0.000000  0.000000  1.000000>  4.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  4.048809  }
         plane {     < 1.000000  0.000000  0.000000>  3.149074 inverse  }
      }           
      translate <-4.048809  0.000000  0.000000>
      scale     < 2.061553  1.000000  1.000000>
      translate <-1.963961  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -60.794068>
      rotate    < 0.000000  116.565051  0.000000>
      translate < 0.000000  0.000000  1.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-0.500000 -2.000000  0.000000>  3.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  3.299832  }
         plane {     < 1.000000  0.000000  0.000000>  2.357023 inverse  }
      }           
      translate <-3.299832  0.000000  0.000000>
      scale     < 2.828427  1.000000  1.000000>
      translate <-1.166667  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -90.000000>
      translate <-0.500000 -2.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-0.500000 -5.000000  0.000000>  2.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2 
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.934174  }
         plane {     < 1.000000  0.000000  0.000000>  2.445145 inverse  }
      }           
      translate <-2.445145  0.000000  0.000000>
      scale     < 4.694678  1.000000  1.000000>
      translate < 0.520833  0.000000  0.000000>
      rotate    < 0.000000  0.000000 -90.000000>
      translate <-0.500000 -5.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-0.500000 -7.400000  0.000000>  3.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.910213  }
         plane {     < 1.000000  0.000000  0.000000>  1.940142 inverse  }
      }           
      translate <-2.910213  0.000000  0.000000>
      scale     < 3.994997  1.000000  1.000000>
      translate <-0.728464  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -60.945396>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-0.500000 -7.400000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-2.500000 -11.000000  0.000000>  2.000000  }
      texture {
         pan1
      }            
   }      
   object {
      Connector_1
      scale     < 2.828427  2.000000  2.000000>
      rotate    < 0.000000  0.000000 -45.000000>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-2.500000 -11.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-4.500000 -13.000000  0.000000>  2.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2 
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.759599  }
         plane {     < 1.000000  0.000000  0.000000>  1.839732 inverse  }
      }           
      translate <-1.839732  0.000000  0.000000>
      scale     < 2.345208  1.000000  1.000000>
      translate < 0.784465  0.000000  0.000000>
      rotate    < 0.000000  0.000000 -11.309932>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-4.500000 -13.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-7.000000 -13.500000  0.000000>  3.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.598076  }
         plane {     < 1.000000  0.000000  0.000000>  1.732051 inverse  }
      }           
      translate <-2.598076  0.000000  0.000000>
      scale     < 1.732051  1.000000  1.000000>
      translate <-1.500000  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -180.000000  0.000000>
      translate <-7.000000 -13.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { <-9.000000 -13.500000  0.000000>  2.000000  }
      texture {
         pan1
      }            
   }      
}

/***************************************************************/
/*----------------------------TAIL--------------------------*/

#declare Connector_1 = 
object {
   quadric {
      < 0.0  1.0  1.0>
      < 0.0  0.0  0.0>
      < 0.0  0.0  0.0>
      -1.0
   }      
   clipped_by {
      plane { < 1.0  0.0  0.0> 1.0  }
      plane { <-1.0  0.0  0.0> 0.0  }
   }     
}

#declare Connector_2 =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

#declare Panther_Tail =
composite {
   object {
      sphere { < 0.000000  0.000000  0.000000>  2.600000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.596073  }
         plane {     < 1.000000  0.000000  0.000000>  2.396375 inverse  }
      }           
      translate <-2.596073  0.000000  0.000000>
      scale     < 18.172782  1.000000  1.000000>
      translate <-0.142855  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -15.945396>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 3.500000 -1.000000  0.000000>  2.400000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.396687  }
         plane {     < 1.000000  0.000000  0.000000>  2.196963 inverse  }
      }           
      translate <-2.396687  0.000000  0.000000>
      scale     < 19.013153  1.000000  1.000000>
      translate <-0.126054  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -23.198591>
      translate < 3.500000 -1.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 7.000000 -2.500000  0.000000>  2.200000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  2.197113  }
         plane {     < 1.000000  0.000000  0.000000>  1.997375 inverse  }
      }           
      translate <-2.197113  0.000000  0.000000>
      scale     < 19.500000  1.000000  1.000000>
      translate <-0.112672  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -39.805571>
      translate < 7.000000 -2.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 10.000000 -5.000000  0.000000>  2.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.997375  }
         plane {     < 1.000000  0.000000  0.000000>  1.797638 inverse  }
      }           
      translate <-1.997375  0.000000  0.000000>
      scale     < 19.500000  1.000000  1.000000>
      translate <-0.102430  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -50.194429>
      translate < 10.000000 -5.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 12.500000 -8.000000  0.000000>  1.800000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.797783  }
         plane {     < 1.000000  0.000000  0.000000>  1.598030 inverse  }
      }           
      translate <-1.797783  0.000000  0.000000>
      scale     < 20.130822  1.000000  1.000000>
      translate <-0.089305  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -60.255119>
      translate < 12.500000 -8.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 14.500000 -11.500000  0.000000>  1.600000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.599448  }
         plane {     < 1.000000  0.000000  0.000000>  1.499483 inverse  }
      }           
      translate <-1.599448  0.000000  0.000000>
      scale     < 38.065733  1.000000  1.000000>
      translate <-0.042018  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -66.801409>
      translate < 14.500000 -11.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 16.000000 -15.000000  0.000000>  1.500000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.499367  }
         plane {     < 1.000000  0.000000  0.000000>  1.399410 inverse  }
      }           
      translate <-1.499367  0.000000  0.000000>
      scale     < 34.423829  1.000000  1.000000>
      translate <-0.043556  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -64.179008>
      translate < 16.000000 -15.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 17.500000 -18.100000  0.000000>  1.400000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.399126  }
         plane {     < 1.000000  0.000000  0.000000>  1.299188 inverse  }
      }           
      translate <-1.399126  0.000000  0.000000>
      scale     < 28.284271  1.000000  1.000000>
      translate <-0.049467  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -57.994617>
      translate < 17.500000 -18.100000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 19.000000 -20.500000  0.000000>  1.300000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.299235  }
         plane {     < 1.000000  0.000000  0.000000>  1.199294 inverse  }
      }           
      translate <-1.299235  0.000000  0.000000>
      scale     < 29.137605  1.000000  1.000000>
      translate <-0.044590  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -59.036243>
      translate < 19.000000 -20.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 20.500000 -23.000000  0.000000>  1.200000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.199414  }
         plane {     < 1.000000  0.000000  0.000000>  1.099463 inverse  }
      }           
      translate <-1.199414  0.000000  0.000000>
      scale     < 32.000000  1.000000  1.000000>
      translate <-0.037482  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -51.340192>
      translate < 20.500000 -23.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 22.500000 -25.500000  0.000000>  1.100000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.099450  }
         plane {     < 1.000000  0.000000  0.000000>  0.999500 inverse  }
      }           
      translate <-1.099450  0.000000  0.000000>
      scale     < 31.606961  1.000000  1.000000>
      translate <-0.034785  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000 -18.434949>
      translate < 22.500000 -25.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 25.500000 -26.500000  0.000000>  1.000000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  0.997775  }
         plane {     < 1.000000  0.000000  0.000000>  0.798220 inverse  }
      }           
      translate <-0.997775  0.000000  0.000000>
      scale     < 14.966630  1.000000  1.000000>
      translate <-0.066667  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      translate < 25.500000 -26.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 28.500000 -26.500000  0.000000>  0.800000  }
      texture {
         pan1
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  0.798115  }
         plane {     < 1.000000  0.000000  0.000000>  0.598587 inverse  }
      }           
      translate <-0.798115  0.000000  0.000000>
      scale     < 14.543040  1.000000  1.000000>
      translate <-0.054880  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  30.963757>
      translate < 28.500000 -26.500000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 31.000000 -25.000000  0.000000>  0.600000  }
      texture {
         pan1
      }            
   }      
   object {
      Connector_1
      scale     < 2.236068  0.600000  0.600000>
      rotate    < 0.000000  0.000000  63.434949>
      translate < 31.000000 -25.000000  0.000000>
      texture {
         pan1
      }            
   }      
   object {
      sphere { < 32.000000 -23.000000  0.000000>  0.600000  }
      texture {
         pan1
      }            
   }      
}



/*******************************************************************/
/*-------------------------THE BODY--------------------------------*/

#declare s0z = quadric { Cylinder_Z scale <0.9 0.2 0.2>  }

#declare x01 = union {   /*--The New & Improved Space Age Solution--*/
   difference {
      union {
         quadric { s0z rotate <0 0 0> translate <0 -0.8 0>  }
         quadric { s0z rotate <0 0 0> translate <0 0.8 0>  }
         quadric { s0z rotate <0 0 90> translate <-0.8 0 0>  }
         quadric { s0z rotate <0 0 90> translate <0.8 0 0>  }
      }            
      plane { <0 0 1> -1  } plane { <0 0 1> 1 inverse  }
   }       
}

#declare x01x = union { x01 rotate <0 90 0>  }
#declare x01y = union { x01 rotate <90 0 0>  }
#declare x01z = union { x01  }

#declare hp07 = object {    /*---------The Main Bod---------------------*/
   union {
      quadric { Ellipsoid scale <25.4 9 8.5> rotate <0 0 10> translate <1.9 20.6 0>  }
      quadric { Ellipsoid scale <11 8 8> rotate <0 0 10> translate <16.5 22.9 0>  }
      quadric { Ellipsoid scale <11 6.5 6.5> rotate <0 0 15> translate <-15.5 22 0>  }
      quadric { Ellipsoid scale <6 5.5 4> translate <-11.5 14.5 4>  }
      quadric { Ellipsoid scale <6 5.5 4> translate <-16.5 14.5 -4>  }
      quadric { Ellipsoid scale <14.2 9.75 8.5> translate <-7.2 18.7 0>  }
      quadric { Ellipsoid scale <14 6.5 6.5> translate <-18 22.5 0>  }
      quadric { Ellipsoid scale <6 6 6> translate <-14.5 23 -3.1>  }
   }      
   texture { pan1  }
}

#declare lp02 = object {   /*-----------BACK HIP--------------------------*/
   union { 
      quadric { Ellipsoid scale <6.4 8.7 4.4> rotate <0 0 -30> translate <14.6 19.6 -6>  }
      quadric { Ellipsoid scale <6.4 8.8 4.4> rotate <0 0 -40> translate <18 20 -6>  }
   }      
   texture { pan1  }
}

#declare lp03 = object {   /*-----------FRONT Back Leg-------------------*/

   union {
      quadric { Ellipsoid scale <4 3 3> translate <-3.7 2.0 6>  }
      quadric { Ellipsoid scale <3 3 2> translate <-6 1.8 6>  }
      cylinder { <0,0,0>, y, 1 scale <2.5 13 2.5> translate <0 -6.5 0>
         rotate <0 0 10> translate <-3.1 7.9 6>  }
   }      
   texture { pan1  }
}

#declare lp04 = object {   /*-----------Back Stretched Leg----*/
   union {
      quadric { Ellipsoid scale <5.2 9.5 3.25> rotate <0 0 61> translate <25.6 16.5 6>  }
      quadric { Ellipsoid scale <7.25 7 3.5> translate <22.2 20.5 6.5>  }
      quadric { Ellipsoid scale <4.5 2.5 2> translate <35 11 6>  }
      quadric { Ellipsoid scale <4 3.5 4> translate <40 1 6>  }
      quadric { Ellipsoid scale <2 3 2> translate <37 0.8 6>  }
      cylinder { <0,0,0>, y, 1 scale <1.8 11.5 2> translate <0 -5.75 0>
         rotate <0 0 27> translate <39.7 5.7 6>  }
   }      
   texture { pan1  }
}




/**********************************************************************/
/*--------------------PUT IT TOGETHER---------------------------------*/
#declare panther = composite {

   object { hp07  }
   object { lp02  }
   object { lp03  }
   object { lp04  }
   composite { Panther_Tail translate <20 28 0>  }
   composite { leg01 translate <-17 15 -6>  }
   composite { leg02 translate <13 15.5 -6>  }
   composite { head translate <0 0 0>  }
}

#version Panther_Inc_Temp;
