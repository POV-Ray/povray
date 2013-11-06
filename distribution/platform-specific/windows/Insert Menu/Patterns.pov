// Insert menu illustration scene
// Created June-August 2001 by Christoph Hormann
// Modified (added to) October 2001 by Bob Hughes
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012.

// ----- patterns submenu -----

// -w120 -h64 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#include "colors.inc"

/*
#declare Typ=1;   // agate
#declare Typ=2;   // boxed
#declare Typ=3;   // bozo
#declare Typ=4;   // brick
#declare Typ=5;   // bumps
#declare Typ=6;   // cells
#declare Typ=7;   // checker
#declare Typ=8;   // crackle
#declare Typ=9;   // cylindrical
#declare Typ=10;  // density_file
#declare Typ=11;  // dents
#declare Typ=12;  // fractal
#declare Typ=13;  // function
#declare Typ=14;  // gradient
#declare Typ=15;  // granite
#declare Typ=16;  // hexagon
#declare Typ=17;  // image_pattern
#declare Typ=18;  // leopard
#declare Typ=19;  // marble
#declare Typ=20;  // onion
#declare Typ=21;  // planar
#declare Typ=22;  // quilted
#declare Typ=23;  // radial
#declare Typ=24;  // ripples
#declare Typ=25;  // spherical
#declare Typ=26;  // spiral1
#declare Typ=27;  // spiral2
#declare Typ=28;  // spotted
#declare Typ=29;  // waves
#declare Typ=30;  // wood
#declare Typ=31;  // wrinkles
#declare Typ=32;  // facets
#declare Typ=33;  // average
#declare Typ=34;  // object
#declare Typ=35;  // pigment_pattern
#declare Typ=36;  // slope
*/

#if (Typ=17)     // image_pattern
  #include "logo.inc"
#end

//global_settings { assumed_gamma 1.0 }

camera {
  orthographic
  location <0,0,1>
  look_at  <0,0,0>
  right -x*image_width/image_height// -(120/64)*x
  up y
}

light_source {
  <-1000, 2500, 2000>
  color rgb 1.2
}

#declare P1=pigment {checker
         color rgb <0.85, 0.00, 0.25>, color rgb <0.4, 0.20, 0.85> scale 0.2}
#declare P2=pigment {hexagon
         color rgb <0.85, 0.00, 0.25>, color rgb <0.0, 0.30, 0.42>, color rgb <0.25, 0.40, 0.35> scale 0.3 rotate 90*x}

/*
#declare P1=pigment {checker
         color rgb <0.85, 0.55, 0.45>, color rgb <0.00, 0.00, 0.42> scale 0.2}
#declare P2=pigment {hexagon
         color rgb <0.85, 0.55, 0.45>, color rgb <0.00, 0.00, 0.42>, color rgb <0.25, 0.40, 0.35> scale 0.3 rotate 90*x}
*/
#if (Typ=36)
union {
  sphere { 0, 0.47 translate <0.5, 0, 0>}
  sphere { 0, 1.7 }

  cylinder { -z*0.5+0.3*(x+y), z*0.5-0.3*(x+y), 0.2 translate <-0.5, 0, 0>}


#else
plane {
  z, 0
#end
  texture {
 #if (Typ<39)
    pigment {
      #switch (Typ)
        #case (1)
          agate
          #break
        #case (2)
          boxed
          #break
        #case (3)
          bozo
          #break
        #case (4)
          brick
          #break
        #case (5)
          bumps
          #break
        #case (6)
          cells
          #break
        #case (7)
          checker
          #break
        #case (8)
          crackle
          #break
        #case (9)
          cylindrical
          #break
        #case (10)
          density_file df3 "spiral.df3"
          #break
        #case (11)
          dents
          #break
        #case (12)
          mandel 50
            interior 2,.5
            exterior 1,.01
          #break
        #case (13)
          function { pow(min(min(abs(x) + y, (x*2)*(x*2)),y*5),2) }
          #break
        #case (14)
          gradient x
          #break
        #case (15)
          granite
          #break
        #case (16)
          hexagon
          #break
        #case (17)
          image_pattern {
            //png "mtmandj.png"
            function 50, 50 {
              pattern {
                object {
                  Povray_Logo_Prism
                  scale <1,-1,1>*0.5
                  translate 0.5*(x+y)
                }
              }
            }
            interpolate 2
          }
          #break
        #case (18)
          leopard
          #break
        #case (19)
          marble
          #break
        #case (20)
          onion
          #break
        #case (21)
          planar
          #break
        #case (22)
          quilted
          #break
        #case (23)
          radial frequency 6
          #break
        #case (24)
          ripples
          #break
        #case (25)
          spherical
          #break
        #case (26)
          spiral1 5
          #break
        #case (27)
          spiral2 5
          #break
        #case (28)
          spotted
          #break
        #case (29)
          waves
          #break
        #case (30)
          wood
          #break
        #case (31)
          wrinkles
          #break
        #case (32)
          // facets
          #break
        #case (33)
          average
          pigment_map {
                [1, P1]
                [1, P2]
                }
          #break
        #case (34)
          object {
            object {
              union {
                cone {-y,1,y/2,0}
                difference {
                  sphere {y,1}
                  cylinder {-z,z,0.25 translate y*2}
                  cylinder {-z,z,0.25 scale <1,0.5,1> translate <0.5,1.5,0>}
                }
                sphere {0,0.125 scale <2,1,1> translate <1,2,0> }
              }
              scale <0.3,0.6,0.3> rotate 90*z translate 0.25*x
            }
            color rgb <0.85, 0.00, 0.25>
            color rgb <0.00, 0.30, 0.42>
          }
          #break
        #case (35)
          pigment_pattern {
            checker
            pigment { gradient x color_map {[0 color rgb 0.0][1 color rgb 0.5]} scale 0.34 rotate 45*z },
            pigment { marble  color_map {[0 color rgb 0.3][1 color rgb 1.0]} scale 2 turbulence 1 }
            scale 0.5
          }
          #break
        #case (36)
          slope {<0,1,0>,0,1 altitude <0,0,0>,0,1}
          #break

        #case (37) // new in 3.7
          square //max.4 colors/textures
          color White
          color rgb<1.00, 0.00, 0.30>,
          color rgb<0.10, 0.70, 0.00>,
          color Orange  rotate<-90,0,0> scale 0.2
          #break
       
        #case (38) // new in 3.7
          triangular //max.6 colors/textures 
          color Red,
          color rgb<1.0, 0.4, 0.0>,
          color rgb<1.0, 0.9, 0.0>,
          color rgb<0.5, 1.0, 0.0>,
          color rgb<0.5, 0.5, 1.0>,
          color rgb<0.5, 0.0, 1.0> rotate<-90,0,0> scale 0.2
          #break

     #end // switch(Typ)

     #if ((Typ=33) | (Typ=34) | (Typ=37) | (Typ=38))
     // nothing
     #else
      #if ((Typ=4) | (Typ=7) | (Typ=16))
        color rgb <0.85, 0.00, 0.15>,
        color rgb <0.00, 0.60, 0.42>,
        #if (Typ=16)
          color rgb <0.25, 0.40, 0.35>
        #end
      #else
        #if (Typ=32)
          color rgb <0.25, 0.40, 0.35>
        #else
        color_map {
          [0.0 color rgb <0.85, 0.10, 0.10> ]
          [0.1 color rgb <0.85, 0.25, 0.45> ]
          //[0.5 color rgb <0.25, 0.40, 0.35> ]
          [0.5 color rgb <1, 0.85, 0.3> ]
          [0.9 color rgb <0.00, 0.30, 0.42> ]
          [1.0 color rgb <0.00, 0.10, 0.22> ]
        }
        #end
      #end

      scale 0.7

      #if (Typ=17)
        translate 1 scale 0.9  // image_pattern
      #end
      #if (Typ=10)
        translate -<0.4, 0.4, 0.4> scale 1.3 // density_file
      #end
      #if (Typ=4)
        scale 0.19  // brick
      #end
      #if ((Typ=6) | (Typ=7) | (Typ=16) |
           (Typ=18) | (Typ=21) | (Typ=24) | (Typ=29))
        scale 0.3   // cells / checker / hexagon / leopard / planar / ripples / waves
      #end
      #if (Typ=0)
        rotate 90*y // agate
      #end
      #if ((Typ=16) | (Typ=23) | (Typ=24))
        rotate 90*x // hexagon / radial / ripples
      #end
     #end
    } // close pigment

    #if (Typ=32)  // facets
      normal {
        facets 0.5
        coords 1.5
        scale 3
      }
    #end

 #end //  #if (Typ<39)

  // 101 - 299 -------------------------------------------------------------
  #if (Typ>=101 & Typ<=127) // tiling - new in 3.7 
      #local Nr= Typ-100; 

        pigment{
           tiling Nr // 1~24 Pattern_Number
              color_map{
                [ 0.0 color rgb<1,1,1>*1]
                [ 0.5 color rgb<1,0.5,0>*1]
                [ 1.0 color rgb<1,1,1>*0]
                } // end color_map
        #if (Nr = 6|  Nr=7 | Nr=11 | Nr=12 | Nr=13 | Nr=14 | Nr=15 | Nr=18 )
          scale 0.20
         
        #else
         #if (Nr= 22 | Nr=23 | Nr=24) 
          scale 0.10
         #else
          scale 0.25
         #end 
        #end
          rotate<-90,0,0>
        } // end pigment

  #end //------------------------------------------------------------------


   #if (Typ>=150 & Typ<=271) // pavement - new in 3.7 
 

   pigment{ // ------------------------------------------------------------ 
     pavement  // polyform paving the x-z plane

     // ----------------------
     #if (Typ>=150 & Typ<=171)  
        number_of_sides 3 //   3 triangle 

         #if (Typ>=150 & Typ<=152)  
             number_of_tiles Typ-149 pattern 1      //  11,21,31 ;    
         #end
         #if (Typ>=153 & Typ<=155 )  
             number_of_tiles 4      pattern Typ-152 // 41, 42, 43,  
         #end
         #if (Typ>=156 & Typ<=159 )  
             number_of_tiles 5      pattern Typ-155 // 51, 52, 53, 54  
         #end
         #if (Typ>=160 & Typ<=171 )   
             number_of_tiles 6      pattern Typ-159 //   6 with 1,...12  
             
        #end
     #end  // end triangle 
     //-----------------------------
     //----------------------------
     #if (Typ>=172 & Typ<=173)  
        number_of_sides 4 //   4 sqare
        number_of_tiles  Typ-171   pattern 1  // 11,21
     #end // end sqare
     //----------------------------
     #if (Typ>=174 & Typ<=175)  
        number_of_sides 4 //   
        number_of_tiles 3   pattern Typ-173  // 31,32
     #end // end sqare
     //----------------------------
     #if (Typ>=176 & Typ<=180)  
        number_of_sides 4 //   
        number_of_tiles 4   pattern Typ-175  // 41,42,... 45
     #end // end sqare
     //----------------------------
     #if (Typ>=181 & Typ<=192)  
        number_of_sides 4 //    
        number_of_tiles 5   pattern Typ-180  // 5 01,..., 5 12 //1...12
     #end // end sqare
     //----------------------------
     #if (Typ>=195 & Typ<=229)  
        number_of_sides 4 //    
        number_of_tiles 6     pattern Typ-194  // 1,...35
     #end // end sqare
     //-----------------------------
     //-----------------------------
     #if (Typ>=238 & Typ<=239) 
        number_of_sides 6 //   6 hexagon
        number_of_tiles Typ-237 pattern 1  // 1,2,3 
     #end

     #if (Typ>=240 & Typ<=242) 
        number_of_sides 6 //    
        number_of_tiles 3 pattern Typ-239  // 1,2,3 
     #end

     #if (Typ>=243 & Typ<=249) 
        number_of_sides 6 //    
        number_of_tiles 4 pattern Typ-242  // 1, ... 7
     #end

     #if (Typ>=250 & Typ<=271)  
        number_of_sides 6 //   
        number_of_tiles 5 pattern Typ-249  // 1, ... 22
     #end // end hexagon

     //-----------------------------



            exterior 0 //  0, 1, 2;   
            interior 0  // 0, 1, 2  
            form 0//  0, 1 or 2, (3 for square only) copies the look of interior for some additional variations. 
            color_map{
                       [ 0.00 color rgb<1.00, 1.00, 1.00> ] 
                       [ 0.20 color rgb<1.00, 1.00, 1.00> ] 
                       [ 0.20 color rgb<0.00, 0.00, 0.00> ] 
                       [ 0.60 color rgb<0.00, 0.00, 0.00> ] 
                       [ 0.60 color rgb<1.00, 0.60, 0.20> ] 
                       [ 1.00 color rgb<1.00, 0.60, 0.20> ] 
                     } // end color_map


      #if (Typ>=150 & Typ<=152 ) frequency 3 #end 
      // scales:               
      #if (Typ>=150 & Typ<=152 ) scale 0.3 #end 
      #if (Typ>=153 & Typ<=171 ) scale 0.15 #end 
    
             
      #if (Typ>=172 &  Typ<=237 ) scale 0.1 #end 
      #if (Typ>=238 &  Typ<=271 ) scale 0.1 #end 
          rotate<-90,0,0>
         } // end pigment ------------------------------------------------  




 #end //------------------------------------------------------------------

    finish {
      #if (Typ=32)
        specular 0.4  // facets
      #else
        diffuse 0.0
        ambient 1.0
      #end
    }


  } // close texture
} // close plane
