// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: woods.inc
//    Last updated: 2001.7.24
//    Description:
/*                         Basic wooden textures

  Credits:
  Textures T_Wood1 through T_Wood30 by Dan Farmer

  Textures T_Wood31 through T_Wood35 contributed by
  Paul Novak, 75274.1613@compuserve.com

  Notes on layered textures T_Wood1 through T_Wood30:

  The pigment definitions:
  ========================
  Declares pigments P_Woodgrain1A through P_Woodgrain19A and
  P_Woodgrain1B through P_Woodgrain19B.  The only difference between the
  "A" designations and the "B" designations is in design.  The "A"'s
  were designed to generally work better as underlying patterns, using
  opaque color_maps,  while the "B" pigments were designed to work
  better as top layers, with some degree of transparency in their
  color_maps.

  The pigments with the "A" and "B" designations are combinations that
  were designed to work well together, but do not neccessarily have to
  be used as pairs.

  These pigment definitions do not have colormaps assigned so
  that they can mix 'n matched in various combinations by the user when
  designing their own colors.  In this file, the color_maps are added at
  the point of defining the final textures.


  The color_map definitions:
  ==========================
  These are declared in the #include file "woodmaps.inc".  They are
  named M_Wood1A through M_Wood19A and M_Wood1B through M_Wood19B.

  There are two types of declarations in woodmaps.inc. The "A" color_maps
  are intended as the 1st layer of multi-layer textures, but many work
  well by themselves, as well.

  The "B" color_maps contain transparent colors and are intended for
  use as top layers to influence the "A" maps.


  The wood textures
  =================
  Here the various P_WoodGrainx textures are combined with color_maps to
  create textures named T_Wood1 through T_Wood30.


  General
  =======
  Wood textures are designed with the major axis of the woodgrain
  "cylinder" aligned along the Z axis.  With the exception of the few
  of the textures below that have a small amount of x-axis rotation
  built-in, the textures below will exhibit a very straight grain
  pattern unless you apply a small amount of x-axis rotation to them
  (generally 2 to 4 degrees seems to work well).
*/

#ifndef(Woods_Inc_Temp)
#declare Woods_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including woods.inc\n"
#end

#include "woodmaps.inc"


#declare P_WoodGrain1A =
pigment {
    wood
    turbulence 0.04
    octaves 3
    scale <0.05, .05, 1>
}

#declare P_WoodGrain1B =
pigment {
    wood
    turbulence <0.1, 0.5, 1>
    octaves 5
    lambda 3.25
    scale <0.15, .5, 1>
    rotate <5, 10, 5>
    translate -x*2
}

#declare P_WoodGrain2A =
pigment {
    wood
    turbulence 0.04
    octaves 3
    scale <0.15, .15, 1>
}

#declare P_WoodGrain2B =
pigment {
    wood  
    turbulence <0.1, 0.35, 0.1>
    octaves 5
    lambda 3.25
    scale <0.25, .25, 1>
    rotate <5, 10, 5>
    translate -x*2
}

#declare P_WoodGrain3A = 
pigment {
    wood  
    turbulence 0.04
    octaves 3
    scale <0.05, .05, 1>
    rotate <2,2,0>
}

#declare P_WoodGrain3B =
pigment {
    wood  
    turbulence <0.1, 0.5, 1> 
    octaves 5
    lambda 3.25
    scale <0.15, .5, 1>
    rotate <5, 10, 5>
    translate -x*2
}

#declare P_WoodGrain4A = 
pigment {
    wood
    turbulence 0.04
    octaves 4
    omega 1.25
    scale <0.15, .15, 1>
}

#declare P_WoodGrain4B =
pigment {
    bozo
    turbulence <0.03, 0.03, 0.1>
    octaves 5
    lambda 3.25
    scale <0.025, .025, 1.0>
}
#declare P_WoodGrain5A =
pigment {
    wood  
    turbulence <0.04, 0.04, 0.1>
    octaves 4
    omega 1.25
    scale <0.15, .15, 1>
}

#declare P_WoodGrain5B =
pigment {
    wood  
    turbulence <0.025, 0.025, 0.1>
    octaves 4
    omega 1.3
    scale <0.30, .20, 1>
    translate <0.1, 0.1, 0.20>
}


#declare P_WoodGrain6A =
pigment {
    wood 
    turbulence <0.05, 0.08, 1>
    octaves 4
    scale <0.15, .15, 1>
    translate -x*100
}

#declare P_WoodGrain6B =
pigment {
    wood  
    turbulence <0.05, 0.05, 0>
    octaves 4
    omega 0.95
    scale <0.20, 0.20, 1>             
    rotate x*20
}


#declare P_WoodGrain7A =
pigment {
    wood
    turbulence <0.05, 0.08, 1000>
    octaves 4
    scale <0.15, .15, 1>
}

#declare P_WoodGrain7B =
pigment {
    bozo
    scale <0.01, 0.01, 100000>
}


#declare P_WoodGrain8A =
pigment {
    wood
    turbulence 0.04
    octaves 3
    scale <0.05, .05, 1>
}

#declare P_WoodGrain8B =
pigment {
    wood
    turbulence <0.04, 0.04, 1>
    octaves 4
    scale <0.05, 0.05, 1> * 0.66
}


#declare P_WoodGrain9A =
pigment {
    wood  
    turbulence 0.1
    octaves 4
    lambda 3
    scale 0.2
    rotate <0.125, 0.125, 0>
}

#declare P_WoodGrain9B =
pigment {
    wood
    turbulence 0.1          
    octaves 4
    lambda 3.5
    scale 0.2
    rotate <0.125, 0.125, 0>
    translate <0.015, 0.015, 0.015>
}


#declare P_WoodGrain10A =
pigment {
    wood  
    turbulence 0.02
    octaves 4
    lambda 3
    scale 0.175
    rotate <2, 2, 0>
}


#declare P_WoodGrain10B =
pigment {
    wood
    turbulence 0.02
    octaves 4
    lambda 2.8
    scale 0.2
    rotate <2, 2, 0>
    translate <0.0175, 0.0175, 0.0175>
}


#declare P_WoodGrain11A =
pigment {
    wood
    turbulence 0.02
    scale 0.1
    translate <10, 0, 0>
}

#declare P_WoodGrain11B =
pigment {
    wood
    turbulence 0.01
    scale 0.5
    translate <10, 0, 0>
}


#declare P_WoodGrain12A =
pigment {
    bozo
    turbulence 0.04
    lambda 2.5
    omega 0.1
    octaves 7
    scale <0.5, 0.05, 0.05>
}

#declare P_WoodGrain12B =
pigment {
    wood
    turbulence <0.1, 0.04, 1>
    scale <0.15, 0.5, 1>
//    translate <10, 0, 0>
    rotate x*2
}


#declare P_WoodGrain13A =
pigment {
    wood
    turbulence 0.02
    scale 0.1
    translate <10, 0, 0>
}

#declare P_WoodGrain13B =
pigment {
    wood
    turbulence 0.01
    scale 0.05 
//    translate <10, 0, 0>
}


#declare P_WoodGrain14A =
pigment {
    wood
    colour_map { M_Wood14A }
    turbulence 0.065
    octaves 2
    scale <0.15, .15, 1>
    translate < -1 0 0 >
    rotate <-3, -3, 0>
}

#declare P_WoodGrain14B=
pigment {
    wood
    colour_map { M_Wood14B }
    turbulence <0, 0.1, 0>
    lambda 2.75
    omega 1.15
    octaves 4
    scale <5, 0.075, 1>
    rotate x*90
}


#declare P_WoodGrain15A =
pigment {
    bozo
    colour_map { M_Wood15A }
    turbulence 0.04
    scale <0.05 0.05 1>
}


#declare P_WoodGrain15B =
pigment {
    wood
    colour_map { M_Wood15B }
    scale <0.20 0.20 1>
    turbulence 0.04
    rotate <-2, 2, 0>
}


#declare P_WoodGrain16A =
pigment {
    bozo
    colour_map { M_Wood16A }
    turbulence 0.04
    scale <0.02 0.02 1>
}


#declare P_WoodGrain16B =
pigment {
    wood
    colour_map { M_Wood16B }
    scale <0.075 0.075 1>
    turbulence 0.035
    rotate <-2, 2, 0>
}


#declare P_WoodGrain17A = 
pigment {
    wood
    colour_map { M_Wood17A }
    turbulence 0.04
    omega 0.4
    scale <0.1 0.1 1>
    rotate -x*4
}

#declare P_WoodGrain17B =
pigment {
    wood
    colour_map { M_Wood17B }
    turbulence 0.05
    omega 0.65
    scale <0.2 0.2 1>
    rotate -x*2
}

#declare P_WoodGrain18A =
pigment {
    wood
    colour_map { M_Wood18A }
    turbulence 0.02
    octaves 4
    lambda 4
//    scale 0.3
    scale 0.1
    rotate <2, 0, 0>
}


#declare P_WoodGrain18B =
pigment {
    wood
    colour_map { M_Wood18B }
    turbulence 0.02
    octaves 6
    lambda 2.8
    omega 0.6
//    scale 0.1
    scale 0.05
    rotate <2, 0, 0>
}

#declare P_WoodGrain19A =
pigment {
    wood
    scale <0.075, 0.075, 1>
    turbulence 0.065
    omega 0.45
    lambda 2.3
    color_map { M_Wood19A }
    rotate x*4
}
#declare P_WoodGrain19B =
pigment {
    bozo
    color_map { M_Wood19B }
    scale <0.013, 0.013, 0.75>
}



// Natural oak (light)
#declare T_Wood1 =
    texture { pigment { P_WoodGrain1A  color_map { M_Wood1A }}}
    texture { pigment { P_WoodGrain1B  color_map { M_Wood1B }}}

// Dark brown
#declare T_Wood2   =
    texture { pigment { P_WoodGrain2A  color_map { M_Wood2A }}}
    texture { pigment { P_WoodGrain2B  color_map { M_Wood2B }}}

// Bleached oak (white)
#declare T_Wood3   =         
    texture { pigment { P_WoodGrain3A  color_map { M_Wood3A }}} 
    texture { pigment { P_WoodGrain3B  color_map { M_Wood3B }}}

// Mahogany (purplish-red)
#declare T_Wood4   =
    texture { pigment { P_WoodGrain4A  color_map { M_Wood4A }}}
    texture { pigment { P_WoodGrain4B  color_map { M_Wood4B }}}

// Dark yellow with reddish overgrain
#declare T_Wood5   =         
    texture { pigment { P_WoodGrain5A  color_map { M_Wood5A }}} 
    texture { pigment { P_WoodGrain5B  color_map { M_Wood5B }}}

// Cocabola (red)
#declare T_Wood6   =         
    texture { pigment { P_WoodGrain6A  color_map { M_Wood6A }}}
    texture { pigment { P_WoodGrain6B  color_map { M_Wood6B }}}

// Yellow pine (ragged grain)
#declare T_Wood7   =         
    texture { pigment { P_WoodGrain7A  color_map { M_Wood7A }}} 
    texture { pigment { P_WoodGrain7B  color_map { M_Wood7B }}}

// Dark brown. Walnut?    
#declare T_Wood8   =
    texture { pigment { P_WoodGrain8A  color_map { M_Wood8A }}} 
    texture { pigment { P_WoodGrain8B  color_map { M_Wood8B }}}

// Yellowish-brown burl (heavily turbulated)
#declare T_Wood9   =         
    texture { pigment { P_WoodGrain9A  color_map { M_Wood9A }}}
    texture { pigment { P_WoodGrain9B  color_map { M_Wood9B }}}

// Soft pine (light yellow, smooth grain)
#declare T_Wood10 = 
    texture {  pigment{ P_WoodGrain10A color_map { M_Wood10A }}} 
    texture {  pigment{ P_WoodGrain10B color_map { M_Wood10B }}}

// Spruce (yellowish, very straight, fine grain)
#declare T_Wood11 = 
    texture {  pigment{ P_WoodGrain11A color_map { M_Wood11A }}} 
    texture {  pigment{ P_WoodGrain11B color_map { M_Wood11B }}}

// Another very dark brown.  Walnut-stained pine, perhaps?
#declare T_Wood12 = 
    texture {  pigment{ P_WoodGrain12A color_map { M_Wood12A }}} 
    texture {  pigment{ P_WoodGrain12B color_map { M_Wood12B }}}

// Very straight grained, whitish
#declare T_Wood13 = 
    texture {  pigment{ P_WoodGrain13A color_map { M_Wood13A }}}
    texture {  pigment{ P_WoodGrain13B color_map { M_Wood13B }}}

// Red, rough grain
#declare T_Wood14 = 
    texture {  pigment{ P_WoodGrain14A color_map { M_Wood14A }}} 
    texture {  pigment{ P_WoodGrain14B color_map { M_Wood14B }}}

// Medium brown
#declare T_Wood15 =
    texture {  pigment{ P_WoodGrain15A color_map { M_Wood15A }}} 
    texture {  pigment{ P_WoodGrain15B color_map { M_Wood15B }}}

// Medium brown
#declare T_Wood16 = 
    texture {  pigment{ P_WoodGrain16A color_map { M_Wood16A }}} 
    texture {  pigment{ P_WoodGrain16B color_map { M_Wood16B }}}

// Medium brown
#declare T_Wood17 = 
    texture {  pigment{ P_WoodGrain17A color_map { M_Wood17A }}} 
    texture {  pigment{ P_WoodGrain17B color_map { M_Wood17B }}}

// Orange
#declare T_Wood18 =
    texture {  pigment{ P_WoodGrain18A color_map { M_Wood18A }}}
    texture {  pigment{ P_WoodGrain18B color_map { M_Wood18B }}}

// Golden Oak.
//  M_Woods 1,3,7,8,10,14,15,17,18,19 work well, also.
#declare T_Wood19 =
    texture {  pigment{ P_WoodGrain19A color_map { M_Wood19A }}}
    texture {  pigment{ P_WoodGrain19B color_map { M_Wood19B }}}

#declare T_Wood20 =
    texture {  pigment{ P_WoodGrain19A color_map { M_Wood19A }}}
    texture {  pigment{ P_WoodGrain19B color_map { M_Wood19B }}}

#declare T_Wood21 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood11A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood11B }}}

#declare T_Wood22 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood12A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood12B }}}

#declare T_Wood23 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood13A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood13B }}}

#declare T_Wood24 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood14A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood14B }}}

#declare T_Wood25 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood15A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood15B }}}

#declare T_Wood26 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood16A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood16B }}}

#declare T_Wood27 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood17A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood17B }}}

#declare T_Wood28 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood18A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood18B }}}

#declare T_Wood29 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood19A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood19B }}}

#declare T_Wood30 =
    texture {  pigment{ P_WoodGrain1A color_map { M_Wood1A }}}
    texture {  pigment{ P_WoodGrain1B color_map { M_Wood1B }}}

// A light tan wood - heavily grained (variable coloration)
#declare T_Wood31=
texture {
    pigment {
        wood
        turbulence 0.045
        scale 0.125
        color_map {
          [0.10 color rgbt <0.49020, 0.22353, 0.00000, 0.00>]
          [0.16 color rgbt <0.71863, 0.69412, 0.46275, 0.00>]
          [0.20 color rgbt <0.49020, 0.22353, 0.00000, 0.00>]
          [0.25 color rgbt <0.76863, 0.69412, 0.46275, 0.00>]
          [0.32 color rgbt <0.44020, 0.22353, 0.00000, 0.00>]
          [0.37 color rgbt <0.74863, 0.69412, 0.46275, 0.00>]
          [0.40 color rgbt <0.49020, 0.20353, 0.00000, 0.00>]
          [0.45 color rgbt <0.76863, 0.66412, 0.48275, 0.00>]
          [0.51 color rgbt <0.49020, 0.22353, 0.00000, 0.00>]
          [0.55 color rgbt <0.76863, 0.67059, 0.37255, 0.00>]
          [0.61 color rgbt <0.49020, 0.25353, 0.00000, 0.00>]
          [0.65 color rgbt <0.76863, 0.67059, 0.37255, 0.00>]
          [0.70 color rgbt <0.49020, 0.22353, 0.00000, 0.00>]
          [0.75 color rgbt <0.76863, 0.63059, 0.37255, 0.00>]
          [0.82 color rgbt <0.49020, 0.22353, 0.00000, 0.00>]
          [0.88 color rgbt <0.77863, 0.67059, 0.37255, 0.00>]
          [0.91 color rgbt <0.49020, 0.22353, 0.00000, 0.00>]
          [0.95 color rgbt <0.79863, 0.67059, 0.39550, 0.00>]
          [1.00 color rgbt <0.48020, 0.22353, 0.00000, 0.00>]
        }
    }
}



//A rich dark reddish wood, like rosewood, with smooth-flowing grain
#declare T_Wood32=
texture {
    pigment {
        wood turbulence 0.04
        scale 0.15
        color_map {
            [0.15 color rgbt <0.38039, 0.14902, 0.0, 0.0>]
            [0.25 color rgbt <0.23539, 0.00000, 0.0, 0.0>]
            [0.35 color rgbt <0.38139, 0.14912, 0.0, 0.0>]
            [0.45 color rgbt <0.23549, 0.00000, 0.0, 0.0>]
            [0.55 color rgbt <0.38139, 0.14902, 0.0, 0.0>]
            [0.65 color rgbt <0.23559, 0.00000, 0.0, 0.0>]
            [0.75 color rgbt <0.38139, 0.14922, 0.0, 0.0>]
            [0.85 color rgbt <0.23549, 0.00000, 0.0, 0.0>]
            [0.95 color rgbt <0.38039, 0.14902, 0.0, 0.0>]
            [0.90 color rgbt <0.23539, 0.00000, 0.0, 0.0>]
            [1.00 color rgbt <0.38039, 0.14932, 0.0, 0.0>]
        }
    }
}

// Similar to T_WoodB, but brighter
#declare T_Wood33=
texture {
    pigment {
        wood turbulence 0.0425
        scale 0.2
        color_map {
            [0.05 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.15 color rgbt <0.32549, 0.13725, 0.00000, 0.0>]
            [0.25 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.35 color rgbt <0.32549, 0.11765, 0.00000, 0.0>]
            [0.48 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.55 color rgbt <0.29412, 0.13725, 0.01176, 0.0>]
            [0.65 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.78 color rgbt <0.32549, 0.13725, 0.00000, 0.0>]
            [0.85 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.96 color rgbt <0.28627, 0.13725, 0.00000, 0.0>]
            [1.00 color rgbt <0.54510, 0.17647, 0.03529, 0.0>]
        }
        translate <.015, 0, 0>
    }
}

// Reddish-orange, large, smooth grain.
#declare T_Wood34 =
texture { T_Wood32 }                  // opaque under-layer
texture  {
    pigment {
        onion
        turbulence 0.2125
        colour_map {
            [0.225 colour rgbt <1.000000, 0.53333, 0.11767, 0.4875>]
            [0.350 colour rgbt <0.662750, 0.28617, 0.00001, 0.7250>]
            [0.500 colour rgbt <1.000100, 0.53333, 0.11765, 0.5745>]
            [0.625 colour rgbt <0.662775, 0.28627, 0.00005, 0.6875>]
            [0.750 colour rgbt <1.000200, 0.53333, 0.11755, 0.5275>]
            [0.875 colour rgbt <0.662755, 0.28629, 0.00001, 0.3795>]
            [1.000 colour rgbt <1.000000, 0.53333, 0.11665, 0.6165>]
        }
        scale <0.225, 0.20, 1.15>
    }
}


// Orangish, with a grain more like a veneer than a plank
#declare T_Wood35=
texture {
    pigment {
        wood
        turbulence 0.03725
        omega 0.65725
        lambda 2.425
        color_map {
            [0.250 color rgbt <1.00000, 0.53373, 0.11665, 0.000>]
            [0.350 color rgbt <0.66275, 0.28607, 0.00000, 0.000>]
            [0.525 color rgbt <1.00000, 0.53363, 0.11715, 0.000>]
            [0.600 color rgbt <0.66475, 0.28647, 0.00000, 0.000>]
            [0.750 color rgbt <1.00000, 0.53353, 0.11565, 0.000>]
            [0.850 color rgbt <0.66275, 0.28667, 0.00000, 0.000>]
            [1.000 color rgbt <1.00000, 0.53143, 0.11795, 0.000>]
        }
        scale <0.25, 0.225, 1.0>
    }
}
texture {
    pigment {
        wood
        scale 1.01275
        turbulence 0.0435
        omega 0.65
        lambda 3.15
        color_map {
            [0.200 color rgbt <0.56695, 0.17347, 0.00000, 0.8250>]
            [0.350 color rgbt <0.96471, 0.54510, 0.22753, 0.7710>]
            [0.400 color rgbt <0.56341, 0.17547, 0.00000, 0.9150>]
            [0.615 color rgbt <0.96472, 0.54510, 0.22553, 0.7590>]
            [0.700 color rgbt <0.56671, 0.17687, 0.00000, 0.7920>]
            [0.850 color rgbt <0.96485, 0.54510, 0.22453, 0.8975>]
            [1.000 color rgbt <0.56478, 0.17247, 0.00000, 0.9750>]
        }
        scale <0.225, 0.2725, 1.0>
        translate <-0.35, 0.095, 1.25>
    }
}

#version Woods_Inc_Temp;
#end
