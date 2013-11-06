// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: transforms.inc
//    Last updated: 2001.8.9
//    Description: Macros for dealing with transforms

#ifndef(TRANSFORMS_INC_TEMP)
#declare TRANSFORMS_INC_TEMP = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
   #debug "including transforms.inc\n"
#end

#include "math.inc"

// --------------------
// Transformation macros:
// --------------------
// Reorients and deforms object so original x axis points along A, original y along B,
// and original z along C.
#macro Shear_Trans(A, B, C)
   transform {
      matrix < A.x, A.y, A.z,
             B.x, B.y, B.z,
             C.x, C.y, C.z,
             0, 0, 0>
   }
#end

#macro Matrix_Trans(A, B, C, D)
   transform {
      matrix < A.x, A.y, A.z,
             B.x, B.y, B.z,
             C.x, C.y, C.z,
             D.x, D.y, D.z>
   }
#end


// "stretch" along a specific axis
#macro Axial_Scale_Trans(Axis, Amt)
   transform {
      transform {Point_At_Trans(Axis) inverse}
      scale <1,Amt,1>
      Point_At_Trans(Axis)
   }
#end


// rotate around a specific axis
// Author: Rune S. Johansen
#macro Axis_Rotate_Trans(Axis, Angle)
   #local vX = vaxis_rotate(x,Axis,Angle);
   #local vY = vaxis_rotate(y,Axis,Angle);
   #local vZ = vaxis_rotate(z,Axis,Angle);
   transform {
      matrix < vX.x,vX.y,vX.z, vY.x,vY.y,vY.z, vZ.x,vZ.y,vZ.z, 0,0,0 >
   }
#end


// Rotate around a specific point
#macro Rotate_Around_Trans(Rotation, Point)
   transform {
      translate -Point
      rotate Rotation
      translate Point
   }
#end


// based on original Reorient() macro by John VanSickle
#macro Reorient_Trans(Axis1, Axis2)
   #local vX1 = vnormalize(Axis1);
   #local vX2 = vnormalize(Axis2);
   #local Y = vcross(vX1, vX2);
   #if(vlength(Y) > 0)
      #local vY = vnormalize(Y);
      #local vZ1 = vnormalize(vcross(vX1, vY));
      #local vZ2 = vnormalize(vcross(vX2, vY));
      transform {
         matrix < vX1.x, vY.x,vZ1.x, vX1.y,vY.y,vZ1.y, vX1.z,vY.z, vZ1.z, 0,0,0 >
         matrix < vX2.x,vX2.y,vX2.z,  vY.x,vY.y, vY.z, vZ2.x,vZ2.y,vZ2.z, 0,0,0 >
      }
   #else
      #if (vlength(vX1-vX2)=0)
         transform {}
      #else
         #local vZ = VPerp_To_Vector(vX2);
         transform { Axis_Rotate_Trans(vZ,180) }
      #end
   #end
#end

// Similar to Reorient_Trans(), points y axis along Axis.
#macro Point_At_Trans(YAxis)
   #local Y = vnormalize(YAxis);
   #local X = VPerp_To_Vector(Y);
   #local Z = vcross(X, Y);
   Shear_Trans(X, Y, Z)
#end


// Calculates a transformation which will center the bounding box of Object
// along specified axis Axis
// Usage:
// object {MyObj
//     Center_Trans(MyObj, x) center along x axis
// You can also center along multiple axis:
//     Center_Trans(MyObj, x+y) center along x and y axis
#macro Center_Trans(Object, Axis)
   #local Mn = min_extent(Object);
   #local Mx = max_extent(Object);
   transform {translate -Axis*((Mx - Mn)/2 + Mn)}
#end

// Calculates a transformation which will align the bounding box
// of an object to a point. Negative values on Axis will align to
// the sides facing the negative ends of the coordinate system,
// positive values will align to the opposite sides, 0 means
// not to do any alignment on that axis.
// Usage:
// object {MyObj
//     Align_Trans(MyObj, x, Pt)
//     Align right side of object to be coplanar with Pt
//     Align_Trans(MyObj, -y, Pt)
//     Align bottom of object to be coplanar with Pt
#macro Align_Trans(Object, Axis, Pt)
   #local Mn = min_extent(Object);
   #local Mx = max_extent(Object);
   transform {
      #if(Axis.x < -0.5)
         translate x*(Pt - Mn.x)
      #else
         #if(Axis.x > 0.5)
            translate x*(Pt - Mx.x)
         #end
      #end
      
      #if(Axis.y < -0.5)
         translate y*(Pt - Mn.y)
      #else
         #if(Axis.y > 0.5)
            translate y*(Pt - Mx.y)
         #end
      #end
      
      #if(Axis.z < -0.5)
         translate z*(Pt - Mn.z)
      #else
         #if(Axis.z > 0.5)
            translate z*(Pt - Mx.z)
         #end
      #end
   }
#end

// Aligns an object to a spline for a given time value.
// The Z axis of the object will point in the forward direction
// of the spline and the Y axis of the object will point upwards.
// 
// usage:     object {MyObj Spline_Trans(MySpline, clock, y, 0.1, 0.5)}
// 
// Spline:    The spline that the object is aligned to.
// 
// Time:      The time value to feed to the spline, for example clock.
// 
// Sky:       The vector that is upwards in your scene, usually y.
// 
// Foresight: How much in advance the object will turn and bank.
//            Values close to 0 will give precise results, while higher
//            values give smoother results. It will not affect parsing
//            speed, so just find the value that looks best.
// 
// Banking:   How much the object tilts when turning. Note that the amount
//            of tilting is equally much controlled by the ForeSight value.
// 
// Author: Rune S. Johansen
#macro Spline_Trans (Spline, Time, Sky, Foresight, Banking)
   #local Location = <0,0,0>+Spline(Time);
   #local LocationNext = <0,0,0>+Spline(Time+Foresight);
   #local LocationPrev = <0,0,0>+Spline(Time-Foresight);
   #local Forward = vnormalize(LocationNext-Location);
   #local Right   = VPerp_To_Plane(Sky,Forward);
   #local Up      = VPerp_To_Plane(Forward,Right);
   #local Matrix = Matrix_Trans(Right,Up,Forward,Location)
   #local BankingRotation =
   degrees(atan2(
      VRotation(
         VProject_Plane((LocationNext-Location),Sky),
         VProject_Plane((Location-LocationPrev),Sky),
         Up
      )*Banking
      ,1
   ));
   transform {
      rotate BankingRotation*z
      transform Matrix
   }
#end


#macro vtransform(vec, trans)
   #local fn = function { transform { trans } }
   #local result = (fn(vec.x, vec.y, vec.z));
   result
#end

#macro vinv_transform(vec, trans)
   #local fn = function { transform { trans inverse } }
   #local result = (fn(vec.x, vec.y, vec.z));
   result
#end


#version TRANSFORMS_INC_TEMP;
#end
