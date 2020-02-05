// Pentspir.pov  - Spiraling blob animation
// Jeff Bowermaster - Splat! Graphics 12/29/95

// NOTE:

// Requires a 4x1 aspect (640x160)

// Animation:
// povray +ipentmap.pov +opm0.tga +B50 +SF0 +EF300 +KI0.0 +KF1.0 +KFI0 +KFF300 +D0 +KC

#declare Wild_Map =
color_map {
   [0.000, 0.021 color rgb <0.010, 0.010, 0.010>
                 color rgb <0.040, 0.010, 0.020>]
   [0.021, 0.067 color rgb <0.040, 0.010, 0.020>
                 color rgb <0.105, 0.080, 0.060>]
   [0.067, 0.117 color rgb <0.105, 0.080, 0.060>
                 color rgb <0.275, 0.220, 0.140>]
   [0.117, 0.167 color rgb <0.275, 0.220, 0.140>
                 color rgb <0.490, 0.155, 0.260>]
   [0.167, 0.230 color rgb <0.490, 0.155, 0.260>
                 color rgb <0.615, 0.060, 0.425>]
   [0.230, 0.305 color rgb <0.615, 0.060, 0.425>
                 color rgb <0.860, 0.140, 0.485>]
   [0.305, 0.385 color rgb <0.860, 0.140, 0.485>
                 color rgb <1.150, 0.325, 0.335>]
   [0.385, 0.469 color rgb <1.150, 0.325, 0.335>
                 color rgb <1.235, 0.560, 0.125>]
   [0.469, 0.548 color rgb <1.235, 0.560, 0.125>
                 color rgb <0.980, 0.665, 0.035>]
   [0.548, 0.623 color rgb <0.980, 0.665, 0.035>
                 color rgb <0.590, 0.550, 0.160>]
   [0.623, 0.703 color rgb <0.590, 0.550, 0.160>
                 color rgb <0.270, 0.320, 0.370>]
   [0.703, 0.787 color rgb <0.270, 0.320, 0.370>
                 color rgb <0.095, 0.165, 0.230>]
   [0.787, 0.891 color rgb <0.095, 0.165, 0.230>
                 color rgb <0.020, 0.145, 0.575>]
   [0.891, 0.950 color rgb <0.020, 0.145, 0.575>
                 color rgb <0.005, 0.140, 0.425>]
   [0.950, 1.000 color rgb <0.005, 0.140, 0.425>
                 color rgb <0.010, 0.010, 0.010>]
}
 
#declare shiny_mess = 
 texture{
   pigment{
    marble
    turbulence 1
    octaves 4
    omega 0.5
    lambda 4
    color_map {Wild_Map }
   }
   finish{phong 0.8 phong_size 200}
 }

#include "colors.inc"
#include "textures.inc"

// Sphere textures could also be a texture map (not included)
#declare shiny_mess =
texture {
  pigment {
   image_map {
     png "plasma2.png"
     map_type 1
   }
  }
  finish {Shiny }
}

 
#declare shiny_yellow = 
texture {
   pigment { rgb <1.2,0.9,0> }
   finish  { Shiny }
}
 
camera {
   location  <0, 0, -6>
   look_at  <0, 0, 0>
   angle 47
   right x*1/4
   right <4,0,0>
}

light_source { <-5, 1, -8> colour red 1.0 green 1.0 blue 1.0 }
light_source { < 5, 1, -8> colour red 1.0 green 1.0 blue 1.0 }
light_source { < 0, 5, -2> colour red 1.0 green 1.0 blue 1.0 }

// setup the initial spiral
 
#declare move = 0.95;  // static x-distance per blob
#declare px = 0.2;     // semi-random spacing parameter  (offset per blob)
#declare pr = 0.3;     // amount of radial in/out
 
#declare bx = -6.7;    // seed element position
#declare by = 1;
#declare bz = 0;
 
// gaussian multiplier to the pertubation function for the spheres
// dependency on the clock causes the effect to travel from left to right

// w is the width of the effect
#declare w = 0.1;

#declare  g1 = 1 / exp(((clock - .2) / w) * ((clock - .2) / w));
#declare  g2 = 1 / exp(((clock - .25) / w) * ((clock - .25) / w));
#declare  g3 = 1 / exp(((clock - .3) / w) * ((clock - .3) / w));
#declare  g4 = 1 / exp(((clock - .35) / w) * ((clock - .35) / w));
#declare  g5 = 1 / exp(((clock - .4) / w) * ((clock - .4) / w));
#declare  g6 = 1 / exp(((clock - .45) / w) * ((clock - .45) / w));
#declare  g7 = 1 / exp(((clock - .5) / w) * ((clock - .5) / w));
#declare  g8 = 1 / exp(((clock - .55) / w) * ((clock - .55) / w));
#declare  g9 = 1 / exp(((clock - .6) / w) * ((clock - .6) / w));
#declare g10 = 1 / exp(((clock - .65) / w) * ((clock - .65) / w));
#declare g11 = 1 / exp(((clock - .7) / w) * ((clock - .7) / w));
#declare g12 = 1 / exp(((clock - .75) / w) * ((clock - .75) / w));
#declare g13 = 1 / exp(((clock - .8) / w) * ((clock - .8) / w));
 
// sinusoidally modify the initial height of the seed element
// gosh I could sure go for a loop function in a ray tracer right about now <g>

#declare y01 =  g1*pr*sin((clock*5+px*1)*2*pi);
#declare y02 =  g2*pr*sin((clock*5+px*2)*2*pi);
#declare y03 =  g3*pr*sin((clock*5+px*3)*2*pi);
#declare y04 =  g4*pr*sin((clock*5+px*4)*2*pi);
#declare y05 =  g5*pr*sin((clock*5+px*5)*2*pi);
#declare y06 =  g6*pr*sin((clock*5+px*6)*2*pi);
#declare y07 =  g7*pr*sin((clock*5+px*7)*2*pi);
#declare y08 =  g8*pr*sin((clock*5+px*8)*2*pi);
#declare y09 =  g9*pr*sin((clock*5+px*9)*2*pi);
#declare y10 = g10*pr*sin((clock*5+px*10)*2*pi);
#declare y11 = g11*pr*sin((clock*5+px*11)*2*pi);
#declare y12 = g12*pr*sin((clock*5+px*12)*2*pi);
#declare y13 = g13*pr*sin((clock*5+px*13)*2*pi);
 
#declare y14 =  g1*pr*sin((clock*5+px*14)*2*pi);
#declare y15 =  g2*pr*sin((clock*5+px*15)*2*pi);
#declare y16 =  g3*pr*sin((clock*5+px*16)*2*pi);
#declare y17 =  g4*pr*sin((clock*5+px*17)*2*pi);
#declare y18 =  g5*pr*sin((clock*5+px*18)*2*pi);
#declare y19 =  g6*pr*sin((clock*5+px*19)*2*pi);
#declare y20 =  g7*pr*sin((clock*5+px*20)*2*pi);
#declare y21 =  g8*pr*sin((clock*5+px*21)*2*pi);
#declare y22 =  g9*pr*sin((clock*5+px*22)*2*pi);
#declare y23 = g10*pr*sin((clock*5+px*23)*2*pi);
#declare y24 = g11*pr*sin((clock*5+px*24)*2*pi);
#declare y25 = g12*pr*sin((clock*5+px*25)*2*pi);
#declare y26 = g13*pr*sin((clock*5+px*26)*2*pi);
 
#declare y27 =  g1*pr*sin((clock*5+px*27)*2*pi);
#declare y28 =  g2*pr*sin((clock*5+px*28)*2*pi);
#declare y29 =  g3*pr*sin((clock*5+px*29)*2*pi);
#declare y30 =  g4*pr*sin((clock*5+px*30)*2*pi);
#declare y31 =  g5*pr*sin((clock*5+px*31)*2*pi);
#declare y32 =  g6*pr*sin((clock*5+px*32)*2*pi);
#declare y33 =  g7*pr*sin((clock*5+px*33)*2*pi);
#declare y34 =  g8*pr*sin((clock*5+px*34)*2*pi);
#declare y35 =  g9*pr*sin((clock*5+px*35)*2*pi);
#declare y36 = g10*pr*sin((clock*5+px*36)*2*pi);
#declare y37 = g11*pr*sin((clock*5+px*37)*2*pi);
#declare y38 = g12*pr*sin((clock*5+px*38)*2*pi);
#declare y39 = g13*pr*sin((clock*5+px*39)*2*pi);
 
#declare y40 =  g1*pr*sin((clock*5+px*40)*2*pi);
#declare y41 =  g2*pr*sin((clock*5+px*41)*2*pi);
#declare y42 =  g3*pr*sin((clock*5+px*42)*2*pi);
#declare y43 =  g4*pr*sin((clock*5+px*43)*2*pi);
#declare y44 =  g5*pr*sin((clock*5+px*44)*2*pi);
#declare y45 =  g6*pr*sin((clock*5+px*45)*2*pi);
#declare y46 =  g7*pr*sin((clock*5+px*46)*2*pi);
#declare y47 =  g8*pr*sin((clock*5+px*47)*2*pi);
#declare y48 =  g9*pr*sin((clock*5+px*48)*2*pi);
#declare y49 = g10*pr*sin((clock*5+px*49)*2*pi);
#declare y50 = g11*pr*sin((clock*5+px*50)*2*pi);
#declare y51 = g12*pr*sin((clock*5+px*51)*2*pi);
#declare y52 = g13*pr*sin((clock*5+px*52)*2*pi);
 
#declare y53 =  g1*pr*sin((clock*5+px*53)*2*pi);
#declare y54 =  g2*pr*sin((clock*5+px*54)*2*pi);
#declare y55 =  g3*pr*sin((clock*5+px*55)*2*pi);
#declare y56 =  g4*pr*sin((clock*5+px*56)*2*pi);
#declare y57 =  g5*pr*sin((clock*5+px*57)*2*pi);
#declare y58 =  g6*pr*sin((clock*5+px*58)*2*pi);
#declare y59 =  g7*pr*sin((clock*5+px*59)*2*pi);
#declare y60 =  g8*pr*sin((clock*5+px*60)*2*pi);
#declare y61 =  g9*pr*sin((clock*5+px*61)*2*pi);
#declare y62 = g10*pr*sin((clock*5+px*62)*2*pi);
#declare y63 = g11*pr*sin((clock*5+px*63)*2*pi);
#declare y64 = g12*pr*sin((clock*5+px*64)*2*pi);
#declare y65 = g13*pr*sin((clock*5+px*65)*2*pi);
 
#declare s01 = vrotate(<bx,by+y01,bz>,<  0,0,0>) + <move*1, 0,0>;
#declare s02 = vrotate(<bx,by+y02,bz>,< 40,0,0>) + <move*2, 0,0>;
#declare s03 = vrotate(<bx,by+y03,bz>,< 80,0,0>) + <move*3, 0,0>;
#declare s04 = vrotate(<bx,by+y04,bz>,<120,0,0>) + <move*4, 0,0>;
#declare s05 = vrotate(<bx,by+y05,bz>,<160,0,0>) + <move*5, 0,0>;
#declare s06 = vrotate(<bx,by+y06,bz>,<200,0,0>) + <move*6, 0,0>;
#declare s07 = vrotate(<bx,by+y07,bz>,<240,0,0>) + <move*7, 0,0>;
#declare s08 = vrotate(<bx,by+y08,bz>,<280,0,0>) + <move*8, 0,0>;
#declare s09 = vrotate(<bx,by+y09,bz>,<320,0,0>) + <move*9, 0,0>;
#declare s10 = vrotate(<bx,by+y10,bz>,<  0,0,0>) + <move*10,0,0>;
#declare s11 = vrotate(<bx,by+y11,bz>,< 40,0,0>) + <move*11,0,0>;
#declare s12 = vrotate(<bx,by+y12,bz>,< 80,0,0>) + <move*12,0,0>;
#declare s13 = vrotate(<bx,by+y13,bz>,<120,0,0>) + <move*13,0,0>;
 
#declare s14 = vrotate(<bx,by+y14,bz>,<  0+72,0,0>) + <move*1, 0,0>;
#declare s15 = vrotate(<bx,by+y15,bz>,< 40+72,0,0>) + <move*2, 0,0>;
#declare s16 = vrotate(<bx,by+y16,bz>,< 80+72,0,0>) + <move*3, 0,0>;
#declare s17 = vrotate(<bx,by+y17,bz>,<120+72,0,0>) + <move*4, 0,0>;
#declare s18 = vrotate(<bx,by+y18,bz>,<160+72,0,0>) + <move*5, 0,0>;
#declare s19 = vrotate(<bx,by+y19,bz>,<200+72,0,0>) + <move*6, 0,0>;
#declare s20 = vrotate(<bx,by+y20,bz>,<240+72,0,0>) + <move*7, 0,0>;
#declare s21 = vrotate(<bx,by+y21,bz>,<280+72,0,0>) + <move*8, 0,0>;
#declare s22 = vrotate(<bx,by+y22,bz>,<320+72,0,0>) + <move*9, 0,0>;
#declare s23 = vrotate(<bx,by+y23,bz>,<  0+72,0,0>) + <move*10,0,0>;
#declare s24 = vrotate(<bx,by+y24,bz>,< 40+72,0,0>) + <move*11,0,0>;
#declare s25 = vrotate(<bx,by+y25,bz>,< 80+72,0,0>) + <move*12,0,0>;
#declare s26 = vrotate(<bx,by+y26,bz>,<120+72,0,0>) + <move*13,0,0>;
 
#declare s27 = vrotate(<bx,by+y27,bz>,<  0+144,0,0>) + <move*1, 0,0>;
#declare s28 = vrotate(<bx,by+y28,bz>,< 40+144,0,0>) + <move*2, 0,0>;
#declare s29 = vrotate(<bx,by+y29,bz>,< 80+144,0,0>) + <move*3, 0,0>;
#declare s30 = vrotate(<bx,by+y30,bz>,<120+144,0,0>) + <move*4, 0,0>;
#declare s31 = vrotate(<bx,by+y31,bz>,<160+144,0,0>) + <move*5, 0,0>;
#declare s32 = vrotate(<bx,by+y32,bz>,<200+144,0,0>) + <move*6, 0,0>;
#declare s33 = vrotate(<bx,by+y33,bz>,<240+144,0,0>) + <move*7, 0,0>;
#declare s34 = vrotate(<bx,by+y34,bz>,<280+144,0,0>) + <move*8, 0,0>;
#declare s35 = vrotate(<bx,by+y35,bz>,<320+144,0,0>) + <move*9, 0,0>;
#declare s36 = vrotate(<bx,by+y36,bz>,<  0+144,0,0>) + <move*10,0,0>;
#declare s37 = vrotate(<bx,by+y37,bz>,< 40+144,0,0>) + <move*11,0,0>;
#declare s38 = vrotate(<bx,by+y38,bz>,< 80+144,0,0>) + <move*12,0,0>;
#declare s39 = vrotate(<bx,by+y39,bz>,<120+144,0,0>) + <move*13,0,0>;
 
#declare s40 = vrotate(<bx,by+y40,bz>,<  0+216,0,0>) + <move*1, 0,0>;
#declare s41 = vrotate(<bx,by+y41,bz>,< 40+216,0,0>) + <move*2, 0,0>;
#declare s42 = vrotate(<bx,by+y42,bz>,< 80+216,0,0>) + <move*3, 0,0>;
#declare s43 = vrotate(<bx,by+y43,bz>,<120+216,0,0>) + <move*4, 0,0>;
#declare s44 = vrotate(<bx,by+y44,bz>,<160+216,0,0>) + <move*5, 0,0>;
#declare s45 = vrotate(<bx,by+y45,bz>,<200+216,0,0>) + <move*6, 0,0>;
#declare s46 = vrotate(<bx,by+y46,bz>,<240+216,0,0>) + <move*7, 0,0>;
#declare s47 = vrotate(<bx,by+y47,bz>,<280+216,0,0>) + <move*8, 0,0>;
#declare s48 = vrotate(<bx,by+y48,bz>,<320+216,0,0>) + <move*9, 0,0>;
#declare s49 = vrotate(<bx,by+y49,bz>,<  0+216,0,0>) + <move*10,0,0>;
#declare s50 = vrotate(<bx,by+y50,bz>,< 40+216,0,0>) + <move*11,0,0>;
#declare s51 = vrotate(<bx,by+y51,bz>,< 80+216,0,0>) + <move*12,0,0>;
#declare s52 = vrotate(<bx,by+y52,bz>,<120+216,0,0>) + <move*13,0,0>;
 
#declare s53 = vrotate(<bx,by+y53,bz>,<  0+288,0,0>) + <move*1, 0,0>;
#declare s54 = vrotate(<bx,by+y54,bz>,< 40+288,0,0>) + <move*2, 0,0>;
#declare s55 = vrotate(<bx,by+y55,bz>,< 80+288,0,0>) + <move*3, 0,0>;
#declare s56 = vrotate(<bx,by+y56,bz>,<120+288,0,0>) + <move*4, 0,0>;
#declare s57 = vrotate(<bx,by+y57,bz>,<160+288,0,0>) + <move*5, 0,0>;
#declare s58 = vrotate(<bx,by+y58,bz>,<200+288,0,0>) + <move*6, 0,0>;
#declare s59 = vrotate(<bx,by+y59,bz>,<240+288,0,0>) + <move*7, 0,0>;
#declare s60 = vrotate(<bx,by+y60,bz>,<280+288,0,0>) + <move*8, 0,0>;
#declare s61 = vrotate(<bx,by+y61,bz>,<320+288,0,0>) + <move*9, 0,0>;
#declare s62 = vrotate(<bx,by+y62,bz>,<  0+288,0,0>) + <move*10,0,0>;
#declare s63 = vrotate(<bx,by+y63,bz>,< 40+288,0,0>) + <move*11,0,0>;
#declare s64 = vrotate(<bx,by+y64,bz>,< 80+288,0,0>) + <move*12,0,0>;
#declare s65 = vrotate(<bx,by+y65,bz>,<120+288,0,0>) + <move*13,0,0>;
 
#declare radius1 = 0.92;
#declare strength1 = 0.6;
 
#declare spire = 
blob {
  threshold 0.5
  cylinder { <-5.5 ,0, 0 >, <5.5 ,0, 0 >,0.8,0.75 texture { shiny_yellow }}
  sphere { s01,radius1+y01,strength1 texture { shiny_mess rotate < 1*40+0*72,0,0> translate s01 rotate <clock*360,0,0> }}
  sphere { s02,radius1+y02,strength1 texture { shiny_mess rotate < 2*40+0*72,0,0> translate s02 rotate <clock*360,0,0> }}
  sphere { s03,radius1+y03,strength1 texture { shiny_mess rotate < 3*40+0*72,0,0> translate s03 rotate <clock*360,0,0> }}
  sphere { s04,radius1+y04,strength1 texture { shiny_mess rotate < 4*40+0*72,0,0> translate s04 rotate <clock*360,0,0> }}
  sphere { s05,radius1+y05,strength1 texture { shiny_mess rotate < 5*40+0*72,0,0> translate s05 rotate <clock*360,0,0> }}
  sphere { s06,radius1+y06,strength1 texture { shiny_mess rotate < 6*40+0*72,0,0> translate s06 rotate <clock*360,0,0> }}
  sphere { s07,radius1+y07,strength1 texture { shiny_mess rotate < 7*40+0*72,0,0> translate s07 rotate <clock*360,0,0> }}
  sphere { s08,radius1+y08,strength1 texture { shiny_mess rotate < 8*40+0*72,0,0> translate s08 rotate <clock*360,0,0> }}
  sphere { s09,radius1+y09,strength1 texture { shiny_mess rotate < 9*40+0*72,0,0> translate s09 rotate <clock*360,0,0> }}
  sphere { s10,radius1+y10,strength1 texture { shiny_mess rotate <10*40+0*72,0,0> translate s10 rotate <clock*360,0,0> }}
  sphere { s11,radius1+y11,strength1 texture { shiny_mess rotate <11*40+0*72,0,0> translate s11 rotate <clock*360,0,0> }}
  sphere { s12,radius1+y12,strength1 texture { shiny_mess rotate <12*40+0*72,0,0> translate s12 rotate <clock*360,0,0> }}
  sphere { s13,radius1+y13,strength1 texture { shiny_mess rotate <13*40+0*72,0,0> translate s13 rotate <clock*360,0,0> }}
 
  sphere { s14,radius1+y14,strength1 texture { shiny_mess rotate < 1*40+1*72,0,0> translate s14 rotate <clock*360,0,0> }}
  sphere { s15,radius1+y15,strength1 texture { shiny_mess rotate < 2*40+1*72,0,0> translate s15 rotate <clock*360,0,0> }}
  sphere { s16,radius1+y16,strength1 texture { shiny_mess rotate < 3*40+1*72,0,0> translate s16 rotate <clock*360,0,0> }}
  sphere { s17,radius1+y17,strength1 texture { shiny_mess rotate < 4*40+1*72,0,0> translate s17 rotate <clock*360,0,0> }}
  sphere { s18,radius1+y18,strength1 texture { shiny_mess rotate < 5*40+1*72,0,0> translate s18 rotate <clock*360,0,0> }}
  sphere { s19,radius1+y19,strength1 texture { shiny_mess rotate < 6*40+1*72,0,0> translate s19 rotate <clock*360,0,0> }}
  sphere { s20,radius1+y20,strength1 texture { shiny_mess rotate < 7*40+1*72,0,0> translate s20 rotate <clock*360,0,0> }}
  sphere { s21,radius1+y21,strength1 texture { shiny_mess rotate < 8*40+1*72,0,0> translate s21 rotate <clock*360,0,0> }}
  sphere { s22,radius1+y22,strength1 texture { shiny_mess rotate < 9*40+1*72,0,0> translate s22 rotate <clock*360,0,0> }}
  sphere { s23,radius1+y23,strength1 texture { shiny_mess rotate <10*40+1*72,0,0> translate s23 rotate <clock*360,0,0> }}
  sphere { s24,radius1+y24,strength1 texture { shiny_mess rotate <11*40+1*72,0,0> translate s24 rotate <clock*360,0,0> }}
  sphere { s25,radius1+y25,strength1 texture { shiny_mess rotate <12*40+1*72,0,0> translate s25 rotate <clock*360,0,0> }}
  sphere { s26,radius1+y26,strength1 texture { shiny_mess rotate <13*40+1*72,0,0> translate s26 rotate <clock*360,0,0> }}

  sphere { s27,radius1+y27,strength1 texture { shiny_mess rotate < 1*40+2*72,0,0> translate s27 rotate <clock*360,0,0> }}             
  sphere { s28,radius1+y28,strength1 texture { shiny_mess rotate < 2*40+2*72,0,0> translate s28 rotate <clock*360,0,0> }}             
  sphere { s29,radius1+y29,strength1 texture { shiny_mess rotate < 3*40+2*72,0,0> translate s29 rotate <clock*360,0,0> }}             
  sphere { s30,radius1+y30,strength1 texture { shiny_mess rotate < 4*40+2*72,0,0> translate s30 rotate <clock*360,0,0> }}             
  sphere { s31,radius1+y31,strength1 texture { shiny_mess rotate < 5*40+2*72,0,0> translate s31 rotate <clock*360,0,0> }}             
  sphere { s32,radius1+y32,strength1 texture { shiny_mess rotate < 6*40+2*72,0,0> translate s32 rotate <clock*360,0,0> }}             
  sphere { s33,radius1+y33,strength1 texture { shiny_mess rotate < 7*40+2*72,0,0> translate s33 rotate <clock*360,0,0> }}             
  sphere { s34,radius1+y34,strength1 texture { shiny_mess rotate < 8*40+2*72,0,0> translate s34 rotate <clock*360,0,0> }}
  sphere { s35,radius1+y35,strength1 texture { shiny_mess rotate < 9*40+2*72,0,0> translate s35 rotate <clock*360,0,0> }}
  sphere { s36,radius1+y36,strength1 texture { shiny_mess rotate <10*40+2*72,0,0> translate s36 rotate <clock*360,0,0> }}
  sphere { s37,radius1+y37,strength1 texture { shiny_mess rotate <11*40+2*72,0,0> translate s37 rotate <clock*360,0,0> }}
  sphere { s38,radius1+y38,strength1 texture { shiny_mess rotate <12*40+2*72,0,0> translate s38 rotate <clock*360,0,0> }}
  sphere { s39,radius1+y39,strength1 texture { shiny_mess rotate <13*40+2*72,0,0> translate s39 rotate <clock*360,0,0> }}
 
  sphere { s40,radius1+y40,strength1 texture { shiny_mess rotate < 1*40+3*72,0,0> translate s40 rotate <clock*360,0,0> }}
  sphere { s41,radius1+y41,strength1 texture { shiny_mess rotate < 2*40+3*72,0,0> translate s41 rotate <clock*360,0,0> }}
  sphere { s42,radius1+y42,strength1 texture { shiny_mess rotate < 3*40+3*72,0,0> translate s42 rotate <clock*360,0,0> }}
  sphere { s43,radius1+y43,strength1 texture { shiny_mess rotate < 4*40+3*72,0,0> translate s43 rotate <clock*360,0,0> }}
  sphere { s44,radius1+y44,strength1 texture { shiny_mess rotate < 5*40+3*72,0,0> translate s44 rotate <clock*360,0,0> }}
  sphere { s45,radius1+y45,strength1 texture { shiny_mess rotate < 6*40+3*72,0,0> translate s45 rotate <clock*360,0,0> }}
  sphere { s46,radius1+y46,strength1 texture { shiny_mess rotate < 7*40+3*72,0,0> translate s46 rotate <clock*360,0,0> }}
  sphere { s47,radius1+y47,strength1 texture { shiny_mess rotate < 8*40+3*72,0,0> translate s47 rotate <clock*360,0,0> }}
  sphere { s48,radius1+y48,strength1 texture { shiny_mess rotate < 9*40+3*72,0,0> translate s48 rotate <clock*360,0,0> }}
  sphere { s49,radius1+y49,strength1 texture { shiny_mess rotate <10*40+3*72,0,0> translate s49 rotate <clock*360,0,0> }}
  sphere { s50,radius1+y50,strength1 texture { shiny_mess rotate <11*40+3*72,0,0> translate s50 rotate <clock*360,0,0> }}
  sphere { s51,radius1+y51,strength1 texture { shiny_mess rotate <12*40+3*72,0,0> translate s51 rotate <clock*360,0,0> }}
  sphere { s52,radius1+y52,strength1 texture { shiny_mess rotate <13*40+3*72,0,0> translate s52 rotate <clock*360,0,0> }}
 
  sphere { s53,radius1+y53,strength1 texture { shiny_mess rotate < 1*40+4*72,0,0> translate s53 rotate <clock*360,0,0> }}
  sphere { s54,radius1+y54,strength1 texture { shiny_mess rotate < 2*40+4*72,0,0> translate s54 rotate <clock*360,0,0> }}
  sphere { s55,radius1+y55,strength1 texture { shiny_mess rotate < 3*40+4*72,0,0> translate s55 rotate <clock*360,0,0> }}
  sphere { s56,radius1+y56,strength1 texture { shiny_mess rotate < 4*40+4*72,0,0> translate s56 rotate <clock*360,0,0> }}
  sphere { s57,radius1+y57,strength1 texture { shiny_mess rotate < 5*40+4*72,0,0> translate s57 rotate <clock*360,0,0> }}
  sphere { s58,radius1+y58,strength1 texture { shiny_mess rotate < 6*40+4*72,0,0> translate s58 rotate <clock*360,0,0> }}
  sphere { s59,radius1+y59,strength1 texture { shiny_mess rotate < 7*40+4*72,0,0> translate s59 rotate <clock*360,0,0> }}
  sphere { s60,radius1+y60,strength1 texture { shiny_mess rotate < 8*40+4*72,0,0> translate s60 rotate <clock*360,0,0> }}
  sphere { s61,radius1+y61,strength1 texture { shiny_mess rotate < 9*40+4*72,0,0> translate s61 rotate <clock*360,0,0> }}
  sphere { s62,radius1+y62,strength1 texture { shiny_mess rotate <10*40+4*72,0,0> translate s62 rotate <clock*360,0,0> }}
  sphere { s63,radius1+y63,strength1 texture { shiny_mess rotate <11*40+4*72,0,0> translate s63 rotate <clock*360,0,0> }}
  sphere { s64,radius1+y64,strength1 texture { shiny_mess rotate <12*40+4*72,0,0> translate s64 rotate <clock*360,0,0> }}
  sphere { s65,radius1+y65,strength1 texture { shiny_mess rotate <13*40+4*72,0,0> translate s65 rotate <clock*360,0,0> }}
}  
 
object { spire pigment {color White} rotate <clock*360,0,0>}
