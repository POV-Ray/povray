// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  allpatiso1_.pov
// Vers: 3.5
// Desc: Render via allpatiso1.ini,
//       generates html files and images for all the
//       pattern-functions in functions.inc
// Date: 2001/07/30
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "functions.inc"
#include "html_gen.inc"


#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all pattern-function names extracted from functions.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
//function
#declare str_functionArr=array[26] {
 "f_agate", "f_boxed", "f_bozo", "f_brick", "f_bumps",
 "f_checker", "f_crackle", "f_cylindrical", "f_dents", "f_gradientX",
 "f_gradientY", "f_gradientZ", "f_granite", "f_hexagon", "f_leopard",
 "f_mandel", "f_marble", "f_onion",
 "f_ripples", "f_spherical", "f_spiral1", "f_spiral2", "f_spotted",
 "f_waves", "f_wood", "f_wrinkles",
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="functions.inc"   // the name of the include file the data came from.
      #declare OutName="allpat1iso"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="patterns as isosurface"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_functionArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment="<p>Most of the patterns, as isosurfaces.</p>"
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)

   #switch(frame_number-1)
      #case(0)
         isosurface {
            function{(f_agate(x/2,y/2,z/2)*0.2)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 2.705
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }
         isosurface {
            function{abs(1-(f_agate(x/2,y/2,z/2)*0.2))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 2.738
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }
      #break
      #case(1)
         isosurface {
            function{abs(1-(f_boxed(x,y,z)))}
            threshold 0.95
            contained_by {box {<-2,-2,-2>,<2,2,2>}}
            max_gradient 0.866
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }
      #break
      #case(3)
         isosurface {
            function{(f_brick(x*3,y*3,z*3)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 200
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
          isosurface {
            function{abs(1-(f_brick(x*3,y*3,z*3)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 200
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(5)
         isosurface {
            function{(f_checker(x*1.33,y*1.33,z*1.33)*0.1)}
            threshold 0.05
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 200
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(6)
         isosurface {
            function{(f_crackle(x*1.33,y*1.33,z*1.33)*0.5)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 1.324
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_crackle(x*1.33,y*1.33,z*1.33)*0.5))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 1.324
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(7)
         isosurface {
            function{abs(1-(f_cylindrical(x/2,y/2,z/2)))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 0.493
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(9)
         isosurface {
            function{(f_gradientX(x*2,y*2,z*2)*0.1)}
            threshold 0.05
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 200
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(10)
         isosurface {
            function{(f_gradientY(x*2.1,y*2.1,z*2.1)*0.1)}
            threshold 0.05
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 200
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(11)
         isosurface {
            function{(f_gradientZ(x*2.1,y*2.1,z*2.1)*0.1)}
            threshold 0.05
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 200
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(12)
         isosurface {
            function{(f_granite(x/9,y/9,z/9)*0.4)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 1.274
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_granite(x/9,y/9,z/9)*0.4))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 1.007
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(13)
         isosurface {
            function{(f_hexagon(x*2,y*2,z*2).gray*0.4)}
            threshold 0.05
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 777.756
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(14)
         isosurface {
            function{(f_leopard(x*3,y*3,z*3)*0.3)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.492
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_leopard(x*3,y*3,z*3)*0.3))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 0.514
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(15)
         isosurface {
            function{abs(1-(f_mandel(x/1.5,y/1.5,z/1.5)*0.2))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 193.380
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(16)
         isosurface {
            function{(f_marble(x*2,y*2,z*2)*0.1)}
            threshold 0.05
            contained_by {box {<-2,-2.5,-2>,<2,2.5,2>}}
            max_gradient 0.312
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }      
      #break
      #case(17)
         isosurface {
            function{(f_onion(x*3,y*3,z*3)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 199.915
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_onion(x*3,y*3,z*3)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 199.861
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(18)
         isosurface {
            function{(f_ripples(x*1.5,y*1.5,z*1.5)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.278
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_ripples(x*1.5,y*1.5,z*1.5)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 0.231
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(19)
         isosurface {
            function{abs(1-(f_spherical(x/2,y/2,z/2)))}
            threshold 0.95
            contained_by {box {<-2,-2,-2>,<2,2,2>}}
            max_gradient 0.5
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb 1}
            finish{specular 0.4}
         }
      #break
      #case(20)
         isosurface {
            function{(f_spiral1(x,y,z)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.295
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_spiral1(x,y,z)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 34.728
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(21)
         isosurface {
            function{(f_spiral2(x,y,z)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.605
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_spiral2(x,y,z)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 18.035
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(23)
         isosurface {
            function{(f_waves(x*2,y*2,z*2)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.170
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_waves(x*2,y*2,z*2)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 0.170
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(24)
         isosurface {
            function{(f_wood(x*2,y*2,z*2)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.361
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_wood(x*2,y*2,z*2)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 0.361
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break
      #case(25)
         isosurface {
            function{(f_wrinkles(x/2,y/2,z/2)*0.1)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.361
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_wrinkles(x/2,y/2,z/2)*0.1))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 0.302
            evaluate 1,30,0.99
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
      #break

      #else   //case2 bozo, case4 bumps, case8 dents, case 22 spotted
         isosurface {
            function{(f_bozo(x*2,y*2,z*2)*0.12)}
            threshold 0.05
            contained_by {box {<-2,0.5,-2>,<2,2.5,2>}}
            max_gradient 0.381
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <0.9,0.9,1>}
            finish{specular 0.4}
         }      
         isosurface {
            function{abs(1-(f_bozo(x*2,y*2,z*2)*0.12))}
            threshold 0.95
            contained_by {box {<-2,-2.5,-2>,<2,0.5,2>}}
            max_gradient 0.381
            rotate <0,-30,0>
            rotate<-30,0,0>  
            pigment {rgb <1,1,0.9>}
            finish{specular 0.4}
         }      
   #end
   camera {
     location  <0,0,-8>
     right x*image_width/image_height
     look_at   <0,0,0>
   }
   
   light_source {
     <500,100,-500>
     rgb 1  // light's color
   }
   
   light_source {
     <10,50,-500>
     rgb <0.4,0.3,0.3>
     shadowless
   }
   
   light_source {
     <-500,50,-500>
     rgb <0.2,0.2,0.4>
     shadowless
   }
#end

