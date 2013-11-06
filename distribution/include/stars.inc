// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

#ifndef(Stars_Inc_Temp)
#declare Stars_Inc_Temp=version;
#version 3.5;

/*
         Persistence of Vision Raytracer Version 3.1


Here are some updated/additional Starfield textures which you can include 
in distribution with POVRAY 3.1.  These textures use some of the new 
features in POVRAY 3.1.  The starfields become more "dense" as you progress 
from Starfield1 to Starfield6 and add a little color since all stars are not 
white but light shades of white, blue, red, and yellow.

Respectfully;

Jeff Burton
jburton@apex.net
http://www.geocities.com/SoHo/2365
*/


#declare Starfield1 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000  0.270 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.270  0.280 color rgb <.5,.5,.4> color rgb <.8,.8,.4> ]
            [ 0.280  0.470 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.470  0.480 color rgb <.4,.4,.5> color rgb <.4,.4,.8> ]
            [ 0.480  0.680 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.680  0.690 color rgb <.5,.4,.4> color rgb <.8,.4,.4> ]
            [ 0.690  0.880 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.880  0.890 color rgb <.5,.5,.5> color rgb < 1, 1, 1> ]
            [ 0.890  1.000 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
        }
    turbulence 1
    sine_wave
    scale .5
    }
    finish { diffuse 0 ambient 1 }
}

#declare Starfield2 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000  0.270 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.270  0.285 color rgb <.5,.5,.4> color rgb <.8,.8,.4> ]
            [ 0.285  0.470 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.470  0.485 color rgb <.4,.4,.5> color rgb <.4,.4,.8> ]
            [ 0.485  0.680 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.680  0.695 color rgb <.5,.4,.4> color rgb <.8,.4,.4> ]
            [ 0.695  0.880 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.880  0.895 color rgb <.5,.5,.5> color rgb < 1, 1, 1> ]
            [ 0.895  1.000 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
        }
    turbulence 1
    sine_wave
    scale .5
    }
    finish { diffuse 0 ambient 1 }
}

#declare Starfield3 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000  0.270 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.270  0.290 color rgb <.5,.5,.4> color rgb <.8,.8,.4> ]
            [ 0.290  0.470 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.470  0.490 color rgb <.4,.4,.5> color rgb <.4,.4,.8> ]
            [ 0.490  0.680 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.680  0.700 color rgb <.5,.4,.4> color rgb <.8,.4,.4> ]
            [ 0.700  0.880 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.880  0.900 color rgb <.5,.5,.5> color rgb < 1, 1, 1> ]
            [ 0.900  1.000 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
        }
    turbulence 1
    sine_wave
    scale .5
    }
    finish { diffuse 0 ambient 1 }
}

#declare Starfield4 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000  0.270 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.270  0.300 color rgb <.5,.5,.4> color rgb <.8,.8,.4> ]
            [ 0.300  0.470 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.470  0.500 color rgb <.4,.4,.5> color rgb <.4,.4,.8> ]
            [ 0.500  0.670 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.670  0.700 color rgb <.5,.4,.4> color rgb <.8,.4,.4> ]
            [ 0.700  0.870 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.870  0.900 color rgb <.5,.5,.5> color rgb < 1, 1, 1> ]
            [ 0.900  1.000 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
        }
    turbulence 1
    sine_wave
    scale .5
    }
    finish { diffuse 0 ambient 1 }
}

#declare Starfield5 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000  0.260 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.260  0.300 color rgb <.5,.5,.4> color rgb <.8,.8,.4> ]
            [ 0.300  0.460 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.460  0.500 color rgb <.4,.4,.5> color rgb <.4,.4,.8> ]
            [ 0.500  0.660 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.660  0.700 color rgb <.5,.4,.4> color rgb <.8,.4,.4> ]
            [ 0.700  0.860 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.860  0.900 color rgb <.5,.5,.5> color rgb < 1, 1, 1> ]
            [ 0.900  1.000 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
        }
    turbulence 1
    sine_wave
    scale .5
    }
    finish { diffuse 0 ambient 1 }
}

#declare Starfield6 =
texture {
    pigment {
        granite
        color_map {
            [ 0.000  0.250 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.250  0.300 color rgb <.5,.5,.4> color rgb <.8,.8,.4> ]
            [ 0.300  0.450 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.450  0.500 color rgb <.4,.4,.5> color rgb <.4,.4,.8> ]
            [ 0.500  0.650 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.650  0.700 color rgb <.5,.4,.4> color rgb <.8,.4,.4> ]
            [ 0.700  0.850 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
            [ 0.850  0.900 color rgb <.5,.5,.5> color rgb < 1, 1, 1> ]
            [ 0.900  1.000 color rgb < 0, 0, 0> color rgb < 0, 0, 0> ]
        }
    turbulence 1
    sine_wave
    scale .5
    }
    finish { diffuse 0 ambient 1 }
}

#version Stars_Inc_Temp;
#end
