// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 2.2
  max_trace_level 5
}

#include "colors.inc"
#include "shapes.inc"
#include "metals.inc"

#declare T01 = texture { T_Silver_1A }
#declare T02 = texture { T_Silver_1B }
#declare T03 = texture { T_Silver_1C }
#declare T04 = texture { T_Silver_1D }
#declare T05 = texture { T_Silver_1E }

#declare T06 = texture { T_Silver_2A }
#declare T07 = texture { T_Silver_2B }
#declare T08 = texture { T_Silver_2C }
#declare T09 = texture { T_Silver_2D }
#declare T10 = texture { T_Silver_2E }

#declare T11 = texture { T_Silver_3A }
#declare T12 = texture { T_Silver_3B }
#declare T13 = texture { T_Silver_3C }
#declare T14 = texture { T_Silver_3D }
#declare T15 = texture { T_Silver_3E }

#declare T16 = texture { T_Silver_4A }
#declare T17 = texture { T_Silver_4B }
#declare T18 = texture { T_Silver_4C }
#declare T19 = texture { T_Silver_4D }
#declare T20 = texture { T_Silver_4E }

#declare T21 = texture { T_Silver_5A }
#declare T22 = texture { T_Silver_5B }
#declare T23 = texture { T_Silver_5C }
#declare T24 = texture { T_Silver_5D }
#declare T25 = texture { T_Silver_5E }


//#include "stage_xy.inc"
#include "stage_xz.inc"
