// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Radiosity demonstration

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

#include "consts.inc"
#declare Rad_Quality = Radiosity_Final;
#include "rad_def.inc"

// Uses ray_count and error_bound as the main speed/quality tradeoffs
// Other changes made to emphasize or minimize effects.
global_settings {
#switch(Rad_Quality)

    // Run it fast, don't try to make it look good, make sure that
    // you can actually see where the radiosity boundaries are.
    #case (Radiosity_Debug)
    radiosity {
        count 10                 // Quick, but very blotchy
        error_bound 0.3          // Match to value of the quality you're debugging
        gray_threshold 0         // Emphasize color bleeding drastically
        distance_maximum 10      // Scene-dependent!  Leave 0 if unsure of proper value.
        low_error_factor 0.8     // Match to value of the quality you're debugging
        nearest_count 1          // Will tend to cause boundaries to show
        minimum_reuse 0.015      // Match to value of the quality you're debugging
        brightness 3.3           // Doesn't really matter.  Not used in final output.
        recursion_limit 1        // 1 is quickest 
    }
    #debug "\nRadiosity_Debug in use"
    #break

    // Make it look as good as you can, but I'm in a hurry
    #case (Radiosity_Fast)
    radiosity {
        count 80                 // Do more calculations to calculate each sample
        error_bound 0.4          // Main quality/time adjustment = sample spacing
        gray_threshold 0.6       // Higher than usual to hide colour bleed errors
        distance_maximum 10      // Scene-dependent!  Leave 0 if unsure of proper value.
        low_error_factor 0.9     // Only slightly lower error bound during preview
        nearest_count 5
        minimum_reuse 0.025      // Don't do too many samples in corners
        brightness 3.3           // doesn't really matter.  Not used in final output.
        recursion_limit 1        // can be 1 (usual) or 2 (for patient people)
    }
    #debug "\nRadiosity_Fast in use"
    #break

    // Typical values
    #case (Radiosity_Normal)
    radiosity {
        count 200                // Calculate reasonable accurate samples
        error_bound 0.3          // Main quality/time adjustment = sample spacing
        gray_threshold 0.5       // Try 0.33-0.50. Just a matter of taste
        distance_maximum 10      // Scene-dependent!  Leave 0 if unsure of proper value.
        low_error_factor 0.75
        nearest_count 7
        minimum_reuse 0.017      // reasonable number of samples in corners
        brightness 3.3           // doesn't really matter.  Not used in final output.
        recursion_limit 1        // can be 1 (usual) or 2 (for patient people)
    }
    #debug "\nRadiosity_Normal in use"
    #break

    // Typical values, but with 2 bounces.  Starts slow, but picks up steam!
    #case (Radiosity_2Bounce)
    radiosity {
        count 200                // Calculate reasonable accurate samples
        error_bound 0.3          // Main quality/time adjustment = sample spacing
        gray_threshold 0.5	     // Try 0.33-0.50. Just a matter of taste
        distance_maximum 10      // Scene-dependent!  Leave 0 if unsure of proper value.
        low_error_factor 0.75
        nearest_count 7
        minimum_reuse 0.017      // reasonable number of samples in corners
        brightness 3.3           // doesn't really matter.  Not used in final output.
        recursion_limit 2        // Slow at first, but don't give up, it gets faster
    }
    #debug "\nRadiosity_Normal in use"
    #break

    // For patient quality freaks with fast computers about to leave on vacation
    #case (Radiosity_Final)
    radiosity {
        count 800                // Ensure that we get good, accurate samples
        error_bound 0.2          // And calculate lots of them.  (more important than count)
        gray_threshold 0.5
        distance_maximum 4       // Scene-dependant! 
        low_error_factor 0.7     // force many extra samples to be calculated...
        nearest_count 9          // so we can average them together for smoothness
        minimum_reuse 0.01       // get quite tightly into corners
        brightness 3.3           // doesn't really matter.  Not used in final output.
        recursion_limit 1        // Try this = 2, but drop the count to maybe 300
   }
   #debug "\nRadiosity_Final in use"
   #break
#end
}

/******************************************
#declare Radiosity_Defaults = 0;
#declare Radiosity_Test = 1;
#declare Radiosity_Fast = 2;
#declare Radiosity_Final = 3;

#declare Rad_Quality = Radiosity_Test

global_settings {

#switch(Rad_Quality)
    #case (Radiosity_Defaults)
    #break
    #case (Radiosity_Test)
        radiosity {
            brightness 3.3
            count 50
            distance_maximum 6
            gray_threshold 0.0
            low_error_factor 0.775
            nearest_count 1
            recursion_limit 1

            error_bound 0.25          // Experiment with this
            minimum_reuse 0.15        // Experiment with this
        }
    #break
    #case (Radiosity_Fast)
        radiosity {
            brightness 3.3
            count 500
            distance_maximum 6
            gray_threshold 0.0
            low_error_factor 0.775
            nearest_count 8
            recursion_limit 1

            error_bound 0.75
            minimum_reuse 0.15
        }
    #break
    #case (Radiosity_Final)
        radiosity {
            brightness 3.3
            count 1500
            distance_maximum 6
            gray_threshold 0.15
            low_error_factor 0.015
            nearest_count 12
            recursion_limit 1

            error_bound 0.015
            minimum_reuse 0.015
        }
    #break
#end

}
******************************/


camera {
  location <-1.5, 2, -29.9>
  direction z * 1.75
  up y
  right x * 1.3333
  look_at <0.5, -1.0, 0.0>
}

#declare Dist=15;
//#declare L = 0.65;
#declare L = 0.35;

light_source { <0, 9.5, 0>
  color rgb L
  fade_distance Dist fade_power 2
  shadowless
}

light_source { <-5, 7.5, 10.>
  color rgb L
  fade_distance Dist fade_power 2
  shadowless
}


//#declare Ambient = 0.35;
#declare Ambient = 0.25;
box { -1, 1
    scale <10, 10, 30>
    pigment { White }
    finish { ambient Ambient }
    inverse
}

box { -1, 1 scale <9, 8, 0.2>
    pigment {
        gradient z
        color_map {
            [0.0 color Red ]
            [0.5 color Red ]
            [0.5 color Blue ]
            [1.0 color Blue ]
        }
        translate -z*0.5
    }
    finish { ambient Ambient }
    rotate y*90
    rotate y*(clock*360)
    translate z*10
}

