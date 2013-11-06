// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Tom Price, modified by Dan Farmer
//
//                         !! NOTE !!
// Designed to be run as a 3-frame animation, which will generate the
// self-including image_maps in the picture frame.  See DESK.INI.
// Specifies .PNG format for the image_maps, so you may need to do
// some editing if your standard output format is different than that.
//
// -w320 -h240 +kfi1 +kff4 +ki1.0 +kf4.0 +fn
// -w800 -h600 +a0.3 +kfi1 +kff4 +ki1.0 +kf4.0 +fn

#version 3.6;

global_settings {
  assumed_gamma 2.2
  }

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "woods.inc"
#include "glass.inc"
#include "metals.inc"

#declare This_Brass = texture { T_Brass_4B }

#declare
RedPencil = union {
   cylinder {0, y*30, 0.5
      finish {
         crand 0.05
         ambient 0.3
         diffuse 0.7
      }
      pigment { Red }
   }

   cylinder {0, y*32, 0.5
      finish {
         crand 0.05
         ambient 0.3
         diffuse 0.7
      }
      pigment { Tan }
   }
}

#declare GreenPencil =
union {
   cylinder {0, y*30, 0.5
      finish {
         crand 0.05
         ambient 0.3
         diffuse 0.7
      }
      pigment { Green }
   }

   cylinder {0, y*32, 0.5
      finish {
         crand 0.05
         ambient 0.3
         diffuse 0.7
      }
      pigment { Tan }
   }
}

#declare BluePencil =
union {
   cylinder {0, y*30, 0.5
      finish {
         crand 0.05
         ambient 0.3
         diffuse 0.7
      }
      pigment { Blue }
   }

   cylinder {0, y*32, 0.5
      finish {
         crand 0.05
         ambient 0.3
         diffuse 0.7
      }
      pigment { Tan }
   }
}

#declare Back_Wall =
plane { z, 200.0
      hollow on
      finish {
      crand 0.05
      ambient 0.3
      diffuse 0.7
   }
   pigment { LightGray }
}

#declare Ceiling =
plane { y,  500.0
   hollow on
   finish {
      ambient 0.3
      diffuse 0.7
   }
   pigment { White }
}


#declare Desk_Top =
box { <-125, -2, -100> <125, 2, 100>
   translate -20.0*y                     // top surface at -18*y
   texture {
      // T_Wood30
      pigment { P_WoodGrain6A color_map { M_Wood6A }}
      #if (clock=4)
          finish { reflection 0.2 }
      #end
      rotate y*90
      translate z*30
      rotate z*5
      scale 5
   }
}

#declare Blotter =
union {
   triangle {
      <0.0, 0.0, 0.0>
      <8.5, 0.0, 0.0>
      <0.0, 0.0, -11.0>
   }
   triangle {
      <0.0, 0.0, -11.0>
      <8.5, 0.0, -11.0>
      <8.5, 0.0, 0.0>
   }

   scale <4.0, 1.0, 4.0>
   rotate -30.0*y
   translate <-20.0, -17.9999, -40.0>

   finish {
      crand 0.04
      ambient 0.15
      diffuse 0.5
   }
   pigment { colour red 0.5 green 0.5 blue 0.3 }
}

#declare Paperweight=
intersection {
   sphere { <0.0, -5.0, 0.0>, 10.0 }
   disc { 0, -y, 10.1 }
   translate <0.0, -17.9998, -35.0>
   //texture { T_Green_Glass }
   // converted to material 26Sep2008 (jh)
   material {
     texture {
       pigment {color rgbf <0.8, 1, 0.95, 0.9>}
       finish {F_Glass3}
       }
     interior {ior 1.5}
     }
}


/*The Picture itself*/
#declare Picture =
union {
      box { -1, 1
      translate <1.0, 1.0, 1.0>
      scale <20.0, 15.0, 1.0>

      finish {
         ambient 0.05
         diffuse 0.9
      }
      pigment {
         #switch(clock)
         #case(4)
             image_map { png "desk3.png" once interpolate 2.0 }
             scale <40.0, 30.0, 1.0>
             scale <1.5, 1.5, 1.0>
             #break
         #case(3)
             image_map { png "desk2.png" once interpolate 2.0 }
             scale <40.0, 30.0, 1.0>
             scale <1.5, 1.5, 1.0>
             #break
         #case(2)
             image_map { png "desk1.png" once interpolate 2.0 }
             scale <40.0, 30.0, 1.0>
             scale <1.5, 1.5, 1.0>
             #break
         #else
             Gray50
         #end
      }
   }

   /* The picture frame */
   union {
      cylinder {-y,31*y,1 translate 41*x }
      cylinder {-y,31*y,1 translate -1*x }
      cylinder {-x,41*x,1 translate 31*y }
      cylinder {-x,41*x,1 translate -1*y }
      sphere { <-1.0, -1.0, 0.0>, 1.0 }
      sphere { <-1.0, 31.0, 0.0>, 1.0 }
      sphere { <41.0, -1.0, 0.0>, 1.0 }
      sphere { <41.0, 31.0, 0.0>, 1.0 }
      texture { This_Brass }
   }

   scale 1.5
   rotate <10.0, -35.0, 0.0>
   translate <-65.0, -15.0, -25.0>
}

#declare Pencil_Holder =
union {
   intersection {
      object { Cylinder_Y scale <5.0, 1.0, 5.0> }
      object { Cylinder_Y scale <4.8, 1.0, 4.8> inverse }
      plane { y, 0.0 inverse }
      plane { y, 15.0 rotate -45*x }
      texture { This_Brass }
   }
   object {
      RedPencil
      rotate -2*z
      translate <1.0, 0.0, 1.0>
   }
   object {
      GreenPencil
      rotate 2.0*z
      translate <-1.0, 3.0, 0.0>
   }
   object {
      BluePencil
      rotate <-2.0, 0.0, 3.0>
      translate <0.0, -2.0, -1.0>
   }
   rotate 45*y
   translate <70.0, -18.0, -20.0>
}

#declare Lamp =
union {
   object {
      cylinder { -y*18, y*40, 3 }
      texture { This_Brass }
   }
   cylinder { -y*2, y*2, 25
//    intersection {
//       object { Cylinder_Y scale <25.0, 1.0, 25.0> }
//       plane { y, 2.0 }
//       plane { y, -2.0 inverse }

      translate <0.0, -16.0, -5.0>
      texture { This_Brass
         normal { bumps 0.1 }
      }
   }

   intersection {
      object { Cylinder_X scale <1.0, 10.0, 10.0> }
      object { Cylinder_X scale <1.0, 9.95, 9.95> inverse }
      plane { y, 0.0 inverse }
      plane { x, -30.0 inverse }
      plane { x, 30.0 }

      translate <0.0, 35.0, -13.0>

      finish {
         Shiny
         crand 0.05
         ambient 0.5
         diffuse 0.5
         reflection 0.3
         brilliance 4.0
      }
      pigment { DarkGreen }
   }

   union {
      intersection {
         sphere { <-30.0, 35.0, -13.0>, 10.0 }
         sphere { <-30.0, 35.0, -13.0>, 9.95 inverse }
         plane { y, 35.0 inverse }
         plane { x, -30.0 }
      }
      intersection {
         plane { y, 35.0 inverse }
         plane { x, 30.0 inverse }
         sphere { <30.0, 35.0, -13.0>, 10.0 }
         sphere { <30.0, 35.0, -13.0>, 9.95 inverse }
      }
      texture { This_Brass }
   }

   rotate 35*y
   translate <50.0, 0.0, 30.0>
}

/*The fluorescent tube inside the lamp*/
#declare Lamp_Light_Source =
light_source { <0, 0, 0> color White

    looks_like {
       cylinder { -x*25, x*25, 2
          pigment { White filter 0 }
          finish { ambient 1 diffuse 0 }
      }
   }

   translate <0.0, 43.0, -10.0>
   rotate 35*y
   translate <50.0, 0.0, 30.0>
}
camera {
   location <0.0, 40.0, -150.0>
   angle 65 
   right x*image_width/image_height // keep propotions with any aspect ratio
   look_at <0.0, 0.0, 0.0>
}

light_source { <20.0, 100.0, -200.0> colour White }

object { Back_Wall }
object { Ceiling }
object { Desk_Top }
object { Blotter }
object { Paperweight }
object { Lamp }
object { Lamp_Light_Source }
object { Picture }
object { Pencil_Holder }
