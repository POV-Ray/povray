// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Pigment_pattern pattern example pigment_pattern.pov.
//
// Demonstrates a possible use of the pigment_pattern pattern.
// First, we'll define a reasonably complex pigment, made of wrinkles
// and leopard pigments mapped within a bozo pattern in another pigment.
//
// -w800 -h600 +a0.3

#version 3.71;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

//some usual scene elements
camera {
    location  <10,20,10>
    right     x*image_width/image_height
    look_at   <0,2,4>
    angle     50
}

#declare PlaneColor = srgb <0.18824,0.4,0.29412>;
plane { y,0 pigment { PlaneColor } }

#declare Wheat = srgb <0.85098,0.85098,0.75294>;
#declare White = srgb <1,1,1>;
light_source {<20,30,40> White*1.4}
light_source {<-20,30,-40> Wheat*.5 shadowless}

//the two basic pigments
#declare Yellow    = srgb <1,1,0>;
#declare SteelBlue = srgb <0.11765,0.41961,0.56078>;
#declare Pig1 = pigment {
    leopard sine_wave
    color_map {
        blend_mode 2 blend_gamma 2.5
        [0 SteelBlue]
        [1 Yellow]
    }
    scale 0.05
}
#declare Orange = srgb <1,0.5,0>;
#declare Pig2 = pigment {
    wrinkles
    color_map {
        blend_mode 2 blend_gamma 2.5
        [0.0 Orange]
        [0.5 White]
        [1.0 White]
    }
}

//the complex pigment of two pigments
#declare Pig3 = pigment {
    bozo triangle_wave
    pigment_map {
        blend_mode 2 blend_gamma 2.5
        [0 Pig1]
        [1 Pig2]
    }
    scale 1.5
}

//The sphere inside the torus shows the pigment on the sphere.
sphere {
    <0,4,0>,4
    texture { pigment { Pig3 } }
}

//The second, gray, sphere shows how the pigment becomes
//a new pattern with values from 0 to 1 - why it turns grey.
sphere {
    <0,4,0>,4
    texture {
        pigment {
            pigment_pattern { Pig3 }
        }
    }
    translate z*12
}

//The torus shows the new pattern with an orange color_map,
//and an added normal using the same pattern to create visible craters
//on the surface, following the complex pattern.
#declare OrangeRed = srgb <1,0.23922,0>;
torus {
    7 2
    texture {
        pigment {
            pigment_pattern { Pig3 }
            color_map {
                [0 OrangeRed*0.2]
                [1 OrangeRed*1.5]
            }
        }
        normal { pigment_pattern { Pig3 } 3.4 }
        finish { phong .8 phong_size 10 }
    }
    translate y*2
}

