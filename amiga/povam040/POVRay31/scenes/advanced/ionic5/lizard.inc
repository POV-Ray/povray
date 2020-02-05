// Persistence Of Vision raytracer version 3.1 sample file.
#include "colors.inc"

#declare Lizard_Inc_Temp = version;
#version 1.0;


/*-----------DECLARE SKINS FOR CTDS DATA --------------------------------*/


#declare   skin = texture {
   gradient
   <1 0 0>
   colour_map {
      [0.000 0.051   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  1.000  green  0.698  blue  0.000  alpha  0.000]
      [0.051 0.094   colour red  1.000  green  0.698  blue  0.000  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.094 0.145   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.153  green  0.090  blue  0.169  alpha  0.000]
      [0.145 0.265   colour red  0.153  green  0.090  blue  0.169  alpha  0.000
      colour red  0.475  green  0.278  blue  0.522  alpha  0.000]
      [0.265 0.906   colour red  0.475  green  0.278  blue  0.522  alpha  0.000
      colour red  0.475  green  0.224  blue  0.227  alpha  0.000]
      [0.906 1.001   colour red  0.475  green  0.224  blue  0.227  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
   }
   0.1

   ambient 0.3 phong 1.0
   rotate <0 0 0>
   scale <0.2 1 0.2>
}


#declare shead = texture {
   gradient
   <1 0 0>
   colour_map {
      [0.000 0.179   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.286  green  0.133  blue  0.137  alpha  0.000]
      [0.179 0.308   colour red  0.286  green  0.133  blue  0.137  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.308 0.368   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.153  green  0.090  blue  0.169  alpha  0.000]
      [0.368 0.479   colour red  0.153  green  0.090  blue  0.169  alpha  0.000
      colour red  0.353  green  0.212  blue  0.388  alpha  0.000]
      [0.479 0.906   colour red  0.353  green  0.212  blue  0.388  alpha  0.000
      colour red  0.475  green  0.224  blue  0.227  alpha  0.000]
      [0.906 1.001   colour red  0.475  green  0.224  blue  0.227  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
   }

   ambient 0.3 phong 1.0
   0.1
   rotate <0 0 0>
   scale <26 15 5> translate <9 0 0>
}



#declare bones = texture {
   colour red  0.475  green  0.224  blue  0.227
}

#declare Dot =
quadric {
   < 1.0  1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   -1.0
}

/*  ***** cylindrical connector *****  */

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

/*  ***** conical connector *****  */

#declare Connector_2 =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

#version Lizard_Inc_Temp;
