/*
 * planet.pov
 *
 * POV-Ray 3.1 scene description for Earth-like planet
 *
 * Copyright (c) 1990, 1996 Ville Saari
 *
 * Created: 29-Dec-90
 * Updated: 02-Jul-96
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

default { finish { ambient 0.000002 diffuse 0.999998 } }

camera { location -80*z }

light_source { 20000*<-1, 1, -1> rgb 1 }

sphere
   {
   0, 30

   pigment
      {
      bozo

      colour_map
         {
         [.7   colour rgb <0, .11, .67>   ]
         [.7   colour rgb <0, .4, 0>      ]
         [.999 colour rgb <.4, .53, .0>   ]
         [1.01 colour rgb <.53, .11, .11> ]
         }

      turbulence .5
      scale 10
      translate 100*x
      }

   finish { crand .08 }
   }

sphere
   {
   0, 30.2

   pigment
      {
      bozo

      colour_map { [.4 .9 colour rgbf 1 colour rgb 1 ] }
      turbulence 1.0
      scale <12, 3, 12>
      rotate <30, 0, -45>
      }
   }
