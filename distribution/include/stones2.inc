// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: stones2.inc
//    Last updated: 2001.7.24
//    Description: Stone textures.

#ifndef(Stones2_Inc_Temp)
#declare Stones2_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including stones2.inc\n"
#end

/* This file was modified July 2001 to work as it should with POV-Ray 3.5,
   all filters being changed to transmits*/

/*

              Persistence of Vision Raytracer Version 3.1
       T_Stone25 through T_Stone35 created by Dan Farmer, 1995
       T_Stone36 through T_Stone43 created by Paul Novak, 1995
       T_Stone44 by Dan Farmer 1995

*/

#declare T_Stone25 =
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map {
            [0.02 color rgbt <0.30, 0.60, 0.45, 0.0> ]
            [0.06 color rgbt <0.35, 0.55, 0.40, 0.0> ]
            [0.10 color rgbt <0.25, 0.25, 0.20, 0.0> ]
            [1.00 color rgbt <0.75, 0.75, 0.60, 0.0> ]
        }
    }
}

texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map { 
            [0.02 color rgbt <0.30, 0.60, 0.45, 0.95> ]
            [0.06 color rgbt <0.35, 0.55, 0.40, 0.75> ]
            [0.10 color rgbt <0.25, 0.25, 0.20, 0.65> ]
            [1.00 color rgbt <0.75, 0.75, 0.60, 0.60> ]
        }
    }
}

texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.15
        rotate <30, 30, 30>
        color_map {
            [0.00 color rgbt <0.95, 0.95, 0.95, 0.30> ]
            [0.03 color rgbt <0.85, 0.85, 0.85, 0.60> ]
            [0.06 color rgbt <0.50, 0.50, 0.50, 0.90> ]
            [0.10 color rgbt <1.00, 1.00, 1.00, 1.00> ]
        }
    }
}

#declare T_Stone26 =
texture {
    pigment {
        granite
        rotate y*90
        color_map { 
            [0.02 color rgbt <0.25, 0.45, 0.60, 0.0> ]
            [0.06 color rgbt <0.20, 0.40, 0.55, 0.0> ]
            [0.10 color rgbt <0.15, 0.20, 0.25, 0.0> ]
            [1.00 color rgbt <0.40, 0.60, 0.75, 0.0> ]
        }
        frequency 6
        rotate <0, 10, 0>
        turbulence <0.05, 0.05, 0.05>
        octaves 6
        omega 0.7
        lambda 2
    }
}
texture {
    pigment {
        gradient x
        color_map {
            [0.00 color Clear ]
            [0.10 color rgbt <1.00, 1.00, 1.00, 0.75> ]
            [0.15 color rgbt <0.15, 0.25, 0.40, 0.50> ]
            [0.20 color rgbt <0.00, 0.05, 0.10, 0.25> ]
            [0.25 color rgbt <0.15, 0.25, 0.40, 0.25> ]
            [0.30 color rgbt <0.35, 0.55, 0.40, 0.50> ]
            [0.35 color rgbt <0.25, 0.25, 0.20, 0.75> ]
            [0.35 color Clear ]
        }
        frequency 3
        rotate <1, 10, 45>
        rotate x*60
        turbulence <0.30, 0.30, 0.30>
        octaves 6
        omega 0.7
        lambda 2.2
    }
}
texture {
    pigment {
        agate
        color_map {
            [0.00 color Clear ]
            [0.10 color rgbt <1.00, 1.00, 1.00, 0.75> ]
            [0.15 color rgbt <0.35, 0.55, 0.40, 0.50> ]
            [0.20 color rgbt <0.25, 0.25, 0.20, 0.25> ]
            [0.25 color rgbt <0.30, 0.60, 0.45, 0.25> ]
            [0.30 color rgbt <0.35, 0.55, 0.40, 0.50> ]
            [0.35 color rgbt <0.25, 0.25, 0.20, 0.75> ]
            [0.35 color Clear ]
        }
        frequency 3
        rotate <2, 10, 75>
        rotate x*120
        turbulence <0.05, 0.05, 0.05>
        octaves 6
        omega 0.7
        lambda 2
    }
}

#declare T_Stone27 =
texture {
    pigment {
        crackle
        turbulence 0.35
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.5
        frequency 3
        rotate <0, -45, 30>
        color_map { 
            [0.02 color rgbt <0.25, 0.45, 0.60, 0.0> ]
            [0.06 color rgbt <0.20, 0.40, 0.55, 0.0> ]
            [0.10 color rgbt <0.05, 0.10, 0.15, 0.0> ]
            [1.00 color rgbt <0.40, 0.60, 0.75, 0.0> ]
        }
    }
}

texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map { 
            [0.00 color Clear ]
            [0.20 color rgbt <1.00, 1.00, 1.00, 0.75> ]
            [0.25 color rgbt <0.25, 0.45, 0.40, 0.50> ]
            [0.30 color rgbt <0.00, 0.00, 0.00, 0.00> ]
            [0.35 color rgbt <0.15, 0.75, 0.60, 0.50> ]
            [0.40 color rgbt <0.35, 0.85, 0.40, 0.60> ]
            [0.45 color rgbt <0.35, 0.45, 0.20, 0.75> ]
            [0.65 color Clear ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.35
        lambda 2.5
        omega 0.75
        octaves 5
        scale 2
        frequency 2
        rotate <0, 90, 0>
        color_map { 
            [0.00 color Clear ]
            [0.20 color rgbt <1.00, 1.00, 1.00, 0.75> ]
            [0.25 color rgbt <0.35, 0.55, 0.40, 0.50> ]
            [0.30 color rgbt <0.00, 0.00, 0.00, 0.00> ]
            [0.45 color rgbt <0.30, 0.60, 0.45, 0.25> ]
            [0.50 color rgbt <0.35, 0.55, 0.40, 0.50> ]
            [0.65 color rgbt <0.25, 0.25, 0.20, 0.75> ]
            [0.65 color Clear ]
        }
    }
}


#declare T_Stone28 =
texture {
    pigment {
        agate
        agate_turb 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 5>
        color_map { 
            [0.02 color rgbt <0.65, 0.45, 0.25, 0.0> ]
            [0.06 color rgbt <0.55, 0.40, 0.20, 0.0> ]
            [0.10 color rgbt <0.15, 0.10, 0.05, 0.0> ]
            [1.00 color rgbt <0.75, 0.60, 0.40, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.75
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map { 
            [0.00 color Clear ]
            [0.25 color rgbt <0.40, 0.25, 0.15, 0.50> ]
            [0.30 color rgbt <0.00, 0.00, 0.00, 0.00> ]
            [0.35 color rgbt <0.40, 0.25, 0.15, 0.50> ]
            [0.40 color rgbt <0.45, 0.55, 0.35, 0.60> ]
            [0.65 color Clear ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map { 
            [0.00 color Clear ]
            [0.25 color rgbt <0.65, 0.25, 0.40, 0.50> ]
            [0.30 color rgbt <0.00, 0.00, 0.00, 0.00> ]
            [0.45 color rgbt <0.50, 0.60, 0.35, 0.25> ]
            [0.50 color rgbt <0.65, 0.25, 0.40, 0.50> ]
            [0.65 color Clear ]
        }
    }
}


#declare T_Stone29 =
texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map {
            [0.02 color rgb <0.65, 0.35, 0.25> ]
            [0.06 color rgb <0.55, 0.25, 0.10> ]
            [0.10 color rgb <0.45, 0.30, 0.25> ]
            [1.00 color rgb <0.65, 0.35, 0.25> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.6
        lambda 1.75
        omega 0.55
        octaves 4
        scale 0.45
        rotate <0, 5, 60>
        color_map { 
            [0.01 color rgbt <0.00, 0.00, 0.00, 0.00> ]
            [0.02 color rgbt <0.40, 0.25, 0.15, 0.20> ]
            [0.05 color rgbt <0.40, 0.35, 0.20, 0.40> ]
            [0.10 color rgbt <0.45, 0.55, 0.25, 0.60> ]
            [0.65 color Clear ]
        }
    }
}
texture {
    pigment {
        granite
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.75
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map {
            [0.15 color rgbt <0.40, 0.25, 0.15, 0.40> ]
            [0.20 color rgbt <0.20, 0.10, 0.00, 0.00> ]
            [0.25 color rgbt <0.20, 0.15, 0.15, 0.40> ]
            [0.30 color rgbt <0.40, 0.25, 0.15, 0.70> ]
            [0.65 color Clear ]
        }
    }
}

#declare T_Stone30 =
texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map { 
            [0.02 color rgbt <0.85, 0.85, 0.45, 0.0> ]
            [0.06 color rgbt <0.65, 0.65, 0.40, 0.0> ]
            [0.10 color rgbt <0.90, 0.90, 0.65, 0.0> ]
            [1.00 color rgbt <0.85, 0.85, 0.45, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map { 
            [0.00 color Clear ]
            [0.25 color rgbt <0.40, 0.40, 0.15, 0.50> ]
            [0.30 color rgbt <0.00, 0.00, 0.00, 0.00> ]
            [0.35 color rgbt <0.40, 0.40, 0.20, 0.50> ]
            [0.40 color rgbt <0.65, 0.65, 0.30, 0.60> ]
            [0.65 color Clear ]
        }
    }
}

texture {
    pigment {
        crackle
        turbulence 0.1
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.75
        frequency 1
        phase 0.25
        rotate <0, 45, 60>
        color_map { 
            [0.00 color Clear ]
            [0.15 color rgbt <0.40, 0.15, 0.45, 0.60> ]
            [0.18 color rgbt <0.20, 0.10, 0.00, 0.00> ]
            [0.25 color rgbt <0.20, 0.15, 0.45, 0.60> ]
            [0.30 color rgbt <0.40, 0.15, 0.45, 0.80> ]
            [0.65 color Clear ]
        }
    }
}

#declare T_Stone31 =
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map { 
            [0.00 color Gray90 ]
            [0.02 color rgb <0.95, 0.90, 0.85> ]
            [0.03 color rgb <0.75, 0.85, 0.80> ]
            [0.04 color rgb <0.45, 0.40, 0.50> ]
            [0.10 color Gray15 ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map { 
            [0.00 color Clear ]
            [0.02 color rgb <0.95, 0.90, 0.85> ]
            [0.03 color rgb <0.75, 0.85, 0.80> ]
            [0.04 color rgb <0.45, 0.40, 0.50> ]
            [0.10 color Clear ]
        }
    }
}

#declare T_Stone32 =
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map { 
            [0, 1 color rgb <0.9, 0.75, 0.75>
                  color rgb <0.6, 0.6, 0.6> ]
        }
    }
}

texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map { 
            [0.0, 0.9 color rgbt <0.52, 0.39, 0.39, 1.0> 
                      color rgbt <0.52, 0.39, 0.39, 0.5>]
            [0.9, 1.0 color rgbt <0.42, 0.14, 0.55, 0.0>
                      color rgbt <0.42, 0.14, 0.55, 0.0>]
        }
    }
}

texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.15
        rotate <30, 30, 30>
        color_map {
            [0.00 color rgbt <0.95, 0.95, 0.95, 0.30> ]
            [0.03 color rgbt <0.85, 0.85, 0.85, 0.60> ]
            [0.06 color rgbt <0.50, 0.50, 0.50, 0.90> ]
            [0.10 color rgbt <1.00, 1.00, 1.00, 1.00> ]
        }
    }
}

#declare T_Stone33 =
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        rotate <0, 5, 60>
        color_map {
            [0.0, 0.1 color rgb <0.0, 0.0, 0.0>
                      color rgb <0.9, 0.7, 0.6>]
            [0.1, 0.3 color rgb <0.9, 0.7, 0.6>
                      color rgb <0.9, 0.7, 0.4>]
            [0.3, 1.0 color rgb <0.9, 0.7, 0.4>
                      color rgb <0.7, 0.4, 0.2>]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.2
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.65
        rotate <0, 45, 60>
        color_map {
            [0.0, 0.2 color rgbt <0.7, 0.4, 0.2, 0.20>
                      color rgbt <0.9, 0.7, 0.6, 0.30>]
            [0.2, 0.3 color rgbt <0.9, 0.7, 0.6, 0.50>
                      color rgbt <0.9, 0.7, 0.4, 0.70>]
            [0.3, 1.0 color rgbt <0.9, 0.7, 0.4, 0.80>
                      color rgbt <0.7, 0.4, 0.2, 0.80>]
        }
    }
}

#declare T_Stone34 =
texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map {
            [0.02 color rgbt <0.85, 0.85, 0.85, 0.0> ]
            [0.10 color rgbt <1.00, 1.00, 1.00, 0.0> ]
            [1.00 color rgbt <0.85, 0.85, 0.85, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map {
            [0.00 color Clear ]
            [0.15 color rgbt <0.40, 0.30, 0.30, 0.50> ]
            [0.30 color rgbt <0.15, 0.08, 0.02, 0.00> ]
            [0.35 color rgbt <0.60, 0.40, 0.35, 0.50> ]
            [0.45 color rgbt <0.40, 0.35, 0.30, 0.60> ]
            [0.65 color Clear ]
        }
    }
}

// White marble
#declare T_Stone35 =
texture {
    pigment {
        gradient x
        turbulence 0.75
        frequency 6
        scale 0.5
        rotate <0, -45, 30>
        color_map {
            [0.02 color rgbt <0.85, 0.85, 0.85, 0.0> ]
            [0.10 color rgbt <1.00, 1.00, 1.00, 0.0> ]
            [1.00 color rgbt <0.85, 0.85, 0.85, 0.0> ]
        }
    }
}
texture {
    pigment {
        crackle
        turbulence 0.3
        lambda 2.5
        omega 0.75
        octaves 5
        scale 0.45
        frequency 2
        phase 0.25
        rotate <0, 5, 60>
        color_map {
            [0.00 color Clear ]
            [0.15 color rgbt <0.40, 0.30, 0.30, 0.50> ]
            [0.30 color rgbt <0.15, 0.08, 0.02, 0.00> ]
            [0.35 color rgbt <0.60, 0.40, 0.35, 0.50> ]
            [0.45 color rgbt <0.40, 0.35, 0.30, 0.60> ]
            [0.65 color Clear ]
        }
    }
}

// Creamy coffee w/greenish-grey veins & faint avacado swirls
#declare T_Stone36 =
texture  {
    pigment {
        granite
        turbulence 0.815
        colour_map {
            [0.10 colour rgbt <0.43529, 0.49804, 0.32941,0.0>]
            [0.20 colour rgbt <0.19608, 0.27843, 0.19608,0.0>]
            [0.35 colour rgbt <0.43529, 0.49804, 0.32941,0.0>]
            [0.55 colour rgbt <0.45490, 0.44706, 0.20784,0.0>]
            [0.75 colour rgbt <0.43529, 0.49804, 0.32941,0.0>]
            [0.90 colour rgbt <0.19608, 0.22745, 0.00000,0.0>]
            [1.00 colour rgbt <0.43529, 0.49804, 0.32941,0.0>]
        }
    scale <0.85, 1.5, 0.5>
    rotate <10, 0, 40>
    }
    finish { brilliance 1.825 }
}
texture  {
    pigment {
        granite
        turbulence 0.825
        colour_map {
            [0.18 colour rgbt <0.20784, 0.13333, 0.00000, 0.427>]
            [0.35 colour rgbt <0.20784, 0.13333, 0.00000, 0.735>]
            [0.45 colour rgbt <0.27059, 0.11373, 0.00000, 0.625>]
            [0.90 colour rgbt <0.27059, 0.11373, 0.00000, 0.875>]
            [1.00 colour rgbt <0.20784, 0.13333, 0.00000, 0.890>]
        }
     scale <0.65, 1.18, 0.34>
     rotate <5, 10, -55>
    }
    finish { ambient 0.275 diffuse 0.775 crand 0.01975 }
}

// Olive greens w/lighter swirls & hints of salmon
#declare T_Stone37 =
texture  {
    pigment {
        granite
        turbulence 0.9
        colour_map  {
            [0.18 colour rgbt <0.29020, 0.24706, 0.00000,0>]
            [0.35 colour rgbt <0.29020, 0.24706, 0.00000,0>]
            [0.45 colour rgbt <0.16471, 0.15294, 0.00000,0>]
            [0.90 colour rgbt <0.16471, 0.15294, 0.00000,0>]
            [1.00 colour rgbt <0.32941, 0.25882, 0.00000,0>]
        }
        scale <0.825, 1.7, 0.25>
        rotate <10,5,40>
    }
    finish { brilliance 2.5 }
}
texture  {
    pigment {
        granite
        turbulence 0.85
        colour_map   {
            [0.05 colour rgbt <0.86275, 0.65490, 0.40392, 0.6250>]
            [0.35 colour rgbt <0.48627, 0.16471, 0.00000, 0.8250>]
            [0.60 colour rgbt <0.86275, 0.65490, 0.40392, 0.7150>]
            [0.85 colour rgbt <0.48627, 0.16471, 0.00000, 0.5450>]
            [1.00 colour rgbt <0.52941, 0.34118, 0.17647, 0.8975>]
        }
        scale <0.85, 1.6, 0.45>
        rotate <0,5,-50>
    }
    finish { ambient 0.275 diffuse 0.6925 crand 0.01975}
}

// Deep rich coffee w/darker veins & lots of creamy swirl
#declare T_Stone38 =
texture  {
    pigment {
        granite
        turbulence 1.25
        colour_map  {
            [0.15 colour rgbt<0.72549, 0.73725, 0.54118, 0.0>]
            [0.25 colour rgbt<0.22745, 0.14510, 0.00000, 0.0>]
            [0.45 colour rgbt<0.72549, 0.73725, 0.54118, 0.0>]
            [0.60 colour rgbt<0.22745, 0.14510, 0.00000, 0.0>]
            [0.80 colour rgbt<0.22745, 0.14510, 0.00000, 0.0>]
            [1.00 colour rgbt<0.60392, 0.61176, 0.46667, 0.0>]
        }
        scale <0.95, 1.7, 0.39>
        rotate <10, 5, 40>
    }
    finish { brilliance 2.125 }
}
texture  {
    pigment {
        granite
        turbulence 0.825
        colour_map {
            [0.35 colour rgbt <0.56078, 0.54902, 0.42353,0.65>]
            [0.65 colour rgbt <0.10196, 0.05882, 0.00000,0.85>]
            [1.00 colour rgbt <0.56078, 0.54902, 0.42353,0.90>]
        }
        scale <0.76, 1.4, 0.4>
        rotate <0, 10, -55>
    }
    finish { ambient 0.4 diffuse 0.575 crand 0.021 }
}
// Light mauve w/large plum swirls
#declare T_Stone39 =
texture  {
    pigment {
        granite
        turbulence 0.825
        colour_map  {
             [0.12 colour rgbt <0.78039, 0.54902, 0.46667, 0.0>]
             [0.35 colour rgbt <0.78039, 0.54902, 0.46667, 0.0>]
             [0.65 colour rgbt <0.49804, 0.30980, 0.30196, 0.0>]
             [0.90 colour rgbt <0.49804, 0.30980, 0.30196, 0.0>]
             [1.00 colour rgbt <0.35294, 0.09020, 0.00000, 0.0>]
        }
    }
    scale <0.78,1.45, 0.4>
    rotate <5,10,-55>
    finish { brilliance 1.9275 ambient .3 diffuse .575 crand .0215}
}

// Creamy aqua w/green hi-lites & subtle hints of grey
#declare T_Stone40 =
texture  {
    pigment {
        agate
        agate_turb 1.1
        colour_map  {
            [0.10 colour rgbt <0.56078, 0.67451, 0.62353, 0.0>]
            [0.35 colour rgbt <0.27843, 0.41569, 0.30196, 0.0>]
            [0.55 colour rgbt <0.56078, 0.67451, 0.62353, 0.0>]
            [0.85 colour rgbt <0.56078, 0.67451, 0.62353, 0.0>]
            [0.95 colour rgbt <0.27843, 0.41569, 0.30196, 0.0>]
            [1.00 colour rgbt <0.54118, 0.58039, 0.56078, 0.0>]
        }
        scale <0.97, 0.97, 0.5>
        rotate <0, 5, 40>
    }
    finish {brilliance 1.25}
}
texture  {
    pigment {
        granite
        turbulence 1.25
        colour_map  {
            [0.15 colour rgbt <0.46667, 0.59216, 0.54118, 0.005>]
            [0.65 colour rgbt <0.35294, 0.67451, 0.54902, 0.750>]
            [0.80 colour rgbt <0.46667, 0.59216, 0.54118, 0.660>]
            [1.00 colour rgbt <0.35294, 0.67451, 0.54902, 0.879>]
        }
        scale <0.85, 1.5, 0.5>
        rotate <10, 5, -75>
    }
    finish { ambient 0.175 diffuse 0.6975 crand 0.025 }
}

// Dark powder blue w/steel blue & grey swirls
#declare T_Stone41 =
texture  {
    pigment {
        marble
        turbulence 1.5
        omega 0.6235
        lambda 2.25
        colour_map  {
            [0.15 colour rgbt <0.03922, 0.20784, 0.52941, 0.0>]
            [0.45 colour rgbt <0.03922, 0.20784, 0.52941, 0.0>]
            [0.55 colour rgbt <0.22745, 0.23922, 0.42353, 0.0>]
            [0.75 colour rgbt <0.20784, 0.30980, 0.54118, 0.0>]
            [1.00 colour rgbt <0.20784, 0.30980, 0.54118, 0.0>]
        }
        rotate <0, 0, 85>
        scale <0.75, 1.33, 0.35>
        rotate <10, 5, 45>
    }
    finish { brilliance 1.825 }
}
texture  {
    pigment {
        granite
        turbulence  1.15
        colour_map  {
            [0.25 colour rgbt <0.30196, 0.35294, 0.48627, 0.600>]
            [0.45 colour rgbt <0.30196, 0.35294, 0.48627, 0.475>]
            [0.85 colour rgbt <0.17647, 0.24706, 0.39216, 0.715>]
            [1.00 colour rgbt <0.17647, 0.24706, 0.39216, 0.750>]
        }
        rotate <0,0,45>
       scale <0.78, 1.0, 0.4>
       rotate <5, 10, -55>
    }
    finish { ambient 0.225 diffuse 0.75 crand 0.01975}
}

//Brick red w/yellow-green swirls
#declare T_Stone42 =
texture  {
    pigment {
        onion
        turbulence  2.1
        omega 0.575
        colour_map  {
           [0.15 colour rgbt<0.29020, 0.08235, 0.00000, 0.0>]
           [0.35 colour rgbt<0.34118, 0.29020, 0.09020, 0.0>]
           [0.65 colour rgbt<0.29020, 0.08235, 0.00000, 0.0>]
           [0.85 colour rgbt<0.34118, 0.29020, 0.09020, 0.0>]
           [1.00 colour rgbt<0.29020, 0.08235, 0.00000, 0.0>]
        }
        scale <0.9, 1.6, .45>
        rotate <10, 5, 45>
    }
    finish { brilliance 1.825 }
}
texture  {
    pigment {
        granite
        turbulence 0.925
        colour_map  {
            [0.25 colour rgbt<0.42353, 0.37255, 0.05882, 0.45>]
            [0.50 colour rgbt<0.18431, 0.01569, 0.00000, 0.55>]
            [0.72 colour rgbt<0.42353, 0.37255, 0.05882, 0.55>]
            [1.00 colour rgbt<0.18431, 0.01569, 0.00000, 0.65>]
        }
        rotate <5, 15, 90>
        scale <0.78, 1.05, 0.4>
        rotate <5, 10, -55>
    }
    finish { ambient 0.225 diffuse 0.75 crand 0.01975 }
}

// Rusty red w/cream swirls and duck overtones
#declare T_Stone43 =
texture  {
    pigment {
        granite
        turbulence 0.815
        colour_map {
            [0.15 colour rgbt<0.86275, 0.75686, 0.61176, 0.0>]
            [0.45 colour rgbt<0.49804, 0.21569, 0.03922, 0.0>]
            [0.65 colour rgbt<0.84314, 0.72157, 0.63529, 0.0>]
            [0.85 colour rgbt<0.46667, 0.23922, 0.05098, 0.0>]
            [1.00 colour rgbt<0.87059, 0.73725, 0.62745, 0.0>]
        }
        scale <0.87, 1.235, 0.47>
        rotate <15, 25, 45>
    }
    finish {brilliance 2.585}
}
texture  {
    pigment {
        agate
        agate_turb 1.275
        colour_map {
            [0.35 colour rgbt<0.42353, 0.12157, 0.01176, 0.650>]
            [0.65 colour rgbt<0.50196, 0.35294, 0.21176, 0.750>]
            [1.00 colour rgbt<0.41569, 0.20784, 0.02745, 0.550>]
        }
        scale <0.69,1.27, 0.35>
        rotate <10, 5, -60>
    }
    finish { ambient 0.225 diffuse 0.6975 crand 0.02175 }
}

// This one is quite different from the rest.
// Its a dark, dull, bumpy rock texture.
#declare T_Stone44 =
texture {
    pigment {
        granite
        color_map {
            [0.0 rgb 0.3 ]
            [1.0 rgb 0.7 ]
        }
        scale 0.075
    }
    normal  { granite 0.75 scale 0.075 }
}
texture {
    pigment {
        wrinkles
        turbulence 0.3
        scale 0.3
        color_map {
            [0.0 rgbt< 0.50, 0.25, 0.10, 0.85>]
            [1.0 rgbt< 0.65, 0.40, 0.00, 0.65>]
        }
    }
}

#version Stones2_Inc_Temp;
#end
