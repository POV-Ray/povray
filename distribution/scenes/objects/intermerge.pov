#version 3.8;
global_settings{ assumed_gamma 1.0 }
#default { finish {emission 0.15 diffuse 0.75 reflection 0.0 phong 0.2 } }

#declare Unzoom = 3;
#declare Radius = 1.444;

camera { orthographic
location 6*Radius*<-6,2,-9>
direction Radius*z
up Unzoom*Radius*y
right Unzoom*Radius*x*image_width/image_height
look_at 0
}

light_source { Radius*<-2,50,-30>*100, 1 }
light_source { Radius*<0,0,-3>*100, 0.67 }
light_source { Radius*<-10,0,3>*100, 0.67 }

intermerge {
sphere { x, Radius
  texture { pigment { color red 0.75  } }
}
sphere { -x, Radius
  texture { pigment { color blue 0.75 green 0.75  } }
}
sphere { y, Radius
  texture { pigment { color green 0.75  } }
}
sphere { -y, Radius
  texture { pigment { color red 0.75 blue 0.75  } }
}
sphere { z, Radius
  texture { pigment { color blue 0.75  } }
}
sphere { -z, Radius
  texture { pigment { color green 0.75 red 0.75  } }
}
sphere { 0, Radius*5/5
  texture { uv_mapping
    pigment {checker
    pigment {
     color srgb 0.95 filter 0.95 },
     pigment { color srgb 0.4 filter 0.7  }
}
scale <1/50,1/25,1>
}
}
  range{ 3 }
  interior { ior 1.333 }
}
plane { y, -Radius*1.2
  texture { pigment { color srgb 0.75  } }
}
plane { z, Radius*2.2
  texture { pigment { color srgb 0.95  } }
}
