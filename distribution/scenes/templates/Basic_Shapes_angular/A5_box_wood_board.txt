//-------------------------------------------------------------------------- 
#declare Pigment_Pine_Wood3 = 
     pigment{ wood turbulence 0.02 octaves 4 lambda 3
              scale 0.175 rotate <1, 1, 0> 
              color_map {
              [0.0 color rgb <1.30, 0.99, 0.64>]
              [0.5 color rgb <0.75, 0.39, 0.17>]
              [0.7 color rgb <0.75, 0.39, 0.17>]
              [1.0 color rgb <1.30, 0.99, 0.64>]
              }// end of color_map
            } // end of pigment
//--------------------------------------------------------------------------
#declare Pine_Wood_Bord_Texture =
      texture{ pigment{ Pigment_Pine_Wood3  }
               normal { pigment_pattern{  Pigment_Pine_Wood3 }
                        0.25 // pattern intensity ! 
                        translate<0,0,0>
                      } // end normal
               finish { phong 0.3  phong_size 5 }
               scale 0.2
               translate <0.16,0.025,0.2>
             } // end of texture ------------- 


//-------------------------------------------------------------------------- 
box { <0.00, 0.00, 0.00>,< 0.30, 0.05, 1.00>   
      texture{ Pine_Wood_Bord_Texture }
      scale<1,1,1>  rotate<0,0,0>  translate<0,0,0>  
    }  // end of box ......------------------------------------------------- 
//--------------------------------------------------------------------------
