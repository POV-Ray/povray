// Persistence Of Vision raytracer version 3.1 sample scene by Dieter Bayer.
//
// This scene demonstrates distance based attenuation in translucent objects.
//

#global_settings { assumed_gamma 2.2 }

#include "colors.inc"

#declare IOR = 1.05;
#declare Distance = 20;

#declare Col1 = -15;
#declare Col2 =  35;
#declare Row1 =  25;
#declare Row2 =  -5;

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
// Translucent sphere without attenuation
//

sphere { <Col1, Row1, 0>, 10
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
// Translucent sphere with linear attenuation
//

sphere { <Col2, Row1, 0>, 10
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
// Translucent sphere with quadratic attenuation
//

sphere { <Col1, Row2, 0>, 10
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
// Translucent sphere with cubic attenuation
//

sphere { <Col2, Row2, 0>, 10
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

