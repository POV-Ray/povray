// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// Created by Fabien Mosen - April 8 2001
// This file contains the environnement objects for the camera
// demo files.

#declare Blue_1   = rgb <0.263,0.345,0.549>;
#declare Blue_2   = rgb <0.243,0.451,0.765>;
#declare Red_1    = rgb <0.761,0.278,0.220>;
#declare Orange_1 = rgb <0.776,0.482,0.212>;
#declare Yellow_1 = rgb <0.757,0.604,0.231>;
#declare Green_1  = rgb <0.282,0.455,0.216>;
#declare Green_2   = rgb <0.675,0.671,0.416>;
#declare Rose_1   = rgb <0.694,0.549,0.514>;
#declare Rose_2   = rgb <0.871,0.702,0.675>;

#declare Cubes1 = union {
box {<0,0,0>,<8,8,8> pigment {Blue_1}}
box {<0,0,0>,<1,3,6> translate <9,0,5> pigment {Yellow_1}}
box {<0,0,0>,<1,2,1> translate <6,8,1> pigment {Orange_1}}
}

#declare Cubes2=union {
box {<0,0,0>,<2,6,1> pigment {Red_1}}
box {<0,0,0>,<4,2,8> translate <-4.5,0,-1> pigment {Green_1}}
box {<0,0,0>,<3,4,3> translate <-5,2,3> pigment {Yellow_1}}
box {<0,0,0>,<1,1,9> translate <-3,6,-2> pigment {Rose_1}}
box {<0,0,0>,<1,3,1.5> translate <-4.5,7,5> pigment {Blue_2}}
}

#declare Cubes3=union {
box {<0,0,0>,<4,5,9> pigment {Green_2}}
box {<0,0,0>,<2,8,2> translate <2,5,2> pigment {Rose_2}}
box {<0,0,0>,<2,2,2> translate <2.2,13,2.4> pigment {Red_1}}
box {<0,0,0>,<4,3,6> translate <1,15,-1> pigment {Rose_1}}
}

#declare Cubes4=union {
box {<0,0,0>,<4,22,3> pigment {Green_2}}
box {<0,0,0>,<2,8,2> translate <3,0,5> pigment {Orange_1}}
}
