// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Crackle pattern example crackle_solid.pov.
//
// -w800 -h600 +a0.3

#version 3.71;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

camera {
    location <70,100,60>
    right    x*image_width/image_height
    angle    40
    look_at  <0,15,0>
}

#declare White = srgb <1,1,1>;
light_source {<40,350,300> White*1.5 spotlight point_at 0 radius 9 falloff 11}

//--textures definitions--------------------------------
#declare Black  = srgb <0,0,0>;
#declare Gray60 = srgb <0.6,0.6,0.6>;
#declare Gray80 = srgb <0.8,0.8,0.8>;
#declare Gray90 = srgb <0.9,0.9,0.9>;
#declare Red    = srgb <1,0,0>;
#declare Tan    = srgb <0.86646,0.58571,0.44532>;
#declare Wheat  = srgb <0.85494,0.85494,0.75836>;
#declare Stone1 = texture {
    pigment {
        crackle solid
        color_map {
            blend_mode 2 blend_gamma 2.5
            [0  Black]
            [.2 Wheat*.2]
            [.4 Black]
            [.6 Wheat*.2]
            [.8 Black]
            [1  Wheat*.2]
        }
    }
    finish {
        phong .7
        reflection {.4}
    }
    normal {
        bumps .1
        scale .2
    }
    scale .1
}

#declare Stone2 = texture {
    pigment {
        crackle solid
        color_map {
            blend_mode 2 blend_gamma 2.5
            [0  Red]
            [.2 Wheat]
            [.4 Tan]
            [.6 Wheat]
            [.8 Red*.2]
            [1  Wheat]
        }
    }
    scale .05
}

#declare Zinc1 = texture {
    pigment {Gray80}
    finish {phong .7 reflection {.3}}
    normal {bumps .002 scale <.1,.1,10>}
}

#declare Zinc2 = texture {
    pigment {Gray90}
    finish {phong .6 reflection {.05}}
    normal {bumps .002 scale <.1,.1,10> rotate y*90}
}

#declare Zinc3 = texture {
    pigment {Gray60}
    finish {phong .5 reflection {.3}}
    normal {bumps .002 scale <.1,.1,10> rotate y*180}
}

#declare Zinc = texture {
    crackle solid
    texture_map {
        [0.0 Zinc1]
        [0.5 Zinc2]
        [1.0 Zinc3]
    }
}

//--floor--------------------------------
plane {
    y,0
    texture {
        checker
        texture {Stone1}
        texture {Stone2}
        scale 20
        translate y*10
    }
}

//--Wood Texture (A 3.7 version of T_Wood21)
#declare ColorWood11A_00 = srgb <0.80392,0.67451,0.239220>;
#declare ColorWood11A_01 = srgb <0.60392,0.33726,0.011765>;
#declare ColorWood11A_02 = srgb <0.80392,0.67451,0.239220>;
#declare ColorWood11A_03 = srgb <0.53333,0.28235,0.003922>;
#declare ColorWood11A_04 = srgb <0.80392,0.67451,0.239220>;
#declare M_Wood11A = color_map {
    blend_mode 2 blend_gamma 2.5
    [0.000, 0.222 color ColorWood11A_00
                  color ColorWood11A_00]
    [0.222, 0.342 color ColorWood11A_00
                  color ColorWood11A_01]
    [0.342, 0.393 color ColorWood11A_01
                  color ColorWood11A_02]
    [0.393, 0.709 color ColorWood11A_02
                  color ColorWood11A_02]
    [0.709, 0.821 color ColorWood11A_02
                  color ColorWood11A_03]
    [0.821, 1.000 color ColorWood11A_03
                  color ColorWood11A_04]
}
#declare ColorWood11B_Clr = srgbt <1.00,1.00,1.00,1.00>;
#declare ColorWood11B_00  = srgbt <0.70588,0.41176,0.086275,0.60>;
#declare ColorWood11B_01  = srgbt <0.70588,0.46275,0.086275,0.60>;
#declare M_Wood11B = color_map {
    blend_mode 2 blend_gamma 2.5
    [0.000, 0.120 color ColorWood11B_Clr
                  color ColorWood11B_00]
    [0.120, 0.231 color ColorWood11B_00
                  color ColorWood11B_01]
    [0.231, 0.496 color ColorWood11B_01
                  color ColorWood11B_Clr]
    [0.496, 0.701 color ColorWood11B_Clr
                  color ColorWood11B_Clr]
    [0.701, 0.829 color ColorWood11B_Clr
                  color ColorWood11B_01]
    [0.829, 1.000 color ColorWood11B_01
                  color ColorWood11B_Clr]
}
#declare P_WoodGrain1A = pigment {
    wood
    scale <0.05,.05,1>
    warp { turbulence 0.04 octaves 3 }
}
#declare P_WoodGrain1B = pigment {
    wood
    scale <0.15,.5,1>
    warp { turbulence <0.1,0.5,1> octaves 5 lambda 3.25 }
    rotate <5,10,5>
    translate -x*2
}
#declare T_Wood21 =
    texture { pigment{ P_WoodGrain1A color_map { M_Wood11A }}}
    texture { pigment{ P_WoodGrain1B color_map { M_Wood11B }}}

//--bucket--------------------------------
union {
    cylinder {<0,0,0>,<0,2,0>,15}
    difference {
        cone {<0,2,0>,15,<0,40,0>,20}
        cone {<0,2,0>,14.9,<0,41,0>,19.9}
    }
    torus {20 .4 translate y*40}
    union {
        torus {22 .4 clipped_by {plane {x,0}}}
        cylinder {<0,0,18>,<0,0,22>,.4}
        sphere {<0,0,22>,.4}
        cylinder {<0,0,-18>,<0,0,-22>,.4}
        sphere {<0,0,-22>,.4}
        cylinder {
            <-21.5,0,-5>,<-21.5,0,5>,1.5
            texture {T_Wood21 scale 20}
        }
        rotate z*150
        translate y*35
    }
    texture {Zinc}
    translate z*15
}
