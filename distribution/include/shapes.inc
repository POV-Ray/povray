// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: shapes.inc
//    Last updated: April-2013
//    Description: This file contains macros for working with objects, as well
//    as macros for creating special objects, such as bevelled text,
//    height fields, and rounded shapes.

#ifndef( Shapes_Inc_Temp )
#declare Shapes_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
   #debug "including shapes.inc\n"
#end

#include "shapes_old.inc"
#include "consts.inc"
#include "transforms.inc"
#include "strings.inc"
#include "math.inc"

// These macros are just interfaces to the trace() function.
// They return values through their parameters:
// If an intersection is found, they return true and set
// OPt to the intersection point, and ONorm to the normal.
// Otherwise they return false, and do not modify OPt or ONorm.
#macro Isect(Pt, Dir, Obj, OPt)
   #local Norm = <0,0,0>;
   #local IPt = trace(Obj, Pt, Dir, Norm);
   #if (vlength(Norm) > 0)
      #declare OPt = IPt;
      #local Return=true;
   #else
      #local Return=false;
   #end
   (Return)
#end
#macro IsectN(Pt, Dir, Obj, OPt, ONorm)
   #local Norm = <0,0,0>;
   #local IPt = trace(Obj, Pt, Dir, Norm);
   #if (vlength(Norm) > 0)
      #declare OPt = IPt;
      #declare ONorm = Norm;
      #local Return=true;
   #else
      #local Return=false;
   #end
   (Return)
#end


// A shortcut for getting both min and max extents of an object
#macro Extents(Obj, Min, Max)
   #declare Min = min_extent(Obj);
   #declare Max = max_extent(Obj);
#end


// shortcuts for using the CenterTrans and AlignTrans
// macros with objects.
#macro Center_Object(Object, Axis)
   object {Object Center_Trans(Object, Axis)}
#end

#macro Align_Object(Object, Axis, Pt)
   object {Object Align_Trans(Object, Axis, Pt)}
#end


// A simple beveled text macro. The parameters are:
// Font: the name of the font file.
// String: the text string the text object is composed of.
// Cuts: the number of times excess material is cut off, to form the bevel.
//       More cuts will give smoother results, but take longer to render.
// BevelAng: the angle of the bevel.
// BevelDepth: the depth of the bevelled portion of the text.
// Depth: the total depth of the text object.
// Offset: the offset value for the text object.  Since the front faces of each
//         letter need to be in the same plane, z values are ignored.
#macro Bevelled_Text(Font, String, Cuts, BevelAng, BevelDepth, Depth, Offset, UseMerge)
   #if(UseMerge)
      merge {
   #else
      union {
   #end
      text {ttf Font, String Depth-BevelDepth, Offset*(x+y)}
      intersection {
         #local J=0;
         #while(J<Cuts)
            #local A = 2*pi*J/(Cuts);
            #local CA = cos(radians(BevelAng));
            #local SA = sin(radians(BevelAng));
            text {ttf Font, String BevelDepth, Offset*(x+y)
               translate -z*(BevelDepth+J*0.0001)
               Shear_Trans(x, y, < cos(A)*SA, sin(A)*SA, CA>/CA)
            }
            #local J=J+1;
         #end
      }
      translate z*BevelDepth
   }
#end


// Constants used for the text macros
#declare Align_Left = 1;
#declare Align_Right = 2;
#declare Align_Center = 3;

/* Text_Space( Font, String, Size, Spacing )
Computes the width of a text string, including "white space". It
returns the advance widths of all n letters. Text_Space gives the
space a text or a glyph occupies in regard to its surroundings.

Font:    The font to use (see the documentation for the text object)
String:  The text for which we want to know the width
Size:    The size to which the text should be scaled
Spacing: The amount of space to add between the letters. */

#macro Text_Space(Font, String, Size, Spacing)
   #local TO = text {ttf Font concat("|",String,"|") 1 Spacing*x scale <Size,Size,1>}
   #local SO = text {ttf Font "||"                   1 Spacing*x scale <Size,Size,1>}
   ((max_extent(TO).x-min_extent(TO).x)-(max_extent(SO).x-min_extent(SO).x))
#end

/* Text_Width( Font, String, Size, Spacing )
Computes the width of a text string. It returns the advance widths
of the first n-1 letters, plus the glyph width of the last letter.
Text_Width gives the "fysical" width of the text and if you use
only one letter the "fysical" width of one glyph.

Font:    The font to use (see the documentation for the text object)
String:  The text for which we want to know the width
Size:    The size to which the text should be scaled
Spacing: The amount of space to add between the letters. */

#macro Text_Width(Font, String, Size, Spacing)
   #local TO = text {ttf Font String 1 Spacing*x scale <Size,Size,1>}
   (max_extent(TO).x-min_extent(TO).x)
#end

// Circle_Text author: Ron Parker
/* Circle_Text( Font, Text, Size, Spacing, Thickness, Radius, Inverted,
                Justification, Angle )
Creates a text object with the bottom (or top) of the character cells aligned
with all or part of a circle.  This macro should be used inside an object{...}
block.

         Font: The font to use (see the documentation for the text object)
         Text: The text string to be created
         Size: The height of the text string, as you would use to scale a
               standard text object
      Spacing: The amount of space to add between the letters.
    Thickness: The thickness of the letters (see the documentation for the
               text object)
       Radius: The radius of the circle along which the letters are aligned
     Inverted: If this parameter is nonzero, the tops of the letters will
               point toward the center of the circle.  Otherwise, the bottoms
               of the letters will do so.
Justification: One of the constants Align_Left, Align_Right, or Align_Center
        Angle: The point on the circle from which rendering will begin.  The
               +x direction is 0 and the +y direction is 90 (i.e. the angle
               increases anti-clockwise. */
 
#macro Circle_Text(F, T, S, Sp, Th, R, I, J, A) //----------------------------------------
object{ Circle_Text_Valigned( F, // Font, i.e.: "arial.ttf", 
                              T, // Text, i.e.: "POVRay",
                              S, // LetterSize,    i.e.:  0.75, 
                              Sp,// LetterSpacing, i.e.: 0.025,
                              Th,// Deepth,        i.e.: 15.00, 
                              R, // Radius,        i.e.: 1.25
                              I, // Inverted,      0 or 1
                              J, // Justification: Align_Left, Align_Right, or Align_Center  
                              A, // Circle angle
                              0  // Valign:  Rotates for vertical objects. 
                                 //   -90 = right side up, 90 = upside-down, 0 = horzontal.
                            ) }  //-------------------------------------------------------- 
#end //-------------------------------------------------------------- end macro Circle_Text

// Cicle_Text macro expanded by rotating the letters: 
#macro Circle_Text_Valigned( F, // Font, i.e.: "arial.ttf", 
                             T, // Text, i.e.: "POVRay",
                             S, // LetterSize,    i.e.:  0.75, 
                             Sp,// LetterSpacing, i.e.: 0.025,
                             Th,// Deepth,        i.e.: 15.00, 
                             R, // Radius,        i.e.: 1.25
                             I, // Inverted,      0 or 1
                             J, // Justification: Align_Left, Align_Right, or Align_Center  
                             A, // Circle angle
                             Valign// Valign:  Rotates the letters. -90 = right side up, 90 = upside-down, 0 = horzontal.
                           ) //----------------------------------------------------------------------------------------------  
   #local FW = Text_Width(F, T, S, Sp);
   #local TO = text {ttf F T 1 0 scale<S, S, 1>}
   #local TH = max_extent(TO).y;
   #local C = array[strlen(T)]
   #if(FW > 2*pi*R)
      #error concat("\n\n**** Text string \"", T, "\" is too long for a circle of the specified radius.\n\n\n")
   #end
   #local AW = -FW*180/pi/R;
   #local SA = A;
   #local EA = A + AW;
   #if(((J = Align_Right) & !I)|((J = Align_Left) & I))
      #local SA = A - AW;
      #local EA = A;
   #else
      #if(J = Align_Center)
         #local SA = A - AW/2;
         #local EA = A + AW/2;
      #end
   #end

   #local CI = 1;
   #while(CI <= strlen(T))
      #local OE = Text_Width(F, substr(T,CI,1), S, Sp);
      #local LW = Text_Width(F, substr(T,1,CI), S, Sp) - OE;
      #local LA = SA + AW*LW/FW + OE/2/FW*AW;
      #if(I)
         #local LA = EA - (LA - SA);
      #end
      #local TO = text {ttf F substr(T, CI, 1) Th 0 scale<S,S,1> rotate x*Valign}
      #if(I)
         #local C[CI-1] =
         object {TO
            rotate 180*z
            translate <OE/2, TH, 0>
            rotate -90*z
            translate R*x
            rotate LA*z
         }
      #else
         #local C[CI-1] =
         object {TO
            translate -OE/2*x
            rotate -90*z
            translate R*x
            rotate LA*z
         }
      #end
      #local CI = CI + 1;
   #end

   // Create the final object, a union of individual text object letters.
   union {
      #local CI=0;
      #while(CI < strlen(T))
         object {C[CI]}
         #local CI = CI + 1;
      #end
   }
// --------------------------------------------------------------------------------------
#end// of macro --------------------------------------------- end of macro Circle_Text_Valigned


#macro Wedge(Angle)
   #local A = clamp(Angle, 0, 360);
   #if(A < 180)
      difference {
         plane {-x, 0}
         plane {-x, 0 rotate y*A}
      }
   #else
      #if(A = 180)
         plane {-x, 0}
      #else
         intersection {
            plane {x, 0}
            plane {-x, 0 rotate y*A}
            inverse
         }
      #end
   #end
#end


#macro Spheroid(Center, Radius)
   sphere { 0, 1 scale Radius translate Center }
#end


#macro Supertorus(RMj, RMn, MajorControl, MinorControl, Accuracy, MaxGradient)
   #local CP = 2/MinorControl;
   #local RP = 2/MajorControl;
   isosurface {
      function { pow( pow(abs(pow(pow(abs(x),RP) + pow(abs(z),RP), 1/RP) - RMj),CP) + pow(abs(y),CP) ,1/CP) - RMn }
      threshold 0
      contained_by {box {<-RMj-RMn,-RMn,-RMj-RMn>, < RMj+RMn, RMn, RMj+RMn>}}
      #if(MaxGradient >= 1)
         max_gradient MaxGradient
      #else
         evaluate 1, 10, 0.1
      #end
      accuracy Accuracy
   }
#end


// Supercone author: Juha Nieminen
// A cone object where each end is an ellipse, you specify two radii
// for each end.
// SuperCone function: (x^2/a^2+y^2/b^2-1)*(1-z) + (x^2/c^2+y^2/d^2-1)*z = 0
//
// camera { location <6,5,-10> look_at 0 angle 35 }
// light_source { <100,100,-20>,1 }
// plane { y,-1.5 pigment { checker rgb 1, rgb .5 } }
// object { SuperCone(<0,-1.5,0>,1,2, <0,1.5,0>,1,.5)
//     pigment { rgb x } finish { specular .5 }
// }
#macro Supercone(PtA, A, B, PtB, C, D)
   intersection {
      quartic {
         <0, 0,  0,  0,  0,  0,  0,  B*B-2*B*D+D*D, 2*(B*D-B*B), B*B,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, A*A-2*A*C+C*C, 2*(A*C-A*A), A*A, 0,  0,  0,  0,
         -(A*A-2*A*C+C*C)*(B*B-2*B*D+D*D),
         -(2*((B*D-B*B)*(A*A-2*A*C+C*C)+(A*C-A*A)*(B*B-2*B*D+D*D))),
         -(B*B*(A*A-2*A*C+C*C)+4*(A*C-A*A)*(B*D-B*B)+A*A*(B*B-2*B*D+D*D)),
         -(2*(B*B*(A*C-A*A)+A*A*(B*D-B*B))), -A*A*B*B>
         sturm
      }
      cylinder {0, z, max(max(abs(A), abs(B)), max(abs(C), abs(D)))}

      bounded_by {cone {0, max(abs(A), abs(B)), z, max(abs(C), abs(D))}}

      #local Dirv = PtB - PtA;
      scale <1,1,vlength(Dirv)>
      #local Dirv = vnormalize(Dirv);
      #if(vlength(Dirv-<0,0,-1>)=0) scale <1,1,-1>
      #else Reorient_Trans(z, Dirv)
      #end
      translate PtA
   }
#end


// Connect two spheres with a cylinder.
// Derived from Connect() macro by John VanSickle
#macro Connect_Spheres(PtA, RadiusA, PtB, RadiusB)
   #local Axis = PtB - PtA;
   #local RadDif = RadiusA - RadiusB;
   #local Len = VDist(PtA, PtB);
   #local D2 = sqrt(f_sqr(Len) - f_sqr(RadDif));

   cone {
      PtA + Axis/Len*RadDif*RadiusA/Len, RadiusA*D2/Len,
      PtB + Axis/Len*RadDif*RadiusB/Len, RadiusB*D2/Len
   }
#end


#macro Wire_Box_Union(A, B, WireRadius)
   Wire_Box(A, B, WireRadius, no)
#end
#macro Wire_Box_Merge(A, B, WireRadius)
   Wire_Box(A, B, WireRadius, yes)
#end
#macro Wire_Box(A, B, WireRadius, UseMerge)
   #local AA = <min(A.x, B.x), min(A.y, B.y), min(A.z, B.z)>;
   #local BB = <max(A.x, B.x), max(A.y, B.y), max(A.z, B.z)>;

   #local Delta=abs(BB.x-AA.x)/2;
   #if (Delta<WireRadius)
      #warning "\nWire_Box() macro called with x-size < Radius,\nresults may not be as expected\n"

      #local AA = <AA.x+Delta, AA.y, AA.z>;
      #local BB = <BB.x-Delta, BB.y, BB.z>;
   #else
      #local AA = <AA.x+WireRadius, AA.y, AA.z>;
      #local BB = <BB.x-WireRadius, BB.y, BB.z>;
   #end

   #local Delta=abs(BB.y-AA.y)/2;
   #if (Delta<WireRadius)
      #warning "\nWire_Box() macro called with y-size < Radius,\nresults may not be as expected\n"

      #local AA = <AA.x, AA.y+Delta, AA.z>;
      #local BB = <BB.x, BB.y-Delta, BB.z>;
   #else
      #local AA = <AA.x, AA.y+WireRadius, AA.z>;
      #local BB = <BB.x, BB.y-WireRadius, BB.z>;
   #end

   #local Delta=abs(BB.z-AA.z)/2;
   #if (Delta<WireRadius)
      #warning "\nWire_Box() macro called with z-size < Radius,\nresults may not be as expected\n"

      #local AA = <AA.x, AA.y, AA.z+Delta>;
      #local BB = <BB.x, BB.y, BB.z-Delta>;
   #else
      #local AA = <AA.x, AA.y, AA.z+WireRadius>;
      #local BB = <BB.x, BB.y, BB.z-WireRadius>;
   #end

   #local LBF = AA;
   #local RBF = < BB.x, AA.y, AA.z>;
   #local RBB = < BB.x, AA.y, BB.z>;
   #local LBB = < AA.x, AA.y, BB.z>;
   #local LTF = < AA.x, BB.y, AA.z>;
   #local RTF = < BB.x, BB.y, AA.z>;
   #local RTB = BB;
   #local LTB = < AA.x, BB.y, BB.z>;

   #if(UseMerge)
      merge {
   #else
      union {
   #end
      sphere {LBF, WireRadius}

      #if (AA.x != BB.x)
         sphere {RBF, WireRadius}
      #end
      #if ((AA.x != BB.x) & (AA.z != BB.z))
         sphere {RBB, WireRadius}
      #end
      #if (AA.z != BB.z)
         sphere {LBB, WireRadius}
      #end

      #if (AA.y != BB.y)
         sphere {LTF, WireRadius}
      #end
      #if ((AA.x != BB.x) & (AA.y != BB.y))
         sphere {RTF, WireRadius}
      #end
      #if ((AA.x != BB.x) & (AA.y != BB.y) & (AA.z != BB.z))
         sphere {RTB, WireRadius}
      #end
      #if ((AA.y != BB.y) & (AA.z != BB.z))
         sphere {LTB, WireRadius}
      #end

      #if (AA.x != BB.x)
         cylinder {LBF, RBF, WireRadius}
         cylinder {LBB, RBB, WireRadius}
         cylinder {LTB, RTB, WireRadius}
         cylinder {LTF, RTF, WireRadius}
      #end

      #if (AA.y != BB.y)
         cylinder {LBF, LTF, WireRadius}
         cylinder {RBF, RTF, WireRadius}
         cylinder {RBB, RTB, WireRadius}
         cylinder {LBB, LTB, WireRadius}
      #end

      #if (AA.z != BB.z)
         cylinder {LTB, LTF, WireRadius}
         cylinder {LBB, LBF, WireRadius}
         cylinder {RTB, RTF, WireRadius}
         cylinder {RBB, RBF, WireRadius}
      #end
   }
#end

#macro Round_Box_Union(A, B, EdgeRadius)
   Round_Box(A, B, EdgeRadius, no)
#end
#macro Round_Box_Merge(A, B, EdgeRadius)
   Round_Box(A, B, EdgeRadius, yes)
#end
#macro Round_Box(A, B, EdgeRadius, UseMerge)
   #local AA = <min(A.x, B.x), min(A.y, B.y), min(A.z, B.z)>;
   #local BB = <max(A.x, B.x), max(A.y, B.y), max(A.z, B.z)>;

   #local Delta=abs(BB.x-AA.x)/2;
   #if (Delta<EdgeRadius)
      #warning "\nRound_Box() macro called with x-size < Radius,\nresults may not be as expected\n"

      #local AA = <AA.x+Delta, AA.y, AA.z>;
      #local BB = <BB.x-Delta, BB.y, BB.z>;
   #else
      #local AA = <AA.x+EdgeRadius, AA.y, AA.z>;
      #local BB = <BB.x-EdgeRadius, BB.y, BB.z>;
   #end

   #local Delta=abs(BB.y-AA.y)/2;
   #if (Delta<EdgeRadius)
      #warning "\nRound_Box() macro called with y-size < Radius,\nresults may not be as expected\n"

      #local AA = <AA.x, AA.y+Delta, AA.z>;
      #local BB = <BB.x, BB.y-Delta, BB.z>;
   #else
      #local AA = <AA.x, AA.y+EdgeRadius, AA.z>;
      #local BB = <BB.x, BB.y-EdgeRadius, BB.z>;
   #end

   #local Delta=abs(BB.z-AA.z)/2;
   #if (Delta<EdgeRadius)
      #warning "\nRound_Box() macro called with z-size < Radius,\nresults may not be as expected\n"

      #local AA = <AA.x, AA.y, AA.z+Delta>;
      #local BB = <BB.x, BB.y, BB.z-Delta>;
   #else
      #local AA = <AA.x, AA.y, AA.z+EdgeRadius>;
      #local BB = <BB.x, BB.y, BB.z-EdgeRadius>;
   #end

   #local LBF = AA;
   #local RBF = < BB.x, AA.y, AA.z>;
   #local RBB = < BB.x, AA.y, BB.z>;
   #local LBB = < AA.x, AA.y, BB.z>;
   #local LTF = < AA.x, BB.y, AA.z>;
   #local RTF = < BB.x, BB.y, AA.z>;
   #local RTB = BB;
   #local LTB = < AA.x, BB.y, BB.z>;

   #if(UseMerge)
      merge {
   #else
      union {
   #end
      sphere {LBF, EdgeRadius}

      #if (AA.x != BB.x)
         sphere {RBF, EdgeRadius}
      #end
      #if ((AA.x != BB.x) & (AA.z != BB.z))
         sphere {RBB, EdgeRadius}
      #end
      #if (AA.z != BB.z)
         sphere {LBB, EdgeRadius}
      #end

      #if (AA.y != BB.y)
         sphere {LTF, EdgeRadius}
      #end
      #if ((AA.x != BB.x) & (AA.y != BB.y))
         sphere {RTF, EdgeRadius}
      #end
      #if ((AA.x != BB.x) & (AA.y != BB.y) & (AA.z != BB.z))
         sphere {RTB, EdgeRadius}
      #end
      #if ((AA.y != BB.y) & (AA.z != BB.z))
         sphere {LTB, EdgeRadius}
      #end

      #if (AA.x != BB.x)
         cylinder {LBF, RBF, EdgeRadius}
         cylinder {LBB, RBB, EdgeRadius}
         cylinder {LTB, RTB, EdgeRadius}
         cylinder {LTF, RTF, EdgeRadius}
      #end

      #if (AA.y != BB.y)
         cylinder {LBF, LTF, EdgeRadius}
         cylinder {RBF, RTF, EdgeRadius}
         cylinder {RBB, RTB, EdgeRadius}
         cylinder {LBB, LTB, EdgeRadius}
      #end

      #if (AA.z != BB.z)
         cylinder {LTB, LTF, EdgeRadius}
         cylinder {LBB, LBF, EdgeRadius}
         cylinder {RTB, RTF, EdgeRadius}
         cylinder {RBB, RBF, EdgeRadius}
      #end

      box {AA-EdgeRadius*x, BB+EdgeRadius*x}
      box {AA-EdgeRadius*y, BB+EdgeRadius*y}
      box {AA-EdgeRadius*z, BB+EdgeRadius*z}
   }
#end

#macro Round_Cylinder_Union(A, B, Radius, EdgeRadius)
   Round_Cylinder(A, B, Radius, EdgeRadius, no)
#end
#macro Round_Cylinder_Merge(A, B, Radius, EdgeRadius)
   Round_Cylinder(A, B, Radius, EdgeRadius, yes)
#end
#macro Round_Cylinder(A, B, Radius, EdgeRadius, UseMerge)

   #if(UseMerge)
      merge {
   #else
      union {
   #end

      #if(Radius<EdgeRadius)
         #warning "\nRound_Cylinder() macro called with Radius < EdgeRadius,\nresults may not be as expected\n"

         #local AA = A + vnormalize(B - A)*Radius;
         #local BB = B + vnormalize(A - B)*Radius;

         cylinder {AA, BB, Radius}
         sphere {0, Radius translate AA }
         sphere {0, Radius translate BB }

      #else

         #local AA = A + vnormalize(B - A)*EdgeRadius;
         #local BB = B + vnormalize(A - B)*EdgeRadius;

         cylinder {A, B, Radius - EdgeRadius}
         cylinder {AA, BB, Radius}
         torus {Radius - EdgeRadius, EdgeRadius translate y*EdgeRadius
            Point_At_Trans(B - A)
            translate A
         }
         torus {Radius - EdgeRadius, EdgeRadius translate y*(vlength(A - B) - EdgeRadius)
            Point_At_Trans(B - A)
            translate A
         }

      #end
   }
#end


// Rounded cone with torus edges
// This shape will fit entirely within a cone given the same parameters.
#macro Round_Cone_Union(PtA, RadiusA, PtB, RadiusB, EdgeRadius)
   Round_Cone(PtA, RadiusA, PtB, RadiusB, EdgeRadius, no)
#end
#macro Round_Cone_Merge(PtA, RadiusA, PtB, RadiusB, EdgeRadius)
   Round_Cone(PtA, RadiusA, PtB, RadiusB, EdgeRadius, yes)
#end
#macro Round_Cone(PtA, RadiusA, PtB, RadiusB, EdgeRadius, UseMerge)
   #if(min(RadiusA, RadiusB) < EdgeRadius)
     #warning "\nRound_Cone() macro called with Radius < EdgeRadius,\nresults may not be as expected\n"
   #end

   #if(RadiusA > RadiusB)
      #local RA = RadiusB;
      #local RB = RadiusA;
      #local PA = PtB;
      #local PB = PtA;
   #else
      #local RA = RadiusA;
      #local RB = RadiusB;
      #local PA = PtA;
      #local PB = PtB;
   #end

   #local Axis = vnormalize(PB - PA);
   #local Len = VDist(PA, PB);
   #local SA = atan2(RB - RA, Len);

   #if(UseMerge)
      merge {
   #else
      union {
   #end
      #local R1 = RA - EdgeRadius*tan(pi/4 - SA/2);
      #local R2 = RB - EdgeRadius/tan(pi/4 - SA/2);

      torus {R1, EdgeRadius
         Point_At_Trans(Axis) translate PA + Axis*EdgeRadius
      }
      torus {R2, EdgeRadius
         Point_At_Trans(Axis) translate PB - Axis*EdgeRadius
      }

      #local D1 = EdgeRadius - EdgeRadius*sin(SA);
      #local D2 = EdgeRadius + EdgeRadius*sin(SA);

      cone {
         PA + Axis*D1, R1 + EdgeRadius*cos(SA),
         PB - Axis*D2, R2 + EdgeRadius*cos(SA)
      }

      cone {PA, R1, PB, R2}
   }
#end


// Cones with spherical caps
// Sphere-capped cone object with spheres centered on end points.
// Derived from Connect() macro by John VanSickle
#macro Round_Cone2_Union(PtA, RadiusA, PtB, RadiusB)
   Round_Cone2(PtA, RadiusA, PtB, RadiusB, no)
#end
#macro Round_Cone2_Merge(PtA, RadiusA, PtB, RadiusB)
   Round_Cone2(PtA, RadiusA, PtB, RadiusB, yes)
#end
#macro Round_Cone2(PtA, RadiusA, PtB, RadiusB, UseMerge)
   #local Axis = PtB - PtA;
   #local RadDif = RadiusA - RadiusB;
   #local Len = VDist(PtA, PtB);

   #local D2 = f_sqr(Len) - f_sqr(RadDif);
   #if(D2<0)
     #error "Round_Cone2() macro called with parameters that can't be handled correctly"
   #end
   #local D2 = sqrt(D2);

   #if(UseMerge)
      merge {
   #else
      union {
   #end
      sphere {PtA, RadiusA}
      sphere {PtB, RadiusB}

      cone {
         PtA + Axis/Len*RadDif*RadiusA/Len, RadiusA*D2/Len,
         PtB + Axis/Len*RadDif*RadiusB/Len, RadiusB*D2/Len
      }
   }
#end

// Sphere-capped cone object with spheres moved and resized
// to fit ends of cone.
// The cone portion is identical to what you would get using
// a cone object with the same parameters, but the spheres are
// not centered on the endpoints of the cone, but are moved
// to give a smooth transition with the surface
#macro Round_Cone3_Union(PtA, RadiusA, PtB, RadiusB)
   Round_Cone3(PtA, RadiusA, PtB, RadiusB, no)
#end
#macro Round_Cone3_Merge(PtA, RadiusA, PtB, RadiusB)
   Round_Cone3(PtA, RadiusA, PtB, RadiusB, yes)
#end
#macro Round_Cone3(PtA, RadiusA, PtB, RadiusB, UseMerge)
   #local Axis = vnormalize(PtB - PtA);
   #local Len = VDist(PtA, PtB);
   #local SA = atan2(RadiusB - RadiusA, Len);

   #if(UseMerge)
      merge {
   #else
      union {
   #end
      cone {PtA, RadiusA, PtB, RadiusB}
      sphere {PtA + Axis*tan(SA)*RadiusA, RadiusA/cos(SA)}
      sphere {PtB + Axis*tan(SA)*RadiusB, RadiusB/cos(SA)}
   }
#end

// Two-triangle quad
//  A---B
//  |\  |
//  | \ |
//  |  \|
//  D---C
#macro Quad(A, B, C, D)
   triangle {A, B, C}
   triangle {A, C, D}
#end
#macro Smooth_Quad(A, NA, B, NB, C, NC, D, ND)
   smooth_triangle {A, NA, B, NB, C, NC}
   smooth_triangle {A, NA, C, NC, D, ND}
#end


// HF Macros author: Rune S. Johansen
// Optimizations by: Wlodzimierz ABX Skiba
// There are several HF macros in shapes.inc, which generate meshes in various shapes.
// See more information in the help file.

#macro HF_Square (Function,UseUVheight,UseUVtexture,Res,Smooth,FileName,MnExt,MxExt)
   #local WriteFile = (strlen(FileName) > 0);
   #local xRes = (< 1, 1>*Res).x;
   #local zRes = (< 1, 1>*Res).y;
   #local UVheight  = (UseUVheight=1);
   #local UVtex = (UseUVtexture=1);
   #local Smooth = (Smooth=1);

   #local Ext = MxExt-MnExt;

   // CALCULTION OF POINT GRID
   // Note that the grid extents one element further in all directions
   // if a smooth heightfield is calculated. This is to ensure correct
   // normal calculation later on.
   #local PArr = array[xRes+1+Smooth][zRes+1+Smooth]
   #local J = 1-Smooth;
   #while (J<xRes+1+Smooth)
      #local K = 1-Smooth;
      #while (K<zRes+1+Smooth)

         #local UV = <(J-1)/(xRes-1),0,(K-1)/(zRes-1)>;

         #local P  = (UV*Ext*<1,0,1> + MnExt);

         #if (UVheight)
            #local H = Function(UV.x, UV.z, 0);
         #else
            #local H = Function(P.x, P.y, P.z);
         #end

         #declare PArr[J][K] = P + H*Ext*y;

         #declare K = K+1;
      #end
      #declare J = J+1;
   #end

   HFCreate_()
#end

#macro HF_Sphere (Function,UseUVheight,UseUVtexture,Res,Smooth,FileName,Center,Radius,Depth)
   #local WriteFile = (strlen(FileName) > 0);
   #local xRes = (< 1, 1>*Res).x;
   #local zRes = (< 1, 1>*Res).y;
   #local UVheight  = (UseUVheight=1);
   #local UVtex = (UseUVtexture=1);
   #local Smooth = (Smooth=1);

   // CALCULTION OF POINT GRID
   // Note that the grid extents one element further in all directions
   // if a smooth heightfield is calculated. This is to ensure correct
   // normal calculation later on.
   #local PArr = array[xRes+1+Smooth][zRes+1+Smooth]
   #local J = 1-Smooth;
   #while (J<xRes+1+Smooth)
      #local K = 1-Smooth;
      #while (K<zRes+1+Smooth)

         #local UV = <(J-1)/(xRes-1),0,(K-1)/(zRes-1)>;

         #local Dir = vrotate( vrotate(x,(-89.9999+179.9998*UV.z)*z), -360*UV.x*y );
         #local P  = Center + Dir * Radius;

         #if (UVheight)
            #local H = Function(UV.x, UV.z, 0);
         #else
            #local H = Function(P.x, P.y, P.z);
         #end

         #declare PArr[J][K] = P + H*Dir*Depth;

         #declare K = K+1;
      #end
      #declare J = J+1;
   #end

   HFCreate_()
#end

#macro HF_Cylinder (Function,UseUVheight,UseUVtexture,Res,Smooth,FileName,EndA,EndB,Radius,Depth)
   #local WriteFile = (strlen(FileName) > 0);
   #local xRes = (< 1, 1>*Res).x;
   #local zRes = (< 1, 1>*Res).y;
   #local UVheight  = (UseUVheight=1);
   #local UVtex = (UseUVtexture=1);
   #local Smooth = (Smooth=1);

   #local Axis = EndB-EndA;
   #local Base = VPerp_To_Vector(Axis);

   // CALCULTION OF POINT GRID
   // Note that the grid extents one element further in all directions
   // if a smooth heightfield is calculated. This is to ensure correct
   // normal calculation later on.
   #local PArr = array[xRes+1+Smooth][zRes+1+Smooth]
   #local J = 1-Smooth;
   #while (J<xRes+1+Smooth)
      #local K = 1-Smooth;
      #while (K<zRes+1+Smooth)

         #local UV = <(J-1)/(xRes-1),0,(K-1)/(zRes-1)>;

         #local Dir = vaxis_rotate(Base,Axis,-360*UV.x-90);
         #local P  = EndA+Axis*UV.z+Dir*Radius;

         #if (UVheight)
            #local H = Function(UV.x, UV.z, 0);
         #else
            #local H = Function(P.x, P.y, P.z);
         #end

         #declare PArr[J][K] = P + H*Dir*Depth;

         #declare K = K+1;
      #end
      #declare J = J+1;
   #end

   HFCreate_()
#end

#macro HF_Torus (Function,UseUVheight,UseUVtexture,Res,Smooth,FileName,Major,Minor,Depth)
   #local WriteFile = (strlen(FileName) > 0);
   #local xRes = (< 1, 1>*Res).x;
   #local zRes = (< 1, 1>*Res).y;
   #local UVheight  = (UseUVheight=1);
   #local UVtex = (UseUVtexture=1);
   #local Smooth = (Smooth=1);

   // CALCULTION OF POINT GRID
   // Note that the grid extents one element further in all directions
   // if a smooth heightfield is calculated. This is to ensure correct
   // normal calculation later on.
   #local PArr = array[xRes+1+Smooth][zRes+1+Smooth]
   #local J = 1-Smooth;
   #while (J<xRes+1+Smooth)
      #local K = 1-Smooth;
      #while (K<zRes+1+Smooth)

         #local UV = <(J-1)/(xRes-1),0,(K-1)/(zRes-1)>;

         #local Dir = vrotate(vrotate(-x,360*UV.z*z),-360*UV.x*y);
         #local P  = vrotate(Major*x,-360*UV.x*y)+Dir*Minor;

         #if (UVheight)
            #local H = Function(UV.x, UV.z, 0);
         #else
            #local H = Function(P.x, P.y, P.z);
         #end

         #declare PArr[J][K] = P + H*Dir*Depth;

         #declare K = K+1;
      #end
      #declare J = J+1;
   #end

   HFCreate_()
#end

// Internal macro - not intended to be called by user.
#macro HFCreate_ ()

   #if(WriteFile)
      #fopen _HFMACRO_OUTPUT_FILE FileName write
      #write(_HFMACRO_OUTPUT_FILE,"mesh2 {\nvertex_vectors {\n",xRes*zRes,
   #else
      mesh2 {vertex_vectors{xRes*zRes,
   #end

   #local J = 1;
   #while (J<=xRes)
      #local K = 1;
      #while (K<=zRes)
        #if(WriteFile)
          ",\n",PArr[J][K],
        #else
          PArr[J][K],
        #end
         #declare K = K+1;
      #end
      #declare J = J+1;
   #end

   #if(WriteFile)
      "\n}\n")
   #else
      }
   #end

   #if (Smooth)
      #if(WriteFile)
         #write(_HFMACRO_OUTPUT_FILE,"normal_vectors {\n",xRes*zRes,
      #else
         normal_vectors{xRes*zRes,
      #end

      // CALCULATION OF NORMAL VECTOR
      // We don't vnormalize the vectors from the current center point
      // to its neightbor points because we want a weighted average
      // where bigger areas contribute more. This also means that the
      // center point can be left out completely of the calculations:
      #local J = 1;
      #while (J<=xRes)
         #local K = 1;
         #while (K<=zRes)
           #if(WriteFile)
             ",\n",vnormalize(vcross(PArr[J][K+1]-PArr[J][K-1], PArr[J+1][K]-PArr[J-1][K])),
           #else
             vnormalize(vcross(PArr[J][K+1]-PArr[J][K-1], PArr[J+1][K]-PArr[J-1][K])),
           #end
            #declare K = K+1;
         #end
         #declare J = J+1;
      #end
      #if(WriteFile)
         "\n}\n")
      #else
         }
      #end
   #end

   #if (UVtex)
      #if(WriteFile)
         #write(_HFMACRO_OUTPUT_FILE,"uv_vectors {\n",xRes*zRes,
      #else
         uv_vectors{xRes*zRes,
      #end
      #local J = 1;
      #while (J<=xRes)
         #local K = 1;
         #while (K<=zRes)
           #if(WriteFile)
             ",\n",<(J-1)/(xRes-1),(K-1)/(zRes-1)>,
           #else
             <(J-1)/(xRes-1),(K-1)/(zRes-1)>,
           #end
            #declare K = K+1;
         #end
         #declare J = J+1;
      #end
      #if(WriteFile)
         "\n}\n")
      #else
         }
      #end
   #end

   #if(WriteFile)
      #write(_HFMACRO_OUTPUT_FILE,"face_indices {\n",(xRes-1)*(zRes-1)*2,
   #else
      face_indices{(xRes-1)*(zRes-1)*2,
   #end
   #local F1 = <0,zRes,zRes+1>;
   #local F2 = <0,zRes+1,1>;
   #local J = 0;
   #while (J<xRes-1)
      #local A = J*zRes;
      #while (mod(A+1,zRes))
        #if(WriteFile)
          ",\n",F1+A,",\n",F2+A,
        #else
          F1+A, F2+A,
        #end
         #local A = A+1;
      #end
      #local J = J+1;
   #end
   #if (UVtex)
      #if(WriteFile)
         "\n}\nuv_mapping\n}")
         #fclose _HFMACRO_OUTPUT_FILE
      #else
         } uv_mapping}
      #end
   #else
      #if(WriteFile)
         "\n}\n}")
         #fclose _HFMACRO_OUTPUT_FILE
      #else
         }}
      #end
   #end

#end

#version Shapes_Inc_Temp;
#end//shapes.inc
