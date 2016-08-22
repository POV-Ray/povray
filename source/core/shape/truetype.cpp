//******************************************************************************
///
/// @file core/shape/truetype.cpp
///
/// Implementation of the TrueType-based text geometric primitive.
///
/// @author Alexander Enzmann
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
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
#include "core/shape/truetype.h"

#include "base/fileinputoutput.h"

#include "core/bounding/boundingbox.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/csg.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/* uncomment this to debug ttf. DEBUG1 gives less output than DEBUG2
#define TTF_DEBUG2 1
#define TTF_DEBUG 1
#define TTF_DEBUG3 1
*/

const DBL TTF_Tolerance = 1.0e-6;    /* -4 worked, -8 failed */

const int MAX_ITERATIONS = 50;
const DBL COEFF_LIMIT = 1.0e-20;

/* For decoding glyph coordinate bit flags */
const int ONCURVE            = 0x01;
const int XSHORT             = 0x02;
const int YSHORT             = 0x04;
const int REPEAT_FLAGS       = 0x08;  /* repeat flag n times */
const int SHORT_X_IS_POS     = 0x10;  /* the short vector is positive */
const int NEXT_X_IS_ZERO     = 0x10;  /* the relative x coordinate is zero */
const int SHORT_Y_IS_POS     = 0x20;  /* the short vector is positive */
const int NEXT_Y_IS_ZERO     = 0x20;  /* the relative y coordinate is zero */

/* For decoding multi-component glyph bit flags */
const int ARG_1_AND_2_ARE_WORDS    = 0x0001;
const int ARGS_ARE_XY_VALUES       = 0x0002;
const int ROUND_XY_TO_GRID         = 0x0004;
const int WE_HAVE_A_SCALE          = 0x0008;
/*      RESERVED                 = 0x0010 */
const int MORE_COMPONENTS          = 0x0020;
const int WE_HAVE_AN_X_AND_Y_SCALE = 0x0040;
const int WE_HAVE_A_TWO_BY_TWO     = 0x0080;
const int WE_HAVE_INSTRUCTIONS     = 0x0100;
const int USE_MY_METRICS           = 0x0200;

/* For decoding kern coverage bit flags */
const int KERN_HORIZONTAL    = 0x01;
const int KERN_MINIMUM       = 0x02;
const int KERN_CROSS_STREAM  = 0x04;
const int KERN_OVERRIDE      = 0x08;

/* Some marcos to make error detection easier, as well as clarify code */
#define READSHORT(fp) readSHORT(fp, __LINE__, __FILE__)
#define READLONG(fp) readLONG(fp, __LINE__, __FILE__)
#define READUSHORT(fp) readUSHORT(fp, __LINE__, __FILE__)
#define READULONG(fp) readULONG(fp, __LINE__, __FILE__)
#define READFIXED(fp) readLONG(fp, __LINE__, __FILE__)
#define READFWORD(fp) readSHORT(fp, __LINE__, __FILE__)
#define READUFWORD(fp) readUSHORT(fp, __LINE__, __FILE__)

/*****************************************************************************
* Local typedefs
******************************************************************************/

/* Type definitions to match the TTF spec, makes code clearer */
typedef POV_UINT8   BYTE;
typedef POV_INT16   SHORT;
typedef POV_UINT16  USHORT;
typedef POV_INT32   LONG;
typedef POV_UINT32  ULONG;
typedef POV_INT16   FWord;
typedef POV_UINT16  uFWord;

#if !defined(TARGET_OS_MAC)
typedef int Fixed;
#endif

typedef struct
{
    Fixed version;                /* 0x10000 (1.0) */
    USHORT numTables;             /* number of tables */
    USHORT searchRange;           /* (max2 <= numTables)*16 */
    USHORT entrySelector;         /* log2 (max2 <= numTables) */
    USHORT rangeShift;            /* numTables*16-searchRange */
} sfnt_OffsetTable;

typedef struct
{
    BYTE tag[4];
    ULONG checkSum;
    ULONG offset;
    ULONG length;
} sfnt_TableDirectory;

typedef sfnt_TableDirectory *sfnt_TableDirectoryPtr;

typedef struct
{
    ULONG bc;
    ULONG ad;
} longDateTime;

typedef struct
{
    Fixed version;                /* for this table, set to 1.0 */
    Fixed fontRevision;           /* For Font Manufacturer */
    ULONG checkSumAdjustment;
    ULONG magicNumber;            /* signature, must be 0x5F0F3CF5 == MAGIC */
    USHORT flags;
    USHORT unitsPerEm;            /* How many in Font Units per EM */

    longDateTime created;
    longDateTime modified;

    FWord xMin;                   /* Font wide bounding box in ideal space */
    FWord yMin;                   /* Baselines and metrics are NOT worked */
    FWord xMax;                   /* into these numbers) */
    FWord yMax;

    USHORT macStyle;              /* macintosh style word */
    USHORT lowestRecPPEM;         /* lowest recommended pixels per Em */

    SHORT fontDirectionHint;
    SHORT indexToLocFormat;       /* 0 - short offsets, 1 - long offsets */
    SHORT glyphDataFormat;
} sfnt_FontHeader;

typedef struct
{
    USHORT platformID;
    USHORT specificID;
    ULONG offset;
} sfnt_platformEntry;

typedef sfnt_platformEntry *sfnt_platformEntryPtr;

typedef struct
{
    USHORT format;
    USHORT length;
    USHORT version;
} sfnt_mappingTable;

typedef struct
{
    Fixed version;

    FWord Ascender;
    FWord Descender;
    FWord LineGap;

    uFWord advanceWidthMax;
    FWord minLeftSideBearing;
    FWord minRightSideBearing;
    FWord xMaxExtent;
    SHORT caretSlopeRise;
    SHORT caretSlopeRun;

    SHORT reserved1;
    SHORT reserved2;
    SHORT reserved3;
    SHORT reserved4;
    SHORT reserved5;

    SHORT metricDataFormat;
    USHORT numberOfHMetrics;      /* number of hMetrics in the hmtx table */
} sfnt_HorizHeader;

struct GlyphHeader
{
    SHORT numContours;
    SHORT xMin;
    SHORT yMin;
    SHORT xMax;
    SHORT yMax;
    GlyphHeader() : numContours(0), xMin(0), yMin(0), xMax(0), yMax(0) {}
};

struct GlyphOutline
{
    GlyphHeader header;
    USHORT numPoints;
    USHORT *endPoints;
    BYTE *flags;
    DBL *x, *y;
    USHORT myMetrics;

    GlyphOutline() :
        header(),
        numPoints(0),
        endPoints(NULL),
        flags(NULL),
        x(NULL), y(NULL),
        myMetrics(0)
    {}

    ~GlyphOutline()
    {
        if (endPoints != NULL) POV_FREE(endPoints);
        if (flags     != NULL) POV_FREE(flags);
        if (x         != NULL) POV_FREE(x);
        if (y         != NULL) POV_FREE(y);
    }
};

typedef struct
{
    BYTE inside_flag;             /* 1 if this an inside contour, 0 if outside */
    USHORT count;                 /* Number of points in the contour */
    BYTE *flags;                  /* On/off curve flags */
    DBL *x, *y;                   /* Coordinates of control vertices */
} Contour;


/* Contour information for a single glyph */
struct GlyphStruct
{
    GlyphHeader header;           /* Count and sizing information about this
                                   * glyph */
    USHORT glyph_index;           /* Internal glyph index for this character */
    Contour *contours;            /* Array of outline contours */
    USHORT unitsPerEm;            /* Max units character */
    USHORT myMetrics;             /* Which glyph index this is for metrics */
};

typedef struct KernData_struct
{
    USHORT left, right;           /* Glyph index of left/right to kern */
    FWord value;                  /* Delta in FUnits to apply in between */
} KernData;

/*
 * [esp] There's already a "KernTable" on the Mac... renamed to TTKernTable for
 * now in memorium to its author.
 */

typedef struct KernStruct
{
    USHORT coverage;              /* Coverage bit field of this subtable */
    USHORT nPairs;                /* # of kerning pairs in this table */
    KernData *kern_pairs;         /* Array of kerning values */
} TTKernTable;

typedef struct KernTableStruct
{
    USHORT nTables;               /* # of subtables in the kerning table */
    TTKernTable *tables;
} KernTables;

typedef struct longHorMertric
{
    uFWord advanceWidth;          /* Total width of a glyph in FUnits */
    FWord lsb;                    /* FUnits to the left of the glyph */
} longHorMetric;


typedef std::map<USHORT, GlyphPtr> GlyphPtrMap;

struct TrueTypeInfo
{
    TrueTypeInfo();
    ~TrueTypeInfo();

    USHORT platformID[4];             /* Character encoding search order */
    USHORT specificID[4];
    ULONG cmap_table_offset;          /* File locations for these tables */
    ULONG glyf_table_offset;
    USHORT numGlyphs;                 /* How many symbols in this file */
    USHORT unitsPerEm;                /* The "resoultion" of this font */
    SHORT indexToLocFormat;           /* 0 - short format, 1 - long format */
    ULONG *loca_table;                /* Mapping from characters to glyphs */
    GlyphPtrMap glyphsByChar;         /* Cached info for this font */
    GlyphPtrMap glyphsByIndex;        /* Cached info for this font */
    KernTables kerning_tables;        /* Kerning info for this font */
    USHORT numberOfHMetrics;          /* The number of explicit spacings */
    longHorMetric *hmtx_table;        /* Horizontal spacing info */
    ULONG glyphIDoffset;              /* Offset for Type 4 encoding tables */
    USHORT segCount, searchRange,     /* Counts for Type 4 encoding tables */
           entrySelector, rangeShift;
    USHORT *startCount, *endCount,    /* Type 4 (MS) encoding tables */
           *idDelta, *idRangeOffset;
};

/*****************************************************************************
* Local variables
******************************************************************************/

const BYTE tag_CharToIndexMap[] = "cmap"; /* 0x636d6170; */
const BYTE tag_FontHeader[]     = "head"; /* 0x68656164; */
const BYTE tag_GlyphData[]      = "glyf"; /* 0x676c7966; */
const BYTE tag_IndexToLoc[]     = "loca"; /* 0x6c6f6361; */
const BYTE tag_Kerning[]        = "kern"; /* 0x6b65726e; */
const BYTE tag_MaxProfile[]     = "maxp"; /* 0x6d617870; */
const BYTE tag_HorizHeader[]    = "hhea"; /* 0x68686561; */
const BYTE tag_HorizMetric[]    = "hmtx"; /* 0x686d7478; */
const BYTE tag_TTCFontFile[]    = "ttcf"; /* */

/*****************************************************************************
* Static functions
******************************************************************************/

/* Byte order independent I/O routines (probably already in other routines) */
SHORT readSHORT(IStream *infile, int line, const char *file);
USHORT readUSHORT(IStream *infile, int line, const char *file);
LONG readLONG(IStream *infile, int line, const char *file);
ULONG readULONG(IStream *infile, int line, const char *file);
int compare_tag4(BYTE *ttf_tag, BYTE *known_tag);

/* Internal TTF input routines */
void ProcessFontFile(TrueTypeFont* ffile);
void ProcessHeadTable(TrueTypeFont *ffile, int head_table_offset);
void ProcessLocaTable(TrueTypeFont *ffile, int loca_table_offset);
void ProcessMaxpTable(TrueTypeFont *ffile, int maxp_table_offset);
void ProcessKernTable(TrueTypeFont *ffile, int kern_table_offset);
void ProcessHheaTable(TrueTypeFont *ffile, int hhea_table_offset);
void ProcessHmtxTable(TrueTypeFont *ffile, int hmtx_table_offset);
GlyphPtr ProcessCharacter(TrueTypeFont *ffile, unsigned int search_char, unsigned int *glyph_index);
USHORT ProcessCharMap(TrueTypeFont *ffile, unsigned int search_char);
USHORT ProcessFormat0Glyph(TrueTypeFont *ffile, unsigned int search_char);
USHORT ProcessFormat4Glyph(TrueTypeFont *ffile, unsigned int search_char);
USHORT ProcessFormat6Glyph(TrueTypeFont *ffile, unsigned int search_char);
GlyphPtr ExtractGlyphInfo(TrueTypeFont *ffile, unsigned int glyph_index, unsigned int c);
GlyphOutline *ExtractGlyphOutline(TrueTypeFont *ffile, unsigned int glyph_index, unsigned int c);
GlyphPtr ConvertOutlineToGlyph(TrueTypeFont *ffile, const GlyphOutline *ttglyph);

SHORT readSHORT(IStream *infile, int line, const char *file)
{
    /// @impl
    /// @parblock

    /// On platforms using non-2's-complement representation for negative numbers, any approach
    /// starting with a signed interpretation of the data is doomed to fail over those
    /// representations' property of having two representations for zero, and none for -2^(N-1).
    /// We therefore must start off with an unsigned representation of the data.
    USHORT u = readUSHORT(infile, line, file);

    /// We go on by testing for a set signed bit, which in 2's complement format indicates that
    /// the actual value we're looking for is the unsigned interpretation minus 2^N.
    if (u | INTEGER16_SIGN_MASK)
        /// To compute that value, we first discard the sign bit (which is equivalent to subtracting
        /// 2^(N-1)), then add -2^(N-1). Finally we cram the result into the target type.
        return (SHORT(u ^ INTEGER16_SIGN_MASK)) + SIGNED16_MIN;
    else
        return SHORT(u);

    /// @endparblock
}

USHORT readUSHORT(IStream *infile, int line, const char *file)
{
    int i0, i1 = 0; /* To quiet warnings */

    if ((i0  = infile->Read_Byte ()) == EOF || (i1  = infile->Read_Byte ()) == EOF)
    {
        throw pov_base::Exception(__FUNCTION__, file, line, kFileDataErr, "Cannot read TrueType font file.");
    }

    return (USHORT)((((USHORT)i0) << 8) | ((USHORT)i1));
}

LONG readLONG(IStream *infile, int line, const char *file)
{
    /// @impl
    /// @parblock

    /// On platforms using non-2's-complement representation for negative numbers, any approach
    /// starting with a signed interpretation of the data is doomed to fail over those
    /// representations' property of having two representations for zero, and none for -2^(N-1).
    /// We therefore must start off with an unsigned representation of the data.
    ULONG u = readULONG(infile, line, file);

    /// We go on by testing for a set signed bit, which in 2's complement format indicates that
    /// the actual value we're looking for is the unsigned interpretation minus 2^N.
    if (u | INTEGER32_SIGN_MASK)
        /// To compute that value, we first discard the sign bit (which is equivalent to subtracting
        /// 2^(N-1)), then add -2^(N-1). Finally we cram the result into the target type.
        return (LONG(u ^ INTEGER32_SIGN_MASK)) + SIGNED32_MIN;
    else
        return LONG(u);

    /// @endparblock
}

ULONG readULONG(IStream *infile, int line, const char *file)
{
    int i0, i1 = 0, i2 = 0, i3 = 0;  /* To quiet warnings */

    if ((i0 = infile->Read_Byte ()) == EOF || (i1 = infile->Read_Byte ()) == EOF ||
        (i2 = infile->Read_Byte ()) == EOF || (i3 = infile->Read_Byte ()) == EOF)
    {
        throw pov_base::Exception(__FUNCTION__, file, line, kFileDataErr, "Cannot read TrueType font file.");
    }

    return (ULONG) ((((ULONG) i0) << 24) | (((ULONG) i1) << 16) |
                    (((ULONG) i2) << 8)  |  ((ULONG) i3));
}

static int compare_tag4(const BYTE *ttf_tag, const BYTE *known_tag)
{
    return (ttf_tag[0] == known_tag[0] && ttf_tag[1] == known_tag[1] &&
            ttf_tag[2] == known_tag[2] && ttf_tag[3] == known_tag[3]);
}

/*****************************************************************************
*
* FUNCTION
*
*   ProcessNewTTF
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Ennzmann
*
* DESCRIPTION
*
*   Takes an input string and a font filename, and creates a POV-Ray CSG
*   object for each letter in the string.
*
* CHANGES
*
*   Allow usage of built-in fonts via an additional parameter
*   (triggered when filename is null) - Oct 2012 [JG]
*
******************************************************************************/
void TrueType::ProcessNewTTF(CSG *Object, TrueTypeFont *ffile, const UCS2 *text_string, DBL depth, const Vector3d& offset, Parser *parser)
{
    Vector3d local_offset, total_offset;
    TrueType *ttf;
    DBL funit_size;
    TTKernTable *table;
    USHORT coverage;
    unsigned int search_char;
    unsigned int glyph_index, last_index = 0;
    FWord kern_value_x, kern_value_min_x;
    FWord kern_value_y, kern_value_min_y;
    int i, j, k;
    TRANSFORM Trans;

    /* Get info about each character in the string */
    total_offset = Vector3d(0.0, 0.0, 0.0);

    for (i = 0; text_string[i] != 0; i++)
    {
        /*
         * We need to make sure (for now) that this is only the lower 8 bits,
         * so we don't have all the high bits set if converted from a signed
         * char to an unsigned short.
         */
        search_char = (unsigned int)(text_string[i]);

#ifdef TTF_DEBUG
        Debug_Info("\nChar: '%c' (0x%X), Offset[%d]: <%g,%g,%g>\n", (char)search_char,
            search_char, i, total_offset[X], total_offset[Y], total_offset[Z]);
#endif

        /* Make a new child for each character */
        ttf = new TrueType();

        /* Set the depth information for the character */
        ttf->depth = depth;

        /*
         * Get pointers to the contour information for each character
         * in the text string.
         */
        ttf->glyph = ProcessCharacter(ffile, search_char, &glyph_index);
        funit_size = 1.0 / (DBL)(ffile->info->unitsPerEm);

        /*
         * Spacing based on the horizontal metric table, the kerning table,
         * and (possibly) the previous glyph.
         */
        if (i == 0) /* Ignore spacing on the left for the first character only */
        {
            /* Shift the glyph to start at the origin */
            total_offset[X] = -ttf->glyph->header.xMin * funit_size;

            Compute_Translation_Transform(&Trans, total_offset);

            ttf->Translate(total_offset, &Trans);

            /* Shift next glyph by the width of this one excluding the left offset*/
            total_offset[X] = (ffile->info->hmtx_table[ttf->glyph->myMetrics].advanceWidth -
                               ffile->info->hmtx_table[ttf->glyph->myMetrics].lsb) * funit_size;

#ifdef TTF_DEBUG
            Debug_Info("aw(%d): %g\n", i,
                               (ffile->info->hmtx_table[ttf->glyph->myMetrics].advanceWidth -
                                ffile->info->hmtx_table[ttf->glyph->myMetrics].lsb)*funit_size);
#endif
        }
        else /* Kern all of the other characters */
        {
            kern_value_x = kern_value_y = 0;
            kern_value_min_x = kern_value_min_y = -ffile->info->unitsPerEm;
            local_offset = Vector3d(0.0, 0.0, 0.0);

            for (j = 0; j < ffile->info->kerning_tables.nTables; j++)
            {
                table = ffile->info->kerning_tables.tables;
                coverage = table->coverage;

                /*
                 * Don't use vertical kerning until such a time when we support
                 * characters moving in the vertical direction...
                 */
                if (!(coverage & KERN_HORIZONTAL))
                    continue;

                /*
                 * If we were keen, we could do a binary search for this
                 * character combination, since the pairs are sorted in
                 * order as if the left and right index values were a 32 bit
                 * unsigned int (mostly - at least they are sorted on the
                 * left glyph).  Something to do when everything else works...
                 */
                for (k = 0; k < table[j].nPairs; k++)
                {
                    if (table[j].kern_pairs[k].left == last_index &&
                        table[j].kern_pairs[k].right == ttf->glyph->myMetrics)
                    {
#ifdef TTF_DEBUG2
                        Debug_Info("Found a kerning for <%d, %d> = %d\n",
                                   last_index, glyph_index, table[j].kern_pairs[k].value);
#endif

                        /*
                         * By default, Windows & OS/2 assume at most a single table with
                         * !KERN_MINIMUM, !KERN_CROSS_STREAM, KERN_OVERRIDE.
                         */
                        if (coverage & KERN_MINIMUM)
                        {
#ifdef TTF_DEBUG2
                            Debug_Info(" KERN_MINIMUM\n");
#endif
                            if (coverage & KERN_CROSS_STREAM)
                                kern_value_min_y = table[j].kern_pairs[k].value;
                            else
                                kern_value_min_x = table[j].kern_pairs[k].value;
                        }
                        else
                        {
                            if (coverage & KERN_CROSS_STREAM)
                            {
#ifdef TTF_DEBUG2
                                Debug_Info(" KERN_CROSS_STREAM\n");
#endif
                                if (table[j].kern_pairs[k].value == (FWord)0x8000)
                                {
                                    kern_value_y = 0;
                                }
                                else
                                {
                                    if (coverage & KERN_OVERRIDE)
                                        kern_value_y = table[j].kern_pairs[k].value;
                                    else
                                        kern_value_y += table[j].kern_pairs[k].value;
                                }
                            }
                            else
                            {
#ifdef TTF_DEBUG2
                                Debug_Info(" KERN_VALUE\n");
#endif
                                if (coverage & KERN_OVERRIDE)
                                    kern_value_x = table[j].kern_pairs[k].value;
                                else
                                    kern_value_x += table[j].kern_pairs[k].value;
                            }
                        }
                        break;
                    }
                    /* Abort now if we have passed all potential matches */
                    else if (table[j].kern_pairs[k].left > last_index)
                    {
                        break;
                    }
                }
            }
            kern_value_x = (kern_value_x > kern_value_min_x ?
                            kern_value_x : kern_value_min_x);
            kern_value_y = (kern_value_y > kern_value_min_y ?
                            kern_value_y : kern_value_min_y);

            /*
             * Offset this character so that the left edge of the glyph is at
             * the previous offset + the lsb + any kerning amount.
             */
            local_offset[X] = total_offset[X] +
                              (DBL)(ffile->info->hmtx_table[ttf->glyph->myMetrics].lsb -
                                    ttf->glyph->header.xMin + kern_value_x) * funit_size;
            local_offset[Y] = total_offset[Y] + (DBL)kern_value_y * funit_size;

            /* Translate this glyph to its final position in the string */
            Compute_Translation_Transform(&Trans, local_offset);

            ttf->Translate(local_offset, &Trans);

            /* Shift next glyph by the width of this one + any kerning amount */
            total_offset[X] += (ffile->info->hmtx_table[ttf->glyph->myMetrics].advanceWidth +kern_value_x) * funit_size;

#ifdef TTF_DEBUG
            Debug_Info("kern(%d): <%d, %d> (%g,%g)\n", i, last_index, glyph_index,
                       (DBL)kern_value_x*funit_size, (DBL)kern_value_y * funit_size);
            Debug_Info("lsb(%d): %g\n", i,
                       (DBL)ffile->info->hmtx_table[glyph->myMetrics].lsb * funit_size);
            Debug_Info("aw(%d): %g\n", i,
                       (DBL)ffile->info->hmtx_table[glyph->myMetrics].advanceWidth *
                       funit_size);
#endif
        }

        /*
         * Add to the offset of the next character the minimum spacing specified.
         */
        total_offset += offset;

        /* Link this glyph with the others in the union */
        Object->Type |= (ttf->Type & CHILDREN_FLAGS);
        ttf->Type |= IS_CHILD_OBJECT;
        Object->children.push_back(ttf);

        last_index = glyph_index;
    }

#ifdef TTF_DEBUG
    // TODO - text_string is an UCS2 strings, while Debug_Info will expect char strings.
    #error broken code
    if (filename)
    {
        Debug_Info("TTF parsing of \"%s\" from %s complete\n", text_string, filename);
    }
    else
    {
        Debug_Info("TTF parsing of \"%s\" from builtin %d complete\n", text_string, font_id);
    }
#endif

    /* Close the font file descriptor */
    if(ffile->fp!=NULL)
    {
        delete ffile->fp;
        ffile->fp = NULL;
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   ProcessFontFile
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Ennzmann
*
* DESCRIPTION
*
* Read the header information about the specific font.  Parse the tables
* as we come across them.
*
* CHANGES
*
*   Added tests for reading manditory tables/validity checks - Jan 1996 [AED]
*   Reordered table parsing to avoid lots of file seeking - Jan 1996 [AED]
*
*   Added builtin fonts when fontfilename is nullptr - Oct 2012 [JG]
*
******************************************************************************/
void ProcessFontFile(TrueTypeFont* ffile)
{
    unsigned i;
    int head_table_offset = 0;
    int loca_table_offset = 0;
    int maxp_table_offset = 0;
    int kern_table_offset = 0;
    int hhea_table_offset = 0;
    int hmtx_table_offset = 0;
    BYTE temp_tag[4];
    sfnt_OffsetTable OffsetTable;
    sfnt_TableDirectory Table;

    /* We have already read all the header info, no need to do it again */

    if (ffile->info != NULL)
        return;

    ffile->info = new TrueTypeInfo;

    /*
        * For Microsoft encodings 3, 1 is for Unicode
        *                         3, 0 is for Non-Unicode (ie symbols)
        * For Macintosh encodings 1, 0 is for Roman character set
        * For Unicode encodings   0, 3 is for Unicode
        */
    switch(ffile->textEncoding)
    {
        case kStringEncoding_ASCII:
            // first choice
            ffile->info->platformID[0] = 1;
            ffile->info->specificID[0] = 0;
            // second choice
            ffile->info->platformID[1] = 3;
            ffile->info->specificID[1] = 1;
            // third choice
            ffile->info->platformID[2] = 0;
            ffile->info->specificID[2] = 3;
            // fourth choice
            ffile->info->platformID[3] = 3;
            ffile->info->specificID[3] = 0;
            break;
        case kStringEncoding_UTF8:
        case kStringEncoding_System:
            // first choice
            ffile->info->platformID[0] = 0;
            ffile->info->specificID[0] = 3;
            // second choice
            ffile->info->platformID[1] = 3;
            ffile->info->specificID[1] = 1;
            // third choice
            ffile->info->platformID[2] = 1;
            ffile->info->specificID[2] = 0;
            // fourth choice
            ffile->info->platformID[3] = 3;
            ffile->info->specificID[3] = 0;
            break;
    }

    /*
     * Read the initial directory header on the TTF.  The numTables variable
     * tells us how many tables are present in this file.
     */
    /// @compat
    /// This piece of code relies on BYTE having the same size as char.
    if (!ffile->fp->read(reinterpret_cast<char *>(&temp_tag), sizeof(BYTE) * 4))
    {
        throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file table tag");
    }
    if (compare_tag4(temp_tag, tag_TTCFontFile))
    {
        READFIXED(ffile->fp); // header version - ignored [trf]
        READULONG(ffile->fp); // directory count - ignored [trf]
        // go to first font data block listed in the directory table entry [trf]
        ffile->fp->seekg(READULONG(ffile->fp), IOBase::seek_set);
    }
    else
    {
        // if it is no TTC style file, it is a regular TTF style file
        ffile->fp->seekg(0, IOBase::seek_set);
    }

    OffsetTable.version = READFIXED(ffile->fp);
    OffsetTable.numTables = READUSHORT(ffile->fp);
    OffsetTable.searchRange = READUSHORT(ffile->fp);
    OffsetTable.entrySelector = READUSHORT(ffile->fp);
    OffsetTable.rangeShift = READUSHORT(ffile->fp);

#ifdef TTF_DEBUG
    Debug_Info("OffsetTable:\n");
    Debug_Info("version=%d\n", OffsetTable.version);
    Debug_Info("numTables=%u\n", OffsetTable.numTables);
    Debug_Info("searchRange=%u\n", OffsetTable.searchRange);
    Debug_Info("entrySelector=%u\n", OffsetTable.entrySelector);
    Debug_Info("rangeShift=%u\n", OffsetTable.rangeShift);
#endif

    /*
     * I don't know why we limit this to 40 tables, since the spec says there
     * can be any number, but that's how it was when I got it.  Added a warning
     * just in case it ever happens in real life. [AED]
     */
    if (OffsetTable.numTables > 40)
    {
// TODO MESSAGE    Warning("More than 40 (%d) TTF Tables in %s - some info may be lost!",
//            OffsetTable.numTables, ffile->filename);
    }

    /* Process general font information and save it. */

    for (i = 0; i < OffsetTable.numTables && i < 40; i++)
    {
        /// @compat
        /// This piece of code relies on BYTE having the same size as char.
        if (!ffile->fp->read(reinterpret_cast<char *>(&Table.tag), sizeof(BYTE) * 4))
        {
            throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file table tag");
        }
        Table.checkSum = READULONG(ffile->fp);
        Table.offset   = READULONG(ffile->fp);
        Table.length   = READULONG(ffile->fp);

#ifdef TTF_DEBUG
        Debug_Info("\nTable %d:\n",i);
        Debug_Info("tag=%c%c%c%c\n", Table.tag[0], Table.tag[1],
                                     Table.tag[2], Table.tag[3]);
        Debug_Info("checkSum=%u\n", Table.checkSum);
        Debug_Info("offset=%u\n", Table.offset);
        Debug_Info("length=%u\n", Table.length);
#endif

        if (compare_tag4(Table.tag, tag_CharToIndexMap))
            ffile->info->cmap_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_GlyphData))
            ffile->info->glyf_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_FontHeader))
            head_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_IndexToLoc))
            loca_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_MaxProfile))
            maxp_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_Kerning))
            kern_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_HorizHeader))
            hhea_table_offset = Table.offset;
        else if (compare_tag4(Table.tag, tag_HorizMetric))
            hmtx_table_offset = Table.offset;
    }

    if (ffile->info->cmap_table_offset == 0 || ffile->info->glyf_table_offset == 0 ||
        head_table_offset == 0 || loca_table_offset == 0 ||
        hhea_table_offset == 0 || hmtx_table_offset == 0 ||
        maxp_table_offset == 0)
    {
// TODO MESSAGE    throw POV_EXCEPTION(kFileDataErr, "Invalid TrueType font headers in %s", ffile->filename);
    }

    ProcessHeadTable(ffile, head_table_offset);  /* Need indexToLocFormat */
    if ((ffile->info->indexToLocFormat != 0 && ffile->info->indexToLocFormat != 1) ||
        (ffile->info->unitsPerEm < 16 || ffile->info->unitsPerEm > 16384))
;// TODO MESSAGE    Error("Invalid TrueType font data in %s", ffile->filename);

    ProcessMaxpTable(ffile, maxp_table_offset);  /* Need numGlyphs */
    if (ffile->info->numGlyphs <= 0)
;// TODO MESSAGE    Error("Invalid TrueType font data in %s", ffile->filename);

    ProcessLocaTable(ffile, loca_table_offset);  /* Now we can do loca_table */

    ProcessHheaTable(ffile, hhea_table_offset);  /* Need numberOfHMetrics */
    if (ffile->info->numberOfHMetrics <= 0)
;// TODO MESSAGE    Error("Invalid TrueType font data in %s", ffile->filename);

    ProcessHmtxTable(ffile, hmtx_table_offset);  /* Now we can read HMetrics */

    if (kern_table_offset != 0)
        ProcessKernTable(ffile, kern_table_offset);
}

/* Process the font header table */
void ProcessHeadTable(TrueTypeFont *ffile, int head_table_offset)
{
    sfnt_FontHeader fontHeader;

    /* Read head table */
    ffile->fp->seekg(head_table_offset);

    fontHeader.version = READFIXED(ffile->fp);
    fontHeader.fontRevision = READFIXED(ffile->fp);
    fontHeader.checkSumAdjustment = READULONG(ffile->fp);
    fontHeader.magicNumber = READULONG(ffile->fp);   /* should be 0x5F0F3CF5 */
    fontHeader.flags = READUSHORT(ffile->fp);
    fontHeader.unitsPerEm = READUSHORT(ffile->fp);
    fontHeader.created.bc = READULONG(ffile->fp);
    fontHeader.created.ad = READULONG(ffile->fp);
    fontHeader.modified.bc = READULONG(ffile->fp);
    fontHeader.modified.ad = READULONG(ffile->fp);
    fontHeader.xMin = READFWORD(ffile->fp);
    fontHeader.yMin = READFWORD(ffile->fp);
    fontHeader.xMax = READFWORD(ffile->fp);
    fontHeader.yMax = READFWORD(ffile->fp);
    fontHeader.macStyle = READUSHORT(ffile->fp);
    fontHeader.lowestRecPPEM = READUSHORT(ffile->fp);
    fontHeader.fontDirectionHint = READSHORT(ffile->fp);
    fontHeader.indexToLocFormat = READSHORT(ffile->fp);
    fontHeader.glyphDataFormat = READSHORT(ffile->fp);

#ifdef TTF_DEBUG
    Debug_Info("\nfontHeader:\n");
    Debug_Info("version: %d\n",fontHeader.version);
    Debug_Info("fontRevision: %d\n",fontHeader.fontRevision);
    Debug_Info("checkSumAdjustment: %u\n",fontHeader.checkSumAdjustment);
    Debug_Info("magicNumber: 0x%8X\n",fontHeader.magicNumber);
    Debug_Info("flags: %u\n",fontHeader.flags);
    Debug_Info("unitsPerEm: %u\n",fontHeader.unitsPerEm);
    Debug_Info("created.bc: %u\n",fontHeader.created.bc);
    Debug_Info("created.ad: %u\n",fontHeader.created.ad);
    Debug_Info("modified.bc: %u\n",fontHeader.modified.bc);
    Debug_Info("modified.ad: %u\n",fontHeader.modified.ad);
    Debug_Info("xMin: %d\n",fontHeader.xMin);
    Debug_Info("yMin: %d\n",fontHeader.yMin);
    Debug_Info("xMax: %d\n",fontHeader.xMax);
    Debug_Info("yMax: %d\n",fontHeader.yMax);
    Debug_Info("macStyle: %u\n",fontHeader.macStyle);
    Debug_Info("lowestRecPPEM: %u\n",fontHeader.lowestRecPPEM);
    Debug_Info("fontDirectionHint: %d\n",fontHeader.fontDirectionHint);
    Debug_Info("indexToLocFormat: %d\n",fontHeader.indexToLocFormat);
    Debug_Info("glyphDataFormat: %d\n",fontHeader.glyphDataFormat);
#endif

    if (fontHeader.magicNumber != 0x5F0F3CF5)
    {
        throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font.");
    }

    ffile->info->indexToLocFormat = fontHeader.indexToLocFormat;
    ffile->info->unitsPerEm = fontHeader.unitsPerEm;
}

/* Determine the relative offsets of glyphs */
void ProcessLocaTable(TrueTypeFont *ffile, int loca_table_offset)
{
    int i;

    /* Move to location of table in file */
    ffile->fp->seekg(loca_table_offset);

    ffile->info->loca_table = new ULONG[ffile->info->numGlyphs+1];

#ifdef TTF_DEBUG
    Debug_Info("\nlocation table:\n");
    Debug_Info("version: %s\n",(ffile->info->indexToLocFormat?"long":"short"));
#endif

    /* Now read and save the location table */

    if (ffile->info->indexToLocFormat == 0)                  /* short version */
    {
        for (i = 0; i < ffile->info->numGlyphs; i++)
        {
            ffile->info->loca_table[i] = ((ULONG)READUSHORT(ffile->fp)) << 1;
#ifdef TTF_DEBUG2
            Debug_Info("loca_table[%d] @ %u\n", i, ffile->info->loca_table[i]);
#endif
        }
    }
    else                                               /* long version */
    {
        for (i = 0; i < ffile->info->numGlyphs; i++)
        {
            ffile->info->loca_table[i] = READULONG(ffile->fp);
#ifdef TTF_DEBUG2
            Debug_Info("loca_table[%d] @ %u\n", i, ffile->info->loca_table[i]);
#endif
        }
    }
}


/*
 * This routine determines the total number of glyphs in a TrueType file.
 * Necessary so that we can allocate the proper amount of storage for the glyph
 * location table.
 */
void ProcessMaxpTable(TrueTypeFont *ffile, int maxp_table_offset)
{
    /* seekg to the maxp table, skipping the 4 byte version number */
    ffile->fp->seekg(maxp_table_offset + 4);

    ffile->info->numGlyphs = READUSHORT(ffile->fp);

#ifdef TTF_DEBUG
    Debug_Info("\nmaximum profile table:\n");
    Debug_Info("numGlyphs: %u\n", ffile->info->numGlyphs);
#endif
}


/* Read the kerning information for a glyph */
void ProcessKernTable(TrueTypeFont *ffile, int kern_table_offset)
{
    int i, j;
    USHORT temp16;
    USHORT length;
    KernTables *kern_table;

    kern_table = &ffile->info->kerning_tables;

    /* Move to the beginning of the kerning table, skipping the 2 byte version */
    ffile->fp->seekg(kern_table_offset + 2);

    /* Read in the number of kerning tables */

    kern_table->nTables = READUSHORT(ffile->fp);
    kern_table->tables = NULL;      /*<==[esp] added (in case nTables is zero)*/

#ifdef TTF_DEBUG
    Debug_Info("\nKerning table:\n", kern_table_offset);
    Debug_Info("Offset: %d\n", kern_table_offset);
    Debug_Info("Number of tables: %u\n",kern_table->nTables);
#endif

    /* Don't do any more work if there isn't kerning info */

    if (kern_table->nTables == 0)
        return;

    kern_table->tables = new TTKernTable[kern_table->nTables];

    for (i = 0; i < kern_table->nTables; i++)
    {
        /* Read in a subtable */

        temp16 = READUSHORT(ffile->fp);                      /* Subtable version */
        length = READUSHORT(ffile->fp);                       /* Subtable length */
        kern_table->tables[i].coverage = READUSHORT(ffile->fp); /* Coverage bits */

#ifdef TTF_DEBUG
        Debug_Info("Coverage table[%d] (0x%X):", i, kern_table->tables[i].coverage);
        Debug_Info("  type %u", (kern_table->tables[i].coverage >> 8));
        Debug_Info(" %s", (kern_table->tables[i].coverage & KERN_HORIZONTAL ?
                             "Horizontal" : "Vertical" ));
        Debug_Info(" %s values", (kern_table->tables[i].coverage & KERN_MINIMUM ?
                             "Minimum" : "Kerning" ));
        Debug_Info("%s", (kern_table->tables[i].coverage & KERN_CROSS_STREAM ?
                             " Cross-stream" : "" ));
        Debug_Info("%s\n", (kern_table->tables[i].coverage & KERN_OVERRIDE ?
                             " Override" : "" ));
#endif

        kern_table->tables[i].kern_pairs = NULL;   /*<==[esp] added*/
        kern_table->tables[i].nPairs = 0;          /*<==[esp] added*/

        if ((kern_table->tables[i].coverage >> 8) == 0)
        {
            /* Can only handle format 0 kerning subtables */
            kern_table->tables[i].nPairs = READUSHORT(ffile->fp);

#ifdef TTF_DEBUG
            Debug_Info("entries in table[%d]: %d\n", i, kern_table->tables[i].nPairs);
#endif

            temp16 = READUSHORT(ffile->fp);     /* searchRange */
            temp16 = READUSHORT(ffile->fp);     /* entrySelector */
            temp16 = READUSHORT(ffile->fp);     /* rangeShift */

            kern_table->tables[i].kern_pairs = new KernData[kern_table->tables[i].nPairs];

            for (j = 0; j < kern_table->tables[i].nPairs; j++)
            {
                /* Read in a kerning pair */
                kern_table->tables[i].kern_pairs[j].left = READUSHORT(ffile->fp);
                kern_table->tables[i].kern_pairs[j].right = READUSHORT(ffile->fp);
                kern_table->tables[i].kern_pairs[j].value = READFWORD(ffile->fp);

#ifdef TTF_DEBUG2
                Debug_Info("Kern pair: <%d,%d> = %d\n",
                           (int)kern_table->tables[i].kern_pairs[j].left,
                           (int)kern_table->tables[i].kern_pairs[j].right,
                           (int)kern_table->tables[i].kern_pairs[j].value);
#endif
            }
        }
        else
        {
#ifdef TTF_DEBUG2
            Warning("Cannot handle format %u kerning data",
                    (kern_table->tables[i].coverage >> 8));
#endif
            /*
             * seekg to the end of this table, excluding the length of the version,
             * length, and coverage USHORTs, which we have already read.
             */
            ffile->fp->seekg((int)(length - 6), IOBase::seek_cur);
            kern_table->tables[i].nPairs = 0;
        }
    }
}

/*
 * This routine determines the total number of horizontal metrics.
 */
void ProcessHheaTable(TrueTypeFont *ffile, int hhea_table_offset)
{
#ifdef TTF_DEBUG
    sfnt_HorizHeader horizHeader;

    /* seekg to the hhea table */
    ffile->fp->seekg(hhea_table_offset);

    horizHeader.version = READFIXED(ffile->fp);
    horizHeader.Ascender = READFWORD(ffile->fp);
    horizHeader.Descender = READFWORD(ffile->fp);
    horizHeader.LineGap = READFWORD(ffile->fp);
    horizHeader.advanceWidthMax = READUFWORD(ffile->fp);
    horizHeader.minLeftSideBearing = READFWORD(ffile->fp);
    horizHeader.minRightSideBearing = READFWORD(ffile->fp);
    horizHeader.xMaxExtent = READFWORD(ffile->fp);
    horizHeader.caretSlopeRise = READSHORT(ffile->fp);
    horizHeader.caretSlopeRun = READSHORT(ffile->fp);
    horizHeader.reserved1 = READSHORT(ffile->fp);
    horizHeader.reserved2 = READSHORT(ffile->fp);
    horizHeader.reserved3 = READSHORT(ffile->fp);
    horizHeader.reserved4 = READSHORT(ffile->fp);
    horizHeader.reserved5 = READSHORT(ffile->fp);
    horizHeader.metricDataFormat = READSHORT(ffile->fp);
#else

    /* seekg to the hhea table, skipping all that stuff we don't need */
    ffile->fp->seekg (hhea_table_offset + 34);

#endif

    ffile->info->numberOfHMetrics = READUSHORT(ffile->fp);

#ifdef TTF_DEBUG
    Debug_Info("\nhorizontal header table:\n");
    Debug_Info("Ascender: %d\n",horizHeader.Ascender);
    Debug_Info("Descender: %d\n",horizHeader.Descender);
    Debug_Info("LineGap: %d\n",horizHeader.LineGap);
    Debug_Info("advanceWidthMax: %d\n",horizHeader.advanceWidthMax);
    Debug_Info("minLeftSideBearing: %d\n",horizHeader.minLeftSideBearing);
    Debug_Info("minRightSideBearing: %d\n",horizHeader.minRightSideBearing);
    Debug_Info("xMaxExtent: %d\n",horizHeader.xMaxExtent);
    Debug_Info("caretSlopeRise: %d\n",horizHeader.caretSlopeRise);
    Debug_Info("caretSlopeRun: %d\n",horizHeader.caretSlopeRun);
    Debug_Info("metricDataFormat: %d\n",horizHeader.metricDataFormat);
    Debug_Info("numberOfHMetrics: %d\n",ffile->numberOfHMetrics);
#endif
}

void ProcessHmtxTable (TrueTypeFont *ffile, int hmtx_table_offset)
{
    int i;
    longHorMetric *metric;
    uFWord lastAW = 0;     /* Just to quiet warnings. */

    ffile->fp->seekg (hmtx_table_offset);

    ffile->info->hmtx_table = new longHorMetric[ffile->info->numGlyphs];

    /*
     * Read in the total glyph width, and the left side offset.  There is
     * guaranteed to be at least one longHorMetric entry in this table to
     * set the advanceWidth for the subsequent lsb entries.
     */
    for (i=0, metric=ffile->info->hmtx_table; i < ffile->info->numberOfHMetrics; i++,metric++)
    {
        lastAW = metric->advanceWidth = READUFWORD(ffile->fp);
        metric->lsb = READFWORD(ffile->fp);
    }

    /* Read in the remaining left offsets */
    for (; i < ffile->info->numGlyphs; i++, metric++)
    {
        metric->advanceWidth = lastAW;
        metric->lsb = READFWORD(ffile->fp);
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   ProcessCharacter
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Finds the glyph description for the current character.
*
* CHANGES
*
*   -
*
******************************************************************************/

GlyphPtr ProcessCharacter(TrueTypeFont *ffile, unsigned int search_char, unsigned int *glyph_index)
{
    GlyphPtrMap::iterator iGlyph;

    /* See if we have already processed this glyph */
    iGlyph = ffile->info->glyphsByChar.find(search_char);
    if (iGlyph != ffile->info->glyphsByChar.end())
    {
        /* Found it, no need to do any more work */
#ifdef TTF_DEBUG
        Debug_Info("Cached glyph: %c/%u\n",(char)search_char,(*iGlyph).second->glyph_index);
#endif
        *glyph_index = (*iGlyph).second->glyph_index;
        return (*iGlyph).second;
    }

    *glyph_index = ProcessCharMap(ffile, search_char);
    if (*glyph_index == 0)
;// TODO MESSAGE    Warning("Character %d (0x%X) not found in %s", (BYTE)search_char,
//            search_char, ffile->filename);

    /* See if we have already processed this glyph (using the glyph index) */
    iGlyph = ffile->info->glyphsByIndex.find(*glyph_index);
    if (iGlyph != ffile->info->glyphsByIndex.end())
    {
        /* Found it, no need to do any more work (except remember the char-to-glyph mapping for next time) */
        ffile->info->glyphsByChar[search_char] = (*iGlyph).second;
#ifdef TTF_DEBUG
        Debug_Info("Cached glyph: %c/%u\n",(char)search_char,(*iGlyph).second->glyph_index);
#endif
        *glyph_index = (*iGlyph).second->glyph_index;
        return (*iGlyph).second;
    }

    GlyphPtr glyph = ExtractGlyphInfo(ffile, *glyph_index, search_char);

    /* Add this glyph to the ones we already know about */

    ffile->info->glyphsByChar[search_char] = glyph;
    ffile->info->glyphsByIndex[*glyph_index] = glyph;

    /* Glyph is all built */

    return glyph;
}

/*****************************************************************************
*
* FUNCTION
*
*   ProcessCharMap
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Find the character mapping for 'search_char'.  We should really know
*   which character set we are using (ie ISO 8859-1, Mac, Unicode, etc).
*   Search char should really be a USHORT to handle double byte systems.
*
* CHANGES
*
*   961120  esp  Added check to allow Macintosh encodings to pass
*
******************************************************************************/
USHORT ProcessCharMap(TrueTypeFont *ffile, unsigned int search_char)
{
    int initial_table_offset;
    int old_table_offset;
    int entry_offset;
    sfnt_platformEntry cmapEntry;
    sfnt_mappingTable encodingTable;
    int i, j, table_count;

    /* Move to the start of the character map, skipping the 2 byte version */
    ffile->fp->seekg (ffile->info->cmap_table_offset + 2);

    table_count = READUSHORT(ffile->fp);

    #ifdef TTF_DEBUG
    Debug_Info("table_count=%d\n", table_count);
    #endif

    /*
     * Search the tables until we find the glyph index for the search character.
     * Just return the first one we find...
     */

    initial_table_offset = ffile->fp->tellg (); /* Save the initial position */

    for(j = 0; j <= 3; j++)
    {
        ffile->fp->seekg(initial_table_offset); /* Always start new search at the initial position */

        for (i = 0; i < table_count; i++)
        {
            cmapEntry.platformID = READUSHORT(ffile->fp);
            cmapEntry.specificID = READUSHORT(ffile->fp);
            cmapEntry.offset     = READULONG(ffile->fp);

            #ifdef TTF_DEBUG
            Debug_Info("cmapEntry: platformID=%d\n", cmapEntry.platformID);
            Debug_Info("cmapEntry: specificID=%d\n", cmapEntry.specificID);
            Debug_Info("cmapEntry: offset=%d\n", cmapEntry.offset);
            #endif

            /*
             * Check if this is the encoding table we want to use.
             * The search is done according to user preference.
             */
            if ( ffile->info->platformID[j] != cmapEntry.platformID ) /* [JAC 01/99] */
            {
                continue;
            }

            entry_offset = cmapEntry.offset;

            old_table_offset = ffile->fp->tellg (); /* Save the current position */

            ffile->fp->seekg (ffile->info->cmap_table_offset + entry_offset);

            encodingTable.format = READUSHORT(ffile->fp);
            encodingTable.length = READUSHORT(ffile->fp);
            encodingTable.version = READUSHORT(ffile->fp);

            #ifdef TTF_DEBUG
            Debug_Info("Encoding table, format: %u, length: %u, version: %u\n",
                       encodingTable.format, encodingTable.length, encodingTable.version);
            #endif

            if (encodingTable.format == 0)
            {
                /*
                 * Translation is simple - add 'entry_char' to the start of the
                 * table and grab what's there.
                 */
                #ifdef TTF_DEBUG
                Debug_Info("Apple standard index mapping\n");
                #endif

                return(ProcessFormat0Glyph(ffile, search_char));
            }
            #if 0  /* Want to get the rest of these working first */
            else if (encodingTable.format == 2)
            {
                /* Used for multi-byte character encoding (Chinese, Japanese, etc) */
                #ifdef TTF_DEBUG
                Debug_Info("High-byte index mapping\n");
                #endif

                return(ProcessFormat2Glyph(ffile, search_char));
            }
            #endif
            else if (encodingTable.format == 4)
            {
                /* Microsoft UGL encoding */
                #ifdef TTF_DEBUG
                Debug_Info("Microsoft standard index mapping\n");
                #endif

                return(ProcessFormat4Glyph(ffile, search_char));
            }
            else if (encodingTable.format == 6)
            {
                #ifdef TTF_DEBUG
                Debug_Info("Trimmed table mapping\n");
                #endif

                return(ProcessFormat6Glyph(ffile, search_char));
            }
            #ifdef TTF_DEBUG
            else
                Debug_Info("Unsupported TrueType font index mapping format: %u\n",
                           encodingTable.format);
            #endif

            /* Go to the next table entry if we didn't find a match */
            ffile->fp->seekg (old_table_offset);
        }
    }

    /*
     * No character mapping was found - very odd, we should really have had the
     * character in at least one table.  Perhaps getting here means we didn't
     * have any character mapping tables.  '0' means no mapping.
     */

    return 0;
}


/*****************************************************************************
*
* FUNCTION
*
*   ProcessFormat0Glyph
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* This handles the Apple standard index mapping for glyphs.
* The file pointer must be pointing immediately after the version entry in the
* encoding table for the next two functions to work.
*
* CHANGES
*
*   -
*
******************************************************************************/
USHORT ProcessFormat0Glyph(TrueTypeFont *ffile, unsigned int search_char)
{
    BYTE temp_index;

    ffile->fp->seekg ((int)search_char, IOBase::seek_cur);

    /// @compat
    /// This piece of code relies on BYTE having the same size as char.
    if (!ffile->fp->read (reinterpret_cast<char *>(&temp_index), 1)) /* Each index is 1 byte */
    {
        throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file.");
    }

    return (USHORT)(temp_index);
}

/*****************************************************************************
*
* FUNCTION
*
*   ProcessFormat4Glyph
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* This handles the Microsoft standard index mapping for glyph tables
*
* CHANGES
*
*   Mar 26, 1996: Cache segment info rather than read each time.  [AED]
*
******************************************************************************/
USHORT ProcessFormat4Glyph(TrueTypeFont *ffile, unsigned int search_char)
{
    int i;
    unsigned int glyph_index = 0;  /* Set the glyph index to "not present" */

    /*
     * If this is the first time we are here, read all of the segment headers,
     * and save them for later calls to this function, rather than seeking and
     * mallocing for each character
     */
    if (ffile->info->segCount == 0)
    {
        USHORT temp16;

        ffile->info->segCount = READUSHORT(ffile->fp) >> 1;
        ffile->info->searchRange = READUSHORT(ffile->fp);
        ffile->info->entrySelector = READUSHORT(ffile->fp);
        ffile->info->rangeShift = READUSHORT(ffile->fp);

        /* Now allocate and read in the segment arrays */

        ffile->info->endCount = new USHORT[ffile->info->segCount];
        ffile->info->startCount = new USHORT[ffile->info->segCount];
        ffile->info->idDelta = new USHORT[ffile->info->segCount];
        ffile->info->idRangeOffset = new USHORT[ffile->info->segCount];

        for (i = 0; i < ffile->info->segCount; i++)
        {
            ffile->info->endCount[i] = READUSHORT(ffile->fp);
        }

        temp16 = READUSHORT(ffile->fp);  /* Skip over 'reservedPad' */

        for (i = 0; i < ffile->info->segCount; i++)
        {
            ffile->info->startCount[i] = READUSHORT(ffile->fp);
        }

        for (i = 0; i < ffile->info->segCount; i++)
        {
            ffile->info->idDelta[i] = READUSHORT(ffile->fp);
        }

        /* location of start of idRangeOffset */
        ffile->info->glyphIDoffset = ffile->fp->tellg ();

        for (i = 0; i < ffile->info->segCount; i++)
        {
            ffile->info->idRangeOffset[i] = READUSHORT(ffile->fp);
        }
    }

    /* Search the segments for our character */

glyph_search:
    for (i = 0; i < ffile->info->segCount; i++)
    {
        if (search_char <= ffile->info->endCount[i])
        {
            if (search_char >= ffile->info->startCount[i])
            {
                /* Found correct range for this character */

                if (ffile->info->idRangeOffset[i] == 0)
                {
                    glyph_index = search_char + ffile->info->idDelta[i];
                }
                else
                {
                    /*
                     * Alternate encoding of glyph indices, relies on a quite unusual way
                     * of storing the offsets.  We need the *2s because we are talking
                     * about addresses of shorts and not bytes.
                     *
                     * (glyphIDoffset + i*2 + idRangeOffset[i]) == &idRangeOffset[i]
                     */
                    ffile->fp->seekg (ffile->info->glyphIDoffset + 2*i + ffile->info->idRangeOffset[i]+
                                     2*(search_char - ffile->info->startCount[i]));

                    glyph_index = READUSHORT(ffile->fp);

                    if (glyph_index != 0)
                        glyph_index = glyph_index + ffile->info->idDelta[i];
                }
            }
            break;
        }
    }

    /*
     * If we haven't found the character yet, and this is the first time to
     * search the tables, try looking in the Unicode user space, since this
     * is the location Microsoft recommends for symbol characters like those
     * in wingdings and dingbats.
     */
    if (glyph_index == 0 && search_char < 0x100)
    {
        search_char += 0xF000;
#ifdef TTF_DEBUG
        Debug_Info("Looking for glyph in Unicode user space (0x%X)\n", search_char);
#endif
        goto glyph_search;
    }

    /* Deallocate the memory we used for the segment arrays */

    return glyph_index;
}

/*****************************************************************************
*
* FUNCTION
*
*   ProcessFormat6Glyph
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*  This handles the trimmed table mapping for glyphs.
*
* CHANGES
*
*   -
*
******************************************************************************/
USHORT ProcessFormat6Glyph(TrueTypeFont *ffile, unsigned int search_char)
{
    USHORT firstCode, entryCount;
    USHORT glyph_index;

    firstCode = READUSHORT(ffile->fp);
    entryCount = READUSHORT(ffile->fp);

    if (search_char >= firstCode && search_char < firstCode + entryCount)
    {
        ffile->fp->seekg (((int)(search_char - firstCode))*2, IOBase::seek_cur);
        glyph_index = READUSHORT(ffile->fp);
    }
    else
        glyph_index = 0;

    return glyph_index;
}


/*****************************************************************************
*
* FUNCTION
*
*   ExtractGlyphInfo
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Change TTF outline information for the glyph(s) into a useful format
*
* CHANGES
*
*   -
*
******************************************************************************/
GlyphPtr ExtractGlyphInfo(TrueTypeFont *ffile, unsigned int glyph_index, unsigned int c)
{
    GlyphOutline *ttglyph;
    GlyphPtr glyph;

    ttglyph = ExtractGlyphOutline(ffile, glyph_index, c);

    /*
     * Convert the glyph outline information from TrueType layout into a more
     * easily processed format
     */

    glyph = ConvertOutlineToGlyph(ffile, ttglyph);
    glyph->glyph_index = glyph_index;
    glyph->myMetrics = ttglyph->myMetrics;

    /* Free up outline information */

    if (ttglyph)
        delete ttglyph;

#ifdef TTF_DEBUG3
    int i, j;

    Debug_Info("// Character '%c'\n", (char)c);

    for(i = 0; i < (int)glyph->header.numContours; i++)
    {
        Debug_Info("BYTE gGlypthFlags_%c_%d[] = \n", (char)c, i);
        Debug_Info("{");
        for(j = 0; j <= (int)glyph->contours[i].count; j++)
        {
            if((j % 10) == 0)
                Debug_Info("\n\t");
            Debug_Info("0x%x, ", (unsigned int)glyph->contours[i].flags[j]);
        }
        Debug_Info("\n};\n\n");

        Debug_Info("DBL gGlypthX_%c_%d[] = \n", (char)c, i);
        Debug_Info("{");
        for(j = 0; j <= (int)glyph->contours[i].count; j++)
        {
            if((j % 10) == 0)
                Debug_Info("\n\t");
            Debug_Info("%f, ", (DBL)glyph->contours[i].x[j]);
        }
        Debug_Info("\n};\n\n");

        Debug_Info("DBL gGlypthY_%c_%d[] = \n", (char)c, i);
        Debug_Info("{");
        for(j = 0; j <= (int)glyph->contours[i].count; j++)
        {
            if((j % 10) == 0)
                Debug_Info("\n\t");
            Debug_Info("%f, ", (DBL)glyph->contours[i].y[j]);
        }
        Debug_Info("\n};\n\n");
    }

    Debug_Info("Contour gGlypthContour_%c[] = \n", (char)c);
    Debug_Info("{\n");
    for(i = 0; i < glyph->header.numContours; i++)
    {
        Debug_Info("\t{\n");
        Debug_Info("\t\t%u, // inside_flag \n", (unsigned int)glyph->contours[i].inside_flag);
        Debug_Info("\t\t%u, // count \n", (unsigned int)glyph->contours[i].count);
        Debug_Info("\t\tgGlypthFlags_%c_%d, // flags[]\n", (char)c, i);
        Debug_Info("\t\tgGlypthX_%c_%d, // x[]\n", (char)c, i);
        Debug_Info("\t\tgGlypthY_%c_%d // y[]\n", (char)c, i);
        Debug_Info("\t},\n");
    }
    Debug_Info("\n};\n\n");

    Debug_Info("Glyph gGlypth_%c = \n", (char)c);
    Debug_Info("{\n");
    Debug_Info("\t{ // header\n");
    Debug_Info("\t\t%u, // header.numContours \n", (unsigned int)glyph->header.numContours);
    Debug_Info("\t\t%f, // header.xMin\n", (DBL)glyph->header.xMin);
    Debug_Info("\t\t%f, // header.yMin\n", (DBL)glyph->header.yMin);
    Debug_Info("\t\t%f, // header.xMax\n", (DBL)glyph->header.xMax);
    Debug_Info("\t\t%f // header.yMax\n", (DBL)glyph->header.yMax);
    Debug_Info("\t},\n");
    Debug_Info("\t%u, // glyph_index\n", (unsigned int)glyph->glyph_index);
    Debug_Info("\tgGlypthContour_%c, // contours[]\n", (char)c);
    Debug_Info("\t%u, // unitsPerEm\n", (unsigned int)glyph->unitsPerEm);
    Debug_Info("\tNULL, // next\n");
    Debug_Info("\t%u, // c\n", (unsigned int)glyph->c);
    Debug_Info("\t%u // myMetrics\n", (unsigned int)glyph->myMetrics);

    Debug_Info("};\n\n");
#endif

    return glyph;
}



/*****************************************************************************
*
* FUNCTION
*
*   ExtractGlyphOutline
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
*   Read the contour information for a specific glyph.  This has to be a
*   separate routine from ExtractGlyphInfo because we call it recurisvely
*   for multiple component glyphs.
*
* CHANGES
*
*   -
*
******************************************************************************/
GlyphOutline *ExtractGlyphOutline(TrueTypeFont *ffile, unsigned int glyph_index, unsigned int c)
{
    int i;
    USHORT n;
    SHORT nc;
    GlyphOutline *ttglyph;

    ttglyph = new GlyphOutline;
    ttglyph->myMetrics = glyph_index;

    /* Have to treat space characters differently */
    if (c != ' ')
    {
        ffile->fp->seekg (ffile->info->glyf_table_offset+ffile->info->loca_table[glyph_index]);

        ttglyph->header.numContours = READSHORT(ffile->fp);
        ttglyph->header.xMin = READFWORD(ffile->fp);   /* These may be  */
        ttglyph->header.yMin = READFWORD(ffile->fp);   /* unreliable in */
        ttglyph->header.xMax = READFWORD(ffile->fp);   /* some fonts.   */
        ttglyph->header.yMax = READFWORD(ffile->fp);
    }

#ifdef TTF_DEBUG
    Debug_Info("ttglyph->header:\n");
    Debug_Info("glyph_index=%d\n", glyph_index);
    Debug_Info("loca_table[%d]=%d\n",glyph_index,ffile->info->loca_table[glyph_index]);
    Debug_Info("numContours=%d\n", (int)ttglyph->header.numContours);
#endif

    nc = ttglyph->header.numContours;

    /*
     * A positive number of contours means a regular glyph, with possibly
     * several separate line segments making up the outline.
     */
    if (nc > 0)
    {
        FWord coord;
        BYTE flag, repeat_count;
        USHORT temp16;

        /* Grab the contour endpoints */

        ttglyph->endPoints = reinterpret_cast<USHORT *>(POV_MALLOC(nc * sizeof(USHORT), "ttf"));

        for (i = 0; i < nc; i++)
        {
            ttglyph->endPoints[i] = READUSHORT(ffile->fp);
#ifdef TTF_DEBUG
            Debug_Info("endPoints[%d]=%d\n", i, ttglyph->endPoints[i]);
#endif
        }

        /* Skip over the instructions */
        temp16 = READUSHORT(ffile->fp);
        ffile->fp->seekg (temp16, IOBase::seek_cur);
#ifdef TTF_DEBUG
        Debug_Info("skipping instruction bytes: %d\n", temp16);
#endif

        /* Determine the number of points making up this glyph */

        n = ttglyph->numPoints = ttglyph->endPoints[nc - 1] + 1;
#ifdef TTF_DEBUG
        Debug_Info("numPoints=%d\n", ttglyph->numPoints);
#endif

        /* Read the flags */

        ttglyph->flags = reinterpret_cast<BYTE *>(POV_MALLOC(n * sizeof(BYTE), "ttf"));

        for (i = 0; i < ttglyph->numPoints; i++)
        {
            /// @compat
            /// This piece of code relies on BYTE having the same size as char.
            if (!ffile->fp->read(reinterpret_cast<char *>(&ttglyph->flags[i]), sizeof(BYTE)))
            {
                throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file.");
            }

            if (ttglyph->flags[i] & REPEAT_FLAGS)
            {
                /// @compat
                /// This piece of code relies on BYTE having the same size as char.
                if (!ffile->fp->read(reinterpret_cast<char *>(&repeat_count), sizeof(BYTE)))
                {
                    throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file.");
                }
                for (; repeat_count > 0; repeat_count--, i++)
                {
#ifdef TTF_DEBUG
                    if (i>=n)
                    {
                        Debug_Info("readflags ERROR: i >= n (%d > %d)\n", i, n);
                    }
#endif
                    if (i<n)      /* hack around a bug that is trying to write too many flags */
                        ttglyph->flags[i + 1] = ttglyph->flags[i];
                }
            }
        }
#ifdef  TTF_DEBUG
        Debug_Info("flags:");
        for (i=0; i<n; i++)
            Debug_Info(" %02x", ttglyph->flags[i]);
        Debug_Info("\n");
#endif
        /* Read the coordinate vectors */

        ttglyph->x = reinterpret_cast<DBL *>(POV_MALLOC(n * sizeof(DBL), "ttf"));
        ttglyph->y = reinterpret_cast<DBL *>(POV_MALLOC(n * sizeof(DBL), "ttf"));

        coord = 0;

        for (i = 0; i < ttglyph->numPoints; i++)
        {
            /* Read each x coordinate */

            flag = ttglyph->flags[i];

            if (flag & XSHORT)
            {
                BYTE temp8;

                /// @compat
                /// This piece of code relies on BYTE having the same size as char.
                if (!ffile->fp->read(reinterpret_cast<char *>(&temp8), 1))
                {
                    throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file.");
                }

                if (flag & SHORT_X_IS_POS)
                    coord += temp8;
                else
                    coord -= temp8;
            }
            else if (!(flag & NEXT_X_IS_ZERO))
            {
                coord += READSHORT(ffile->fp);
            }

            /* Find our own maximum and minimum x coordinates */
            if (coord > ttglyph->header.xMax)
                ttglyph->header.xMax = coord;
            if (coord < ttglyph->header.xMin)
                ttglyph->header.xMin = coord;

            ttglyph->x[i] = (DBL)coord / (DBL)ffile->info->unitsPerEm;
        }

        coord = 0;

        for (i = 0; i < ttglyph->numPoints; i++)
        {
            /* Read each y coordinate */

            flag = ttglyph->flags[i];

            if (flag & YSHORT)
            {
                BYTE temp8;

                /// @compat
                /// This piece of code relies on BYTE having the same size as char.
                if (!ffile->fp->read(reinterpret_cast<char *>(&temp8), 1))
                {
                    throw POV_EXCEPTION(kFileDataErr, "Cannot read TrueType font file.");
                }

                if (flag & SHORT_Y_IS_POS)
                    coord += temp8;
                else
                    coord -= temp8;
            }
            else if (!(flag & NEXT_Y_IS_ZERO))
            {
                coord += READSHORT(ffile->fp);
            }

            /* Find out our own maximum and minimum y coordinates */
            if (coord > ttglyph->header.yMax)
                ttglyph->header.yMax = coord;
            if (coord < ttglyph->header.yMin)
                ttglyph->header.yMin = coord;

            ttglyph->y[i] = (DBL)coord / (DBL)ffile->info->unitsPerEm;
        }
    }
    /*
     * A negative number for numContours means that this glyph is
     * made up of several separate glyphs.
     */
    else if (nc < 0)
    {
        USHORT flags;

        ttglyph->header.numContours = 0;
        ttglyph->numPoints = 0;

        do
        {
            GlyphOutline *sub_ttglyph;
            unsigned int sub_glyph_index;
            int   current_pos;
            SHORT arg1, arg2;
            DBL xoff = 0, yoff = 0;
            DBL xscale = 1, yscale = 1;
            DBL scale01 = 0, scale10 = 0;
            USHORT n2;
            SHORT nc2;

            flags = READUSHORT(ffile->fp);
            sub_glyph_index = READUSHORT(ffile->fp);

#ifdef TTF_DEBUG
            Debug_Info("sub_glyph %d: ", sub_glyph_index);
#endif

            if (flags & ARG_1_AND_2_ARE_WORDS)
            {
#ifdef TTF_DEBUG
                Debug_Info("ARG_1_AND_2_ARE_WORDS ");
#endif
                arg1 = READSHORT(ffile->fp);
                arg2 = READSHORT(ffile->fp);
            }
            else
            {
                arg1 = READUSHORT(ffile->fp);
                arg2 = arg1 & 0xFF;
                arg1 = (arg1 >> 8) & 0xFF;
            }

#ifdef TTF_DEBUG
            if (flags & ROUND_XY_TO_GRID)
            {
                Debug_Info("ROUND_XY_TO_GRID ");
            }

            if (flags & MORE_COMPONENTS)
            {
                Debug_Info("MORE_COMPONENTS ");
            }
#endif

            if (flags & WE_HAVE_A_SCALE)
            {
                xscale = yscale = (DBL)READSHORT(ffile->fp)/0x4000;
#ifdef TTF_DEBUG
                Debug_Info("WE_HAVE_A_SCALE ");
                Debug_Info("xscale = %lf\t", xscale);
                Debug_Info("scale01 = %lf\n", scale01);
                Debug_Info("scale10 = %lf\t", scale10);
                Debug_Info("yscale = %lf\n", yscale);
#endif
            }
            else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
            {
                xscale = (DBL)READSHORT(ffile->fp)/0x4000;
                yscale = (DBL)READSHORT(ffile->fp)/0x4000;
#ifdef TTF_DEBUG
                Debug_Info("WE_HAVE_AN_X_AND_Y_SCALE ");
                Debug_Info("xscale = %lf\t", xscale);
                Debug_Info("scale01 = %lf\n", scale01);
                Debug_Info("scale10 = %lf\t", scale10);
                Debug_Info("yscale = %lf\n", yscale);
#endif
            }
            else if (flags & WE_HAVE_A_TWO_BY_TWO)
            {
                xscale  = (DBL)READSHORT(ffile->fp)/0x4000;
                scale01 = (DBL)READSHORT(ffile->fp)/0x4000;
                scale10 = (DBL)READSHORT(ffile->fp)/0x4000;
                yscale  = (DBL)READSHORT(ffile->fp)/0x4000;
#ifdef TTF_DEBUG
                Debug_Info("WE_HAVE_A_TWO_BY_TWO ");
                Debug_Info("xscale = %lf\t", xscale);
                Debug_Info("scale01 = %lf\n", scale01);
                Debug_Info("scale10 = %lf\t", scale10);
                Debug_Info("yscale = %lf\n", yscale);
#endif
            }

            if (flags & ARGS_ARE_XY_VALUES)
            {
                xoff = (DBL)arg1 / ffile->info->unitsPerEm;
                yoff = (DBL)arg2 / ffile->info->unitsPerEm;

#ifdef TTF_DEBUG
                Debug_Info("ARGS_ARE_XY_VALUES ");
                Debug_Info("\narg1 = %d  xoff = %lf\t", arg1, xoff);
                Debug_Info("arg2 = %d  yoff = %lf\n", arg2, yoff);
#endif
            }
            else  /* until I understand how this method works... */
            {
// TODO MESSAGE        Warning("Cannot handle part of glyph %d (0x%X).", c, c);
                continue;
            }

            if (flags & USE_MY_METRICS)
            {
#ifdef TTF_DEBUG
                Debug_Info("USE_MY_METRICS ");
#endif
                ttglyph->myMetrics = sub_glyph_index;
            }

            current_pos = ffile->fp->tellg ();
            sub_ttglyph = ExtractGlyphOutline(ffile, sub_glyph_index, c);
            ffile->fp->seekg (current_pos);

            if ((nc2 = sub_ttglyph->header.numContours) == 0)
                continue;

            nc = ttglyph->header.numContours;
            n = ttglyph->numPoints;
            n2 = sub_ttglyph->numPoints;

            ttglyph->endPoints = reinterpret_cast<USHORT *>(POV_REALLOC(ttglyph->endPoints,
                                                                        (nc + nc2) * sizeof(USHORT), "ttf"));
            ttglyph->flags = reinterpret_cast<BYTE *>(POV_REALLOC(ttglyph->flags, (n+n2)*sizeof(BYTE), "ttf"));
            ttglyph->x = reinterpret_cast<DBL *>(POV_REALLOC(ttglyph->x, (n + n2) * sizeof(DBL), "ttf"));
            ttglyph->y = reinterpret_cast<DBL *>(POV_REALLOC(ttglyph->y, (n + n2) * sizeof(DBL), "ttf"));

            /* Add the sub glyph info to the end of the current glyph */

            ttglyph->header.numContours += nc2;
            ttglyph->numPoints += n2;

            for (i = 0; i < nc2; i++)
            {
                ttglyph->endPoints[i + nc] = sub_ttglyph->endPoints[i] + n;
#ifdef TTF_DEBUG
                Debug_Info("endPoints[%d]=%d\n", i + nc, ttglyph->endPoints[i + nc]);
#endif
            }

            for (i = 0; i < n2; i++)
            {
#ifdef TTF_DEBUG
                Debug_Info("x[%d]=%lf\t", i, sub_ttglyph->x[i]);
                Debug_Info("y[%d]=%lf\n", i, sub_ttglyph->y[i]);
#endif
                ttglyph->flags[i + n] = sub_ttglyph->flags[i];
                ttglyph->x[i + n] = xscale * sub_ttglyph->x[i] +
                                    scale01 * sub_ttglyph->y[i] + xoff;
                ttglyph->y[i + n] = scale10 * sub_ttglyph->x[i] +
                                    yscale * sub_ttglyph->y[i] + yoff;

#ifdef TTF_DEBUG
                Debug_Info("x[%d]=%lf\t", i+n, ttglyph->x[i+n]);
                Debug_Info("y[%d]=%lf\n", i+n, ttglyph->y[i+n]);
#endif

                if (ttglyph->x[i + n] < ttglyph->header.xMin)
                    ttglyph->header.xMin = ttglyph->x[i + n];

                if (ttglyph->x[i + n] > ttglyph->header.xMax)
                    ttglyph->header.xMax = ttglyph->x[i + n];

                if (ttglyph->y[i + n] < ttglyph->header.yMin)
                    ttglyph->header.yMin = ttglyph->y[i + n];

                if (ttglyph->y[i + n] > ttglyph->header.yMax)
                    ttglyph->header.yMax = ttglyph->y[i + n];
            }

            /* Free up the sub glyph outline information */

            delete sub_ttglyph;
        } while (flags & MORE_COMPONENTS);
    }

#ifdef TTF_DEBUG
        Debug_Info("xMin=%d\n",ttglyph->header.xMin);
        Debug_Info("yMin=%d\n",ttglyph->header.yMin);
        Debug_Info("xMax=%d\n",ttglyph->header.xMax);
        Debug_Info("yMax=%d\n",ttglyph->header.yMax);
#endif

    return ttglyph;
}



/*****************************************************************************
*
* FUNCTION
*
*   ConvertOutlineToGlyph
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* Transform a glyph from TrueType storage format to something a little easier
* to manage.
*
* CHANGES
*
*   -
*
******************************************************************************/
GlyphPtr ConvertOutlineToGlyph(TrueTypeFont *ffile, const GlyphOutline *ttglyph)
{
    GlyphPtr glyph;
    DBL *temp_x, *temp_y;
    BYTE *temp_f;
    USHORT i, j, last_j;

    /* Create storage for this glyph */

    glyph = new GlyphStruct;
    if (ttglyph->header.numContours > 0)
    {
        glyph->contours = new Contour[ttglyph->header.numContours];
    }
    else
    {
        glyph->contours = NULL;
    }

    /* Copy sizing information about this glyph */

    POV_MEMCPY(&glyph->header, &ttglyph->header, sizeof(GlyphHeader));

    /* Keep track of the size for this glyph */

    glyph->unitsPerEm = ffile->info->unitsPerEm;

    /* Now copy the vertex information into the contours */

    for (i = 0, last_j = 0; i < (USHORT) ttglyph->header.numContours; i++)
    {
        /* Figure out number of points in contour */

        j = ttglyph->endPoints[i] - last_j + 1;

        /* Copy the coordinate information into the glyph */

        temp_x = reinterpret_cast<DBL *>(POV_MALLOC((j + 1) * sizeof(DBL), "ttf"));
        temp_y = reinterpret_cast<DBL *>(POV_MALLOC((j + 1) * sizeof(DBL), "ttf"));

        temp_f = reinterpret_cast<BYTE *>(POV_MALLOC((j + 1) * sizeof(BYTE), "ttf"));
        POV_MEMCPY(temp_x, &ttglyph->x[last_j], j * sizeof(DBL));
        POV_MEMCPY(temp_y, &ttglyph->y[last_j], j * sizeof(DBL));

        POV_MEMCPY(temp_f, &ttglyph->flags[last_j], j * sizeof(BYTE));
        temp_x[j] = ttglyph->x[last_j];
        temp_y[j] = ttglyph->y[last_j];
        temp_f[j] = ttglyph->flags[last_j];

        /* Figure out if this is an inside or outside contour */

        glyph->contours[i].inside_flag = 0;

        /* Plug in the reset of the contour components into the glyph */

        glyph->contours[i].count = j;
        glyph->contours[i].x = temp_x;
        glyph->contours[i].y = temp_y;
        glyph->contours[i].flags = temp_f;

        /*
         * Set last_j to point to the beginning of the next contour's coordinate
         * information
         */

        last_j = ttglyph->endPoints[i] + 1;
    }

    /* Show statistics about this glyph */

#ifdef TTF_DEBUG
    Debug_Info("Number of contours: %u\n", glyph->header.numContours);
    Debug_Info("X extent: [%f, %f]\n",
        (DBL)glyph->header.xMin / (DBL)ffile->info->unitsPerEm,
        (DBL)glyph->header.xMax / (DBL)ffile->info->unitsPerEm);

    Debug_Info("Y extent: [%f, %f]\n",
        (DBL)glyph->header.yMin / (DBL)ffile->info->unitsPerEm,
        (DBL)glyph->header.yMax / (DBL)ffile->info->unitsPerEm);

    Debug_Info("Converted coord list(%d):\n", (int)glyph->header.numContours);

    for (i=0;i<(USHORT)glyph->header.numContours;i++)
    {
        for (j=0;j<=glyph->contours[i].count;j++)
            Debug_Info("  %c[%f, %f]\n",
                (glyph->contours[i].flags[j] & ONCURVE ? '*' : ' '),
                glyph->contours[i].x[j], glyph->contours[i].y[j]);
        Debug_Info("\n");
    }
#endif

    return glyph;
}

/* Test to see if "point" is inside the splined polygon "points". */
bool TrueType::Inside_Glyph(double x, double y, const GlyphStruct* glyph) const
{
    int i, j, k, n, n1, crossings;
    int qi, ri, qj, rj;
    Contour *contour;
    double xt[3], yt[3], roots[2];
    DBL *xv, *yv;
    double x0, x1, x2, t;
    double y0, y1, y2;
    double m, b, xc;
    BYTE *fv;

    crossings = 0;

    n = glyph->header.numContours;

    contour = glyph->contours;

    for (i = 0; i < n; i++)
    {
        xv = contour[i].x;
        yv = contour[i].y;
        fv = contour[i].flags;
        x0 = xv[0];
        y0 = yv[0];
        n1 = contour[i].count;

        for (j = 1; j <= n1; j++)
        {
            x1 = xv[j];
            y1 = yv[j];

            if (fv[j] & ONCURVE)
            {
                /* Straight line - first set up for the next */
                /* Now do the crossing test */

                qi = ri = qj = rj = 0;

                if (y0 == y1)
                    goto end_line_test;

                /* if (fabs((y - y0) / (y1 - y0)) < EPSILON) goto end_line_test; */

                if (y0 < y)
                    qi = 1;

                if (y1 < y)
                    qj = 1;

                if (qi == qj)
                    goto end_line_test;

                if (x0 > x)
                    ri = 1;

                if (x1 > x)
                    rj = 1;

                if (ri & rj)
                {
                    crossings++;
                    goto end_line_test;
                }

                if ((ri | rj) == 0)
                    goto end_line_test;

                m = (y1 - y0) / (x1 - x0);
                b = (y1 - y) - m * (x1 - x);

                if ((b / m) < EPSILON)
                {
                    crossings++;
                }

            end_line_test:
                x0 = x1;
                y0 = y1;
            }
            else
            {
                if (j == n1)
                {
                    x2 = xv[0];
                    y2 = yv[0];
                }
                else
                {
                    x2 = xv[j + 1];
                    y2 = yv[j + 1];

                    if (!(fv[j + 1] & ONCURVE))
                    {
                        /*
                         * Parabola with far end floating - readjust the far end so that it
                         * is on the curve.
                         */

                        x2 = 0.5 * (x1 + x2);
                        y2 = 0.5 * (y1 + y2);
                    }
                }

                /* only test crossing when y is in the range */
                /* this should also help saving some computations */
                if (((y0 < y) && (y1 < y) && (y2 < y)) ||
                    ((y0 > y) && (y1 > y) && (y2 > y)))
                    goto end_curve_test;

                yt[0] = y0 - 2.0 * y1 + y2;
                yt[1] = 2.0 * (y1 - y0);
                yt[2] = y0 - y;

                k = solve_quad(yt, roots, 0.0, 1.0);

                for (ri = 0; ri < k;) {
                    if (roots[ri] <= EPSILON) {
                        /* if y actually is not in range, discard the root */
                        if (((y <= y0) && (y < y1)) || ((y >= y0) && (y > y1))) {
                            k--;
                            if (k > ri)
                                roots[ri] = roots[ri+1];
                            continue;
                        }
                    }
                    else if (roots[ri] >= (1.0 - EPSILON)) {
                        /* if y actually is not in range, discard the root */
                        if (((y < y2) && (y < y1)) || ((y > y2) && (y > y1))) {
                            k--;
                            if (k > ri)
                                roots[ri] = roots[ri+1];
                            continue;
                        }
                    }

                    ri++;
                }

                if (k > 0)
                {
                    xt[0] = x0 - 2.0 * x1 + x2;
                    xt[1] = 2.0 * (x1 - x0);
                    xt[2] = x0;

                    t = roots[0];

                    xc = (xt[0] * t + xt[1]) * t + xt[2];

                    if (xc > x)
                        crossings++;

                    if (k > 1)
                    {
                        t = roots[1];
                        xc = (xt[0] * t + xt[1]) * t + xt[2];

                        if (xc > x)
                            crossings++;
                    }
                }

end_curve_test:

                x0 = x2;

                y0 = y2;
            }
        }
    }

    return ((crossings & 1) != 0);
}


int TrueType::solve_quad(double *x, double *y, double mindist, DBL maxdist) const
{
    double d, t, a, b, c, q;

    a = x[0];
    b = -x[1];
    c = x[2];

    if (fabs(a) < COEFF_LIMIT)
    {
        if (fabs(b) < COEFF_LIMIT)
            return 0;

        q = c / b;

        if (q >= mindist && q <= maxdist)
        {
            y[0] = q;
            return 1;
        }
        else
            return 0;
    }

    d = b * b - 4.0 * a * c;

    if (d < EPSILON)
        return 0;

    d = sqrt(d);
    t = 2.0 * a;
    q = (b + d) / t;

    if (q >= mindist && q <= maxdist)
    {
        y[0] = q;
        q = (b - d) / t;

        if (q >= mindist && q <= maxdist)
        {
            y[1] = q;
            return 2;
        }

        return 1;
    }

    q = (b - d) / t;

    if (q >= mindist && q <= maxdist)
    {
        y[0] = q;
        return 1;
    }

    return 0;
}

/*
 * Returns the distance to z = 0 in t0, and the distance to z = 1 in t1.
 * These distances are to the the bottom and top surfaces of the glyph.
 * The distances are set to -1 if there is no hit.
 */
void TrueType::GetZeroOneHits(const GlyphStruct* glyph, const Vector3d& P, const Vector3d& D, DBL glyph_depth, double *t0, double *t1) const
{
    double x0, y0, t;

    *t0 = -1.0;
    *t1 = -1.0;

    /* Are we parallel to the x-y plane? */

    if (fabs(D[Z]) < EPSILON)
        return;

    /* Solve: P[Y] + t * D[Y] = 0 */

    t = -P[Z] / D[Z];

    x0 = P[X] + t * D[X];
    y0 = P[Y] + t * D[Y];

    if (Inside_Glyph(x0, y0, glyph))
        *t0 = t;

    /* Solve: P[Y] + t * D[Y] = glyph_depth */

    t += (glyph_depth / D[Z]);

    x0 = P[X] + t * D[X];
    y0 = P[Y] + t * D[Y];

    if (Inside_Glyph(x0, y0, glyph))
        *t1 = t;
}

/*
 * Solving for a linear sweep of a non-linear curve can be performed by
 * projecting the ray onto the x-y plane, giving a parametric equation for the
 * ray as:
 *
 * x = x0 + x1 t, y = y0 + y1 t
 *
 * Eliminating t from the above gives the implicit equation:
 *
 * y1 x - x1 y - (x0 y1 - y0 x1) = 0.
 *
 * Substituting a parametric equation for x and y gives:
 *
 * y1 x(s) - x1 y(s) - (x0 y1 - y0 x1) = 0.
 *
 * which can be written as
 *
 * a x(s) + b y(s) + c = 0,
 *
 * where a = y1, b = -x1, c = (y0 x1 - x0 y1).
 *
 * For piecewise quadratics, the parametric equations will have the forms:
 *
 * x(s) = (1-s)^2 P0(x) + 2 s (1 - s) P1(x) + s^2 P2(x) y(s) = (1-s)^2 P0(y) + 2 s
 * (1 - s) P1(y) + s^2 P2(y)
 *
 * where P0 is the first defining vertex of the spline, P1 is the second, P2 is
 * the third.  Using the substitutions:
 *
 * xt2 = x0 - 2 x1 + x2, xt1 = 2 * (x1 - x0), xt0 = x0; yt2 = y0 - 2 y1 + y2, yt1
 * = 2 * (y1 - y0), yt0 = y0;
 *
 * the equations can be written as:
 *
 * x(s) = xt2 s^2 + xt1 s + xt0, y(s) = yt2 s^2 + yt1 s + yt0.
 *
 * Substituting and multiplying out gives the following equation in s:
 *
 * s^2 * (a*xt2 + b*yt2) + s   * (a*xt1 + b*yt1) + c + a*xt0 + b*yt0
 *
 * This is then solved using the quadratic formula.  Any solutions of s that are
 * between 0 and 1 (inclusive) are valid solutions.
 */
bool TrueType::GlyphIntersect(const Vector3d& P, const Vector3d& D, const GlyphStruct* glyph, DBL glyph_depth, const BasicRay& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    Contour *contour;
    int i, j, k, l, n, m;
    bool Flag = false;
    Vector3d N, IPoint;
    DBL Depth;
    double x0, x1, y0, y1, x2, y2, t, t0, t1, z;
    double xt0, xt1, xt2, yt0, yt1, yt2;
    double a, b, c, d0, d1, C[3], S[2];
    DBL *xv, *yv;
    BYTE *fv;
    int dirflag = 0;

    /*
     * First thing to do is to get any hits at z = 0 and z = 1 (which are the
     * bottom and top surfaces of the glyph.
     */

    GetZeroOneHits(glyph, P, D, glyph_depth, &t0, &t1);

    if (t0 > 0.0)
    {
        Depth = t0 /* / len */;
        IPoint = ray.Evaluate(Depth);

        if (Depth > TTF_Tolerance && (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread)))
        {
            N = Vector3d(0.0, 0.0, -1.0);
            MTransNormal(N, N, Trans);
            N.normalize();
            Depth_Stack->push(Intersection(Depth, IPoint, N, this));
            Flag = true;
        }
    }

    if (t1 > 0.0)
    {
        Depth = t1 /* / len */;
        IPoint = ray.Evaluate(Depth);

        if (Depth > TTF_Tolerance && (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread)))
        {
            N = Vector3d(0.0, 0.0, 1.0);
            MTransNormal(N, N, Trans);
            N.normalize();
            Depth_Stack->push(Intersection(Depth, IPoint, N, this));
            Flag = true;
        }
    }

    /* Simple test to see if we can just toss this ray */

    if (fabs(D[X]) < EPSILON)
    {
        if (fabs(D[Y]) < EPSILON)
        {
            /*
             * This means the ray is moving parallel to the walls of the sweep
             * surface
             */
            return Flag;
        }
        else
        {
            dirflag = 0;
        }
    }
    else
    {
        dirflag = 1;
    }

    /*
     * Now walk through the glyph, looking for places where the ray hits the
     * walls
     */

    a = D[Y];
    b = -D[X];
    c = (P[Y] * D[X] - P[X] * D[Y]);

    n = glyph->header.numContours;

    for (i = 0, contour = glyph->contours; i < n; i++, contour++)
    {
        xv = contour->x;
        yv = contour->y;
        fv = contour->flags;
        x0 = xv[0];
        y0 = yv[0];
        m = contour->count;

        for (j = 1; j <= m; j++)
        {
            x1 = xv[j];
            y1 = yv[j];

            if (fv[j] & ONCURVE)
            {
                /* Straight line */
                d0 = (x1 - x0);
                d1 = (y1 - y0);

                t0 = d1 * D[X] - d0 * D[Y];

                if (fabs(t0) < EPSILON)
                    /* No possible intersection */
                    goto end_line_test;

                t = (D[X] * (P[Y] - y0) - D[Y] * (P[X] - x0)) / t0;

                if (t < 0.0 || t > 1.0)
                    goto end_line_test;

                if (dirflag)
                    t = ((x0 + t * d0) - P[X]) / D[X];
                else
                    t = ((y0 + t * d1) - P[Y]) / D[Y];

                z = P[Z] + t * D[Z];

                Depth = t /* / len */;

                if (z >= 0 && z <= glyph_depth && Depth > TTF_Tolerance)
                {
                    IPoint = ray.Evaluate(Depth);

                    if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                    {
                        N = Vector3d(d1, -d0, 0.0);
                        MTransNormal(N, N, Trans);
                        N.normalize();
                        Depth_Stack->push(Intersection(Depth, IPoint, N, this));
                        Flag = true;
                    }
                }
            end_line_test:
                x0 = x1;
                y0 = y1;
            }
            else
            {
                if (j == m)
                {
                    x2 = xv[0];
                    y2 = yv[0];
                }
                else
                {
                    x2 = xv[j + 1];
                    y2 = yv[j + 1];

                    if (!(fv[j + 1] & ONCURVE))
                    {

                        /*
                         * Parabola with far end DBLing - readjust the far end so that it
                         * is on the curve.  (In the correct place too.)
                         */

                        x2 = 0.5 * (x1 + x2);
                        y2 = 0.5 * (y1 + y2);
                    }
                }

                /* Make the interpolating quadrics */

                xt2 = x0 - 2.0 * x1 + x2;
                xt1 = 2.0 * (x1 - x0);
                xt0 = x0;
                yt2 = y0 - 2.0 * y1 + y2;
                yt1 = 2.0 * (y1 - y0);
                yt0 = y0;

                C[0] = a * xt2 + b * yt2;
                C[1] = a * xt1 + b * yt1;
                C[2] = a * xt0 + b * yt0 + c;

                k = solve_quad(C, S, 0.0, 1.0);

                for (l = 0; l < k; l++)
                {
                    if (dirflag)
                        t = ((S[l] * S[l] * xt2 + S[l] * xt1 + xt0) - P[X]) / D[X];
                    else
                        t = ((S[l] * S[l] * yt2 + S[l] * yt1 + yt0) - P[Y]) / D[Y];

                    /*
                     * If the intersection with this wall is between 0 and glyph_depth
                     * along the z-axis, then it is a valid hit.
                     */

                    z = P[Z] + t * D[Z];

                    Depth = t /* / len */;

                    if (z >= 0 && z <= glyph_depth && Depth > TTF_Tolerance)
                    {
                        IPoint = ray.Evaluate(Depth);

                        if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                        {
                            N = Vector3d(2.0 * yt2 * S[l] + yt1, -2.0 * xt2 * S[l] - xt1, 0.0);
                            MTransNormal(N, N, Trans);
                            N.normalize();
                            Depth_Stack->push(Intersection(Depth, IPoint, N, this));
                            Flag = true;
                        }
                    }
                }

                x0 = x2;
                y0 = y2;
            }
        }
    }

    return Flag;
}

bool TrueType::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    Vector3d P, D;

    Thread->Stats()[Ray_TTF_Tests]++;

    /* Transform the point into the glyph's space */

    MInvTransPoint(P, ray.Origin, Trans);
    MInvTransDirection(D, ray.Direction, Trans);

    /* Tweak the ray to try to avoid pathalogical intersections */
/*  DBL len;

    D[0] *= 1.0000013147;
    D[1] *= 1.0000022741;
    D[2] *= 1.0000017011;

    D.normalize();
*/

    if (GlyphIntersect(P, D, glyph, depth, ray, Depth_Stack, Thread)) /* tw */
    {
        Thread->Stats()[Ray_TTF_Tests_Succeeded]++;
        return true;
    }

    return false;
}

bool TrueType::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    Vector3d New_Point;

    /* Transform the point into font space */

    MInvTransPoint(New_Point, IPoint, Trans);

    if (New_Point[Z] >= 0.0 && New_Point[Z] <= depth &&
        Inside_Glyph(New_Point[X], New_Point[Y], glyph))
        return (!Test_Flag(this, INVERTED_FLAG));
    else
        return (Test_Flag(this, INVERTED_FLAG));
}

void TrueType::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    /* Use precomputed normal. [ARE 11/94] */

    Result = Inter->INormal;
}

ObjectPtr TrueType::Copy()
{
    TrueType *New = new TrueType();
    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);
    New->glyph = glyph; // TODO - How can this work correctly? [trf]
    return (New);
}

void TrueType::Translate(const Vector3d& /*Vector*/, const TRANSFORM *tr)
{
    Transform(tr);
}

void TrueType::Rotate(const Vector3d& /*Vector*/, const TRANSFORM *tr)
{
    Transform(tr);
}

void TrueType::Scale(const Vector3d& /*Vector*/, const TRANSFORM *tr)
{
    Transform(tr);
}

void TrueType::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    /* Calculate the bounds */

    Compute_BBox();
}

TrueType::TrueType() : ObjectBase(TTF_OBJECT)
{
    /* Initialize TTF specific information */

    Trans = Create_Transform();

    glyph = NULL;
    depth = 1.0;

    /* Default bounds */
    Make_BBox(BBox, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
}

TrueType::~TrueType()
{}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_TTF_BBox
*
* INPUT
*
*   ttf - ttf
*
* OUTPUT
*
*   ttf
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer, August 1994
*
* DESCRIPTION
*
*   Calculate the bounding box of a true type font.
*
* CHANGES
*
*   -
*
******************************************************************************/
void TrueType::Compute_BBox()
{
    DBL funit_size, xMin, yMin, zMin, xMax, yMax, zMax;

    funit_size = 1.0 / (DBL)(glyph->unitsPerEm);

    xMin = (DBL)glyph->header.xMin * funit_size;
    yMin = (DBL)glyph->header.yMin * funit_size;
    zMin = -TTF_Tolerance;

    xMax = (DBL)glyph->header.xMax * funit_size;
    yMax = (DBL)glyph->header.yMax * funit_size;
    zMax = depth + TTF_Tolerance;

    Make_BBox(BBox, xMin, yMin, zMin, xMax - xMin, yMax - yMin, zMax - zMin);

#ifdef TTF_DEBUG
    Debug_Info("Bounds: <%g,%g,%g> -> <%g,%g,%g>\n",
               ttf->BBox.lowerLeft[0],
               ttf->BBox.lowerLeft[1],
               ttf->BBox.lowerLeft[2],
               ttf->BBox.size[0],
               ttf->BBox.size[1],
               ttf->BBox.size[2]);
#endif

    /* Apply the transformation to the bounding box */

    Recompute_BBox(&BBox, Trans);
}


TrueTypeFont::TrueTypeFont(UCS2* fn, IStream* fp, StringEncoding te) :
    filename(fn),
    fp(fp),
    textEncoding(te),
    info(NULL)
{
    ProcessFontFile(this);
}

TrueTypeFont::~TrueTypeFont()
{
    if (fp != NULL)
        delete fp;

    if (filename != NULL)
        POV_FREE(filename);

    if (info != NULL)
        delete info;
}

TrueTypeInfo::TrueTypeInfo() :
    cmap_table_offset(0),
    glyf_table_offset(0),
    numGlyphs(0),
    unitsPerEm(0),
    indexToLocFormat(0),
    loca_table(NULL),
    numberOfHMetrics(0),
    hmtx_table(NULL),
    glyphIDoffset(0),
    segCount(0),
    searchRange(0),
    entrySelector(0),
    rangeShift(0),
    startCount(NULL),
    endCount(NULL),
    idDelta(NULL),
    idRangeOffset(NULL)
{
    for (int i = 0; i < 4; i ++)
    {
        platformID[i] = 0;
        specificID[i] = 0;
    }

    kerning_tables.nTables = 0;
    kerning_tables.tables = NULL;
}

TrueTypeInfo::~TrueTypeInfo()
{
    if (loca_table != NULL)
        delete[] loca_table;

    for (GlyphPtrMap::iterator iGlyph = glyphsByIndex.begin(); iGlyph != glyphsByIndex.end(); ++iGlyph)
    {
        if ((*iGlyph).second->contours != NULL)
        {
            for (int i = 0; i < (*iGlyph).second->header.numContours; i++)
            {
                POV_FREE((*iGlyph).second->contours[i].flags);
                POV_FREE((*iGlyph).second->contours[i].x);
                POV_FREE((*iGlyph).second->contours[i].y);
            }
            delete[] (*iGlyph).second->contours;
        }
        delete (*iGlyph).second;
    }

    if (kerning_tables.tables != NULL)
    {
        for (int i = 0; i < kerning_tables.nTables; i++)
        {
            if (kerning_tables.tables[i].kern_pairs)
                delete[] kerning_tables.tables[i].kern_pairs;
        }

        delete[] kerning_tables.tables;
    }

    if (hmtx_table != NULL)
        delete[] hmtx_table;

    if (endCount != NULL)
        delete[] endCount;

    if (startCount != NULL)
        delete[] startCount;

    if (idDelta != NULL)
        delete[] idDelta;

    if (idRangeOffset != NULL)
        delete[] idRangeOffset;
}

}

