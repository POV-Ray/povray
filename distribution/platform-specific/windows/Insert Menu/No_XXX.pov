// Insert menu illustration scene
// Created June-August 2001 by Christoph Hormann
// Adapted and extended variation for version 3.7
// by Friedrich A. Lohmueller, June-2012
// ----- no_image / no_reflection / no_shadow / combinations -----

// -w120 -h48 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#include "colors.inc"
#include "textures.inc"
//-----------------------------------------------------------------

//#declare Typ=0; // nothing
//#declare Typ=1;     // no_shadow
//#declare Typ=2;     // no_reflection
//#declare Typ=3;     // no_image
//#declare Typ=4;     // no_reflection no_shadow
//#declare Typ=5;     // no_reflection no_shadow
//#declare Typ=6;     // no_image no_shadow

//#declare Typ=7;     // open demo

//-----------------------------------------------------------------
global_settings{ max_trace_level 5 }

light_source { <-3.5, 1,-0.35>*10000 color rgb<1,1,1> }

camera {
  location    <-8, 6.5, -20>
  right x*image_width/image_height 
  look_at     <-2, 1.25, 0>
  angle       28
}//-----------------------------------

sky_sphere{
  pigment{
    gradient y
    color_map{ [0.0 rgb <0.6,0.7,1.0>]
               [1.0 rgb <0.2,0.2,0.8>]
             }
         }
}//-----------------------------------


#declare Tex1=
  texture {
    pigment { color MediumAquamarine }
    finish { specular 0.4 roughness 0.02 }
  }

#declare Tex2=
  texture {
    pigment{ color rgb<0.6,1,0> }
    finish { specular 0.4 roughness 0.02 }
  }

#declare Tex3=
  texture {
    pigment{ color rgb<1,0.75, 0> }
    finish { specular 0.4 roughness 0.02 }
  }
#declare Tex4=
  texture {
    pigment{ color rgb<1,0.35, 0> }
    finish { specular 0.4 roughness 0.02 }
  }

// mirror floor --------------------------------------------------
plane{ <0,1,0>, 0
       texture{ pigment{ color rgb <1,1,1> }
                finish { diffuse 0.7 specular 0.4 roughness 0.01
                         reflection{ 0.5 , 1.0 fresnel on metallic 0.8 }
                         conserve_energy } // end finish
              } // end texture
} //-------------------------------------------------




#if(Typ<7)/////////////////////////////////////////////////////

// mirror
union{ 
 box{ <-6+0.3,0.3,0>,<6-0.3,4-0.3,0.05>
     texture{ Polished_Chrome 
              pigment{color rgb<1,0.6,0>*0.5} 
              finish{ phong 1 } 
            }
    } 
 box{ <-6,0,0.01>,<6,4,0.1>
     texture{ //Polished_Chrome 
              pigment{color rgb<1,0.6,0>*0.5} 
              finish{ phong 1 } 
            }
    } 


    translate<0,0,5> 
} //-------------------------------------------------
//---------------------------------------------------


// =============================================
cylinder { <0,0,0>,<0,3,0>, 1.0 texture { Tex1 }
    translate <1.5, -0.6, 0>

 }



sphere { <-3, 1.25, 0>, 1.25 texture { Tex2 } 

 #if(Typ>0)
  #if (Typ=1)
    no_shadow
  #end
  #if (Typ=2)
    no_reflection
  #end
  #if (Typ=3)
    no_image
  #end

  // combinations: 
  #if (Typ=4)
    no_reflection no_shadow
  #end
  #if (Typ=5)
    no_image no_reflection 
  #end
  #if (Typ=6)
    no_image no_shadow
  #end
 #end // of '#ifdef(Typ)'
  }// end cylinder
 
#end // of #if(Typ<7)//////////////////////////////////////////////

// =============================================
#if(Typ=7) // open //////////////////////////////////////////////
// =============================================
/*
// white floor --------------------------------------------------
plane{ <0,1,0>, 0
       texture{ pigment{ color rgb <1,1,1> }
                finish { phong 1} 
              } // end texture
} //-------------------------------------------------
*/


camera {
  location    <-8, 12.5, -20>
  right x*image_width/image_height 
  look_at     <-0.8, 1.25, -1>
  angle       30
}//-----------------------------------

cylinder{ <0,0,0>,<0,1.25,0>,2 open
          texture{ Tex2 } 
          translate<-4,1,1>
        }  
cone    { <0,0,0>,2,<0,1.25,0>,1.5 open
          texture{ Tex3 } 
          translate<0,1,0>
        }        
prism   { 0, 1.25
          5
          <0,0>,
          <1,0>,
          <1,0.5>,
          <0.5,1>,
          <0,0>
          open
          texture{ Tex4 } 
          
          scale <3,1,3>
          rotate<0,130,0>
          translate<3,1,0>
        }        
       
       
// =============================================
#end // 
// =============================================
         