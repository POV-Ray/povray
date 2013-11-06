// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File

#ifndef(Shapes_Old_Inc_Temp)
#declare Shapes_Old_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including shapes_old.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.5

                               IMPORTANT!
   This collection of standard shapes has been around since the days
   of DKB-Trace and early versions of POV-Ray.  Those versions had no
optomized primatives for planes, cones, disks etc.  Some of the definitions
  below may seem trivial or unnecessary given the POV-Ray 2.0 and higher
 object primatives.  We have retained these objects for compatibility with
                           earlier versions.

 With the release of POV-Ray 1.0, some of these shapes, in particular,
the "Disk_?" group, were changed from an earlier beta test and DKB-Trace
   style.  The file "shapes.old" is also included in this package for
                   compatibility with pre-1.0 scenes.

*/
//2001.7.27: renamed as shapes_old.inc for POV 3.5. The new shapes.inc
//file includes this file, so backwards compatability is preserved.


#declare Ellipsoid =
 sphere {<0, 0, 0>,1}

#declare Sphere =
 sphere {<0, 0, 0>,1}

#declare Cylinder_X =
 quadric
  {<0, 1, 1>,
   <0, 0, 0>,
   <0, 0, 0>, -1
  }

#declare Cylinder_Y =
 quadric
  {<1, 0, 1>,
   <0, 0, 0>,
   <0, 0, 0>, -1
  }

#declare Cylinder_Z =
 quadric
  {<1, 1, 0>,
   <0, 0, 0>,
   <0, 0, 0>, -1
  }

// Infinite cones
#declare QCone_X =
 quadric
  {<-1, 1, 1>,
   < 0, 0, 0>,
   < 0, 0, 0>, 0
  }

#declare QCone_Y =
 quadric
  {<1, -1, 1>,
   <0, 0, 0>,
   <0, 0, 0>, 0
  }

#declare QCone_Z =
 quadric
  {<1, 1, -1>,
   <0, 0, 0>,
   <0, 0, 0>, 0
  }

// Unit cones    
// The Cone_n objects were formerly defined as intersections of
// quadrics and boxes but now can be redefined with the cone primative.

#declare Cone_X = cone {x,0,-x,1}
#declare Cone_Y = cone {y,0,-y,1}
#declare Cone_Z = cone {z,0,-z,1}

// The Plane_nn objects were formerly defined as quadrics but now can
// be redefined as a plane.

#declare Plane_YZ = plane {x,0}
#declare Plane_XZ = plane {y,0}
#declare Plane_XY = plane {z,0}

/* y^2 + z^2 - x = 0 */
#declare Paraboloid_X =
 quadric
  {< 0, 1, 1>,
   < 0, 0, 0>,
   <-1, 0, 0>, 0
  }

/* x^2 + z^2 - y = 0 */
#declare Paraboloid_Y =
 quadric
  {<1,  0,  1>,
   <0,  0,  0>,
   <0, -1,  0>, 0
  }

/* x^2 + y^2 - z = 0 */
#declare Paraboloid_Z =
 quadric
  {<1,  1,  0>,
   <0,  0,  0>,
   <0,  0, -1>, 0
  }

/* y - x^2 + z^2 = 0 */
#declare Hyperboloid =
 quadric
  {<-1,  0,  1>,
   < 0,  0,  0>,
   < 0,  1,  0>, 0
  }

#declare Hyperboloid_Y =
 quadric                 /* Vertical hyperboloid */
  {<1, -1,  1>,          /*                      */
   <0,  0,  0>,          /*            \   /     */
   <0,  0,  0>, -1       /* Like this:  ) (      */
  }                      /*            /   \     */

// Cube using the procedural box primitive
#declare UnitBox = box { <-1, -1, -1>, <1, 1, 1> }

// This primitive used to be an intersection of six planes.  For speed,
// it is now a box and nothing else.
#declare Cube = box { <-1, -1, -1>, <1, 1, 1> }

// The Disk primitives are "capped" cylinders of unit length.
//
// They are now "unit" size, the same as a sphere with a radius of 1.
// They will now scale evenly in all directions.

#declare Disk_X =    /* Capped cylinder, Length in x axis */
 cylinder { x,-x,1}

#declare Disk_Y =    /* Capped cylinder, Length in y axis */
 cylinder { y,-y,1}

#declare Disk_Z =    /* Capped cylinder, Length in z axis */
 cylinder { z,-z,1}

#version Shapes_Old_Inc_Temp;
#end
