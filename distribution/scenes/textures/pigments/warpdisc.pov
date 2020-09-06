#version 3.8;
global_settings{ assumed_gamma 1.0 }

camera {
  location <5, -20, 14>
  direction y
  sky z
  up z
  right image_width*x/image_height
  look_at 0
  angle 15
}

#default{ finish { emission 0.5 reflection 0.5 } }

cylinder { 0.5*z, -0.5*z, 3
  texture { checker
    texture { pigment { rgb <0.3,0.9,0> } }
    texture { pigment { rgb <1,0.0,0.5> } }
    warp { disc radius 2.75 scale 0.75 }
  }
}
light_source { <0, -10, 20> 1 }
