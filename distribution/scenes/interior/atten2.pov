// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Scene by Dieter Bayer.
//
// This scene demonstrates distance based attenuation in translucent objects.
//
// -w320 -h240
// -w800 -h600 +a0.3
//

#version 3.7;

global_settings {
  assumed_gamma 1
  }

#include "colors.inc"

#declare IOR = 1.5;
#declare Distance = 5;

#declare Col1 = -15;
#declare Col2 =  35;
#declare Row1 =  30;
#declare Row2 = -10;

camera {
  orthographic
  location <0, 0, -100>
  right 80 * 4/3 * x
  up    80 * y
  look_at <0, 0, 0>
}

//
// Use beloved famous raytrace green/yellow checkered wall
//

plane { -z, -20
   pigment {
      checker color Yellow color Green
      scale 4
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

//
// Make a prism.
//

#declare Prism = prism {
  -0.5, 0.5, 4
  <-1, -0.3>, <1, 0>, <-1, 0.3>, <-1, -0.3>
  scale 12
}

//
// Translucent prism without attenuation
//

object { Prism translate <Col1, Row1, 0>
  pigment { rgbt<1, 1, 1, 0.9> }
  finish {
    ambient 0
    diffuse 0
    phong 1
    phong_size 200
  }
  interior {
    ior IOR
    fade_distance Distance
    fade_power 0
  }
}

//
// Translucent prism with linear attenuation
//

object { Prism translate <Col2, Row1, 0>
  pigment { rgbt<1, 1, 1, 0.9> }
  finish {
    ambient 0
    diffuse 0
    phong 1
    phong_size 200
  }
  interior {
    ior IOR
    fade_distance Distance
    fade_power 1
  }
}

//
// Translucent prism with quadratic attenuation
//

object { Prism translate <Col1, Row2, 0>
  pigment { rgbt<1, 1, 1, 0.9> }
  finish {
    ambient 0
    diffuse 0
    phong 1
    phong_size 200
  }
  interior {
    ior IOR
    fade_distance Distance
    fade_power 2
  }
}

//
// Translucent prism with cubic attenuation
//

object { Prism translate <Col2, Row2, 0>
  pigment { rgbt<1, 1, 1, 0.9> }
  finish {
    ambient 0
    diffuse 0
    phong 1
    phong_size 200
  }
  interior {
    ior IOR
    fade_distance Distance
    fade_power 3
  }
}

light_source { <10000, 10000, -10000> color White }

