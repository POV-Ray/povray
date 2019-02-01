//******************************************************************************
///
/// @file parser/reservedwords.h
///
/// This header file is included by all all language parsing C modules in
/// POV-Ray.
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

#ifndef POVRAY_PARSER_RESERVEDWORDS_H
#define POVRAY_PARSER_RESERVEDWORDS_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "parser/configparser.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
// POV-Ray header files (core module)
//  (none at the moment)

// POV-Ray header files (parser module)
#include "parser/parsertypes.h"

namespace pov_parser
{

struct Reserved_Word_Struct final
{
    TokenId Token_Number;
    const char *Token_Name;
};
using RESERVED_WORD = Reserved_Word_Struct; ///< @deprecated

// Token Definitions for Parser
enum TokenId : int
{
    //------------------------------------------------------------------------------
    // Signature Tokens.
    //
    // All tokens that indicate a file format must go here.
    //
    // Please keep this section neatly sorted by the token identifier name,
    // sorting underscore characters before digits, digits before letters,
    // and short names before long ones, but _ignoring_ the trailing `_TOKEN` or
    // `_ID_TOKEN`.

    UTF8_SIGNATURE_TOKEN,

    SIGNATURE_TOKEN_CATEGORY, // must be last in this section

    //------------------------------------------------------------------------------
    // Float Tokens.
    //
    // All keyword tokens that may start a float expression must go here.
    //
    // Please keep this section neatly sorted by the token identifier name,
    // sorting underscore characters before digits, digits before letters,
    // and short names before long ones, but _ignoring_ the trailing `_TOKEN` or
    // `_ID_TOKEN`.

    ABS_TOKEN,
    ACOS_TOKEN,
    ACOSH_TOKEN,
    ASC_TOKEN,
    ASIN_TOKEN,
    ASINH_TOKEN,
    ATAN_TOKEN,
    ATAN2_TOKEN,
    ATANH_TOKEN,
    BITWISE_AND_TOKEN,
    BITWISE_OR_TOKEN,
    BITWISE_XOR_TOKEN,
    CEIL_TOKEN,
    CLOCK_TOKEN,
    CLOCK_ON_TOKEN,
    COS_TOKEN,
    COSH_TOKEN,
    DEFINED_TOKEN,
    DEGREES_TOKEN,
    DIMENSION_SIZE_TOKEN,
    DIMENSIONS_TOKEN,
    DIV_TOKEN,
    EXP_TOKEN,
    FALSE_TOKEN,
    FILE_EXISTS_TOKEN,
    FLOAT_ID_TOKEN,
    FLOAT_TOKEN,
    FLOOR_TOKEN,
    INSIDE_TOKEN,
    INT_TOKEN,
    LN_TOKEN,
    LOG_TOKEN,
    MAX_TOKEN,
    MIN_TOKEN,
    MOD_TOKEN,
    NO_TOKEN,
    NOW_TOKEN,
    OFF_TOKEN,
    ON_TOKEN,
    PI_TOKEN,
    POW_TOKEN,
    PROD_TOKEN,
    RADIANS_TOKEN,
    RAND_TOKEN,
    SEED_TOKEN,
    SELECT_TOKEN,
    SIN_TOKEN,
    SINH_TOKEN,
    SQRT_TOKEN,
    STRCMP_TOKEN,
    STRLEN_TOKEN,
    SUM_TOKEN,
    TAN_TOKEN,
    TANH_TOKEN,
    TAU_TOKEN,
    TRUE_TOKEN,
    VAL_TOKEN,
    VDOT_TOKEN,
    VERSION_TOKEN,
    VLENGTH_TOKEN,
    YES_TOKEN,

    FLOAT_TOKEN_CATEGORY, // must be last in this section

    //------------------------------------------------------------------------------
    // Vector Tokens.
    //
    // All keyword tokens that may start a vector expression must go here.
    //
    // Please keep this section neatly sorted by the token identifier name,
    // sorting underscore characters before digits, digits before letters,
    // and short names before long ones, but _ignoring_ the trailing `_TOKEN` or
    // `_ID_TOKEN`.

    MAX_EXTENT_TOKEN,
    MIN_EXTENT_TOKEN,
    TRACE_TOKEN,
    VAXIS_ROTATE_TOKEN,
    VCROSS_TOKEN,
    VECTOR_ID_TOKEN,
    VNORMALIZE_TOKEN,
    VROTATE_TOKEN,
    VTURBULENCE_TOKEN,
    X_TOKEN,
    Y_TOKEN,
    Z_TOKEN,

    VECTOR_TOKEN_CATEGORY, // must be last in this section

    //------------------------------------------------------------------------------
    // Colour Tokens.
    //
    // Keyword tokens that may start a colour expression must typically go here.
    //
    // Please keep this section neatly sorted by the token identifier name,
    // sorting underscore characters before digits, digits before letters,
    // and short names before long ones, but _ignoring_ the trailing `_TOKEN` or
    // `_ID_TOKEN`.

    ALPHA_TOKEN,
    BLUE_TOKEN,
    FILTER_TOKEN,
    GRAY_TOKEN,
    GREEN_TOKEN,
    RED_TOKEN,
    RGB_TOKEN,
    RGBF_TOKEN,
    RGBFT_TOKEN,
    RGBT_TOKEN,
#if 0 // sred, sgreen and sblue tokens not enabled at present
    SBLUE_TOKEN,
    SGREEN_TOKEN,
    SRED_TOKEN,
#endif
    SRGB_TOKEN,
    SRGBF_TOKEN,
    SRGBFT_TOKEN,
    SRGBT_TOKEN,
    TRANSMIT_TOKEN,

    COLOUR_TOKEN_CATEGORY, // must be last in this section

    //------------------------------------------------------------------------------
    // More Colour Tokens.
    //
    // These tokens are exempt from the previous section for one reason or another.

    COLOUR_ID_TOKEN,
    COLOUR_TOKEN,

    //------------------------------------------------------------------------------
    // Miscellaneous Tokens.
    //
    // Please keep this section neatly sorted by the token identifier name,
    // sorting underscore characters before digits, digits before letters,
    // and short names before long ones, but _ignoring_ the trailing `_TOKEN` or
    // `_ID_TOKEN`. Sort `_TOKEN` before `_ID_TOKEN`.

    AA_LEVEL_TOKEN,
    AA_THRESHOLD_TOKEN,
    ABSORPTION_TOKEN,
    ACCURACY_TOKEN,
    ADAPTIVE_TOKEN,
    ADC_BAILOUT_TOKEN,
    AGATE_TOKEN,
    AGATE_TURB_TOKEN,
    ALBEDO_TOKEN,
    ALL_TOKEN,
    ALL_INTERSECTIONS_TOKEN,
    ALTITUDE_TOKEN,
    ALWAYS_SAMPLE_TOKEN,
    AMBIENT_TOKEN,
    AMBIENT_LIGHT_TOKEN,
    AMPERSAND_TOKEN,
    ANGLE_TOKEN,
    ANISOTROPY_TOKEN,
    AOI_TOKEN,
    APERTURE_TOKEN,
    APPEND_TOKEN,
    ARC_ANGLE_TOKEN,
    AREA_ILLUMINATION_TOKEN,
    AREA_LIGHT_TOKEN,
    ARRAY_TOKEN,
    ARRAY_ID_TOKEN,
    ASCII_TOKEN,
    ASSUMED_GAMMA_TOKEN,
    AT_TOKEN,
    AUTOSTOP_TOKEN,
    AVERAGE_TOKEN,

    B_SPLINE_TOKEN,
    BACK_QUOTE_TOKEN,
    BACK_SLASH_TOKEN,
    BACKGROUND_TOKEN,
    BAR_TOKEN,
    BEZIER_SPLINE_TOKEN,
    BICUBIC_PATCH_TOKEN,
    BLACK_HOLE_TOKEN,
    BLEND_GAMMA_TOKEN,
    BLEND_MODE_TOKEN,
    BLOB_TOKEN,
    BLUR_SAMPLES_TOKEN,
    BMP_TOKEN,
    BOKEH_TOKEN,
    BOUNDED_BY_TOKEN,
    BOX_TOKEN,
    BOXED_TOKEN,
    BOZO_TOKEN,
    BREAK_TOKEN,
#if POV_DEBUG
    BREAKPOINT_TOKEN,
#endif
    BRICK_TOKEN,
    BRICK_SIZE_TOKEN,
    BRIGHTNESS_TOKEN,
    BRILLIANCE_TOKEN,
    BT2020_TOKEN,
    BT709_TOKEN,
    BUMP_MAP_TOKEN,
    BUMP_SIZE_TOKEN,
    BUMPS_TOKEN,

    CAMERA_TOKEN,
    CAMERA_ID_TOKEN,
    CASE_TOKEN,
    CAUSTICS_TOKEN,
    CELLS_TOKEN,
    CHARSET_TOKEN,
    CHECKER_TOKEN,
    CHR_TOKEN,
    CIRCULAR_TOKEN,
    CLIPPED_BY_TOKEN,
    CMAP_TOKEN,
    COLLECT_TOKEN,
    COLON_TOKEN,
    COLOUR_MAP_TOKEN,
    COLOUR_MAP_ID_TOKEN,
    COMMA_TOKEN,
    COMPONENT_TOKEN,
    COMPOSITE_TOKEN,
    CONCAT_TOKEN,
    CONE_TOKEN,
    CONFIDENCE_TOKEN,
    CONIC_SWEEP_TOKEN,
    CONSERVE_ENERGY_TOKEN,
    CONTAINED_BY_TOKEN,
    CONTROL0_TOKEN,
    CONTROL1_TOKEN,
    COUNT_TOKEN,
    COORDS_TOKEN,
    CRACKLE_TOKEN,
    CRAND_TOKEN,
    CUBE_TOKEN,
    CUBIC_TOKEN,
    CUBIC_SPLINE_TOKEN,
    CUBIC_WAVE_TOKEN,
    CUTAWAY_TEXTURES_TOKEN,
    CYLINDER_TOKEN,
    CYLINDRICAL_TOKEN,

    DASH_TOKEN,
    DATETIME_TOKEN,
    DEBUG_TOKEN,
    DEBUG_TAG_TOKEN,
    DECLARE_TOKEN,
    DEFAULT_TOKEN,
    DENSITY_TOKEN,
    DENSITY_ID_TOKEN,
    DENSITY_FILE_TOKEN,
    DENSITY_MAP_TOKEN,
    DENSITY_MAP_ID_TOKEN,
    DENTS_TOKEN,
    DEPRECATED_TOKEN,
    DF3_TOKEN,
    DICTIONARY_TOKEN,
    DICTIONARY_ID_TOKEN,
    DIFFERENCE_TOKEN,
    DIFFUSE_TOKEN,
    DIRECTION_TOKEN,
    DISC_TOKEN,
    DISPERSION_TOKEN,
    DISPERSION_SAMPLES_TOKEN,
    DIST_EXP_TOKEN,
    DISTANCE_TOKEN,
    DOLLAR_TOKEN,
    DOUBLE_ILLUMINATE_TOKEN,
    DUMMY_SYMBOL_TOKEN,

    ECCENTRICITY_TOKEN,
    ELSE_TOKEN,
    ELSEIF_TOKEN,
    EMISSION_TOKEN,
    EMPTY_ARRAY_TOKEN,
    END_TOKEN,
    END_OF_FILE_TOKEN,
    EQUALS_TOKEN,
    ERROR_TOKEN,
    ERROR_BOUND_TOKEN,
    EVALUATE_TOKEN,
    EXCLAMATION_TOKEN,
    EXPAND_THRESHOLDS_TOKEN,
    EXPONENT_TOKEN,
    EXR_TOKEN,
    EXTERIOR_TOKEN,
    EXTINCTION_TOKEN,

    FACE_INDICES_TOKEN,
    FACETS_TOKEN,
    FADE_COLOUR_TOKEN,
    FADE_DISTANCE_TOKEN,
    FADE_POWER_TOKEN,
    FALLOFF_TOKEN,
    FALLOFF_ANGLE_TOKEN,
    FCLOSE_TOKEN,
    FILE_ID_TOKEN,
    FINISH_TOKEN,
    FINISH_ID_TOKEN,
    FISHEYE_TOKEN,
    FLATNESS_TOKEN,
    FLIP_TOKEN,
    FOCAL_POINT_TOKEN,
    FOG_TOKEN,
    FOG_ID_TOKEN,
    FOG_ALT_TOKEN,
    FOG_OFFSET_TOKEN,
    FOG_TYPE_TOKEN,
    FOPEN_TOKEN,
    FOR_TOKEN,
    FORM_TOKEN,
    FREQUENCY_TOKEN,
    FRESNEL_TOKEN,
    FUNCT_ID_TOKEN,
    FUNCTION_TOKEN,

    GAMMA_TOKEN,
    GATHER_TOKEN,
    GIF_TOKEN,
    GLOBAL_TOKEN,
    GLOBAL_LIGHTS_TOKEN,
    GLOBAL_SETTINGS_TOKEN,
    GRADIENT_TOKEN,
    GRANITE_TOKEN,
    GRAY_THRESHOLD_TOKEN,

    HASH_TOKEN,
    HAT_TOKEN,
    HDR_TOKEN,
    HEIGHT_FIELD_TOKEN,
    HEXAGON_TOKEN,
    HF_GRAY_16_TOKEN,
    HIERARCHY_TOKEN,
    HOLLOW_TOKEN,
    HYPERCOMPLEX_TOKEN,

    IDENTIFIER_TOKEN,
    IF_TOKEN,
    IFDEF_TOKEN,
    IFF_TOKEN,
    IFNDEF_TOKEN,
    IMAGE_MAP_TOKEN,
    IMAGE_PATTERN_TOKEN,
    IMPORTANCE_TOKEN,
    INCLUDE_TOKEN,
    INSIDE_VECTOR_TOKEN,
    INTERIOR_TOKEN,
    INTERIOR_ID_TOKEN,
    INTERIOR_TEXTURE_TOKEN,
    INTERNAL_TOKEN,
    INTERPOLATE_TOKEN,
    INTERSECTION_TOKEN,
    INTERVALS_TOKEN,
    INVERSE_TOKEN,
    IOR_TOKEN,
    IRID_TOKEN,
    IRID_WAVELENGTH_TOKEN,
    ISOSURFACE_TOKEN,

    JITTER_TOKEN,
    JPEG_TOKEN,
    JULIA_TOKEN,
    JULIA_FRACTAL_TOKEN,

    LAMBDA_TOKEN,
    LATHE_TOKEN,
    LEFT_ANGLE_TOKEN,
    LEFT_CURLY_TOKEN,
    LEFT_PAREN_TOKEN,
    LEFT_SQUARE_TOKEN,
    LEMON_TOKEN,
    LEOPARD_TOKEN,
    LIGHT_GROUP_TOKEN,
    LIGHT_SOURCE_TOKEN,
    LINEAR_SPLINE_TOKEN,
    LINEAR_SWEEP_TOKEN,
    LOAD_FILE_TOKEN,
    LOCAL_TOKEN,
    LOCATION_TOKEN,
    LOOK_AT_TOKEN,
    LOOKS_LIKE_TOKEN,
    LOW_ERROR_FACTOR_TOKEN,

    MACRO_TOKEN,
    MACRO_ID_TOKEN,
    MAGNET_TOKEN,
    MAJOR_RADIUS_TOKEN,
    MANDEL_TOKEN,
    MAP_TYPE_TOKEN,
    MARBLE_TOKEN,
    MATERIAL_TOKEN,
    MATERIAL_ID_TOKEN,
    MATERIAL_MAP_TOKEN,
    MATRIX_TOKEN,
    MAX_GRADIENT_TOKEN,
    MAX_INTERSECTIONS_TOKEN,
    MAX_ITERATION_TOKEN,
    MAX_SAMPLE_TOKEN,
    MAX_TRACE_TOKEN,
    MAX_TRACE_LEVEL_TOKEN,
    MAXIMUM_REUSE_TOKEN,
    MEDIA_TOKEN,
    MEDIA_ID_TOKEN,
    MEDIA_ATTENUATION_TOKEN,
    MEDIA_INTERACTION_TOKEN,
    MERGE_TOKEN,
    MESH_TOKEN,
    MESH_CAMERA_TOKEN,
    MESH2_TOKEN,
    METALLIC_TOKEN,
    METHOD_TOKEN,
    METRIC_TOKEN,
    MINIMUM_REUSE_TOKEN,
    MIXED_TOKEN,
    MM_PER_UNIT_TOKEN,
    MORTAR_TOKEN,

    NATURAL_SPLINE_TOKEN,
    NEAREST_COUNT_TOKEN,
    NO_BUMP_SCALE_TOKEN,
    NO_IMAGE_TOKEN,
    NO_RADIOSITY_TOKEN,
    NO_REFLECTION_TOKEN,
    NO_SHADOW_TOKEN,
    NOISE_GENERATOR_TOKEN,
    NORMAL_TOKEN,
    NORMAL_ID_TOKEN,
    NORMAL_INDICES_TOKEN,
    NORMAL_MAP_TOKEN,
    NORMAL_MAP_ID_TOKEN,
    NORMAL_VECTORS_TOKEN,
    NUMBER_OF_SIDES_TOKEN,
    NUMBER_OF_TILES_TOKEN,
    NUMBER_OF_WAVES_TOKEN,

#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    OBJ_TOKEN,
#endif
    OBJECT_TOKEN,
    OBJECT_ID_TOKEN,
    OCTAVES_TOKEN,
    OFFSET_TOKEN,
    OMEGA_TOKEN,
    OMNIMAX_TOKEN,
    ONCE_TOKEN,
    ONION_TOKEN,
    OPEN_TOKEN,
    OPTIONAL_TOKEN,
    ORIENT_TOKEN,
    ORIENTATION_TOKEN,
    ORTHOGRAPHIC_TOKEN,
    OVUS_TOKEN,

    PANORAMIC_TOKEN,
    PARALLEL_TOKEN,
    PARAMETER_ID_TOKEN,
    PARAMETRIC_TOKEN,
    PASS_THROUGH_TOKEN,
    PATTERN_TOKEN,
    PAVEMENT_TOKEN,
    PERCENT_TOKEN,
    PERIOD_TOKEN,
    PERSPECTIVE_TOKEN,
    PGM_TOKEN,
    PHASE_TOKEN,
    PHONG_TOKEN,
    PHONG_SIZE_TOKEN,
    PHOTONS_TOKEN,
    PIGMENT_TOKEN,
    PIGMENT_ID_TOKEN,
    PIGMENT_MAP_TOKEN,
    PIGMENT_MAP_ID_TOKEN,
    PIGMENT_PATTERN_TOKEN,
    PLANAR_TOKEN,
    PLANE_TOKEN,
    PLUS_TOKEN,
    PNG_TOKEN,
    POINT_AT_TOKEN,
    POLARITY_TOKEN,
    POLY_TOKEN,
    POLY_WAVE_TOKEN,
    POLYGON_TOKEN,
    POLYNOMIAL_TOKEN,
    POT_TOKEN,
    POTENTIAL_TOKEN,
    PPM_TOKEN,
    PRECISION_TOKEN,
    PRECOMPUTE_TOKEN,
#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    PREFIX_TOKEN,
#endif
    PREMULTIPLIED_TOKEN,
    PRETRACE_END_TOKEN,
    PRETRACE_START_TOKEN,
    PRISM_TOKEN,
    PROJECTED_THROUGH_TOKEN,
    PWR_TOKEN,

    QUADRIC_TOKEN,
    QUADRATIC_SPLINE_TOKEN,
    QUARTIC_TOKEN,
    QUATERNION_TOKEN,
    QUESTION_TOKEN,
    QUICK_COLOUR_TOKEN,
    QUILTED_TOKEN,

    RADIAL_TOKEN,
    RADIOSITY_TOKEN,
    RADIUS_TOKEN,
    RAINBOW_TOKEN,
    RAINBOW_ID_TOKEN,
    RAMP_WAVE_TOKEN,
    RANGE_TOKEN,
    RATIO_TOKEN,
    READ_TOKEN,
    RECIPROCAL_TOKEN,
    RECURSION_LIMIT_TOKEN,
    REFLECTION_TOKEN,
    REFLECTION_EXPONENT_TOKEN,
    REFRACTION_TOKEN,
    REL_GE_TOKEN,
    REL_LE_TOKEN,
    REL_NE_TOKEN,
    RENDER_TOKEN,
    REPEAT_TOKEN,
    RIGHT_TOKEN,
    RIGHT_ANGLE_TOKEN,
    RIGHT_CURLY_TOKEN,
    RIGHT_PAREN_TOKEN,
    RIGHT_SQUARE_TOKEN,
    RIPPLES_TOKEN,
    ROTATE_TOKEN,
    ROUGHNESS_TOKEN,

    SAMPLES_TOKEN,
    SAVE_FILE_TOKEN,
    SCALE_TOKEN,
    SCALLOP_WAVE_TOKEN,
    SCATTERING_TOKEN,
    SEMI_COLON_TOKEN,
    SHADOWLESS_TOKEN,
    SINE_WAVE_TOKEN,
    SINGLE_QUOTE_TOKEN,
    SINT16BE_TOKEN,
    SINT16LE_TOKEN,
    SINT32BE_TOKEN,
    SINT32LE_TOKEN,
    SINT8_TOKEN,
    SIZE_TOKEN,
    SKY_TOKEN,
    SKYSPHERE_TOKEN,
    SKYSPHERE_ID_TOKEN,
    SLASH_TOKEN,
    SLICE_TOKEN,
    SLOPE_TOKEN,
    SLOPE_MAP_TOKEN,
    SLOPE_MAP_ID_TOKEN,
    SMOOTH_TOKEN,
    SMOOTH_TRIANGLE_TOKEN,
    SOLID_TOKEN,
    SOR_TOKEN,
    SPACING_TOKEN,
    SPECULAR_TOKEN,
    SPHERE_TOKEN,
    SPHERE_SWEEP_TOKEN,
    SPHERICAL_TOKEN,
    SPIRAL1_TOKEN,
    SPIRAL2_TOKEN,
    SPLINE_TOKEN,
    SPLINE_ID_TOKEN,
    SPLIT_UNION_TOKEN,
    SPOTLIGHT_TOKEN,
    SPOTTED_TOKEN,
    SQR_TOKEN,
    SQUARE_TOKEN,
    STAR_TOKEN,
    STATISTICS_TOKEN,
    STR_TOKEN,
    STRENGTH_TOKEN,
    STRING_ID_TOKEN,
    STRING_LITERAL_TOKEN,
    STRLWR_TOKEN,
    STRUPR_TOKEN,
    STURM_TOKEN,
    SUBSTR_TOKEN,
    SUBSURFACE_TOKEN,
#if POV_PARSER_EXPERIMENTAL_OBJ_IMPORT
    SUFFIX_TOKEN,
#endif
    SUPERELLIPSOID_TOKEN,
    SWITCH_TOKEN,
    SYS_TOKEN,

    T_TOKEN,
    TARGET_TOKEN,
    TEMPORARY_MACRO_ID_TOKEN,
    TEXT_TOKEN,
    TEXTURE_TOKEN,
    TEXTURE_ID_TOKEN,
    TEXTURE_LIST_TOKEN,
    TEXTURE_MAP_TOKEN,
    TEXTURE_MAP_ID_TOKEN,
    TGA_TOKEN,
    THICKNESS_TOKEN,
    THRESHOLD_TOKEN,
    TIFF_TOKEN,
    TIGHTNESS_TOKEN,
    TILDE_TOKEN,
    TILE2_TOKEN,
    TILES_TOKEN,
    TILING_TOKEN,
    TOLERANCE_TOKEN,
    TOROIDAL_TOKEN,
    TORUS_TOKEN,
    TRANSFORM_TOKEN,
    TRANSFORM_ID_TOKEN,
    TRANSLATE_TOKEN,
    TRANSLUCENCY_TOKEN,
    TRIANGLE_TOKEN,
    TRIANGLE_WAVE_TOKEN,
    TRIANGULAR_TOKEN,
    TTF_TOKEN,
    TURB_DEPTH_TOKEN,
    TURBULENCE_TOKEN,
    TYPE_TOKEN,

    U_TOKEN,
    U_STEPS_TOKEN,
    UINT16BE_TOKEN,
    UINT16LE_TOKEN,
    UINT8_TOKEN,
    ULTRA_WIDE_ANGLE_TOKEN,
    UNDEF_TOKEN,
    UNION_TOKEN,
    UNOFFICIAL_TOKEN,
    UP_TOKEN,
    USE_ALPHA_TOKEN,
    USE_COLOUR_TOKEN,
    USE_INDEX_TOKEN,
    USER_DEFINED_TOKEN,
    UTF8_TOKEN,
    UV_ID_TOKEN,
    UV_INDICES_TOKEN,
    UV_MAPPING_TOKEN,
    UV_VECTORS_TOKEN,

    V_TOKEN,
    V_STEPS_TOKEN,
    VARIANCE_TOKEN,
    VECTFUNCT_ID_TOKEN,
    VECTOR_4D_ID_TOKEN,
    VERTEX_VECTORS_TOKEN,
    VSTR_TOKEN,

    WARNING_TOKEN,
    WARP_TOKEN,
    WATER_LEVEL_TOKEN,
    WAVES_TOKEN,
    WHILE_TOKEN,
    WIDTH_TOKEN,
    WOOD_TOKEN,
    WRINKLES_TOKEN,
    WRITE_TOKEN,

    XYZ_TOKEN,

    //------------------------------------------------------------------------------
    // End of list.

    TOKEN_COUNT_, // Pseudo-Token to count the number of token identifiers.
    NOT_A_TOKEN = -1 // Pseudo-Token used to invalidate a token identifier variable.
};

constexpr int TOKEN_COUNT = int(TokenId::TOKEN_COUNT_);

extern const RESERVED_WORD Reserved_Words[];

TokenId GetCategorizedTokenId(TokenId tokenId);

}
// end of namespace pov_parser

#endif // POVRAY_PARSER_RESERVEDWORDS_H
