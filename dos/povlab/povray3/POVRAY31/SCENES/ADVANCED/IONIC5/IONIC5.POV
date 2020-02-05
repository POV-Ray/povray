// Persistence Of Vision raytracer version 3.1 sample file.
//   This data includes 1 Wall, 1 large 2nd floor column,
//   & 2 smaller ionic colums.

//   b-snake.dat is the lower ctds twist that goes around the
//   door openings.
//   s-head3.dat is the head & top twist of creature
//   turn.dat is ctds data that creates the ionic turned capitals.
//   panther.dat is csg of panther figure.

//  Modified for use in stereo pair. Two of the PNG
//  images have been replaced with single colors, and a third column
//  was added to the bottom row.
//
//  Three cameras are included, the original, and two for stereo.

// gamma devised to approximate the illustration in Ray Tracing Creations II


global_settings { assumed_gamma 1.8 }

#include "colors.inc"
#include "shapes.inc"
#include "marble.inc"
#include "b-snake.inc"
#include "turn.inc"
#include "panther.inc"
#include "s-head3.inc"

#declare pink = color red 1.0 green 0.5 blue 0.5;


/* original camera */
camera {
   location <-50, 80, -220>
   direction <0, 0, 1.5>
   up <0, 1, 0>
   right <4/3, 0, 0>
}

// Optional stereo views:
/* camera for left eye, render at 768 x 480 or similar aspect ratio */
/*
camera {
    location <-61, 80, -220>
    direction <0, 0, 1.4>
    up <0, 1, 0>
    right <1.6, 0, 0>
}
*/

/* camera for right eye, render at 768 x 480 or similar aspect ratio */
/*
camera {
    location <-39, 80, -220>
    direction <0, 0, 1.4>
    up <0, 1, 0>
    right <1.6, 0, 0>
}
*/

/*-------------- WORLD WALLS ---------------------------------*/
sphere {
   <0, 0, 0>, 50000
   hollow on
   texture {
      pigment { MidnightBlue }
      finish {
         ambient 1.0
         diffuse 0.0
      }
   }
}

/*---------------LIGHT #1------------------------------------*/
light_source { <700, 150, -500> color White }

/*--------------LIGHT #2-------------------------------------*/
light_source { <-4000, 100, -1000> color White }


/**********************************************************************/

/*-----------BASE OF COLUMN A / TOP FLOOR-------------------*/
#declare base_a = union {
   sphere { <0, 0, 0>, 1 scale <13, 5.5, 13> }
   sphere { <0, 0, 0>, 1 scale <11, 3, 11> translate 4*y  }
   cylinder { <0,0,0>, y, 1 scale <10, 4, 10> translate 5*y }

   texture {
      marble1
      scale <9, 6, 4>
      finish {
         ambient 0.5
         diffuse 1
      }
   }
   texture {
      marble4
      scale <7, 12, 9>
      rotate <0, 0, -40>
      finish {
         diffuse 1.0
         phong 0.6
         phong_size 50
      }
   }

   rotate 40*y
}

/*-----------MOTIF TILE WORK--------------------*/
#declare motif1 = cylinder {
   <0,0,0>, y, 1
   scale <9.5, 7, 9.5>

   texture {
      pigment {
         image_map { png "congo4.png" }
         scale <19, 7, 1>
         translate <-9.5, 0, -1>
       }
       finish {
          ambient 0.3
          diffuse 0.9
          phong 0.6
       }
    }
 }


/*----------- COLUMN A -------------------------------------------------*/
#declare column_a = cylinder {
   <0,0,0>, y, 1
   scale <9.3, 50, 9.3>

   texture {
      marble1
      finish {
         crand 0.01
         ambient 0.2
         diffuse 0.8
      }
      scale <16, 4, 4>
   }
   texture {
      marble2
      scale <10, 6, 5>
      rotate -30*z
      finish { diffuse 1.0 }
   }
   texture {
      marble4
      finish {
         phong 0.6
         phong_size 45
      }
      scale <8, 15, 5>
      rotate <0, 0, 50>
   }

   rotate 10*y
}



/*-----------COMBINE COLUMN & BASE --------------------------------*/
#declare top_column = union {
   object { base_a }
   object { motif1 translate 9*y }
   object { column_a translate 12*y }
}


/*--------------------TOP  FLOOR---------------------------------------*/
#declare gfloor = object {
   Cube
   scale <100, 1, 50>

   texture {
      pigment { Salmon }
      finish {
         ambient 0.2
         diffuse 0.5
         phong 1.0
         phong_size 10
      }
   }
}

/*----------- ROUND CORNER ----------------------------*/
#declare sp1 = sphere { <0, 0, 0>, 2 }

#declare round_cap = object {
   union {
      object { sp1 translate <-100, 0, -50> }
      object { sp1 translate <-100, 0,  50> }
      object { sp1 translate <100, 0, -50> }
      object { sp1 translate <100, 0,  50>  }
      cylinder { <0,0,0>, x, 1 scale <200, 2, 2> translate <-100, 0, -50> }
      cylinder { <0,0,0>, z, 1 scale <2, 2, 100> translate <-100, 0, -50> }
   }

   texture {
      marble1
      scale <50, 10, 40>
      finish {
         ambient 0.4
         diffuse 0.9
      }
   }
   texture {
      marble2
      scale <40, 5, 20>
      rotate <0, 0, 40>
   }
   texture {
      marble4
      scale <20, 20, 20>
      rotate <0, 0, -40>
      finish { phong 0.6 phong_size 20 }
   }
}

/*--------------- ONE DENTILE ----------------------------------*/
#declare d1 = object { Cube scale <1, 1.5, 2> }

#declare cornice = union {
   object { Cube scale <103, 0.5, 53> translate <0, 2.5, 0> }
   object { Cube scale <103, 1, 53>   translate <0, -2, 0>  }
   object { Cube scale <101, 2, 51>   translate <0, 0, 0>   }

   texture {
      marble1
      scale <10, 6, 4>
      finish {
         ambient 0.4
         diffuse 1.0
      }
   }
   texture {
      marble2
      scale <7, 6, 4>
      rotate <0, 0, 40>
   }
   texture {
      marble4
      scale <3, 6, 5>
      rotate <0, 0, -40>
      finish {
         phong 0.6
         phong_size 20
      }
   }
}

/*-------------------- ROW OF DENTILES ---------------*/
#declare dentile_a = union {
   object { d1 translate -50*x }
   object { d1 translate -45*x }
   object { d1 translate -40*x }
   object { d1 translate -35*x }
   object { d1 translate -30*x }
   object { d1 translate -25*x }
   object { d1 translate -20*x }
   object { d1 translate -15*x }
   object { d1 translate -10*x }
   object { d1 translate  -5*x }
   object { d1 translate   0*x }
   object { d1 translate  50*x }
   object { d1 translate  45*x }
   object { d1 translate  40*x }
   object { d1 translate  35*x }
   object { d1 translate  30*x }
   object { d1 translate  25*x }
   object { d1 translate  20*x }
   object { d1 translate  15*x }
   object { d1 translate  10*x }
   object { d1 translate   5*x }

   texture {
      marble1
      scale <10, 6, 4>
      finish {
         ambient 0.4
         diffuse 0.9
      }
   }
   texture {
      marble2
      scale <7, 6, 4>
      rotate <0, 0, 40>
   }
   texture {
      marble4
      scale <3, 6, 4>
      rotate <0, 0, -40>
      finish {
         phong 0.6
         phong_size 20
      }
   }
}

/*--------------  COMBINE CORNICE, FLOOR, CAP & DENTILES -----------*/
#declare top_floor = union {
   object { gfloor    translate <0, 9, 0>     }
   object { round_cap translate <0, 7, 0>     }
   object { cornice   translate <0, 3, 0>     }
   object { dentile_a translate <-50, 4, -52> }
   object { dentile_a translate < 50, 4, -52>  }
}

/*------------------MAKE AN IONIC CAPIAL---------------------------*/

#declare turn = object { s1 rotate -90*x }

/*--------------- 1/2 SECTION OF CAP ------------------------------*/
#declare cap_a = union {
   sphere {
      <0, 0, 0>, 1
      scale <7, 2, 7>
      translate <0, 1, 0>
   }
   cylinder {
      <0,0,0>, y, 1
      scale <6, 5, 6>
      translate <0, 2, 0>
   }
   sphere {
      <0, 0, 0>, 1
      scale <8, 3, 8>
      translate <0, 8, 0>
   }
   cylinder {
      <0,0,0>, y, 1
      scale <8, 2, 8>
      translate <0, 8, 0>
   }

   texture {
      marble1
      finish {
         ambient 0.4
         diffuse 1.0
      }
      scale <5, 13, 4>
   }
   texture {
      marble4
      finish {
         diffuse 1.0
         phong 0.6
         phong_size 20
      }
      scale <5, 7, 3>
      rotate <0, 0, 50>
   }
}


/*-------------- THE OTHER 1/2-----------------------------*/
#declare cap_b = union {
   cylinder { <0,0,0>, y, 1 scale <1.2, 8, 1.2> translate <1, 0, -4>  }
   cylinder { <0,0,0>, y, 1 scale <1.2, 8, 1.2> translate <-1, 0, -4> }

   cylinder { <0,0,0>, y, 1 scale <1.2, 4, 1.2> translate <-2, 0, -4> }
   cylinder { <0,0,0>, y, 1 scale <1.2, 4, 1.2> translate <2, 0, -4>  }

   object { Cube scale <10, 2, 8> translate <0, 10, 2> }
   object { Cube scale <7, 4, 4> translate <0, 4, 0>  }
   cylinder { <0,0,0>, z, 1 scale <4.5, 4.5, 8> translate <-6, 4, -4> }
   cylinder { <0,0,0>, z, 1 scale <4.5, 4.5, 8> translate <6, 4, -4>  }

   texture {
      marble1
      finish {
         ambient 0.4
         diffuse 1.0
      }
      scale <5, 13, 4>
   }
   texture {
      marble4
      finish {
         diffuse 1.0
         phong 0.6
         phong_size 20
      }
      scale <5, 7, 3>
      rotate 50*z
   }
}


/*-----------2 HALFS & 2 TURNS MAKE A WHOLE ------------------------*/
#declare capital = union {
   object { cap_a translate <0, 0, 0> }
   object { cap_b translate <0, 10, 0> }
   object { turn translate <-6, 14, -4> }
   object { turn rotate <0, 180, 0> translate <6, 14, -4> }
}

/*--------------MAKE COLUMN-------------------*/


/*-----------MOTIF TILE WORK--------------------*/
#declare motif2 = cylinder {
   <0,0,0>, y, 1
   scale <6.5, 8, 6.5>

   texture {
      pigment {
         image_map { png "congo4.png" }
         scale <13, 8, 1>
         translate <-6.5, 0, -1>
      }
      finish {
         ambient 0.3
         diffuse 0.9
         phong 1.0
      }
   }
}

#declare column_b = cylinder {
   <0,0,0>, y, 1
   scale <6, 54, 6>

   texture {
      marble1
      finish {
         ambient 0.4
         diffuse 0.8
         crand 0.1
      }
      scale <9, 30, 18>
   }
   texture {
      marble2
      scale <10, 18, 5>
      rotate <0, 0, -30>
      finish { diffuse 1.0 }
   }
   texture {
      marble4
      finish {
         diffuse 0.9
         phong 0.8
         phong_size 45
      }
      scale <4, 15, 5>
      rotate 50*z
   }
}


/*-----------THE COMPLETE BOTTOM COLUMN---------------------------------*/
#declare bottom_column = union {
   object { capital  translate <0, 58, 0> }
   object { motif2   translate <0, 50, 0>  }
   object { column_b translate <0, 0, 0> }
}

/*---------------------START LOWER WALLS-----------------------------*/

/*----------- ABOVE DOOR SECTION OF WALL -----------------------------*/
#declare brick_wall = object {
   Cube
   scale <30, 6, 3>

   texture {
      marble1
      finish {
         ambient 0.2
         diffuse 0.9
      }
      scale <20, 30, 4>
   }
   texture {
      marble2
      scale <20, 9, 5>
      finish { diffuse 1.0 }
      rotate -30*z
   }
   texture {
      marble4
      finish {
         phong 0.6
         phong_size 25
      }
      scale <18, 18, 3>
      rotate 50*z
   }
}

/*-------------- ARCH SECTION OF WALL --------------------------------*/
#declare top_wall = object {
   difference {
      object { Cube scale <30, 10, 3> translate <0, 0, 0.01> }
      object { Cylinder_Z scale <13, 13, 1> translate <0, -10, 0> }
   }

   texture {
      marble1
      finish {
         ambient 0.2
         diffuse 0.9
      }
      scale <30, 20, 4>
   }
   texture {
      marble2
      scale <26, 16, 5>
      finish { diffuse 1.0 }
      rotate -30*z
   }
   texture {
      marble4
      finish {
         phong 0.6
         phong_size 25
      }
      scale <15, 27, 2>
      rotate 50*z
   }
}


/*------------------ TRIM ---------------------------------------------*/
#declare molding_2 = union {
   difference {
      cylinder { <0,0,0>, z, 1 scale <16, 16, 12> translate <0, 0, -6>  }
      object { Cylinder_Z scale <13, 13, 1> }
   }
   object { Cube scale <10, 2, 6> translate <-21, -2, 0>  }
   object { Cube scale <10, 2, 6> translate <21, -2, 0>  }
   object { Cube scale <31, 1, 6> translate <0, 17, 0>  }

   texture {
      marble1
      finish {
         ambient 0.5
         diffuse 1.0
      }
      scale <15, 3, 4>
   }
   texture {
      marble4
      finish {
         phong 0.8
         phong_size 15
         ambient 0.6
      }
      scale <15, 7, 6>
      rotate 50*z
   }
}

#declare motif3 = object {
   Cube
   scale <9.5, 5, 3.1>

   texture {
      pigment {
         image_map { png "congo4.png"  }
         scale <19, 10, 1>
         translate <-9.5, -5, -1>
      }
      finish {
         ambient 0.3
         diffuse 0.9
         phong 1.0
      }
   }
}

/*----------WALLS FLANKING DOOR WAY -----------------------------------*/
#declare low_wall = object {
   Cube
   scale <9, 22, 3>

   texture {
      marble1
      finish {
         ambient 0.3
         diffuse 0.9
      }
      scale <15, 20, 7>
   }
   texture {
      marble2
      scale <10, 25, 3>
      finish { diffuse 1.0 }
      rotate <0, 0, -30>
   }
   texture {
      marble4
      finish {
         phong 0.8
         phong_size 25
         ambient 0.5
      }
      scale <8, 16, 3>
      rotate 50*z
   }
}

/*----------COMPOSITE THE WALL------------------------------*/

#declare wall = union {
   object { low_wall translate <-21, 22, 0>  }
   object { low_wall translate <21, 22, 0>  }
   object { motif3 translate <-21, 42, 0>  }
   object { motif3 translate <21, 42, 0>  }
   object { molding_2 translate <0, 50, 0>  }
   object { top_wall translate <0, 60, 0>  }
   object { brick_wall translate <0, 74, 0>  }
}


/*--------CREATE INSIDE BACK WALL-----------------------*/
object {
   Cube
   scale <95, 55, 1>

   texture {
      marble1
      finish {
         ambient 0.2
         diffuse 0.9
      }
      scale <40, 30, 5>
   }
   texture {
      marble2
      scale <30, 60, 3>
      rotate -30*z
   }
   texture {
      marble4
      finish {
         phong 0.8
         phong_size 25
      }
      scale <30, 20, 3>
      rotate 50*z
   }
   translate <0, 15, 10>
}


/*----------*** PUT UP THE WALLS ****--------------------*/

object { top_column rotate 10*y  scale 1.9 translate <-30, 94, -25> }
object { top_floor  translate <-10, 80, 0> }
object { bottom_column scale <1.3, 1, 1.3> translate <-100, 0, -40>  }
object { bottom_column scale <1.3, 1, 1.3> translate <-30, 0, -40>  }
object { bottom_column scale <1.3, 1, 1.3> translate <40, 0, -40>  }
object { wall translate <5, 0, -32>    }
object { wall translate <-65, 0, -32>  }


   /*---------------------BACKDROP---------------------------------*/
object {
   Cube
   scale <2000, 1500, 1>

   texture {
      pigment { Maroon }
      finish {
         ambient 0.2
         diffuse 0.6
      }
   }

   texture {
      pigment {
         gradient y
         color_map {
            [0.0 0.3 color Salmon color BlueViolet filter 0.3]
            [0.3 1.0 color BlueViolet filter 0.3 color Black filter 0.2]
         }
         scale <4000, 3000, 1>
         translate <0, -1500, 0>
      }
      finish {
         ambient 1.0
         diffuse 0.0
      }
   }
   scale <1, 1, 1>
   translate <0, 0, 2000>
}


/*------------------- CREATURE & MOTHER-IN-LAW -------------------------*/

object {
   panther
   rotate -20*y
   scale 1.2
   translate <-70, 89.5, -10>
}

object {
   lizard
   scale <1, 0.9, 1>
   rotate -10*z
   translate <-33, 16, -20>
}

#declare snake = union {
    object { top }
    object {
       head
       scale <1.9, 1.6, 1.6>
       rotate <0, -90, -90>
       translate <23, 0, -13>
    }
    rotate <-90, -180, -90>
}

object { snake  translate <-4, 87, -68> }

// end-of-file
