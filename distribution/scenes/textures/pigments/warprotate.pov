#version 3.8;
global_settings{ assumed_gamma 1.0 }

camera {
  location <5, -20, 14>
  direction y
  sky z
  up z
  right image_width*x/image_height
  look_at 0
  angle 30
}

#default{ finish { emission 0.5 reflection 0.5 } }

cylinder { 3.5*z, -3.5*z, 3
  texture { checker
    texture { pigment { red 1 } }
    texture { pigment { green 1 } }
    warp { rotate -1/29 }
  }
}
light_source { <0, -10, 20> 1 }
