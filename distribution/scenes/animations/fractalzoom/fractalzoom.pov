// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Raytracer Scene Description File
//    File: fractalzoom.pov
//    Author: Parts by Rune S. Johansen and Greg M. Johnson,
//      combined in one file by Chris Huff.
//    Description: Demonstrates the use of fractal patterns,
// by zooming in or out of a fractal texture.
// Render as an animation, and change the value of Choice to
// choose which scene you want to render:
// 0 = render both in one animation
// 1 = Fractal zoom by Rune
// 2 = Fractal zoom by Greg
//
//*******************************************

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#declare Choice = 0;

#if(Choice = 0)
    #if(clock < 0.5)
        #declare Clock = clock*2;
        #declare Choice = 1;
    #else
        #declare Clock = clock*2 - 1;
        #declare Choice = 2;
    #end
#else
    #declare Clock = clock;
#end

#switch(Choice)
    #case(1)
        // Rune S. Johansen
        // This animation
        // zooms into the Mandelbrot fractal by a factor of 500000:1.
        // It is done by scaling up the pigment exponentially.

        camera {location -z*4 look_at 0}

        plane {-z, 0
            pigment {mandel 10000
                color_map {
                    [0/4, color <0.0,0.0,0.0>]
                    [1/4, color <0.5,0.0,0.5>]
                    [2/4, color <1.0,0.0,0.0>]
                    [3/4, color <1.0,1.0,0.0>]
                    [4/4, color <1.0,1.0,1.0>]
                }
                // Find an interesting point to zoom into:
                translate <
                +0.7499738161281454000,
                -0.0086354745579594171
                >
                // Scale up pigment exponentially:
                scale pow(10,4+5*Clock)
            }
            finish {ambient 1 diffuse 0}
        }
    #break

    #case(2)
        // Greg M. Johnson
        #declare Place=<-0.07480991501111, 0.97102335799465,-10>;

        camera{
            location Place
            right     x*image_width/image_height
            look_at Place + 10*z

            angle 0.0000000000002*pow(10,Clock*14.2)
        }

        plane {-z, 0
            pigment {mandel 10000
                color_map {
                    [0/60 Black]
                    [0.0625/60 SeaGreen]
                    [0.125/60 Orange]
                    [0.25/60 Blue]
                    [0.375/60 Yellow]
                    [0.45/60 Brown]
                    [0.5/60 Green]
                    [1/60 Orange]
                    [1.5/60 Magenta]
                    [1.75/60 Gray50]
                    [2/60 Red]
                    [3.5/60 Blue]
                    [3.7/60 Orange]
                    [3.875/60 Yellow]
                    [3.95/60 Blue]
                    [3.975/60 Coral]
                    [4/60 Black]
                    [5/60 Yellow]
                    [6/60 Red]
                    [5/30 Blue]
                    [7/30 Green/3]
                    [9/30 Orange]
                    [28/30 Blue]
                    [30/30 Black]
                }
            }
            finish{ambient 1 diffuse 0}
        }
    #break
#end
