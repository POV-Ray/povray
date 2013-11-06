// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: radiosity2.pov
// Desc: radiosity tutorial scene
// Date: 2000-2001
// Auth: Christoph Hormann

// -w240 -h180 +a0.3

//#version 3.6;
#version 3.7;

#declare use_light=true;

global_settings {
  assumed_gamma 1.0
  radiosity {
    pretrace_start 0.08
    pretrace_end   0.04
    count 35

    nearest_count 5
    error_bound 0.2
    recursion_limit 1

    low_error_factor .5
    gray_threshold 0.0
    minimum_reuse 0.015
    brightness 1

    adc_bailout 0.01/2

  }
}

#if (use_light)
  light_source {
    <-1.5, 1, -0.5>*10000
    color rgb <1.0, 0.92, 0.77>
  }
#end

camera {
  location <3.8, 7.8, -10>
  look_at <0.0, 0.0, 0.0>
}


sphere {
  <0, 0, 0>, 1
  texture {
   pigment {
     gradient y
     color_map {
       [0.0 color rgb < 0.880, 0.935, 0.976 >]
       [0.2 color rgb < 0.300, 0.450, 0.950 >]
     }
   }
   finish { diffuse 0 #if (version < 3.7) ambient 1 #else emission 1 #end }
  }
  hollow on
  no_shadow
  scale 30000
}


#declare FinX= finish { diffuse 0.65 #if (version < 3.7) ambient 0 #end }

#declare Wing_Length=4;
#declare Wing_Round=1.2;
#declare Wing_Rad=0.4;

#declare Wing=
union {
  cylinder { <0, 0, 0>, < 0, 0, Wing_Length>, Wing_Rad }
  intersection {
    torus { Wing_Round, Wing_Rad }
    box { <0, -5, 0>, < 5, 5, 5> }
    rotate 90*z
    rotate 180*y
    translate < 0, -Wing_Round, -Wing_Length>
  }
  cylinder { <0, -Wing_Round,    -Wing_Length-Wing_Round>,
             <0, -Wing_Round-10, -Wing_Length-Wing_Round>, Wing_Rad }
}

union {
  superellipsoid { <0.7, 0.3>
  texture {
    pigment { color rgb <1.0, 0.7, 0.6> }
    finish { FinX }
  }
  }
  object { Wing }
  object { Wing rotate 180*y }

  torus { Wing_Length+Wing_Round, 0.5 translate (0.5-4)*y }
  torus { Wing_Length+Wing_Round, 1.0 scale < 1, 0.1, 1>  translate (0.5-4)*y
  texture {
    pigment { color rgb <1.0, 0.7, 0.6> }
    finish { FinX }
  }
  }

  cylinder { < 0, -4, 0>,   < 0, -1,   0>, 1 }

  sphere { < 0, 0, 0> 1
    scale <1.7,0.2,1.7>
    translate -1*y
  }

  box { <-2.8, -3.2, -2.8>, < 2.8,  -3.1,  2.8> }

  union {
    cylinder { < -2, 0, 0>, < 2, 0, 0>, 1.0 }
    sphere { < -2, 0, 0> 1.0 }
    sphere { <  2, 0, 0> 1.0 }
    scale < 1.2, 0.2, 1.2>
  }

  translate <0, 4, 0>

  texture {
    pigment { color rgb 1 }
    finish { FinX }
  }
}



plane {
  y, 0
  texture {
    pigment { color rgb 1 }
    finish { FinX }
  }
}
