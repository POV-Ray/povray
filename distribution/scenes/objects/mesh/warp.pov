/*
 *  -Iwarp.pov -H120 -W160 +A0.01 +FJ +KI0 +KF1 +KFI0 +KFF200 +KC
 *
 * Imagemagick:
 * convert -delay 4 -loop 0 -layers optimize warp*.jpg  warp.gif
 *
 */
#version 3.7;
global_settings{ assumed_gamma 1.0 }

#include "colors.inc"

#declare haut=1.5;
camera { location <1.5,haut/2,-100> up y direction z
right 4/3*x look_at <1.5,haut/2,0> angle 2 }
#declare t0= texture { pigment { Gold } }
#declare t1= texture { pigment { Blue } }
#declare t2= texture { pigment { White } }
#declare t3= texture { pigment { Red } }
#declare t4= texture { pigment { Aquamarine } }
#declare epai=30;
#default { texture { pigment { Green } } }
background { Black }

#declare base=0.3;
#declare ep=0.1/2;
#declare epa=0.001;
light_source { <-30,100,-100>,1 }
light_source { <0,00,-100>,1 }

#declare obj1= 
union {
        box { <0,0,0>,<1,haut,epai> texture { t1 } }
        box { <1,0,0>,<2,haut,epai> texture { t2 } }
        box { <2,0,0>,<3,haut,epai> texture { t3 } }
        box { <1.5-ep,base,-epa>,<1.5+ep,haut-base,0> texture { t0 } }
        box { <1.5-(haut-base)/4,haut/2-ep,-epa>,
              <1.5+(haut-base)/4,haut/2+ep,0> 
              texture { t0 } }
        box { <1.5-(haut-base)/7,haut/2-ep+(haut-2*base)/4,-epa>,
              <1.5+(haut-base)/7,haut/2+ep+(haut-2*base)/4,0> 
              texture { t0 } }
        texture { t1 }
      }

#declare vector =  <0.1,0.2,0.3>;
#declare TM = texture { pigment{ gradient x pigment_map
{
[0 color rgb 0  ]
[0.5 color rgb 1  ]
[1 color rgb 1  ]
}
scale 3.2
}
} ;

#declare Angle=radians(clock*360);

warp { original 
keep { original 
tesselate { original 
    obj1 
                                              offset 0.1 
                                              accuracy <100,50,20> 
                                            }
                         with box { <-1,-1,-1>,<4,haut+1,0.04> } 
                         inner 
                       }
       warp { turbulence vector 
              octaves 3 
            } 
       modulation { TM }
       move < 0, cos(Angle), sin(Angle), 0, -sin(Angle), cos(Angle), 1,0,0, 0,0,0>    }

