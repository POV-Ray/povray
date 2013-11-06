// Persistence of Vision Ray Tracer POV-Ray scene file ".pov"
// POV-Ray 3.7
// Desc: Orthographic Scene Example
//       useful for generating image_maps, heightfields, etc..
//       To be rendered with quadratic field of view i.e. +w512 +h512 ....
//--------------------------------------------------------------------------
#version 3.7;
global_settings{ assumed_gamma 1.0 }
#default{ finish{ ambient 0.1 diffuse 0.9 }} 
//--------------------------------------------------------------------------

camera {
  orthographic
  location <0,0,1>     // position & direction of view
  look_at  <0,0,0>
  right    x*image_width/image_height
}

// ----------------------------------------

box {                  // with a quadatic field of view this box fits exactly in view 
  <-0.5, -0.5, 0>, <0.5, 0.5, 0>
  texture {
    pigment {
      agate
      color_map {
        [0.0 color rgb 0.0 ]
        [1.0 color rgb 1.0 ]
      }
    }
    finish {
      ambient 1.0
      diffuse 0.0
    }
  }
}

//------------------------------------------------------