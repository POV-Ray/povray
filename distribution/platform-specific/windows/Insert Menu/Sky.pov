// Insert menu illustration scene "Sky.pov"
// Author: Friedrich A. Lohmueller, June-2012
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare In_Path  = "H0 - Sky, fog, rainbow/"   

    // #declare Typ =35; // for tests

#switch (Typ)  //----------------------------------------------------------

#case(10)  #declare Txt_Path="10 - background.txt" #break 
#case(20)  #declare Txt_Path="20 - Blue Sky by sky_sphere.txt" #break
#case(30)  #declare Txt_Path="30 - Blue sky small clouds fog.txt" #break
#case(37)  #declare Txt_Path="37 - Rainy sky with S_Cloud4.txt" #break
#case(35)  #declare Txt_Path="35 - rainbow arc.txt" #break
#case(40)  #declare Txt_Path="40 - starfield.txt" #break
//-------- fogs // 50 - Fog definition.txt
#case(60)  #declare Txt_Path="60 - Constant fog.txt" #break
#case(70)  #declare Txt_Path="70 - Ground fog.txt" #break
#case(80)  #declare Txt_Path="80 - Atmospheric media.txt" #break

#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------



//------------------------------------------------------------------------- 
//----------
#if (Typ=10) // In_Path,"10 - background.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.0 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
#include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=20) // In_Path,"20 - Blue Sky by sky_sphere.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.0 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
#include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=30) // In_Path,"30 - Blue sky small clouds fog.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.0 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
	         normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
#include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=37) // In_Path,"32 - Rainy sky with S_Cloud4.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.5 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
	         normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
#include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=35) // In_Path,"35 - rainbow arc.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.5 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
plane{<0,1,0>,1 hollow  
       texture{ pigment{ bozo turbulence 0.92
                         color_map { [0.00 rgb <0.20, 0.20, 1.0>*0.9]
                                     [0.50 rgb <0.20, 0.20, 1.0>*0.9]
                                     [0.70 rgb <1,1,1>]
                                     [0.85 rgb <0.25,0.25,0.25>]
                                     [1.0 rgb <0.5,0.5,0.5>]}
                        scale<1,1,1.5>*2.5  translate< 0,0,0>
                       }
                finish {ambient 1 diffuse 0} }      
       scale 10000
    } //-----------
fog { fog_type   2
      distance   50
      color      White  
      fog_offset 0.1
      fog_alt    1.5
      turbulence 1.8
    }
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
	         normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
#include concat(In_Path,Txt_Path) //"30 - Blue sky small clouds fog.txt"
#end
//----------
#if (Typ=40) // In_Path,"40 - starfield.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.0 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
#include concat(In_Path,Txt_Path) 
#end
//----------




// fogs:
//----------
#if (Typ=60) // In_Path,"60 - Constant fog.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.5 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
	         normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
#for(Count,0,10 )
 cylinder{<0,0,0>,<0,3,0>,0.1 pigment{color rgb<1,1,1>} translate<1,0,Count*2>} 
#end // of for loop
#include concat(In_Path,Txt_Path) // "20 - Blue Sky by sky_sphere.txt"
#end
//----------
#if (Typ=70) // In_Path,"70 - Ground fog.txt"
camera { angle 75 location<0.0, 1.0,-3.0> look_at   <0.0 , 1.5 , 0.0> right x*image_width/image_height }
light_source{<-1500,2000,-2500> color White}
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
	         normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
#for(Count,0,10 )
 cylinder{<0,0,0>,<0,3,0>,0.1 pigment{color rgb<1,1,1>} translate<1,0,Count*2>} 
#end // of for loop

#include concat(In_Path,Txt_Path) // "20 - Blue Sky by sky_sphere.txt" 
#end
//----------
#if (Typ=80) // In_Path,"80 - Atmospheric media.txt"
camera { angle 75 location<0.0, 2.5,-3.0> look_at   <-0.2 , 1.5 , 0.0> right x*image_width/image_height }
#include concat(In_Path,"20 - Blue Sky by sky_sphere.txt")
light_source {<-125,50,100>,<5,5,2.5>*0.6  spotlight radius 2 falloff 3 point_at 0 }

/* atmospheric media example, light beams through window/door */
union { 
// window/door
difference {
box {-1,1 scale <1,1,.1>}
box {-1,1 scale <.4,.4,.11> translate <-.5,-.5,0>}
box {-1,1 scale <.4,.4,.11> translate <.5,-.5,0>}
box {-1,1 scale <.4,.4,.11> translate <.5,.5,0>}
box {-1,1 scale <.4,.4,.11> translate <-.5,.5,0>}
 pigment {rgb <1,1,1>} finish {ambient .2 diffuse .5 specular .3 roughness .1}
 scale 2 translate <2,1,-2>
}
// room wall
box {-1,1 scale <100,100,0.01> translate -2.01*z
        clipped_by {box {-1,1 scale <2,2,0.1> translate <2,1,-2> inverse}}
         pigment {rgb 1}}
// room air
box {
  -1,1
 material {
  texture {
    pigment {
      rgbf 1
    }
  }
  interior {
    media {method 3 intervals 1 samples 50,50 emission 0 absorption 0.01
         scattering {1,0.1 extinction 0.3}
    }
  }
 }
 scale <3,2,1>*3 translate z
 hollow
}
rotate 180*y
}// end of room
#end

//---------- End Sky, fog, rainbow