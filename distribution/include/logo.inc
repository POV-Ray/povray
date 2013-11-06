// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//	Persistence of Vision Raytracer Version 3.5 Scene Description File

#ifndef(Logo_Inc_Temp)
#declare Logo_Inc_Temp=version;
#version 3.5;

/*
The official POV-Ray Logo
Designed by Chris Colefax

This logo was the one that won the POV-Ray Logo Contest
held and organized by Rune S. Johansen in year 2000 and
monitored by the POV-Team.

Comment from Chris Colefax:

"This logo (or symbol, rather) attempts to encapsulate a
number of ideas. Firstly, the symbol represents an eye with
a ray being traced from it, capturing the fundamentally
visual nature of POV-Ray and the rendering process POV-Ray
uses.  More obviously,  it's a P, and both an O and a V can
be seen in it as well, making the symbol specific to (and
immediately identifiable with) POV-Ray, rather than
raytracing in general.  The basic symbol itself is a simple
CSG construction composed of five POV-Ray primitives.  The
simplicity of the symbol means that it can be presented
recognisably in a range of ways, from a graphical 2D logo
to a full blown rendering, or incorporated discreetly into
other designs and images."


Prism version and bevel version by Rune S. Johansen.

*/

#include "shapes.inc"

// The original version is made of various objects.
#declare Povray_Logo =
merge {
   sphere {2*y, 1}
   difference {
      cone {2*y, 1, -4*y, 0}
      sphere {2*y, 1.4 scale <1,1,2>}
   }
   difference {
      sphere {0, 1 scale <2.6, 2.2, 1>}
      sphere {0, 1 scale <2.3, 1.8, 2> translate <-0.35, 0, 0>}
      rotate z*30 translate 2*y
   }
   rotate <0, 0, -25>
   translate <-0.5,-0.35,0>
   scale 1/4
}

// The prism version is not really made of a prism object alone,
// but it looks that way.
#declare Povray_Logo_Prism =
merge {
   cylinder {-z, z, 1 translate 2*y}
   difference {
      prism {-0.9999, 0.9999, 4, <-1,2>, <1,2>, <0,-4>, <-1,2> rotate -90*x}
      cylinder {-z, z, 1.4 scale <1,0.9,1.1> translate 1.9*y}
   }
   difference {
      cylinder {-z, z, 1 scale <2.6, 2.2, 1>}
      cylinder {-z, z, 1 scale <2.22, 1.72, 1.1> translate <-0.43, 0, 0>}
      rotate z*30 translate 2*y
   }
   rotate <0, 0, -25>
   translate <-0.5,-0.35,0>
   scale <1/4,1/4,1/8>
}

// The bevel version has its base at the xy plane and
// is beveled in 45 degrees in the -z direction.
// Intersect the object with a plane to get a flat object
// with beveled edges.
#declare Povray_Logo_Bevel =
merge {
   cone {0, 1, z, 0 translate 2*y}
   difference {
      box {<-0.8,-4,0>, <0.8,0.9,0.7>}
      object{Supercone(-0.0001*z, 1.4,1.26, 1.0001*z, 2.4,2.26) translate 1.9*y}
      plane {vrotate( x-z, degrees(atan2(1/6,1))*z), 0 translate -4*y}
      plane {vrotate(-x-z,-degrees(atan2(1/6,1))*z), 0 translate -4*y}
   }
   difference {
      object{Supercone(0, 2.6,2.2, z, 1.6,1.2)}
      object{Supercone(-0.0001*z, 2.22,1.72, 1.0001*z, 3.22,2.72) translate -0.43*x}
      clipped_by {
         difference {
            cylinder { 0.0*z, 0.5*z, 1 scale <2.60,2.20,1.0>}
            cylinder {-0.1*z, 0.6*z, 1 scale <2.22,1.72,1.1> translate -0.43*x}
         }
      }
      rotate z*30 translate 2*y
   }
   rotate <0, 0, -25>
   translate <-0.5,-0.35,0>
   scale <1,1,-1>/4
}

#version Logo_Inc_Temp;
#end
