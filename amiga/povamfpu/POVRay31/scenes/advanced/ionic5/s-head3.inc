// Persistence Of Vision raytracer version 3.1 sample file.
#include "lizard.inc"

#declare SHead3_Inc_Temp = version;
#version 1.0;


/*---This dat is the top "head" half of snake------*/


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



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
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
   cylinder { <0,0,0>, y, 1 scale <3 0.7 3.5> translate <-1 1.3 0>  }
   cylinder { <0,0,0>, y, 1 scale <3 0.7 3.5> translate <-0.3 -1.2 0>  }
}

/*-----------Poodle Chew - Poodle Bite-----------------------------------*/
#declare choppers = object {
   union {
      cylinder { <0,0,0>, y, 1 scale <2.8 0.7 3.6> translate <-1 1.1 0>  }
      cylinder { <0,0,0>, y, 1 scale <2.8 0.7 3.6> translate <-0.5 -0.95 0>  }
   }      
   texture { color Firebrick phong 1.0  }
}



#declare fangs = object {
   union {
      intersection { DCone scale <0.5 1.9 0.5> translate <-2.5 -0.5 -2.5>  }
      intersection { DCone scale <0.5 1.9 0.5> translate <-2.5 -0.5 2.5>  }
   }       
   texture { color White phong 1.0  }
}





/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
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
quadric { Ellipsoid SCALE <4 4 2> tranlate <2.3 -0.5 -4.7> }
*/
      }            

      /*
{----------------Eye Sockets------------------------------------------}
quadric Cylinder_X scale <1 1.3 1.3> translate <0 3.1 3.9> }
quadric Cylinder_X scale <1 1.3 1.3> translate <0 3.1 -3.9> }
*/
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
   quadric { Ellipsoid scale <1.5 1.5 1.5>  }
   texture {
      /*
image_map <1 -1.0 0> gif "s-eye.gif" }
scale <3 3 3> translate <-1.5 -1.5 -1> phong 1.0
rotate <0 25 0>
*/
      color Black phong 1.0
   }      
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
      intersection { c4 scale <1.2 1 1.2> rotate <0 0 4> translate <-38 26 0>  }
      union { ears  translate <-33 29 0>  }
      union { bridge scale <1 1 3> rotate <0 0 16> translate <-44 27.4 0>  }
      union { gum translate <-41 23.2 0>  }


   }      


   texture {
      shead
   }      



}

#declare head =
composite {
   object { ed  }
   object { eye translate <-39 28.3 -6.3>  }
   object { eye translate <-39 28.3 6.3>  }
   object { choppers translate <-41 23.2 0>  }
   object { fangs translate <-41 23.2 0>  }

   translate <38 -26 0>

}



/*-----STRIPPED CTDS DATA / MOVED TEXTURE STATEMENT BEFORE SCALE/TRANS.--*/



#declare top =
composite {
   object {
      quadric {
         Dot  } texture { bones  } 
      scale     < 9.000000  9.000000  9.000000>
      translate < 23.000000  0.000000  0.000000>
   }      




















   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.998481  }
         plane {     < 1.000000  0.000000  0.000000>  8.922304 inverse  }
      }            
      translate <-8.998481  0.000000  0.000000>
      scale     < 54.415615  1.000000  1.000000>
      translate <-0.165366  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  7.920896>
      rotate    < 0.000000 -99.646625  0.000000>
      translate < 23.000000  0.000000  0.000000>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.923810  8.923810  8.923810>
      translate < 22.311775  0.571429  4.048990>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.922261  }
         plane {     < 1.000000  0.000000  0.000000>  8.846083 inverse  }
      }            
      translate <-8.922261  0.000000  0.000000>
      scale     < 53.662584  1.000000  1.000000>
      translate <-0.166266  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.032618>
      rotate    < 0.000000 -109.996853  0.000000>
      translate < 22.311775  0.571429  4.048990>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.847619  8.847619  8.847619>
      translate < 20.927078  1.142857  7.854064>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.846039  }
         plane {     < 1.000000  0.000000  0.000000>  8.769863 inverse  }
      }            
      translate <-8.846039  0.000000  0.000000>
      scale     < 52.911216  1.000000  1.000000>
      translate <-0.167186  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.147536>
      rotate    < 0.000000 -120.348924  0.000000>
      translate < 20.927078  1.142857  7.854064>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.771429  8.771429  8.771429>
      translate < 18.910401  1.714286  11.298438>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.769817  }
         plane {     < 1.000000  0.000000  0.000000>  8.693640 inverse  }
      }            
      translate <-8.769817  0.000000  0.000000>
      scale     < 52.158772  1.000000  1.000000>
      translate <-0.168137  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.265722>
      rotate    < 0.000000 -130.702976  0.000000>
      translate < 18.910401  1.714286  11.298438>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.695238  8.695238  8.695238>
      translate < 16.345237  2.285714  14.280403>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.693593  }
         plane {     < 1.000000  0.000000  0.000000>  8.617418 inverse  }
      }            
      translate <-8.693593  0.000000  0.000000>
      scale     < 51.407990  1.000000  1.000000>
      translate <-0.169110  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.387381>
      rotate    < 0.000000 -141.059043  0.000000>
      translate < 16.345237  2.285714  14.280403>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.619048  8.619048  8.619048>
      translate < 13.330806  2.857143  16.716302>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.617369  }
         plane {     < 1.000000  0.000000  0.000000>  8.541193 inverse  }
      }            
      translate <-8.617369  0.000000  0.000000>
      scale     < 50.656162  1.000000  1.000000>
      translate <-0.170115  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.512606>
      rotate    < 0.000000 -151.417270  0.000000>
      translate < 13.330806  2.857143  16.716302>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.542857  8.542857  8.542857>
      translate < 9.978320  3.428571  18.542826>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.541143  }
         plane {     < 1.000000  0.000000  0.000000>  8.464968 inverse  }
      }            
      translate <-8.541143  0.000000  0.000000>
      scale     < 49.905998  1.000000  1.000000>
      translate <-0.171145  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.641613>
      rotate    < 0.000000 -161.777707  0.000000>
      translate < 9.978320  3.428571  18.542826>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.466667  8.466667  8.466667>
      translate < 6.406952  4.000000  19.718572>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.464915  }
         plane {     < 1.000000  0.000000  0.000000>  8.388740 inverse  }
      }            
      translate <-8.464915  0.000000  0.000000>
      scale     < 49.154855  1.000000  1.000000>
      translate <-0.172209  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.774531>
      rotate    < 0.000000 -172.140486  0.000000>
      translate < 6.406952  4.000000  19.718572>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.390476  8.390476  8.390476>
      translate < 2.739637  4.571429  20.224813>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.388686  }
         plane {     < 1.000000  0.000000  0.000000>  8.312512 inverse  }
      }            
      translate <-8.388686  0.000000  0.000000>
      scale     < 48.405368  1.000000  1.000000>
      translate <-0.173301  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  8.911533>
      rotate    < 0.000000  177.494280  0.000000>
      translate < 2.739637  4.571429  20.224813>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.314286  8.314286  8.314286>
      translate <-0.901142  5.142857  20.065489>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.312456  }
         plane {     < 1.000000  0.000000  0.000000>  8.236282 inverse  }
      }            
      translate <-8.312456  0.000000  0.000000>
      scale     < 47.654995  1.000000  1.000000>
      translate <-0.174430  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  9.052858>
      rotate    < 0.000000  167.126515  0.000000>
      translate <-0.901142  5.142857  20.065489>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.238095  8.238095  8.238095>
      translate <-4.397438  5.714286  19.266433>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.236224  }
         plane {     < 1.000000  0.000000  0.000000>  8.160051 inverse  }
      }            
      translate <-8.236224  0.000000  0.000000>
      scale     < 46.906246  1.000000  1.000000>
      translate <-0.175589  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  9.198650>
      rotate    < 0.000000  156.756007  0.000000>
      translate <-4.397438  5.714286  19.266433>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.161905  8.161905  8.161905>
      translate <-7.639658  6.285714  17.873868>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.159990  }
         plane {     < 1.000000  0.000000  0.000000>  8.083817 inverse  }
      }            
      translate <-8.159990  0.000000  0.000000>
      scale     < 46.156681  1.000000  1.000000>
      translate <-0.176789  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  9.349184>
      rotate    < 0.000000  146.382730  0.000000>
      translate <-7.639658  6.285714  17.873868>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.085714  8.085714  8.085714>
      translate <-10.530002  6.857143  15.952272>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.083754  }
         plane {     < 1.000000  0.000000  0.000000>  8.007582 inverse  }
      }            
      translate <-8.083754  0.000000  0.000000>
      scale     < 45.408774  1.000000  1.000000>
      translate <-0.178022  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  9.504621>
      rotate    < 0.000000  136.006454  0.000000>
      translate <-10.530002  6.857143  15.952272>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 8.009524  8.009524  8.009524>
      translate <-12.985396  7.428571  13.581660>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  8.007517  }
         plane {     < 1.000000  0.000000  0.000000>  7.931345 inverse  }
      }            
      translate <-8.007517  0.000000  0.000000>
      scale     < 44.660129  1.000000  1.000000>
      translate <-0.179299  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  9.665273>
      rotate    < 0.000000  125.627061  0.000000>
      translate <-12.985396  7.428571  13.581660>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.933333  7.933333  7.933333>
      translate <-14.939847  8.000000  10.854434>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.931277  }
         plane {     < 1.000000  0.000000  0.000000>  7.855107 inverse  }
      }            
      translate <-7.931277  0.000000  0.000000>
      scale     < 43.913108  1.000000  1.000000>
      translate <-0.180613  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  9.831356>
      rotate    < 0.000000  115.244396  0.000000>
      translate <-14.939847  8.000000  10.854434>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.857143  7.857143  7.857143>
      translate <-16.346149  8.571429  7.871891>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.855035  }
         plane {     < 1.000000  0.000000  0.000000>  7.778865 inverse  }
      }            
      translate <-7.855035  0.000000  0.000000>
      scale     < 43.165467  1.000000  1.000000>
      translate <-0.181975  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  10.003133>
      rotate    < 0.000000  104.858271  0.000000>
      translate <-16.346149  8.571429  7.871891>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.780952  7.780952  7.780952>
      translate <-17.176900  9.142857  4.740523>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.778791  }
         plane {     < 1.000000  0.000000  0.000000>  7.702622 inverse  }
      }            
      translate <-7.778791  0.000000  0.000000>
      scale     < 42.419466  1.000000  1.000000>
      translate <-0.183378  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  10.180965>
      rotate    < 0.000000  94.468488  0.000000>
      translate <-17.176900  9.142857  4.740523>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.704762  7.704762  7.704762>
      translate <-17.424807  9.714286  1.568261>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.702545  }
         plane {     < 1.000000  0.000000  0.000000>  7.626376 inverse  }
      }            
      translate <-7.702545  0.000000  0.000000>
      scale     < 41.672914  1.000000  1.000000>
      translate <-0.184833  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  10.365089>
      rotate    < 0.000000  84.074871  0.000000>
      translate <-17.424807  9.714286  1.568261>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.628571  7.628571  7.628571>
      translate <-17.102301  10.285714 -1.539235>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.626295  }
         plane {     < 1.000000  0.000000  0.000000>  7.550128 inverse  }
      }            
      translate <-7.626295  0.000000  0.000000>
      scale     < 40.928041  1.000000  1.000000>
      translate <-0.186334  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  10.555919>
      rotate    < 0.000000  73.677191  0.000000>
      translate <-17.102301  10.285714 -1.539235>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.552381  7.552381  7.552381>
      translate <-16.240479  10.857143 -4.482087>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.550043  }
         plane {     < 1.000000  0.000000  0.000000>  7.473876 inverse  }
      }            
      translate <-7.550043  0.000000  0.000000>
      scale     < 40.182722  1.000000  1.000000>
      translate <-0.187893  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  10.753739>
      rotate    < 0.000000  63.275207  0.000000>
      translate <-16.240479  10.857143 -4.482087>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.476190  7.476190  7.476190>
      translate <-14.887438  11.428571 -7.169412>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.473788  }
         plane {     < 1.000000  0.000000  0.000000>  7.397622 inverse  }
      }            
      translate <-7.473788  0.000000  0.000000>
      scale     < 39.439113  1.000000  1.000000>
      translate <-0.189502  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  10.959016>
      rotate    < 0.000000  52.868670  0.000000>
      translate <-14.887438  11.428571 -7.169412>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.400000  7.400000  7.400000>
      translate <-13.106075  12.000000 -9.522121>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.397530  }
         plane {     < 1.000000  0.000000  0.000000>  7.321366 inverse  }
      }            
      translate <-7.397530  0.000000  0.000000>
      scale     < 38.695660  1.000000  1.000000>
      translate <-0.191172  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  11.172111>
      rotate    < 0.000000  42.457327  0.000000>
      translate <-13.106075  12.000000 -9.522121>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.323810  7.323810  7.323810>
      translate <-10.971442  12.571429 -11.475229>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.321269  }
         plane {     < 1.000000  0.000000  0.000000>  7.245104 inverse  }
      }            
      translate <-7.321269  0.000000  0.000000>
      scale     < 37.952435  1.000000  1.000000>
      translate <-0.192906  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  11.393472>
      rotate    < 0.000000  32.040864  0.000000>
      translate <-10.971442  12.571429 -11.475229>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.247619  7.247619  7.247619>
      translate <-8.567760  13.142857 -12.979601>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.245003  }
         plane {     < 1.000000  0.000000  0.000000>  7.168841 inverse  }
      }            
      translate <-7.245003  0.000000  0.000000>
      scale     < 37.211005  1.000000  1.000000>
      translate <-0.194701  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  11.623633>
      rotate    < 0.000000  21.618968  0.000000>
      translate <-8.567760  13.142857 -12.979601>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.171429  7.171429  7.171429>
      translate <-5.985210  13.714286 -14.003094>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.168735  }
         plane {     < 1.000000  0.000000  0.000000>  7.092572 inverse  }
      }            
      translate <-7.168735  0.000000  0.000000>
      scale     < 36.469407  1.000000  1.000000>
      translate <-0.196568  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  11.863040>
      rotate    < 0.000000  11.191309  0.000000>
      translate <-5.985210  13.714286 -14.003094>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.095238  7.095238  7.095238>
      translate <-3.316622  14.285714 -14.531068>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.092461  }
         plane {     < 1.000000  0.000000  0.000000>  7.016301 inverse  }
      }            
      translate <-7.092461  0.000000  0.000000>
      scale     < 35.729659  1.000000  1.000000>
      translate <-0.198503  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  12.112328>
      rotate    < 0.000000  0.757501  0.000000>
      translate <-3.316622  14.285714 -14.531068>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 7.019048  7.019048  7.019048>
      translate <-0.654172  14.857143 -14.566270>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  7.016183  }
         plane {     < 1.000000  0.000000  0.000000>  6.940023 inverse  }
      }            
      translate <-7.016183  0.000000  0.000000>
      scale     < 34.989887  1.000000  1.000000>
      translate <-0.200520  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  12.372027>
      rotate    < 0.000000 -9.682860  0.000000>
      translate <-0.654172  14.857143 -14.566270>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.942857  6.942857  6.942857>
      translate < 1.913783  15.428571 -14.128112>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.939900  }
         plane {     < 1.000000  0.000000  0.000000>  6.863742 inverse  }
      }            
      translate <-6.939900  0.000000  0.000000>
      scale     < 34.252035  1.000000  1.000000>
      translate <-0.202613  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  12.642869>
      rotate    < 0.000000 -20.130219  0.000000>
      translate < 1.913783  15.428571 -14.128112>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.866667  6.866667  6.866667>
      translate < 4.305637  16.000000 -13.251387>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.863612  }
         plane {     < 1.000000  0.000000  0.000000>  6.787455 inverse  }
      }            
      translate <-6.863612  0.000000  0.000000>
      scale     < 33.514332  1.000000  1.000000>
      translate <-0.204796  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  12.925501>
      rotate    < 0.000000 -30.585037  0.000000>
      translate < 4.305637  16.000000 -13.251387>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.790476  6.790476  6.790476>
      translate < 6.449127  16.571429 -11.984486>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.787318  }
         plane {     < 1.000000  0.000000  0.000000>  6.711164 inverse  }
      }            
      translate <-6.787318  0.000000  0.000000>
      scale     < 32.778640  1.000000  1.000000>
      translate <-0.207065  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  13.220685>
      rotate    < 0.000000 -41.047890  0.000000>
      translate < 6.449127  16.571429 -11.984486>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.714286  6.714286  6.714286>
      translate < 8.283507  17.142857 -10.387190>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.711019  }
         plane {     < 1.000000  0.000000  0.000000>  6.634865 inverse  }
      }            
      translate <-6.711019  0.000000  0.000000>
      scale     < 32.043317  1.000000  1.000000>
      translate <-0.209436  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  13.529315>
      rotate    < 0.000000 -51.519296  0.000000>
      translate < 8.283507  17.142857 -10.387190>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.638095  6.638095  6.638095>
      translate < 9.761241  17.714286 -8.528139>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.634712  }
         plane {     < 1.000000  0.000000  0.000000>  6.558561 inverse  }
      }            
      translate <-6.634712  0.000000  0.000000>
      scale     < 31.310094  1.000000  1.000000>
      translate <-0.211903  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  13.852225>
      rotate    < 0.000000 -61.999939  0.000000>
      translate < 9.761241  17.714286 -8.528139>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.561905  6.561905  6.561905>
      translate < 10.849158  18.285714 -6.482070>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.558399  }
         plane {     < 1.000000  0.000000  0.000000>  6.482248 inverse  }
      }            
      translate <-6.558399  0.000000  0.000000>
      scale     < 30.577453  1.000000  1.000000>
      translate <-0.214485  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  14.190474>
      rotate    < 0.000000 -72.490498  0.000000>
      translate < 10.849158  18.285714 -6.482070>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.485714  6.485714  6.485714>
      translate < 11.529064  18.857143 -4.326930>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.482077  }
         plane {     < 1.000000  0.000000  0.000000>  6.405930 inverse  }
      }            
      translate <-6.482077  0.000000  0.000000>
      scale     < 29.847095  1.000000  1.000000>
      translate <-0.217176  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  14.545049>
      rotate    < 0.000000 -82.991754  0.000000>
      translate < 11.529064  18.857143 -4.326930>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.409524  6.409524  6.409524>
      translate < 11.797784  19.428571 -2.140982>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.405747  }
         plane {     < 1.000000  0.000000  0.000000>  6.329601 inverse  }
      }            
      translate <-6.405747  0.000000  0.000000>
      scale     < 29.117551  1.000000  1.000000>
      translate <-0.219996  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  14.917222>
      rotate    < 0.000000 -93.504504  0.000000>
      translate < 11.797784  19.428571 -2.140982>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.333333  6.333333  6.333333>
      translate < 11.666667  20.000000  0.000000>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.329408  }
         plane {     < 1.000000  0.000000  0.000000>  6.253265 inverse  }
      }            
      translate <-6.329408  0.000000  0.000000>
      scale     < 28.390495  1.000000  1.000000>
      translate <-0.222941  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  15.308197>
      rotate    < 0.000000 -104.029793  0.000000>
      translate < 11.666667  20.000000  0.000000>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.257143  6.257143  6.257143>
      translate < 11.160573  20.571429  2.025345>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.253059  }
         plane {     < 1.000000  0.000000  0.000000>  6.176918 inverse  }
      }            
      translate <-6.253059  0.000000  0.000000>
      scale     < 27.664556  1.000000  1.000000>
      translate <-0.226031  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  15.719373>
      rotate    < 0.000000 -114.568582  0.000000>
      translate < 11.160573  20.571429  2.025345>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.180952  6.180952  6.180952>
      translate < 10.316417  21.142857  3.871816>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.176699  }
         plane {     < 1.000000  0.000000  0.000000>  6.100561 inverse  }
      }            
      translate <-6.176699  0.000000  0.000000>
      scale     < 26.941351  1.000000  1.000000>
      translate <-0.229265  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  16.152378>
      rotate    < 0.000000 -125.122112  0.000000>
      translate < 10.316417  21.142857  3.871816>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.104762  6.104762  6.104762>
      translate < 9.181314  21.714286  5.485580>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.100327  }
         plane {     < 1.000000  0.000000  0.000000>  6.024191 inverse  }
      }            
      translate <-6.100327  0.000000  0.000000>
      scale     < 26.219599  1.000000  1.000000>
      translate <-0.232663  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  16.608797>
      rotate    < 0.000000 -135.691601  0.000000>
      translate < 9.181314  21.714286  5.485580>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 6.028571  6.028571  6.028571>
      translate < 7.810427  22.285714  6.823765>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  6.023941  }
         plane {     < 1.000000  0.000000  0.000000>  5.947810 inverse  }
      }            
      translate <-6.023941  0.000000  0.000000>
      scale     < 25.500904  1.000000  1.000000>
      translate <-0.236225  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  17.090593>
      rotate    < 0.000000 -146.278622  0.000000>
      translate < 7.810427  22.285714  6.823765>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.952381  5.952381  5.952381>
      translate < 6.264588  22.857143  7.855545>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.947542  }
         plane {     < 1.000000  0.000000  0.000000>  5.871413 inverse  }
      }            
      translate <-5.947542  0.000000  0.000000>
      scale     < 24.784093  1.000000  1.000000>
      translate <-0.239974  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  17.599709>
      rotate    < 0.000000 -156.884732  0.000000>
      translate < 6.264588  22.857143  7.855545>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.876190  5.876190  5.876190>
      translate < 4.607809  23.428571  8.562743>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.871126  }
         plane {     < 1.000000  0.000000  0.000000>  5.795001 inverse  }
      }            
      translate <-5.871126  0.000000  0.000000>
      scale     < 24.070745  1.000000  1.000000>
      translate <-0.243911  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  18.138508>
      rotate    < 0.000000 -167.511829  0.000000>
      translate < 4.607809  23.428571  8.562743>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.800000  5.800000  5.800000>
      translate < 2.904760  24.000000  8.939931>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.794693  }
         plane {     < 1.000000  0.000000  0.000000>  5.718573 inverse  }
      }            
      translate <-5.794693  0.000000  0.000000>
      scale     < 23.360106  1.000000  1.000000>
      translate <-0.248059  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  18.709416>
      rotate    < 0.000000 -178.161997  0.000000>
      translate < 2.904760  24.000000  8.939931>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.723810  5.723810  5.723810>
      translate < 1.218327  24.571429  8.994049>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.718241  }
         plane {     < 1.000000  0.000000  0.000000>  5.642124 inverse  }
      }            
      translate <-5.718241  0.000000  0.000000>
      scale     < 22.652506  1.000000  1.000000>
      translate <-0.252433  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  19.315181>
      rotate    < 0.000000  171.162323  0.000000>
      translate < 1.218327  24.571429  8.994049>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.647619  5.647619  5.647619>
      translate <-0.392674  25.142857  8.743568>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.641767  }
         plane {     < 1.000000  0.000000  0.000000>  5.565656 inverse  }
      }            
      translate <-5.641767  0.000000  0.000000>
      scale     < 21.949166  1.000000  1.000000>
      translate <-0.257038  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  19.958958>
      rotate    < 0.000000  160.458477  0.000000>
      translate <-0.392674  25.142857  8.743568>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.571429  5.571429  5.571429>
      translate <-1.875534  25.714286  8.217250>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.565270  }
         plane {     < 1.000000  0.000000  0.000000>  5.489163 inverse  }
      }            
      translate <-5.565270  0.000000  0.000000>
      scale     < 21.249248  1.000000  1.000000>
      translate <-0.261904  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  20.644019>
      rotate    < 0.000000  149.723245  0.000000>
      translate <-1.875534  25.714286  8.217250>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.495238  5.495238  5.495238>
      translate <-3.185374  26.285714  7.452554>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.488746  }
         plane {     < 1.000000  0.000000  0.000000>  5.412646 inverse  }
      }            
      translate <-5.488746  0.000000  0.000000>
      scale     < 20.554350  1.000000  1.000000>
      translate <-0.267036  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  21.374216>
      rotate    < 0.000000  138.953142  0.000000>
      translate <-3.185374  26.285714  7.452554>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.419048  5.419048  5.419048>
      translate <-4.286503  26.857143  6.493775>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.412194  }
         plane {     < 1.000000  0.000000  0.000000>  5.336099 inverse  }
      }            
      translate <-5.412194  0.000000  0.000000>
      scale     < 19.863787  1.000000  1.000000>
      translate <-0.272465  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  22.153607>
      rotate    < 0.000000  128.143951  0.000000>
      translate <-4.286503  26.857143  6.493775>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.342857  5.342857  5.342857>
      translate <-5.153353  27.428571  5.389985>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.335609  }
         plane {     < 1.000000  0.000000  0.000000>  5.259523 inverse  }
      }            
      translate <-5.335609  0.000000  0.000000>
      scale     < 19.179191  1.000000  1.000000>
      translate <-0.278198  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  22.986927>
      rotate    < 0.000000  117.290809  0.000000>
      translate <-5.153353  27.428571  5.389985>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.266667  5.266667  5.266667>
      translate <-5.770988  28.000000  4.192868>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.258990  }
         plane {     < 1.000000  0.000000  0.000000>  5.182910 inverse  }
      }            
      translate <-5.258990  0.000000  0.000000>
      scale     < 18.500095  1.000000  1.000000>
      translate <-0.284268  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  23.879214>
      rotate    < 0.000000  106.388155  0.000000>
      translate <-5.770988  28.000000  4.192868>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.190476  5.190476  5.190476>
      translate <-6.135169  28.571429  2.954542>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.182330  }
         plane {     < 1.000000  0.000000  0.000000>  5.106260 inverse  }
      }            
      translate <-5.182330  0.000000  0.000000>
      scale     < 17.828219  1.000000  1.000000>
      translate <-0.290681  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  24.836063>
      rotate    < 0.000000  95.429317  0.000000>
      translate <-6.135169  28.571429  2.954542>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.114286  5.114286  5.114286>
      translate <-6.251988  29.142857  1.725439>




   }      


   object {
      quadric {
         Connector_2  } texture { skin  }

      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  5.094784  }
         plane {     < 1.000000  0.000000  0.000000>  4.980934 inverse  }
      }            
      translate <-5.094784  0.000000  0.000000>
      scale     < 11.417999  1.000000  1.000000>
      translate <-0.446206  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  0.000000  25.863872>
      rotate    < 0.000000  84.406457  0.000000>
      translate <-6.251988  29.142857  1.725439>



   }      


   object {
      quadric {
         Dot  } texture { bones  }
      scale     < 5.000000  5.000000  5.000000>
      translate <-6.137099  29.714286  0.552349>




   }      



}

#version SHead3_Inc_Temp;
