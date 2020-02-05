// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Broken dowel, uses clipped heightfields and heightfield as a clipping
// object to create the fractured end of the dowel.  Uses a Fractint
// "plasma cloud" image for the heightfield.  (just about any size will do).


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "stones.inc"

camera {
   location <0, 6, -6>
   direction <0, 0, 2>
   right <1.333, 0, 0>
   look_at <0, 0, 0>
}

#declare Column_Texture = texture {
   pigment {
      DMFWood1                  // (or whatever its called now)
      scale <0.75, 0.75, 1>     // smaller rings
      rotate 89.85*x            // turn it so the rings are (almost) up
   }

   finish {
      ambient 0.1
      diffuse 0.55
   }
}

// Note: using the HF_Image declaration gives an Exception 17. Why?
#declare HF_Image = height_field { png "plasma2.png" }

#declare HF_Translate = <-0.5, 0, -0.5>;
#declare HF_Roughness = 2;
#declare HF_Scale = <6, HF_Roughness, 6>;

union {
    // This first object is a heightfield clipped to a round disk shape
    // and is used for the "end cap" for the cylinder object that follows.
    height_field {
       png "plasma2.png"
       translate HF_Translate
       scale HF_Scale

       clipped_by { object { Cylinder_Y } }
       texture { Column_Texture }
    }

    // This is essentially the inverse of the above shape; a cylinder that
    // has been clipped by the same heightfield as used to create the cap
    // above.  This yeilds a cylinder with a jaggy edge that mates with
    // the clipped heightfield.  Note that this cylinder, while it starts
    // life with an infinate length, will now be clipped on both the top
    // and the bottom to the same length as the heightfield height.
    object {
        Cylinder_Y
        clipped_by {
            height_field {
                png "plasma2.png"
                translate HF_Translate
                scale HF_Scale
            }
        }
        texture { Column_Texture }
    }
    // Now we've gotta "glue" a disk to the underside of the cylinder
    // so that the object can be made longer.  Overall object height
    // will be HF_Roughness + the Y scale used below.
    object {
        object { Disk_Y translate -1*y }
        texture { Column_Texture }
        scale <1, 3, 1>
    }
}
  
sphere { <0, 0, 0>, 100000
   hollow on
   pigment { Gray30 }
   finish { ambient 0.75}
}

light_source { <10, 50, 1> color Gray30 }
light_source { <60, 50, -100> color red 1 green 1 blue 1 }
