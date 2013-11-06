// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: woodmaps.inc
//    Last updated: 2001.7.24
//    Description:
/*                         Basic wooden colormaps

 The "M_WoodxA" color_maps are intended as the 1st layer of multi-layer
 textures, but many work well by themselves, as well.

 The "M_WoodxB" color_maps contain transparent colors and are intended for
 use as top layers.
*/

#ifndef(Woodmaps_Inc_Temp)
#declare Woodmaps_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including woodmaps.inc\n"
#end


#declare M_Wood1A =
color_map { 
    [0.0, 0.1 color rgb <0.88, 0.60, 0.40>
              color rgb <0.88, 0.60, 0.40>]
    [0.1, 0.9 color rgb <0.88, 0.60, 0.40>
              color rgb <0.60, 0.30, 0.20>]
    [0.9, 1.0 color rgb <0.60, 0.30, 0.20>
              color rgb <0.60, 0.30, 0.20>]
}
#declare M_Wood1B = 
color_map { 
    [0.0, 0.1 color rgbt <0.55, 0.32, 0.20, 0.100> 
              color rgbt <0.55, 0.32, 0.20, 0.500>]
    [0.1, 0.2 color rgbt <0.55, 0.35, 0.20, 0.650>
              color rgbt <0.88, 0.60, 0.40, 0.975>]
    [0.2, 0.3 color rgbt <0.88, 0.60, 0.40, 0.975>
              color rgbt <0.60, 0.30, 0.20, 1.000>]
    [0.3, 0.4 color rgbt <0.60, 0.30, 0.20, 0.100> 
              color rgbt <0.60, 0.30, 0.20, 0.500>]
    [0.4, 0.9 color rgbt <0.60, 0.30, 0.20, 0.650>
              color rgbt <0.88, 0.60, 0.40, 0.975>]
    [0.9, 1.0 color rgbt <0.88, 0.60, 0.40, 0.975>
              color rgbt <0.55, 0.32, 0.20, 1.000>]
}


#declare M_Wood2A = 
color_map { 
    [0.0, 0.1 color rgb <0.35, 0.16, 0.12 >* 0.5
              color rgb <0.35, 0.16, 0.12>* 0.5]
    [0.1, 0.9 color rgb <0.35, 0.20, 0.16>* 0.5
              color rgb <0.46, 0.26, 0.18>* 0.5]
    [0.9, 1.0 color rgb <0.46, 0.26, 0.18>* 0.5
              color rgb <0.35, 0.16, 0.12>* 0.5]
}
#declare M_Wood2B = 
color_map { 
    [0.0, 0.1 color rgbt <0.50, 0.30, 0.20, 0.100> 
              color rgbt <0.50, 0.30, 0.20, 0.500>]
    [0.1, 0.9 color rgbt <0.50, 0.30, 0.20, 0.650>
              color rgbt <0.50, 0.25, 0.15, 0.975>]
    [0.9, 1.0 color rgbt <0.50, 0.25, 0.15, 0.975>
              color rgbt <0.50, 0.30, 0.20, 0.000>]
}
#declare M_Wood3A =
color_map {
    [0.0, 0.1 color rgb <0.75, 0.65, 0.45>
              color rgb <0.75, 0.65, 0.45>]
    [0.1, 0.9 color rgb <0.75, 0.65, 0.45>
              color rgb <0.70, 0.55, 0.40>]
    [0.9, 1.0 color rgb <0.70, 0.55, 0.40>
              color rgb <0.75, 0.65, 0.45>]
}
#declare M_Wood3B =
color_map {
    [0.0, 0.1 color rgbt <0.70, 0.60, 0.40, 0.100>
              color rgbt <0.70, 0.60, 0.40, 0.500>]
    [0.1, 0.9 color rgbt <0.70, 0.60, 0.40, 0.650>
              color rgbt <0.75, 0.70, 0.60, 0.975>]
    [0.9, 1.0 color rgbt <0.75, 0.70, 0.60, 0.975>
              color rgbt <0.70, 0.60, 0.40, 1.000>]
}
#declare M_Wood4A = 
color_map { 
    [0.0, 0.3 color rgb <0.30, 0.10, 0.15>
              color rgb <0.30, 0.15, 0.15>]
    [0.3, 0.6 color rgb <0.30, 0.15, 0.15>
              color rgb <0.50, 0.20, 0.15>]
    [0.6, 1.0 color rgb <0.50, 0.20, 0.15>
              color rgb <0.30, 0.10, 0.15>]
}
#declare M_Wood4B = 
color_map { 
/*
    [0.0, 0.3 color rgbt <0.40, 0.18, 0.20, 0.00> 
              color rgbt <0.20, 0.10, 0.15, 0.20>]
    [0.3, 0.4 color rgbt <0.20, 0.10, 0.15, 0.35>
              color rgbt <0.10, 0.05, 0.07, 0.50>]
    [0.4, 0.6 color rgbt <0.10, 0.05, 0.07, 0.20>
              color rgbt <0.20, 0.10, 0.15, 0.35>]
    [0.6, 1.0 color rgbt <0.20, 0.10, 0.15, 0.50>
              color rgbt <0.40, 0.18, 0.20, 0.00>]
*/
    [0.0, 0.3 color rgbt <0.32, 0.15, 0.17, 0.00> 
              color rgbt <0.32, 0.13, 0.15, 0.20>]
    [0.3, 0.4 color rgbt <0.32, 0.13, 0.13, 0.35>
              color rgbt <0.52, 0.25, 0.23, 0.40>]
    [0.4, 0.6 color rgbt <0.52, 0.25, 0.23, 0.20>
              color rgbt <0.32, 0.13, 0.13, 0.35>]
    [0.6, 1.0 color rgbt <0.32, 0.13, 0.15, 0.50>
              color rgbt <0.32, 0.15, 0.20, 0.00>]

}
#declare M_Wood5A = 
color_map { 
    [0.0, 0.3 color rgb <0.50, 0.35, 0.10>
              color rgb <0.50, 0.35, 0.10>]
    [0.3, 0.6 color rgb <0.50, 0.35, 0.10>
              color rgb <0.60, 0.45, 0.10>]
    [0.6, 1.0 color rgb <0.60, 0.45, 0.10>
              color rgb <0.50, 0.35, 0.10>]
}
#declare M_Wood5B = 
color_map { 
    [0.0, 0.1 color rgbt <0.40, 0.35, 0.10, 1.00> 
              color rgbt <0.40, 0.00, 0.10, 0.60>]
    [0.1, 0.9 color rgbt <0.40, 0.00, 0.10, 0.75>
              color rgbt <0.35, 0.40, 0.15, 0.90>]
    [0.9, 1.0 color rgbt <0.35, 0.40, 0.15, 0.90>
              color rgbt <0.40, 0.35, 0.10, 1.00>]
}
#declare M_Wood6A = 
color_map { 
    [0.0, 0.3 color rgb <0.25, 0.10, 0.10>
              color rgb <0.25, 0.10, 0.10>]
    [0.3, 0.6 color rgb <0.25, 0.10, 0.10>
              color rgb <0.60, 0.15, 0.10>]
    [0.6, 1.0 color rgb <0.60, 0.15, 0.10>
              color rgb <0.25, 0.10, 0.10>]
}
#declare M_Wood6B = 
color_map { 
    [0.0, 0.3 color rgbt <0.25, 0.10, 0.10, 0.00> 
              color rgbt <0.25, 0.10, 0.10, 0.40>]
    [0.3, 0.5 color rgbt <0.25, 0.10, 0.10, 0.40>
              color rgbt <0.60, 0.15, 0.10, 1.00>]
    [0.5, 0.7 color rgbt <0.60, 0.15, 0.10, 1.00>
              color rgbt <0.25, 0.10, 0.10, 0.40>]
    [0.7, 1.0 color rgbt <0.60, 0.15, 0.10, 0.40>
              color rgbt <0.25, 0.10, 0.10, 0.00>]
}
#declare M_Wood7A = 
color_map { 
    [0.0, 0.1 color rgb <0.60, 0.35, 0.20>
              color rgb <0.60, 0.35, 0.20>]
    [0.1, 0.9 color rgb <0.60, 0.35, 0.20>
              color rgb <0.90, 0.65, 0.30>]
    [0.9, 1.0 color rgb <0.90, 0.65, 0.30>
              color rgb <0.60, 0.35, 0.20>]
}
#declare M_Wood7B = 
color_map { 
    [0.0, 0.1 color rgbt <0.90, 0.65, 0.30, 0.00>
              color rgbt <0.90, 0.65, 0.30, 0.30>]
    [0.1, 1.0 color rgbt <0.90, 0.65, 0.30, 0.30>
              color rgbt <1.00, 1.00, 1.00, 1.00> ] 
}
#declare M_Wood8A = 
color_map { 
    [0.0, 0.1 color rgb <0.45, 0.30, 0.15>
              color rgb <0.45, 0.30, 0.15>]
    [0.1, 0.9 color rgb <0.55, 0.30, 0.15>
              color rgb <0.20, 0.15, 0.05>]
    [0.9, 1.0 color rgb <0.20, 0.15, 0.05>
              color rgb <0.45, 0.30, 0.15>]
}
#declare M_Wood8B = 
color_map { 
    [0.0, 0.5 color rgbt <0.60, 0.35, 0.20, 0.30>
              color rgbt <0.60, 0.35, 0.20, 0.80>]
    [0.5, 1.0 color rgbt <0.60, 0.35, 0.20, 0.80>
              color rgbt <1.00, 1.00, 1.00, 1.00> ] 
}
#declare M_Wood9A = 
color_map { 
    [0.0, 0.5 color rgb <1.00, 0.85, 0.50>
              color rgb <0.60, 0.40, 0.16>]
    [0.5, 0.7 color rgb <0.60, 0.40, 0.16>
              color rgb <0.60, 0.40, 0.16>]
    [0.7, 1.0 color rgb <0.60, 0.40, 0.16>
              color rgb <1.00, 0.85, 0.50>]
}
#declare M_Wood9B = 
color_map { 
    [0.0, 0.5 color rgbt <1.00, 0.70, 0.25, 1.00>
              color rgbt <0.50, 0.30, 0.06, 0.40>]
    [0.5, 0.7 color rgbt <0.50, 0.30, 0.06, 0.40>
              color rgbt <0.50, 0.30, 0.06, 0.30>]
    [0.7, 1.0 color rgbt <0.50, 0.30, 0.06, 0.40>
              color rgbt <1.00, 0.70, 0.25, 1.00>]
}
#declare M_Wood10A = 
color_map { 
    [0.0, 0.5 color rgb <1.00, 0.85, 0.50>
              color rgb <0.90, 0.70, 0.46>]
    [0.5, 0.7 color rgb <0.90, 0.70, 0.46>
              color rgb <0.90, 0.70, 0.46>]
    [0.7, 1.0 color rgb <0.90, 0.70, 0.46>
              color rgb <1.00, 0.85, 0.50>]
}
#declare M_Wood10B = 
color_map { 
    [0.0, 0.5 color rgbt <1.00, 0.45, 0.10, 0.80>
              color rgbt <0.85, 0.65, 0.40, 0.40>]
    [0.5, 0.7 color rgbt <0.85, 0.65, 0.40, 0.40>
              color rgbt <0.85, 0.65, 0.40, 0.40>]
    [0.7, 1.0 color rgbt <0.85, 0.65, 0.40, 0.40>
              color rgbt <1.00, 0.45, 0.10, 0.80>]
}
#declare M_Wood11A = 
color_map {
    [0.000, 0.222 color rgb <0.80, 0.67, 0.25>
                  color rgb <0.80, 0.67, 0.25>]
    [0.222, 0.342 color rgb <0.80, 0.67, 0.25>
                  color rgb <0.60, 0.34, 0.04>]
    [0.342, 0.393 color rgb <0.60, 0.34, 0.04>
                  color rgb <0.80, 0.67, 0.25>]
    [0.393, 0.709 color rgb <0.80, 0.67, 0.25>
                  color rgb <0.80, 0.67, 0.25>]
    [0.709, 0.821 color rgb <0.80, 0.67, 0.25>
                  color rgb <0.53, 0.29, 0.02>]
    [0.821, 1.000 color rgb <0.53, 0.29, 0.02>
                  color rgb <0.80, 0.67, 0.25>]
}
#declare M_Wood11B = 
color_map {
    [0.000, 0.120 color rgbt <1.00, 1.00, 1.00, 1.00>
                  color rgbt <0.70, 0.41, 0.11, 0.60>]
    [0.120, 0.231 color rgbt <0.70, 0.41, 0.11, 0.60>
                  color rgbt <0.70, 0.46, 0.11, 0.60>]
    [0.231, 0.496 color rgbt <0.70, 0.46, 0.11, 0.60>
                  color rgbt <1.00, 1.00, 1.00, 1.00>]
    [0.496, 0.701 color rgbt <1.00, 1.00, 1.00, 1.00>
                  color rgbt <1.00, 1.00, 1.00, 1.00>]
    [0.701, 0.829 color rgbt <1.00, 1.00, 1.00, 1.00>
                  color rgbt <0.70, 0.46, 0.11, 0.60>]
    [0.829, 1.000 color rgbt <0.70, 0.46, 0.11, 0.60>
                  color rgbt <1.00, 1.00, 1.00, 1.00>]
}
#declare M_Wood12A = 
color_map {
    [0.000, 0.256 color rgb <0.20, 0.11, 0.07>*1.25 
                  color rgb <0.23, 0.12, 0.09>*1.25]
    [0.256, 0.393 color rgb <0.23, 0.12, 0.09>*1.25
                  color rgb <0.24, 0.13, 0.09>*1.25]
    [0.393, 0.581 color rgb <0.24, 0.13, 0.09>*1.25
                  color rgb <0.20, 0.11, 0.07>*1.25]
    [0.581, 0.726 color rgb <0.20, 0.11, 0.07>*1.25
                  color rgb <0.25, 0.12, 0.10>*1.25]
    [0.726, 0.983 color rgb <0.25, 0.12, 0.10>*1.25
                  color rgb <0.23, 0.12, 0.08>*1.25]
    [0.983, 1.000 color rgb <0.23, 0.12, 0.08>*1.25
                  color rgb <0.20, 0.11, 0.07>*1.25]
}
#declare M_Wood12B = 
color_map {
    [0.000, 0.139 color rgbt <0.545, 0.349, 0.247, 1.000>
                  color rgbt <0.450, 0.350, 0.320, 0.800>]
    [0.139, 0.148 color rgbt <0.450, 0.350, 0.320, 0.800>
                  color rgbt <0.450, 0.350, 0.320, 0.800>]
    [0.148, 0.287 color rgbt <0.450, 0.350, 0.320, 0.800>
                  color rgbt <0.545, 0.349, 0.247, 1.000>]
    [0.287, 0.443 color rgbt <0.545, 0.349, 0.247, 1.000>
                  color rgbt <0.545, 0.349, 0.247, 1.000>]
    [0.443, 0.626 color rgbt <0.545, 0.349, 0.247, 1.000>
                  color rgbt <0.450, 0.350, 0.320, 0.800>]
    [0.626, 0.635 color rgbt <0.450, 0.350, 0.320, 0.800>
                  color rgbt <0.450, 0.350, 0.320, 0.800>]
    [0.635, 0.843 color rgbt <0.450, 0.350, 0.320, 0.800>
                  color rgbt <0.545, 0.349, 0.247, 1.000>]
    [0.843, 1.000 color rgbt <0.545, 0.349, 0.247, 1.000>
                  color rgbt <0.545, 0.349, 0.247, 1.000>]
}
// Same as M_Wood7A
#declare M_Wood13A = 
color_map { 
    [0.0, 0.1 color rgb <0.60, 0.35, 0.20>
              color rgb <0.60, 0.35, 0.20>]
    [0.1, 0.9 color rgb <0.60, 0.35, 0.20>
              color rgb <0.90, 0.65, 0.30>]
    [0.9, 1.0 color rgb <0.90, 0.65, 0.30>
              color rgb <0.60, 0.35, 0.20>]
}
// Same as M_Wood7B
#declare M_Wood13B = 
/********
color_map { 
    [0.0, 0.1 color rgbt <0.90, 0.65, 0.30, 0.00>
              color rgbt <0.90, 0.65, 0.30, 0.30>]
    [0.1, 1.0 color rgbt <0.90, 0.65, 0.30, 0.30>
              color rgbt <1.00, 1.00, 1.00, 1.00> ] 
}
********/
color_map { 
    [0.0, 0.4 color rgbt <1.00, 1.00, 1.00, 1.00>
              color rgbt <0.90, 0.65, 0.30, 0.30>]
    [0.4, 0.5 color rgbt <0.90, 0.65, 0.30, 0.00>
              color rgbt <0.90, 0.65, 0.30, 0.30>]
    [0.5, 1.0 color rgbt <0.90, 0.65, 0.30, 0.30>
              color rgbt <1.00, 1.00, 1.00, 1.00> ] 
}
#declare M_Wood14A = 
colour_map {
    [0.00 0.10 color rgb < 0.80, 0.232, 0.115 >
               color rgb < 0.80, 0.232, 0.115 >]
    [0.10 0.90 color rgb < 0.80, 0.232, 0.115 >
               color rgb < 0.45, 0.115, 0.060 >]
    [0.90 1.0  color rgb < 0.45, 0.115, 0.060 >
               color rgb < 0.45, 0.115, 0.060 >]
}

#declare M_Wood14B = 
colour_map {
    [0.00 0.10 color rgbt < 0.70, 0.232, 0.115, 0.5 >
               color rgbt < 0.70, 0.232, 0.115, 0.7 >]
    [0.10 0.15 color rgbt < 0.70, 0.232, 0.115, 0.7 >
               color rgbt < 0.35, 0.115, 0.060, 0.9 >]
    [0.15 0.20 color rgbt < 0.70, 0.232, 0.115, 0.9 >
               color rgbt < 0.35, 0.115, 0.060, 0.7 >]
    [0.20 1.0  color rgbt < 0.35, 0.115, 0.060, 0.7 >
               color rgbt < 0.35, 0.115, 0.060, 0.5 >]
}
#declare M_Wood15A = 
colour_map {
    [0.00 0.25   color rgb < 0.504, 0.310, 0.078> * 0.7
                 color rgb < 0.531, 0.325, 0.090> * 0.8 ]
    [0.25 0.40   color rgb < 0.531, 0.325, 0.090> * 0.8
                 color rgb < 0.547, 0.333, 0.090> * 0.5 ]
    [0.40 0.50   color rgb < 0.547, 0.333, 0.090> * 0.5
                 color rgb < 0.504, 0.310, 0.075> * 0.6 ]
    [0.50 0.70   color rgb < 0.504, 0.310, 0.075> * 0.6 
                 color rgb < 0.559, 0.322, 0.102> * 0.4 ]
    [0.70 0.98   color rgb < 0.559, 0.322, 0.102> * 0.4
                 color rgb < 0.531, 0.325, 0.086> * 0.4 ]
    [0.98 1.00   color rgb < 0.531, 0.325, 0.086> * 0.4
                 color rgb < 0.504, 0.310, 0.078> * 0.7 ]
}
#declare M_Wood15B = 
colour_map {
    [0.00 0.25   color rgbt < 0.404, 0.210, 0.078, 0.20>
                 color rgbt < 0.431, 0.225, 0.090, 0.80>]
    [0.25 0.40   color rgbt < 0.431, 0.225, 0.090, 0.80>
                 color rgbt < 0.447, 0.233, 0.090, 0.20>]
    [0.40 0.50   color rgbt < 0.447, 0.233, 0.090, 0.20>
                 color rgbt < 0.404, 0.210, 0.075, 0.60>]
    [0.50 0.70   color rgbt < 0.404, 0.210, 0.075, 0.60>
                 color rgbt < 0.459, 0.222, 0.102, 0.20>]
    [0.70 0.98   color rgbt < 0.459, 0.222, 0.102, 0.20>
                 color rgbt < 0.431, 0.225, 0.086, 0.40>]
    [0.98 1.00   color rgbt < 0.431, 0.225, 0.086, 0.40>
                 color rgbt < 0.404, 0.210, 0.078, 0.10>]
}
#declare M_Wood16A =
colour_map {
    [0.00 0.25   color rgb < 0.504, 0.310, 0.078> * 0.7
                 color rgb < 0.531, 0.325, 0.090> * 0.8 ]
    [0.25 0.40   color rgb < 0.531, 0.325, 0.090> * 0.8
                 color rgb < 0.547, 0.333, 0.090> * 0.5 ]
    [0.40 0.50   color rgb < 0.547, 0.333, 0.090> * 0.5
                 color rgb < 0.504, 0.310, 0.075> * 0.6 ]
    [0.50 0.70   color rgb < 0.504, 0.310, 0.075> * 0.6
                 color rgb < 0.559, 0.322, 0.102> * 0.4 ]
    [0.70 0.98   color rgb < 0.559, 0.322, 0.102> * 0.4
                 color rgb < 0.531, 0.325, 0.086> * 0.4 ]
    [0.98 1.00   color rgb < 0.531, 0.325, 0.086> * 0.4
                 color rgb < 0.504, 0.310, 0.078> * 0.7 ]
}
#declare M_Wood16B =
colour_map {
    [0.00 0.25   color rgbt < 0.404, 0.210, 0.078, 0.30>
                 color rgbt < 0.431, 0.225, 0.090, 0.60>]
    [0.25 0.40   color rgbt < 0.431, 0.225, 0.090, 0.60>
                 color rgbt < 0.447, 0.233, 0.090, 0.30>]
    [0.40 0.50   color rgbt < 0.447, 0.233, 0.090, 0.30>
                 color rgbt < 0.404, 0.210, 0.075, 0.40>]
    [0.50 0.70   color rgbt < 0.404, 0.210, 0.075, 0.40>
                 color rgbt < 0.459, 0.222, 0.102, 0.20>]
    [0.70 0.98   color rgbt < 0.459, 0.222, 0.102, 0.20>
                 color rgbt < 0.431, 0.225, 0.086, 0.50>]
    [0.98 1.00   color rgbt < 0.431, 0.225, 0.086, 0.50>
                 color rgbt < 0.404, 0.210, 0.078, 0.30>]
}
#declare M_Wood17A =
colour_map {
    [0.00 0.25   color rgb < 0.70, 0.40, 0.15> * 0.9
                 color rgb < 0.75, 0.42, 0.30> * 0.7 ]
    [0.25 0.40   color rgb < 0.75, 0.42, 0.25> * 0.7
                 color rgb < 0.74, 0.43, 0.30> * 0.5 ]
    [0.40 0.50   color rgb < 0.74, 0.43, 0.20> * 0.5
                 color rgb < 0.70, 0.42, 0.15> * 0.3 ]
    [0.50 0.70   color rgb < 0.70, 0.42, 0.15> * 0.3 
                 color rgb < 0.64, 0.46, 0.10> * 0.4 ]
    [0.70 0.98   color rgb < 0.64, 0.46, 0.10> * 0.4
                 color rgb < 0.65, 0.42, 0.20> * 0.6 ]
    [0.98 1.00   color rgb < 0.65, 0.42, 0.20> * 0.6
                 color rgb < 0.60, 0.40, 0.15> * 0.9 ]
}
#declare M_Wood17B = 
colour_map {
    [0.00 0.25   color rgbt < 0.40, 0.20, 0.08, 0.10>
                 color rgbt < 0.44, 0.23, 0.09, 0.20>]
    [0.25 0.40   color rgbt < 0.44, 0.23, 0.09, 0.20>
                 color rgbt < 0.45, 0.25, 0.09, 0.30>]
    [0.40 0.50   color rgbt < 0.45, 0.25, 0.09, 0.30>
                 color rgbt < 0.40, 0.20, 0.08, 0.60>]
    [0.50 0.70   color rgbt < 0.40, 0.20, 0.08, 0.60>  
                 color rgbt < 0.46, 0.23, 0.10, 0.30>]
    [0.70 0.98   color rgbt < 0.46, 0.23, 0.10, 0.30> 
                 color rgbt < 0.44, 0.23, 0.09, 0.20>]
    [0.98 1.00   color rgbt < 0.44, 0.23, 0.09, 0.20> 
                 color rgbt < 0.40, 0.20, 0.08, 0.10>]
}
#declare M_Wood18A = 
colour_map {
    [0.00 0.15   color rgb < 1.0, 0.50, 0.00> 
                 color rgb < 1.0, 0.50, 0.00> *0.5 ]
    [0.15 0.25   color rgb < 1.0, 0.50, 0.00> *0.5 
                 color rgb < 1.0, 0.45, 0.00> *0.7 ]
    [0.25 0.28   color rgb < 1.0, 0.45, 0.00> *0.8 
                 color rgb < 1.0, 0.36, 0.00> *0.3 ]
    [0.28 0.40   color rgb < 1.0, 0.36, 0.00> *0.3 
                 color rgb < 1.0, 0.40, 0.00> *0.4 ]
    [0.40 0.50   color rgb < 1.0, 0.40, 0.00> *0.4 
                 color rgb < 1.0, 0.40, 0.00> *0.6 ]
    [0.50 0.70   color rgb < 1.0, 0.50, 0.00> *0.6 
                 color rgb < 1.0, 0.50, 0.00> *0.7 ]
    [0.70 0.98   color rgb < 1.0, 0.45, 0.00> *0.7 
                 color rgb < 1.0, 0.45, 0.00> *0.5 ]
    [0.98 1.00   color rgb < 1.0, 0.45, 0.00> *0.5 
                 color rgb < 1.0, 0.50, 0.00> ]
}
#declare M_Wood18B = 
colour_map {
    [0.00 0.25   color rgbt < 0.50, 0.26, 0.12, 0.30>
                 color rgbt < 0.54, 0.29, 0.13, 0.40>]
    [0.25 0.40   color rgbt < 0.54, 0.29, 0.13, 0.40>
                 color rgbt < 0.55, 0.28, 0.10, 0.60>]
    [0.40 0.50   color rgbt < 0.55, 0.28, 0.10, 0.60>
                 color rgbt < 0.50, 0.23, 0.15, 1.00>]
    [0.50 0.70   color rgbt < 0.50, 0.23, 0.15, 1.00>  
                 color rgbt < 0.56, 0.29, 0.17, 0.60>]
    [0.70 0.98   color rgbt < 0.56, 0.29, 0.17, 0.60> 
                 color rgbt < 0.54, 0.29, 0.13, 0.40>]
    [0.98 1.00   color rgbt < 0.54, 0.29, 0.13, 0.40> 
                 color rgbt < 0.50, 0.26, 0.12, 0.30>]
}
#declare M_Wood19A =
color_map {
    [0.00 color rgb <0.5, 0.25, 0.125>]
    [0.40 color rgb <1.0, 0.50, 0.250> ]
    [0.60 color rgb <1.0, 0.50, 0.250> ]
    [1.00 color rgb <0.5, 0.25, 0.125>]
}
#declare M_Wood19B =
color_map {
    [0.00 0.30 color rgb <0.35, 0.175, 0.0875> 
               color rgb <1.00, 0.500, 0.2500> ]
    [0.30 1.00 color rgbf 1 color rgbf 1]
}
#version Woodmaps_Inc_Temp;
#end
