// Persistence Of Vision raytracer version 3.1 sample file.
#declare Marble_Inc_Temp = version;
#version 1.0;

// Persistence of Vision Raytracer Version 2.0
/*------- DECLARE TEXTURES FOR MULTI LAYERED MARBLES -----------*/


/*---------------LIGHT CREAM MOSS / GOOD FOR 1st LAYER TONE----------------*/
#declare marble1 = texture {
   marble
   colour_map {
      [0.000 0.154   colour red  0.690  green  0.612  blue  0.569  alpha  0.000
      colour red  0.737  green  0.596  blue  0.522  alpha  0.000]
      [0.154 0.368   colour red  0.737  green  0.596  blue  0.522  alpha  0.000
      colour red  0.776  green  0.702  blue  0.624  alpha  0.000]
      [0.368 0.538   colour red  0.776  green  0.702  blue  0.624  alpha  0.000
      colour red  0.796  green  0.678  blue  0.643  alpha  0.000]
      [0.538 0.846   colour red  0.796  green  0.678  blue  0.643  alpha  0.000
      colour red  0.690  green  0.612  blue  0.569  alpha  0.000]
      [0.846 0.932   colour red  0.690  green  0.612  blue  0.569  alpha  0.000
      colour red  0.773  green  0.612  blue  0.569  alpha  0.000]
      [0.932 1.001   colour red  0.773  green  0.612  blue  0.569  alpha  0.000
      colour red  0.690  green  0.612  blue  0.569  alpha  0.000]
   }      /* end_colour_map */
   turbulence 0.4
   0.1
}/* end_texture */

/*-------------------DEEP THAN MARBLE1 With ALPHA For SECOND LAYERS--------*/
#declare marble2 = texture {
   marble
   colour_map {
      [0.000 0.291   colour red  0.690  green  0.612  blue  0.569  alpha  1.00
      colour red  0.737  green  0.596  blue  0.522  alpha  0.500]
      [0.291 0.316   colour red  0.737  green  0.596  blue  0.522  alpha  0.500
      colour red  0.667  green  0.561  blue  0.514  alpha  0.300]
      [0.316 0.436   colour red  0.667  green  0.561  blue  0.514  alpha  0.300
      colour red  0.788  green  0.667  blue  0.631  alpha  0.500]
      [0.436 0.641   colour red  0.788  green  0.667  blue  0.631  alpha  0.500
      colour red  0.604  green  0.506  blue  0.455  alpha  0.700]
      [0.641 0.692   colour red  0.604  green  0.506  blue  0.455  alpha  0.700
      colour red  0.773  green  0.612  blue  0.569  alpha  0.400]
      [0.692 1.001   colour red  0.773  green  0.612  blue  0.569  alpha  0.400
      colour red  0.690  green  0.612  blue  0.569  alpha  1.000]
   }      /* end_colour_map */
   turbulence 0.5
   0.03
}/* end_texture */


/*-------------------LIGHTER THAN MARBLE2 With ALPHA For SECOND LAYERS--------*/
#declare marble_light = texture {
   marble
   colour_map {
      [0.000 0.291   colour red  0.690  green  0.612  blue  0.569  alpha  0.6
      colour red  0.737  green  0.596  blue  0.522  alpha  0.900]
      [0.291 0.316   colour red  0.737  green  0.596  blue  0.522  alpha  0.900
      colour red  0.667  green  0.561  blue  0.514  alpha  0.700]
      [0.316 0.436   colour red  0.667  green  0.561  blue  0.514  alpha  0.700
      colour red  0.788  green  0.667  blue  0.631  alpha  0.600]
      [0.436 0.641   colour red  0.788  green  0.667  blue  0.631  alpha  0.600
      colour red  0.604  green  0.506  blue  0.455  alpha  0.700]
      [0.641 0.692   colour red  0.604  green  0.506  blue  0.455  alpha  0.700
      colour red  0.773  green  0.612  blue  0.569  alpha  0.100]
      [0.692 1.001   colour red  0.773  green  0.612  blue  0.569  alpha  0.100
      colour red  0.690  green  0.612  blue  0.569  alpha  1.000]
   }      /* end_colour_map */
   turbulence 0.5
}/* end_texture */




/*--------------------Crack & OverTint /Green---------------------*/
#declare marble3 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color DimGray alpha 0.85 color DimGray alpha 0.90] 
      [0.90 1.001 color Black alpha 0.9 color Black alpha 1.0]
   }      /* end_color_map */
   diffuse 0.3
   ambient 0.2
   turbulence 0.7
}/* end_texture */	

/*--------------------Crack & OverTint /Red---------------------*/
#declare marble4 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color Red alpha 0.95 color Red alpha 0.95] 
      [0.90 1.001 color Black alpha 0.7 color Black alpha 1.0]
   }      /* end_color_map */
   turbulence 0.7
   0.03
}/* end_texture */	

#declare pink = color red 1.0 green 0.5 blue 0.5

/*--------------------Crack & OverTint /LIGHT Red---------------------*/
#declare marble7a = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color pink alpha 0.90 color Red alpha 0.90]
      [0.90 1.001 color Black alpha 0.7 color Black alpha 1.0]
   }      /* end_color_map */
   turbulence 0.7
}/* end_texture */	



/*-----------Mostly Blacks W/thin Shades of D.Chroma----------------------*/
#declare marble5 = texture {
   marble
   colour_map {
      [0.000 0.104   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.110  green  0.082  blue  0.071  alpha  0.000]
      [0.104 0.252   colour red  0.110  green  0.082  blue  0.071  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
      [0.252 0.383   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.383 0.643   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
      [0.643 0.783   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.783 0.922   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.922 0.983   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.983 1.001   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
   }      /* end_colour_map */
   turbulence 5.0
}/* end_texture */

/*-----------MOSTLY BLACY W/ALPHA For SECOND LAYERS-----------------------*/
#declare marble6 = texture {
   marble
   colour_map {
      [0.000 0.035   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.110  green  0.082  blue  0.071  alpha  0.800]
      [0.035 0.104   colour red  0.110  green  0.082  blue  0.071  alpha  0.800
      colour red  0.161  green  0.133  blue  0.118  alpha  0.300]
      [0.104 0.357   colour red  0.161  green  0.133  blue  0.118  alpha  0.300
      colour red  0.000  green  0.000  blue  0.000  alpha  0.700]
      [0.357 0.478   colour red  0.000  green  0.000  blue  0.000  alpha  0.700
      colour red  0.161  green  0.133  blue  0.118  alpha  0.500]
      [0.478 0.539   colour red  0.161  green  0.133  blue  0.118  alpha  0.500
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.539 0.783   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.783 0.983   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.983 1.001   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
   }      /* end_colour_map */
   turbulence 0.4
}/* end_texture */


/*--------------THIN WHITE VIENS------------------------------------------*/
#declare marble7 = texture {
   marble
   colour_map {
      [0.000 0.078   colour red  0.769  green  0.722  blue  0.690  alpha  0.180
      colour red  0.745  green  0.690  blue  0.655  alpha  1.000]
      [0.078 0.374   colour red  0.745  green  0.690  blue  0.655  alpha  1.000
      colour red  0.839  green  0.804  blue  0.780  alpha  1.000]
      [0.374 0.461   colour red  0.839  green  0.804  blue  0.780  alpha  1.000
      colour red  0.839  green  0.804  blue  0.780  alpha  0.263]
      [0.461 0.574   colour red  0.839  green  0.804  blue  0.780  alpha  0.263
      colour red  0.729  green  0.671  blue  0.631  alpha  0.929]
      [0.574 0.643   colour red  0.729  green  0.671  blue  0.631  alpha  0.929
      colour red  0.839  green  0.804  blue  0.780  alpha  1.000]
      [0.643 0.783   colour red  0.839  green  0.804  blue  0.780  alpha  1.000
      colour red  0.839  green  0.804  blue  0.780  alpha  0.976]
      [0.783 0.948   colour red  0.839  green  0.804  blue  0.780  alpha  0.976
      colour red  0.839  green  0.804  blue  0.780  alpha  0.278]
      [0.948 1.001   colour red  0.839  green  0.804  blue  0.780  alpha  0.278
      colour red  0.769  green  0.722  blue  0.690  alpha  0.180]
   }      /* end_colour_map */
   turbulence 0.6
}/* end_texture */

/*---------------CREAM MARBLE W/Gray Viens----------------------------*/
#declare marble12 = texture {
//   marble1
//   marble2
   marble4 scale <2 2 2>
   diffuse 0.8
}/* end_texture */


/*----------------DARKER CREAM MARBLE W/Gray Viens-------------------*/
#declare marble13 = texture {
//   marble1
//   marble2
   marble4 scale <2 2 2>
   diffuse 0.6
}/* end_texture */


/*--------------EVEN DARKER CREAM MARBLE W/Gray Viens----------------*/
#declare marble14 = texture {
//   marble1
//   marble2
   marble4 scale <2 2 2>
   diffuse 0.4
}/* end_texture */



/*---------------BLACK MARBLE W/White Viens----------------------------*/
#declare marble17 = texture {
//   marble5
//   marble6
   marble7 scale <2 2 2>
   diffuse 0.5
}/* end_texture */



#declare marble51 = texture {
//   marble1 scale <10 6 4>
//   marble2 scale <7 6 4> rotate <0 0 40>
   marble4 scale <3 6 4> rotate <0 0 -40>
}/* end_texture */



#declare snk1 = texture {
   marble
   colour_map {
      [0.000 0.303   colour red  0.000  green  0.239  blue  0.000  alpha  0.000
      colour red  0.333  green  0.294  blue  0.000  alpha  0.000]
      [0.303 0.588   colour red  0.333  green  0.294  blue  0.000  alpha  0.000
      colour red  0.000  green  0.239  blue  0.341  alpha  0.000]
      [0.588 0.790   colour red  0.000  green  0.239  blue  0.341  alpha  0.000
      colour red  0.000  green  0.020  blue  0.000  alpha  0.000]
      [0.790 1.001   colour red  0.000  green  0.020  blue  0.000  alpha  0.000
      colour red  0.000  green  0.239  blue  0.000  alpha  0.000]
   }      /* end_colour_map */
   ripples 1.0 frequency 100
   turbulence 0.3

}/* end_texture */

#declare snk2 = texture {
   marble
   colour_map {
      [0.000 0.303   colour red  0.000  green  0.239  blue  0.000  alpha  0.7
      colour red  0.333  green  0.294  blue  0.000  alpha  0.7]
      [0.303 0.588   colour red  0.333  green  0.294  blue  0.000  alpha  0.7
      colour red  0.000  green  0.239  blue  0.341  alpha  0.7]
      [0.588 0.790   colour red  0.000  green  0.239  blue  0.341  alpha  0.7
      colour red  0.000  green  0.020  blue  0.000  alpha  0.7]
      [0.790 1.001   colour red  0.000  green  0.020  blue  0.000  alpha  0.7
      colour red  0.000  green  0.239  blue  0.000  alpha  0.7]
   }      /* end_colour_map */
   turbulence 0.2
   0.3
   ripples 0.75 frequency 50
   scale <1 10 1>
}/* end_texture */



#declare snake_skin = texture {
   snk1 scale <2 5 2> ambient 0.6 phong 1.0 diffuse 1.0 
   /*SNK2 SCALE <20 30 5> ROTATE <0 0 90> AMBIENT 0.7 DIFFUSE 0.8  PHONG 1.0 */
}/* end_texture */


/*------------Very Light White & Pinks-------------------------------*/
#declare m1 = texture {
   marble
   turbulence 0.3
   0.05
   colour_map {
      [0.000 0.316   colour red  0.910  green  0.788  blue  0.788  alpha  0.000
      colour red  1.000  green  1.000  blue  1.000  alpha  0.000]
      [0.316 0.453   colour red  1.000  green  1.000  blue  1.000  alpha  0.000
      colour red  0.965  green  1.000  blue  0.918  alpha  0.000]
      [0.453 0.624   colour red  0.965  green  1.000  blue  0.918  alpha  0.000
      colour red  0.784  green  0.788  blue  0.788  alpha  0.000]
      [0.624 0.726   colour red  0.784  green  0.788  blue  0.788  alpha  0.000
      colour red  0.965  green  1.000  blue  0.918  alpha  0.000]
      [0.726 0.863   colour red  0.965  green  1.000  blue  0.918  alpha  0.000
      colour red  0.784  green  0.788  blue  0.788  alpha  0.000]
      [0.863 1.001   colour red  0.784  green  0.788  blue  0.788  alpha  0.000
      colour red  0.910  green  0.788  blue  0.788  alpha  0.000]
   }      /* end_colour_map */
}/* end_texture */

/*---------------Slighty Darker & Grayer Than M1-------------------------*/
#declare m2 = texture {
   marble
   turbulence 0.3
   0.05
   colour_map {
      [0.000 0.316   colour red  0.910  green  0.788  blue  0.788  alpha  0.000
      colour red  0.922  green  0.914  blue  0.871  alpha  0.000]
      [0.316 0.453   colour red  0.922  green  0.914  blue  0.871  alpha  0.000
      colour red  0.894  green  0.867  blue  0.780  alpha  0.000]
      [0.453 0.624   colour red  0.894  green  0.867  blue  0.780  alpha  0.000
      colour red  0.784  green  0.788  blue  0.788  alpha  0.000]
      [0.624 0.726   colour red  0.784  green  0.788  blue  0.788  alpha  0.000
      colour red  0.851  green  0.812  blue  0.741  alpha  0.000]
      [0.726 0.863   colour red  0.851  green  0.812  blue  0.741  alpha  0.000
      colour red  0.647  green  0.655  blue  0.655  alpha  0.000]
      [0.863 1.001   colour red  0.647  green  0.655  blue  0.655  alpha  0.000
      colour red  0.910  green  0.788  blue  0.788  alpha  0.000]
   }      /* end_colour_map */
}/* end_texture */


#declare m3 = texture {
   marble
   turbulence 0.3
   0.05
   colour_map {
      [0.000 0.034   colour red  0.027  green  0.012  blue  0.012  alpha  0.000
      colour red  0.851  green  0.812  blue  0.741  alpha  0.235]
      [0.034 0.342   colour red  0.851  green  0.812  blue  0.741  alpha  0.235
      colour red  0.792  green  0.694  blue  0.690  alpha  0.839]
      [0.342 0.462   colour red  0.792  green  0.694  blue  0.690  alpha  0.839
      colour red  0.631  green  0.506  blue  0.471  alpha  0.608]
      [0.462 0.632   colour red  0.631  green  0.506  blue  0.471  alpha  0.608
      colour red  0.851  green  0.812  blue  0.741  alpha  0.922]
      [0.632 0.983   colour red  0.851  green  0.812  blue  0.741  alpha  0.922
      colour red  0.647  green  0.655  blue  0.655  alpha  0.282]
      [0.983 1.001   colour red  0.647  green  0.655  blue  0.655  alpha  0.282
      colour red  0.027  green  0.012  blue  0.012  alpha  0.000]
   }      /* end_colour_map */
}/* end_texture */


#declare m4 = texture {
   marble
   turbulence 0.4
   0.05
   colour_map {
      [0.000 0.128   colour red  0.820  green  0.580  blue  0.580  alpha  0.000
      colour red  0.851  green  0.812  blue  0.741  alpha  0.235]
      [0.128 0.282   colour red  0.851  green  0.812  blue  0.741  alpha  0.235
      colour red  0.792  green  0.694  blue  0.690  alpha  0.282]
      [0.282 0.393   colour red  0.792  green  0.694  blue  0.690  alpha  0.282
      colour red  0.647  green  0.655  blue  0.655  alpha  0.133]
      [0.393 0.590   colour red  0.647  green  0.655  blue  0.655  alpha  0.133
      colour red  0.851  green  0.812  blue  0.741  alpha  0.333]
      [0.590 0.983   colour red  0.851  green  0.812  blue  0.741  alpha  0.333
      colour red  0.647  green  0.655  blue  0.655  alpha  0.282]
      [0.983 1.001   colour red  0.647  green  0.655  blue  0.655  alpha  0.282
      colour red  0.820  green  0.580  blue  0.580  alpha  0.000]
   }      /* end_colour_map */
}/* end_texture */


/*--------------------Crack & OverTint /Green---------------------*/
#declare m5 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color DimGray alpha 1.0 color DimGray alpha 1.0] 
      [0.90 1.001 color Black alpha 0.6 color Black alpha 1.0]
   }      /* end_color_map */
   turbulence 0.7
}/* end_texture */	



/*****************************************************************
******************************************************************/



/*---------------LIGHT CREAM MOSS / GOOD FOR 1st LAYER TONE----------------*/
#declare m6 = texture {
   marble
   colour_map {
      [0.000 0.154   colour red  0.690  green  0.612  blue  0.569  alpha  0.000
      colour red  0.737  green  0.596  blue  0.522  alpha  0.000]
      [0.154 0.368   colour red  0.737  green  0.596  blue  0.522  alpha  0.000
      colour red  0.776  green  0.702  blue  0.624  alpha  0.000]
      [0.368 0.538   colour red  0.776  green  0.702  blue  0.624  alpha  0.000
      colour red  0.796  green  0.678  blue  0.643  alpha  0.000]
      [0.538 0.846   colour red  0.796  green  0.678  blue  0.643  alpha  0.000
      colour red  0.690  green  0.612  blue  0.569  alpha  0.000]
      [0.846 0.932   colour red  0.690  green  0.612  blue  0.569  alpha  0.000
      colour red  0.773  green  0.612  blue  0.569  alpha  0.000]
      [0.932 1.001   colour red  0.773  green  0.612  blue  0.569  alpha  0.000
      colour red  0.690  green  0.612  blue  0.569  alpha  0.000]
   }      /* end_colour_map */
   turbulence 0.5
   0.05
}/* end_texture */

/*-------------------DEEP THAN MARBLE1 With ALPHA For SECOND LAYERS--------*/
#declare m7 = texture {
   marble
   colour_map {
      [0.000 0.291   colour red  0.690  green  0.612  blue  0.569  alpha  1.00
      colour red  0.737  green  0.596  blue  0.522  alpha  0.500]
      [0.291 0.316   colour red  0.737  green  0.596  blue  0.522  alpha  0.500
      colour red  0.667  green  0.561  blue  0.514  alpha  0.300]
      [0.316 0.436   colour red  0.667  green  0.561  blue  0.514  alpha  0.300
      colour red  0.788  green  0.667  blue  0.631  alpha  0.500]
      [0.436 0.641   colour red  0.788  green  0.667  blue  0.631  alpha  0.500
      colour red  0.604  green  0.506  blue  0.455  alpha  0.700]
      [0.641 0.692   colour red  0.604  green  0.506  blue  0.455  alpha  0.700
      colour red  0.773  green  0.612  blue  0.569  alpha  0.400]
      [0.692 1.001   colour red  0.773  green  0.612  blue  0.569  alpha  0.400
      colour red  0.690  green  0.612  blue  0.569  alpha  1.000]
   }      /* end_colour_map */
   turbulence 0.5
}/* end_texture */


/*-------------------LIGHTER THAN MARBLE2 With ALPHA For SECOND LAYERS--------*/
#declare m8 = texture {
   marble
   colour_map {
      [0.000 0.291   colour red  0.690  green  0.612  blue  0.569  alpha  0.6
      colour red  0.737  green  0.596  blue  0.522  alpha  0.900]
      [0.291 0.316   colour red  0.737  green  0.596  blue  0.522  alpha  0.900
      colour red  0.667  green  0.561  blue  0.514  alpha  0.700]
      [0.316 0.436   colour red  0.667  green  0.561  blue  0.514  alpha  0.700
      colour red  0.788  green  0.667  blue  0.631  alpha  0.600]
      [0.436 0.641   colour red  0.788  green  0.667  blue  0.631  alpha  0.600
      colour red  0.604  green  0.506  blue  0.455  alpha  0.700]
      [0.641 0.692   colour red  0.604  green  0.506  blue  0.455  alpha  0.700
      colour red  0.773  green  0.612  blue  0.569  alpha  0.100]
      [0.692 1.001   colour red  0.773  green  0.612  blue  0.569  alpha  0.100
      colour red  0.690  green  0.612  blue  0.569  alpha  1.000]
   }      /* end_colour_map */
   turbulence 0.01
}/* end_texture */




/*--------------------Crack & OverTinting ---------------------*/
#declare m9 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color DimGray alpha 0.75 color Salmon alpha 0.70] 
      [0.90 1.001 color Black alpha 0.9 color Black alpha 1.0]
   }      /* end_color_map */
   diffuse 0.3
   ambient 0.2
   0.02
   turbulence 0.7
}/* end_texture */	


/*--------------------Crack & OverTinting ---------------------*/
#declare m19 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color SeaGreen alpha 0.85 color Salmon alpha 0.60] 
      [0.90 1.001 color Black alpha 0.9 color Black alpha 1.0]
   }      /* end_color_map */
   diffuse 0.3
   ambient 0.2
   turbulence 0.6
}/* end_texture */	





/*--------------------Crack & OverTint /Red---------------------*/
#declare m10 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color Red alpha 0.95 color Red alpha 0.95] 
      [0.90 1.001 color Black alpha 0.7 color Black alpha 1.0]
   }      /* end_color_map */
   turbulence 0.7
   0.03
}/* end_texture */	

#declare pink = color red 1.0 green 0.5 blue 0.5

/*--------------------Crack & OverTint /LIGHT Red---------------------*/
#declare m11 = texture {
   marble
   color_map {
      [0.0 0.05 color Black alpha 0.6 color Black alpha 1.0]
      [0.05 0.90 color pink alpha 0.90 color Red alpha 0.90] 
      [0.90 1.001 color Black alpha 0.7 color Black alpha 1.0]
   }      /* end_color_map */
   turbulence 0.7
}/* end_texture */	



/*-----------Mostly Blacks W/thin Shades of D.Chroma----------------------*/
#declare m12 = texture {
   marble
   colour_map {
      [0.000 0.104   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.110  green  0.082  blue  0.071  alpha  0.000]
      [0.104 0.252   colour red  0.110  green  0.082  blue  0.071  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
      [0.252 0.383   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.383 0.643   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
      [0.643 0.783   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.783 0.922   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.922 0.983   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.983 1.001   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
   }      /* end_colour_map */
   turbulence 5.0
}/* end_texture */

/*-----------MOSTLY BLACY W/ALPHA For SECOND LAYERS-----------------------*/
#declare m13 = texture {
   marble
   colour_map {
      [0.000 0.035   colour red  0.161  green  0.133  blue  0.118  alpha  0.000
      colour red  0.110  green  0.082  blue  0.071  alpha  0.800]
      [0.035 0.104   colour red  0.110  green  0.082  blue  0.071  alpha  0.800
      colour red  0.161  green  0.133  blue  0.118  alpha  0.300]
      [0.104 0.357   colour red  0.161  green  0.133  blue  0.118  alpha  0.300
      colour red  0.000  green  0.000  blue  0.000  alpha  0.700]
      [0.357 0.478   colour red  0.000  green  0.000  blue  0.000  alpha  0.700
      colour red  0.161  green  0.133  blue  0.118  alpha  0.500]
      [0.478 0.539   colour red  0.161  green  0.133  blue  0.118  alpha  0.500
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.539 0.783   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.000  green  0.000  blue  0.000  alpha  0.000]
      [0.783 0.983   colour red  0.000  green  0.000  blue  0.000  alpha  0.000
      colour red  0.220  green  0.149  blue  0.137  alpha  0.000]
      [0.983 1.001   colour red  0.220  green  0.149  blue  0.137  alpha  0.000
      colour red  0.161  green  0.133  blue  0.118  alpha  0.000]
   }      /* end_colour_map */
   turbulence 0.4
}/* end_texture */


/*--------------THIN WHITE VIENS------------------------------------------*/
#declare m14 = texture {
   marble
   colour_map {
      [0.000 0.078   colour red  0.769  green  0.722  blue  0.690  alpha  0.180
      colour red  0.745  green  0.690  blue  0.655  alpha  1.000]
      [0.078 0.374   colour red  0.745  green  0.690  blue  0.655  alpha  1.000
      colour red  0.839  green  0.804  blue  0.780  alpha  1.000]
      [0.374 0.461   colour red  0.839  green  0.804  blue  0.780  alpha  1.000
      colour red  0.839  green  0.804  blue  0.780  alpha  0.263]
      [0.461 0.574   colour red  0.839  green  0.804  blue  0.780  alpha  0.263
      colour red  0.729  green  0.671  blue  0.631  alpha  0.929]
      [0.574 0.643   colour red  0.729  green  0.671  blue  0.631  alpha  0.929
      colour red  0.839  green  0.804  blue  0.780  alpha  1.000]
      [0.643 0.783   colour red  0.839  green  0.804  blue  0.780  alpha  1.000
      colour red  0.839  green  0.804  blue  0.780  alpha  0.976]
      [0.783 0.948   colour red  0.839  green  0.804  blue  0.780  alpha  0.976
      colour red  0.839  green  0.804  blue  0.780  alpha  0.278]
      [0.948 1.001   colour red  0.839  green  0.804  blue  0.780  alpha  0.278
      colour red  0.769  green  0.722  blue  0.690  alpha  0.180]
   }      /* end_colour_map */
   turbulence 0.6
}/* end_texture */

#declare m21 = texture {
   marble
   turbulence 0.5
   colour_map {
      [0.000 0.479   colour red  0.714  green  0.690  blue  0.706  alpha  0.000
      colour red  0.663  green  0.596  blue  0.624  alpha  0.000]
      [0.479 0.723   colour red  0.663  green  0.596  blue  0.624  alpha  0.000
      colour red  0.655  green  0.690  blue  0.706  alpha  0.000]
      [0.723 0.866   colour red  0.655  green  0.690  blue  0.706  alpha  0.000
      colour red  0.784  green  0.808  blue  0.737  alpha  0.000]
      [0.866 1.001   colour red  0.784  green  0.808  blue  0.737  alpha  0.000
      colour red  0.714  green  0.690  blue  0.706  alpha  0.000]
   }      /* end_colour_map */
}/* end_texture */

#version Marble_Inc_Temp;
