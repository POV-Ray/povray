// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: transmitfx.pov
// Last updated: 30/8/01
// Author: Rune S. Johansen
// Description:
// This file demonstrates how to create special effect filters
// by using "transmit" creatively, also outside of the 0 to 1 range.

// The shadows are a bit confusing, but try to render both
// with and without them.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#declare Shadows = off;

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
// A camera that looks down from above.
camera {
   orthographic
   location 6*y
   look_at 0
}

// A light.
light_source {<0,300,100>, color 1.5}

// And a nice blue checkered plane
plane {
   y, -3
   pigment {checker color <0.1,0.4,0.9>, color <0.1,0.6,0.9> scale 0.5}
}

union {
   box {<-5,-1, 0>, <5,-1, 0.9>}
   box {<-5,-1,-3>, <5,-1,-2.1>}
   pigment {color <0.0,0.2,0.5>}
   finish {ambient 1 diffuse 0}
   no_shadow
}

// A union with a brown sphere, a white torus
// and a disc with no texture.
#declare FilterAndSphere =
union {
   sphere {
      <0.3,-2,0.4>, 0.7
      pigment {color <0.9,0.6,0.1>}
      finish {phong 1}
   }
   torus {
      0.9, 0.05
      pigment {color rgb 1}
   }
   disc {0, y, 0.9} // the disc will be textured later
   #if (Shadows=off) no_shadow #end
   translate -0.2*z
}

// Just a macro to create the text labels
#macro Label (String1,String2,Location)
   union {
      text {
         ttf "crystal.ttf", String1, 0.1, <0,0>
         rotate 90*x scale 0.5 translate <-0.9,0,-1.5>
      }
      text {
         ttf "crystal.ttf", String2, 0.1, <0,0>
         rotate 90*x scale 0.5 translate <-0.9,0,-1.9>
      }
      clipped_by {plane {y, -0.01}}
      translate Location
      pigment {color rgb 1}
      finish {ambient 1 diffuse 0}
      no_shadow
   }
#end

// CENTER DISC: NO EFFECT
// With a transmit value of 1.0, things seen through this
// disc look the same.
object {
   FilterAndSphere

// This texture will apply only to the disc:
   pigment {color rgb 0.0 transmit 1.0}
   finish {ambient 1 diffuse 0}

   translate <-2,0,+2>
}
Label ("rgb 0.0","t   1.0",<-2,0,+2>)

// TOP DISC: DARK EFFECT
// With a black color and a transmit value of 0.5, things
// seen through this disc look darker.
object {
   FilterAndSphere
// This texture will apply only to the disc:
   pigment {color rgb 0.0 transmit 0.5}
   finish {ambient 1 diffuse 0}
   translate < 0,0,+2>
}
Label ("rgb 0.0","t   0.5",< 0,0,+2>)

// UPPER RIGHT DISC: BRIGHT EFFECT
// With a black color and a transmit value of 2.0, things
// seen through this disc look brighter.
object {
   FilterAndSphere
// This texture will apply only to the disc:
   pigment {color rgb 0.0 transmit 2.0}
   finish {ambient 1 diffuse 0}
   translate <+2,0,+2>
}
Label ("rgb 0.0","t   2.0",<+2,0,+2>)

// LOWER RIGHT DISC: CONTRAST DOWN
// With a gray color and a transmit value of 0.5, things
// seen through this disc have less contrast.
object {
   FilterAndSphere
// This texture will apply only to the disc:
   pigment {color rgb 0.5 transmit 0.5}
   finish {ambient 1 diffuse 0}
   translate <-3,0,-1>
}
Label ("rgb 0.5","t   0.5",<-3,0,-1>)

// BOTTOM DISC: CONTRAST UP
// With a gray color and a transmit value of 2.0, things
// seen through this disc have more contrast.
object {
   FilterAndSphere
// This texture will apply only to the disc:
   pigment {color rgb 0.5 transmit 2.0}
   finish {ambient 1 diffuse 0}
   translate <-1,0,-1>
}
Label ("rgb 0.5","t   2.0",<-1,0,-1>)

// LOWER LEFT DISC: TOTAL CONTRAST
// With a gray color and a transmit value of 1000, things
// seen through this disc have very high contrast.
object {
   FilterAndSphere
// This texture will apply only to the disc:
   pigment {color rgb 0.5 transmit 1000}
   finish {ambient 1 diffuse 0}
   translate <+1,0,-1>
}
Label ("rgb 0.5","t  1000",<+1,0,-1>)

// UPPER LEFT DISC: INVERT COLORS
// With a gray color and a transmit value of -1, things
// seen through this disc have inverted colors! Fun, eh?
object {
   FilterAndSphere
// This texture will apply only to the disc:
   pigment {color rgb 0.5 transmit -1}
   finish {ambient 1 diffuse 0}
   translate <+3,0,-1>
}
Label ("rgb 0.5","t  -1.0",<+3,0,-1>)
