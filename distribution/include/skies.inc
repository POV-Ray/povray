// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: skies.inc
//    Last updated: 2001.7.24
//    Description: Sky textures, sky_spheres, and sky objects.

#ifndef(Skies_Inc_Temp)
#declare Skies_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including skies.inc\n"
#end

/*

              Persistence of Vision Raytracer Version 3.1

  Contents:
    Pigments:    (building blocks for the stuff below)
    P_Cloud1     pigment layer, contains clear regions.  Use with background.
    P_Cloud2     pigment layer, contains clear regions.  Use with background.
    P_Cloud3     pigment layer, contains clear regions.  Use with background.

    SkySpheres:
    S_Cloud1     sky_sphere, uses P_Cloud2.
    S_Cloud2     sky_sphere  uses P_Cloud2.
    S_Cloud3     sky_sphere  uses P_Cloud3.
    S_Cloud4     sky_sphere  uses P_Cloud3.
    S_Cloud5     sky_sphere.  Opaque.

    Textures:
    T_Cloud1     2-layer texture using P_Cloud1 pigment, contains clear regions.
    T_Cloud2     1-layer texture, contains clear regions.
    T_Cloud3     2-layer texture, contains clear regions.

    Objects:
    O_Cloud1     sphere, radius 10000 with T_Cloud1 texture.
    O_Cloud2     union of 2 planes, with T_Cloud2 and T_Cloud3.

*/

#declare P_Cloud1 =
pigment {
    bozo
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map {
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85
                    color red 0.75 green 0.75 blue 0.75]
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75
                    color Clear]
        [0.5, 1.001 color Clear
                    color Clear]
    }
    scale <6, 1, 6>
}

#declare P_Cloud2 =
pigment {
    wrinkles
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map {
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85
                    color red 0.75 green 0.75 blue 0.75]
        [0.1, 0.5   color red 0.75 green 0.75 blue 0.75
                    color rgb <0.258, 0.258, 0.435>  ]
        [0.5, 1.001 color rgb <0.258, 0.258, 0.435>
                    color rgb <0.258, 0.258, 0.435> ]
    }
    scale <6, 1, 6>
}
#declare P_Cloud3 =
pigment {
    bozo
    color_map {
        [0.0, 0.1   color red 0.85 green 0.85 blue 0.85
                    color red 0.55 green 0.60 blue 0.65]
        [0.1, 0.5   color red 0.55 green 0.60 blue 0.65
                    color rgb <0.184, 0.184, 0.309> ]
        [0.5, 1.001 color rgb <0.184, 0.184, 0.309>
                    color rgb <0.1, 0.1, 0.2>]
    }
    turbulence 0.65
    octaves 6
    omega 0.707
    lambda 2
    scale <6, 4, 6>
}

#declare P_Cloud4 =
pigment {
    wrinkles
    turbulence 0.1
    lambda 2.2
    omega 0.707
    color_map {
        [0.20 SkyBlue * 0.85 ]
        [0.50 White ]
        [1.00 Gray70 ]
    }
    scale <0.5, 0.15, 1>
}

#declare S_Cloud1 =
sky_sphere {
    pigment {
        gradient y
        pigment_map {
            [0.01 rgb <0.847, 0.749, 0.847> ] // horizon
            [0.25 P_Cloud2 scale 0.25 rotate z*5]
            [0.60 P_Cloud3 scale <0.25, 0.15, 0.25> rotate z*10]
        }
    }
}

#declare S_Cloud2 =
sky_sphere {
    pigment {
        gradient y
        pigment_map {
            [0.00 rgb <0.847, 0.749, 0.847> ] // horizon
            [0.10 SkyBlue ]                   // horizon
            [0.20 P_Cloud4 ]
        }
    }
}

#declare S_Cloud3 =
sky_sphere {
    pigment {
        gradient y
        pigment_map {
            [0.10 rgb <0.258, 0.258, 0.435> ]
            [0.25 P_Cloud2 scale 0.15 ]
        }
    }
}

#declare S_Cloud4 =
sky_sphere {
    pigment {
        gradient y
        pigment_map {
            [0.00 rgb <0.184, 0.184, 0.309> ]
            [0.15 P_Cloud3 scale <0.05, 0.40, 0.05> rotate z*30 ]
            [0.45 P_Cloud3 scale <0.25, 0.15, 0.50> ]
        }
    }
}


#declare S_Cloud5 =
sky_sphere {
    pigment { rgb <0.258, 0.258, 0.435> }
    pigment {
        bozo
        turbulence 1.5
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.5 color rgbt<.75, .75, .75, 0>
                      color rgbt<.9, .9, .9, .9> ]
            [0.5, 0.7 color rgbt<.9, .9, .9, .9>
                      color rgbt<1, 1, 1, 1> ]
            [0.7, 1.0 color rgbt<1, 1, 1, 1>
                      color rgbt<1, 1, 1, 1> ]
        }
    scale <1, 0.3, 10>
    }
    pigment {
        bozo
        turbulence 0.6
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.4 color rgbt<.75, .75, .75, 0>
                      color rgbt<.9, .9, .9, .9> ]
            [0.4, 0.7 color rgbt<.9, .9, .9, .9>
                      color rgbt<1, 1, 1, 1> ]
            [0.7, 1.0 color rgbt<1, 1, 1, 1>
                      color rgbt<1, 1, 1, 1> ]
        }
    scale <1, 0.3, 10>
    }
    pigment {
        bozo
        turbulence 0.8
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.4 color rgbt<.75, .75, .75, 0>
                      color rgbt<.9, .9, .9, .9> ]
            [0.4, 0.6 color rgbt<.9, .9, .9, .9>
                      color rgbt<1, 1, 1, 1> ]
            [0.6, 1.0 color rgbt<1, 1, 1, 1>
                      color rgbt<1, 1, 1, 1> ]
        }
    scale <1, 0.3, 10>
    }
}


// ***********************************
//  Bill Pulver's FBM Clouds
// A three-layer cloud texture.
// ***********************************
#declare T_Cloud1 =
texture {                              // The upper part of the clouds
    pigment {
        P_Cloud1
    }
    finish {
        ambient 1.0
        diffuse 0
    }
}
texture {                              // The darker underside of the clouds
    pigment {
        P_Cloud1
        translate -0.15*y
    }
    finish {
        ambient 0.6
        diffuse 0
    }
}

// T_Cloud1 mapped onto a sphere
#declare O_Cloud1 =
sphere { <0,0,0>, 10000
    texture {
        T_Cloud1
        scale 1000
    }
}

// ***********************************
//  Darin Dugger's "Kite" clouds
// ***********************************
#declare T_Cloud2 =
texture {
    pigment {
        bozo
        turbulence 1.5
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.5 color rgbt<.75, .75, .75, 0>
                      color rgbt<.9, .9, .9, .9> ]
            [0.5, 0.7 color rgbt<.9, .9, .9, .9>
                      color rgbt<1, 1, 1, 1> ]
            [0.7, 1.0 color rgbt<1, 1, 1, 1>
                      color rgbt<1, 1, 1, 1> ]
        }
    }

    finish {
        ambient 0.9
        diffuse 0.1
    }
}

#declare T_Cloud3 =
texture {
    pigment {
        bozo
        turbulence 0.8 //0.6
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.4 color rgbt<.75, .75, .75, 0>
                      color rgbt<.9, .9, .9, .9> ]
            [0.4, 0.7 color rgbt<.9, .9, .9, .9>
                      color rgbt<1, 1, 1, 1> ]
            [0.7, 1.0 color rgbt<1, 1, 1, 1>
                      color rgbt<1, 1, 1, 1> ]
        }
    }
    finish {
        ambient 1.0
        diffuse 0.0
    }
}
texture {
    pigment {
        bozo
        turbulence 0.8 //0.6
        octaves 10
        omega .5
        lambda 2.5
        color_map {
            [0.0, 0.4 color rgbt<.75, .75, .75, 0>
                      color rgbt<.9, .9, .9, .9> ]
            [0.4, 0.6 color rgbt<.9, .9, .9, .9>
                      color rgbt<1, 1, 1, 1> ]
            [0.6, 1.0 color rgbt<1, 1, 1, 1>
                      color rgbt<1, 1, 1, 1> ]
        }
    }
    finish {
        ambient 0.95
        diffuse 0.0
    }
scale .9
translate y*-0.15
}





// Darin Dugger's DD_Cloud_Sky texture mapped onto a pair of planes
// NOTE: Lowest plane is at y=500
#declare O_Cloud2 =
union {
    plane { y, 500
        texture {
            T_Cloud3
            scale 600
        }
    }
    plane { y, 3000
        texture {
            T_Cloud2
            scale <900,1,6000>
            translate x*3000
            rotate -30*y
        }
    }
}


#version Skies_Inc_Temp;
#end
