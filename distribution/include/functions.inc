// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File

#ifndef(Functions_Inc_Temp)
#declare Functions_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
#   debug "including function.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.5
            Many pre-defined functions for use in scene files.
*/

#declare f_algbr_cyl1 = function { internal(0) }
// Parameters: x, y, z 
    // Five extra parameters required:
    // 1. Field Strength 
    // 2. Field Limit 
    // 3. SOR Switch 
    // 4. SOR Offset 
    // 5. SOR Angle

#declare f_algbr_cyl2 = function { internal(1) }
// Parameters: x, y, z
    // Five extra parameters required:
    // 1. Field Strength 
    // 2. Field Limit 
    // 3. SOR Switch 
    // 4. SOR Offset 
    // 5. SOR Angle 
    
#declare f_algbr_cyl3 = function { internal(2) }
// Parameters: x, y, z
    // Five extra parameters required:
    // 1. Field Strength 
    // 2. Field Limit 
    // 3. SOR Switch 
    // 4. SOR Offset 
    // 5. SOR Angle 
    
#declare f_algbr_cyl4 = function { internal(3) }
// Parameters: x, y, z 
    // Five extra parameters required:
    // 1. Field Strength 
    // 2. Field Limit 
    // 3. SOR Switch 
    // 4. SOR Offset 
    // 5. SOR Angle 
    
#declare f_bicorn = function { internal(4) }
// Parameters: x, y, z 
    // Two extra parameters required:
    // 1. Field Strength 
    // 2. Scale. The surface is always the same shape. 
    //           Changing this parameter has the same effect as adding a scale modifier.
    //           Setting the scale to 1 gives a surface with a radius of about 1 unit.

#declare f_bifolia = function { internal(5) }
// Parameters: x, y, z 
    // Two extra parameters required:
    // 1. Field Strength 
    // 2. Scale. The mathematics of this surface suggest that the shape 
    //           should be different for different values of this parameter. 
    //           In practice the difference in shape is hard to spot. 
    //           Setting the scale to 3 gives a surface with a radius of about 1 unit.

#declare f_blob = function { internal(6) }
// Parameters: x, y, z 
    // Five extra parameters required:
    // 1. x distance between the two components 
    // 2. blob strength of component 1 
    // 3. inverse blob radius of component 1 
    // 4. blob strength of component 2 
    // 5.inverse blob radius of component 2
    
#declare f_blob2 = function { internal(7) }
// Parameters: x, y, z
    //Four extra parameters required: 
    // 1. Separation. One blob component is at the origin 
    //    and the other is this distance away on the x axis 
    // 2. inverse size. Increase this to decrease the size of the surface 
    // 3. blob strength 
    // 4. threshold. Setting this parameter to 1 
    //    and the threshold to zero has exactly the same effect
    //    as setting this parameter to zero and the threshold to -1

#declare f_boy_surface = function { internal(8) }
// Parameters: x, y, z 
    // Two extra parameters required:
    // 1. Field Strength 
    // 2. Scale. The surface is always the same shape. 
    //    Changing this parameter has the same effect as adding a scale modifier
    
#declare f_comma = function { internal(9) }
// Parameters: x, y, z
    // One extra parameter required: 
    // 1. scale

#declare f_cross_ellipsoids = function { internal(10) }
// Parameters: x, y, z 
    // Four extra paraameters required
    // 1. eccentricity. When less than 1, 
    //    the ellipsoids are oblate, 
    //    when greater than 1 the ellipsoids are prolate,
    //    when zero the ellipsoids are spherical 
    //    (and hence the whole surface is a sphere) 
    // 2. inverse size. Increase this to decrease the size of the surface 
    // 3. Diameter. Increase this to increase the size of the ellipsoids 
    // 4. threshold. Setting this parameter to 1 and the threshold to zero 
    //    has exactly the same effect as setting this parameter to zero 
    //    and the threshold to -1

#declare f_crossed_trough = function { internal(11) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1.  Field Strength 
    
#declare f_cubic_saddle = function { internal(12) }
// Parameters: x, y, z
    // One extra parameter required:
    // 1. Field Strength

#declare f_cushion = function { internal(13) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_devils_curve = function { internal(14) }
// Parameters: x, y, z
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_devils_curve_2d = function { internal(15) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. Field Strength 
    // 2. X factor 
    // 3. Y factor 
    // 4. SOR Switch 
    // 5. SOR Offset 
    // 6. SOR Angle
    
#declare f_dupin_cyclid = function { internal(16) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. Field Strength 
    // 2. Major radius of torus 
    // 3. Minor radius of torus 
    // 4. X displacement of torus 
    // 5. Y displacement of torus 
    // 6. Radius of inversion

#declare f_ellipsoid = function { internal(17) }
// Parameters: x, y, z 
    // Three extra parameters required:
    // 1. x scale (inverse) 
    // 2. y scale (inverse) 
    // 3. z scale (inverse)

#if (Functions_Inc_Temp < 3.8)
    #declare deprecated once "f_enneper was broken prior to v3.8; results will most likely differ."
             f_enneper = function { internal(18) }
#else
    #declare f_enneper = function { internal(18) }
#end
// Parameters: x, y, z
    // One extra parameter required: 
    // 1. Field strength
    
#declare f_flange_cover = function { internal(19) }
// Parameters: x, y, z 
    // Four extra parameters required:
    // 1. Spikiness. Set this to very low values to increase the spikes. 
    //    Set it to 1 and you get a sphere 
    // 2. inverse size. Increase this to decrease the size of the surface. 
    //    (The other parameters also drastically affect the size, 
    //     but this parameter has no other effects) 
    // 3. Flange. Increase this to increase the flanges that appear between the spikes. Set it to 1 for no flanges 
    // 4. threshold. Setting this parameter to 1 and the threshold to zero
    //    has exactly the same effect as setting this parameter to zero 
    //    and the threshold to -1
    
#declare f_folium_surface = function { internal(20) }
// Parameters: x, y, z 
    // Three extra parameters required:
    // 1. Field Strength 
    // 2. Neck width factor - 
    //    the larger you set this, 
    //    the narrower the neck where the paraboloid meets the plane 
    // 3. Divergence - 
    //    the higher you set this value, the wider the paraboloid gets

#declare f_folium_surface_2d = function { internal(21) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. Field Strength 
    // 2. Neck width factor - same as the 3d surface if you're revolving it around the Y axis 
    // 3. Divergence - 
    //    same as the 3d surface if you're revolving it around the Y axis 
    // 4. SOR Switch 
    // 5. SOR Offset 
    // 6. SOR Angle

#declare f_glob = function { internal(22) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_heart = function { internal(23) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_helical_torus = function { internal(24) }
// Parameters: x, y, z
    // Ten extra parameters required: 
    // 1.Major radius 
    // 2. Number of winding loops 
    // 3. Twistiness of winding. 
    //    When zero, each winding loop is separate. 
    //    When set to one, each loop twists into the next one. 
    //    When set to two, each loop twists into the one after next 
    // 4. Fatness of winding? 
    // 5. threshold. Setting this parameter to 1 and the threshold to zero 
    //    has s similar effect as setting this parameter to zero 
    //    and the threshold to 1 
    // 6. Negative minor radius? Reducing this parameter 
    //    increases the minor radius of the central torus. 
    //    Increasing it can make the torus disappear 
    //    and be replaced by a vertical column. 
    //    The value at which the surface switches 
    //    from one form to the other depends on several other parameters 
    // 7. Another fatness of winding control? 
    // 8. Groove period. Increase this for more grooves 
    // 9. Groove amplitude. Increase this for deeper grooves 
    // 10. Groove phase. Set this to zero for symmetrical grooves

#declare f_helix1 = function { internal(25) }
// Parameters: x, y, z 
// Parameters: x, y, z
    // Seven extra parameters required: 
    // 1. Number of helixes - e.g. 2 for a double helix 
    // 2. Period - is related to the number of turns per unit length 
    // 3. Minor radius 
    // 4. Major radius 
    // 5. Shape parameter. If this is greater than 1 then the tube becomes fatter in the y direction 
    // 6. Cross section type. 
    // 7. Cross section rotation angle (degrees). E.g. if you choose a square cross section and rotate it by 45 degrees you get a diamond cross section.
#declare f_helix2 = function { internal(26) }
    // Parameters: x, y, z
    // Seven extra parameters required: 
    // 1. Not used 
    // 2. Period - is related to the number of turns per unit length 
    // 3. Minor radius 
    // 4. Major radius 
    // 5. Not used 
    // 6. Cross section type. 
    // 7. Cross section rotation angle (degrees). E.g. if you choose a square cross section and rotate it by 45 degrees you get a diamond cross section.

#declare f_hex_x = function { internal(27) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Has no effect  ??????????????????????????????
     
#declare f_hex_y = function { internal(28) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Has no effect  ??????????????????????????????
    
#declare f_hetero_mf = function { internal(29) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. H 
    // 2. lacunarity 
    // 3. octaves 
    // 4. offset 
    // 5. T  
    // 6. noise
    
#declare f_hunt_surface = function { internal(30) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
#declare f_hyperbolic_torus = function { internal(31) }
// Parameters: x, y, z 
    // Three extra parameters required:
    // 1. Field strength 
    // 2. Major radius: separation between the centers of the tubes at the closest point 
    // 3. Minor radius: thickness of the tubes at the closest point 
    
#declare f_isect_ellipsoids = function { internal(32) }
// Parameters: x, y, z
    // Four extra parameters required:
    // 1. eccentricity. When less than 1, the ellipsoids are oblate, 
    //    when greater than 1 the ellipsoids are prolate, 
    //    when zero the ellipsoids are spherical 
    //   (and hence the whole surface is a sphere) 
    // 2. inverse size. Increase this to decrease the size of the surface 
    // 3. Diameter. Increase this to increase the size of the ellipsoids 
    // 4. threshold. Setting this parameter to 1 and the threshold to zero 
    //    has exactly the same effect as setting this parameter to zero 
    //    and the threshold to -1

#declare f_kampyle_of_eudoxus = function { internal(33) }
// Parameters: x, y, z
    // Three extra parameters required:
    // 1. Field strength 
    // 2. Dimple: When zero, the two dimples punch right through 
    //    and meet at the center. Non-zero values give less dimpling 
    // 3. Closeness: Higher values make the two planes become closer 
    
#declare f_kampyle_of_eudoxus_2d = function { internal(34) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. Field strength 
    // 2. Dimple: When zero, the two dimples punch right through and meet at the center. Non-zero values give less dimpling 
    // 3. loseness: Higher values make the two planes become closer 
    // 4. sor switch 
    // 5. sor offset 
    // 6. sor angle 

#declare f_klein_bottle = function { internal(35) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength

#declare f_kummer_surface_v1 = function { internal(36) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_kummer_surface_v2 = function { internal(37) }
// Parameters: x, y, z 
    // Four extra parameters required:
    // 1. Field strength 
    // 2. Rod width (negative): Setting this parameter 
    //    to larger negative values increases the diameter of the rods 
    // 3. Divergence (negative): Setting this number to -1 
    //    causes the rods to become approximately cylindrical. 
    //    Larger negative values cause the rods to become fatter further from the origin. 
    //    Smaller negative numbers cause the rods to become narrower away from the origin,
    //    and have a finite length 
    // 4. Influences the length of half of the rods. 
    //    Changing the sign affects the other half of the rods. 
    //    0 has no effect 
    
#declare f_lemniscate_of_gerono = function { internal(38) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_lemniscate_of_gerono_2d = function { internal(39) }
// Parameters: x, y, z
    // Six extra parameters required: 
    // 1. Field strength 
    // 2. size: increasing this makes the 2d curve larger and less rounded 
    // 3. width: increasing this makes the 2d curve fatter 
    // 4. sor switch 
    // 5. sor offset 
    // 6. sor angle
    
#declare f_mesh1 = function { internal(40) }
// Parameters: x, y, z 
    // Five extra Parameters required: 
    // 1. Distance between neighboring threads in the x direction 
    // 2. Distance between neighboring threads in the z direction 
    // 3. Relative thickness in the x and z directions 
    // 4. Amplitude of the weaving effect. Set to zero for a flat grid 
    // 5. Relative thickness in the y direction 
    
#declare f_mitre = function { internal(41) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength 
    
#declare f_nodal_cubic = function { internal(42) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength 
    
#declare f_odd = function { internal(43) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_ovals_of_cassini = function { internal(44) }
// Parameters: x, y, z 
    // Four parameters required:
    // 1. Field Strength 
    // 2. Major radius - like the major radius of a torus 
    // 3. Filling. Set this to zero, and you get a torus. Set this to a higher value and the hole in the middle starts to heal up. Set it even higher and you get an ellipsoid with a dimple 
    // 4. Thickness. The higher you set this value, the plumper is the result.

#declare f_paraboloid = function { internal(45) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_parabolic_torus = function { internal(46) }
// Parameters: x, y, z 
    // Tree extra parameters required:
    // 1. Field Strength 
    // 2. Major radius 
    // 3. Minor radius
        
#declare f_ph = function { internal(47) }
// Parameters: x, y, z 
    // no extra parameters required

#declare f_pillow = function { internal(48) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_piriform = function { internal(49) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_piriform_2d = function { internal(50) }
// Parameters: x, y, z 
    // Seven extra parameters required:
    // 1. Field strength 
    // 2. size factor 1: increasing this makes the curve larger 
    // 3. size factor 2: making this less negative makes the curve larger but also thinner 
    // 4. Fatness: increasing this makes the curve fatter 
    // 5. sor switch 
    // 6. sor offset 
    // 7. sor angle

#declare f_poly4 = function { internal(51) }
// Parameters: x, y, z 
    // Five extra parameters required:
    // 1. Constant 
    // 2. y coefficient 
    // 3. Y2 coefficient 
    // 4. Y3 coefficient 
    // 5. Y4 coefficient
    
#declare f_polytubes = function { internal(52) }
// Parameters: x, y, z
    // Six extra parameters required: 
    // 1. Number of tubes 
    // 2. Constant 
    // 3. y coefficient 
    // 4. Y2 coefficient 
    // 5. Y3 coefficient 
    // 6. Y4 coefficient
    
#declare f_quantum = function { internal(53) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Has no effect  ??????????????????????????????
    
#declare f_quartic_paraboloid = function { internal(54) }
// Parameters: x, y, z
    // One extra parameter required:
    // 1. Field Strength 
    
#declare f_quartic_saddle = function { internal(55) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength
    
#declare f_quartic_cylinder = function { internal(56) }
// Parameters: x, y, z 
    // Three extra parameters required:
    // 1. Field strength 
    // 2. Diameter of the "egg" 
    // 3. Controls the width of the tube and the vertical scale of the "egg"

#declare f_r = function { internal(57) }
// Parameters: x, y, z 
    // no extra parameters required
    
#declare f_ridge = function { internal(58) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1.lambda 
    // 2. octaves 
    // 3. omega 
    // 4. offset 
    // 5. Ridge
    // 6. noise
    
#declare f_ridged_mf = function { internal(59) }
// Parameters: x, y, z
    // Six extra parameters required:                          
    // 1. H 
    // 2. Lacunarity 
    // 3. octaves
    // 4. offset 
    // 5. Gain 
    // 6. noise
    
#declare f_rounded_box = function { internal(60) }
// Parameters: x, y, z, P0, P1, P2, P3
    // Four extra parameters required:
    // 1. Radius of curvature. 
    //    Zero gives square corners, 
    //    0.1 gives corners that match "sphere {0,0.1}"
    // 2. scale x
    // 3. scale y
    // 4. scale z
    
#declare f_sphere = function { internal(61) }
// Parameters: x, y, z  
    //One extra parameter required: 
    // 1. radius of sphere
    
#declare f_spikes = function { internal(62) }
// Parameters: x, y, z 
    // Five extra parameters required:
    // 1. Spikiness. Set this to very low values to increase the spikes. 
    //    Set it to 1 and you get a sphere 
    // 2. Hollowness. Increasing this causes the sides to bend in more 
    // 3. size. Increasing this increases the size of the object 
    // 4. Roundness. This parameter has a subtle effect on the roundness of the spikes 
    // 5. Fatness. Increasing this makes the spikes fatter
    
#declare f_spikes_2d = function { internal(63) }
// Parameters: x, y, z 
    // Four extra parameters required:
    // 1. Height of central spike 
    // 2. frequency of spikes in the x direction 
    // 3. frequency of spikes in the z direction 
    // 4. Rate at which the spikes reduce as you move away from the center 

#declare f_spiral = function { internal(64) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. Distance between windings: setting this to 0.3 means that the spiral is 0.3 pov units further from the origin each time it completes one whole turn 
    // 2. Thickness 
    // 3. Outer diameter of the spiral. The surface behaves as if it is contained_by a sphere of this diameter 
    // 4. Not used 
    // 5. Not used 
    // 6. Cross section type

#declare f_steiners_roman = function { internal(65) }
// Parameters: x, y, z
    // One extra parameter required:
    // 1. Field Strength 
    
#declare f_strophoid = function { internal(66) }
// Parameters: x, y, z
    // Four extra parameters required: 
    // 1. Field strength 
    // 2. size of bulb. Larger values give larger bulbs. 
    //    Negative values give a bulb on the other side of the plane 
    // 3. Sharpness. When zero, the bulb is like a sphere that just touches the plane. 
    //    When positive, there is a crossover point. 
    //    When negative the bulb simply bulges out of the plane like a pimple 
    // 4. Fatness. Higher values make the top end of the bulb fatter

#declare f_strophoid_2d = function { internal(67) }
// Parameters: x, y, z
    // Seven extra parameters required: 
    // 1. Field strength 
    // 2. size of bulb. Larger values give larger bulbs. 
    //    Negative values give a bulb on the other side of the plane 
    // 3. Sharpness. When zero, the bulb is like a sphere that just touches the plane. 
    //    When positive, there is a crossover point. 
    //    When negative the bulb simply bulges out of the plane like a pimple 
    // 4. Fatness. Higher values make the top end of the bulb fatter 
    // 5. sor switch 
    // 6. sor offset 
    // 7. sor angle
    
#declare f_superellipsoid = function { internal(68) }
// Parameters: x, y, z 
    // two extra parameters required:
    // 1. NW 
    // 2. SW
    
#declare f_th = function { internal(69) }
// Parameters: x, y, z 
    // no extra parameters required
    
#declare f_torus = function { internal(70) }
// Parameters: x, y, z
    // two extra parameters required:
    // 1. Major radius 
    // 2. Minor radius
    
#declare f_torus2 = function { internal(71) }
// Parameters: x, y, z
    // Three extra parameters required:
    // 1. Field strength 
    // 2. Major radius 
    // 3. Minor radius

#declare f_torus_gumdrop = function { internal(72) }
// Parameters: x, y, z 
    // One extra parameter required:
    // 1. Field Strength

#declare f_umbrella = function { internal(73) }
// Parameters: x, y, z
    // One extra parameter required:
    // 1. Field Strength 
#declare f_witch_of_agnesi = function { internal(74) }
// Parameters: x, y, z
    // Two extra parameters required:
    // 1. Field Strength
    // 2. Controls the width of the spike. 
    //    The height of the spike is always about 1 unit

#declare f_witch_of_agnesi_2d = function { internal(75) }
// Parameters: x, y, z 
    // Six extra parameters required:
    // 1. Field strength 
    // 2. Controls the size of the spike 
    // 3. Controls the height of the spike 
    // 4. sor switch 
    // 5. sor offset 
    // 6. sor angle

#declare f_noise3d = function { internal(76) }
// Parameters: x, y, z
//Noise in the range [-1, 1]
#declare f_snoise3d = function {2*f_noise3d(x, y, z) - 1}
// Parameters: x, y, z

#declare f_noise_generator = function { internal(78) }
// Parameters: x, y, z
    // One extra parameter required:
    // 1. noise_generator number


#macro eval_pigment(pigm, vec)
    #local fn = function { pigment { pigm } }
    #local result = (fn(vec.x, vec.y, vec.z));
    result
#end


// Isosurface pattern functions
// Some of these are not implemented because they require special
// parameters that must be specified in the definition. For this
// reason, you probably would want to define your own versions of
// these functions.
#declare f_agate = function {pattern {agate}}
// average not implemented
#declare f_boxed = function {pattern {boxed}}
#declare f_bozo = function {f_noise3d(x, y, z)}
#declare f_brick = function {pattern {brick}}
#declare f_bumps = function {f_noise3d(x, y, z)}
#declare f_checker = function {pattern {checker}}
#declare f_crackle = function {pattern {crackle}}
#declare f_cylindrical = function {pattern {cylindrical}}
// density_file not implemented
#declare f_dents = function {pow(f_noise3d(x, y, z), 3)}
#declare f_gradientX = function {pattern {gradient x}}
#declare f_gradientY = function {pattern {gradient y}}
#declare f_gradientZ = function {pattern {gradient z}}
#declare f_granite = function {pattern {granite}}
// image_pattern not implemented
#declare f_hexagon = function {pigment {hexagon color rgb 0, color rgb 0.5, color rgb 1}}
#declare f_leopard = function {pattern {leopard}}
//only the basic mandel pattern is implemented, its variants and
//the other fractal patterns are not implemented.
#declare f_mandel = function {pattern {mandel 25}}
#declare f_marble = function {pattern {marble}}
// object not implemented
#declare f_onion = function {pattern {onion}}
// pigment_pattern not implemented
#declare f_planar = function {pattern {planar}}
// quilted not implemented
#declare f_radial = function {pattern {radial}}
#declare f_ripples = function {pattern {ripples}}
#declare f_spherical = function {pattern {spherical}}
#declare f_spiral1 = function {pattern {spiral1 2}}
#declare f_spiral2 = function {pattern {spiral2 2}}
#declare f_spotted = function {f_noise3d(x, y, z)}
#declare f_waves = function {pattern {waves}}
#declare f_wood = function {pattern {wood}}
#declare f_wrinkles = function {pattern {wrinkles}}


// Waveform functions
#declare f_sine_wave = function {sin(x*z*pi)*y}
    // f_sine_wave(val, amplitude, frequency)
#declare f_scallop_wave = function {abs(sin(x*z*pi)*y)}
    // f_scallop_wave(val, amplitude, frequency)

#version Functions_Inc_Temp;
#end//functions.inc

