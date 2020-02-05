// Persistence Of Vision raytracer version 3.1 sample file.
// Simpler but faster version of Joel NewKirk's GlassBoing animation

#include "colors.inc"
global_settings { assumed_gamma 2.2 }

#declare xfactor = clock*4;
#declare spherey = 5+abs(50*sin((clock)*2*pi));
#switch (xfactor)
    #range(0,1)  // x value cycles from 0 to 1
    #debug "Range A"
        #declare spherex = 50*xfactor
        #break
    #range(1,3)  // x value cycles from 1 to -1
    #debug "Range B"
        #declare spherex = 50*(2-xfactor)
        #break
    #range(3,4)  // x value cycles from -1 back to 0
    #debug "Range C"
        #declare spherex = -50*(4-xfactor)
        #break
#end

camera {
    location  <0, 10.5,-100>
    direction <0,  0,   1>
    up        <0,  1,   0>
    right   <4/3,  0,   0>
    look_at <0, 11, 0>
}

light_source {<30, 120, 0> colour White
    fade_distance 90
    fade_power 1
}

sky_sphere {
    pigment {
        gradient y
        color_map {
            [0.0 Gray50 ]
            [1.0 Gray15 ]
        }
    }
}

#declare Brick =
texture {
    pigment { Gray75 }
    finish {
        ambient 0.0
        diffuse 0.8
    }
    scale 6
}

plane { x, -60 texture { Brick rotate y*90 }}               // left wall
plane { x, 60 texture { Brick rotate y* 90}}                // right wall
plane { z, 40 hollow on texture { Brick }}                            // back wall
plane { y, 0 texture { Brick } translate -y*5 }             // floor

sphere { <spherex,spherey, 0>, 10
    texture {
        pigment { White }
        finish { specular 1 roughness 0.001 }
    }
}

