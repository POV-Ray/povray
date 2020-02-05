// Persistence Of Vision raytracer version 3.1 sample file.
// Used by rad2.pov

global_settings { assumed_gamma 2.2 }

// For radiosity only!
#default { finish { ambient 0.35 diffuse 0.65 } }

//  $MRY$: 'Room_Texture'
#declare Room_Texture = texture {
   pigment {
     color rgb<0.82, 0.82, 0.82>
   }
}


#declare Cone_Texture = texture {
    T_Glass2 
    finish { Shiny
        reflection .15
        specular .9
        roughness .001
        ambient .1
        diffuse .1
    }
    /*
    pigment {
        color rgb<0.8, 0.8, 0.0>
    }
    */
}

#declare Box_Texture = texture {
   pigment {
     color rgb<0.8, 0.8, 0.05>
   }
}

#declare Ball_Texture = texture {
   pigment {
     color rgb<0.95, 0.95, 1.0>
   }
   finish {
     Shiny
     diffuse 0.65
     ambient 0.375
   }
}

#declare Torus_Texture = texture {
    T_Brass_3B
    normal {
      crackle 0.25
      turbulence 0.3
      octaves 4
      scale 0.35
    }
}

#declare Cyl_Texture = texture {
   pigment {
     color rgb<0.504, 0.050, 0.050>
   }

}

#declare LeftWallTex = texture {
   pigment {
     color rgb <.85, .05, .05>
   }
   finish {   
   }

}

#declare RightWallTex = texture {
   pigment {
     color rgb<0.041, 0.050, 0.479>
   }
}

