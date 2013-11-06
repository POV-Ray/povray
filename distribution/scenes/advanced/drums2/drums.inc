// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Include file for DRUMS.POV by Dan Farmer

// Updated: 09Aug2008 (jh) for v3.7 distribution

#declare Brass = colour red 0.88 green 0.84 blue 0.10;
#declare ChromeBlue = colour red 0.84 green 0.89 blue 1.00;

#declare BrassFinish = finish {
   reflection 0.15
   brilliance 10.0
   metallic
   phong 0.75
   phong_size 50
}

#declare Chrome = finish { Mirror }

#declare VeryShiny = finish {
   ambient 0.05  diffuse 0.75
   phong 1.0
   phong_size 100.0
   brilliance 5
   reflection 0.5
}

// The old Disk_X definition
#declare Disk_X = cylinder { <0, 0, 0>, <1, 0, 0>, 1 }

#declare X_Tube = difference {
   cylinder { -x, x, 1.0 }
   cylinder { -1.1*x, 1.1*x, 0.97 }
}

#declare Cymbal = union {            /* Overall radius of 6 units */
   sphere {                             /* Dome */
      <0, 0, 0>, 1
      scale <2.0, 0.75, 2.0>
      translate 1*y
   }
   sphere {
      <0, 0, 0>, 1
      scale <6.0, 1.0, 6.0>
   }

   finish { BrassFinish }
   pigment { Brass }

   clipped_by { plane { y, 0.25 inverse } }
}

#declare Tensioner1 =  union {
   object {
      Disk_X
      translate -0.5*x
      scale <0.5, 0.04, 0.02>
   }

   intersection {
      sphere { <0, 0, 0>, 1 scale <0.25, 0.1, 0.06> }
      plane { x, 0 }
   }

}

#declare Tensioner = union {
   object { Tensioner1 translate 0.5*x }
   object { Tensioner1 rotate 180*z translate -0.5*x }

   translate 1.03*z        /* Position for rotation around drum cylinder */

   finish { Chrome }
   pigment { ChromeBlue }
}


#declare Drum = union {
   sphere {                                       /* Top (and rear) Drumhead */
      <0, 0, 0>, 1
      scale <0.001, 1.0, 1.0>
      translate 0.75*x

      finish {
         ambient 0.1
         diffuse 0.7
         specular 0.2
         roughness 0.01
      }
      pigment { White }
   }

   object {                                       /* Chrome edge band #1 */
      X_Tube
      scale <0.05, 1.025, 1.0>
      translate -0.8*x

      texture {
         finish { Chrome }
         pigment { ChromeBlue }
      }
   }

   intersection {                         /* Drum body */
      cylinder {
         -1.1*x, 1.1*x, 1.0

         finish { VeryShiny }
         pigment { White }
      }

      cylinder {
         -1.1*x, 1.1*x, 0.97         /* Inside of the drum cylinder */
         inverse

         pigment { Tan }
         finish {
            ambient 0.05
            diffuse 0.5
         }
      }

      box { <-1, -1, -1>, <1, 1, 1>
          finish { Chrome }
          pigment { ChromeBlue }
      }


      scale <0.8, 1.0, 1.0>
   }

   object {                                       /* Chrome edge band #2 */
      X_Tube
      scale <0.05, 1.025, 1.0>
      translate 0.8*x
      texture {
         finish { Chrome }
         pigment { ChromeBlue }
      }
   }

   /* Now a bunch of (10) tensioners */

   object { Tensioner rotate 000*x }
   object { Tensioner rotate 036*x }
   object { Tensioner rotate 072*x }
   object { Tensioner rotate 108*x }
   object { Tensioner rotate 144*x }
   object { Tensioner rotate 180*x }
   object { Tensioner rotate 216*x }
   object { Tensioner rotate 252*x }
   object { Tensioner rotate 288*x }
   object { Tensioner rotate 324*x }

}

#declare BassDrum = union {
   object { Drum }

   sphere {             /* Front Drumhead for Bass */
      <0, 0, 0>, 1
      scale <0.001, 1.0, 1.0>
      translate -0.75*x

      finish {
         ambient 0.2
         diffuse 0.7
         specular 0.5
         roughness 0.05
      }
      pigment { White filter 0.25 }
   }

   scale <8.0, 10.0, 10.0>
}

/********************** not used at this time
#declare SnareDrum = object {
    Drum
    rotate 90.0*z
    scale <5.0, 4.0, 5.0>
}
***********************/

#declare TomTom = object {
   Drum
   rotate 90.0*z
   scale <5.0, 5.0, 5.0>
}

/* Combine a base and a pair of tomtoms into a half-set */
/* Object is still centered at 0,0 at this point */
#declare HalfSet = union {
   object {              /* Right bass  */
      BassDrum
      rotate -90.0*y     /* Turn it on its end */
   }

   object {
      TomTom
      rotate +30.0*x                   /* Lean it towards drummer */
      translate 16.0*y                 /* Raise it up */
      translate -7.0*x                 /* Move it left */
   }

   object {
      TomTom
      rotate +30.0*x                   /* Lean it towards drummer */
      translate 16.0*y                 /* Raise it up */
      translate +7.0*x                 /* Move it right */
   }

   union {         /* tomtom bracket */
      object {
         Disk_X                         /* Horizontal bar */
         translate -0.5*x
         scale <4, 0.25, 0.25>          /* Fits between tomtoms */
         translate 16.0*y               /* Raise to top of vertical bar*/
      }

      sphere { <0, 0, 0>, 1 translate <-4, 16, 0> scale <0.5, 1, 1> }
      sphere { <0, 0, 0>, 1 translate < 4, 16, 0> scale <0.5, 1, 1> }

      object {                   /* Vertical bar */
         Disk_Y
         scale <0.30, 6.0, 0.30>
         translate <0.0, 10.0, 0.0>
      }

      sphere { <0, 0, 0>, 1 scale <1.25, 0.5, 1.25> translate 10*y }
   }

   finish { Mirror }
   pigment { White }
}
