// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: optics.pov
// Author: Christopher J. Huff
// Updated: 2013/02/15 for 3.7
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 } 

#include "colors.inc"

#default {finish {ambient 0}}

global_settings {
    assumed_gamma 1
    max_trace_level 5
    photons {
//        spacing 0.025
        count 150000
        max_trace_level 9
        media 100, 2
//        media 500, 3
    }
}

#declare CamPos = < 0, 18, 0>;

camera {
    location CamPos
    right x*image_width/image_height // keep propotions with any aspect ratio
    look_at < 0, 0, 0>
    angle 35
}

light_source {CamPos, color Gray25
    photons {refraction off reflection off}
    media_interaction off
}
light_source {<-150, 0.5, 0>, color rgb < 1.2, 1, 1.5>
    spotlight radius 0.3 falloff 0.35 point_at < 0, 0.5, 0>
    photons {refraction on reflection on}
}

#macro Block(From, To)
    union {
        cylinder {From, To*(x+z), 0.1 scale < 1, 10*To.y, 1>
            texture {
                pigment {checker color Gray90, color Gray70
                    scale 0.1
                }
                finish {brilliance 0.5}
            }
        }
        cylinder {From, To*(x+z), 0.025
            translate y*To
            texture {
                pigment {color rgb < 1, 0.7, 0.2>}
                finish {ambient 0.8}
            }
        }
    }
#end

box {<-100,-1,-100>, < 100, 0, 100>
    texture {
        pigment {checker color Gray90, color rgb < 0.2, 0, 0.4>}
        finish {brilliance 0.25}
    }
}
box {<-7,-0.1,-3>, < 6, 1, 4> hollow
    texture {pigment {color rgbf 1}}
    interior {
        media {
            scattering {1, color White extinction 0}
//            emission color White*0.2
            method 3
            intervals 1 samples 4
        }
    }
    photons {target}
}

union {
    difference {
        object {Block(<-4, 0,-3>, <-4, 1.5, 3>)}
        box {<-5, 0.25,-0.5>, < -3, 0.75, 0.5>}
    }
    cylinder {<-4, 0, 0>, <-4, 1.5, 0>, 0.1 translate z*0.15}
    cylinder {<-4, 0, 0>, <-4, 1.5, 0>, 0.1 translate -z*0.15}
    texture {pigment {color rgb 1}}
}

#declare MirrorTex1 =
texture {
    pigment {color White}
    finish {ambient 0 diffuse 0 reflection 1}
}
#declare HalfMirrorTex1 =
texture {
    pigment {color White filter 0.5}
    finish {ambient 0 diffuse 0 reflection 0.5}
}
#declare RedMirrorTex =
texture {
    pigment {color rgb < 0, 1, 1> filter 1}
    finish {ambient 0 diffuse 0 reflection Red}
}
#declare BlueMirrorTex =
texture {
    pigment {color rgb < 1, 1, 0> filter 1}
    finish {ambient 0 diffuse 0 reflection Blue}
}
#declare GlassTex1 =
texture {
    pigment {color White filter 0.99}
    finish {ambient 0 diffuse 0 reflection 0.01}
}
#declare GreenGlassTex1 =
texture {
    pigment {color Green filter 0.99}
    finish {ambient 0 diffuse 0 reflection 0.01}
}
#declare RedGlassTex1 =
texture {
    pigment {color Red filter 0.99}
    finish {ambient 0 diffuse 0 reflection 0.01}
}
#declare GlassInt1 =
interior {ior 1.33}

#macro PhotonTarget(Reflect, Refract, IgnorePhotons)
	photons {
		target
		reflection Reflect
		refraction Refract
		#if(IgnorePhotons) collect off #end
	}
#end


/*#declare Fn = function {sin(z*pi)/5 - x}
isosurface {
    function {Fn(x,y,z)}
    threshold 0
    eval
//    max_gradient 9.25
    contained_by {box {<-1, 0,-2>, < 1, 1, 2>}}
    texture {
        pigment {color White}
        finish {ambient 0 diffuse 0.2 reflection 0.8}
    }
    photons {target collect off}
    rotate -y*15
    translate < 2, 0, 0>
}*/

#macro Mirror(Pos, Ang, Width, Height, Tex)
	box {<-0.1,-0.1,-Width/2>, < 0, Height, Width/2>
	    texture {Tex}
//	    PhotonTarget(yes, yes, yes)
	    rotate -y*Ang
	    translate Pos
	}
#end

object {Mirror(<-3, 0, 0>, 3*45, 2, 1, BlueMirrorTex)}
object {Mirror(<-3, 0, 3>,-45, 2, 1, MirrorTex1)}

object {Mirror(<-1, 0, 0>, 180+22.5, 2, 1, RedMirrorTex)}
object {Mirror(<-3, 0,-2>, 22.5, 2, 1, MirrorTex1)}


//lenses
sphere {< 0, 0, 0>, 1
    texture {GlassTex1}
    interior {GlassInt1}
    PhotonTarget(no, yes, yes)
    scale < 0.475, 1, 1>
    translate < 1, 0.5, 0>
}

#declare T = 0.475*2;
#declare R = 1;
intersection {
    sphere {<-R, 0, 0>, R translate x*T/2}
    sphere {< R, 0, 0>, R translate -x*T/2}
    texture {GlassTex1}
    interior {GlassInt1}
    PhotonTarget(no, yes, yes)
    translate < 0, 0.5,-2>
}

#declare R = 1;
difference {
	cylinder {<-0.1, 0, 0>, < R, 0, 0>, R}
	sphere {< R, 0, 0>, R}
//	texture {pigment {color White}}
	texture {GlassTex1}
	interior {GlassInt1}
//	PhotonTarget(no, yes, yes)
	translate <-1, 0.5, 3>
}
