// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: rad_def.inc
//    Last updated: 2010.5.24
//    Description: Set some common radiosity settings.

// These settings are extremely general and are intended for
// ease of use, and don't necessarily give the best results.

// Modified 2001.7.26: Made Normal and Media parameters work
// no matter what setting is used. (Chris Huff)

#ifndef(Rad_Def2_Inc_Temp)
#declare Rad_Def2_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including rad_def.inc\n"
#end

#declare Radiosity_Default = 0;
#declare Radiosity_Debug = 1;
#declare Radiosity_Fast = 2;
#declare Radiosity_Normal = 3;
#declare Radiosity_2Bounce = 4;
#declare Radiosity_Final = 5;

#declare Radiosity_OutdoorLQ = 6;
#declare Radiosity_OutdoorHQ = 7;
#declare Radiosity_OutdoorLight = 8;
#declare Radiosity_IndoorLQ = 9;
#declare Radiosity_IndoorHQ = 10;


// The correct format for using this file is:
//
// #include "rad_def.inc"
// global_settings {
//   radiosity {
//     Rad_Settings(Radiosity_Default, off, off)
//   }
// }
//
//
// for first parameter use one of the above constants
//
// second parameter for radiosity normal switch
// (turn 'on' if normal should be taken into account when calculating radiosity)
//
// third parameter for radiosity media switch
// (turn 'on' if media should be taken into account when calculating radiosity)
//


#macro Rad_Settings(Nbr, Normal, Media)

  #switch(Nbr)

    // An empty radiosity block using default settings
    #case (Radiosity_Default)
    #debug "\nRadiosity_Default in use\n"
    #break

    // Run it fast, don't try to make it look good, make sure that
    // you can actually see where the radiosity boundaries are.
    #case (Radiosity_Debug)
      pretrace_start 0.08
      pretrace_end   0.02
      count 10
      nearest_count 1
      error_bound 0.3
      recursion_limit 1
      low_error_factor 0.8
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_Debug in use\n"
    #break

    // Make it look as good as you can, but I'm in a hurry
    #case (Radiosity_Fast)
      pretrace_start 0.08
      pretrace_end   0.02
      count 80
      nearest_count 5
      error_bound 0.4
      recursion_limit 1
      low_error_factor 0.9
      gray_threshold 0
      minimum_reuse 0.025
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_Fast in use\n"
    #break

    // Typical values
    #case (Radiosity_Normal)
      pretrace_start 0.08
      pretrace_end   0.01
      count 200
      nearest_count 7
      error_bound 0.3
      recursion_limit 1
      low_error_factor 0.75
      gray_threshold 0
      minimum_reuse 0.017
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_Normal in use\n"
    #break

    // Typical values, but with 2 bounces.  Starts slow, but picks up steam!
    #case (Radiosity_2Bounce)
      pretrace_start 0.08
      pretrace_end   0.01
      count 200
      nearest_count 7
      error_bound 0.3
      recursion_limit 2
      low_error_factor 0.75
      gray_threshold 0
      minimum_reuse 0.017
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_2Bounce in use\n"
    #break

    // For patient quality freaks with fast computers about to leave on vacation
    #case (Radiosity_Final)
      pretrace_start 0.08
      pretrace_end   0.004
      count 800
      nearest_count 9
      error_bound 0.2
      recursion_limit 1
      low_error_factor 0.7
      gray_threshold 0
      minimum_reuse 0.01
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_Final in use\n"
    #break

    // For outdoor scenes without light sources
    // Low quality being reasonably fast in most scenes
    #case (Radiosity_OutdoorLQ)
      pretrace_start 0.08
      pretrace_end   0.01
      count 80
      nearest_count 4
      error_bound 0.6
      recursion_limit 1      // increase this in scenes with complicated geometries when necessary
      low_error_factor 0.8
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_OutdoorLQ in use\n"
    #break

    // For outdoor scenes without light sources
    // High quality - can be very slow
    #case (Radiosity_OutdoorHQ)
      pretrace_start 0.08
      pretrace_end   0.004
      count 500
      nearest_count 7
      error_bound 0.1
      recursion_limit 1      // increase this in scenes with complicated geometries when necessary
      low_error_factor 0.5
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_OutdoorHQ in use\n"
    #break

    // Settings for outdoor scenes with (Sun)light
    // for getting a general skylight effect
    #case (Radiosity_OutdoorLight)
      pretrace_start 0.08
      pretrace_end   0.01
      count 50
      nearest_count 4
      error_bound 0.8
      recursion_limit 1
      low_error_factor 0.9
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_OutdoorLight in use\n"
    #break

    // quite fast settings for indoor radiosity
    // good values much depend on actual situation
    // these settings can be taken as a basis
    #case (Radiosity_IndoorLQ)
      pretrace_start 0.08
      pretrace_end   0.01
      count 80
      nearest_count 5
      error_bound 0.7
      recursion_limit 2
      low_error_factor 0.8
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_IndoorLQ in use\n"
    #break

    // slower settings for indoor radiosity
    // good values much depend on actual situation
    // these settings can be taken as a basis
    #case (Radiosity_IndoorHQ)
      pretrace_start 0.08
      pretrace_end   0.004
      count 400
      nearest_count 8
      error_bound 0.15
      recursion_limit 3
      low_error_factor 0.5
      gray_threshold 0
      minimum_reuse 0.015
      brightness 1.0
      adc_bailout 0.01/2
    #debug "\nRadiosity_IndoorHQ in use\n"
    #break
  #end

    normal Normal
    media Media
#end

#version Rad_Def2_Inc_Temp;
#end
