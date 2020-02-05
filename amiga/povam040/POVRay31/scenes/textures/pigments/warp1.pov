// Persistence Of Vision raytracer version 3.1 sample file.
// Texture warp example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

camera { 
  location <0,3,-31>
  direction 3*z
} 

plane {  y,-1.01  pigment {checker Yellow,White}}

plane {  z, 4.01  hollow on pigment {checker Yellow,White}}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}

#declare Thing = plane{z,0.1 clipped_by{box{-2,2}}}
#declare Test = pigment{image_map{png "test.png"} translate -1/2 scale 4}

object{Thing
  pigment { Test

//This is the traditional non-warp turbulence in the X direction only
    turbulence 0.4*x octaves 2  

// followed by a rotate left 90 degrees.  The result is a rotated image
// with the turbulence rotated with it as see in the upper left
    rotate 90*z
  }
  translate <-3,5.5,0>
}

object{Thing
  pigment { Test
// Here we rotate first, then do X turbulence.  However the upper
//  right image shows that traditional POV-Ray syntax always transforms
//  the turbulence with the pattern, regardless of the order specified.

    rotate 90*z
    turbulence 0.4*x octaves 2
  }
  translate <3,5.5,0>
}

object{Thing
  pigment { Test
// Here we do the rotation first
    rotate 90*z

// and then do the turbulence in a new warp statement.  The results in the 
// lower left are what we wanted all along.
    warp{turbulence 0.4*x octaves 2}
  }
  translate <-3,1,0>
}

object{Thing
  pigment { Test
// This lower right image shows that putting the warp statement first,
//  reverses the order and behaves the same as the upper images.
//  The turbulence is rotated with the pattern.  The results are slightly
//  different than the upper images.  That is because here, the turb happens
//  after the transformations that are inside the declared Test pigment.
    warp{turbulence 0.4*x octaves 2}
    rotate 90*z
  }
  translate <3,1,0>
}
