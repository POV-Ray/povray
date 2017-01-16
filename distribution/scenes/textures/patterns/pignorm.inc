// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// This include file used by pigment/normal pattern example scenes.

camera {
  location <0,1,-14>
  right    x*image_width/image_height
  angle    25
}

#declare Black = srgb <0,0,0>;
#declare White = srgb <1,1,1>;

plane {
    y, -1.01
    pigment { checker color White, color Black }
}

plane {
    z, 3.01
    hollow on
    pigment { White }
}

light_source { <300, 200, -700> color White }

sphere {
    0,1
    texture { T1 }
    translate <-2, 2, 0>
}

sphere {
    0,1
    texture { T2 }
    translate -2*x
}

cylinder {
    -z,z,1
    scale 0.75
    texture { T1 }
    rotate <-30,30,0>
    translate 2*y
}

cylinder {
    -z,z,1
    scale 0.75
    texture { T2 }
    rotate <-30,30,0>
}

box {
    <-1,-1,-1>,<1,1,1>
    scale 0.75
    texture { T1 }
    rotate <-30,30,0>
    translate <2,2,0>
}

box {
    <-1,-1,-1>,<1,1,1>
    scale 0.75
    texture { T2 }
    rotate <-30,30,0>
    translate 2*x
}

