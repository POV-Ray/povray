// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
// Features pseudo-Gaussian distribution and use of trace function
// Scene concept and collision algorithm by Greg M. Johnson 2001
// Textures by Gilles Tran
// ------------------------------
// This file creates piles of cubes
// The cubes are generated using a collision detection algorithm
// and the cubes rotations and positions are written to a file called "stacks.inc"
// If you want to run the script a second time without re-generating this file (and thus save on parsing time)
// just uncomment the #declare WriteFile=false line below
// ------------------------------
// Stats on a PIII 733 @ 640*480, default antialiasing
// Parsing time with WriteFile=true : 1 min
// Tracing time : 10 min
// Peak memory : 10 Mb
// ------------------------------
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

#declare WriteFile=true;  // turns on the generation of the stacks and write them to a file
#declare WriteFile=false; // turns off the generation of the file, just read them from the previous file
// ------------------------------
// choose a number of cubes (has no effect unless WriteFile=True)
// large n values give long parsing time
#declare num=400;
//#declare num=10;
//#declare num=5;
// ------------------------------

#include "colors.inc"
#include "functions.inc"
#include "textures.inc"
#include "metals.inc"

global_settings{max_trace_level 25 assumed_gamma 1.0}

// ------------------------------
// Scene settings
// ------------------------------
#declare CamLoc=<0,7,-21>;
#declare CamEye=<0,4.5,-10>;
#declare CamSky=y;
//#declare AspectRatio=4/3;
#declare CamZoom=1;

camera {
        location CamLoc
        direction z*CamZoom
        //right x*AspectRatio
        right x*image_width/image_height
        look_at CamEye
}
light_source{<-500,1000,1000> color rgb<110,245,255>*2/255}
light_source{<0,0.5,-30> color Copper*3 spotlight radius 1 falloff 10 point_at <-2,0.5,-10>}
light_source{<0,50,-100> color Copper*3 spotlight radius 1 falloff 5 point_at y*4 rotate -y*50}
light_source{<0,200,-100> color Copper*3 spotlight radius 1 falloff 5 point_at y*4 rotate y*50}
light_source{<0,400,-100> color Copper*3 spotlight radius 1 falloff 5 point_at y*4 rotate y*180}

plane{y,0
    texture{
        pigment{Black}
        finish{ambient 0 diffuse 1 brilliance 4 specular 0.5 roughness 1/100 reflection 0.8}
    }
}

// ------------------------------
// centers the text
// ------------------------------
#macro centertext(Text)
        #local MinText=min_extent(Text);
        #local MaxText=max_extent(Text);
        translate -(MinText+(MaxText-MinText)/2)
#end

// ------------------------------
// creates the cube
// ------------------------------
#macro unitbox()
 union{
    difference{
        isosurface{
            function{f_rounded_box(x,y,z, 0.05, 0.5, 0.5, 0.5)}
            contained_by{box{-0.5,0.5}}
        }
        sphere{0,1 scale <0.45,0.45,0.05> translate z*-0.5}
        sphere{0,1 scale <0.45,0.45,0.05> translate z*0.5}
        sphere{0,1 scale <0.45,0.45,0.05> translate z*-0.5 rotate x*90}
        sphere{0,1 scale <0.45,0.45,0.05> translate z*0.5 rotate x*90}
        sphere{0,1 scale <0.45,0.45,0.05> translate z*-0.5 rotate y*90}
        sphere{0,1 scale <0.45,0.45,0.05> translate z*0.5 rotate y*90}

    }
    #declare Font="crystal"
    #declare sFont=<0.76,0.76,0.05>;
    #local P=text { ttf Font "P" 1, 0 scale sFont}
    #local O=text { ttf Font "O" 1, 0 scale sFont}
    #local V=text { ttf Font "V" 1, 0 scale sFont}
    #local R=text { ttf Font "R" 1, 0 scale sFont}
    #local A=text { ttf Font "A" 1, 0 scale sFont}
    #local Y=text { ttf Font "Y" 1, 0 scale sFont}
    union{
        object{P centertext(P) translate -0.5*z rotate y*90}
        object{O centertext(O) translate -0.5*z }
        object{V centertext(V) translate -0.5*z rotate -90*y}
        object{R centertext(R) translate -0.5*z rotate 180*y}
        object{A centertext(A) translate -0.5*z rotate 90*x}
        object{Y centertext(Y) translate -0.5*z rotate -90*x}
        txtBox()
    }
    txtBox()
}

#end
// ------------------------------
// cube texture
// ------------------------------
#declare rd=seed(0); // color random stream
#declare TG=array[6]
#declare TG[0]=texture{T_Gold_1E}
#declare TG[1]=texture{T_Gold_2E}
#declare TG[2]=texture{T_Gold_3E}
#declare TG[3]=texture{T_Gold_4E}
#declare TG[4]=texture{T_Gold_5D}
#declare TG[5]=texture{T_Gold_5E}
#macro txtBox()
    texture{TG[int(rand(rd)*6)]}
#end

// ------------------------------
// scene
// ------------------------------
#if (WriteFile)
    #include "makestacks.inc" // calls the stacking routine
#end
#include "stacks.inc"     // place the cubes








