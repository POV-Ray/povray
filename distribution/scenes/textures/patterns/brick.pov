// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Brick pattern example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

#declare T1=
 texture{
   pigment{
     brick color rgb<1,1,1> color rgb<0.5,0.1,0>
     scale 0.1
   }
 }

#declare T2=
 texture{
   pigment{White}
   normal{ 
     pigment_pattern{ 
          brick color rgb<1,1,1> color rgb<0.5,0.1,0> 
          scale 0.1
        }   
     , 0.5  
   }
   finish{phong 0.8 phong_size 200}
 }

#include "pignorm.inc"
