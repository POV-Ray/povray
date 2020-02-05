// Slinky.pov
// Three sets of perpendicular rings whose count and position varies
// as it breathes in and out
// 01/10/95 - Jeff Bowermaster

#declare d = 3.45+1*sin(2*pi*clock);
#declare d_angle=pow(2,d);

#include "colors.inc"
#include "textures.inc"

camera {
  location <4, 4, 4>
  right <4/3, 0, 0>
  up <0, 1, 0>
  direction <-1,-1,-1>
  look_at <0, 0.25, 0>
}

background { color rgb <0.078,0.361,0.753>*0.5 }

#declare dim = 0.5;

light_source { <  5,  0, 0 > color rgb <0.25,0.5,0.75> }
light_source { <  0,  5, 0 > color rgb <1,0.1,0.1> }
light_source { <  0,  0, 5 > color rgb <1,0.8,0.1> }

light_source { <  0,  0, 0 > color rgb <1,1,1>*dim }

#declare rad = pi/180;

#declare white_plastic = 
texture {
    pigment { color rgb <1, 1, 1> }
    finish {ambient 0.1 diffuse 0.8 phong 0.5 phong_size 100 }
}

#declare thick = 0.025 ;    // thickness of the rings
#declare smidge = 0.0001;   // slight excess to bore out the cylinders
#declare ang=d_angle;       // parameter that determines the number of rings
                           // it's the number of degrees between each ring

#declare f = 0.2 ;          // scalar for in/out motion
#declare w = 1.0 ;          // sets the width of the effect

// although each axis could move at different times, they don't

#declare cx = 0.50;         // center of maximum displacement
#declare cy = 0.50;
#declare cz = 0.50;

#while (ang < 90)

   #declare r = sin(ang * rad);
   #declare c = cos(ang * rad);

   #declare fx = f/exp(((clock - cx)/w)*((clock - cx)/w))/r;
   #declare fy = f/exp(((clock - cy)/w)*((clock - cy)/w))/r;
   #declare fz = f/exp(((clock - cz)/w)*((clock - cz)/w))/r;

   difference {
      object { cylinder { <0,0,-c-fz-thick>,<0,0,-c-fz+thick>,r }}
      object { cylinder { <0,0,-c-fz-thick-fz-smidge>,<0,0,-c-fz+thick+smidge>,r-thick*2 }}
      texture { white_plastic }
   }
   difference {
      object { cylinder { <0,0, c+fz-thick>,<0,0, c+fz+thick>,r }}
      object { cylinder { <0,0, c+fz-thick-smidge>,<0,0, c+fz+thick+smidge>,r-thick*2 }}
      texture { white_plastic }
   }
   difference {
      object { cylinder { <0,-c-fy-thick,0>,<0,-c-fy+thick,0>,r }}
      object { cylinder { <0,-c-fy-thick-smidge,0>,<0,-c-fy+thick+smidge,0>,r-thick*2 }}
      texture { white_plastic }
   }
   difference {
      object { cylinder { <0, c+fy-thick,0>,<0, c+fy+thick,0>,r }}
      object { cylinder { <0, c+fy-thick-smidge,0>,<0, c+fy+thick+smidge,0>,r-thick*2 }}
      texture { white_plastic }
   }
   difference {
      object { cylinder { <-c-fx-thick,0,0>,<-c-fx+thick,0,0>,r }}
      object { cylinder { <-c-fx-thick-smidge,0,0>,<-c-fx+thick+smidge,0,0>,r-thick*2 }}
      texture { white_plastic }
   }
   difference {
      object { cylinder { < c+fx-thick,0,0>,< c+fx+thick,0,0>,r }}
      object { cylinder { < c+fx-thick-smidge,0,0>,< c+fx+thick+smidge,0,0>,r-thick*2 }}
      texture { white_plastic }
   }
  #declare ang=ang+d_angle;

#end


disc { <0,0,0>, <0,0,1>,10 texture { white_plastic } translate <0,0,-3> }
disc { <0,0,0>, <0,1,0>,10 texture { white_plastic } translate <0,-3,0> }
disc { <0,0,0>, <1,0,0>,10 texture { white_plastic } translate <-3,0,0> }
