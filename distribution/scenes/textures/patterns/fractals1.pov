// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: fractals1.pov
// Last updated: 6/5/02
// Author: Juha Nieminen
// Description:
// Demonstrates the use of fractal patterns.
// The fractals used are:
// - Floor: magnet1m as both pigment and normal with
//   interior type 1 in both and exterior type 5 in pigment.
// - Left box: A julia pigment with interior type 1.
// - Middle box: A mandelbrot pigment with interior type 5
//   and exterior type 6.
// - Right box: A mandel4 pigment with interior type 1
//   and exterior type 5.
// - Sphere: A julia4 normal.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

camera { location <-1,2,-5>*1.2 
         right     x*image_width/image_height
         look_at <0,0.2,0>
         angle 35
        }
light_source { <10,50,-30>, 1 }

// Floor made with magnet1m:
plane
{ -z,0
  texture
  { pigment
    { magnet 1 mandel 50
      color_map
      { [0 rgb <1,.8,.5>]
        [1 rgb <.8,.5,.4>]
      }
      interior 1,200
      exterior 5,1
    }
    normal
    { magnet 1 mandel 50 .2
      slope_map
      { [0 <1,0>][1 <0,-1>]
      }
      interior 1,200
      accuracy .0005
    }
    finish { specular .5 reflection .5 }

    translate <-1.9,-1,0>
    scale 20
  }
  rotate x*90
}

// Julia:
box
{ <-2,-2,0><2,2,-.1>
  pigment
  { julia <.3,.44> 30
    interior 1,1
    color_map
    { [0 rgb <.3,.2,.1>][.25 rgb x][.5 rgb x+y][1 rgb 1]
    }
    scale 1.3
  }
  translate y*2 scale .4
  rotate <30,-20,0>
  translate <-1.1,0,2>
}

// Mandel:
box
{ <-2,-2,0><2,2,-.1>
  pigment
  { mandel 10 color_map
    { [0 rgb 0][.25 rgb z][.5 rgb <.1,.8,1>][1 rgb 1]
    }
    interior 5,2
    exterior 6,.05
    translate x*.6
    scale 1.3
  }
  translate y*2 scale .4
  rotate <30,0,0>
  translate <.5,0,2.2>
}

// Mandel4:
box
{ <-2,-2,0><2,2,-.1>
  pigment
  { mandel 50
    exponent 4
    interior 1,.5
    exterior 5,.01
    color_map
    { [0 rgb 0][.3 rgb y][.6 rgb <.5,1,0>][1 rgb x+y]
    }
    scale 1.3
  }
  translate y*2 scale .4
  rotate <30,20,0>
  translate <2,0,1.6>
}

// Julia4:
sphere
{ 0,2
  pigment { rgb 1 }
  normal
  { julia <-.5,.5> 10 1
    exponent 4
    slope_map { [0 <0,0>][.5 <.5,1>][1 <1,0>] }
    scale 1.5
    rotate x*30
  }
  finish { specular .5 reflection .4 }
  translate y*2
  scale .15 translate <-.2,0,-1>
}
