//******************************************************************************
///
/// @file parser/reservedwords.cpp
///
/// This file contains the list of reserved words as a global array. It is
/// kept separate from the parser to allow it to be linked in with GUI's that
/// may not link the core rendering code.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "parser/parser.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
// POV-Ray header files (parser module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov_parser
{

using namespace pov;

/*
 * Here are the reserved words.  If you need to add new words,
 * be sure to declare them in reservedwords.h
 */

const RESERVED_WORD Reserved_Words[] = {

    //------------------------------------------------------------------------------
    // Genuine Keywords.

    // Please keep this section neatly sorted by the actual keyword, sorting underscore characters
    // before digits, digits before letters, and short names before long ones.
    // Note that this should match with sorting the token identifier names accordingly, ignoring
    // the trailing `_TOKEN`.

    { AA_LEVEL_TOKEN,               "aa_level" },
    { AA_THRESHOLD_TOKEN,           "aa_threshold" },
    { ABSORPTION_TOKEN,             "absorption" },
    { ABS_TOKEN,                    "abs" },
    { ACCURACY_TOKEN,               "accuracy" },
    { ACOS_TOKEN,                   "acos" },
    { ACOSH_TOKEN,                  "acosh" },
    { ADAPTIVE_TOKEN,               "adaptive" },
    { ADC_BAILOUT_TOKEN,            "adc_bailout" },
    { AGATE_TOKEN,                  "agate" },
    { AGATE_TURB_TOKEN,             "agate_turb" },
    { ALBEDO_TOKEN,                 "albedo" },
    { ALL_TOKEN,                    "all" },
    { ALL_INTERSECTIONS_TOKEN,      "all_intersections" },
    { ALPHA_TOKEN,                  "alpha" },
    { ALTITUDE_TOKEN,               "altitude" },
    { ALWAYS_SAMPLE_TOKEN,          "always_sample" },
    { AMBIENT_TOKEN,                "ambient" },
    { AMBIENT_LIGHT_TOKEN,          "ambient_light" },
    { ANGLE_TOKEN,                  "angle" },
    { ANISOTROPY_TOKEN,             "anisotropy" },
    { AOI_TOKEN,                    "aoi" },
    { APERTURE_TOKEN,               "aperture" },
    { APPEND_TOKEN,                 "append" },
    { ARC_ANGLE_TOKEN,              "arc_angle" },
    { AREA_ILLUMINATION_TOKEN,      "area_illumination" },
    { AREA_LIGHT_TOKEN,             "area_light" },
    { ARRAY_TOKEN,                  "array" },
    { ASC_TOKEN,                    "asc" },
    { ASCII_TOKEN,                  "ascii" },
    { ASIN_TOKEN,                   "asin" },
    { ASINH_TOKEN,                  "asinh" },
    { ASSUMED_GAMMA_TOKEN,          "assumed_gamma" },
    { ATAN_TOKEN,                   "atan" },
    { ATAN2_TOKEN,                  "atan2" },
    { ATANH_TOKEN,                  "atanh" },
    { AUTOSTOP_TOKEN,               "autostop" },
    { AVERAGE_TOKEN,                "average" },

    { BACKGROUND_TOKEN,             "background" },
    { BEZIER_SPLINE_TOKEN,          "bezier_spline" },
    { BICUBIC_PATCH_TOKEN,          "bicubic_patch" },
    { BITWISE_AND_TOKEN,            "bitwise_and" },
    { BITWISE_OR_TOKEN,             "bitwise_or" },
    { BITWISE_XOR_TOKEN,            "bitwise_xor" },
    { BLACK_HOLE_TOKEN,             "black_hole" },
    { BLEND_GAMMA_TOKEN,            "blend_gamma" },
    { BLEND_MODE_TOKEN,             "blend_mode" },
    { BLOB_TOKEN,                   "blob" },
    { BLUE_TOKEN,                   "blue" },
    { BLUR_SAMPLES_TOKEN,           "blur_samples" },
    { BMP_TOKEN,                    "bmp" },
    { BOKEH_TOKEN,                  "bokeh" },
    { BOUNDED_BY_TOKEN,             "bounded_by" },
    { BOX_TOKEN,                    "box" },
    { BOXED_TOKEN,                  "boxed" },
    { BOZO_TOKEN,                   "bozo" },
    { B_SPLINE_TOKEN,               "b_spline" },
    { BREAK_TOKEN,                  "break" },
#if POV_DEBUG
    { BREAKPOINT_TOKEN,             "breakpoint" },
#endif
    { BRICK_TOKEN,                  "brick" },
    { BRICK_SIZE_TOKEN,             "brick_size" },
    { BRIGHTNESS_TOKEN,             "brightness" },
    { BRILLIANCE_TOKEN,             "brilliance" },
    { BT2020_TOKEN,                 "bt2020" },
    { BT709_TOKEN,                  "bt709" },
    { BUMP_MAP_TOKEN,               "bump_map" },
    { BUMP_SIZE_TOKEN,              "bump_size" },
    { BUMPS_TOKEN,                  "bumps" },

    { CAMERA_TOKEN,                 "camera" },
    { CASE_TOKEN,                   "case" },
    { CAUSTICS_TOKEN,               "caustics" },
    { CEIL_TOKEN,                   "ceil" },
    { CELLS_TOKEN,                  "cells" },
    { CHARSET_TOKEN,                "charset" },
    { CHECKER_TOKEN,                "checker" },
    { CHR_TOKEN,                    "chr" },
    { CIRCULAR_TOKEN,               "circular" },
    { CLIPPED_BY_TOKEN,             "clipped_by" },
    { CLOCK_TOKEN,                  "clock" },
    { CLOCK_ON_TOKEN,               "clock_on" },
    { CMAP_TOKEN,                   "cmap" },
    { COLLECT_TOKEN,                "collect" },
    { COLOUR_TOKEN,                 "colour" },
    { COLOUR_MAP_TOKEN,             "colour_map" },
    { COMPONENT_TOKEN,              "component" },
    { COMPOSITE_TOKEN,              "composite" },
    { CONCAT_TOKEN,                 "concat" },
    { CONE_TOKEN,                   "cone" },
    { CONFIDENCE_TOKEN,             "confidence" },
    { CONIC_SWEEP_TOKEN,            "conic_sweep" },
    { CONSERVE_ENERGY_TOKEN,        "conserve_energy" },
    { CONTAINED_BY_TOKEN,           "contained_by" },
    { CONTROL0_TOKEN,               "control0" },
    { CONTROL1_TOKEN,               "control1" },
    { COORDS_TOKEN,                 "coords" },
    { COS_TOKEN,                    "cos" },
    { COSH_TOKEN,                   "cosh" },
    { COUNT_TOKEN,                  "count" },
    { CRACKLE_TOKEN,                "crackle" },
    { CRAND_TOKEN,                  "crand" },
    { CUBE_TOKEN,                   "cube" },
    { CUBIC_TOKEN,                  "cubic" },
    { CUBIC_SPLINE_TOKEN,           "cubic_spline" },
    { CUBIC_WAVE_TOKEN,             "cubic_wave" },
    { CUTAWAY_TEXTURES_TOKEN,       "cutaway_textures" },
    { CYLINDER_TOKEN,               "cylinder" },
    { CYLINDRICAL_TOKEN,            "cylindrical" },

    { DATETIME_TOKEN,               "datetime" },
    { DEBUG_TOKEN,                  "debug" },
    { DECLARE_TOKEN,                "declare" },
    { DEFAULT_TOKEN,                "default" },
    { DEFINED_TOKEN,                "defined" },
    { DEGREES_TOKEN,                "degrees" },
    { DENSITY_TOKEN,                "density" },
    { DENSITY_FILE_TOKEN,           "density_file" },
    { DENSITY_MAP_TOKEN,            "density_map" },
    { DENTS_TOKEN,                  "dents" },
    { DEPRECATED_TOKEN,             "deprecated" },
    { DF3_TOKEN,                    "df3" },
    { DICTIONARY_TOKEN,             "dictionary" },
    { DIFFERENCE_TOKEN,             "difference" },
    { DIFFUSE_TOKEN,                "diffuse" },
    { DIMENSION_SIZE_TOKEN,         "dimension_size" },
    { DIMENSIONS_TOKEN,             "dimensions" },
    { DIRECTION_TOKEN,              "direction" },
    { DISC_TOKEN,                   "disc" },
    { DISPERSION_TOKEN,             "dispersion" },
    { DISPERSION_SAMPLES_TOKEN,     "dispersion_samples" },
    { DIST_EXP_TOKEN,               "dist_exp" },
    { DISTANCE_TOKEN,               "distance" },
#if 0 // [CLi] the distance_maximum token is perfectly obsolete
    { DISTANCE_MAXIMUM_TOKEN,       "distance_maximum" },
#endif
    { DIV_TOKEN,                    "div" },
    { DOUBLE_ILLUMINATE_TOKEN,      "double_illuminate" },
    { DEBUG_TAG_TOKEN,              "dtag" },

    { ECCENTRICITY_TOKEN,           "eccentricity" },
    { ELSE_TOKEN,                   "else" },
    { ELSEIF_TOKEN,                 "elseif" },
    { EMISSION_TOKEN,               "emission" },
    { END_TOKEN,                    "end" },
    { ERROR_TOKEN,                  "error" },
    { ERROR_BOUND_TOKEN,            "error_bound" },
    { EVALUATE_TOKEN,               "evaluate" },
    { EXP_TOKEN,                    "exp" },
    { EXPAND_THRESHOLDS_TOKEN,      "expand_thresholds" },
    { EXPONENT_TOKEN,               "exponent" },
    { EXR_TOKEN,                    "exr" },
    { EXTERIOR_TOKEN,               "exterior" },
    { EXTINCTION_TOKEN,             "extinction" },

    { FACE_INDICES_TOKEN,           "face_indices" },
    { FACETS_TOKEN,                 "facets" },
    { FADE_COLOUR_TOKEN,            "fade_colour" },
    { FADE_DISTANCE_TOKEN,          "fade_distance" },
    { FADE_POWER_TOKEN,             "fade_power" },
    { FALLOFF_TOKEN,                "falloff" },
    { FALLOFF_ANGLE_TOKEN,          "falloff_angle" },
    { FALSE_TOKEN,                  "false" },
    { FCLOSE_TOKEN,                 "fclose" },
    { FILE_EXISTS_TOKEN,            "file_exists" },
    { FILTER_TOKEN,                 "filter" },
    { FINISH_TOKEN,                 "finish" },
    { FISHEYE_TOKEN,                "fisheye" },
    { FLATNESS_TOKEN,               "flatness" },
    { FLIP_TOKEN,                   "flip" },
    { FLOOR_TOKEN,                  "floor" },
    { FOCAL_POINT_TOKEN,            "focal_point" },
    { FOG_TOKEN,                    "fog" },
    { FOG_ALT_TOKEN,                "fog_alt" },
    { FOG_OFFSET_TOKEN,             "fog_offset" },
    { FOG_TYPE_TOKEN,               "fog_type" },
    { FOPEN_TOKEN,                  "fopen" },
    { FOR_TOKEN,                    "for" },
    { FORM_TOKEN,                   "form" },
    { FREQUENCY_TOKEN,              "frequency" },
    { FRESNEL_TOKEN,                "fresnel" },
    { FUNCTION_TOKEN,               "function" },

    { GAMMA_TOKEN,                  "gamma" },
    { GATHER_TOKEN,                 "gather" },
    { GIF_TOKEN,                    "gif" },
    { GLOBAL_TOKEN,                 "global" },
    { GLOBAL_LIGHTS_TOKEN,          "global_lights" },
    { GLOBAL_SETTINGS_TOKEN,        "global_settings" },
    { GRADIENT_TOKEN,               "gradient" },
    { GRANITE_TOKEN,                "granite" },
    { GRAY_TOKEN,                   "gray" },
    { GRAY_THRESHOLD_TOKEN,         "gray_threshold" },
    { GREEN_TOKEN,                  "green" },

    { HDR_TOKEN,                    "hdr" },
    { HEIGHT_FIELD_TOKEN,           "height_field" },
    { HEXAGON_TOKEN,                "hexagon" },
    { HF_GRAY_16_TOKEN,             "hf_gray_16" },
    { HIERARCHY_TOKEN,              "hierarchy" },
    { HOLLOW_TOKEN,                 "hollow" },
    { HYPERCOMPLEX_TOKEN,           "hypercomplex" },

    { IF_TOKEN,                     "if" },
    { IFDEF_TOKEN,                  "ifdef" },
    { IFF_TOKEN,                    "iff" },
    { IFNDEF_TOKEN,                 "ifndef" },
    { IMAGE_MAP_TOKEN,              "image_map" },
    { IMAGE_PATTERN_TOKEN,          "image_pattern" },
    { IMPORTANCE_TOKEN,             "importance" },
    { INCLUDE_TOKEN,                "include" },
    { INSIDE_TOKEN,                 "inside" },
    { INSIDE_VECTOR_TOKEN,          "inside_vector" },
    { INT_TOKEN,                    "int" },
    { INTERIOR_TOKEN,               "interior" },
    { INTERIOR_TEXTURE_TOKEN,       "interior_texture" },
    { INTERNAL_TOKEN,               "internal" },
    { INTERPOLATE_TOKEN,            "interpolate" },
    { INTERSECTION_TOKEN,           "intersection" },
    { INTERVALS_TOKEN,              "intervals" },
    { INVERSE_TOKEN,                "inverse" },
    { IOR_TOKEN,                    "ior" },
    { IRID_TOKEN,                   "irid" },
    { IRID_WAVELENGTH_TOKEN,        "irid_wavelength" },
    { ISOSURFACE_TOKEN,             "isosurface" },

    { JITTER_TOKEN,                 "jitter" },
    { JPEG_TOKEN,                   "jpeg" },
    { JULIA_TOKEN,                  "julia" },
    { JULIA_FRACTAL_TOKEN,          "julia_fractal" },

    { LAMBDA_TOKEN,                 "lambda" },
    { LATHE_TOKEN,                  "lathe" },
    { LEMON_TOKEN,                  "lemon" },
    { LEOPARD_TOKEN,                "leopard" },
    { LIGHT_GROUP_TOKEN,            "light_group" },
    { LIGHT_SOURCE_TOKEN,           "light_source" },
    { LINEAR_SPLINE_TOKEN,          "linear_spline" },
    { LINEAR_SWEEP_TOKEN,           "linear_sweep" },
    { LN_TOKEN,                     "ln" },
    { LOAD_FILE_TOKEN,              "load_file" },
    { LOCAL_TOKEN,                  "local" },
    { LOCATION_TOKEN,               "location" },
    { LOG_TOKEN,                    "log" },
    { LOOK_AT_TOKEN,                "look_at" },
    { LOOKS_LIKE_TOKEN,             "looks_like" },
    { LOW_ERROR_FACTOR_TOKEN,       "low_error_factor" },

    { MACRO_TOKEN,                  "macro" },
    { MAGNET_TOKEN,                 "magnet" },
    { MAJOR_RADIUS_TOKEN,           "major_radius" },
    { MANDEL_TOKEN,                 "mandel" },
    { MAP_TYPE_TOKEN,               "map_type" },
    { MARBLE_TOKEN,                 "marble" },
    { MATERIAL_TOKEN,               "material" },
    { MATERIAL_MAP_TOKEN,           "material_map" },
    { MATRIX_TOKEN,                 "matrix" },
    { MAX_TOKEN,                    "max" },
    { MAX_EXTENT_TOKEN,             "max_extent" },
    { MAX_GRADIENT_TOKEN,           "max_gradient" },
    { MAX_INTERSECTIONS_TOKEN,      "max_intersections" },
    { MAX_ITERATION_TOKEN,          "max_iteration" },
    { MAX_SAMPLE_TOKEN,             "max_sample" },
    { MAX_TRACE_TOKEN,              "max_trace" },
    { MAX_TRACE_LEVEL_TOKEN,        "max_trace_level" },
    { MAXIMUM_REUSE_TOKEN,          "maximum_reuse" },
    { MEDIA_TOKEN,                  "media" },
    { MEDIA_ATTENUATION_TOKEN,      "media_attenuation" },
    { MEDIA_INTERACTION_TOKEN,      "media_interaction" },
    { MERGE_TOKEN,                  "merge" },
    { MESH_TOKEN,                   "mesh" },
    { MESH_CAMERA_TOKEN,            "mesh_camera" },
    { MESH2_TOKEN,                  "mesh2" },
    { METALLIC_TOKEN,               "metallic" },
    { METHOD_TOKEN,                 "method" },
    { METRIC_TOKEN,                 "metric" },
    { MIN_TOKEN,                    "min" },
    { MIN_EXTENT_TOKEN,             "min_extent" },
    { MINIMUM_REUSE_TOKEN,          "minimum_reuse" },
    { MIXED_TOKEN,                  "mixed" },
    { MM_PER_UNIT_TOKEN,            "mm_per_unit" },
    { MOD_TOKEN,                    "mod" },
    { MORTAR_TOKEN,                 "mortar" },

    { NATURAL_SPLINE_TOKEN,         "natural_spline" },
    { NEAREST_COUNT_TOKEN,          "nearest_count" },
    { NO_TOKEN,                     "no" },
    { NO_BUMP_SCALE_TOKEN,          "no_bump_scale" },
    { NO_IMAGE_TOKEN,               "no_image" },
    { NO_RADIOSITY_TOKEN,           "no_radiosity" },
    { NO_REFLECTION_TOKEN,          "no_reflection" },
    { NO_SHADOW_TOKEN,              "no_shadow" },
    { NOISE_GENERATOR_TOKEN,        "noise_generator" },
    { NORMAL_TOKEN,                 "normal" },
    { NORMAL_INDICES_TOKEN,         "normal_indices" },
    { NORMAL_MAP_TOKEN,             "normal_map" },
    { NORMAL_VECTORS_TOKEN,         "normal_vectors" },
    { NOW_TOKEN,                    "now" },
    { NUMBER_OF_SIDES_TOKEN,        "number_of_sides" },
    { NUMBER_OF_TILES_TOKEN,        "number_of_tiles" },
    { NUMBER_OF_WAVES_TOKEN,        "number_of_waves" },

#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    { OBJ_TOKEN,                    "obj" },
#endif
    { OBJECT_TOKEN,                 "object" },
    { OCTAVES_TOKEN,                "octaves" },
    { OFF_TOKEN,                    "off" },
    { OFFSET_TOKEN,                 "offset" },
    { OMEGA_TOKEN,                  "omega" },
    { OMNIMAX_TOKEN,                "omnimax" },
    { ON_TOKEN,                     "on" },
    { ONCE_TOKEN,                   "once" },
    { ONION_TOKEN,                  "onion" },
    { OPEN_TOKEN,                   "open" },
    { OPTIONAL_TOKEN,               "optional" },
    { ORIENT_TOKEN,                 "orient" },
    { ORIENTATION_TOKEN,            "orientation" },
    { ORTHOGRAPHIC_TOKEN,           "orthographic" },
    { OVUS_TOKEN,                   "ovus" },

    { PANORAMIC_TOKEN,              "panoramic" },
    { PARALLEL_TOKEN,               "parallel" },
    { PARAMETRIC_TOKEN,             "parametric" },
    { PASS_THROUGH_TOKEN,           "pass_through" },
    { PATTERN_TOKEN,                "pattern" },
    { PAVEMENT_TOKEN,               "pavement" },
    { PERSPECTIVE_TOKEN,            "perspective" },
    { PGM_TOKEN,                    "pgm" },
    { PHASE_TOKEN,                  "phase" },
    { PHONG_TOKEN,                  "phong" },
    { PHONG_SIZE_TOKEN,             "phong_size" },
    { PHOTONS_TOKEN,                "photons" },
    { PI_TOKEN,                     "pi" },
    { PIGMENT_TOKEN,                "pigment" },
    { PIGMENT_MAP_TOKEN,            "pigment_map" },
    { PIGMENT_PATTERN_TOKEN,        "pigment_pattern" },
    { PLANAR_TOKEN,                 "planar" },
    { PLANE_TOKEN,                  "plane" },
    { PNG_TOKEN,                    "png" },
    { POINT_AT_TOKEN,               "point_at" },
    { POLARITY_TOKEN,               "polarity" },
    { POLY_TOKEN,                   "poly" },
    { POLY_WAVE_TOKEN,              "poly_wave" },
    { POLYGON_TOKEN,                "polygon" },
    { POLYNOMIAL_TOKEN,             "polynomial" },
    { POT_TOKEN,                    "pot" },
    { POTENTIAL_TOKEN,              "potential" },
    { POW_TOKEN,                    "pow" },
    { PPM_TOKEN,                    "ppm" },
    { PRECISION_TOKEN,              "precision" },
    { PRECOMPUTE_TOKEN,             "precompute" },
#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    { PREFIX_TOKEN,                 "prefix" },
#endif
    { PREMULTIPLIED_TOKEN,          "premultiplied" },
    { PRETRACE_END_TOKEN,           "pretrace_end" },
    { PRETRACE_START_TOKEN,         "pretrace_start" },
    { PRISM_TOKEN,                  "prism" },
    { PROD_TOKEN,                   "prod" },
    { PROJECTED_THROUGH_TOKEN,      "projected_through" },
    { PWR_TOKEN,                    "pwr" },

    { QUADRATIC_SPLINE_TOKEN,       "quadratic_spline" },
    { QUADRIC_TOKEN,                "quadric" },
    { QUARTIC_TOKEN,                "quartic" },
    { QUATERNION_TOKEN,             "quaternion" },
    { QUICK_COLOUR_TOKEN,           "quick_colour" },
    { QUILTED_TOKEN,                "quilted" },

    { RADIAL_TOKEN,                 "radial" },
    { RADIANS_TOKEN,                "radians" },
    { RADIOSITY_TOKEN,              "radiosity" },
    { RADIUS_TOKEN,                 "radius" },
    { RAINBOW_TOKEN,                "rainbow" },
    { RAMP_WAVE_TOKEN,              "ramp_wave" },
    { RAND_TOKEN,                   "rand" },
    { RANGE_TOKEN,                  "range" },
    { RATIO_TOKEN,                  "ratio" },
    { READ_TOKEN,                   "read" },
    { RECIPROCAL_TOKEN,             "reciprocal" },
    { RECURSION_LIMIT_TOKEN,        "recursion_limit" },
    { RED_TOKEN,                    "red" },
    { REFLECTION_TOKEN,             "reflection" },
    { REFLECTION_EXPONENT_TOKEN,    "reflection_exponent" },
    { REFRACTION_TOKEN,             "refraction" },
    { RENDER_TOKEN,                 "render" },
    { REPEAT_TOKEN,                 "repeat" },
    { RGB_TOKEN,                    "rgb" },
    { RGBF_TOKEN,                   "rgbf" },
    { RGBFT_TOKEN,                  "rgbft" },
    { RGBT_TOKEN,                   "rgbt" },
    { RIGHT_TOKEN,                  "right" },
    { RIPPLES_TOKEN,                "ripples" },
    { ROTATE_TOKEN,                 "rotate" },
    { ROUGHNESS_TOKEN,              "roughness" },

    { SAMPLES_TOKEN,                "samples" },
    { SAVE_FILE_TOKEN,              "save_file" },
#if 0 // sred, sgreen and sblue tokens not enabled at present
    { SBLUE_TOKEN,                  "sblue" },
#endif
    { SCALE_TOKEN,                  "scale" },
    { SCALLOP_WAVE_TOKEN,           "scallop_wave" },
    { SCATTERING_TOKEN,             "scattering" },
    { SEED_TOKEN,                   "seed" },
    { SELECT_TOKEN,                 "select" },
#if 0 // sred, sgreen and sblue tokens not enabled at present
    { SGREEN_TOKEN,                 "sgreen" },
#endif
    { SHADOWLESS_TOKEN,             "shadowless" },
    { SINE_WAVE_TOKEN,              "sine_wave" },
    { SIN_TOKEN,                    "sin" },
    { SINH_TOKEN,                   "sinh" },
    { SINT16BE_TOKEN,               "sint16be" },
    { SINT16LE_TOKEN,               "sint16le" },
    { SINT32BE_TOKEN,               "sint32be" },
    { SINT32LE_TOKEN,               "sint32le" },
    { SINT8_TOKEN,                  "sint8" },
    { SIZE_TOKEN,                   "size" },
    { SKY_TOKEN,                    "sky" },
    { SKYSPHERE_TOKEN,              "sky_sphere" },
    { SLICE_TOKEN,                  "slice" },
    { SLOPE_TOKEN,                  "slope" },
    { SLOPE_MAP_TOKEN,              "slope_map" },
    { SMOOTH_TOKEN,                 "smooth" },
    { SMOOTH_TRIANGLE_TOKEN,        "smooth_triangle" },
    { SOLID_TOKEN,                  "solid" },
    { SOR_TOKEN,                    "sor" },
    { SPACING_TOKEN,                "spacing" },
    { SPECULAR_TOKEN,               "specular" },
    { SPHERE_TOKEN,                 "sphere" },
    { SPHERE_SWEEP_TOKEN,           "sphere_sweep" },
    { SPHERICAL_TOKEN,              "spherical" },
    { SPIRAL1_TOKEN,                "spiral1" },
    { SPIRAL2_TOKEN,                "spiral2" },
    { SPLINE_TOKEN,                 "spline" },
    { SPLIT_UNION_TOKEN,            "split_union" },
    { SPOTLIGHT_TOKEN,              "spotlight" },
    { SPOTTED_TOKEN,                "spotted" },
    { SQRT_TOKEN,                   "sqrt" },
    { SQR_TOKEN,                    "sqr" },
    { SQUARE_TOKEN,                 "square" },
#if 0 // sred, sgreen and sblue tokens not enabled at present
    { SRED_TOKEN,                   "sred" },
#endif
    { SRGB_TOKEN,                   "srgb" },
    { SRGBF_TOKEN,                  "srgbf" },
    { SRGBFT_TOKEN,                 "srgbft" },
    { SRGBT_TOKEN,                  "srgbt" },
    { STATISTICS_TOKEN,             "statistics" },
    { STR_TOKEN,                    "str" },
    { STRCMP_TOKEN,                 "strcmp" },
    { STRENGTH_TOKEN,               "strength" },
    { STRLEN_TOKEN,                 "strlen" },
    { STRLWR_TOKEN,                 "strlwr" },
    { STRUPR_TOKEN,                 "strupr" },
    { STURM_TOKEN,                  "sturm" },
    { SUBSTR_TOKEN,                 "substr" },
    { SUBSURFACE_TOKEN,             "subsurface" },
#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    { SUFFIX_TOKEN,                 "suffix" },
#endif
    { SUM_TOKEN,                    "sum" },
    { SUPERELLIPSOID_TOKEN,         "superellipsoid" },
    { SWITCH_TOKEN,                 "switch" },
    { SYS_TOKEN,                    "sys" },

    { T_TOKEN,                      "t" },
    { TAN_TOKEN,                    "tan" },
    { TANH_TOKEN,                   "tanh" },
    { TARGET_TOKEN,                 "target" },
    { TAU_TOKEN,                    "tau" },
    { TEXT_TOKEN,                   "text" },
    { TEXTURE_TOKEN,                "texture" },
    { TEXTURE_LIST_TOKEN,           "texture_list" },
    { TEXTURE_MAP_TOKEN,            "texture_map" },
    { TGA_TOKEN,                    "tga" },
    { THICKNESS_TOKEN,              "thickness" },
    { THRESHOLD_TOKEN,              "threshold" },
    { TIFF_TOKEN,                   "tiff" },
    { TIGHTNESS_TOKEN,              "tightness" },
    { TILE2_TOKEN,                  "tile2" },
    { TILES_TOKEN,                  "tiles" },
    { TILING_TOKEN,                 "tiling" },
    { TOLERANCE_TOKEN,              "tolerance" },
    { TOROIDAL_TOKEN,               "toroidal" },
    { TORUS_TOKEN,                  "torus" },
    { TRACE_TOKEN,                  "trace" },
    { TRANSFORM_TOKEN,              "transform" },
    { TRANSLATE_TOKEN,              "translate" },
    { TRANSLUCENCY_TOKEN,           "translucency" },
    { TRANSMIT_TOKEN,               "transmit" },
    { TRIANGLE_TOKEN,               "triangle" },
    { TRIANGLE_WAVE_TOKEN,          "triangle_wave" },
    { TRIANGULAR_TOKEN,             "triangular" },
    { TRUE_TOKEN,                   "true" },
    { TTF_TOKEN,                    "ttf" },
    { TURB_DEPTH_TOKEN,             "turb_depth" },
    { TURBULENCE_TOKEN,             "turbulence" },
    { TYPE_TOKEN,                   "type" },

    { U_TOKEN,                      "u" },
    { U_STEPS_TOKEN,                "u_steps" },
    { UINT16BE_TOKEN,               "uint16be" },
    { UINT16LE_TOKEN,               "uint16le" },
    { UINT8_TOKEN,                  "uint8" },
    { ULTRA_WIDE_ANGLE_TOKEN,       "ultra_wide_angle" },
    { UNDEF_TOKEN,                  "undef" },
    { UNION_TOKEN,                  "union" },
    { UNOFFICIAL_TOKEN,             "unofficial" },
    { UP_TOKEN,                     "up" },
    { USE_ALPHA_TOKEN,              "use_alpha" },
    { USE_COLOUR_TOKEN,             "use_colour" },
    { USE_INDEX_TOKEN,              "use_index" },
    { USER_DEFINED_TOKEN,           "user_defined" },
    { UTF8_TOKEN,                   "utf8" },
    { UV_INDICES_TOKEN,             "uv_indices" },
    { UV_MAPPING_TOKEN,             "uv_mapping" },
    { UV_VECTORS_TOKEN,             "uv_vectors" },

    { V_TOKEN,                      "v" },
    { V_STEPS_TOKEN,                "v_steps" },
    { VAL_TOKEN,                    "val" },
    { VARIANCE_TOKEN,               "variance" },
    { VAXIS_ROTATE_TOKEN,           "vaxis_rotate" },
    { VCROSS_TOKEN,                 "vcross" },
    { VDOT_TOKEN,                   "vdot" },
    { VERSION_TOKEN,                "version" },
    { VERTEX_VECTORS_TOKEN,         "vertex_vectors" },
    { VLENGTH_TOKEN,                "vlength" },
    { VNORMALIZE_TOKEN,             "vnormalize" },
    { VROTATE_TOKEN,                "vrotate" },
    { VSTR_TOKEN,                   "vstr" },
    { VTURBULENCE_TOKEN,            "vturbulence" },

    { WARNING_TOKEN,                "warning" },
    { WARP_TOKEN,                   "warp" },
    { WATER_LEVEL_TOKEN,            "water_level" },
    { WAVES_TOKEN,                  "waves" },
    { WHILE_TOKEN,                  "while" },
    { WIDTH_TOKEN,                  "width" },
    { WOOD_TOKEN,                   "wood" },
    { WRINKLES_TOKEN,               "wrinkles" },
    { WRITE_TOKEN,                  "write" },

    { X_TOKEN,                      "x" },
    { XYZ_TOKEN,                    "xyz" },

    { Y_TOKEN,                      "y" },
    { YES_TOKEN,                    "yes" },

    { Z_TOKEN,                      "z" },

    //------------------------------------------------------------------------------
    // Alternative Spellings.

    { COLOUR_TOKEN,                 "color" },
    { COLOUR_MAP_TOKEN,             "color_map" },
    { FADE_COLOUR_TOKEN,            "fade_color" },
    { GRAY_TOKEN,                   "grey" },
    { GRAY_THRESHOLD_TOKEN,         "grey_threshold" },
    { HF_GRAY_16_TOKEN,             "hf_grey_16" },
    { QUICK_COLOUR_TOKEN,           "quick_color" },
    { USE_COLOUR_TOKEN,             "use_color" },

    //------------------------------------------------------------------------------
    // Operators.

    // Please keep this section neatly sorted by the token identifier name,
    // _ignoring_ the trailing `_TOKEN`.

    { AMPERSAND_TOKEN,              "&" },
    { AT_TOKEN,                     "@" },
    { BACK_QUOTE_TOKEN,             "`" },
    { BACK_SLASH_TOKEN,             "\\" },
    { BAR_TOKEN,                    "|" },
    { COLON_TOKEN,                  ":" },
    { COMMA_TOKEN,                  "," },
    { DASH_TOKEN,                   "-" },
    { DOLLAR_TOKEN,                 "$" },
    { EQUALS_TOKEN,                 "=" },
    { EXCLAMATION_TOKEN,            "!" },
    { HASH_TOKEN,                   "#" },
    { HAT_TOKEN,                    "^" },
    { LEFT_ANGLE_TOKEN,             "<" },
    { LEFT_CURLY_TOKEN,             "{" },
    { LEFT_PAREN_TOKEN,             "(" },
    { LEFT_SQUARE_TOKEN,            "[" },
    { PERCENT_TOKEN,                "%" },
    { PERIOD_TOKEN,                 "." },
    { PLUS_TOKEN,                   "+" },
    { QUESTION_TOKEN,               "?" },
    { REL_GE_TOKEN,                 ">=" },
    { REL_LE_TOKEN,                 "<=" },
    { REL_NE_TOKEN,                 "!=" },
    { RIGHT_ANGLE_TOKEN,            ">" },
    { RIGHT_CURLY_TOKEN,            "}" },
    { RIGHT_PAREN_TOKEN,            ")" },
    { RIGHT_SQUARE_TOKEN,           "]" },
    { SEMI_COLON_TOKEN,             ";" },
    { SINGLE_QUOTE_TOKEN,           "'" },
    { SLASH_TOKEN,                  "/" },
    { STAR_TOKEN,                   "*" },
    { TILDE_TOKEN,                  "~" },

    //------------------------------------------------------------------------------
    // Category Pseudo-Tokens.

    { COLOUR_TOKEN_CATEGORY,        "colour keyword" },
    { FLOAT_TOKEN_CATEGORY,         "float function" },
    { SIGNATURE_TOKEN_CATEGORY,     "file signature" },
    { VECTOR_TOKEN_CATEGORY,        "vector function" },

    //------------------------------------------------------------------------------
    // Identifier Pseudo-Tokens.

    // Please keep this section neatly sorted by the token identifier name,
    // _ignoring_ the trailing `_ID_TOKEN`.

    { ARRAY_ID_TOKEN,               "array identifier" },
    { CAMERA_ID_TOKEN,              "camera identifier" },
    { COLOUR_ID_TOKEN,              "colour identifier" },
    { COLOUR_MAP_ID_TOKEN,          "colour_map identifier" },
    { DENSITY_ID_TOKEN,             "density identifier" },
    { DENSITY_MAP_ID_TOKEN,         "density_map identifier" },
    { DICTIONARY_ID_TOKEN,          "dictionary identifier" },
    { FILE_ID_TOKEN,                "file identifier" },
    { FINISH_ID_TOKEN,              "finish identifier" },
    { FLOAT_ID_TOKEN,               "float identifier" },
    { FOG_ID_TOKEN,                 "fog identifier" },
    { FUNCT_ID_TOKEN,               "function identifier" },
    { INTERIOR_ID_TOKEN,            "interior identifier" },
    { MACRO_ID_TOKEN,               "macro identifier" },
    { MATERIAL_ID_TOKEN,            "material identifier" },
    { MEDIA_ID_TOKEN,               "media identifier" },
    { NORMAL_ID_TOKEN,              "normal identifier" },
    { NORMAL_MAP_ID_TOKEN,          "normal_map identifier" },
    { OBJECT_ID_TOKEN,              "object identifier" },
    { PARAMETER_ID_TOKEN,           "parameter identifier" },
    { PIGMENT_ID_TOKEN,             "pigment identifier" },
    { PIGMENT_MAP_ID_TOKEN,         "pigment_map identifier" },
    { RAINBOW_ID_TOKEN,             "rainbow identifier" },
    { SKYSPHERE_ID_TOKEN,           "sky_sphere identifier" },
    { SLOPE_MAP_ID_TOKEN,           "slope_map identifier" },
    { SPLINE_ID_TOKEN,              "spline identifier" },
    { STRING_ID_TOKEN,              "string identifier" },
    { TEMPORARY_MACRO_ID_TOKEN,     "unfinished macro declaration" },
    { TEXTURE_ID_TOKEN,             "texture identifier" },
    { TEXTURE_MAP_ID_TOKEN,         "texture_map identifier" },
    { TRANSFORM_ID_TOKEN,           "transform identifier" },
    { UV_ID_TOKEN,                  "uv vector identifier" },
    { VECTFUNCT_ID_TOKEN,           "vector function identifier" },
    { VECTOR_4D_ID_TOKEN,           "4d-vector identifier" },
    { VECTOR_ID_TOKEN,              "vector identifier" },

    { EMPTY_ARRAY_TOKEN,            "empty array" },
    { IDENTIFIER_TOKEN,             "undeclared identifier" },

    //------------------------------------------------------------------------------
    // Miscellaneous Pseudo-Tokens.

    { DUMMY_SYMBOL_TOKEN,           "dummy symbol" },
    { END_OF_FILE_TOKEN,            "End of File" },
    { FLOAT_TOKEN,                  "float constant" },
    { STRING_LITERAL_TOKEN,         "string literal" },
    { UTF8_SIGNATURE_TOKEN,         "UTF-8 signature BOM" },

    //------------------------------------------------------------------------------
    // End of list, marked by TokenId TOKEN_COUNT_ and `nullptr` token string.

    { TOKEN_COUNT_,                 nullptr }
};

TokenId GetCategorizedTokenId(TokenId tokenId)
{
    if (tokenId <= SIGNATURE_TOKEN_CATEGORY)
        return SIGNATURE_TOKEN_CATEGORY;
    else if (tokenId <= FLOAT_TOKEN_CATEGORY)
        return FLOAT_TOKEN_CATEGORY;
    else if (tokenId <= VECTOR_TOKEN_CATEGORY)
        return VECTOR_TOKEN_CATEGORY;
    else if (tokenId <= COLOUR_TOKEN_CATEGORY)
        return COLOUR_TOKEN_CATEGORY;
    else
        return tokenId;
}

}
// end of namespace pov_parser
