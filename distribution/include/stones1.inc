// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: stones1.inc
//    Last updated: 2001.7.24
//    Description:

#ifndef(Stones1_Inc_Temp)
#declare Stones1_Inc_Temp = version;
#version 3.5;

/*     T_Stone1 through T_Stone24 created by Mike Miller, 1992

    Contains declared texture statements defining a variety of
    stone granite & marble textures. Most use the granite texture.
    Turbulence has no effect on granite, but turbulence is stated
    before the color map for convenience of switching to marble, which
    does need a turbulence to swirl the color. I tried to avoid using
    a random dither, but I find it helps create a subtle grain & can
    spark lost colors in the map. On multi-layered texture, try a float
    of about 0.05 on the first texture.

    Final Note: I would not "INCLUDE" this whole data file into a scene
                file if only one or two of these textures were being
                used...There are too many declares being used.

    -----------------------------------------------------------------------
    The textures T_Grnt0-T_Grnt29, T_Grnt0A-T_Grnt24A and Crack1-Crack4 are
    "building blocks" that are used to create the final "usable" textures,
    T_Stone1 - T_Stone24 (and other textures that *you* design, of course!)

    INDEX:

            T_Grnt0  - T_Grnt29   color maps (generally) contain no transmit values
            ------------------------------
                T_Grnt0  - Gray/Tan with Rose.
                T_Grnt1  - Creamy Whites with Yellow & Light Gray.
                T_Grnt2  - Deep Cream with Light Rose, Yellow, Orchid, & Tan.
                T_Grnt3  - Warm tans olive & light rose with cream.
                T_Grnt4  - Orchid, Sand & Mauve.
                T_Grnt5  - Medium Mauve Med.Rose & Deep Cream.
                T_Grnt6  - Med. Orchid, Olive & Dark Tan "mud pie".
                T_Grnt7  - Dark Orchid, Olive & Dark Putty.
                T_Grnt8  - Rose & Light Cream Yellows
                T_Grnt9  - Light Steely Grays
                T_Grnt10 - Gray Creams & Lavender Tans
                T_Grnt11 - Creams & Grays  Kahki
                T_Grnt12 - Tan Cream & Red Rose
                T_Grnt13 - Cream Rose Orange
                T_Grnt14 - Cream Rose & Light Moss w/Light Violet
                T_Grnt15 - Black with subtle chroma
                T_Grnt16 - White Cream & Peach
                T_Grnt17 - Bug Juice & Green
                T_Grnt18 - Rose & Creamy Yellow
                T_Grnt19 - Gray Marble with White feather Viens
                T_Grnt20 - White Marble with Gray feather Viens
                T_Grnt21 - Green Jade
                T_Grnt22 - Clear with White feather Viens (has some transparency)
                T_Grnt23 - Light Tan to Mauve
                T_Grnt24 - Light Grays
                T_Grnt25 - Moss Greens & Tan
                T_Grnt26 - Salmon with thin Green Viens
                T_Grnt27 - Dark Green & Browns
                T_Grnt28 - Red Swirl
                T_Grnt29 - White, Tan, w/ thin Red Viens

            T_Grnt0A - T_Grnt24A  color maps containing transmit
            ------------------------------
                T_Grnt0a  - Translucent T_Grnt0
                T_Grnt1a  - Translucent T_Grnt1
                T_Grnt2a  - Translucent T_Grnt2
                T_Grnt3a  - Translucent T_Grnt3
                T_Grnt4a  - Translucent T_Grnt4
                T_Grnt5a  - Translucent T_Grnt5
                T_Grnt6a  - Translucent T_Grnt6
                T_Grnt7a  - Translucent T_Grnt7
                T_Grnt8a  - Aqua Tints
                T_Grnt9a  - Transmit Creams With Cracks
                T_Grnt10a - Transmit Cream Rose & light yellow
                T_Grnt11a - Transmit Light Grays
                T_Grnt12a - Transmit Creams & Tans
                T_Grnt13a - Transmit Creams & Grays
                T_Grnt14a - Cream Rose & light moss
                T_Grnt15a - Transmit Sand & light Orange
                T_Grnt16a - Cream Rose & light moss (again?)
                T_Grnt17a - ???
                T_Grnt18a - ???
                T_Grnt19a - Gray Marble with White feather Viens with Transmit
                T_Grnt20a - White Feather Viens
                T_Grnt21a - Thin White Feather Viens
                T_Grnt22a - ???
                T_Grnt23a - Transparent Green Moss
                T_Grnt24a - ???

            T_Crack1 - T_Crack4   clear with an thin opaque band for T_Cracks
            ------------------------------
                T_Crack1 - T_Crack & Red Overtint
                T_Crack2 - Translucent Dark T_Cracks
                T_Crack3 - Overtint Green w/ Black T_Cracks
                T_Crack4 - Overtint w/ White T_Crack

            OTHERS

            Stone1 - Stone24  complete texture statements - edit to your
                              scene & lighting situations.
            ------------------------------
                T_Stone1 - Deep Rose & Green Marble with large White Swirls
                T_Stone2 - Light Greenish Tan Marble with Agate style veining
                T_Stone3 - Rose & Yellow Marble with fog white veining
                T_Stone4 - Tan Marble with Rose patches
                T_Stone5 - White Cream Marble with Pink veining
                T_Stone6 - Rose & Yellow Cream Marble
                T_Stone7 - Light Coffee Marble with darker patches
                T_Stone8 - Gray Granite with white patches
                T_Stone9 - White & Light Blue Marble with light violets
                T_Stone10- Dark Brown & Tan swirl Granite with gray undertones
                T_Stone11- Rose & White Marble with dark tan swirl
                T_Stone12- White & Pinkish Tan Marble
                T_Stone13- Medium Gray Blue Marble
                T_Stone14- Tan & Olive Marble with gray white veins
                T_Stone15- Deep Gray Marble with white veining
                T_Stone16- Peach & Yellow Marble with white veining
                T_Stone17- White Marble with gray veining
                T_Stone18- Green Jade with white veining
                T_Stone19- Peach Granite with white patches & green trim
                T_Stone20- Brown & Olive Marble with white veining
                T_Stone21- Red Marble with gray & white veining
                T_Stone22- Dark Tan Marble with gray & white veining
                T_Stone23- Peach & Cream Marble with orange veining
                T_Stone24- Green & Tan Moss Marble

    -----------------------------------------------------------------------
    This file was modified July 2001 to work as it should with POV-Ray 3.5,
all filters being changed to transmits.
*/

#ifdef(View_POV_Include_Stack)
    #debug "including stones1.inc\n"
#end

//--------- Gray Tan with Rose
#declare T_Grnt0 =
texture {
pigment
 {granite
  turbulence 0.4
  color_map
   {[0.000, 0.153   color rgbt <0.729, 0.502, 0.451, 0.000>
                    color rgbt <0.769, 0.686, 0.592, 0.000>]
    [0.153, 0.398   color rgbt <0.769, 0.686, 0.592, 0.000>
                    color rgbt <0.843, 0.753, 0.718, 0.000>]
    [0.398, 0.559   color rgbt <0.843, 0.753, 0.718, 0.000>
                    color rgbt <0.780, 0.667, 0.561, 0.000>]
    [0.559, 0.729   color rgbt <0.780, 0.667, 0.561, 0.000>
                    color rgbt <0.741, 0.659, 0.576, 0.000>]
    [0.729, 1.001   color rgbt <0.741, 0.659, 0.576, 0.000>
                    color rgbt <0.729, 0.502, 0.451, 0.000>]
   }
 }
 }

//----- Creamy Whites with yellow & light gray
#declare T_Grnt1 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.212   color rgbt <0.898, 0.898, 0.851, 0.000>
                    color rgbt <0.969, 0.980, 0.875, 0.000>]
    [0.212, 0.424   color rgbt <0.969, 0.980, 0.875, 0.000>
                    color rgbt <0.859, 0.859, 0.859, 0.000>]
    [0.424, 0.627   color rgbt <0.859, 0.859, 0.859, 0.000>
                    color rgbt <0.992, 0.922, 0.659, 0.000>]
    [0.627, 0.881   color rgbt <0.992, 0.922, 0.659, 0.000>
                    color rgbt <0.937, 0.965, 0.902, 0.000>]
    [0.881, 1.001   color rgbt <0.937, 0.965, 0.902, 0.000>
                    color rgbt <0.898, 0.898, 0.851, 0.000>]
   }
 }
 }

//------- Deep Cream with light rose, yellow orchid & tan
#declare T_Grnt2 =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.178   color rgbt <0.863, 0.757, 0.596, 0.000>
                    color rgbt <0.925, 0.792, 0.714, 0.000>]
    [0.178, 0.356   color rgbt <0.925, 0.792, 0.714, 0.000>
                    color rgbt <0.871, 0.702, 0.659, 0.000>]
    [0.356, 0.525   color rgbt <0.871, 0.702, 0.659, 0.000>
                    color rgbt <0.992, 0.922, 0.659, 0.000>]
    [0.525, 0.729   color rgbt <0.992, 0.922, 0.659, 0.000>
                    color rgbt <0.902, 0.812, 0.714, 0.000>]
    [0.729, 1.001   color rgbt <0.902, 0.812, 0.714, 0.000>
                    color rgbt <0.863, 0.757, 0.596, 0.000>]
   }
 }
 }

//------- Warm tans olive & light rose with cream
#declare T_Grnt3 =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.178   color rgbt <0.831, 0.631, 0.569, 0.000>
                    color rgbt <0.925, 0.831, 0.714, 0.000>]
    [0.178, 0.356   color rgbt <0.925, 0.831, 0.714, 0.000>
                    color rgbt <0.871, 0.702, 0.659, 0.000>]
    [0.356, 0.525   color rgbt <0.871, 0.702, 0.659, 0.000>
                    color rgbt <0.831, 0.631, 0.569, 0.000>]
    [0.525, 0.729   color rgbt <0.831, 0.631, 0.569, 0.000>
                    color rgbt <0.937, 0.882, 0.820, 0.000>]
    [0.729, 1.001   color rgbt <0.937, 0.882, 0.820, 0.000>
                    color rgbt <0.831, 0.631, 0.569, 0.000>]
   }
 }
 }

//-------- Orchid sand & mouve
#declare T_Grnt4 =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.178   color rgbt <0.804, 0.569, 0.494, 0.000>
                    color rgbt <0.816, 0.725, 0.537, 0.000>]
    [0.178, 0.356   color rgbt <0.816, 0.725, 0.537, 0.000>
                    color rgbt <0.820, 0.580, 0.522, 0.000>]
    [0.356, 0.525   color rgbt <0.820, 0.580, 0.522, 0.000>
                    color rgbt <0.882, 0.725, 0.537, 0.000>]
    [0.525, 0.729   color rgbt <0.882, 0.725, 0.537, 0.000>
                    color rgbt <0.855, 0.729, 0.584, 0.000>]
    [0.729, 1.001   color rgbt <0.855, 0.729, 0.584, 0.000>
                    color rgbt <0.804, 0.569, 0.494, 0.000>]
   }
 }
 }

//------- Medium Mauve Med.Rose & deep cream
#declare T_Grnt5 =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.178   color rgbt <0.804, 0.569, 0.494, 0.000>
                    color rgbt <0.855, 0.729, 0.584, 0.000>]
    [0.178, 0.356   color rgbt <0.855, 0.729, 0.584, 0.000>
                    color rgbt <0.667, 0.502, 0.478, 0.000>]
    [0.356, 0.525   color rgbt <0.667, 0.502, 0.478, 0.000>
                    color rgbt <0.859, 0.624, 0.545, 0.000>]
    [0.525, 0.729   color rgbt <0.859, 0.624, 0.545, 0.000>
                    color rgbt <0.855, 0.729, 0.584, 0.000>]
    [0.729, 1.001   color rgbt <0.855, 0.729, 0.584, 0.000>
                    color rgbt <0.804, 0.569, 0.494, 0.000>]
   }
 }
 }

//--------- Med. Orchid Olive & Dark Tan "mud pie"
#declare T_Grnt6 =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.153   color rgbt <0.545, 0.380, 0.345, 0.000>
                    color rgbt <0.588, 0.475, 0.333, 0.000>]
    [0.153, 0.398   color rgbt <0.588, 0.475, 0.333, 0.000>
                    color rgbt <0.675, 0.478, 0.404, 0.000>]
    [0.398, 0.559   color rgbt <0.675, 0.478, 0.404, 0.000>
                    color rgbt <0.757, 0.635, 0.522, 0.000>]
    [0.559, 0.729   color rgbt <0.757, 0.635, 0.522, 0.000>
                    color rgbt <0.659, 0.549, 0.443, 0.000>]
    [0.729, 1.001   color rgbt <0.659, 0.549, 0.443, 0.000>
                    color rgbt <0.545, 0.380, 0.345, 0.000>]
   }
 }
 }

//------- Dark Orchid Olive & Dark Putty
#declare T_Grnt7 =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.153   color rgbt <0.439, 0.310, 0.282, 0.000>
                    color rgbt <0.463, 0.369, 0.259, 0.000>]
    [0.153, 0.398   color rgbt <0.463, 0.369, 0.259, 0.000>
                    color rgbt <0.541, 0.369, 0.298, 0.000>]
    [0.398, 0.559   color rgbt <0.541, 0.369, 0.298, 0.000>
                    color rgbt <0.573, 0.424, 0.286, 0.000>]
    [0.559, 0.729   color rgbt <0.573, 0.424, 0.286, 0.000>
                    color rgbt <0.494, 0.396, 0.306, 0.000>]
    [0.729, 1.001   color rgbt <0.494, 0.396, 0.306, 0.000>
                    color rgbt <0.439, 0.310, 0.282, 0.000>]
   }
 }
 }

//--------- Rose & Light cream Yellows
#declare T_Grnt8 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.179   color rgbt <0.843, 0.655, 0.655, 0.000>
                    color rgbt <0.886, 0.769, 0.627, 0.000>]
    [0.179, 0.368   color rgbt <0.886, 0.769, 0.627, 0.000>
                    color rgbt <0.906, 0.820, 0.714, 0.000>]
    [0.368, 0.538   color rgbt <0.906, 0.820, 0.714, 0.000>
                    color rgbt <0.851, 0.671, 0.671, 0.000>]
    [0.538, 0.846   color rgbt <0.851, 0.671, 0.671, 0.000>
                    color rgbt <0.890, 0.792, 0.675, 0.000>]
    [0.846, 0.983   color rgbt <0.890, 0.792, 0.675, 0.000>
                    color rgbt <0.827, 0.612, 0.612, 0.000>]
    [0.983, 1.001   color rgbt <0.827, 0.612, 0.612, 0.000>
                    color rgbt <0.843, 0.655, 0.655, 0.000>]
   }
 }
 }

//--------- Light Steely Grays
#declare T_Grnt9 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.894, 0.886, 0.886, 0.000>
                    color rgbt <0.745, 0.745, 0.753, 0.000>]
    [0.154, 0.308   color rgbt <0.745, 0.745, 0.753, 0.000>
                    color rgbt <0.902, 0.902, 0.859, 0.000>]
    [0.308, 0.444   color rgbt <0.902, 0.902, 0.859, 0.000>
                    color rgbt <0.729, 0.706, 0.694, 0.000>]
    [0.444, 0.615   color rgbt <0.729, 0.706, 0.694, 0.000>
                    color rgbt <0.588, 0.592, 0.635, 0.000>]
    [0.615, 0.803   color rgbt <0.588, 0.592, 0.635, 0.000>
                    color rgbt <0.608, 0.616, 0.659, 0.000>]
    [0.803, 1.001   color rgbt <0.608, 0.616, 0.659, 0.000>
                    color rgbt <0.894, 0.886, 0.886, 0.000>]
   }
 }
 }

//--------- Gray Creams & lavender tans
#declare T_Grnt10 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.890, 0.690, 0.690, 0.000>
                    color rgbt <0.996, 0.835, 0.737, 0.000>]
    [0.154, 0.308   color rgbt <0.996, 0.835, 0.737, 0.000>
                    color rgbt <0.745, 0.635, 0.651, 0.004>]
    [0.308, 0.444   color rgbt <0.745, 0.635, 0.651, 0.004>
                    color rgbt <0.733, 0.596, 0.557, 0.004>]
    [0.444, 0.615   color rgbt <0.733, 0.596, 0.557, 0.004>
                    color rgbt <0.996, 0.835, 0.737, 0.000>]
    [0.615, 0.803   color rgbt <0.996, 0.835, 0.737, 0.000>
                    color rgbt <0.765, 0.616, 0.659, 0.000>]
    [0.803, 1.001   color rgbt <0.765, 0.616, 0.659, 0.000>
                    color rgbt <0.890, 0.690, 0.690, 0.000>]
   }
 }
 }

//--------- Creams & Grays Kakhi
#declare T_Grnt11 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.800, 0.651, 0.557, 0.000>
                    color rgbt <0.996, 0.835, 0.737, 0.000>]
    [0.154, 0.308   color rgbt <0.996, 0.835, 0.737, 0.000>
                    color rgbt <0.800, 0.651, 0.557, 0.000>]
    [0.308, 0.444   color rgbt <0.800, 0.651, 0.557, 0.000>
                    color rgbt <0.694, 0.624, 0.604, 0.004>]
    [0.444, 0.615   color rgbt <0.694, 0.624, 0.604, 0.004>
                    color rgbt <0.800, 0.651, 0.557, 0.000>]
    [0.615, 0.812   color rgbt <0.800, 0.651, 0.557, 0.000>
                    color rgbt <0.725, 0.655, 0.651, 0.000>]
    [0.812, 1.001   color rgbt <0.725, 0.655, 0.651, 0.000>
                    color rgbt <0.800, 0.651, 0.557, 0.000>]
   }
 }
 }

//--------- Tan Cream & Red Rose
#declare T_Grnt12 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.996, 0.969, 0.800, 0.000>
                    color rgbt <0.996, 0.682, 0.604, 0.000>]
    [0.154, 0.308   color rgbt <0.996, 0.682, 0.604, 0.000>
                    color rgbt <0.906, 0.820, 0.714, 0.000>]
    [0.308, 0.444   color rgbt <0.906, 0.820, 0.714, 0.000>
                    color rgbt <0.816, 0.631, 0.537, 0.000>]
    [0.444, 0.615   color rgbt <0.816, 0.631, 0.537, 0.000>
                    color rgbt <0.890, 0.792, 0.675, 0.000>]
    [0.615, 0.812   color rgbt <0.890, 0.792, 0.675, 0.000>
                    color rgbt <0.973, 0.627, 0.627, 0.000>]
    [0.812, 1.001   color rgbt <0.973, 0.627, 0.627, 0.000>
                    color rgbt <0.996, 0.969, 0.800, 0.000>]
   }
 }
 }

//--------- Cream Rose orange
#declare T_Grnt13 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.996, 0.824, 0.780, 0.000>
                    color rgbt <0.996, 0.698, 0.624, 0.000>]
    [0.154, 0.308   color rgbt <0.996, 0.698, 0.624, 0.000>
                    color rgbt <0.906, 0.675, 0.553, 0.000>]
    [0.308, 0.444   color rgbt <0.906, 0.675, 0.553, 0.000>
                    color rgbt <0.996, 0.682, 0.604, 0.000>]
    [0.444, 0.615   color rgbt <0.996, 0.682, 0.604, 0.000>
                    color rgbt <0.996, 0.824, 0.780, 0.000>]
    [0.615, 0.812   color rgbt <0.996, 0.824, 0.780, 0.000>
                    color rgbt <0.973, 0.627, 0.627, 0.000>]
    [0.812, 1.001   color rgbt <0.973, 0.627, 0.627, 0.000>
                    color rgbt <0.996, 0.824, 0.780, 0.000>]
   }
 }
 }

//--------- Cream Rose & light moss & light Violet
#declare T_Grnt14 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.690, 0.612, 0.569, 0.000>
                    color rgbt <0.737, 0.596, 0.522, 0.000>]
    [0.154, 0.368   color rgbt <0.737, 0.596, 0.522, 0.000>
                    color rgbt <0.776, 0.702, 0.624, 0.000>]
    [0.368, 0.538   color rgbt <0.776, 0.702, 0.624, 0.000>
                    color rgbt <0.796, 0.678, 0.643, 0.000>]
    [0.538, 0.846   color rgbt <0.796, 0.678, 0.643, 0.000>
                    color rgbt <0.690, 0.612, 0.569, 0.000>]
    [0.846, 0.932   color rgbt <0.690, 0.612, 0.569, 0.000>
                    color rgbt <0.773, 0.612, 0.569, 0.000>]
    [0.932, 1.001   color rgbt <0.773, 0.612, 0.569, 0.000>
                    color rgbt <0.690, 0.612, 0.569, 0.000>]
   }
 }
 }

//--------- Black with subtle chroma
#declare T_Grnt15 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.104   color rgbt <0.161, 0.133, 0.118, 0.000>
                    color rgbt <0.110, 0.082, 0.071, 0.000>]
    [0.104, 0.252   color rgbt <0.110, 0.082, 0.071, 0.000>
                    color rgbt <0.161, 0.133, 0.118, 0.000>]
    [0.252, 0.383   color rgbt <0.161, 0.133, 0.118, 0.000>
                    color rgbt <0.000, 0.000, 0.000, 0.000>]
    [0.383, 0.643   color rgbt <0.000, 0.000, 0.000, 0.000>
                    color rgbt <0.161, 0.133, 0.118, 0.000>]
    [0.643, 0.783   color rgbt <0.161, 0.133, 0.118, 0.000>
                    color rgbt <0.220, 0.149, 0.137, 0.000>]
    [0.783, 0.922   color rgbt <0.220, 0.149, 0.137, 0.000>
                    color rgbt <0.000, 0.000, 0.000, 0.000>]
    [0.922, 0.983   color rgbt <0.000, 0.000, 0.000, 0.000>
                    color rgbt <0.220, 0.149, 0.137, 0.000>]
    [0.983, 1.001   color rgbt <0.220, 0.149, 0.137, 0.000>
                    color rgbt <0.161, 0.133, 0.118, 0.000>]
   }
 }
 }

//----- White Cream & Peach
#declare T_Grnt16 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.316   color rgbt <0.910, 0.788, 0.788, 0.000>
                    color rgbt <0.922, 0.914, 0.871, 0.000>]
    [0.316, 0.453   color rgbt <0.922, 0.914, 0.871, 0.000>
                    color rgbt <0.894, 0.867, 0.780, 0.000>]
    [0.453, 0.624   color rgbt <0.894, 0.867, 0.780, 0.000>
                    color rgbt <0.784, 0.788, 0.788, 0.000>]
    [0.624, 0.726   color rgbt <0.784, 0.788, 0.788, 0.000>
                    color rgbt <0.851, 0.812, 0.741, 0.000>]
    [0.726, 0.863   color rgbt <0.851, 0.812, 0.741, 0.000>
                    color rgbt <0.647, 0.655, 0.655, 0.000>]
    [0.863, 1.001   color rgbt <0.647, 0.655, 0.655, 0.000>
                    color rgbt <0.910, 0.788, 0.788, 0.000>]
   }
 }
 }

//----- Bug Juice & Green
#declare T_Grnt17 =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.303   color rgbt <0.000, 0.239, 0.000, 0.000>
                    color rgbt <0.333, 0.294, 0.000, 0.000>]
    [0.303, 0.588   color rgbt <0.333, 0.294, 0.000, 0.000>
                    color rgbt <0.000, 0.239, 0.341, 0.000>]
    [0.588, 0.790   color rgbt <0.000, 0.239, 0.341, 0.000>
                    color rgbt <0.000, 0.020, 0.000, 0.000>]
    [0.790, 1.001   color rgbt <0.000, 0.020, 0.000, 0.000>
                    color rgbt <0.000, 0.239, 0.000, 0.000>]
   }
 }
 }

//------------ Rose & cream yellow
#declare T_Grnt18 =
texture {
pigment
 {granite
  turbulence 0.4
  color_map
   {[0.000, 0.202   color rgbt <1.000, 0.718, 0.541, 0.000>
                    color rgbt <0.890, 0.651, 0.612, 0.000>]
    [0.202, 0.298   color rgbt <0.890, 0.651, 0.612, 0.000>
                    color rgbt <1.000, 0.820, 0.675, 0.000>]
    [0.298, 0.377   color rgbt <1.000, 0.820, 0.675, 0.000>
                    color rgbt <0.890, 0.643, 0.612, 0.000>]
    [0.377, 0.465   color rgbt <0.890, 0.643, 0.612, 0.000>
                    color rgbt <0.937, 0.729, 0.561, 0.000>]
    [0.465, 0.544   color rgbt <0.937, 0.729, 0.561, 0.000>
                    color rgbt <0.878, 0.604, 0.565, 0.000>]
    [0.544, 0.640   color rgbt <0.878, 0.604, 0.565, 0.000>
                    color rgbt <0.984, 0.780, 0.655, 0.000>]
    [0.640, 0.860   color rgbt <0.984, 0.780, 0.655, 0.000>
                    color rgbt <1.000, 0.863, 0.635, 0.000>]
    [0.860, 0.982   color rgbt <1.000, 0.863, 0.635, 0.000>
                    color rgbt <1.000, 0.765, 0.620, 0.000>]
    [0.982, 1.001   color rgbt <1.000, 0.765, 0.620, 0.000>
                    color rgbt <1.000, 0.718, 0.541, 0.000>]
   }
 }
 }

//--------- Gray Marble with White feather Viens
#declare T_Grnt19 =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.3 color White color DimGray]
    [0.3, 0.4 color DimGray color DimGray]
    [0.4, 0.6 color DimGray color DimGray]
    [0.6, 1.0 color DimGray color DimGray]
   }
 }
 finish {
  crand 0.02
 }
 }

//--------- White Marble with Gray feather Viens
#declare T_Grnt20 =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.3 color Mica color White]
    [0.3, 0.4 color White color White]
    [0.4, 0.6 color White color White]
    [0.6, 1.0 color White color White]
   }
 }
 finish {
  crand 0.02
 }
 }

//-------- Declare Green Colors
#declare g1 = color red 0.26 green 0.41 blue 0.31; //---Light Gray Green
#declare g2 = color red 0.27 green 0.34 blue 0.26; //---Med Gray Green
#declare g3 = color red 0.13 green 0.29 blue 0.28; //---Med Gray Aqua
#declare g4 = color red 0.03 green 0.18 blue 0.08; //---Dark Green

//--------- Green Jade
#declare T_Grnt21 =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.1 color White    transmit 0.3 color SeaGreen  transmit 0.4]
    [0.1, 0.3 color SeaGreen transmit 0.4 color g2        transmit 0.7]
    [0.3, 0.5 color g2       transmit 0.7 color DarkGreen transmit 0.7]
    [0.5, 0.7 color DarkGreen  transmit 0.7 color g4 transmit 0.7]
    [0.7, 0.8 color g4         transmit 0.7 color DarkGreen transmit 0.7]
    [0.8, 1.0 color DarkGreen  transmit 0.7 color DarkGreen transmit 0.7]
   }
 }
 finish {
  crand 0.02
 }
 }

//--------- Clear with White feather Viens ----- This one does contain Transmit
#declare T_Grnt22 =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.07 color White color White]
    [0.07, 0.2 color White color DimGray]
    [0.2, 0.3 color DimGray color Clear]
    [0.3, 0.7 color Clear color Clear]
    [0.7, 1.0 color Clear color DimGray]
   }
 }
 finish {
  crand 0.02
 }
 }

//---------- Light Tan to Mouve
#declare T_Grnt23 =
texture {
pigment
 {marble
  turbulence 0.5
  color_map
   {[0.000, 0.178   color rgbt <0.831, 0.631, 0.569, 0.000>
                    color rgbt <0.925, 0.831, 0.714, 0.000>]
    [0.178, 0.356   color rgbt <0.925, 0.831, 0.714, 0.000>
                    color rgbt <0.871, 0.702, 0.659, 0.000>]
    [0.356, 0.525   color rgbt <0.871, 0.702, 0.659, 0.000>
                    color rgbt <0.831, 0.631, 0.569, 0.000>]
    [0.525, 0.729   color rgbt <0.831, 0.631, 0.569, 0.000>
                    color rgbt <0.937, 0.882, 0.820, 0.000>]
    [0.729, 1.001   color rgbt <0.937, 0.882, 0.820, 0.000>
                    color rgbt <0.831, 0.631, 0.569, 0.000>]
   }
 }
 }

//--------- Light Grays
#declare T_Grnt24 =
texture {
pigment
 {marble
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.894, 0.886, 0.886, 0.000>
                    color rgbt <0.745, 0.745, 0.753, 0.000>]
    [0.154, 0.308   color rgbt <0.745, 0.745, 0.753, 0.000>
                    color rgbt <0.902, 0.902, 0.859, 0.000>]
    [0.308, 0.444   color rgbt <0.902, 0.902, 0.859, 0.000>
                    color rgbt <0.729, 0.706, 0.694, 0.000>]
    [0.444, 0.615   color rgbt <0.729, 0.706, 0.694, 0.000>
                    color rgbt <0.588, 0.592, 0.635, 0.000>]
    [0.615, 0.803   color rgbt <0.588, 0.592, 0.635, 0.000>
                    color rgbt <0.608, 0.616, 0.659, 0.000>]
    [0.803, 1.001   color rgbt <0.608, 0.616, 0.659, 0.000>
                    color rgbt <0.894, 0.886, 0.886, 0.000>]
   }
 }
 }

//------------ Moss Greens & Tan
#declare T_Grnt25 =
texture {
pigment
 {marble
  turbulence 0.7
  color_map
   {[0.000, 0.168   color rgbt <0.824, 0.725, 0.584, 0.000>
                    color rgbt <0.514, 0.584, 0.533, 0.000>]
    [0.168, 0.301   color rgbt <0.514, 0.584, 0.533, 0.000>
                    color rgbt <0.298, 0.376, 0.318, 0.000>]
    [0.301, 0.398   color rgbt <0.298, 0.376, 0.318, 0.000>
                    color rgbt <0.263, 0.337, 0.282, 0.000>]
    [0.398, 0.558   color rgbt <0.263, 0.337, 0.282, 0.000>
                    color rgbt <0.431, 0.506, 0.451, 0.000>]
    [0.558, 0.655   color rgbt <0.431, 0.506, 0.451, 0.000>
                    color rgbt <0.529, 0.631, 0.471, 0.000>]
    [0.655, 0.735   color rgbt <0.529, 0.631, 0.471, 0.000>
                    color rgbt <0.333, 0.376, 0.318, 0.000>]
    [0.735, 0.823   color rgbt <0.333, 0.376, 0.318, 0.000>
                    color rgbt <0.298, 0.376, 0.318, 0.000>]
    [0.823, 0.876   color rgbt <0.298, 0.376, 0.318, 0.000>
                    color rgbt <0.416, 0.376, 0.318, 0.000>]
    [0.876, 0.929   color rgbt <0.416, 0.376, 0.318, 0.000>
                    color rgbt <0.416, 0.376, 0.318, 0.000>]
    [0.929, 1.001   color rgbt <0.416, 0.376, 0.318, 0.000>
                    color rgbt <0.824, 0.725, 0.584, 0.000>]
   }
 }
 }

//---------- Salmon with thin Green Viens
#declare T_Grnt26 =
texture {
pigment
 {granite
  color_map
   {[0.000, 0.241   color rgbt <0.973, 0.973, 0.976, 0.000>
                    color rgbt <0.973, 0.973, 0.976, 0.000>]
    [0.241, 0.284   color rgbt <0.973, 0.973, 0.976, 0.000>
                    color rgbt <0.600, 0.741, 0.608, 0.000>]
    [0.284, 0.336   color rgbt <0.600, 0.741, 0.608, 0.000>
                    color rgbt <0.820, 0.643, 0.537, 0.000>]
    [0.336, 0.474   color rgbt <0.820, 0.643, 0.537, 0.000>
                    color rgbt <0.886, 0.780, 0.714, 0.000>]
    [0.474, 0.810   color rgbt <0.886, 0.780, 0.714, 0.000>
                    color rgbt <0.996, 0.643, 0.537, 0.000>]
    [0.810, 0.836   color rgbt <0.996, 0.643, 0.537, 0.000>
                    color rgbt <0.973, 0.973, 0.976, 0.000>]
    [0.836, 1.001   color rgbt <0.973, 0.973, 0.976, 0.000>
                    color rgbt <0.973, 0.973, 0.976, 0.000>]
   }
 }
 finish {
  crand 0.02
 }
 }

//------ Dark Green & Browns
#declare T_Grnt27 =
texture {
pigment
 {granite
  color_map
   {[0.000, 0.043   color rgbt <0.773, 0.647, 0.569, 0.000>
                    color rgbt <0.431, 0.322, 0.227, 0.000>]
    [0.043, 0.113   color rgbt <0.431, 0.322, 0.227, 0.000>
                    color rgbt <0.278, 0.282, 0.216, 0.000>]
    [0.113, 0.304   color rgbt <0.278, 0.282, 0.216, 0.000>
                    color rgbt <0.278, 0.282, 0.216, 0.000>]
    [0.304, 0.426   color rgbt <0.278, 0.282, 0.216, 0.000>
                    color rgbt <0.459, 0.341, 0.243, 0.000>]
    [0.426, 0.843   color rgbt <0.459, 0.341, 0.243, 0.000>
                    color rgbt <0.459, 0.341, 0.243, 0.000>]
    [0.843, 0.878   color rgbt <0.459, 0.341, 0.243, 0.000>
                    color rgbt <0.459, 0.341, 0.243, 0.000>]
    [0.878, 0.983   color rgbt <0.459, 0.341, 0.243, 0.000>
                    color rgbt <0.278, 0.282, 0.216, 0.000>]
    [0.983, 1.001   color rgbt <0.278, 0.282, 0.216, 0.000>
                    color rgbt <0.773, 0.647, 0.569, 0.000>]
   }
 }
 }

//------- Red Swirl
#declare T_Grnt28 =
texture {
pigment
 {marble
  turbulence 0.7
  color_map
   {[0.000, 0.155   color rgbt <0.686, 0.235, 0.282, 0.000>
                    color rgbt <0.686, 0.235, 0.282, 0.000>]
    [0.155, 0.328   color rgbt <0.686, 0.235, 0.282, 0.000>
                    color rgbt <0.494, 0.243, 0.294, 0.000>]
    [0.328, 0.474   color rgbt <0.494, 0.243, 0.294, 0.000>
                    color rgbt <0.769, 0.329, 0.373, 0.000>]
    [0.474, 0.647   color rgbt <0.769, 0.329, 0.373, 0.000>
                    color rgbt <0.769, 0.329, 0.373, 0.000>]
    [0.647, 0.810   color rgbt <0.769, 0.329, 0.373, 0.000>
                    color rgbt <0.686, 0.235, 0.282, 0.000>]
    [0.810, 0.922   color rgbt <0.686, 0.235, 0.282, 0.000>
                    color rgbt <0.792, 0.388, 0.427, 0.000>]
    [0.922, 1.001   color rgbt <0.792, 0.388, 0.427, 0.000>
                    color rgbt <0.686, 0.235, 0.282, 0.000>]
   }
 }
 finish {
  crand 0.03
 }
 }

//-------- White Tan & thin Reds
#declare T_Grnt29 =
texture {
pigment
 {marble
  turbulence 0.5
  color_map
   {[0.000, 0.053   color rgbt <0.784, 0.627, 0.522, 0.000>
                    color rgbt <0.784, 0.627, 0.624, 0.000>]
    [0.053, 0.263   color rgbt <0.784, 0.627, 0.624, 0.000>
                    color rgbt <0.824, 0.557, 0.376, 0.000>]
    [0.263, 0.281   color rgbt <0.824, 0.557, 0.376, 0.000>
                    color rgbt <0.643, 0.380, 0.376, 0.000>]
    [0.281, 0.325   color rgbt <0.643, 0.380, 0.376, 0.000>
                    color rgbt <0.839, 0.722, 0.722, 0.000>]
    [0.325, 0.711   color rgbt <0.839, 0.722, 0.722, 0.000>
                    color rgbt <0.784, 0.627, 0.522, 0.000>]
    [0.711, 0.798   color rgbt <0.784, 0.627, 0.522, 0.000>
                    color rgbt <0.769, 0.380, 0.376, 0.000>]
    [0.798, 0.895   color rgbt <0.769, 0.380, 0.376, 0.000>
                    color rgbt <0.824, 0.557, 0.376, 0.000>]
    [0.895, 0.982   color rgbt <0.824, 0.557, 0.376, 0.000>
                    color rgbt <0.784, 0.627, 0.522, 0.000>]
    [0.982, 1.001   color rgbt <0.784, 0.627, 0.522, 0.000>
                    color rgbt <0.784, 0.627, 0.522, 0.000>]
   }
 }
 }

//***************************************************************
//------------ start of textures with transmit

//----- Translucent T_Grnt0
#declare T_Grnt0a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.153   color rgbt <0.729, 0.502, 0.451, 0.306>
                    color rgbt <0.769, 0.686, 0.592, 0.792>]
    [0.153, 0.398   color rgbt <0.769, 0.686, 0.592, 0.792>
                    color rgbt <0.843, 0.753, 0.718, 0.396>]
    [0.398, 0.559   color rgbt <0.843, 0.753, 0.718, 0.396>
                    color rgbt <0.780, 0.667, 0.561, 0.976>]
    [0.559, 0.729   color rgbt <0.780, 0.667, 0.561, 0.976>
                    color rgbt <0.741, 0.659, 0.576, 0.820>]
    [0.729, 1.001   color rgbt <0.741, 0.659, 0.576, 0.820>
                    color rgbt <0.729, 0.502, 0.451, 0.306>]
   }
 }
 }

//----- Translucent T_Grnt1
#declare T_Grnt1a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.212   color rgbt <0.898, 0.898, 0.851, 0.306>
                    color rgbt <0.969, 0.980, 0.875, 0.792>]
    [0.212, 0.424   color rgbt <0.969, 0.980, 0.875, 0.792>
                    color rgbt <0.859, 0.859, 0.859, 0.396>]
    [0.424, 0.627   color rgbt <0.859, 0.859, 0.859, 0.396>
                    color rgbt <0.992, 0.922, 0.659, 0.976>]
    [0.627, 0.881   color rgbt <0.992, 0.922, 0.659, 0.976>
                    color rgbt <0.937, 0.965, 0.902, 0.820>]
    [0.881, 1.001   color rgbt <0.937, 0.965, 0.902, 0.820>
                    color rgbt <0.898, 0.898, 0.851, 0.306>]
   }
 }
 }

//-----Translucent T_Grnt2
#declare T_Grnt2a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.144   color rgbt <0.863, 0.757, 0.596, 0.596>
                    color rgbt <0.925, 0.792, 0.714, 0.349>]
    [0.144, 0.288   color rgbt <0.925, 0.792, 0.714, 0.349>
                    color rgbt <0.871, 0.702, 0.659, 0.784>]
    [0.288, 0.644   color rgbt <0.871, 0.702, 0.659, 0.784>
                    color rgbt <0.992, 0.922, 0.659, 0.498>]
    [0.644, 0.983   color rgbt <0.992, 0.922, 0.659, 0.498>
                    color rgbt <0.902, 0.812, 0.714, 0.722>]
    [0.983, 1.001   color rgbt <0.902, 0.812, 0.714, 0.722>
                    color rgbt <0.863, 0.757, 0.596, 0.596>]
   }
 }
 }

//-----Translucent T_Grnt3
#declare T_Grnt3a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.153   color rgbt <0.831, 0.631, 0.569, 0.447>
                    color rgbt <0.925, 0.831, 0.714, 0.678>]
    [0.153, 0.297   color rgbt <0.925, 0.831, 0.714, 0.678>
                    color rgbt <0.871, 0.702, 0.659, 0.475>]
    [0.297, 0.441   color rgbt <0.871, 0.702, 0.659, 0.475>
                    color rgbt <0.831, 0.631, 0.569, 0.918>]
    [0.441, 0.763   color rgbt <0.831, 0.631, 0.569, 0.918>
                    color rgbt <0.937, 0.882, 0.820, 0.655>]
    [0.763, 1.001   color rgbt <0.937, 0.882, 0.820, 0.655>
                    color rgbt <0.831, 0.631, 0.569, 0.447>]
   }
 }
 }

//-----Translucent T_Grnt4
#declare T_Grnt4a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.144   color rgbt <0.804, 0.569, 0.494, 0.569>
                    color rgbt <0.816, 0.725, 0.537, 0.467>]
    [0.144, 0.449   color rgbt <0.816, 0.725, 0.537, 0.467>
                    color rgbt <0.820, 0.580, 0.522, 0.584>]
    [0.449, 0.568   color rgbt <0.820, 0.580, 0.522, 0.584>
                    color rgbt <0.882, 0.725, 0.537, 0.871>]
    [0.568, 0.754   color rgbt <0.882, 0.725, 0.537, 0.871>
                    color rgbt <0.855, 0.729, 0.584, 0.816>]
    [0.754, 1.001   color rgbt <0.855, 0.729, 0.584, 0.816>
                    color rgbt <0.804, 0.569, 0.494, 0.569>]
   }
 }
 }

//-----Translucent T_Grnt5
#declare T_Grnt5a =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.178   color rgbt <0.804, 0.569, 0.494, 0.569>
                    color rgbt <0.855, 0.729, 0.584, 0.467>]
    [0.178, 0.356   color rgbt <0.855, 0.729, 0.584, 0.467>
                    color rgbt <0.667, 0.502, 0.478, 0.584>]
    [0.356, 0.525   color rgbt <0.667, 0.502, 0.478, 0.584>
                    color rgbt <0.859, 0.624, 0.545, 0.871>]
    [0.525, 0.729   color rgbt <0.859, 0.624, 0.545, 0.871>
                    color rgbt <0.855, 0.729, 0.584, 0.816>]
    [0.729, 1.001   color rgbt <0.855, 0.729, 0.584, 0.816>
                    color rgbt <0.804, 0.569, 0.494, 0.569>]
   }
 }
 }

//-----Translucent T_Grnt6
#declare T_Grnt6a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.263   color rgbt <0.545, 0.380, 0.345, 0.733>
                    color rgbt <0.588, 0.475, 0.333, 0.741>]
    [0.263, 0.432   color rgbt <0.588, 0.475, 0.333, 0.741>
                    color rgbt <0.675, 0.478, 0.404, 0.545>]
    [0.432, 0.551   color rgbt <0.675, 0.478, 0.404, 0.545>
                    color rgbt <0.757, 0.635, 0.522, 0.384>]
    [0.551, 0.720   color rgbt <0.757, 0.635, 0.522, 0.384>
                    color rgbt <0.659, 0.549, 0.443, 0.675>]
    [0.720, 1.001   color rgbt <0.659, 0.549, 0.443, 0.675>
                    color rgbt <0.545, 0.380, 0.345, 0.733>]
   }
 }
 }

//-----Translucent T_Grnt7
#declare T_Grnt7a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.119   color rgbt <0.439, 0.310, 0.282, 0.631>
                    color rgbt <0.463, 0.369, 0.259, 0.847>]
    [0.119, 0.322   color rgbt <0.463, 0.369, 0.259, 0.847>
                    color rgbt <0.541, 0.369, 0.298, 0.549>]
    [0.322, 0.449   color rgbt <0.541, 0.369, 0.298, 0.549>
                    color rgbt <0.573, 0.424, 0.286, 0.965>]
    [0.449, 0.729   color rgbt <0.573, 0.424, 0.286, 0.965>
                    color rgbt <0.494, 0.396, 0.306, 0.741>]
    [0.729, 1.001   color rgbt <0.494, 0.396, 0.306, 0.741>
                    color rgbt <0.439, 0.310, 0.282, 0.631>]
   }
 }
 }

//-----Aqua Tints
#declare T_Grnt8a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.119   color rgbt <0.310, 0.384, 0.420, 0.631>
                    color rgbt <0.322, 0.369, 0.416, 0.847>]
    [0.119, 0.322   color rgbt <0.322, 0.369, 0.416, 0.847>
                    color rgbt <0.424, 0.369, 0.420, 0.549>]
    [0.322, 0.449   color rgbt <0.424, 0.369, 0.420, 0.549>
                    color rgbt <0.373, 0.424, 0.518, 0.965>]
    [0.449, 0.729   color rgbt <0.373, 0.424, 0.518, 0.965>
                    color rgbt <0.482, 0.573, 0.533, 0.741>]
    [0.729, 1.001   color rgbt <0.482, 0.573, 0.533, 0.741>
                    color rgbt <0.310, 0.384, 0.420, 0.631>]
   }
 }
 }

//-----Transmit Creams With T_Cracks
#declare T_Grnt9a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.216   color rgbt <0.812, 0.812, 0.812, 0.835>
                    color rgbt <0.745, 0.843, 0.835, 0.847>]
    [0.216, 0.241   color rgbt <0.745, 0.843, 0.835, 0.847>
                    color rgbt <0.404, 0.337, 0.337, 0.463>]
    [0.241, 0.267   color rgbt <0.404, 0.337, 0.337, 0.463>
                    color rgbt <0.773, 0.729, 0.745, 0.622>]
    [0.267, 0.759   color rgbt <0.773, 0.729, 0.745, 0.622>
                    color rgbt <0.914, 0.843, 0.725, 0.651>]
    [0.759, 0.784   color rgbt <0.914, 0.843, 0.725, 0.651>
                    color rgbt <0.153, 0.133, 0.208, 0.437>]
    [0.784, 0.810   color rgbt <0.153, 0.133, 0.208, 0.437>
                    color rgbt <0.812, 0.812, 0.812, 0.835>]
    [0.810, 1.001   color rgbt <0.812, 0.812, 0.812, 0.835>
                    color rgbt <0.812, 0.812, 0.812, 0.835>]
   }
 }
 }

//--------- Transmit Cream Rose & light yellow
#declare T_Grnt10a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.179   color rgbt <0.843, 0.655, 0.655, 0.455>
                    color rgbt <0.886, 0.769, 0.627, 0.608>]
    [0.179, 0.368   color rgbt <0.886, 0.769, 0.627, 0.608>
                    color rgbt <0.906, 0.820, 0.714, 0.392>]
    [0.368, 0.538   color rgbt <0.906, 0.820, 0.714, 0.392>
                    color rgbt <0.851, 0.671, 0.671, 0.659>]
    [0.538, 0.744   color rgbt <0.851, 0.671, 0.671, 0.659>
                    color rgbt <0.890, 0.792, 0.675, 0.392>]
    [0.744, 0.983   color rgbt <0.890, 0.792, 0.675, 0.392>
                    color rgbt <0.827, 0.612, 0.612, 0.706>]
    [0.983, 1.001   color rgbt <0.827, 0.612, 0.612, 0.706>
                    color rgbt <0.843, 0.655, 0.655, 0.455>]
   }
 }
 }

//--------- Transmit Light Grays
#declare T_Grnt11a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.894, 0.886, 0.886, 0.659>
                    color rgbt <0.745, 0.745, 0.753, 0.584>]
    [0.154, 0.308   color rgbt <0.745, 0.745, 0.753, 0.584>
                    color rgbt <0.902, 0.902, 0.859, 0.780>]
    [0.308, 0.444   color rgbt <0.902, 0.902, 0.859, 0.780>
                    color rgbt <0.729, 0.706, 0.694, 0.686>]
    [0.444, 0.615   color rgbt <0.729, 0.706, 0.694, 0.686>
                    color rgbt <0.588, 0.592, 0.635, 0.424>]
    [0.615, 0.803   color rgbt <0.588, 0.592, 0.635, 0.424>
                    color rgbt <0.608, 0.616, 0.659, 0.761>]
    [0.803, 1.001   color rgbt <0.608, 0.616, 0.659, 0.761>
                    color rgbt <0.894, 0.886, 0.886, 0.659>]
   }
 }
 }

//--------- Transmit Creams & Tans
#declare T_Grnt12a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.890, 0.690, 0.690, 0.659>
                    color rgbt <0.996, 0.835, 0.737, 0.659>]
    [0.154, 0.308   color rgbt <0.996, 0.835, 0.737, 0.659>
                    color rgbt <0.745, 0.635, 0.651, 0.780>]
    [0.308, 0.444   color rgbt <0.745, 0.635, 0.651, 0.780>
                    color rgbt <0.733, 0.596, 0.557, 0.686>]
    [0.444, 0.615   color rgbt <0.733, 0.596, 0.557, 0.686>
                    color rgbt <0.996, 0.835, 0.737, 0.659>]
    [0.615, 0.803   color rgbt <0.996, 0.835, 0.737, 0.659>
                    color rgbt <0.765, 0.616, 0.659, 0.761>]
    [0.803, 1.001   color rgbt <0.765, 0.616, 0.659, 0.761>
                    color rgbt <0.890, 0.690, 0.690, 0.659>]
   }
 }
 }

//--------- Transmit Creams & Grays
#declare T_Grnt13a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.800, 0.651, 0.557, 0.000>
                    color rgbt <0.996, 0.835, 0.737, 0.608>]
    [0.154, 0.308   color rgbt <0.996, 0.835, 0.737, 0.608>
                    color rgbt <0.800, 0.651, 0.557, 0.635>]
    [0.308, 0.444   color rgbt <0.800, 0.651, 0.557, 0.635>
                    color rgbt <0.694, 0.624, 0.604, 0.294>]
    [0.444, 0.615   color rgbt <0.694, 0.624, 0.604, 0.294>
                    color rgbt <0.800, 0.651, 0.557, 0.816>]
    [0.615, 0.812   color rgbt <0.800, 0.651, 0.557, 0.816>
                    color rgbt <0.725, 0.655, 0.651, 0.957>]
    [0.812, 1.001   color rgbt <0.725, 0.655, 0.651, 0.957>
                    color rgbt <0.800, 0.651, 0.557, 0.000>]
   }
 }
 }

//--------- Cream Rose & light moss
#declare T_Grnt14a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.996, 0.969, 0.800, 0.373>
                    color rgbt <0.996, 0.682, 0.604, 0.412>]
    [0.154, 0.308   color rgbt <0.996, 0.682, 0.604, 0.412>
                    color rgbt <0.906, 0.820, 0.714, 0.616>]
    [0.308, 0.444   color rgbt <0.906, 0.820, 0.714, 0.616>
                    color rgbt <0.816, 0.631, 0.537, 0.443>]
    [0.444, 0.615   color rgbt <0.816, 0.631, 0.537, 0.443>
                    color rgbt <0.890, 0.792, 0.675, 0.745>]
    [0.615, 0.812   color rgbt <0.890, 0.792, 0.675, 0.745>
                    color rgbt <0.973, 0.627, 0.627, 0.600>]
    [0.812, 1.001   color rgbt <0.973, 0.627, 0.627, 0.600>
                    color rgbt <0.996, 0.969, 0.800, 0.373>]
   }
 }
 }

//--------- Transmit Sand & light Orange
#declare T_Grnt15a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.154   color rgbt <0.996, 0.824, 0.780, 0.412>
                    color rgbt <0.996, 0.698, 0.624, 0.412>]
    [0.154, 0.308   color rgbt <0.996, 0.698, 0.624, 0.412>
                    color rgbt <0.906, 0.675, 0.553, 0.616>]
    [0.308, 0.444   color rgbt <0.906, 0.675, 0.553, 0.616>
                    color rgbt <0.996, 0.682, 0.604, 0.412>]
    [0.444, 0.615   color rgbt <0.996, 0.682, 0.604, 0.412>
                    color rgbt <0.996, 0.824, 0.780, 0.412>]
    [0.615, 0.812   color rgbt <0.996, 0.824, 0.780, 0.412>
                    color rgbt <0.973, 0.627, 0.627, 0.600>]
    [0.812, 1.001   color rgbt <0.973, 0.627, 0.627, 0.600>
                    color rgbt <0.996, 0.824, 0.780, 0.412>]
   }
 }
 }

//--------- Cream Rose & light moss
#declare T_Grnt16a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.078   color rgbt <0.769, 0.722, 0.690, 0.180>
                    color rgbt <0.745, 0.690, 0.655, 1.000>]
    [0.078, 0.96    color rgbt <0.745, 0.690, 0.655, 1.000>
                    color rgbt <0.839, 0.804, 0.780, 1.000>]
    [0.96, 1.001    color rgbt <0.839, 0.804, 0.780, 0.278>
                    color rgbt <0.769, 0.722, 0.690, 0.180>]
   }
 }
 }

#declare T_Grnt17a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.034   color rgbt <0.027, 0.012, 0.012, 0.000>
                    color rgbt <0.851, 0.812, 0.741, 0.235>]
    [0.034, 0.342   color rgbt <0.851, 0.812, 0.741, 0.235>
                    color rgbt <0.792, 0.694, 0.690, 0.839>]
    [0.342, 0.462   color rgbt <0.792, 0.694, 0.690, 0.839>
                    color rgbt <0.631, 0.506, 0.471, 0.608>]
    [0.462, 0.632   color rgbt <0.631, 0.506, 0.471, 0.608>
                    color rgbt <0.851, 0.812, 0.741, 0.922>]
    [0.632, 0.983   color rgbt <0.851, 0.812, 0.741, 0.922>
                    color rgbt <0.647, 0.655, 0.655, 0.282>]
    [0.983, 1.001   color rgbt <0.647, 0.655, 0.655, 0.282>
                    color rgbt <0.027, 0.012, 0.012, 0.000>]
   }
 }
 }

#declare T_Grnt18a =
texture {
pigment
 {granite
  turbulence 0.6
  color_map
   {[0.000, 0.128   color rgbt <0.820, 0.580, 0.580, 0.000>
                    color rgbt <0.851, 0.812, 0.741, 0.235>]
    [0.128, 0.282   color rgbt <0.851, 0.812, 0.741, 0.235>
                    color rgbt <0.792, 0.694, 0.690, 0.282>]
    [0.282, 0.393   color rgbt <0.792, 0.694, 0.690, 0.282>
                    color rgbt <0.647, 0.655, 0.655, 0.133>]
    [0.393, 0.590   color rgbt <0.647, 0.655, 0.655, 0.133>
                    color rgbt <0.851, 0.812, 0.741, 0.333>]
    [0.590, 0.983   color rgbt <0.851, 0.812, 0.741, 0.333>
                    color rgbt <0.647, 0.655, 0.655, 0.282>]
    [0.983, 1.001   color rgbt <0.647, 0.655, 0.655, 0.282>
                    color rgbt <0.820, 0.580, 0.580, 0.000>]
   }
 }
 }

//--------- Gray Marble with White feather Viens with Transmit
#declare T_Grnt19a =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.3 color White transmit 0.0 color DimGray transmit 0.5]
    [0.3, 0.4 color DimGray transmit 0.5 color DimGray transmit 0.8]
    [0.4, 1.0 color DimGray transmit 0.8 color DimGray transmit 0.9]
   }
 }
 finish {
  crand 0.02
 }
 }

//--------- White Feature Viens
#declare T_Grnt20a =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.2 color White transmit 0.0 color White transmit 0.7]
    [0.2, 0.3 color White transmit 0.7 color Clear]
    [0.3, 1.0 color Clear color Clear]
   }
 }
 finish {
  crand 0.02
 }
 }

//--------- Thinner White Feature Viens
#declare T_Grnt21a =
texture {
pigment
 {granite
  turbulence 0.0
  color_map
   {[0.0, 0.2 color White transmit 0.4 color White transmit 0.8]
    [0.2, 0.3 color White transmit 0.8 color Clear]
    [0.3, 1.0 color Clear color Clear]
   }
 }
 finish {
  crand 0.02
 }
 }

#declare T_Grnt22a =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.175   color rgbt <1.000, 0.718, 0.541, 0.890>
                    color rgbt <0.843, 0.678, 0.655, 0.753>]
    [0.175, 0.228   color rgbt <0.843, 0.678, 0.655, 0.753>
                    color rgbt <0.906, 0.831, 0.773, 0.98>]
    [0.228, 0.386   color rgbt <0.906, 0.831, 0.773, 0.698>
                    color rgbt <0.992, 0.718, 0.545, 0.794>]
    [0.386, 0.412   color rgbt <0.992, 0.718, 0.545, 0.794>
                    color rgbt <0.333, 0.188, 0.067, 0.784>]
    [0.412, 0.439   color rgbt <0.333, 0.188, 0.067, 0.784>
                    color rgbt <0.925, 0.557, 0.514, 0.778>]
    [0.439, 0.684   color rgbt <0.925, 0.557, 0.514, 0.678>
                    color rgbt <0.984, 0.780, 0.655, 0.696>]
    [0.684, 0.781   color rgbt <0.984, 0.780, 0.655, 0.696>
                    color rgbt <0.965, 0.847, 0.675, 0.880>]
    [0.781, 0.982   color rgbt <0.965, 0.847, 0.675, 0.880>
                    color rgbt <1.000, 0.718, 0.541, 0.990>]
    [0.982, 1.001   color rgbt <1.000, 0.718, 0.541, 0.890>
                    color rgbt <1.000, 0.718, 0.541, 0.890>]
   }
 }
 }

//---------- Transparent Green Moss Colors
#declare T_Grnt23a =
texture {
pigment
 {granite
  color_map
   {[0.000, 0.168   color rgbt <0.824, 0.725, 0.584, 0.600>
                    color rgbt <0.514, 0.584, 0.533, 0.600>]
    [0.168, 0.301   color rgbt <0.514, 0.584, 0.533, 0.600>
                    color rgbt <0.298, 0.376, 0.318, 0.600>]
    [0.301, 0.398   color rgbt <0.298, 0.376, 0.318, 0.600>
                    color rgbt <0.263, 0.337, 0.282, 0.700>]
    [0.398, 0.558   color rgbt <0.263, 0.337, 0.282, 0.700>
                    color rgbt <0.431, 0.506, 0.451, 0.600>]
    [0.558, 0.655   color rgbt <0.431, 0.506, 0.451, 0.600>
                    color rgbt <0.529, 0.631, 0.471, 0.500>]
    [0.655, 0.735   color rgbt <0.529, 0.631, 0.471, 0.500>
                    color rgbt <0.333, 0.376, 0.318, 0.700>]
    [0.735, 0.823   color rgbt <0.333, 0.376, 0.318, 0.700>
                    color rgbt <0.298, 0.376, 0.318, 0.600>]
    [0.823, 0.876   color rgbt <0.298, 0.376, 0.318, 0.600>
                    color rgbt <0.416, 0.376, 0.318, 0.500>]
    [0.876, 0.929   color rgbt <0.416, 0.376, 0.318, 0.500>
                    color rgbt <0.416, 0.376, 0.318, 0.600>]
    [0.929, 1.001   color rgbt <0.416, 0.376, 0.318, 0.600>
                    color rgbt <0.824, 0.725, 0.584, 0.700>]
   }
 }
 }

#declare T_Grnt24a =
texture {
pigment
 {granite
  turbulence 0.5
  color_map
   {[0.000, 0.053   color rgbt <0.784, 0.627, 0.522, 0.500>
                    color rgbt <0.784, 0.627, 0.624, 0.500>]
    [0.053, 0.263   color rgbt <0.784, 0.627, 0.624, 0.500>
                    color rgbt <0.824, 0.557, 0.376, 0.500>]
    [0.263, 0.281   color rgbt <0.824, 0.557, 0.376, 0.500>
                    color rgbt <0.643, 0.380, 0.376, 0.500>]
    [0.281, 0.325   color rgbt <0.643, 0.380, 0.376, 0.500>
                    color rgbt <0.839, 0.722, 0.722, 0.500>]
    [0.325, 0.711   color rgbt <0.839, 0.722, 0.722, 0.500>
                    color rgbt <0.784, 0.627, 0.522, 0.500>]
    [0.711, 0.798   color rgbt <0.784, 0.627, 0.522, 0.500>
                    color rgbt <0.769, 0.380, 0.376, 0.500>]
    [0.798, 0.895   color rgbt <0.769, 0.380, 0.376, 0.500>
                    color rgbt <0.824, 0.557, 0.376, 0.500>]
    [0.895, 0.982   color rgbt <0.824, 0.557, 0.376, 0.500>
                    color rgbt <0.784, 0.627, 0.522, 0.500>]
    [0.982, 1.001   color rgbt <0.784, 0.627, 0.522, 0.500>
                    color rgbt <0.784, 0.627, 0.522, 0.500>]
   }
 }
 }

/*--------------------T_Crack & OverTint /Red---------------------*/
#declare T_Crack1 =
texture {
pigment
 {marble
  turbulence 0.85
  color_map
   {[0.0, 0.04 color Black transmit 0.6 color Black transmit 1.0]
    [0.04, 0.97 color Scarlet transmit 0.80 color DimGray transmit 0.90]
    [0.97, 1.001 color Black transmit 0.9 color Black transmit 1.0]
   }
 }
 }

//-----Transmit  Dark T_Cracks
#declare T_Crack2 =
texture {
pigment
 {granite
  turbulence 0.8
  color_map
   {[0.0, 0.5   color Clear color Clear]
    [0.5, 0.54   color Clear color Black]
    [0.54, 1.0 color Clear color Clear]
   }
 }
 }

//---------- Overtint Green with Black T_Cracks
#declare T_Crack3 =
texture {
pigment
 {marble
  turbulence 0.85
  color_map
   {[0.0, 0.04 color Black transmit 0.6 color Black transmit 1.0]
    [0.04, 0.97 color DarkGreen transmit 0.80 color DarkGreen transmit 0.90]
    [0.97, 1.001 color Black transmit 0.9 color Black transmit 1.0]
   }
 }
 }

//--------- Overtint with White T_Crack
#declare T_Crack4 =
texture {
pigment
 {marble
  turbulence 0.85
  color_map
   {[0.0, 0.03 color White transmit 0.3 color White transmit 1.0]
    [0.03, 0.97 color Black transmit 0.70 color DimGray transmit 0.90]
    [0.97, 1.001 color White transmit 0.9 color White transmit 1.0]
   }
 }
 }

//-----------------  complete texture statements
//                   Scaled to cover nicely over a 2 unit Cube

//------------- Deep Rose & Green Marble with large White Swirls
#declare T_Stone1 =
texture{T_Grnt7  scale <2, 3, 2> rotate <0, 0, 40>}
texture{T_Grnt0a scale <2, 3, 2> rotate <0, 0,-30>
        finish{phong 1 phong_size 90}
       }

//------------- Light Greenish Tan Marble with Agate style veining
#declare T_Stone2 =
texture{T_Grnt0  scale <2, 3, 2> rotate <0, 0, 40>}
texture{T_Grnt7a scale <1.3, 2, 1.3> rotate <0, 0, -30> rotate <40, 0, 0>
        finish{phong 1.0 phong_size 90 ambient 0.2 }
}

//------------- Rose & Yellow Marble with fog white veining
#declare T_Stone3 =
texture{T_Grnt5  scale <2, 3, 2> rotate <0, 0, 40>}
texture{T_Grnt0a scale <2, 3, 2> rotate <0, 0, -30>
        finish{phong 1.0 phong_size 90}
}

//------------- Tan Marble with Rose patches
#declare T_Stone4 =
texture{T_Grnt6   scale <1.5, 3, 2> rotate <0, 0, 40>
        finish{diffuse 0.5}
}
texture{T_Grnt10a scale <1, 3, 2> rotate <0, 0, -30>
        finish{phong 1.0 phong_size 90}
}

//------------- White Cream Marble with Pink veining
#declare T_Stone5 =
texture{T_Grnt12  scale <2, 3, 2> rotate <0, 0, 40>}
texture{T_Grnt17a scale <2, 3, 2> rotate <0, 0, -30>}
texture{T_Crack1  scale <1, 2, 1.4> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90 ambient 0.2}
}

//------------- Rose & Yellow Cream Marble
#declare T_Stone6 =
texture{T_Grnt18 scale <1.5, 3, 3> rotate <0, 0, 40>}
texture{T_Grnt19 scale <2, 4, 1.3> rotate <0, 0, 30>
        finish{phong 1.0 phong_size 90}
}
texture{T_Crack1 scale <1, 2, 1.4> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90 ambient 0.2}
}

//------------- Light Coffee Marble with darker patches
#declare T_Stone7 =
texture{pigment{color Salmon}}
texture{T_Grnt6a scale <1, 3, 2> rotate <0, 0, 40>}
texture{T_Grnt9a scale <3.5, 5, 4> rotate <0, 0, 60>
        finish{phong 1.0 phong_size 90}
}

//------------- Gray Granite with white patches
#declare T_Stone8 =
texture{pigment{color White}
        finish{ambient 0.4 crand 0.06 diffuse 0.7}
}
texture{T_Grnt0a scale <2, 3, 2> rotate <0, 0, -30>}
texture{T_Grnt9a scale <5, 3, 4> rotate <0, 0, 40>
        finish{phong 1.0 ambient 0.2 diffuse 0.5 phong_size 90}
}

//------------- White & Light Blue Marble with light violets
#declare T_Stone9 =
texture{T_Grnt9 scale <1.2, 2.4, 1.2> rotate <0, 0, -30> rotate <40, 0, 0>}
texture{T_Crack1 scale <1, 2, 1.4> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90 ambient 0.2}
}

//------------- Dark Brown & Tan swirl Granite with gray undertones
#declare T_Stone10 =
texture{pigment{color Black}}
texture{T_Grnt17a scale <3, 6, 2> rotate <0, 0, 50>}
texture{T_Grnt3a scale <1, 2, 1> rotate <0, 0, -50>
        finish{phong 1.0 phong_size 90}
}

//------------- Rose & White Marble with dark tan swirl
#declare T_Stone11 =
texture{pigment{color Black}}
texture{T_Grnt15a scale <1.2, 3, 1.5> rotate <70, 0, 30>
        finish{crand 0.03}
}
texture{T_Grnt2a scale <3, 3, 4> rotate <0, 0, 40>}
texture{T_Crack1 scale <1, 2, 1.4> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90}
}

//------------- White & Pinkish Tan Marble
#declare T_Stone12 =
texture{T_Grnt23 scale <1, 5, 1> rotate <0, 0, 50>
        finish{ambient 0.2 crand 0.03}
}
texture{T_Grnt0a scale <1, 3, 2> rotate <0, 0, -30>
        finish{phong 1.0 phong_size 90}
}

//------------- Medium Gray Blue Marble
#declare T_Stone13 =
texture{T_Grnt24 scale <2, 5, 2> rotate <0, 0, 50>
        finish{ambient 0.2 crand 0.03}
}
texture{T_Grnt8a scale <1, 3, 2> rotate <0, 0, -30>
        finish{phong 1.0 phong_size 90}
}

//------------- Tan & Olive Marble with gray white veins
#declare T_Stone14 =
texture{T_Grnt6 scale <2, 3, 2> rotate <0, 0, -30>
        finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{T_Grnt19a scale <1, 3, 1> rotate <0, 0, 40>
        finish{phong 1.0 phong_size 90}
}

//------------- Deep Gray Marble with white veining
#declare T_Stone15 =
texture{T_Grnt20 scale <1, 2, 2> rotate <0, 0, -30>
        finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{T_Grnt8a scale <1, 2, 1> rotate <0, 0, 40>
        finish{phong 1.0 phong_size 90}
}

//------------- Peach & Yellow Marble with white veining
#declare T_Stone16 =
texture{T_Grnt18 scale <1.3, 2, 2> rotate <0, 0, -30>
        finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{T_Grnt19 scale <2, 4, 2> rotate <0, 0, -30>
        finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
texture{T_Grnt20a scale <1, 2, 1> rotate <0, 0, 40>
        finish{phong 1.0 phong_size 90}
}

//------------- White Marble with gray veining
#declare T_Stone17 =
texture{T_Grnt20 scale <1, 2, 2> rotate <0, 0, -30>
        finish{ambient 0.2 diffuse 0.9 crand 0.03}
}
//texture{T_Grnt8a scale <2.5, 4.5, 3.5> rotate <0, 0, 40>
//        finish{phong 1.0 phong_size 90}
//}
texture{T_Crack3 scale <1, 2, 1.4> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90}
}

//------------- Green Jade with white veining
#declare T_Stone18 =
texture{pigment{color SeaGreen}
        finish{ambient 0.3 diffuse 0.6 crand 0.03}
}
texture{T_Grnt22 scale <1.5, 0.7, 0.5> rotate <0, 0, 40>}
texture{T_Grnt20a scale <2.5, 2, 0.5> rotate <0, 0, -50>}
texture{T_Crack4 scale <0.7, 1, 1> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90}
}

//------------- Peach Granite with white patches & green trim
#declare T_Stone19 =
texture{T_Grnt26 scale <1, 0.7, 0.5> rotate <0, 0, 40>}
texture{T_Grnt20a scale <2, 3, 1> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90}
}

//------------- Brown & Olive Marble with white veining
#declare T_Stone20 =
texture{T_Grnt27 scale <0.7, 0.99, 0.7> rotate <0, 0, 40>}
texture{T_Grnt12a scale <1, 1.3, 2> rotate <0, 0, 40>}
texture{T_Grnt20a scale <1.9, 3, 0.5> rotate <0, 0, -50>}
texture{T_Crack1 scale <1, 0.6, 1> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90}
}

//------------- Red Marble with gray & white veining
#declare T_Stone21 =
texture{T_Grnt28 scale <1.3, 2.5, 1.7> rotate <0, 0, 40>}
texture{T_Grnt22 scale <1, 2, 2> rotate <0, 0, 40>}
texture{T_Crack4 scale <1, 0.6, 1> rotate <10, 0, -20>
        finish{phong 1.0 phong_size 90}
}

//------------- Dark Tan Marble with gray & white veining
#declare T_Stone22 =
texture{pigment{color Feldspar}}
texture{T_Grnt8a scale <1, 2, 2> rotate <0, 0, 40>}
texture{T_Grnt22 scale <2, 4, 1.5> rotate <0, 0, -50>}
texture{T_Crack4 scale <1, 1, 1> rotate <10, 0, -40>
        finish{phong 1.0 phong_size 90}
}

//------------- Peach & Cream Marble with orange veining
#declare T_Stone23 =
texture{T_Grnt29 scale <1, 1, 2> rotate <40, 0, 0> rotate <0, 0, 30>}
texture{T_Grnt24a scale <2, 1, 2> rotate <40, 0, 0> rotate <0, 0, 30>}
texture{T_Crack1 scale <1, 2, 1.5> rotate <0, 0, 40>}
texture{pigment{color Yellow transmit 0.9}
        finish{phong 1.0 phong_size 90}  // tint to liking
}

//------------- Green & Tan Moss Marble
#declare T_Stone24 =
texture{T_Grnt25 scale <1, 1, 2> rotate <0, 0, 50> rotate <20, 0, 30>}
texture{T_Grnt23a scale <2, 1, 2> rotate <40, 0, 0> rotate <0, 0, -30>}
texture{pigment{color Gray transmit 0.8}
        finish{phong 1.0 phong_size 90} //   tint to liking
}


#version Stones1_Inc_Temp;
#end
