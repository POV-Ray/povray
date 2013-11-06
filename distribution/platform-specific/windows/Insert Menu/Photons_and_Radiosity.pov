// Insert menu illustration scene "Radiosity_and_Photons.pov"
// Author: Friedrich A. Lohmueller, June-2012
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare In_Path  = "I0 - Radiosity and Photons/"   

      // #declare Typ =85; // for tests

#switch (Typ)  //----------------------------------------------------------

#case(30)  #declare Txt_Path="30 - Radiosity scene.txt" #break
#case(35)  #declare Txt_Path="35 - Radiosity scene.txt" #break
#case(40)  #declare Txt_Path="40 - Radiosity scene2.txt" #break 

//
#case(80)  #declare Txt_Path="80 - Photons scene.txt" #break
#case(85)  #declare Txt_Path="85 - Photons scene1.txt" #break
#case(90)  #declare Txt_Path="90 - Object photons on code" #break

#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------



//------------------------------------------------------------------------- 
//----------
#if (Typ=30) // In_Path,"30 - Radiosity scene.txt"
 #include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=35) // In_Path,"35 - Radiosity scene.txt"
 #include concat(In_Path,Txt_Path) 
#end  
//----------
#if (Typ=40) // In_Path,"30 - Radiosity scene.txt"
 #include concat(In_Path,Txt_Path) 
#end

//----------
#if (Typ=80) // In_Path,"80 - Photons scene.txt"
 #include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=85) // In_Path,"85 - Photons scene1.txt"
 #include concat(In_Path,Txt_Path) 
#end
//----------
#if (Typ=90) // In_Path,"90 - Object photons on - code.txt"
global_settings{ 
        max_trace_level 15
        photons {
                spacing 0.03
        }
} //-----------------------------------------
// photon object
// Derived for a sample of Bob Hughes,2001
// by Friedrich A. Lohmueller, May 2010
//
#declare Crystal=
intersection{
 #for (Count, 0,8,1 )
   box{ <-1,-1,-1>,<1,1,1> rotate <45,0,45> rotate Count*40*y }
 #end
} //------------

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.8,1.0>*0.7]
      [0.3 rgb <0.3,0.6,0.9>*0.7]
      [1.0 rgb <0.1,0.4,0.8>*0.7]
    }
  }
}

camera {
  location  <0, 8, -14>
  right    x*image_width/image_height
  rotate 6*y
  look_at   <0.25,0,0>  
  angle 23  
}

light_source { <-200, 150, -200>, color rgb<1,1,1> }
 
plane{  <0,1,0>,-1  
        pigment {rgb 0.5}
}//----
 
object{ Crystal
        pigment{ rgbft <1,1,1,0.9,0.1>}
        finish { specular 0.3 roughness 0.009 reflection {0.1} }
        interior {ior 1.33 dispersion 1.2 dispersion_samples 12}
        translate <1.55,0.5,0>
        photons {target 0.9 refraction on reflection on}
}//----

object{ Crystal
        pigment {rgb 0.1}
        finish {diffuse 0.2 specular 0.6 roughness 0.01 reflection {0.9}}
        translate <-1.55,0.5,0>
        photons {target 0.9 refraction off reflection on}
}//----
#end


//---------- End Radiosity and Photons