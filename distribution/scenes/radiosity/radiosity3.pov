// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: radiosity3.pov
// Desc: radiosity tutorial scene
// Date: 2000-2001
// Auth: Christoph Hormann

// -w240 -h180 +a0.3

// Updated: 29Dec2010 (cli) modified scene to use 3.7 syntax and more realistic diffuse finish

//#version 3.6;
#version 3.7;

#declare use_light=false;

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
  location <3.8, 7.2, -10>
  look_at <0.0, 1.2, 0.0>
}


sphere {
  <0, 0, 0>, 1
  texture {
   pigment {
     gradient y
     color_map {
       [0.0 color rgb < 0.900, 0.910, 1.000 >]
       [0.0 color rgb < 0.700, 0.705, 1.000 >]
     }
   }
   finish { diffuse 0 #if (version < 3.7) ambient 1.4 #else emission 1.4 #end }
  }
  hollow on
  no_shadow
  scale 30000
}


#declare FinX= finish { diffuse 0.65 #if (version < 3.7) ambient 0 #end }

union {

  superellipsoid { <0.7, 0.3>
    translate < -1.6, 0.5, -4.1>
    texture {
      pigment { color rgb <1.0, 0.7, 0.6> }
      finish { FinX }
    }
  }

  torus { 2.1, 0.3
    rotate 90*x
    translate < -1.6, 0.0, -4.1>
  }

  union {
  #local Cnt=0;
  #while (Cnt < 6)
    cylinder { < 5.2-Cnt*0.3, 0, -3+Cnt>, < 5.2-Cnt*0.3, 1+Cnt*0.6, -3+Cnt>, 0.25 }
    sphere { < 0, 0, 0> 0.5 scale<1, 0.3, 1> translate < 5.2-Cnt*0.3, 1+Cnt*0.6, -3+Cnt> }
    #local Cnt=Cnt+1;
  #end
  }

  cylinder { < 1.8, 0, -0.75>, < 1.8, 1.8, -0.75>, 1
    texture {
      pigment { color rgb <0.6, 0.7, 1.0> }
      finish { FinX }
    }
  }
  sphere { < 1.8, 1.8, -0.75> 1
    texture {
      pigment { color rgb <0.6, 0.7, 1.0> }
      finish { FinX }
    }
  }

  sphere { < 0, 0, 0> 1
    scale <2.6,0.3,2.6>
    translate 4.5*y
  }
  sphere { < 0, 4.8, 0> 0.8 }

  box { <-3, 4.2, -3>, < 3.0,  4.0,  3> }
  difference {
    box { <-3,  0.0, -3>, <-2.8,  4.2,  3> }
    box { <-5, -0.1,  0>, <-2.0,  3.2,  2> }
  }
  box { < 3, 0.0, -3>, < 2.8,  4.2,  3> }

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
