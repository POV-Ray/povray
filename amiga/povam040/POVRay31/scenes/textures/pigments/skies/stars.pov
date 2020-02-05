global_settings { assumed_gamma 2.2 }
#include "colors.inc"
#include "stars.inc"

camera {
    location <0, 0, -100>
    up y
    right x*1.33
    direction z
    angle 25
}

#declare Stripe=texture{pigment{White}finish{ambient 1 diffuse 0}}

#declare SF_123=
 texture
 {
   gradient x
   texture_map
   {
     [0/3+0.005 pigment{Cyan}finish{ambient 1 diffuse 0}]
     [0/3+0.005 Starfield1]
     [1/3       Starfield1]
     [1/3       Stripe]
     [1/3+0.005 Stripe]
     [1/3+0.005 Starfield2]
     [2/3       Starfield2]
     [2/3       Stripe]
     [2/3+0.005 Stripe]
     [2/3+0.005 Starfield3]
   }
 }

#declare SF_456=
 texture
 {
   gradient x
   texture_map
   {
     [0/3+0.005 pigment{Cyan}finish{ambient 1 diffuse 0}]
     [0/3+0.005 Starfield4]
     [1/3       Starfield4]
     [1/3       Stripe]
     [1/3+0.005 Stripe]
     [1/3+0.005 Starfield5]
     [2/3       Starfield5]
     [2/3       Stripe]
     [2/3+0.005 Stripe]
     [2/3+0.005 Starfield6]
   }
 }



plane {z,0
 texture
 {
   gradient y
   texture_map
   {
     [0/2+0.005 pigment{Cyan}finish{ambient 1 diffuse 0}]
     [0/2+0.005 SF_456]
     [1/2       SF_456]
     [1/2       Stripe]
     [1/2+0.005 Stripe]
     [1/2+0.005 SF_123]
   }
   }
   translate <-1/2,-1/2,0>
   scale 40
 }

