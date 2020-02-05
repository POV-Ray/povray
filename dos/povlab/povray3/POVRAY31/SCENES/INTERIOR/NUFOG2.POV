// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer

global_settings { assumed_gamma 2.2 }

#include "colors.inc"

#declare Brown = color red 1 green 0.8 blue 0.2

camera { 
   location  <0, 25, -20> 
   up y
   right x*1.33
   direction z*1.5
   look_at   <0, 5, 0> 
} 
    
#declare Orig_Fog = 1    
#declare Ground_Mist= 2

fog{ 
    fog_type Ground_Mist
    fog_alt 10
    fog_offset 0
    distance 2
    color LightSteelBlue
    turbulence <0.05, 0.05, 0.05>
    omega 0.25
    lambda 2.5
    octaves 6
}    
light_source { <100,300,-100> color White} 
    
plane{ 
  y, -3
  pigment{ checker color White color Black} 
  finish{diffuse 1} 
  scale<3,1,3> 
} 
    
//sphere{<0,0,0> 1000 pigment{color Black}} 
background { color Black }
    
#declare dd = 4
#declare dd2 = 4
    
#declare pole =   box { <-0.5, 0, -0.5> <0.5, 10, 0.5> 
                  //cylinder{<0,-3,0>,<0,10,0>,0.7 
                   pigment{color Brown} 
                   finish{diffuse 2} 
                } 
    
#declare row = union{ 
                 object{pole} 
                 object{pole translate<dd,0,0>} 
                 object{pole translate<dd*2,0,0>} 
                 object{pole translate<dd*3,0,0>} 
                 object{pole translate<dd*4,0,0>} 
                } 
    
#declare poles = union{ 
                   object{row} 
                   object{row translate<0,0,dd2>} 
                   object{row translate<0,0,dd2*2>} 
                   object{row translate<0,0,dd2*3>} 
                   object{row translate<0,0,dd2*4>} 
                 } 
    
object{poles rotate y*45 translate<-2*dd,0,0>} 
