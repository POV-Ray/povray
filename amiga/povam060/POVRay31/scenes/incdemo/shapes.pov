// Persistence Of Vision raytracer version 3.1 sample file.
// By Chris Young
// This image contains an example of every shape from SHAPES.INC

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"

camera {  
   location <24, 10, -36>
   direction <0, 0,  1.3>
   look_at 6*y
}

light_source {<200, 50, -100> color LightGray}

light_source {<50, 100, -200> color LightGray}

light_source {<200, 200, -200> color LightGray}

object {
   Ellipsoid
   scale <1, 2, 1>
   pigment {Red}
   translate <17, 2, -10>
}

object {
   Sphere
   pigment {Flesh}
   translate <20, 1, -11>
}

object {
   Cylinder_X
   pigment {Green}
   translate <0, 6, -2>
}

object {
   Cylinder_Y
   pigment {Blue}
   translate <18, 0, -8>
}

object {
   Cylinder_Z
   pigment {Cyan}
   translate <10, 3, 0>
}

object {
   QCone_X
   pigment {Orange}
   scale <9, 1, 1>
   translate <13, 10, -3>
}

object {
   QCone_Y
   pigment {Yellow}
   scale <1, 9, 1>
   translate <7, 10, -18>
}

object {
   QCone_Z
   pigment {Maroon}
   scale <1, 1, 9>
   translate <7, 15, -10>
}

object {
   Plane_YZ 
   pigment {Navy}
}

object {
   Plane_XZ 
   pigment {White}
}

object {
   Plane_XY inverse
   pigment {SkyBlue}
}

object {
   Paraboloid_X
   pigment {Magenta}
   translate <15, 15, -9>
}

object {
   Paraboloid_Y
   pigment {GreenYellow}
   translate <5, 11, -23>
}

object {
   Paraboloid_Z
   pigment {OrangeRed}
   translate <10, 18, -6>
}

object {
   Hyperboloid
   pigment {Tan}
   scale <7, 1, 1>
   translate <10, 2, -5>
}

object {
   Hyperboloid_Y
   pigment {NeonPink}
   scale <1, 6, 1>
   translate <3, 10, -10>
}

object {
   UnitBox
   pigment {Scarlet}
   translate <14, 1, -10>
}

object {
   Cube
   pigment {ForestGreen}
   translate <12, 1, -13>
}

object {
   Disk_X
   pigment {Coral}
   translate <10, 5, -24>
}

object {
   Disk_Y
   pigment {SeaGreen}
   translate <10, 7, -24>
}

object {
   Disk_Z
   pigment {Brass}
   translate <10, 9, -24>
}

object {
   Cone_X
   pigment {SpicyPink}
   translate <18, 1, -13>
}

object {
   Cone_Y
   pigment {SummerSky}
   translate <14, 1, -14>
}

object {
   Cone_Z
   pigment {Wheat}
   translate <12, 1, -17>
}
