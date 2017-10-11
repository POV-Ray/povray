/****************************************************************************
*                tokenize.c
*
*  This module implements the first part of a two part parser for the scene
*  description files.  This phase changes the input file into tokens.
*
*  This module tokenizes the input file and sends the tokens created
*  to the parser (the second stage).  Tokens sent to the parser contain a
*  token ID, the line number of the token, and if necessary, some data for
*  the token.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
* Modifications by Hans-Deltev Fink, January 1999, used with permission
* Modifications by Thomas Willhalm, March 1999, used with permission
*
*****************************************************************************/

#include <ctype.h>
#include "frame.h"
#include "povray.h"
#include "povproto.h"
#include "parse.h"
#include "povray.h"
#include "colour.h"
#include "render.h"
#include "texture.h"
#include "tokenize.h"
#include "express.h"
#include "matrices.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/

#define MAX_INCLUDE_FILES 10

#define CALL(x) { if (!(x)) return (FALSE); }

#define COND_STACK_SIZE 200

typedef enum cond_type
{
  ROOT_COND=0,
  WHILE_COND,
  IF_TRUE_COND,
  IF_FALSE_COND,
  ELSE_COND,
  SWITCH_COND,
  CASE_TRUE_COND,
  CASE_FALSE_COND,
  SKIP_TIL_END_COND,
  INVOKING_MACRO_COND,
  DECLARING_MACRO_COND
} COND_TYPE;


#define SYM_TABLE_SIZE 257
#define MAX_NUMBER_OF_TABLES 100


/*****************************************************************************
* Local variables
******************************************************************************/

typedef struct Sym_Table_Struct SYM_TABLE;

struct Sym_Table_Struct
{
  char *Table_Name;
  SYM_ENTRY *Table[SYM_TABLE_SIZE];
};

SYM_TABLE *Tables[MAX_NUMBER_OF_TABLES];

int Table_Index;

static int String_Index;
char String[MAX_STRING_INDEX];
char String2[MAX_STRING_INDEX];

/* moved here to allow reinitialization */

int token_count = 0;
static int line_count = 10;

static int Include_File_Index;
static DATA_FILE *Data_File;
static DATA_FILE Include_Files[MAX_INCLUDE_FILES];

struct Token_Struct Token;

static char **Echo_Buff;
static char  *Echo_Ptr;
static int    Echo_Indx;
static int    Echo_Line;

#define MAX_PARAMETER_LIST 20

typedef struct Cond_Stack_Entry CS_ENTRY;

struct Cond_Stack_Entry
{
  COND_TYPE Cond_Type;
  DBL Switch_Value;
  char *While_File_Name;
  char *Macro_Return_Name;
  int Macro_Same_Flag;
  POV_MACRO *PMac;
  long Pos,Line_No;
};

static CS_ENTRY *Cond_Stack;
static int CS_Index, Skipping, Got_EOF, Inside_Ifdef, Inside_MacroDef;

int input_file_in_memory = 0 ;

/*
 * Here are the reserved words.  If you need to add new words,
 * be sure to declare them in parse.h
 */

RESERVED_WORD Reserved_Words [LAST_TOKEN] = {
  {ABSORPTION_TOKEN, "absorption"},
  {ABS_TOKEN, "abs"},
  {ACOSH_TOKEN, "acosh"},
  {ACOS_TOKEN, "acos"},
  {ADAPTIVE_TOKEN, "adaptive"},
  {ADC_BAILOUT_TOKEN, "adc_bailout"},
  {AGATE_TOKEN, "agate"},
  {AGATE_TURB_TOKEN, "agate_turb"},
  {ALL_TOKEN, "all"},
  {ALPHA_TOKEN, "alpha"},
  {AMBIENT_LIGHT_TOKEN, "ambient_light"},
  {AMBIENT_TOKEN, "ambient"},
  {AMPERSAND_TOKEN, "&"},
  {ANGLE_TOKEN, "angle"},
  {APERTURE_TOKEN, "aperture"},
  {APPEND_TOKEN, "append"},
  {ARC_ANGLE_TOKEN, "arc_angle"},
  {AREA_LIGHT_TOKEN, "area_light"},
  {ARRAY_ID_TOKEN, "array identifier"},
  {ARRAY_TOKEN, "array"},
  {ASC_TOKEN, "asc"},
  {ASINH_TOKEN, "asinh"},
  {ASIN_TOKEN, "asin"},
  {ASSUMED_GAMMA_TOKEN, "assumed_gamma"},
  {ATAN2_TOKEN, "atan2"},
  {ATANH_TOKEN, "atanh"},
  {ATAN_TOKEN, "atan"},
  {AT_TOKEN, "@"},
  {AVERAGE_TOKEN, "average"},
  {BACKGROUND_TOKEN, "background"},
  {BACK_QUOTE_TOKEN, "`"},
  {BACK_SLASH_TOKEN, "\\"},
  {BAR_TOKEN, "|"},
  {BEZIER_SPLINE_TOKEN, "bezier_spline"},
  {BICUBIC_PATCH_TOKEN, "bicubic_patch"},
  {BLACK_HOLE_TOKEN, "black_hole"},
  {BLOB_TOKEN, "blob"},
  {BLUE_TOKEN, "blue"},
  {BLUR_SAMPLES_TOKEN, "blur_samples"},
  {BOUNDED_BY_TOKEN, "bounded_by"},
  {BOXED_TOKEN, "boxed"},
  {BOX_TOKEN, "box"},
  {BOZO_TOKEN, "bozo"},
  {BREAK_TOKEN, "break"},
  {BRICK_SIZE_TOKEN, "brick_size"},
  {BRICK_TOKEN, "brick"},
  {BRIGHTNESS_TOKEN, "brightness" },
  {BRILLIANCE_TOKEN, "brilliance"},
  {BUMPS_TOKEN, "bumps"},
  {BUMP_MAP_TOKEN, "bump_map"},
  {BUMP_SIZE_TOKEN, "bump_size"},
  {CAMERA_ID_TOKEN, "camera identifier"},
  {CAMERA_TOKEN, "camera"},
  {CASE_TOKEN, "case"},
  {CAUSTICS_TOKEN, "caustics"},
  {CEIL_TOKEN, "ceil"},
  {CHECKER_TOKEN, "checker"},
  {CHR_TOKEN, "chr"},
  {CLIPPED_BY_TOKEN, "clipped_by"},
  {CLOCK_DELTA_TOKEN, "clock_delta"},
  {CLOCK_TOKEN, "clock"},
  {COLON_TOKEN, ":"},
  {COLOUR_ID_TOKEN, "colour identifier"},
  {COLOUR_KEY_TOKEN, "color keyword"},
  {COLOUR_MAP_ID_TOKEN, "colour map identifier"},
  {COLOUR_MAP_TOKEN, "color_map"},
  {COLOUR_MAP_TOKEN, "colour_map"},
  {COLOUR_TOKEN, "color"},
  {COLOUR_TOKEN, "colour"},
  {COMMA_TOKEN, ", "},
  {COMPONENT_TOKEN, "component"},
  {COMPOSITE_TOKEN, "composite"},
  {CONCAT_TOKEN, "concat"},
  {CONE_TOKEN, "cone"},
  {CONFIDENCE_TOKEN, "confidence"},
  {CONIC_SWEEP_TOKEN, "conic_sweep"},
  {CONTROL0_TOKEN, "control0"},
  {CONTROL1_TOKEN, "control1"},
  {COSH_TOKEN, "cosh"},
  {COS_TOKEN, "cos"},
  {COUNT_TOKEN, "count" },
  {CRACKLE_TOKEN, "crackle"},
  {CRAND_TOKEN, "crand"},
  {CUBE_TOKEN, "cube"},
  {CUBIC_SPLINE_TOKEN, "cubic_spline"},
  {CUBIC_TOKEN, "cubic"},
  {CUBIC_WAVE_TOKEN, "cubic_wave"},
  {CYLINDER_TOKEN, "cylinder"},
  {CYLINDRICAL_TOKEN, "cylindrical"},
  {DASH_TOKEN, "-"},
  {DEBUG_TOKEN, "debug"},
  {DECLARE_TOKEN, "declare"},
  {DEFAULT_TOKEN, "default"},
  {DEFINED_TOKEN, "defined"},
  {DEGREES_TOKEN, "degrees"},
  {DENSITY_FILE_TOKEN, "density_file"},
  {DENSITY_ID_TOKEN, "density identifier"},
  {DENSITY_MAP_ID_TOKEN, "density_map identifier"},
  {DENSITY_MAP_TOKEN, "density_map"},
  {DENSITY_TOKEN, "density"},
  {DENTS_TOKEN, "dents"},
  {DF3_TOKEN, "df3"},
  {DIFFERENCE_TOKEN, "difference"},
  {DIFFUSE_TOKEN, "diffuse"},
  {DIMENSIONS_TOKEN, "dimensions"},
  {DIMENSION_SIZE_TOKEN, "dimension_size"},
  {DIRECTION_TOKEN, "direction"},
  {DISC_TOKEN, "disc"},
  {DISTANCE_MAXIMUM_TOKEN, "distance_maximum" },
  {DISTANCE_TOKEN, "distance"},
  {DIV_TOKEN, "div"},
  {DOLLAR_TOKEN, "$"},
  {ECCENTRICITY_TOKEN, "eccentricity"},
  {ELSE_TOKEN, "else"},
  {EMISSION_TOKEN, "emission"},
  {EMPTY_ARRAY_TOKEN, "empty array"},
  {END_OF_FILE_TOKEN, "End of File"},
  {END_TOKEN, "end"},
  {EQUALS_TOKEN, "="},
  {ERROR_BOUND_TOKEN, "error_bound" },
  {ERROR_TOKEN, "error"},
  {EXCLAMATION_TOKEN, "!"},
  {EXP_TOKEN, "exp"},
  {EXTINCTION_TOKEN, "extinction"},
  {FADE_DISTANCE_TOKEN, "fade_distance"},
  {FADE_POWER_TOKEN, "fade_power"},
  {FALLOFF_ANGLE_TOKEN, "falloff_angle"},
  {FALLOFF_TOKEN, "falloff"},
  {FALSE_TOKEN, "false"},
  {FCLOSE_TOKEN, "fclose"},
  {FILE_EXISTS_TOKEN, "file_exists"},
  {FILE_ID_TOKEN, "file identifier"},
  {FILL_LIGHT_TOKEN, "shadowless"},
  {FILTER_TOKEN, "filter"},
  {FINISH_ID_TOKEN, "finish identifier"},
  {FINISH_TOKEN, "finish"},
  {FISHEYE_TOKEN, "fisheye"},
  {FLATNESS_TOKEN, "flatness"},
  {FLIP_TOKEN, "flip"},
  {FLOAT_FUNCT_TOKEN, "float function"},
  {FLOAT_ID_TOKEN, "float identifier"},
  {FLOAT_TOKEN, "float constant"},
  {FLOOR_TOKEN, "floor"},
  {FOCAL_POINT_TOKEN, "focal_point"},
  {FOG_ALT_TOKEN, "fog_alt"},
  {FOG_ID_TOKEN, "fog identifier"},
  {FOG_OFFSET_TOKEN, "fog_offset"},
  {FOG_TOKEN, "fog"},
  {FOG_TYPE_TOKEN, "fog_type"},
  {FOPEN_TOKEN, "fopen"},
  {FREQUENCY_TOKEN, "frequency"},
  {GIF_TOKEN, "gif"},
  {GLOBAL_SETTINGS_TOKEN, "global_settings" },
  {GRADIENT_TOKEN, "gradient"},
  {GRANITE_TOKEN, "granite"},
  {GRAY_THRESHOLD_TOKEN, "gray_threshold" },
  {GREEN_TOKEN, "green"},
  {HASH_TOKEN, "#"},
  {HAT_TOKEN, "^"},
  {HEIGHT_FIELD_TOKEN, "height_field"},
  {HEXAGON_TOKEN, "hexagon"},
  {HF_GRAY_16_TOKEN, "hf_gray_16" },
  {HIERARCHY_TOKEN, "hierarchy"},
  {HOLLOW_TOKEN, "hollow"},
  {HYPERCOMPLEX_TOKEN, "hypercomplex"},
  {IDENTIFIER_TOKEN, "undeclared identifier"},
  {IFDEF_TOKEN, "ifdef"},
  {IFF_TOKEN, "iff"},
  {IFNDEF_TOKEN, "ifndef"},
  {IF_TOKEN, "if"},
  {IMAGE_MAP_TOKEN, "image_map"},
  {INCLUDE_TOKEN, "include"},
  {INTERIOR_ID_TOKEN, "interior identifier"},
  {INTERIOR_TOKEN, "interior"},
  {INTERPOLATE_TOKEN, "interpolate"},
  {INTERSECTION_TOKEN, "intersection"},
  {INTERVALS_TOKEN, "intervals"},
  {INT_TOKEN, "int"},
  {INVERSE_TOKEN, "inverse"},
  {IOR_TOKEN, "ior"},
  {IRID_TOKEN, "irid"},
  {IRID_WAVELENGTH_TOKEN, "irid_wavelength"},
  {JITTER_TOKEN, "jitter"},
  {JULIA_FRACTAL_TOKEN, "julia_fractal"},
  {LAMBDA_TOKEN, "lambda"},
  {LATHE_TOKEN, "lathe"},
  {LEFT_ANGLE_TOKEN, "<"},
  {LEFT_CURLY_TOKEN, "{"},
  {LEFT_PAREN_TOKEN, "("},
  {LEFT_SQUARE_TOKEN, "["},
  {LEOPARD_TOKEN, "leopard"},
  {LIGHT_SOURCE_TOKEN, "light_source"},
  {LINEAR_SPLINE_TOKEN, "linear_spline"},
  {LINEAR_SWEEP_TOKEN, "linear_sweep"},
  {LOCAL_TOKEN, "local"},
  {LOCATION_TOKEN, "location"},
  {LOG_TOKEN, "log"},
  {LOOKS_LIKE_TOKEN, "looks_like"},
  {LOOK_AT_TOKEN, "look_at"},
  {LOW_ERROR_FACTOR_TOKEN, "low_error_factor" },
  {MACRO_ID_TOKEN, "macro identifier"},
  {MACRO_TOKEN, "macro"},
  {MANDEL_TOKEN, "mandel"},
  {MAP_TYPE_TOKEN, "map_type"},
  {MARBLE_TOKEN, "marble"},
  {MATERIAL_ID_TOKEN, "material identifier"},
  {MATERIAL_MAP_TOKEN, "material_map"},
  {MATERIAL_TOKEN, "material"},
  {MATRIX_TOKEN, "matrix"},
  {MAX_INTERSECTIONS, "max_intersections"},
  {MAX_ITERATION_TOKEN, "max_iteration"},
  {MAX_TOKEN, "max"},
  {MAX_TRACE_LEVEL_TOKEN, "max_trace_level"},
  {MEDIA_ATTENUATION_TOKEN, "media_attenuation"},
  {MEDIA_ID_TOKEN, "media identifier"},
  {MEDIA_INTERACTION_TOKEN, "media_interaction"},
  {MEDIA_TOKEN, "media"},
  {MERGE_TOKEN, "merge"},
  {MESH_TOKEN, "mesh"},
  {METALLIC_TOKEN, "metallic"},
  {MINIMUM_REUSE_TOKEN, "minimum_reuse" },
  {MIN_TOKEN, "min"},
  {MOD_TOKEN, "mod"},
  {MORTAR_TOKEN, "mortar"},
  {NEAREST_COUNT_TOKEN, "nearest_count" },
  {NORMAL_MAP_ID_TOKEN, "normal_map identifier"},
  {NORMAL_MAP_TOKEN, "normal_map"},
  {NO_SHADOW_TOKEN, "no_shadow"},
  {NO_TOKEN, "no"},
  {NUMBER_OF_WAVES_TOKEN, "number_of_waves"},
  {OBJECT_ID_TOKEN, "object identifier"},
  {OBJECT_TOKEN, "object"},
  {OCTAVES_TOKEN, "octaves"},
  {OFFSET_TOKEN, "offset"},
  {OFF_TOKEN, "off"},
  {OMEGA_TOKEN, "omega"},
  {OMNIMAX_TOKEN, "omnimax"},
  {ONCE_TOKEN, "once"},
  {ONION_TOKEN, "onion"},
  {ON_TOKEN, "on"},
  {OPEN_TOKEN, "open"},
  {ORTHOGRAPHIC_TOKEN, "orthographic"},
  {PANORAMIC_TOKEN, "panoramic"},
  {PARAMETER_ID_TOKEN, "parameter identifier"},
  {PERCENT_TOKEN, "%"},
  {PERIOD_TOKEN, ". (period)"},
  {PERSPECTIVE_TOKEN, "perspective"},
  {PGM_TOKEN, "pgm"},
  {PHASE_TOKEN, "phase"},
  {PHONG_SIZE_TOKEN, "phong_size"},
  {PHONG_TOKEN, "phong"},
  {PIGMENT_ID_TOKEN, "pigment identifier"},
  {PIGMENT_MAP_ID_TOKEN, "pigment_map identifier"},
  {PIGMENT_MAP_TOKEN, "pigment_map"},
  {PIGMENT_TOKEN, "pigment"},
  {PI_TOKEN, "pi"},
  {PLANAR_TOKEN, "planar"},
  {PLANE_TOKEN, "plane"},
  {PLUS_TOKEN, "+"},
  {PNG_TOKEN, "png"},
  {POINT_AT_TOKEN, "point_at"},
  {POLYGON_TOKEN, "polygon"},
  {POLY_TOKEN, "poly"},
  {POLY_WAVE_TOKEN, "poly_wave"},
  {POT_TOKEN, "pot"},
  {POW_TOKEN, "pow"},
  {PPM_TOKEN, "ppm"},
  {PRECISION_TOKEN, "precision"},
  {PRISM_TOKEN, "prism"},
  {PWR_TOKEN, "pwr"},
  {QUADRATIC_SPLINE_TOKEN, "quadratic_spline"},
  {QUADRIC_TOKEN, "quadric"},
  {QUARTIC_TOKEN, "quartic"},
  {QUATERNION_TOKEN, "quaternion"},
  {QUESTION_TOKEN, "?"},
  {QUICK_COLOUR_TOKEN, "quick_color"},
  {QUICK_COLOUR_TOKEN, "quick_colour"},
  {QUILTED_TOKEN, "quilted"},
  {RADIAL_TOKEN, "radial"},
  {RADIANS_TOKEN, "radians"},
  {RADIOSITY_TOKEN, "radiosity" },
  {RADIUS_TOKEN, "radius"},
  {RAINBOW_ID_TOKEN, "rainbow identifier"},
  {RAINBOW_TOKEN, "rainbow"},
  {RAMP_WAVE_TOKEN, "ramp_wave"},
  {RAND_TOKEN, "rand"},
  {RANGE_TOKEN, "range"},
  {RATIO_TOKEN, "ratio"},
  {READ_TOKEN, "read"},
  {RECIPROCAL_TOKEN, "reciprocal" },
  {RECURSION_LIMIT_TOKEN, "recursion_limit" },
  {RED_TOKEN, "red"},
  {REFLECTION_TOKEN, "reflection"},
  {REFLECTION_EXPONENT_TOKEN, "reflection_exponent"},
  {REFRACTION_TOKEN, "refraction"},
  {REL_GE_TOKEN, ">="},
  {REL_LE_TOKEN, "<="},
  {REL_NE_TOKEN, "!="},
  {RENDER_TOKEN, "render"},
  {REPEAT_TOKEN, "repeat"},
  {RGBFT_TOKEN, "rgbft"},
  {RGBF_TOKEN, "rgbf"},
  {RGBT_TOKEN, "rgbt"},
  {RGB_TOKEN, "rgb"},
  {RIGHT_ANGLE_TOKEN, ">"},
  {RIGHT_CURLY_TOKEN, "}"},
  {RIGHT_PAREN_TOKEN, ")"},
  {RIGHT_SQUARE_TOKEN, "]"},
  {RIGHT_TOKEN, "right"},
  {RIPPLES_TOKEN, "ripples"},
  {ROTATE_TOKEN, "rotate"},
  {ROUGHNESS_TOKEN, "roughness"},
  {SAMPLES_TOKEN, "samples"},
  {SCALE_TOKEN, "scale"},
  {SCALLOP_WAVE_TOKEN, "scallop_wave"},
  {SCATTERING_TOKEN, "scattering"},
  {SEED_TOKEN, "seed"},
  {SEMI_COLON_TOKEN, ";"},
  {SINE_WAVE_TOKEN, "sine_wave"},
  {SINGLE_QUOTE_TOKEN, "'"},
  {SINH_TOKEN, "sinh"},
  {SIN_TOKEN, "sin"},
  {SKYSPHERE_ID_TOKEN, "sky_sphere identifier"},
  {SKYSPHERE_TOKEN, "sky_sphere"},
  {SKY_TOKEN, "sky"},
  {SLASH_TOKEN, "/"},
  {SLICE_TOKEN, "slice"},
  {SLOPE_MAP_ID_TOKEN, "slope_map identifier"},
  {SLOPE_MAP_TOKEN, "slope_map"},
  {SMOOTH_TOKEN, "smooth"},
  {SMOOTH_TRIANGLE_TOKEN, "smooth_triangle"},
  {SOR_TOKEN, "sor"},
  {SPECULAR_TOKEN, "specular"},
  {SPHERE_TOKEN, "sphere"},
  {SPHERICAL_TOKEN, "spherical"},
  {SPIRAL1_TOKEN, "spiral1"},
  {SPIRAL2_TOKEN, "spiral2"},
  {SPOTLIGHT_TOKEN, "spotlight"},
  {SPOTTED_TOKEN, "spotted"},
  {SQRT_TOKEN, "sqrt"},
  {SQR_TOKEN, "sqr"},
  {STAR_TOKEN, "*"},
  {STATISTICS_TOKEN, "statistics"},
  {STRCMP_TOKEN, "strcmp"},
  {STRENGTH_TOKEN, "strength"},
  {STRING_ID_TOKEN, "string identifier"},
  {STRING_LITERAL_TOKEN, "string literal"},
  {STRLEN_TOKEN, "strlen"},
  {STRLWR_TOKEN, "strlwr"},
  {STRUPR_TOKEN, "strupr"},
  {STR_TOKEN, "str"},
  {STURM_TOKEN, "sturm"},
  {SUBSTR_TOKEN, "substr"},
  {SUPERELLIPSOID_TOKEN, "superellipsoid"},
  {SWITCH_TOKEN, "switch"},
  {SYS_TOKEN, "sys"},
  {TANH_TOKEN, "tanh"},
  {TAN_TOKEN, "tan"},
  {TEXTURE_ID_TOKEN, "texture identifier"},
  {TEXTURE_MAP_ID_TOKEN, "texture_map identifier"},
  {TEXTURE_MAP_TOKEN, "texture_map"},
  {TEXTURE_TOKEN, "texture"},
  {TEXT_TOKEN, "text"},
  {TGA_TOKEN, "tga"},
  {THICKNESS_TOKEN, "thickness"},
  {THRESHOLD_TOKEN, "threshold"},
  {TIGHTNESS_TOKEN, "tightness"},
  {TILDE_TOKEN, "~"},
  {TILE2_TOKEN, "tile2"},
  {TILES_TOKEN, "tiles"},
  {TNORMAL_ID_TOKEN, "normal identifier"},
  {TNORMAL_TOKEN, "normal"},
  {TORUS_TOKEN, "torus"},
  {TRACK_TOKEN, "track"},
  {TRANSFORM_ID_TOKEN, "transform identifier"},
  {TRANSFORM_TOKEN, "transform"},
  {TRANSLATE_TOKEN, "translate"},
  {TRANSMIT_TOKEN, "transmit"},
  {TRIANGLE_TOKEN, "triangle"},
  {TRIANGLE_WAVE_TOKEN, "triangle_wave"},
  {TRUE_TOKEN, "true"},
  {TTF_TOKEN, "ttf"},
  {TURBULENCE_TOKEN, "turbulence"},
  {TURB_DEPTH_TOKEN, "turb_depth"},
  {TYPE_TOKEN, "type"},
  {T_TOKEN, "t"},
  {ULTRA_WIDE_ANGLE_TOKEN, "ultra_wide_angle"},
  {UNDEF_TOKEN, "undef"},
  {UNION_TOKEN, "union"},
  {UP_TOKEN, "up"},
  {USE_COLOUR_TOKEN, "use_color"},
  {USE_COLOUR_TOKEN, "use_colour"},
  {USE_INDEX_TOKEN, "use_index"},
  {UV_ID_TOKEN, "uv vector identifier"},
  {U_STEPS_TOKEN, "u_steps"},
  {U_TOKEN, "u"},
  {VAL_TOKEN, "val"},
  {VARIANCE_TOKEN, "variance"},
  {VAXIS_ROTATE_TOKEN, "vaxis_rotate"},
  {VCROSS_TOKEN, "vcross"},
  {VDOT_TOKEN, "vdot"},
  {VECTOR_4D_ID_TOKEN, "4d-vector identifier"},
  {VECTOR_FUNCT_TOKEN, "vector function"},
  {VECTOR_ID_TOKEN, "vector identifier"},
  {VERSION_TOKEN, "version"},
  {VLENGTH_TOKEN, "vlength"},
  {VNORMALIZE_TOKEN, "vnormalize"},
  {VROTATE_TOKEN, "vrotate"},
  {V_STEPS_TOKEN, "v_steps"},
  {V_TOKEN, "v"},
  {WARNING_TOKEN, "warning"},
  {WARP_TOKEN, "warp"},
  {WATER_LEVEL_TOKEN, "water_level"},
  {WAVES_TOKEN, "waves"},
  {WHILE_TOKEN, "while"},
  {WIDTH_TOKEN, "width"},
  {WOOD_TOKEN, "wood"},
  {WRINKLES_TOKEN, "wrinkles"},
  {WRITE_TOKEN, "write"},
  {X_TOKEN, "x"},
  {YES_TOKEN, "yes"},
  {Y_TOKEN, "y"},
  {Z_TOKEN, "z"}                                
};



/*****************************************************************************
* Static functions
******************************************************************************/

static int Echo_ungetc (int c);
static int Echo_getc (void);
static int Skip_Spaces (void);
static int Parse_C_Comments (void);
static void Begin_String (void);
static void Stuff_Character (int c);
static void End_String (void);
static int Read_Float (void);
static void Parse_String_Literal (void);
static void Read_Symbol (void);
static SYM_ENTRY *Find_Symbol (int Index, char *s);
static void Skip_Tokens (COND_TYPE cond);

static int get_hash_value (char *s);
static void Write_Token (TOKEN Token_Id);
static void Destroy_Table (int index);
static void init_sym_tables (void);
static void Add_Sym_Table (char *s);
static void Remove_Symbol (int Index, char *Name);
static POV_MACRO *Parse_Macro(void);
static void Invoke_Macro(void);
static void Return_From_Macro(void);
static void Add_Entry (int Index,SYM_ENTRY *Table_Entry);
static void Parse_Initalizer (int Sub, int Base, POV_ARRAY *a);      

static void Parse_Fopen(void);
static void Parse_Fclose(void);
static void Parse_Read(void);
static void Parse_Write(void);
static int Parse_Comma_RParen(void);
static int Parse_Read_Value(DATA_FILE *User_File,int Previous,int *NumberPtr,void **DataPtr);
static void Check_Macro_Vers(void);
static DBL Parse_Cond_Param(void);
static void Parse_Cond_Param2(DBL *V1,DBL *V2);
static void Inc_CS_Index(void);


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Initialize_Tokenizer()
{
  char b[FILE_NAME_LENGTH];
  Stage = STAGE_TOKEN_INIT;

  pre_init_tokenizer ();

  if (opts.Options & FROM_STDIN)
  {
    Data_File->File = stdin;
  }
  else
  {
     if (input_file_in_memory)
     {
        /* Note platforms which use this feature will
         * trap fopen so the NULL is ok [C.Cason 7/3/96]
         */
        Data_File->File = fopen (NULL, "rt") ;
     }
     else
     {
        Data_File->File = Locate_File (opts.Input_File_Name, READ_TXTFILE_STRING,".pov",".POV",b,TRUE);
        strcpy(opts.Input_File_Name,b);
     }
  }

  if (Data_File->File == NULL)
  {
    Error ("Cannot open input file.");
  }

  Data_File->Filename = (char *)POV_MALLOC(strlen(opts.Input_File_Name)+1, "filename");

  strcpy (Data_File->Filename, opts.Input_File_Name);

  Data_File->Line_Number = 0;
  Data_File->R_Flag = FALSE;

  /* Init echo buffer. */

  Echo_Buff = (char **)POV_MALLOC(sizeof(char *) * Num_Echo_Lines, "echo buffer");

  for (Echo_Line = 0; Echo_Line < Num_Echo_Lines; Echo_Line++)
  {
    Echo_Buff[Echo_Line] = (char *)POV_MALLOC((size_t)Echo_Line_Length+10, "echo buffer");

    Echo_Buff[Echo_Line][0]='\0';
  }

  Echo_Line = 0;
  Echo_Ptr = Echo_Buff[0];
  Got_EOF  = FALSE;
  
  /* Init conditional stack. */

  Cond_Stack = (CS_ENTRY*)POV_MALLOC(sizeof(CS_ENTRY) * COND_STACK_SIZE, "conditional stack");

  Cond_Stack[0].Cond_Type    = ROOT_COND;
  Cond_Stack[0].Switch_Value = 0.0;

  init_sym_tables();
  Max_Trace_Level = 5;      
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void pre_init_tokenizer ()
{
  Token.Token_Line_No = 0;
  Token.Token_String  = NULL;
  Token.Unget_Token   = FALSE;
  Token.End_Of_File   = FALSE;
  Token.Filename      = NULL;
  Token.Data = NULL;

  line_count = 10;
  token_count = 0;
  Include_File_Index = 0;
  Echo_Indx=0;
  Echo_Line=0;
  Echo_Ptr=NULL;
  Echo_Buff=NULL;

  CS_Index            = 0;
  Skipping            = FALSE;
  Inside_Ifdef        = FALSE;
  Inside_MacroDef     = FALSE;
  Cond_Stack          = NULL;
  Data_File = &Include_Files[0];
  Data_File->Filename = NULL;
  Table_Index         = -1;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Terminate_Tokenizer()
{
  while(Table_Index >= 0)
  {
     Destroy_Table(Table_Index--);
  }

  if (Data_File->Filename != NULL)
  {
    fclose (Data_File->File);
    Got_EOF=FALSE;

    POV_FREE (Data_File->Filename);

    Data_File->Filename = NULL;
  }

  while (Include_File_Index >= 0)
  {
    Data_File = &Include_Files[Include_File_Index--];
  
    if (Data_File->Filename != NULL)
    {
      fclose (Data_File->File);
      Got_EOF=FALSE;

      POV_FREE (Data_File->Filename);

      Data_File->Filename = NULL;
    }
  }

  if (Echo_Buff != NULL)
  {
    for (Echo_Line = 0; Echo_Line < Num_Echo_Lines; Echo_Line++)
    {
      if (Echo_Buff[Echo_Line]!=NULL)
      {
        POV_FREE (Echo_Buff[Echo_Line]);

        Echo_Buff[Echo_Line]=NULL;
      }
    }

    POV_FREE (Echo_Buff);

    Echo_Buff = NULL;
  }

  if (Cond_Stack!=NULL)
  {
    POV_FREE (Cond_Stack);

    Cond_Stack = NULL;
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   The main tokenizing routine.  Set up the files and continue parsing
*   until the end of file
*
*   Read a token from the input file and store it in the Token variable.
*   If the token is an INCLUDE token, then set the include file name and
*   read another token.
*
*   This function performs most of the work involved in tokenizing.  It
*   reads the first character of the token and decides which function to
*   call to tokenize the rest.  For simple tokens, it simply writes them
*   out to the token buffer.
*
* CHANGES
*
******************************************************************************/

void Get_Token ()
{
  register int c,c2;

  if (Token.Unget_Token)
  {
    Token.Unget_Token = FALSE;

    return;
  }

  if (Token.End_Of_File)
  {
    return;
  }

  Token.Token_Id = END_OF_FILE_TOKEN;

  while (Token.Token_Id == END_OF_FILE_TOKEN)
  {
    Skip_Spaces();

    c = Echo_getc();

    if (c == EOF)
    {
      if (Data_File->R_Flag)
      {
        Token.Token_Id = END_OF_FILE_TOKEN;
        Token.End_Of_File = TRUE;
        return;
      }
      
      if (Include_File_Index == 0)
      {
        if (CS_Index !=0)
          Error("End of file reached but #end expected.");

        Token.Token_Id = END_OF_FILE_TOKEN;

        Token.End_Of_File = TRUE;

        Status_Info("\n");
        
        return;
      }

      UICB_CLOSE_INCLUDE_FILE  /* Notify UI that we are about to close an include file normally */

      fclose(Data_File->File); /* added to fix open file buildup JLN 12/91 */
      Got_EOF=FALSE;
      
      Destroy_Table(Table_Index--);

      POV_FREE (Data_File->Filename);

      Data_File = &Include_Files[--Include_File_Index];

      continue;
    }

    String[0] = c; /* This isn't necessar but helps debugging */

    String[1] = '\0';

    String_Index = 0;

    switch (c)
    {
      case '\n':
        Data_File->Line_Number++;
        COOPERATE_0
        break;

      case '{' :
        Write_Token (LEFT_CURLY_TOKEN);
        break;
  
      case '}' :
        Write_Token (RIGHT_CURLY_TOKEN);
        break;
  
      case '@' :
        Write_Token (AT_TOKEN);
        break;
  
      case '&' :
        Write_Token (AMPERSAND_TOKEN);
        break;
  
      case '`' :
        Write_Token (BACK_QUOTE_TOKEN);
        break;
  
      case '\\':
        Write_Token (BACK_SLASH_TOKEN);
        break;

      case '|' :
        Write_Token (BAR_TOKEN);
        break;
  
      case ':' :
        Write_Token (COLON_TOKEN);
        break;
  
      case ',' :
        Write_Token (COMMA_TOKEN);
        break;
  
      case '-' :
        Write_Token (DASH_TOKEN);
        break;
  
      case '$' :
        Write_Token (DOLLAR_TOKEN);
        break;
  
      case '=' :
        Write_Token (EQUALS_TOKEN);
        break;
  
      case '!' :
        c2 = Echo_getc();
        if (c2 == (int)'=')
        {
          Write_Token (REL_NE_TOKEN);
        }
        else
        {
          Echo_ungetc(c2);
          Write_Token (EXCLAMATION_TOKEN);
        }
        break;
  
      case '#' : 
        Parse_Directive(TRUE);
        /* Write_Token (HASH_TOKEN);*/
        break;
  
      case '^' :
        Write_Token (HAT_TOKEN);
        break;
  
      case '<' :
        c2 = Echo_getc();
        if (c2 == (int)'=')
        {
          Write_Token (REL_LE_TOKEN);
        }
        else
        {
          Echo_ungetc(c2);
          Write_Token (LEFT_ANGLE_TOKEN);
        }
        break;
  
      case '(' :
        Write_Token (LEFT_PAREN_TOKEN);
        break;
  
      case '[' :
        Write_Token (LEFT_SQUARE_TOKEN);
        break;
  
      case '%' :
        Write_Token (PERCENT_TOKEN);
        break;
  
      case '+' :
        Write_Token (PLUS_TOKEN);
        break;
  
      case '?' :
        Write_Token (QUESTION_TOKEN);
        break;
  
      case '>' :
        c2 = Echo_getc();
        if (c2 == (int)'=')
        {
          Write_Token (REL_GE_TOKEN);
        }
        else
        {
          Echo_ungetc(c2);
          Write_Token (RIGHT_ANGLE_TOKEN);
        }
        break;
  
      case ')' :
        Write_Token (RIGHT_PAREN_TOKEN);
        break;
  
      case ']' :
        Write_Token (RIGHT_SQUARE_TOKEN);
        break;
  
      case ';' : /* Parser doesn't use it, so let's ignore it */
        Write_Token (SEMI_COLON_TOKEN);
        break;
  
      case '\'':
        Write_Token (SINGLE_QUOTE_TOKEN);
        break;
  
        /* enable C++ style commenting */
      case '/' :
        c2 = Echo_getc();
        if(c2 != (int) '/' && c2 != (int) '*')
        {
          Echo_ungetc(c2);
          Write_Token (SLASH_TOKEN);
          break;
        }
        if(c2 == (int)'*')
        {
          Parse_C_Comments();
          break;
        }
        while((c2 != (int)'\n') && (c2 != (int)'\r'))
        {
          c2=Echo_getc();
          if(c2==EOF)
          {
            Echo_ungetc(c2);
            break;
          }
        }
        if (c2 =='\n')
        {
          Data_File->Line_Number++;
        }
        COOPERATE_0
        break;

      case '*' :
        Write_Token (STAR_TOKEN);
        break;
  
      case '~' :
        Write_Token (TILDE_TOKEN);
        break;
  
      case '"' :
        Parse_String_Literal ();
        break;
  
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '.':
        Echo_ungetc(c);
        if (Read_Float () != TRUE)
          return;
        break;
  
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
  
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '_':
        Echo_ungetc(c);
        Read_Symbol ();
        break;
      case '\t':
      case '\r':
      case '\032':   /* Control Z - EOF on many systems */
      case '\0':
        break;
  
      default:
        Error("Illegal character in input file, value is %02x.\n", c);
        break;
    }
  }

  token_count++;

  if (token_count > 1000)
  {
    token_count = 0;

    COOPERATE_0

    Check_User_Abort(FALSE);

    Status_Info(".");

    line_count++;

    if (line_count > 78)
    {
      line_count = 0;

      Status_Info ("\n");
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Mark that the token has been put back into the input stream.  The next
*   call to Get_Token will return the last-read token instead of reading a
*   new one from the file.
*
* CHANGES
*
******************************************************************************/

void Unget_Token ()
{
  Token.Unget_Token = TRUE;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Skip over spaces in the input file.
*
* CHANGES
*
******************************************************************************/

static int Skip_Spaces ()
{
  register int c;

  while (TRUE)
  {
    c = Echo_getc();

    if (c == EOF)
    {
      return (FALSE);
    }

    if (!(isspace(c) || c == 0x0A))
    {
      break;
    }

    if (c == '\n')
    {
      Data_File->Line_Number++;

      COOPERATE_0
    }
  }

  Echo_ungetc(c);

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   C style comments with asterik and slash - CdW 8/91.
*
* CHANGES
*
******************************************************************************/

static int Parse_C_Comments()
{
  register int c, c2;
  int End_Of_Comment;

  End_Of_Comment = FALSE;

  while (!End_Of_Comment)
  {
    c = Echo_getc();

    if (c == EOF)
    {
      Error ("No */ closing comment found.");
    }

    if (c == (int) '\n')
    {
      Data_File->Line_Number++;

      COOPERATE_0
    }

    if (c == (int) '*')
    {
      c2 = Echo_getc();

      if (c2 != (int) '/')
      {
        Echo_ungetc(c2);
      }
      else
      {
        End_Of_Comment = TRUE;
      }
    }

    /* Check for and handle nested comments */

    if (c == (int) '/')
    {
      c2 = Echo_getc();

      if (c2 != (int) '*')
      {
        Echo_ungetc(c2);
      }
      else
      {
        Parse_C_Comments();
      }
    }
  }

  return (TRUE);
}



/* The following routines make it easier to handle strings.  They stuff
   characters into a string buffer one at a time making all the proper
   range checks.  Call Begin_String to start, Stuff_Character to put
   characters in, and End_String to finish.  The String variable contains
   the final string. */

/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Begin_String()
{
  String_Index = 0;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Stuff_Character(int c)
{
  if (String_Index < MAX_STRING_INDEX)
  {
    String[String_Index++] = (char)c;

    if (String_Index >= MAX_STRING_INDEX)
    {
      Error ("String too long.");
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void End_String()
{
  Stuff_Character((int)'\0');
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Read a float from the input file and tokenize it as one token. The phase
*   variable is 0 for the first character, 1 for all subsequent characters
*   up to the decimal point, 2 for all characters after the decimal
*   point, 3 for the E+/- and 4 for the exponent.  This helps to insure
*   that the number is formatted properly. E format added 9/91 CEY
*
* CHANGES
*
******************************************************************************/

static int Read_Float()
{
  register int c, Finished, Phase;

  Finished = FALSE;

  Phase = 0;

  Begin_String();

  while (!Finished)
  {
    c = Echo_getc();

    if (c == EOF)
    {
      Error ("Unexpected end of file.");
    }

    switch (Phase)
    {
      case 0:

        Phase = 1;

        if (isdigit(c))
        {
          Stuff_Character(c);
        }
        else
        {
          if (c == '.')
          {
            c = Echo_getc();

            if (c == EOF)
            {
              Error ("Unexpected end of file");
            }

            if (isdigit(c))
            {
              Stuff_Character('0');
              Stuff_Character('.');
              Stuff_Character(c);

              Phase = 2;
            }
            else
            {
              Echo_ungetc(c);

              Write_Token (PERIOD_TOKEN);

              return(TRUE);
            }
          }
          else
          {
            Error ("Error in decimal number");
          }
        }

        break;

      case 1:
        if (isdigit(c))
        {
          Stuff_Character(c);
        }
        else
        {
          if (c == (int) '.')
          {
            Stuff_Character(c); Phase = 2;
          }
          else
          {
            if ((c == 'e') || (c == 'E'))
            {
              Stuff_Character(c); Phase = 3;
            }
            else
            {
              Finished = TRUE;
            }
          }
        }

        break;

      case 2:

        if (isdigit(c))
        {
          Stuff_Character(c);
        }
        else
        {
          if ((c == 'e') || (c == 'E'))
          {
            Stuff_Character(c); Phase = 3;
          }
          else
          {
            Finished = TRUE;
          }
        }

        break;

      case 3:

        if (isdigit(c) || (c == '+') || (c == '-'))
        {
          Stuff_Character(c); Phase = 4;
        }
        else
        {
          Finished = TRUE;
        }

        break;

      case 4:

        if (isdigit(c))
        {
          Stuff_Character(c);
        }
        else
        {
          Finished = TRUE;
        }

        break;
    }
  }

  Echo_ungetc(c);

  End_String();

  Write_Token (FLOAT_TOKEN);

  if (sscanf (String, DBL_FORMAT_STRING, &Token.Token_Float) == 0)
  {
    return (FALSE);
  }

  return (TRUE);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Parse a string from the input file into a token.
*
* CHANGES
*
******************************************************************************/

static void Parse_String_Literal()
{
  register int c;

  Begin_String();

  while (TRUE)
  {
    c = Echo_getc();

    if (c == EOF)
    {
      Error ("No end quote for string.");
    }

    if (c == '\\')
    {
      switch(c = Echo_getc())
      {
        case '\n':
        case '\r':

          Error("Unterminated string literal.");

          break;

        case '\"':

          c = 0x22;

          break;

        case EOF:

          Error ("No end quote for string.");

          break;

        case '\\' :
          c='\\';
          break;

        default:

          Stuff_Character ('\\');
      }

      Stuff_Character (c);
    }
    else
    {
      if (c != (int) '"')
      {
        Stuff_Character (c);
      }
      else
      {
        break;
      }
    }
  }

  End_String();

  Write_Token (STRING_LITERAL_TOKEN);

  Token.Token_String = String;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*   Read in a symbol from the input file. Check to see if it is a reserved
*   word. If it is, write out the appropriate token. Otherwise, write the
*   symbol out to the symbol table and write out an IDENTIFIER token. An
*   identifier token is a token whose token number is greater than the
*   highest reserved word.
*
* CHANGES
*
******************************************************************************/

static void Read_Symbol()
{
  register int c;
  int Local_Index,i,j,k;
  POV_ARRAY *a;
  SYM_ENTRY *Temp_Entry;
  POV_PARAM *Par;
  DBL val;

  Begin_String();

  while (TRUE)
  {
    c = Echo_getc();

    if (c == EOF)
    {
      Error ("Unexpected end of file.");
    }

    if (isalpha(c) || isdigit(c) || c == (int) '_')
    {
      Stuff_Character(c);
    }
    else
    {
      Echo_ungetc(c);

      break;
    }
  }

  End_String();

  if (Inside_Ifdef)
  {
    Token.Token_Id = IDENTIFIER_TOKEN;

    return;
  }

  /* If its a reserved keyword, write it and return */
  if ( (Temp_Entry = Find_Symbol(0,String)) != NULL)
  {
    Write_Token (Temp_Entry->Token_Number);
    return;
  }
  
  if (!Skipping)
  {
    /* Search tables from newest to oldest */
    for (Local_Index=Table_Index; Local_Index > 0; Local_Index--)
    {
      if ((Temp_Entry = Find_Symbol(Local_Index,String)) != NULL)
      {
         /* Here its a previously declared identifier. */
    
         if (Temp_Entry->Token_Number==MACRO_ID_TOKEN)
         {
           Token.Data = Temp_Entry->Data;
           if (Ok_To_Declare)
           {
              Invoke_Macro();
           }
           else
           {
              Token.Token_Id=MACRO_ID_TOKEN;
           }
           return;
         }
         
         Token.Token_Id  =   Temp_Entry->Token_Number;
         Token.NumberPtr = &(Temp_Entry->Token_Number);
         Token.DataPtr   = &(Temp_Entry->Data);
  
         while ((Token.Token_Id==PARAMETER_ID_TOKEN) ||
                (Token.Token_Id==ARRAY_ID_TOKEN))
         {
           if (Token.Token_Id==ARRAY_ID_TOKEN)
           {
             Skip_Spaces();
             c = Echo_getc();
             Echo_ungetc(c);
             
             if (c!='[')
             {
               break;
             }
             
             a = (POV_ARRAY *)(*(Token.DataPtr));
             j = 0;
  
             for (i=0; i <= a->Dims; i++)
             {
                GET(LEFT_SQUARE_TOKEN)
                val=Parse_Float();
                if (val<0.0)
                {
                  Error("Negative subscript");
                }

                k=(int)(1.0e-08+val);

                if (k>=a->Sizes[i])
                {
                   Error("Array subscript out of range");
                }
                j += k * a->Mags[i];
                GET(RIGHT_SQUARE_TOKEN)
             }
             
             Token.DataPtr   = &(a->DataPtrs[j]);
             Token.NumberPtr = &(a->Type);
             Token.Token_Id = a->Type;
             if (!LValue_Ok)
             {
                if (*Token.DataPtr==NULL)
                {
                  Error("Attempt to access uninitialized array element.");
                }
             }
           }
           else
           {
             Par             = (POV_PARAM *)(Temp_Entry->Data);
             Token.Token_Id  = *(Par->NumberPtr);
             Token.NumberPtr = Par->NumberPtr;
             Token.DataPtr   = Par->DataPtr;
           }
         }
  
         Write_Token (Token.Token_Id);
       
         Token.Data        = *(Token.DataPtr);
         Token.Table_Index = Local_Index;
         return;
      }
    }
  }

  Write_Token(IDENTIFIER_TOKEN);
}

void Write_Token (TOKEN Token_Id)
{
   Token.Token_Line_No = Data_File->Line_Number;
   Token.Filename      = Data_File->Filename;
   Token.Token_String  = String;
   Token.Data          = NULL;
   Token.Token_Id      = Token_Id;
   
   Token.Function_Id = Token.Token_Id;
   if (Token.Token_Id < FLOAT_FUNCT_TOKEN)
   {
     Token.Token_Id = FLOAT_FUNCT_TOKEN;
   }
   else
   {
     if (Token.Token_Id < VECTOR_FUNCT_TOKEN)
     {
       Token.Token_Id = VECTOR_FUNCT_TOKEN;
     }
     else
     {
       if (Token.Token_Id < COLOUR_KEY_TOKEN)
       {
         Token.Token_Id = COLOUR_KEY_TOKEN;
       }
     }
   }
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static int Echo_getc()
{
  register int c;

  if (Got_EOF)
  {
    return(EOF);
  }
  
  c = getc(Data_File->File);
  
  if (c == EOF)
  {
    ungetc(c,Data_File->File);
    c = '\n';
    Got_EOF=TRUE;
  }

  Echo_Ptr[Echo_Indx++] = c;

  if ((Echo_Indx > Echo_Line_Length) || (c == '\n'))
  {
    Echo_Ptr[Echo_Indx] = '\0';

    Echo_Indx = 0;

    Echo_Line++;

    if (Echo_Line == Num_Echo_Lines)
      Echo_Line = 0;

    Echo_Ptr=Echo_Buff[Echo_Line];
  }

  return(c);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static int Echo_ungetc(int c)
{
  if (Echo_Indx > 0)
  {
    Echo_Indx--;
  }

  return(ungetc(c,Data_File->File));
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Where_Error ()
{
  int i;

  /* Return if no filename is specified. [DB 8/94] */

  if (Token.Filename == NULL)
  {
    return;
  }

  strcpy (&(Echo_Ptr[Echo_Indx])," <----ERROR\n");

  for (i=0;i<Num_Echo_Lines;i++)
  {
    Echo_Line++;

    if (Echo_Line==Num_Echo_Lines)
    {
      Echo_Line=0;
    }

    Error_Line(Echo_Buff[Echo_Line]);
  }

  Error_Line("\n%s:%d: error: ", Token.Filename, Token.Token_Line_No+1);
}




/*****************************************************************************
*
* FUNCTION    Parse_Directive
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR      Chris Young
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Parse_Directive(int After_Hash)
{
  DBL Value, Value2;
  int Flag;
  char *ts;
  POV_MACRO *PMac=NULL;
  COND_TYPE Curr_Type = Cond_Stack[CS_Index].Cond_Type;
  long Hash_Loc = ftell(Data_File->File);
  
  if (Curr_Type == INVOKING_MACRO_COND)
  {
     if (Cond_Stack[CS_Index].PMac->Macro_End==Hash_Loc)
     {
        Return_From_Macro();
        if (--CS_Index < 0)
        {
           Error("Mis-matched '#end'.");
        }
        Token.Token_Id = END_OF_FILE_TOKEN;
        
        return;
     }
  }

  if (!Ok_To_Declare)
  {
    if (After_Hash)
    {
       Token.Token_Id=HASH_TOKEN; 
    }
    Token.Unget_Token = FALSE;

    return;
  }

  EXPECT
    CASE(IFDEF_TOKEN)
      Inc_CS_Index();

      if (Skipping)
      {
        Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
        Skip_Tokens(SKIP_TIL_END_COND);
      }
      else
      {
        if (Parse_Ifdef_Param())
        {
           Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
        }
        else
        {
           Cond_Stack[CS_Index].Cond_Type=IF_FALSE_COND;
           Skip_Tokens(IF_FALSE_COND);
        }
      }
      EXIT
    END_CASE
    
    CASE(IFNDEF_TOKEN)
      Inc_CS_Index();

      if (Skipping)
      {
        Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
        Skip_Tokens(SKIP_TIL_END_COND);
      }
      else
      {
        if (Parse_Ifdef_Param())
        {
           Cond_Stack[CS_Index].Cond_Type=IF_FALSE_COND;
           Skip_Tokens(IF_FALSE_COND);
        }
        else
        {
           Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
        }
      }
      EXIT
    END_CASE
    
    CASE(IF_TOKEN)
      Inc_CS_Index();

      if (Skipping)
      {
        Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
        Skip_Tokens(SKIP_TIL_END_COND);
      }
      else
      {
        Value=Parse_Cond_Param();
      
        if (fabs(Value)>EPSILON)
        {
           Cond_Stack[CS_Index].Cond_Type=IF_TRUE_COND;
        }
        else
        {
           Cond_Stack[CS_Index].Cond_Type=IF_FALSE_COND;
           Skip_Tokens(IF_FALSE_COND);
        }
      }
      EXIT
    END_CASE

    CASE(WHILE_TOKEN)
      Inc_CS_Index();

      if (Skipping)
      {
        Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
        Skip_Tokens(SKIP_TIL_END_COND);
      }
      else
      {
        Cond_Stack[CS_Index].While_File_Name = POV_STRDUP(Data_File->Filename);
        Cond_Stack[CS_Index].Pos        = ftell(Data_File->File);
        Cond_Stack[CS_Index].Line_No    = Data_File->Line_Number;

        Value=Parse_Cond_Param();
       
        if (fabs(Value)>EPSILON)
        {
           Cond_Stack[CS_Index].Cond_Type = WHILE_COND;
        }
        else
        {
           Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
           Skip_Tokens(SKIP_TIL_END_COND);
        }
      }
      EXIT
    END_CASE
    
    CASE(ELSE_TOKEN)
      switch (Curr_Type)
      {
         case IF_TRUE_COND:
           Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
           Skip_Tokens(SKIP_TIL_END_COND);
           break;
           
         case IF_FALSE_COND:
           Cond_Stack[CS_Index].Cond_Type = ELSE_COND;
           Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
           UNGET
           break;
         
         case CASE_TRUE_COND:
         case SKIP_TIL_END_COND:
           break;

         case CASE_FALSE_COND:
           Cond_Stack[CS_Index].Cond_Type = CASE_TRUE_COND;
           if (Skipping)
           {
              Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
              UNGET
           }
           break;

         default:
           Error("Mis-matched '#else'.");
      }
      EXIT
    END_CASE

    CASE(SWITCH_TOKEN)
      Inc_CS_Index();

      if (Skipping)
      {
        Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
        Skip_Tokens(SKIP_TIL_END_COND);
      }
      else
      {
        Cond_Stack[CS_Index].Switch_Value=Parse_Cond_Param();
        Cond_Stack[CS_Index].Cond_Type=SWITCH_COND;
        EXPECT
          CASE2(CASE_TOKEN,RANGE_TOKEN)
            if (Token.Token_Id==CASE_TOKEN)
            {
              Value=Parse_Cond_Param();
              Flag = (fabs(Value-Cond_Stack[CS_Index].Switch_Value)<EPSILON);
            }
            else
            {
              Parse_Cond_Param2(&Value,&Value2);
              Flag = ((Cond_Stack[CS_Index].Switch_Value >= Value) &&
                      (Cond_Stack[CS_Index].Switch_Value <= Value2));
            }
 
            if(Flag)
            {
              Cond_Stack[CS_Index].Cond_Type=CASE_TRUE_COND;
            }
            else
            {
              Cond_Stack[CS_Index].Cond_Type=CASE_FALSE_COND;
              Skip_Tokens(CASE_FALSE_COND);
            }
            EXIT
          END_CASE

          OTHERWISE
            Error("#switch not followed by #case or #range.");
          END_CASE
        END_EXPECT
      }
      EXIT
    END_CASE

    CASE(BREAK_TOKEN)
      if (Curr_Type==CASE_TRUE_COND)
      {
        Cond_Stack[CS_Index].Cond_Type=SKIP_TIL_END_COND;
        Skip_Tokens(SKIP_TIL_END_COND);
      }          
      EXIT
    END_CASE
    
    CASE2(CASE_TOKEN,RANGE_TOKEN)
      switch(Curr_Type)
      {
        case CASE_TRUE_COND:
        case CASE_FALSE_COND:
          if (Token.Token_Id==CASE_TOKEN)
          {
            Value=Parse_Cond_Param();
            Flag = (fabs(Value-Cond_Stack[CS_Index].Switch_Value)<EPSILON);
          }
          else
          {
            Parse_Cond_Param2(&Value,&Value2);
            Flag = ((Cond_Stack[CS_Index].Switch_Value >= Value) &&
                    (Cond_Stack[CS_Index].Switch_Value <= Value2));
          }
          
          if(Flag && (Curr_Type==CASE_FALSE_COND))
          {
            Cond_Stack[CS_Index].Cond_Type=CASE_TRUE_COND;
            if (Skipping)
            {
               Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
               UNGET
            }
          }
          break;           

        case SWITCH_COND:
          UNGET
        case SKIP_TIL_END_COND:
          break;

        default:
          Error("Mis-matched '#case' or '#range'.");
      }
      EXIT
    END_CASE
    
    CASE(END_TOKEN)
      switch (Curr_Type)
      {
         case INVOKING_MACRO_COND:
           Return_From_Macro();
           if (--CS_Index < 0)
           {
              Error("Mis-matched '#end'.");
           }
           break;
         
         case IF_FALSE_COND:
           Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
           UNGET
         case IF_TRUE_COND:
         case ELSE_COND:
         case CASE_TRUE_COND:
         case CASE_FALSE_COND:
         case DECLARING_MACRO_COND:
         case SKIP_TIL_END_COND:
           if (Curr_Type==DECLARING_MACRO_COND)
           {
             if ((PMac=Cond_Stack[CS_Index].PMac)!=NULL)
             {
                PMac->Macro_End=Hash_Loc;
             }
           }
           if (--CS_Index < 0)
           {
              Error("Mis-matched '#end'.");
           }
           if (Skipping)
           {
              Token.Token_Id=HASH_TOKEN; /*insures Skip_Token takes notice*/
              UNGET
           }
           break;
         
         case WHILE_COND:
           if (strcmp(Cond_Stack[CS_Index].While_File_Name,Data_File->Filename))
           { 
              Error("#while loop didn't end in file where it started.");
           }
           
           Got_EOF=FALSE;
           if (fseek(Data_File->File, Cond_Stack[CS_Index].Pos,0) < 0)
           {
              Error("Unable to seek in input file for #while directive.\n");
           }

           Data_File->Line_Number = Cond_Stack[CS_Index].Line_No;

           Value=Parse_Cond_Param();
      
           if (fabs(Value)<EPSILON)
           {
             POV_FREE(Cond_Stack[CS_Index].While_File_Name);
             Cond_Stack[CS_Index].While_File_Name=NULL;
             Cond_Stack[CS_Index].Cond_Type = SKIP_TIL_END_COND;
             Skip_Tokens(SKIP_TIL_END_COND);
           }
           break;

         default:
           Error("Mis-matched '#end'.");
      }
      EXIT
    END_CASE

    CASE2 (DECLARE_TOKEN,LOCAL_TOKEN)
      if (Skipping)
      {
         UNGET
         EXIT
      }
      else
      {
         Parse_Declare ();
         Curr_Type = Cond_Stack[CS_Index].Cond_Type;
         if (Token.Unget_Token)
         {
           switch (Token.Token_Id)
           {
              case HASH_TOKEN:
                Token.Unget_Token=FALSE;
                break;
              
              case MACRO_ID_TOKEN:
                break;
              
              default:
                EXIT
           }
         }
         else
         {
            EXIT
         }
      }
    END_CASE
    
    CASE (DEFAULT_TOKEN)
      Parse_Default();
      EXIT
    END_CASE

    CASE (INCLUDE_TOKEN)
      if (Skipping)
      {
         UNGET
      }
      else
      {
         Open_Include();
      }
      EXIT
    END_CASE

    CASE (FLOAT_FUNCT_TOKEN)
      if (Skipping)
      {
        UNGET
        EXIT
      }
      else
      {
        switch(Token.Function_Id)
        {
           case VERSION_TOKEN:
             Ok_To_Declare = FALSE;
             opts.Language_Version = Parse_Float ();
             Parse_Semi_Colon();
             Ok_To_Declare = TRUE;
             Curr_Type = Cond_Stack[CS_Index].Cond_Type;
             if (Token.Unget_Token && (Token.Token_Id==HASH_TOKEN))
             {
                Token.Unget_Token=FALSE;
             }
             else
             {
                EXIT
             }
             break;

           default:
             UNGET
             Parse_Error_Str ("object or directive.");
             break;
        }
      }
    END_CASE

    CASE(WARNING_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {     
        ts=Parse_Formatted_String();
        Warning(0.0,ts);
        POV_FREE(ts);
      }
      EXIT
    END_CASE
      
    CASE(ERROR_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {     
        ts=Parse_Formatted_String();
        POV_FREE(ts);
        Error("User error directive hit.");
      }
      EXIT
    END_CASE
      
    CASE(RENDER_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {     
        ts=Parse_Formatted_String();
        Render_Info(ts);
        POV_FREE(ts);
      }
      EXIT
    END_CASE
      
    CASE(STATISTICS_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {     
        ts=Parse_Formatted_String();
        Statistics(ts);
        POV_FREE(ts);
      }
      EXIT
    END_CASE
      
    CASE(DEBUG_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {     
        ts=Parse_Formatted_String();
        Debug_Info(ts);
        POV_FREE(ts);
      }
      EXIT
    END_CASE

    CASE(FOPEN_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {
        Parse_Fopen();
      }
      EXIT
    END_CASE

    CASE(FCLOSE_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {
        Parse_Fclose();
      }
      EXIT
    END_CASE

    CASE(READ_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {
        Parse_Read();
      }
      EXIT
    END_CASE

    CASE(WRITE_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {
        Parse_Write();
      }
      EXIT
    END_CASE

    CASE(UNDEF_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      {
        EXPECT
          CASE (IDENTIFIER_TOKEN)
            Warn(0.0,"Attempt to undef unknown identifier");
            EXIT
          END_CASE

          CASE4 (TNORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
          CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
          CASE4 (SLOPE_MAP_ID_TOKEN,NORMAL_MAP_ID_TOKEN,TEXTURE_MAP_ID_TOKEN,COLOUR_ID_TOKEN)
          CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN,STRING_ID_TOKEN,INTERIOR_ID_TOKEN)
          CASE4 (ARRAY_ID_TOKEN, DENSITY_ID_TOKEN, DENSITY_MAP_ID_TOKEN, FILE_ID_TOKEN)
          CASE4 (FOG_ID_TOKEN, MACRO_ID_TOKEN, PARAMETER_ID_TOKEN, RAINBOW_ID_TOKEN)
          CASE4 (SKYSPHERE_ID_TOKEN,MATERIAL_ID_TOKEN,UV_ID_TOKEN,VECTOR_4D_ID_TOKEN)
            Remove_Symbol (Token.Table_Index,Token.Token_String);
            EXIT
          END_CASE
        
          CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
            switch(Token.Function_Id)
            {
              case VECTOR_ID_TOKEN:
              case FLOAT_ID_TOKEN:
                 Remove_Symbol (Token.Table_Index,Token.Token_String);
                 break;

              default:
                 Parse_Error(IDENTIFIER_TOKEN);
                 break;
            }
            EXIT
          END_CASE

          OTHERWISE
            Parse_Error(IDENTIFIER_TOKEN);
          END_CASE
        END_EXPECT
      }
      EXIT
    END_CASE

    CASE (MACRO_ID_TOKEN)
      if (Skipping)
      {
        UNGET
      }
      else
      { 
        Invoke_Macro();
      }
      EXIT
    END_CASE

    CASE (MACRO_TOKEN)
      if (!Skipping)
      {
        if (Inside_MacroDef)
        {
          Error("Cannot nest macro definitions");
        }
        Inside_MacroDef=TRUE;
        PMac=Parse_Macro();
        Inside_MacroDef=FALSE;
      }
      Inc_CS_Index();
      Cond_Stack[CS_Index].Cond_Type = DECLARING_MACRO_COND;
      Cond_Stack[CS_Index].PMac      = PMac;
      Skip_Tokens(DECLARING_MACRO_COND);
      EXIT
    END_CASE

    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT
  
  if (Token.Unget_Token)
  {
    Token.Unget_Token = FALSE;
  }
  else
  {
    Token.Token_Id = END_OF_FILE_TOKEN;
  }
}


/*****************************************************************************
*
* FUNCTION    Get_Include_File_Depth
*
* INPUT       -none-
*
* OUTPUT      -none-
*
* RETURNS     The index of the currently opened file (0, 1, 2...)
*
* AUTHOR      Eduard Schwan
*
* DESCRIPTION Call this from GUI to determine which file was being parsed
*             When a fatal error happened.  The index can be used to access
*             the file array (retrieved with the Get_File_Array() call.)
*
* CHANGES
*
******************************************************************************/

int Get_Include_File_Depth(void)
{
	return Include_File_Index;
}


/*****************************************************************************
*
* FUNCTION    Get_Include_File_Array
*
* INPUT       -none-
*
* OUTPUT      -none-
*
* RETURNS     A pointer to the array of file records.
*
* AUTHOR      Eduard Schwan
*
* DESCRIPTION Call this from GUI to determine which file was being parsed
*             When a fatal error happened.
*
* CHANGES
*
******************************************************************************/

DATA_FILE * Get_Include_File_Array(void)
{
	return Include_Files;
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Open_Include()
{
   char *temp;
   char b[FILE_NAME_LENGTH];

   if (Skip_Spaces () != TRUE)
     Error ("Expecting a string after INCLUDE.\n");

   Include_File_Index++;

   if (Include_File_Index >= MAX_INCLUDE_FILES)
   {
     Include_File_Index--;
     Error ("Too many nested include files.\n");
   }
   temp = Parse_String();

   UICB_OPEN_INCLUDE_FILE  /* Notify UI that we are about to open an include file */

   Echo_Ptr[Echo_Indx++] = '\n';
   Echo_Ptr[Echo_Indx] = '\0';
   Echo_Indx = 0;
   Echo_Line++;
   if (Echo_Line == Num_Echo_Lines)
   {
     Echo_Line = 0;
   }
   Echo_Ptr=Echo_Buff[Echo_Line];

   Data_File = &Include_Files[Include_File_Index];
   Data_File->Line_Number = 0;

   if ((Data_File->File = Locate_File (temp, READ_TXTFILE_STRING,".inc",".INC",b,TRUE)) == NULL)
   {
      Data_File->Filename = NULL;  /* Keeps from closing failed file. */
      Stage=STAGE_INCLUDE_ERR;
      Error ("Cannot open include file %s.\n", temp);
   }
   
   POV_FREE(temp);

   Data_File->Filename = POV_STRDUP(b);
   Data_File->R_Flag=FALSE;
   
   Add_Sym_Table(Data_File->Filename);

   Token.Token_Id = END_OF_FILE_TOKEN;
   
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void Skip_Tokens(COND_TYPE cond)
{
  int Temp      = CS_Index;
  int Prev_Skip = Skipping;

  Skipping=TRUE;

  while ((CS_Index > Temp) || ((CS_Index == Temp) && (Cond_Stack[CS_Index].Cond_Type == cond)))
  {
    Get_Token();
  }

  Skipping=Prev_Skip;

  if (Token.Token_Id==HASH_TOKEN)
  {
     Token.Token_Id=END_OF_FILE_TOKEN;
     Token.Unget_Token=FALSE;
  }
  else
  {
     UNGET
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   get_hash_value
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate hash value for a given string.
*
* CHANGES
*
*   Apr 1996 : Creation.
*
******************************************************************************/

static int get_hash_value(char *s)
{
  unsigned int i = 0;

  while (*s)
  {
    i = (i << 1) ^ *s++;
  }

  return((int)(i % SYM_TABLE_SIZE));
}



/*****************************************************************************
*
* FUNCTION
*
*   init_sym_tables
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Young
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void init_sym_tables()
{
  int i;

  Add_Sym_Table("reserved words");

  for (i = 0; i < LAST_TOKEN; i++)
  {
    Add_Symbol(0,Reserved_Words[i].Token_Name,Reserved_Words[i].Token_Number);
  }

  Add_Sym_Table("global identifiers");
}

static void Add_Sym_Table(char *s)
{
  int i;

  SYM_TABLE *New;

  if ((++Table_Index)==MAX_NUMBER_OF_TABLES)
  {
    Table_Index--;
    Error("Too many nested symbol tables");
  }
  
  Tables[Table_Index]=New=(SYM_TABLE *)POV_MALLOC(sizeof(SYM_TABLE),"symbol table");
  
  New->Table_Name=POV_STRDUP(s);

  for (i = 0; i < SYM_TABLE_SIZE; i++)
  {
    New->Table[i] = NULL;
  }

}

static void Destroy_Table(int index)
{
   int i;
   SYM_TABLE *Table = Tables[index];
   SYM_ENTRY *Entry;
   
   for (i = 0 ; i < SYM_TABLE_SIZE; i++)
   {
      Entry=Table->Table[i];

      while (Entry)
      {
         Entry = Destroy_Entry(Entry);
      }
   }

   POV_FREE(Table->Table_Name);
   POV_FREE(Table);
   
}


SYM_ENTRY *Create_Entry (int Index,char *Name,TOKEN Number)
{
  SYM_ENTRY *New;
  
  New = (SYM_ENTRY *)POV_MALLOC(sizeof(SYM_ENTRY), "symbol table entry");

  New->Token_Number = Number;
  New->Data         = NULL;
  if (Index)
  {
     New->Token_Name= POV_STRDUP(Name);
  }
  else
  {
     New->Token_Name=Name;
  }
  
  return(New);
}

SYM_ENTRY *Destroy_Entry (SYM_ENTRY *Entry)
{
   SYM_ENTRY *Next;
   
   if (Entry==NULL)
   {
      return(NULL);
   }

   Next=Entry->next;

   /* if data is NULL then this is a reserved word and the name
      doesn't need freed.
    */
        
   if (Entry->Data)
   {
     POV_FREE(Entry->Token_Name);
   }

   Destroy_Ident_Data (Entry->Data,Entry->Token_Number);
    
   POV_FREE(Entry);

   return(Next);
}


static void Add_Entry (int Index,SYM_ENTRY *Table_Entry)
{
  int i = get_hash_value(Table_Entry->Token_Name);

  Table_Entry->next       = Tables[Index]->Table[i];
  Tables[Index]->Table[i] = Table_Entry;
}


SYM_ENTRY *Add_Symbol (int Index,char *Name,TOKEN Number)
{
  SYM_ENTRY *New;

  New = Create_Entry (Index,Name,Number);
  Add_Entry(Index,New);

  return(New);
}


static SYM_ENTRY *Find_Symbol(int Index,char *Name)
{
  SYM_ENTRY *Entry;
  
  int i = get_hash_value(Name);

  Entry = Tables[Index]->Table[i];

  while (Entry)
  {
    if (strcmp(Name, Entry->Token_Name) == 0)
    {
      return(Entry);
    }

    Entry = Entry->next;
  }

  return(Entry);
}


static void Remove_Symbol (int Index, char *Name)
{
  SYM_ENTRY *Entry;
  SYM_ENTRY **EntryPtr;
  
  int i = get_hash_value(Name);

  EntryPtr = &(Tables[Index]->Table[i]);
  Entry    = *EntryPtr;

  while (Entry)
  {
    if (strcmp(Name, Entry->Token_Name) == 0)
    {
      *EntryPtr = Entry->next;
      Destroy_Entry(Entry);
      return;
    }
    
    EntryPtr = &(Entry->next);
    Entry    = *EntryPtr;
  }
  
  Error("Tried to free undefined symbol");
}

static void Check_Macro_Vers(void)
{
  if (opts.Language_Version < 3.1)
  {
     Error("Macros require #version 3.1 but #version %4.2lf is set.\n",
            opts.Language_Version);
  }
}

static POV_MACRO *Parse_Macro()
{
  POV_MACRO *New;
  SYM_ENTRY *Table_Entry;
  int Old_Ok = Ok_To_Declare;

  Check_Macro_Vers();
  
  Ok_To_Declare = FALSE;

  EXPECT
    CASE (IDENTIFIER_TOKEN)
      Table_Entry = Add_Symbol (1,Token.Token_String,MACRO_ID_TOKEN);
      EXIT
    END_CASE

    CASE (MACRO_ID_TOKEN)
      Remove_Symbol(1,Token.Token_String);
      Table_Entry = Add_Symbol (1,Token.Token_String,MACRO_ID_TOKEN);
      EXIT
    END_CASE
    
    OTHERWISE
      Parse_Error(IDENTIFIER_TOKEN);
      Table_Entry = NULL; /* tw */
    END_CASE
  END_EXPECT
  
  Ok_To_Declare = Old_Ok;

  New=(POV_MACRO *)POV_MALLOC(sizeof(POV_MACRO),"macro");

  Table_Entry->Data=(void *)New;

  New->Macro_Filename = NULL;
  New->Macro_Name=POV_STRDUP(Token.Token_String);
  
  GET (LEFT_PAREN_TOKEN);
  
  New->Num_Of_Pars=0;
  
  EXPECT
    CASE2 (IDENTIFIER_TOKEN,PARAMETER_ID_TOKEN)
    CASE4 (TNORMAL_ID_TOKEN, FINISH_ID_TOKEN, TEXTURE_ID_TOKEN, OBJECT_ID_TOKEN)
    CASE4 (COLOUR_MAP_ID_TOKEN, TRANSFORM_ID_TOKEN, CAMERA_ID_TOKEN, PIGMENT_ID_TOKEN)
    CASE4 (SLOPE_MAP_ID_TOKEN,NORMAL_MAP_ID_TOKEN,TEXTURE_MAP_ID_TOKEN,COLOUR_ID_TOKEN)
    CASE4 (PIGMENT_MAP_ID_TOKEN, MEDIA_ID_TOKEN,STRING_ID_TOKEN,INTERIOR_ID_TOKEN)
    CASE4 (ARRAY_ID_TOKEN, DENSITY_ID_TOKEN, DENSITY_MAP_ID_TOKEN, FILE_ID_TOKEN)
    CASE4 (FOG_ID_TOKEN, RAINBOW_ID_TOKEN, SKYSPHERE_ID_TOKEN,MATERIAL_ID_TOKEN)
    CASE2 (UV_ID_TOKEN,VECTOR_4D_ID_TOKEN)
      New->Par_Name[New->Num_Of_Pars] = POV_STRDUP(Token.Token_String);
      if (++(New->Num_Of_Pars) == MAX_PARAMETER_LIST)
      {
        Error("Too many parameters");
      }
      Parse_Comma();
    END_CASE
    
    CASE2 (VECTOR_FUNCT_TOKEN, FLOAT_FUNCT_TOKEN)
      switch(Token.Function_Id)
      {
         case VECTOR_ID_TOKEN:
         case FLOAT_ID_TOKEN:
           New->Par_Name[New->Num_Of_Pars] = POV_STRDUP(Token.Token_String);
           if (++(New->Num_Of_Pars) == MAX_PARAMETER_LIST)
           {
             Error("Too many parameters");
           }
           Parse_Comma();
           break;

         default:
           Parse_Error_Str ("identifier or expression.");
           break;
      }
    END_CASE
    
    CASE(RIGHT_PAREN_TOKEN)
      UNGET
      EXIT
    END_CASE
    
    OTHERWISE
      Parse_Error_Str ("identifier or expression.");
    END_CASE
  END_EXPECT
  
  New->Macro_Filename = POV_STRDUP(Data_File->Filename);
  New->Macro_Pos      = ftell(Data_File->File);
  New->Macro_Line_No  = Data_File->Line_Number;

  Check_Macro_Vers();

  return (New);
}


static void Invoke_Macro()
{
  POV_MACRO *PMac=(POV_MACRO *)Token.Data;
  SYM_ENTRY **Table_Entries = NULL; /* tw */
  int i;
  char *f;
  
  Check_Macro_Vers();

  GET(LEFT_PAREN_TOKEN);
  
  if (PMac->Num_Of_Pars > 0)
  {
    Table_Entries = (SYM_ENTRY **)POV_MALLOC(sizeof(SYM_ENTRY *)*PMac->Num_Of_Pars,"parameters");

    /* We must parse all parameters before adding new symbol table
       or adding entries.  Otherwise recursion won't always work.
     */

    for (i=0; i<PMac->Num_Of_Pars; i++)
    {
      Table_Entries[i]=Create_Entry(1,PMac->Par_Name[i],IDENTIFIER_TOKEN);
      if (!Parse_RValue(IDENTIFIER_TOKEN,&(Table_Entries[i]->Token_Number),&(Table_Entries[i]->Data),TRUE,FALSE))
      {
        Error("Expected %d parameters but only %d found.",PMac->Num_Of_Pars,i);
      }
      Parse_Comma();
    }
  }
  
  GET(RIGHT_PAREN_TOKEN);
  
  Inc_CS_Index();
  Cond_Stack[CS_Index].Cond_Type = INVOKING_MACRO_COND;
  
  Cond_Stack[CS_Index].Pos               = ftell(Data_File->File);
  Cond_Stack[CS_Index].Line_No           = Data_File->Line_Number;
  Cond_Stack[CS_Index].Macro_Return_Name = Data_File->Filename;
  Cond_Stack[CS_Index].PMac              = PMac;
  
  /* Gotta have new symbol table in case #local is used */
  Add_Sym_Table(PMac->Macro_Name);
  
  if (PMac->Num_Of_Pars > 0)
  {
    for (i=0; i<PMac->Num_Of_Pars; i++)
    {
      Add_Entry(Table_Index,Table_Entries[i]);
    }

    POV_FREE(Table_Entries);
  }
  
  if (strcmp(PMac->Macro_Filename,Data_File->Filename))
  {
    /* Not in same file */
    Cond_Stack[CS_Index].Macro_Same_Flag=FALSE;
    fclose(Data_File->File);
    Got_EOF=FALSE;
    Data_File->Filename = POV_STRDUP(PMac->Macro_Filename);
    Data_File->R_Flag=FALSE;
    if ((Data_File->File = Locate_File (Data_File->Filename, READ_TXTFILE_STRING,"","",NULL,TRUE)) == NULL)
    {
       f = Data_File->Filename;
       Data_File->Filename = NULL;  /* Keeps from closing failed file. */
       Stage=STAGE_INCLUDE_ERR;
       Error ("Cannot open macro file %s.\n", f);
    }
  }
  else
  {
    Cond_Stack[CS_Index].Macro_Same_Flag=TRUE;
  }

  Data_File->Line_Number=PMac->Macro_Line_No;

  Got_EOF=FALSE;
  if (fseek(Data_File->File, PMac->Macro_Pos,0) < 0)
  {
    Error("Unable to file seek in macro.\n");
  }

  Token.Token_Id = END_OF_FILE_TOKEN;

  Check_Macro_Vers();

}

static void Return_From_Macro()
{
  char *f;

  Check_Macro_Vers();

  if (!Cond_Stack[CS_Index].Macro_Same_Flag)
  {
     fclose(Data_File->File);
     Got_EOF=FALSE;
     POV_FREE(Data_File->Filename);
     Data_File->Filename = Cond_Stack[CS_Index].Macro_Return_Name;
     Data_File->R_Flag=FALSE;
     if ((Data_File->File = Locate_File (Data_File->Filename, READ_TXTFILE_STRING,"","",NULL,TRUE)) == NULL)
     {
       f = Data_File->Filename;
       Data_File->Filename = NULL;  /* Keeps from closing failed file. */
       Stage=STAGE_INCLUDE_ERR;
       Error ("Cannot reopen file %s on macro return.\n", f);
     }
  }
 
  Data_File->Line_Number = Cond_Stack[CS_Index].Line_No;

  Got_EOF=FALSE;
  if (fseek(Data_File->File, Cond_Stack[CS_Index].Pos,0) < 0)
  {
    Error("Unable to file seek in return from macro.\n");
  }

  /* Always destroy macro locals */
  Destroy_Table(Table_Index--);
}

void Destroy_Macro(POV_MACRO *PMac)
{
  int i;
  if (PMac==NULL)
  {
    return;
  }
  
  POV_FREE(PMac->Macro_Name);
  if (PMac->Macro_Filename!=NULL)
  {
     POV_FREE(PMac->Macro_Filename);
  } 
  
  for (i=0; i < PMac->Num_Of_Pars; i++)
  {
    POV_FREE(PMac->Par_Name[i]);
  }

  POV_FREE(PMac);
}

POV_ARRAY *Parse_Array_Declare (void)
{
  POV_ARRAY *New;
  int i,j;
  
  New=(POV_ARRAY *)POV_MALLOC(sizeof(POV_ARRAY),"array");
  
  i=0;
  j=1;
  
  Ok_To_Declare = FALSE;

  EXPECT
    CASE (LEFT_SQUARE_TOKEN)
      if (i>4)
      {
         Error("Too many array dimensions");
      }
      New->Sizes[i]=(int)(1.0e-08+Parse_Float());
      j *= New->Sizes[i];
      i++;
      GET(RIGHT_SQUARE_TOKEN)
    END_CASE
    
    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT

  New->Dims     = i-1;
  New->Total    = j;
  New->Type     = EMPTY_ARRAY_TOKEN;
  New->DataPtrs = (void **)POV_MALLOC(sizeof(void *)*j,"array");

  j = 1;
             
  for(i = New->Dims; i>=0; i--)
  {
     New->Mags[i] = j;
     j *= New->Sizes[i];
  }
  
  for (i=0; i<New->Total; i++)
  {
     New->DataPtrs[i] = NULL;
  }
  
  EXPECT
    CASE(LEFT_CURLY_TOKEN)
      UNGET
        Parse_Initalizer(0,0,New);      
      EXIT
    END_CASE
    
    OTHERWISE
      UNGET
      EXIT
    END_CASE
  END_EXPECT
             
  Ok_To_Declare = TRUE;
  return(New);

} /* tw */

static void Parse_Initalizer (int Sub, int Base, POV_ARRAY *a)
{
  int i;

  Parse_Begin();
  if (Sub < a->Dims)
  {
     for(i=0; i < a->Sizes[Sub]; i++)
     {
        Parse_Initalizer(Sub+1,i*a->Mags[Sub]+Base,a);
     }
  }
  else
  {
     for(i=0; i < a->Sizes[Sub]; i++)
     {
        if (!Parse_RValue (a->Type,&(a->Type),&(a->DataPtrs[Base+i]), FALSE,FALSE))
        {
          Error("Insufficent number of initializers");
        }
        Parse_Comma();
     }
  }
  Parse_End();
  Parse_Comma();
} /* tw */

static void Parse_Fopen(void)
{
   DATA_FILE *New;
   char *temp;
   SYM_ENTRY *Entry;
   
   New=(DATA_FILE *)POV_MALLOC(sizeof(DATA_FILE),"user file");
   New->File=NULL;
   New->Filename=NULL;

   GET(IDENTIFIER_TOKEN)
   Entry = Add_Symbol (1,Token.Token_String,FILE_ID_TOKEN);
   Entry->Data=(void *)New;

   temp = New->Filename=Parse_String();

   EXPECT
     CASE(READ_TOKEN)
       New->R_Flag = TRUE;
       New->File = Locate_File (temp, READ_TXTFILE_STRING,"","",NULL,TRUE);
       EXIT
     END_CASE
     
     CASE(WRITE_TOKEN)
       New->R_Flag = FALSE;
       New->File= fopen(temp, WRITE_TXTFILE_STRING);
       EXIT
     END_CASE
     
     CASE(APPEND_TOKEN)
       New->R_Flag = FALSE;
       New->File= fopen(temp, APPEND_TXTFILE_STRING);
       EXIT
     END_CASE
     
     OTHERWISE
       Parse_Error_Str("read or write");
     END_CASE
   END_EXPECT

   if (New->File == NULL)
   {
      New->Filename=NULL;
      Error ("Cannot open user file %s.\n", temp);
   }
   New->Line_Number     = 0;
}

static void Parse_Fclose(void)
{
   DATA_FILE *Data;
   
   EXPECT
     CASE(FILE_ID_TOKEN)
       Data=(DATA_FILE *)Token.Data;
       fflush(Data->File);
       fclose(Data->File);
       Got_EOF=FALSE;
       Data->File = NULL;       /* <--- this line added -hdf- */
       Remove_Symbol (1,Token.Token_String);
       EXIT
     END_CASE
     
     OTHERWISE
       EXIT
     END_CASE
   END_EXPECT
}

static void Parse_Read()
{
   DATA_FILE *User_File;
   SYM_ENTRY *Temp_Entry;
   int End_File=FALSE;
   char *File_Id;

   GET(LEFT_PAREN_TOKEN)

   GET(FILE_ID_TOKEN)
   User_File=(DATA_FILE *)Token.Data;
   File_Id=POV_STRDUP(Token.Token_String);

   Parse_Comma(); /* Scene file comma between File_Id and 1st data ident */
   
   LValue_Ok = TRUE;
   
   EXPECT
     CASE (IDENTIFIER_TOKEN)
       if (!End_File)
       {
          Temp_Entry = Add_Symbol (1,Token.Token_String,IDENTIFIER_TOKEN);
          End_File=Parse_Read_Value (User_File,Token.Token_Id, &(Temp_Entry->Token_Number), &(Temp_Entry->Data));
          if (Parse_Comma_RParen())  /* Scene file comma between 2 idents */
          {
            EXIT
          }
       }            
     END_CASE

     CASE (STRING_ID_TOKEN)
       if (!End_File)
       {            
          End_File=Parse_Read_Value (User_File,Token.Token_Id,Token.NumberPtr,Token.DataPtr);
          if (Parse_Comma_RParen()) /* Scene file comma between 2 idents */
          {
            EXIT
          }
       }            
     END_CASE

     CASE2 (VECTOR_FUNCT_TOKEN,FLOAT_FUNCT_TOKEN)
       switch(Token.Function_Id)
       {
         case VECTOR_ID_TOKEN:
         case FLOAT_ID_TOKEN:
           if (!End_File)
           {
              End_File=Parse_Read_Value (User_File,Token.Function_Id,Token.NumberPtr,Token.DataPtr);
              if (Parse_Comma_RParen()) /* Scene file comma between 2 idents */
              {
                EXIT
              }
           }            
           break;

         default:
           Parse_Error(IDENTIFIER_TOKEN);
           break;
       }
     END_CASE
     
     CASE(COMMA_TOKEN)
       if (!End_File)
       {            
          Parse_Error(IDENTIFIER_TOKEN);
       }            
     END_CASE

     CASE(RIGHT_PAREN_TOKEN)
       EXIT
     END_CASE

     OTHERWISE
       Parse_Error(IDENTIFIER_TOKEN);
     END_CASE
   END_EXPECT

   LValue_Ok = FALSE;

   if (End_File)
   {
      fclose(User_File->File);
      Got_EOF=FALSE;
      User_File->File = NULL;   /* <--- this line added -hdf- */
      Remove_Symbol (1,File_Id);
   }
   POV_FREE(File_Id);
}

static int Parse_Read_Value(DATA_FILE *User_File,int Previous,int *NumberPtr,void **DataPtr)
{
   DATA_FILE *Temp;
   DBL Val;
   int End_File=FALSE;
   int i;
   EXPRESS Express;
   
   Temp      = Data_File;
   Data_File = User_File;
   
   EXPECT
     CASE3 (PLUS_TOKEN,DASH_TOKEN,FLOAT_FUNCT_TOKEN)
       UNGET
       Val=Parse_Signed_Float();
       *NumberPtr = FLOAT_ID_TOKEN;
       Test_Redefine(Previous,NumberPtr,*DataPtr);
       *DataPtr   = (void *) Create_Float();
       *((DBL *)*DataPtr) = Val;
       Parse_Comma(); /* data file comma between 2 data items  */
       EXIT
     END_CASE
     
     CASE (LEFT_ANGLE_TOKEN)
       i=1;
       Express[X]=Parse_Signed_Float();  Parse_Comma();
       Express[Y]=Parse_Signed_Float();  Parse_Comma();

       EXPECT
         CASE3 (PLUS_TOKEN,DASH_TOKEN,FLOAT_FUNCT_TOKEN)
           UNGET
           if (++i>4)
           {
              Error("Vector data too long");
           }
           Express[i]=Parse_Signed_Float(); Parse_Comma();
         END_CASE
         
         CASE (RIGHT_ANGLE_TOKEN)
           EXIT
         END_CASE
         
         OTHERWISE
           Parse_Error_Str("vector");
         END_CASE
       END_EXPECT
       
       switch(i)
       {
          case 1:
            *NumberPtr = UV_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr);
            *DataPtr   = (void *) Create_UV_Vect();
            Assign_UV_Vect(*DataPtr, Express);
            break;
            
          case 2:
            *NumberPtr = VECTOR_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr);
            *DataPtr   = (void *) Create_Vector();
            Assign_Vector(*DataPtr, Express);
            break;
            
          case 3:
            *NumberPtr = VECTOR_4D_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr);
            *DataPtr   = (void *) Create_Vector_4D();
            Assign_Vector_4D(*DataPtr, Express);
            break;
            
          case 4:
            *NumberPtr    = COLOUR_ID_TOKEN;
            Test_Redefine(Previous,NumberPtr,*DataPtr);
            *DataPtr      = (void *) Create_Colour();
            Assign_Colour(*DataPtr, Express);
            break;
       }

       Parse_Comma(); /* data file comma between 2 data items  */
       EXIT
     END_CASE

     CASE(STRING_LITERAL_TOKEN)
       *NumberPtr = STRING_ID_TOKEN;
       Test_Redefine(Previous,NumberPtr,*DataPtr);
       *DataPtr   = POV_MALLOC(strlen(Token.Token_String) + 1, "temporary string");
       strcpy ((char *)*DataPtr, Token.Token_String);
       Parse_Comma(); /* data file comma between 2 data items  */
       EXIT
     END_CASE
     
     CASE (END_OF_FILE_TOKEN)
       EXIT
     END_CASE

     OTHERWISE
       Parse_Error_Str ("float, vector, or string literal");
     END_CASE
   END_EXPECT
   
   if (Token.Token_Id==END_OF_FILE_TOKEN)
   {
      End_File = TRUE;
   }
   
   Token.End_Of_File = FALSE;
   Token.Unget_Token = FALSE;
   Got_EOF           = FALSE;
   Data_File = Temp;
   
   return(End_File);
}

static int Parse_Comma_RParen(void)
{
  int Stop=FALSE;
  int Old_Ok=Ok_To_Declare;
  
  Ok_To_Declare=FALSE;

  EXPECT
    CASE(COMMA_TOKEN)
      EXIT
    END_CASE
    
    CASE(RIGHT_PAREN_TOKEN)
      Stop=TRUE;
      EXIT
    END_CASE
    
    OTHERWISE
      Parse_Error_Str("comma or right paren");
    END_CASE
  END_EXPECT

  Ok_To_Declare=Old_Ok;
  
  return(Stop);  
}

static void Parse_Write(void)
{
   char *temp;
   DATA_FILE *User_File;
   EXPRESS Express;
   int Terms;

   GET(LEFT_PAREN_TOKEN)
   GET(FILE_ID_TOKEN)
   
   User_File=(DATA_FILE *)Token.Data;

   Parse_Comma();
   
   EXPECT
     CASE4 (STRING_LITERAL_TOKEN,CHR_TOKEN,SUBSTR_TOKEN,STR_TOKEN)
     CASE4 (CONCAT_TOKEN,STRUPR_TOKEN,STRLWR_TOKEN,STRING_ID_TOKEN)
       UNGET
       temp=Parse_Formatted_String();
       fprintf(User_File->File,temp);
       POV_FREE(temp);
       if (Parse_Comma_RParen())
       {
         EXIT
       }
     END_CASE
     
     CASE_VECTOR
       Terms = Parse_Unknown_Vector (Express);
       switch (Terms)
       {
         case 1:
           fprintf(User_File->File,"%g",Express[X]);
           break;
           
         case 2:
           fprintf(User_File->File,"<%g,%g> ",Express[U],Express[V]);
           break;

         case 3:
           fprintf(User_File->File,"<%g,%g,%g> ",Express[X],Express[Y],Express[Z]);
           break;

         case 4:
           fprintf(User_File->File,"<%g,%g,%g,%g> ",Express[X],Express[Y],Express[Z],Express[T]);
           break;

         case 5:
           fprintf(User_File->File,"<%g,%g,%g,%g,%g> ",Express[X],Express[Y],Express[Z],Express[3],Express[4]);
           break;
         
         default:
           Parse_Error_Str("expression");
       }
       if (Parse_Comma_RParen())
       {
         EXIT
       }
     END_CASE
     
     OTHERWISE
       Parse_Error_Str("string or expression");
     END_CASE
   END_EXPECT
}

static DBL Parse_Cond_Param(void)
{
  int Old_Ok = Ok_To_Declare;
  int Old_Sk = Skipping;
  DBL Val;
  
  Ok_To_Declare = FALSE;
  Skipping      = FALSE;

  Val=Parse_Float_Param();

  Ok_To_Declare = Old_Ok;
  Skipping      = Old_Sk;
  
  return(Val);
}

static void Parse_Cond_Param2(DBL *V1,DBL *V2)
{
  int Old_Ok = Ok_To_Declare;
  int Old_Sk = Skipping;

  Ok_To_Declare = FALSE;
  Skipping      = FALSE;

  Parse_Float_Param2(V1,V2);

  Ok_To_Declare = Old_Ok;
  Skipping      = Old_Sk;
}

static void Inc_CS_Index(void)
{
  if (++CS_Index >= COND_STACK_SIZE)
  {
    Error("Too many nested conditionals or macros.\n");
  }
}

int Parse_Ifdef_Param (void)
{
  int Local_Index;

   GET(LEFT_PAREN_TOKEN)
   Inside_Ifdef=TRUE;
   Get_Token();
   strcpy(String2,String);
   Inside_Ifdef=FALSE;
   GET(RIGHT_PAREN_TOKEN)
 
   /* Search tables from newest to oldest */
   for (Local_Index=Table_Index; Local_Index > 0; Local_Index--)
   {
     if (Find_Symbol(Local_Index,String2) != NULL)
     {
       return(TRUE);
     }
   }

  return(FALSE);
}
