// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: fractals3.pov
// Last updated: 6/5/02
// Author: Juha Nieminen
// Description:
// Demonstrates the use of fractal patterns.
// Three parts of the Mandelbrot fractal forming an acronym.

// In WinPov, use the right mouse button to copy the following
// to the command line:
// +W800 +H267 +a0.1

#version 3.7;

global_settings {
  assumed_gamma 1.0
}

#declare P =
  box
  { <-2,-2,0><2,2,.1>
    pigment
    { mandel 1000
      color_map { [0 rgb 0][.5 rgb y+z][1 rgb x+y][1 rgb 0] }
      translate -<-.7653,.1005>
      scale 1000
      scale <-1,1,1>
    }
    finish { ambient 1 }
  }

#declare O =
  box
  { <-2,-2,0><2,2,.1>
    pigment
    { mandel 10000
      color_map { [0 rgb 0][.5 rgb x][1 rgb x+y][1 rgb 0] }
      translate
       -<(-0.749979169204317290207435344827-0.749968488051973540207435344827)/2,
         (0.008640613399268010370560473549+0.008630313716650822870560473549)/2>
      scale 350000
      rotate z*-45
    }
    finish { ambient 1 }
  }

#declare V =
  box
  { <-2,-2,0><2,2,.1>
    pigment
    { mandel 300
      color_map { [.1 rgb 0][.5 rgb y][1 rgb 1][1 rgb 0] }
      translate
       -<(-0.596201137068877025831455721002-0.596168159011015697706455721002)/2,
         (0.665096856611702768182969470741+0.665064938141339606920170835929)/2>
      scale 130000
      rotate z*-40
    }
    finish { ambient 1 }
  }

camera { 
          right x*3 
          location -z*4 
          look_at 0
        }
object { P translate -x*4 }
object { O }
object { V translate x*4 }

