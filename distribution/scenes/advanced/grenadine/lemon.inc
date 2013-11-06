// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Ray Tracer Include File
// File: Lemon.inc
// Vers: 3.5
// Desc: Object for Grenadine.pov
// Date: 1999/06/04
// Auth: ingo
//

// Added to comply with common version preservation scheme
// 04Aug2008 (jh)
#ifndef(Lemon_Inc_Temp)
#declare Lemon_Inc_Temp=version;
#version 3.7;

#ifdef(View_POV_Include_Stack)
    #debug "including lemon.inc\n"
#end

#declare SS=seed(7);
#declare R_uit= 3;
#declare R_in=2.9;

#declare Ring = difference {
   cylinder {< 0  , 0, 0>, <1  , 0, 0>, R_uit}
   cylinder {<-0.1, 0, 0>, <1.1, 0, 0>, R_in}
}

#declare R2_uit= 0.8;
#declare R2_in=0.7;

#declare Ring2 = difference {
   cylinder {< 0  , 0, 0>, < 1  , 0, 0>, R2_uit}
   cylinder {<-0.1, 0, 0>, < 1.1, 0, 0>, R2_in}
}

#declare LemonOut= intersection {
   merge {
      difference {
         merge {
            object {Ring translate < 0.01, 0, (R_uit+R_in)/2>}
            object {Ring translate <-1.01, 0,-(R_uit+R_in)/2>}
         }
         box {<-1.1, 0.1,-1>, <1.1, 2, 1>}
      }
      difference {
         box {<-1, 0,-(R_uit-R_in)/2>, < 1, 1.1, (R_uit-R_in)/2>}
         box {
            <-2.5, 0,-1>, <2.5, 2, 1>
            translate <0, 0.5, 0>
            rotate <0, 0,-20>
         }
      }
      difference{
         object {
            Ring2
            translate <-0.5, 0, 0>
            scale <2.2, 1, 1>
            translate < 0, 0, (R2_uit+R2_in)/2>
         }
         box {<-2.1, 0,-1>,<2.1,-3, 1>}
         translate <0, 0.499999, 0>
         rotate <0, 0,-20>
      }
   }
   merge {
      cylinder {<0, 0,-0.5>, <0, 0, 0.5>, 0.8}
      torus {0.8, 0.2 scale <1, 1.1, 1> rotate <90, 0, 0>}
   }
}

#declare BS1= array[24] {
   < 24.8, 49.8>, < 13.0, 31.4>, <  4.0,  8.8>, <  0.1,  9.4>
   <  0.1,  9.4>, <- 7.4, 10.7>, <-12.5, 30.4>, <-21.1, 49.8>
   <-21.1, 49.8>, <-33.3, 76.9>, <-39.8, 87.0>, <-29.2, 91.4>
   <-29.2, 91.4>, <-20.0, 95.3>, <-10.0, 95.9>, <  0.0, 95.9>
   <  0.0, 95.9>, < 10.0, 95.9>, < 21.3, 95.8>, < 30.0, 90.7>
   < 30.0, 90.7>, < 41.3, 84.0>, < 45.1, 86.5>, < 24.8, 49.8>
}

#declare BS2= array[24] {
   < 24.8, 55.8>, < 13.0, 31.4>, <  4.0,  8.8>, <  0.1, 15.0>
   <  0.1, 15.0>, <- 7.4, 10.7>, <-12.5, 30.4>, <-21.1, 49.8>
   <-21.1, 49.8>, <-33.3, 76.9>, <-39.8, 87.0>, <-29.2, 91.4>
   <-29.2, 91.4>, <-20.0, 95.3>, <-10.0, 95.9>, <  0.0, 95.9>
   <  0.0, 95.9>, < 10.0, 95.9>, < 21.3, 95.8>, < 30.0, 90.7>
   < 30.0, 90.7>, < 41.3, 84.0>, < 45.1, 86.5>, < 24.8, 55.8>
}


#declare BS3= array[24] {
   < 23.0, 49.8>, < 13.0, 31.4>, <  4.0,  8.8>, <  0.1,  6.0>
   <  0.1,  6.0>, <- 7.4, 10.7>, <-12.5, 30.4>, <-21.1, 49.8>
   <-21.1, 49.8>, <-33.3, 76.9>, <-39.8, 87.0>, <-29.2, 91.4>
   <-29.2, 91.4>, <-20.0, 95.3>, <-10.0, 95.9>, <  0.0, 95.9>
   <  0.0, 95.9>, < 10.0, 95.9>, < 21.3, 95.8>, < 30.0, 90.7>
   < 30.0, 90.7>, < 41.3, 84.0>, < 45.1, 85.0>, < 23.0, 49.8>
}


#declare BS4= array[24] {
   < 24.8, 49.8>, < 13.0, 31.4>, <  4.0, 11.8>, <  0.1,  9.0>
   <  0.1,  9.0>, <- 7.4, 13.7>, <-12.5, 30.4>, <-21.1, 49.8>
   <-21.1, 49.8>, <-33.3, 76.9>, <-39.8, 87.0>, <-21.2, 91.4>
   <-21.2, 91.4>, <-20.0, 95.3>, <-10.0, 95.9>, <  0.0, 95.9>
   <  0.0, 95.9>, < 10.0, 95.9>, < 21.3, 95.8>, < 30.0, 90.7>
   < 30.0, 90.7>, < 41.3, 84.0>, < 45.1, 86.5>, < 24.8, 49.8>
}

#declare J=0;
#declare Part1= prism {
   bezier_spline
   -0.5, 0.5, 24,
   #while (J<24)
      #declare P= BS1[J];
      P
      #declare J=J+1;
   #end
   scale < 0.0095, 1, 0.0095>
}

#declare J=0;
#declare Part2= prism {
   bezier_spline
   -0.5, 0.5, 24,
   #while (J<24)
      #declare P= BS2[J];
      P
      #declare J=J+1;
   #end
   scale < 0.0095, 1, 0.0095>
}

#declare J=0;
#declare Part3= prism {
   bezier_spline
   -0.5, 0.5, 24,
   #while (J<24)
      #declare P= BS3[J];
      P
      #declare J=J+1;
   #end
   scale < 0.0095, 1, 0.0095>
}

#declare J=0;
#declare Part4= prism {
   bezier_spline
   -0.5, 0.5, 24,
   #while (J<24)
      #declare P= BS4[J];
      P
      #declare J=J+1;
   #end
   scale < 0.0095, 1, 0.0095>
}

#declare LemonTex= texture {
   pigment {
      granite
      scale <0.2,5,1>
      colour_map {
         [0.4 rgbf <1,0.65,0,0.4>]
         [0.6 rgbf <1,0.8,0,0.4>]
         [0.7 rgbf <1,0.9,0,0.6>]
         [0.9 rgb <1,0.7,0>*1.5 ]
      }
   }
   normal {granite -0.1 turbulence 0.3 scale <0.2,5,1>}
   finish {
      specular .9
      roughness 0.01
   }
}

#declare Parts= union {
   object {Part1 }
   object {Part2 rotate <0,   360/7 ,0>}
   object {Part3 rotate <0,2*(360/7),0>}
   object {Part4 rotate <0,3*(360/7),0>}
   object {Part1 rotate <0,4*(360/7),0>}
   object {Part2 rotate <0,5*(360/7),0>}
   object {Part3 rotate <0,6*(360/7),0>}
   rotate <90,0,0>
}

#declare LemonSlice = union {
   intersection {
      object {LemonOut}
      object {Part1 rotate <90,0,0>}
      texture {LemonTex rotate <90,0,0> translate rand(SS)*5}
   }
   intersection {
      object {LemonOut}
      object {Part2 rotate <0,360/7,0> rotate <90,0,0>}
      texture {LemonTex rotate <0,360/7,0> rotate <90,0,0> translate rand(SS)*5}
   }
   intersection {
      object {LemonOut}
      object {Part3 rotate <0,2*(360/7),0> rotate <90,0,0>}
      texture {LemonTex rotate <0,2*(360/7),0> rotate <90,0,0> translate rand(SS)*5}
   }
   intersection {
      object {LemonOut}
      object {Part4 rotate <0,3*(360/7),0> rotate <90,0,0>}
      texture {LemonTex rotate <0,3*(360/7),0> rotate <90,0,0> translate rand(SS)*5}
   }
   intersection {
      object {LemonOut}
      object {Part1 rotate <0,4*(360/7),0> rotate <90,0,0>}
      texture {LemonTex rotate <0,4*(360/7),0> rotate <90,0,0> translate rand(SS)*5}
   }
   intersection {
      object {LemonOut}
      object {Part2 rotate <0,5*(360/7),0> rotate <90,0,0>}
      texture {LemonTex rotate <0,5*(360/7),0> rotate <90,0,0> translate rand(SS)*5}
   }
   intersection {
      object {LemonOut}
      object {Part3 rotate <0,6*(360/7),0> rotate <90,0,0>}
      texture {LemonTex rotate <0,6*(360/7),0> rotate <90,0,0> translate rand(SS)*5}
   }

   difference {          //outside
      object {LemonOut}
      object {Parts}
      texture {
         cylindrical
         rotate <90,0,0>
         texture_map {
            [0.05, pigment {rgb <1,0.8,0>}
                   normal {granite .1 scale 0.1}
                   finish {phong 0.8 phong_size 20}
            ]
            [0.06, pigment {rgb <1,0.9,0.7>}
                   normal {granite .07 scale 0.5}
            ]
         }
      }
   }
}

/*
global_settings{assumed_gamma 1.0}

camera{
  location  <-3.0, 0.01, -3.0>
  look_at   <0.0, 0.0,  0.0>
  angle 45
}

light_source{
  <500, 550, -100>
  rgb 1
}

object { LemonSlice}
*/

// Added to comply with common version preservation scheme
// 04Aug2008 (jh)
#version Lemon_Inc_Temp;
#end