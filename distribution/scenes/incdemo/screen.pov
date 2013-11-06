// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer Scene Description File
// File: screen.pov
// Desc: screen.inc demo scene
// Date: August 2001
// Auth: Christoph Hormann, Chris Huff and Rune S. Johansen.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

#include "screen.inc"

global_settings {charset utf8 assumed_gamma 1.0}

// Screen.inc will enable you to place objects and textures right in front
// of the camera. One use of this is to place your signature or a logo in
// the corner of the image.

// You can only use screen.inc with the perspective camera. Screen.inc
// will automatically create the camera definition for you.

// Note that even though objects aligned using screen.inc follow the
// camera, they are still part of the scene. That means that they will be
// affected by perspective, lighting, the surroundings etc.


// Example of use

// The include file sets default values for everything, but you can use
// several macros to override these settings.

Set_Camera(<25,15,-33>, <2,2,0>, 70) // set location, look_at and angle
Set_Camera_Aspect(4,3) // set width in relation to height
Set_Camera_Sky(<0,1,0.3>) // set the sky vector

// After calling these setup macros you can use the macros Screen_Object
// and Screen_Plane which are described below.

// Screen_Object is a macro that will place an object right in front of
// the camera. You use it as follows:
//
// Screen_Object ( Object, Coords, Spacing, Confine, Scaling )
//
// Object  - The object to place in front of the screen.
//
// Coords  - UV coordinates for the object. <0,0> is lower left corner of
//           the screen and <1,1> is upper right corner.
//
// Spacing - Float describing minimum distance from object to the borders.
//           UV vector can be used to get different horizontal and
//           vertical spacing.
//
// Confine - Set to true to confine objects to visible screen area. Set to
//           false to allow objects to be outside visible screen area.
//
// Scaling - If the object intersects or interacts with the scene, try to
//           move it closer to the camera by decreasing Scaling.

#declare MyTextObject =
text {
   ttf "crystal.ttf", "target: enemy base", 0.01, <0,0>
   scale 0.08
   pigment {color <1.0,0.5,0.2>}
   finish {ambient 1 diffuse 0}
}

// Place MyTextObject in the right bottom corner
// with spacing 0.04 horizontally and 0.02 vertically.
// Confine object to visible area and scale it by 0.01.
Screen_Object ( MyTextObject, <1,0>, <0.04,0.02>, true, 0.01 )

#declare MyCrosshair =
union {
   torus {
      0.2, 0.02 rotate 90*x
      pigment {color rgbf <1.0,0.5,0.2,1.0>}
      finish {reflection {0.3} specular 1 roughness 0.03}
      interior {ior 1.3}
   }
   union {
      #declare C = 0;
      #while(C < 10)
         cone { 0.15*x, 0,  0.20*x, 0.015 rotate z*C*9}
         cone {-0.15*x, 0, -0.20*x, 0.015 rotate z*C*9}
         #declare C = C + 1;
      #end
      cylinder {0.03*x, 0.2*x, 0.01 rotate 000*z}
      cylinder {0.03*x, 0.2*x, 0.01 rotate 090*z}
      cylinder {0.03*x, 0.2*x, 0.01 rotate 180*z}
      cylinder {0.03*x, 0.2*x, 0.01 rotate 270*z}
   }
   pigment {color <1.0,0.4,0.1>}
   finish {brilliance 2 reflection {0.5 metallic} specular 0.5}
}

// Place MyCrosshair in the center of the image with no spacing.
// Confine object to visible area and scale it by 0.01.
Screen_Object ( MyCrosshair, <0.5,0.5>, 0, true, 0.01 )

// Screen_Plane is a macro that will place a texture of your choice on a
// plane right in front of the camera. You use it as follows:
//
// Screen_Plane ( Texture, Scaling, BLCorner, TRCorner )
//
// Texture  - The texture to be displayed on the camera plane. <0,0,0> is
//            lower left corner and <1,1,0> is upper right corner.
//
// Scaling  - If the plane intersects or interacts with the scene, try to
//            move it closer to the camera by decreasing Scaling.
//
// BLCorner - The bottom left corner of the Screen_Plane.
// TRCorner - The top right corner of the Screen_Plane.

#declare MyScreenTexture = // screen texture with black borders
texture {
   pigment {
      boxed
      color_map {
         [0.0, color rgb 0 transmit 0]
         [0.2, color rgb 0 transmit 1]
      }
      scale 0.5 translate <0.5,0.5,0>
   }
   finish {ambient 1 diffuse 0}
}

// Make screen plane with the texture MyScreenTexture and scale
// it by 0.02 to bring it closer to the camera.
// Make the texture fill out the entire screen from <0,0> to <1,1>.
Screen_Plane ( MyScreenTexture, 0.02, <0,0>, <1,1> )

#declare MyScreenTexture2 = // screen texture with test bitmap
texture {
   pigment {
      image_map {
         png "test.png" once
         transmit 0, 0.8
         transmit 1, 0.8
         transmit 2, 0.8
         transmit 3, 0.8
         transmit 4, 0.4
         transmit 5, 0.4
      }
   }
   finish {ambient 1 diffuse 0}
}

// Make screen plane with the texture MyScreenTexture2 and scale
// it by 0.03 to bring it closer to the camera.
// Make the texture fill out the area of the screen from <0.1,0.5> to <0.5,0.9>.

// Screen_Plane ( MyScreenTexture2, 0.03, <0.1,0.5>, <0.5,0.9> )


// Below are some simple scene elements

sky_sphere {
   pigment {
      gradient y
      color_map {
         [0.0, color <0.6,0.7,1.0>]
         [0.5, color <0.2,0.4,0.8>]
         [1.0, color <0.1,0.2,0.5>]
      }
   }
}

light_source {<3,9,-5>*1000, color rgb 1.2}

fog {
   fog_type 2
   fog_offset 0
   fog_alt 3
   distance 40
   color <0.47,0.55,0.70>
}

// A plane with a grayish-brown center and green further out.
plane {
   y, 0
   pigment {
      boxed
      warp {turbulence 0.3}
      scale 22
      color_map {
         [0.0, color <0.5,0.8,0.2>]
         [0.2, color <0.7,0.6,0.5>]
      }
   }
}

// Random gray boxes
#declare S1 = seed(1);
#declare S2 = seed(12);
#declare S3 = seed(123);
#declare C = 0;
#while (C<50)
   box {
      <-1,0,-1>, <1,2,1>
      scale 1.5*<0.5+rand(S1),0.2+1.3*rand(S1),0.5+rand(S1)>
      translate <-20+40*rand(S2),0,-20+40*rand(S2)>
      pigment {color rgb 0.6+0.5*rand(S3)}
   }
   #declare C = C+1;
#end
