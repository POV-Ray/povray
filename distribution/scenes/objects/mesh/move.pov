#version 3.7;
global_settings{assumed_gamma 1.0}
#include "colors.inc"
#default { finish { ambient 0.5 specular 0.5 } }
camera { location <6,6,12> direction  -z right x*image_width/image_height up y look_at 0 angle 35 }
light_source { <-30,100,50>, 1 }

#declare tt=texture { pigment { spiral2 6 scale 2 translate -y*0.95 pigment_map { 
[0 Black]
[1 Gray20]
} } }
 
#declare msize=3.0;
move { original cristal { accuracy 150 original sphere { 0,msize } }
modulation { tt }
move < 0.6,+0.4,0, 0,1,0, 0,-0.2,0.1, 0,0,0 >
pigment { Yellow } 
}

