#ifdef(Rad_Def_Inc_Temp)
// do nothing
#else
#declare Rad_Def_Inc_Temp = version;

#ifdef(View_POV_Include_Stack)
#   debug "including rad_def.inc\n"
#end

// Set some common radiosity settings.  These settings are extremely general
// and are intended for ease of use and don't necessarily give the
// best results.


// These constants are defined in "consts.inc"
// #declare Radiosity_Debug = 1;
// #declare Radiosity_Fast = 2;
// #declare Radiosity_Normal = 3;
// #declare Radiosity_2Bounce = 4;
// #declare Radiosity_Final = 5;

// The correct format for using this file is:
//      #include "consts.inc"
//      #declare Rad_Quality = Radiosity_Debug;
//      #include "rad_def.inc"

// Uses ray_count and error_bound as the main speed/quality tradeoffs
// Other changes made to emphasize or minimize effects.
global_settings {

#ifdef(Rad_Quality)
// do nothing
#else
#   debug "\nFile rad_def.inc reports:"
#   debug "\nRad_Quality not set.  Using Radiosity_Normal by default."
#   debug "\nOptions are:"
#   debug "\n  #declare Rad_Quality = Radiosity_Debug"
#   debug "\n  #declare Rad_Quality = Radiosity_Fast"
#   debug "\n  #declare Rad_Quality = Radiosity_Normal"
#   debug "\n  #declare Rad_Quality = Radiosity_2Bounce"
#   debug "\n  #declare Rad_Quality = Radiosity_Final"
#   debug "\n  This must be set before you '#include' rad_def.inc.\n"
#   declare Rad_Quality = Radiosity_Normal;
#end


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
        gray_threshold 0.5       // Try 0.33-0.50. Just a matter of taste
        distance_maximum 10      // Scene-dependent!  Leave 0 if unsure of proper value.
        low_error_factor 0.75
        nearest_count 7
        minimum_reuse 0.017      // reasonable number of samples in corners
        brightness 3.3           // doesn't really matter.  Not used in final output.
        recursion_limit 2        // Slow at first, but don't give up, it gets faster
    }
    #debug "\nRadiosity_2Bounce in use"
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

#version Rad_Def_Inc_Temp;
#end
