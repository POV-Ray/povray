// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: textures.inc
//    Last updated: 2001.7.25
//    Description:

#ifndef(Textures_Inc_Temp)
#declare Textures_Inc_Temp = version;
#version 3.5;

/*                     Standard textures include file

    Contents:
    ===========================
    DMFWood6
    NBglass            (Norm Bowler)
    NBoldglass         (Norm Bowler)
    NBwinebottle       (Norm Bowler)
    NBbeerbottle       (Norm Bowler)
    Ruby_Glass         (after Norm Bowler)
    Dark_Green_Glass   (after Norm Bowler)
    Yellow_Glass       (after Norm Bowler)
    Orange_Glass       (after Norm Bowler)
    Vicks_Bottle_Glass (after Norm Bowler)
    Soft_Silver        (Dan Farmer)
    New_Penny          (Dan Farmer)
    Tinny_Brass        (Dan Farmer)
    Gold_Nugget        (Dan Farmer)
    Aluminum           (Dan Farmer)
    Bright_Bronze      (Dan Farmer)
    Lightening1        (Dan Farmer)
    Lightening2        (Dan Farmer)
    Brushed_Aluminum   (Dan Farmer)
    Starfield          (Jeff Burton )
    Shadow_Clouds      (Bill Pulver)
*/

#include "finish.inc"

#ifdef(View_POV_Include_Stack)
    #debug "including textures.inc\n"
#end


//*****************************************************************************
//                      STONE PIGMENTS
//*****************************************************************************

#declare Jade_Map =
color_map {
    [0.0 rgb <0.1, 0.6, 0.1>]
    [0.8 rgb <0.0, 0.3, 0.0>]
    [0.8 rgb <0.1, 0.6, 0.1>]
    [1.0 rgb <0.0, 0.2, 0.0>]
}

// Drew Wells' superb Jade.  Color map works nicely with other textures, too.
#declare Jade = 
pigment {
    marble
    turbulence 1.8
    color_map { Jade_Map }
}

#declare Red_Marble_Map = 
color_map {
    [0.0 rgb <0.8, 0.8, 0.6>]
    [0.8 rgb <0.8, 0.4, 0.4>]
    [1.0 rgb <0.8, 0.2, 0.2>]
}
// Classic white marble with red veins.  Over-worked, like checkers.
#declare Red_Marble = 
pigment {
    marble
    color_map { Red_Marble_Map }
    turbulence 1
}

#declare White_Marble_Map =
color_map {
    [0.0 rgb <0.9, 0.9, 0.9>]
    [0.8 rgb <0.5, 0.5, 0.5>]
    [1.0 rgb <0.2, 0.2, 0.2>]
}

// White marble with black veins.
#declare White_Marble = 
pigment {
    marble
    turbulence 1
    color_map { White_Marble_Map }
}

// Light blue and black marble with a thin red vein 
// Try changing LBlue and Vein below to modify the marble.
//#declare LBlue = rgb <0.0, 0.6, 0.6>;
//#declare Vein  = rgb <0.6, 0.0, 0.0>;
//These variables commented out to avoid possibility of name clashes

#declare Blood_Marble_Map =
color_map {
    [0.0 color 0]
    [0.8 color rgb <0.0, 0.6, 0.6>]
    [0.9 color rgb <0.6, 0.0, 0.0>]
//    [0.8 color LBlue]
//    [0.9 color Vein]
    [1.0 color 0]
}
#declare Blood_Marble = 
pigment {
    marble
    color_map { Blood_Marble_Map }
    turbulence 2.3
}

#declare Blue_Agate_Map =
color_map {
    [0.5  rgb <0.30, 0.30, 0.50>]
    [0.55 rgb <0.20, 0.20, 0.30>]
    [0.6  rgb <0.25, 0.25, 0.35>]
    [0.7  rgb <0.15, 0.15, 0.26>]
    [0.8  rgb <0.10, 0.10, 0.20>]
    [0.9  rgb <0.30, 0.30, 0.50>]
    [1.0  rgb <0.10, 0.10, 0.20>]
}

// a grey blue agate -- kind of purplish.
#declare Blue_Agate = 
pigment {
    agate
    color_map { Blue_Agate_Map }
}

#declare Sapphire_Agate_Map = 
color_map {
    [0.0  rgb <0.0, 0.0, 0.9>]
    [0.3  rgb <0.0, 0.0, 0.8>]
    [1.0  rgb <0.0, 0.0, 0.4>]
}


// Deep blue agate -- almost glows.
#declare Sapphire_Agate = 
pigment { 
    agate
    color_map { Sapphire_Agate_Map }
}

#declare Brown_Agate_Map =
color_map {
    [0.0 rgb 0]
    [0.5 rgb <0.9, 0.7, 0.6>]
    [0.6 rgb <0.9, 0.7, 0.4>]
    [1.0 rgb <0.7, 0.4, 0.2>]
}

// Brown and white agate -- very pretty.
#declare Brown_Agate = 
pigment {
    agate
    color_map { Brown_Agate_Map }
}

#declare Pink_Granite_Map =
color_map {
    [0.4  rgb 0]
    [0.4  rgb <0.85, 0.85, 0.95>]
    [0.45 rgb <0.85, 0.85, 0.95>]
    [0.5  rgb <0.75, 0.75, 0.75>]
    [0.55 rgb <0.82, 0.57, 0.46>]
    [0.8  rgb <0.82, 0.57, 0.46>]
    [1.0  rgb <1.00, 0.50, 0.00>]
}

#declare Pink_Granite =  
pigment {
    granite
    color_map { Pink_Granite_Map }
}

//*****************************************************************************
//                      STONE TEXTURES
//*****************************************************************************

// Gray-pink alabaster or marble.  Layers are scaled for a unit object
// and relative to each other.
// NOTE: This texture has very tiny dark blue specks that are often
//       mistaken for rendering errors.  They are not errors.  Just a
//       strange texture design.
#declare PinkAlabaster =
// Underlying surface is very subtly mottled with bozo
texture {
    pigment {
        bozo
        turbulence 0.25
        color_map {
            [0 rgb <0.9, 0.75, 0.75>]
            [1 rgb <0.6, 0.6,  0.6 >]
        }
    scale 0.4
    }
    finish{ ambient 0.25 }
}

// Second layer texture has some transmit values, yet a fair amount of color
// Viening is kept quite thin in color map and by the largish scale.
texture {
    pigment {
        granite
        color_map {
            [0.0 rgbt <0.52, 0.39, 0.39, 1.0>]
            [0.9 rgbt <0.52, 0.39, 0.39, 0.5>]
            [0.9 rgbt <0.42, 0.14, 0.55, 0.0>]
        }
    scale 2  
    }
    finish {
        specular 1   
        roughness 0.0001
        phong 0.25
        phong_size 75
        brilliance 4
    }
}


//*****************************************************************************
//                           SKY PIGMENTS
//*****************************************************************************
#declare Blue_Sky_Map =
color_map {
    [0.5 rgb <0.25, 0.25, 0.5>]
    [0.6 rgb 0.7]
    [1.0 rgb 0.3]
}

// Basic Blue Sky w/ clouds.
#declare Blue_Sky = 
pigment {
    bozo
    color_map { Blue_Sky_Map }
    turbulence 0.3
}

// Bright Blue Sky w/ very white clouds.
#declare Bright_Blue_Sky = 
pigment {
    bozo
    turbulence 0.56
    color_map {
        [0.5 rgb <0.5, 0.5, 1.0>]
        [0.6 rgb 1.0]
        [1.0 rgb 0.5]
    }
}

// Another sky
#declare Blue_Sky2 =
pigment {
    agate
    color_map {
        [0.3 rgb <0, 0, 1>]
        [1.0 rgb 1]
    }
    scale .75
}

// Small puffs of white clouds
#declare Blue_Sky3 = 
pigment {
    granite
    turbulence 0.1
    color_map {
        [0.3 rgb <0, 0, 1>]
        [1.0 rgb 1]
    }
    scale .75
}

// Red sky w/ yellow clouds -- very surreal.
#declare Blood_Sky = 
pigment {
    bozo
    turbulence 0.5
    color_map {
        [0.0 rgb <0.9, 0.700, 0.0>]
        [0.5 rgb <0.3, 0.200, 0.0>]
        [0.5 rgb <0.6, 0.025, 0.0>]
        [0.6 rgb <0.9, 0.700, 0.0>]
        [0.6 rgb <0.6, 0.025, 0.0>]
    }
}

// Black sky with red and purple clouds 
// Try adding turbulence values from 0.1 - 5.0 -- CdW
#declare Apocalypse = 
pigment {
    bozo
    color_map {
        [0.0 rgb <0.8, 0.0, 0.0>]
        [0.4 rgb <0.4, 0.0, 0.4>]
        [0.6 rgb <0.0, 0.0, 0.2>]
        [1.0 rgb 0.0]
    }
}

// White clouds w/ transparent sky.
#declare Clouds = 
pigment {
    bozo
    color_map {
        [0.1 rgbt <0.8, 0.8, 0.8, 0.0>]
        [0.5 rgbt <1.0, 1.0, 1.0, 1.0>] 
    }
}

#declare FBM_Clouds =
pigment {
    bozo
    turbulence 0.65
    octaves 6
    omega 0.7
    lambda 2
    color_map {
        [0.0 rgb 0.85]
        [0.1 rgb 0.75]
        [0.5 rgbt 1]
    }
    scale <6, 1, 6>
}

#declare Shadow_Clouds =
texture {                          // The blue sky background for the clouds 
    pigment { rgb <0.196078, 0.6, 0.8> }
    finish { ambient 0.7 diffuse 0 }
}
texture {                          // The upper part of the clouds
    pigment { FBM_Clouds }
    finish { ambient 1.0 diffuse 0 }
}
texture {                          // The darker underside of the clouds
    pigment { FBM_Clouds  translate -0.15*y }
    finish { ambient 0.6 diffuse 0 }
}



//*****************************************************************************
//                           WOODEN PIGMENTS
//*****************************************************************************

// Several wooden pigments by Tom Price:
// A light reddish wood.
#declare Cherry_Wood =
pigment {
    wood
    turbulence 0.3
    color_map {
        [0.8 rgb <0.66, 0.31, 0.20>]
        [0.8 rgb <0.40, 0.13, 0.06>]
        [1.0 rgb <0.20, 0.06, 0.03>]
    }
}

// A light tan wood with,ish rings.
#declare Pine_Wood = 
pigment {
    wood
    turbulence 0.2
    color_map {
        [0.8 rgb <1.0, 0.72, 0.25>]
        [0.8 rgb <0.5, 0.50, 0.06>]
        [1.0 rgb <0.4, 0.40, 0.03>]
    }
}

// Dark wood with a,ish hue to it.
#declare Dark_Wood = 
pigment {
    wood
    turbulence 0.2
    color_map {
        [0.8 rgb <0.43, 0.24, 0.05>]
        [0.8 rgb <0.40, 0.33, 0.06>]
        [1.0 rgb <0.20, 0.03, 0.03>]
    }
}

// Light tan wood with brown rings.
#declare Tan_Wood = 
pigment {
    wood
    turbulence 0.1
    color_map {
        [0.8 rgb <0.88, 0.60, 0.30>]
        [0.8 rgb <0.60, 0.40, 0.20>]
        [1.0 rgb <0.40, 0.30, 0.20>]
    }
}

// A very pale wood with tan rings -- kind of balsa-ish.
#declare White_Wood = 
pigment {
    wood
    turbulence 0.6
    color_map {
        [0.0 rgb <0.93, 0.71, 0.53>]
        [0.8 rgb <0.98, 0.81, 0.60>]
        [0.8 rgb <0.60, 0.33, 0.27>]
        [1.0 rgb <0.70, 0.60, 0.23>]
    }
}

// Brown wood - looks stained.
#declare Tom_Wood = 
pigment {
    wood
    turbulence 0.31
    color_map {
        [0.8 rgb < 0.7, 0.3, 0.0>]
        [0.8 rgb < 0.5, 0.2, 0.0>]
        [1.0 rgb < 0.4, 0.1, 0.0>]
    }
}

// The scaling in these definitions is relative to a unit-sized object
// (radius 1).  Note that woods are functionally equivilent to a log lying
// along the z axis.  For best results, think like a woodcutter trying to
// extract the nicest board out of that log.  A little tilt along the x axis
// will give elliptical rings of grain like you'd expect to find on most
// boards.  Experiment.
// (The first five came from DODEC2.POV in the POV Scenefile Library.)
#declare DMFWood1 = 
pigment {
    wood
    turbulence 0.04
    octaves 3
    scale <0.05, .05, 1>
    color_map {
        [0.1 rgb <0.60, 0.30, 0.18>]
        [0.9 rgb <0.30, 0.15, 0.09>]
    }
}

#declare DMFWood2 = 
pigment {
    wood
    turbulence 0.03
    octaves 4
    scale <0.05, .05, 1>
    color_map {
        [0.1 rgb <0.52, 0.37, 0.26>]
        [0.9 rgb <0.42, 0.26, 0.15>]
    }
}

#declare DMFWood3 = 
pigment {
    wood
    turbulence 0.05
    octaves 2
    scale <0.05, .05, 1>
    color_map {
        [0.1 rgb <0.4, 0.133, 0.066>]
        [0.9 rgb <0.2, 0.065, 0.033>]
    }
}

#declare DMFWood4 =
pigment {
    wood
    turbulence 0.04
    octaves 3
    scale <0.05, .05, 1>
    color_map {
        [0.1 rgb <0.888, 0.600, 0.3>]
        [0.9 rgb <0.600, 0.400, 0.2>]
    }
}

#declare DMFWood5 = 
pigment {
    wood
    turbulence 0.05
    octaves 6
    scale <0.075, .075, 1>
    color_map {
        [0.1 rgb <0.30, 0.10, 0.050>]
        [0.9 rgb <0.25, 0.07, 0.038>]
    }
}

// This is a three-layer wood texture.  Renders rather slowly because of
// the transparent layers and the two layers of turbulence, but it looks
// great.  Try other colors of "varnish" for simple variations.  
#declare DMFWood6 = 
texture {
    pigment {
        wood  turbulence 0.04
        octaves 3
        scale <0.05, .05, 1>
        color_map { 
            [0.1 rgb <0.88, 0.60, 0.4>]
            [0.9 rgb <0.60, 0.40, 0.3>]
        }
    }
    finish { 
        specular 0.25
        roughness 0.05
        ambient 0.45 
        diffuse 0.33
        reflection 0.15
    }
}
texture {
    pigment {
        wood  turbulence <0.1, 0.5, 1> 
        octaves 5
        lambda 3.25
        scale <0.15, .5, 1>
        color_map { 
            [0.0 rgbt <0.7, 0.6, 0.4, 0.100>]
            [0.1 rgbt <0.8, 0.6, 0.3, 0.500>]
            [0.1 rgbt <0.8, 0.6, 0.3, 0.650>]
            [0.9 rgbt <0.6, 0.4, 0.2, 0.975>]
            [1.0 rgbt <0.6, 0.4, 0.2, 1.000>]
        }
    rotate <5, 10, 5>
    translate -x*2
    }
    finish { 
        specular 0.25 
        roughness 0.0005
        ambient .1 
        diffuse 0.33
    } 
}
// A "coat of varnish" to modify the overall color of the wood
texture {
    pigment { rgbt <0.75, 0.15, 0.0, 0.95> }
    finish { 
        specular 0.25
        roughness 0.01
        ambient 0
        diffuse 0.33
    }
}




// Is this really oak?  I dunno.  Quite light, maybe more like spruce.
#declare DMFLightOak =  
pigment {
    wood
    turbulence 0.05            // For best results,  keep this low!
    scale <0.2, 0.2, 1>        // Scaled for a unit object
    color_map {
        [0.1 rgb <0.42, 0.26, 0.15>]
        [0.9 rgb <0.52, 0.37, 0.26>]
    }
}

// Looks like old desk oak if used correctly.
#declare DMFDarkOak = 
pigment {
    wood
    turbulence 0.04            // For best results,  keep this low!
    octaves 3
    scale <0.2, 0.2, 1>        // Scaled for a unit object
    color_map {
        [0.1 rgb <0.60, 0.30, 0.18>]
        [0.9 rgb <0.30, 0.15, 0.09>]
    }
}

// Wood by Eric Barish
#declare EMBWood1 = 
texture {  /* Bottom wood-grain layer */
    pigment {
        wood
        turbulence 0.05
        color_map {
            [0.00 rgb <0.58, 0.45, 0.23>]
            [0.34 rgb <0.65, 0.45, 0.25>]
            [0.40 rgb <0.33, 0.23, 0.13>]
            [0.47 rgb <0.60, 0.40, 0.20>]
            [1.00 rgb <0.25, 0.15, 0.05>]
        }
    }
    finish {
        crand 0.02
        ambient 0.32
        diffuse 0.63
        phong 0.2
        phong_size 10
    }
    normal { bumps 0.05 }
}
texture {     /* top layer, adds small dark spots */
    pigment {
        bozo
        color_map {
            [0.0 rgbt <1.00, 1.00, 1.00, 1.00>]
            [0.8 rgbt <1.00, 0.90, 0.80, 0.80>]
            [1.0 rgbt <0.30, 0.20, 0.10, 0.40>]
        }
    scale 0.25
    }
}


//   Doug Otwell woods
//   Yellow pine, close grained
//
#declare Yellow_Pine = 
texture {
    pigment {
        wood
        turbulence 0.02
        color_map {
            [0.222 rgb <0.808, 0.671, 0.251>]
            [0.342 rgb <0.600, 0.349, 0.043>]
            [0.393 rgb <0.808, 0.671, 0.251>]
            [0.709 rgb <0.808, 0.671, 0.251>]
            [0.821 rgb <0.533, 0.298, 0.027>]
            [1.000 rgb <0.808, 0.671, 0.251>]
        }
    scale 0.1
    translate <10, 0, 0>
    }
}
// Yellow_Pine layer 2
texture {
    pigment {
        wood
        turbulence 0.01
        color_map {
            [0.000 rgbt <1.000, 1.000, 1.000, 1.000>]
            [0.120 rgbt <0.702, 0.467, 0.118, 0.608>]
            [0.496 rgbt <1.000, 1.000, 1.000, 1.000>]
            [0.701 rgbt <1.000, 1.000, 1.000, 1.000>]
            [0.829 rgbt <0.702, 0.467, 0.118, 0.608>]
            [1.000 rgbt <1.000, 1.000, 1.000, 1.000>]
        }
    scale 0.5 
    translate <10, 0, 0>
    }
}

//
//   Rosewood
//
#declare Rosewood = 
texture {
    pigment {
        bozo
        turbulence 0.04
        color_map {
            [0.000 rgb <0.204, 0.110, 0.078>]
            [0.256 rgb <0.231, 0.125, 0.090>]
            [0.393 rgb <0.247, 0.133, 0.090>]
            [0.581 rgb <0.204, 0.110, 0.075>]
            [0.726 rgb <0.259, 0.122, 0.102>]
            [0.983 rgb <0.231, 0.125, 0.086>]
            [1.000 rgb <0.204, 0.110, 0.078>]
        }
    scale <0.5, 0.5, 1>
    translate <10, 0, 0>
    }
    finish {
        ambient 0.5
        diffuse 0.8
    }
}
// Rosewood layer 2
texture {
    pigment {
        wood
        turbulence 0.04
        color_map {
            [0.000 rgbt <0.545, 0.349, 0.247, 1.000>]
            [0.139 rgbt <0.000, 0.000, 0.000, 0.004>]
            [0.148 rgbt <0.000, 0.000, 0.000, 0.004>]
            [0.287 rgbt <0.545, 0.349, 0.247, 1.000>]
            [0.443 rgbt <0.545, 0.349, 0.247, 1.000>]
            [0.626 rgbt <0.000, 0.000, 0.000, 0.004>]
            [0.635 rgbt <0.000, 0.000, 0.000, 0.004>]
            [0.843 rgbt <0.545, 0.349, 0.247, 1.000>]
        }
    scale <0.5, 0.5, 1>
    translate <10, 0, 0>
    }
    finish {
        ambient 0.5
        diffuse 0.8
    }
}

//
//   Sandalwood ( makes a great burled maple, too)
//
#declare Sandalwood = 
texture {
    pigment {
        bozo
        turbulence 0.2
        color_map {
            [0.000 rgb <0.725, 0.659, 0.455>]
            [0.171 rgb <0.682, 0.549, 0.420>]
            [0.274 rgb <0.557, 0.451, 0.322>]
            [0.393 rgb <0.725, 0.659, 0.455>]
            [0.564 rgb <0.682, 0.549, 0.420>]
            [0.701 rgb <0.482, 0.392, 0.278>]
            [1.000 rgb <0.725, 0.659, 0.455>]
        }
    scale <0.2, 0.2, 1>
    scale 2
    }
}
// Sandalwood layer 2
texture {
    pigment {
        bozo
        turbulence 0.8
        color_map {
            [0.000 rgbt <0.682, 0.604, 0.380, 1.000>]
            [0.087 rgbt <0.761, 0.694, 0.600, 0.020>]
            [0.226 rgbt <0.635, 0.553, 0.325, 1.000>]
            [0.348 rgbt <0.761, 0.694, 0.600, 0.020>]
            [0.496 rgbt <0.682, 0.604, 0.380, 1.000>]
            [0.565 rgbt <0.761, 0.694, 0.600, 0.020>]
            [0.661 rgbt <0.682, 0.604, 0.380, 1.000>]
            [0.835 rgbt <0.761, 0.694, 0.600, 0.020>]
            [1.000 rgbt <0.682, 0.604, 0.380, 1.000>]
        }
    scale 0.4
    }
}


//*****************************************************************************
//                           GLASS TEXTURES
//*****************************************************************************

/* Note: New in POV-Ray 3.1, the "ior" keyword is supposed to be 
         specified in the new "interior{...}" statement.  
  
         Under POV-Ray 3.0x and prior, the "refraction" keyword served
         two puropses.  
           1) Turn on refraction: this is no longer necessary.  Any use
              of the "ior" keyword with a value other than 1.0 will
              turn on refraction.
           2) Attenuate transparency: Values of "refraction" that are
              between 0.0 and 1.0 would attenuate or darken the amount
              of light passing through.  The same effect can be obtained
              by adjusting the filter value.  A more realistic effect
              can be obtained using the new "fade_power" and "fade_distance"
              keywords in the "interior" statement.
         These textures and finishes should still work under POV-Ray 3.1
         but any new textures you create should make use of the new syntax.
 */
/*Modified for POV 3.5, added glass materials
More realistic results can be obtained if a perfectly clear pigment is used,
and either absorbing media or fade_color are used to color the glass.*/

#declare Glass_Finish=
finish {
    specular 1
    roughness 0.001
    ambient 0
    diffuse 0
    reflection 0.1
 #if (version<3.1)
    ior 1.5
 #end
}

#declare Glass_Interior = interior {ior 1.5}

#declare Glass = 
texture {
    pigment { rgbf<1.0, 1.0, 1.0, 0.7> }
    finish  { Glass_Finish }
}
#declare M_Glass = material {texture {Glass} interior {Glass_Interior}}

// Probably more of a "Plexiglas" than glass
#declare Glass2 = 
texture {
    pigment { rgbf <1,1,1,1> }
    finish {
        ambient 0
        diffuse 0
        reflection 0.5
        phong 0.3
        phong_size 60
    }
}
#declare M_Glass2 = material {texture {Glass2} interior {Glass_Interior}}

// An excellent lead crystal glass!
#declare Glass3 = 
texture {
    pigment { rgbf <0.98, 0.98, 0.98, 0.9> }
    finish  {
        ambient 0.1
        diffuse 0.1
        reflection 0.1
        specular 0.8
        roughness 0.0003
        phong 1 
        phong_size 400
     }
}
#declare M_Glass3 = material {texture {Glass3} interior {Glass_Interior}}

#declare Green_Glass = 
texture {
    Glass3
    pigment { rgbf <0.8, 1, 0.95, 0.9> }
}
#declare M_Green_Glass = material {texture {Green_Glass} interior {Glass_Interior}}

// Glass textures contributed by Norm Bowler, of Richland WA 
#declare NBglass =
texture {
    pigment { rgbf <0.98, 1.0, 0.99, 0.75> }
    finish {
        ambient 0.1
        diffuse 0.1
        reflection .25
        specular 1
        roughness .001
 #if (version<3.1)
    ior 1.5
 #end
    }
}
#declare M_NB_Glass = material {texture {NBglass} interior {Glass_Interior}}


#declare NBoldglass=
texture { 
    NBglass
    pigment { rgbf <0.8, 0.9, 0.85, 0.85> }
}
#declare M_NB_Old_Glass = material {texture {NBoldglass} interior {Glass_Interior}}

#declare NBwinebottle=
texture { 
    NBglass
    pigment { rgbf <0.4, 0.72, 0.4, 0.6> }
}
#declare M_NB_Winebottle_Glass = material {texture {NBwinebottle} interior {Glass_Interior}}

#declare NBbeerbottle=
texture { 
    NBglass
    pigment { rgbf <0.7, 0.5, 0.1, 0.6> }
}
#declare M_NB_Beerbottle_Glass = material {texture {NBbeerbottle} interior {Glass_Interior}}

// A few color variations on Norm's glass
// Ruby glass
#declare Ruby_Glass =
texture { 
    NBglass
    pigment { rgbf <0.9, 0.1, 0.2, 0.8> }
}
#declare M_Ruby_Glass = material {texture {Ruby_Glass} interior {Glass_Interior}}

// Dark, green glass
#declare Dark_Green_Glass=
texture { 
    NBglass
    pigment { rgbf <0.1, 0.7, 0.8, 0.8> }
}
#declare M_Dark_Green_Glass = material {texture {Dark_Green_Glass} interior {Glass_Interior}}

// Yellow glass
#declare Yellow_Glass=
texture { 
    NBglass
    pigment { rgbf <0.8, 0.8, 0.2, 0.8> }
}
#declare M_Yellow_Glass = material {texture {Yellow_Glass} interior {Glass_Interior}}

// Orange/Amber glass
#declare Orange_Glass=
texture { 
    NBglass
    pigment { rgbf <1.0, 0.5, 0.0, 0.8> }
}
#declare M_Orange_Glass = material {texture {Orange_Glass} interior {Glass_Interior}}

// Vicks bottle, glass
#declare Vicks_Bottle_Glass=
texture { 
    NBglass
    pigment { rgbf <0.1, 0.15, 0.5, 0.9> }
}
#declare M_Vicks_Bottle_Glass = material {texture {Vicks_Bottle_Glass} interior {Glass_Interior}}

//*****************************************************************************
//                           METAL FINISHES
//*****************************************************************************

#declare Metal =
finish {
    metallic
    ambient 0.2
    diffuse 0.7
    brilliance 6
    reflection 0.25
    phong 0.75
    phong_size 80
}

//*****************************************************************************
//                           METAL TEXTURES
//*****************************************************************************


// Good looking "metal" textures
#declare Chrome_Texture = 
texture {
    pigment { rgb <0.658824, 0.658824, 0.658824> }
    finish {
        ambient 0.3
        diffuse 0.7
        reflection 0.15
        brilliance 8
        specular 0.8
        roughness 0.1
    }
}

// A series of metallic textures using the Metal finish:
#declare Brass_Texture  = texture { pigment{ rgb <0.71, 0.65, 0.26>} finish{ Metal }}
#declare Gold_Texture   = texture { pigment{ rgb <0.85, 0.85, 0.10>} finish{ Metal }}
#declare Bronze_Texture = texture { pigment{ rgb <0.55, 0.47, 0.14>} finish{ Metal }}
#declare Copper_Texture = texture { pigment{ rgb <0.72, 0.45, 0.20>} finish{ Metal }}
#declare Silver_Texture = texture { pigment{ rgb <0.90, 0.91, 0.98>} finish{ Metal }}

// In the future, please refer to Chrome_Texture by this name.  I'd like
// to scrap the old name someday. Ditto with other "_Texture" names!
#declare Chrome_Metal = texture { Chrome_Texture }
#declare Brass_Metal  = texture { Brass_Texture  }
#declare Bronze_Metal = texture { Bronze_Texture }
#declare Gold_Metal   = texture { Gold_Texture   }
#declare Silver_Metal = texture { Silver_Texture }
#declare Copper_Metal = texture { Copper_Texture }

// A couple highly reflective metal textures.
#declare Polished_Chrome = 
texture {
    pigment { rgb <0.2, 0.2, 0.2> }
    finish {
        ambient 0.1
        diffuse 0.7
        brilliance 6.0
        reflection 0.6
        phong 0.8
        phong_size 120
    }
}

#declare Polished_Brass = 
texture {
    pigment { rgb <0.578, 0.422, 0.195> }
    finish {
        metallic
        ambient 0.1
        diffuse 0.8
        brilliance 6.0
        reflection 0.4
        phong 0.8
        phong_size 120
   }
}


// Beautiful military brass texture!
#declare New_Brass = 
texture {
    pigment { rgb <0.70, 0.56, 0.37> }
    finish {
        ambient 0.35
        diffuse 1.0
        brilliance 15
        phong 0.41
        phong_size 5
    }
}


// Spun Brass texture for cymbals & such
#declare Spun_Brass = 
texture { 
    New_Brass
    normal { waves 0.35 frequency 2 scale 0.01 }
}

// Brushed aluminum (brushed along X axis)
#declare Brushed_Aluminum = 
texture {
    Chrome_Metal
    normal {
        bumps -0.5
        scale <1, 0.001, 0.001>
    }
}


#declare SilverFinish =
finish {
    metallic
    ambient 0.25
    diffuse 0.65
    reflection 0.45
    brilliance 6
    phong 1
    phong_size 100
}


// Each of these looks good.  Slightly,r as you go down
#declare Silver1_Colour = color  rgb <0.94, 0.93, 0.83>;
#declare Silver2_Colour = color  rgb <0.94, 0.93, 0.86>;
#declare Silver3_Colour = color  rgb <0.94, 0.93, 0.90>;

#declare Silver1 = 
texture {
    pigment { Silver1_Colour }
    finish { SilverFinish }
}

#declare Silver2 = 
texture {
    pigment { Silver2_Colour }
    finish { SilverFinish }
}

#declare Silver3 = 
texture {
    pigment { Silver3_Colour }
    finish { SilverFinish }
}


// Interesting texture -- Give it a try.
// Sort of a "rgb <0.0, 0.0, 0.0> Hills Gold", black, white, and orange specks or splotches.
#declare Brass_Valley = 
texture {
    pigment {
        granite
        color_map {
            [0.3 rgb <0.82, 0.57, 0.46>]
            [0.3 rgb <0.00, 0.00, 0.00>]
            [0.6 rgb <0.85, 0.85, 0.95>]
            [0.6 rgb <0.82, 0.57, 0.46>]
            [1.0 rgb <0.85, 0.85, 0.95>]
        }
    }
    finish {
        metallic
        brilliance 6.0
        reflection 0.75
        phong 0.75
    }
}

#declare Rust =
texture { 
    pigment {
        granite
        color_map {
            [0.0 rgb <0.89, 0.51, 0.28>]
            [0.4 rgb <0.70, 0.13, 0.00>]
            [0.5 rgb <0.69, 0.41, 0.08>]
            [0.6 rgb <0.49, 0.31, 0.28>]
            [1.0 rgb <0.89, 0.51, 0.28>]
        }
    }
    finish { ambient 0.2 diffuse 0.4 }
}

#declare Rusty_Iron = 
texture {
    pigment {
        granite
        color_map {
            [0.0 rgb <0.42, 0.20, 0.20>]
            [0.5 rgb <0.50, 0.50, 0.02>]
            [0.6 rgb <0.60, 0.20, 0.20>]
            [0.6 rgb <0.30, 0.20, 0.20>]
        }
    }
    finish { ambient 0.2 diffuse 0.6 }
    normal { wrinkles 1 scale 0.1 }
}


#declare Soft_Silver = 
texture {
    pigment { Silver1_Colour }
    finish {
        metallic
        ambient 0.2
        diffuse 0.35
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }
}

#declare Metallic_Finish =
    finish {
        metallic
        ambient 0.1
        diffuse 0.65
        specular 0.85
        roughness 0.01
        reflection 0.45
        brilliance 1.5
    }

#declare New_Penny = 
texture {
    pigment { rgb <0.6, 0.45, 0.4> }
    finish { Metallic_Finish }
}

#declare Tinny_Brass = 
texture {
    pigment { rgb <0.70, 0.56, 0.37> }
    finish { Metallic_Finish }
}

#declare Gold_Nugget = 
texture {
    pigment { rgb <0.5, 0.35, 0.25> }
    finish { Metallic_Finish }
}

#declare Aluminum  = 
texture {
    pigment { rgb <0.55, 0.5, 0.45> }
    finish { Metallic_Finish }
}

#declare Bright_Bronze  = 
texture {
    pigment { rgb <0.36, 0.28, 0.20> }
    finish { Metallic_Finish }
}


//*****************************************************************************
//                    SPECIAL EFFECTS
//*****************************************************************************

// Red & white stripes - Looks best on a y axis Cylinder
// It "spirals" because it's gradient on two axis
#declare Candy_Cane = 
pigment {
    gradient x+y
    color_map {
        [0.25 rgb <1,0,0>]
        [0.25 rgb <1,1,1>]
        [0.75 rgb <1,1,1>]
        [0.75 rgb <1,0,0>]
    }
}

// Orange and Clear stripes spiral around the texture
// to make an object look like it was "Peeled"
// Now, you too can be M.C. Escher 
#declare Peel = 
pigment {
    gradient x+y
    color_map {
        [0.25 rgbf <1.0, 0.5, 0.0, 0.0>]
        [0.25 rgbf <1.0, 1.0, 1.0, 1.0>]
        [0.75 rgbf <1.0, 1.0, 1.0, 1.0>]
        [0.75 rgbf <1.0, 0.5, 0.0, 0.0>]
    }
}

#declare Y_Gradient = 
pigment {
    gradient y
    color_map {
        [0.00  rgb <1,0,0>]
        [0.33  rgb <0,0,1>]
        [0.66  rgb <0,1,0>]
        [1.00  rgb <1,0,0>]
    }
}

#declare X_Gradient = 
pigment {
    gradient x
    color_map {
        [0.00 rgb <1,0,0>]
        [0.33 rgb <0,0,1>]
        [0.66 rgb <1,1,1>]
    }
}

// Wavy water 
// Requires a sub-plane, and may require scaling to fit your scene.
//A far better water texture can be done using a perfectly clear pigment
//and fade_color or absorbing media in the interior.

#declare Water =
texture {
    pigment{ rgbf <0.0, 0.0, 1.0, 0.9> }
    normal {
        ripples 0.75
        frequency 10
    }
    finish {
        reflection {0.3, 1 fresnel}
        conserve_energy
    }
}
#declare Water_Int =
interior {
    ior 1.33
}

#declare M_Water =
material {
    texture {Water}
    interior {Water_Int}
}

#declare Cork =
texture {
    pigment {
        granite
        color_map {
            [0.00 rgb <0.93, 0.71, 0.532>]
            [0.60 rgb <0.98, 0.81, 0.60>]
            [0.60 rgb <0.50, 0.30, 0.20>]
            [0.65 rgb <0.50, 0.30, 0.20>]
            [0.65 rgb <0.80, 0.53, 0.46>]
            [1.00 rgb <0.85, 0.75, 0.35>]
        }
    }
    finish{ 
        specular 0.1 
        roughness 0.5 
    }
    scale 0.25     // Generally looks best scaled longer on one axis
}

//The correct spelling is "Lightning", not "Lightening"

#declare Lightning_CMap1 =  
color_map {
    [0.00 rgbf <1,1,1,0>]
    [0.15 rgbf <0.94, 0.81, 0.99, 0.65>]
    [0.25 rgbf <0.94, 0.81, 0.99, 0.65>]
    [0.30 rgbf <0.87, 0.58, 0.98, 0.85>]
    [0.40 rgbf <0.87, 0.58, 0.98, 0.85>]
    [0.45 rgbf <0.73, 0.16, 0.96, 0.95>]
}

#declare Lightning1 = 
texture {
    pigment {
        marble
        color_map { Lightning_CMap1 }
        turbulence 0.5
    }
    finish { ambient 1 }
}


#declare Lightning_CMap2 =  
color_map {
    [0.00 rgbf <1,1,1,0>]
    [0.10 rgbf <0.94, 0.81, 0.99, 0.65>]
    [0.20 rgbf <0.94, 0.81, 0.99, 0.65>]
    [0.30 rgbf <0.87, 0.58, 0.98, 0.85>]
    [0.45 rgbf <0.87, 0.58, 0.98, 0.85>]
    [0.65 rgbf <0.73, 0.16, 0.96, 0.95>]
}

#declare Lightning2 = 
texture {
    pigment {
        granite
        color_map { Lightning_CMap2 }
        turbulence 0.5
    }
    finish { ambient 1 }
}

// Starfield, by Jeff Burton
#declare Starfield = 
texture {
    pigment {
        granite  
        color_map {
            [0.72 rgb 0.00 ] // No Stars in this area
            [0.72 rgb 0.20 ] // Very Very Faint Stars
            [0.75 rgb 0.40 ] // Very Very Faint Stars
            [0.78 rgb 0.60 ] // Very Faint Stars
            [0.81 rgb 0.80 ] // Faint Stars
            [0.85 rgb 0.95 ] // Medium White Stars
            [0.91 rgb 1.00 ] // White Stars 
            [0.91 rgb 0.00 ] // No Stars in this area
        }
    scale .015
    }
    finish { ambient 1 }
}

// Irregular_Bricks_Ptrn() authors: Ron Parker and Rune S. Johansen
/* Irregular_Bricks_Ptrn( Mortar Thickness, X-scaling, Variation, Roundness )
This function pattern creates a pattern of bricks of varying lengths on the x-y plane.
This can be useful in building walls that don't look like they were built by a computer.
Note that mortar thickness between bricks can vary somewhat, too.

Mortar Thickness: How thick the mortar should be (0-1)
       X-scaling: The scaling of the bricks (but not the mortar) in the x direction
       Variation: The amount by which brick lengths will vary (0=none, 1=100%)
       Roundness: The roundness of the bricks (0.01=almost rectangular, 1.0=very round)

Sample code:
plane {-z, 0
   texture {
      pigment {
         Irregular_Bricks_Ptrn (0.1, 1, 0.5, 0.5)
         color_map {
            [0.01, rgb 0.9]
            [0.01, rgb 0.6]
         }
      }
      normal {
         Irregular_Bricks_Ptrn (0.1, 1, 0.5, 0.5) 2
      }
   }
}*/
#macro Irregular_Bricks_Ptrn (T, S, V, R)
   #local FunctionXGrad =
   function {
      pattern {
         gradient x triangle_wave
         warp {turbulence V*x octaves 1}
         warp {planar z 0}
         warp {repeat z offset< 0.5, 5, 0>}
         rotate -90*x
      }
   }
   #local FunctionYGrad =
   function {
      pattern {
         gradient y triangle_wave
         translate 0.5*y
      }
   }
   function {
      pow
      (1-min(1,
         (
            +pow(FunctionXGrad(x/S,y,z)*(1+T/S),1/R*2)
            +pow(FunctionYGrad(x/S,y,z)*(1+T  ),1/R*2)
         )
      ),R/2)
   }
#end


// Hex_Tiles_Ptrn() authors: Ron Parker and Juha Nieminen
#macro Hex_Tiles_Ptrn()
    #local G =
    pigment {gradient x
        color_map {[0 rgb 1][1 rgb 0]}
        scale sqrt(3)/2+.001
    }
    #local B =
    pigment {radial
        pigment_map {
            #local I = 0;
            #while(I<6)
                [I/6 G rotate(30+60*I)*y]
                [(I+1)/6 G rotate(30+60*I)*y]
                #local I = I + 1;
            #end
        }
    }
    pigment_pattern {
        #local T2 = sqrt(3)/2;
        radial
        pigment_map {
            [0   B translate < 0.5,0,-T2>]
            [1/3 B translate < 0.5,0,-T2>]
            [1/3 B translate <-1.0,0,  0>]
            [2/3 B translate <-1.0,0,  0>]
            [2/3 B translate < 0.5,0, T2>]
            [1   B translate < 0.5,0, T2>]
        }
        translate x
        warp {repeat 1.5*x flip x}
        warp {repeat.5*sqrt(3)*z flip z}
    }
#end

// Tiles_Ptrn() author: Rune S. Johansen
#macro Tiles_Ptrn()
   boxed scale 0.5 translate <0.5,0.5,0>
   warp {repeat x} warp {repeat y} warp {planar z, 0}
#end


// Star_Ptrn() author: Ron Parker
/* Star pattern 
This macro creates a pattern that resembles a star. It can be used together with
color_maps, pigment_maps, texture_maps etc. The pattern is in the X-Z plane,
centered around the origin.
           
 Star_Ptrn( Radius, Points, Skip )
           
     Radius: the radius of a circle drawn through the points
             of the star
     Points: the number of points on the star.
       Skip: the number of points to skip when drawing lines between
             points to form the star.  A normal 5-pointed star skips 
             2 points.  A Star of David also skips 2 points.  
             Skip must be less than <Points>/2 and greater than 0.  
             Integers are preferred but not required.  Skipping 1 
             point makes a regular polygon with <Points> sides.
      
 Example usage: (This creates a five-pointed star as on a US flag)
      pigment {
         Star_Ptrn(1,5,2)
         color_map {[0,rgb 1][1,blue 0.5]}
      }*/

#macro Star_Ptrn(SR, NP, SP)
    #local P =
    pigment {radial
        #local PA = 0.5-(SP/NP);
        pigment_map {
            [.5-PA/2 rgb 0]
            [.5-PA/2 rgb 1]
            [.5+PA/2 rgb 1]
            [.5+PA/2 rgb 0]
        }
        translate SR*x
    }
    pigment_pattern {radial
        pigment_map {
            #local I=0;
            #while(I<NP)
                [(I+.5)/NP P rotate I*360/NP*y]
                [(I+.5)/NP P rotate(I+1)*360/NP*y]
                #local I = I + 1;
            #end
        }
        rotate -90*x +90*z
    }
#end

#version Textures_Inc_Temp;
#end
