// PoVRay 3.7.1 Scene File "torus2.pov"
// author: Christoph Lipka
// date:   2016-05-05
//
// Demonstrates the use and effect of the torus spindle modes for
// self-intersecting torus primitives, as introduced with POV-Ray 3.7.1
//
// +w200 +h200 +kfi1 +kff4 +ua
//------------------------------------------------------------------------

#version 3.71;
global_settings{ assumed_gamma 1.0 }

camera {
  angle 15
  right     x*image_width/image_height
  location  <9,3,-9>
  look_at   <0,0,0>
}

light_source { <0,500,-1000> colour rgb 1 }
background{ colour rgbt 1 }

default {
  finish { ambient 0.1 diffuse albedo 1.0 specular albedo 0.0 }
}

#declare HintTexture      = texture { pigment { colour rgb 0.8 transmit 0.8 } }
#declare OutsideTexture   = texture { pigment { colour green 1 } }
#declare InsideTexture    = texture { pigment { colour red 1 } }
#declare IntersectTexture = texture { pigment { colour blue 1 } }

#declare SpindleMode = frame_number;

// barely visible reference shape
torus {
  0.5,1
  texture { HintTexture }
  no_shadow
  clipped_by {
    #if (SpindleMode = 2)
      box { <-0.5,-sin(pi/3),0>, <0.5,sin(pi/3),0.5> inverse }
    #elseif (SpindleMode = 3)
      union{
        box { <-2,-2,-2>, <2,2,0> }
        box { <-0.5,-sin(pi/3),-0.5>, <0.5,sin(pi/3),0.5> }
      }
    #else
      box { <-2,-2,-2>, <2,2,0> }
    #end
  }
}

#declare TheTorus = torus {
  0.5,1
  #switch (SpindleMode)
  #case (1) difference   #break
  #case (2) intersection #break
  #case (3) merge        #break
  #case (4) union        #break
  #end
}


object { TheTorus
  texture { OutsideTexture }
  interior_texture { InsideTexture }
  clipped_by { box { <-2,-2,0>, <2,2,2> } }
}

plane { y, 0
  clipped_by { TheTorus }
  texture { IntersectTexture }
}
