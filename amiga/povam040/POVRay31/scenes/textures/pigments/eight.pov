/*
 * eight.pov
 *
 * POV-Ray 3.1 scene description for two billiard balls
 *
 * Copyright (c) 1991, 1996 Ville Saari
 *
 * Created: 07-Jan-91
 * Updated: 02-Jul-96
 * Updated: 18-Jan-98
 *
 * Author:
 *
 *   Ville Saari
 *   Tallbergin puistotie 7 B 21
 *   00200 Helsinki
 *   FINLAND
 *
 *   EMail: vs@iki.fi
 */

global_settings { assumed_gamma 1 }
#default { finish { ambient .08 diffuse .92 } }
camera { location <-15, 8, -10> look_at <0, 5, 5> }
light_source { <-30, 30, -15> rgb 1 }

#declare number8 = 
   texture {
      pigment{
         wood
         ramp_wave
         color_map{
            [ .155 color rgb 1 ]
            [ .155 color rgb 0 ]
         }
      }
   }
   texture{
      pigment{
         wood
         ramp_wave
         color_map{
            [ .033 color rgbf 1 ]
            [ .033 color rgb 0  ]
            [ .067 color rgb 0  ]
            [ .067 color rgbf 1 ]
         }
         scale <1.15, 1, 1>
         translate .05*y
      }
   }
   texture{
      pigment{
         wood
         ramp_wave
         color_map{
            [ .033 color rgbf 1 ]
            [ .033 color rgb 0  ]
            [ .067 color rgb 0  ]
            [ .067 color rgbf 1 ]
         }
      quick_colour rgb 0
      scale <1.15, 1, 1>
      translate -.05*y
      }
      finish { specular 1 roughness .005 reflection .12 }
   }

plane{y, 0
   pigment { rgb <0, .53, 0> }
   finish { crand .08 }
}

sphere{5*y, 5
   texture{
      number8
      scale 12.903226
      rotate <-30, 20, -45>
      translate 5*y
   }
}

sphere{<0, 5, 10>, 5
   pigment { rgb 1 }
   finish { specular 1 roughness .005 reflection .12 }
}

union{
   cone{<0, 0, 0> 1 60*z 3
      pigment{
         wood
         colour_map{
            [ .4 colour rgb <.34, .15, 0> ]
            [ .7 colour rgb <.15, .06, 0> ]
         }
         quick_colour rgb <.3, .1, 0>
         translate <50, -50, 0>
         scale .15
         turbulence .02
         scale <1, 1, 5>
     }
     finish { specular 1 roughness .005 reflection .12 }}

   intersection{
      sphere { -0.2*z, 1.1 }
      plane { z, 0 }
      plane {-z, .4 }
      pigment { rgb <.06, .29, 1> }
   }

   rotate <-10, 0, 45>
   translate <-4.5, 6, 14.5>
}
