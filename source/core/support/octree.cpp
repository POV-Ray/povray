//******************************************************************************
///
/// @file core/support/octree.cpp
///
/// Implementation of the radiosity sample octree.
///
/// @author Jim McElhiney.
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

/************************************************************************
*  Oct-tree routines.  Used by Radiosity calculation routines.
*
*  To understand the relationship between an ot_id (x,y,z,size) and
*  a place in model space, you have to scale the integer values:
*  The nominal space occupied is given as follows:
*      fsize = pow(2,size-Pow2Bias);
*      lox = (float)x *fsize; loy = (float)y * fsize; loz = (float)z * fsize;
*      hix = lox + fsize;  hiy = loy + fsize;  hiz = loz + fsize;
*  All elements within this node are guaranteed to stick outside of the
*  nominal box by a distance of less than fsize/2 in x, y, and/or z.
*  Therefore, the following box is guaranteed to contain all of the
*  elements:
*      minx = lox - fsize/2.;  miny = loy - fsize/2.;  minz = loz - fsize/2.;
*      maxx = lox + fsize/2.;  maxy = loy + fsize/2.;  maxz = loz + fsize/2.;
*  Implemented by and (c) 1994-6 Jim McElhiney, mcelhiney@acm.org  or 71201,1326
*  All standard POV distribution rights granted.  All other rights reserved.
*************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/support/octree.h"

// C++ variants of C standard header files
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstring>

// C++ standard header files
#include <algorithm>
#include <limits>

// POV-Ray header files (base module)
#include "base/fileinputoutput.h"
#include "base/mathutil.h"
#include "base/pov_err.h"
#include "base/pov_mem.h"

// POV-Ray header files (core module)
#include "core/colour/spectral.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define SAFE_METHOD 1
// #define OT_DEBUG 1

//******************************************************************************

// compiler / target platform sanity checks
// (note that these don't necessarily catch all possible quirks; they should be quite reliable though)
#if (POV_PORTABLE_RADIOSITY == 0)

// Needs IEEE 754 binary32 (aka single precision) floating-point format
// and an unpadded integer of same size.

// According to the C++11 standard `std::uint32_t` must be exactly 32 bits wide,
// unpadded and unsigned, _or_ be undefined if the platform does not support
// such a type. In the latter case, we expect a compile error here complaining
// about `std::uint32_t`.
// If that's what you are seeing, do _not_ try to substitute a different type,
// as it is unlikely to work. Instead, try a different setting for
// POV_PORTABLE_RADIOSITY.

static_assert(
    (sizeof(float) == sizeof(std::uint32_t)) &&
    (std::numeric_limits<float>::radix == 2) &&
    (std::numeric_limits<float>::digits == 24) &&
    (std::numeric_limits<float>::min_exponent == -125) &&
    (std::numeric_limits<float>::max_exponent == 128),
    "POV_PORTABLE_RADIOSITY setting 0 requires 'float' to conform to IEEE 754 binary32 "
    "(aka single precision) format, which it doesn't. Try a different setting."
    );

#elif (POV_PORTABLE_RADIOSITY == 5)

// Needs IEEE 754 binary64 (aka double precision) floating-point format
// and an unpadded integer of same size.

// According to the C++11 standard `std::uint64_t` must be exactly 32 bits wide,
// unpadded and unsigned, _or_ be undefined if the platform does not support
// such a type. In the latter case, we expect a compile error here complaining
// about `std::uint64_t`.
// If that's what you are seeing, do _not_ try to substitute a different type,
// as it is unlikely to work. Instead, try a different setting for
// POV_PORTABLE_RADIOSITY.

static_assert(
    (sizeof(double) == sizeof(std::uint64_t)) &&
    (std::numeric_limits<double>::radix == 2) &&
    (std::numeric_limits<double>::digits == 53) &&
    (std::numeric_limits<double>::min_exponent == -1021) &&
    (std::numeric_limits<double>::max_exponent == 1024),
    "POV_PORTABLE_RADIOSITY setting 5 requires 'double' to conform to IEEE 754 binary64 "
    "(aka double precision) format, which it doesn't. Try a different setting."
    );

#elif (POV_PORTABLE_RADIOSITY == 1) || (POV_PORTABLE_RADIOSITY == 2) || (POV_PORTABLE_RADIOSITY == 7)

// Needs Radix-2 floating-point format.

static_assert(
    (std::numeric_limits<float>::radix == 2),
    "POV_PORTABLE_RADIOSITY settings 1, 2 and 7 require 'float' to be a radix-2 type, "
    "which it isn't. Try a different setting."
    );

#elif (POV_PORTABLE_RADIOSITY == 3) || (POV_PORTABLE_RADIOSITY == 4) || (POV_PORTABLE_RADIOSITY == 9)

// Needs Radix-2 floating-point format.

static_assert(
    (std::numeric_limits<double>::radix == 2),
    "POV_PORTABLE_RADIOSITY settings 3, 4 and 9 requires 'double' to be a radix-2 type, "
    "which it isn't. Try a different setting."
    );

#endif

//------------------------------------------------------------------------------

#if (POV_PORTABLE_RADIOSITY == 0)

/// Bias inherent in @ref BiasedLog2() and @ref BiasedPow2().
static constexpr auto Pow2Bias = 127;

union FloatHack { float f; std::uint32_t i; };

/// Round down to power of 2.
///
/// This function rounds a value down to a power of 2:
///
/// @f[
///     f(a) = 2 ^{ \left \lfloor log_2 a \right \rfloor }
/// @f]
///
/// @note
///     The function signature may differ depending on
///     @ref POV_PORTABLE_RADIOSITY.
///
/// @pre
///     The parameter value shall be finite and positive.
///
static inline float Pow2Floor(float a)
{
    FloatHack hack;
    hack.f = a;
    // Directly clear all mantissa bits (= 1.0)
    // while leaving sign and exponent intact.
    hack.i &= 0xFF800000;
    return hack.f;
}

/// Compute biased integer base 2 logarithm.
///
/// This function computes the integer part of the base 2 logarithm of the
/// parameter's magnitude, plus a constant bias:
///
/// @f[
///     f(a) = \left \lfloor log_2 \left | a \right | \right \rfloor + b
/// @f]
///
/// The value of the bias _b_ depends on @ref POV_PORTABLE_RADIOSITY, bit is
/// the same value as used by @ref BiasedIntPow2().
///
/// @note
///     The function signature may differ depending on
///     @ref POV_PORTABLE_RADIOSITY.
///
/// @pre
///     The parameter value shall be finite and positive.
///
static inline int BiasedIntLog2(float a)
{
    FloatHack hack;
    hack.f = a;
    // Directly extract the (biased) float exponent as the final result.
    return (hack.i & 0x7F800000u) >> 23;
}

/// Compute biased integer power of 2.
///
/// This function computes 2 raised to the power of the integer parameter
/// minus a constant bias:
///
/// @f[
///     f(a) = 2 ^{ a - b }
/// @f]
///
/// The value of the bias _b_ depends on @ref POV_PORTABLE_RADIOSITY, bit is
/// the same value as used by @ref BiasedIntLog2().
///
/// @note
///     The function signature may differ depending on
///     @ref POV_PORTABLE_RADIOSITY.
///
static inline float BiasedIntPow2(int a)
{
    FloatHack hack;
    // Directly copy `a` into the (biased) float exponent,
    // while clearing sign and mantissa bits (= positive, 1.0).
    hack.i = uint32_t(a) << 23;
    return hack.f;
}

#elif (POV_PORTABLE_RADIOSITY == 1)

static constexpr auto Pow2Bias = 0;

static inline float Pow2Floor(float a)
{
    return std::powf(2.0f, std::logbf(a));
}

static inline int BiasedIntLog2(float a)
{
    return (int)std::logbf(a);
}

static inline float BiasedIntPow2(int a)
{
    if (a >= 0)
        return float(1 << a);
    else
        return 1.0f / float(1 << -a);
}

#elif (POV_PORTABLE_RADIOSITY == 2)

static constexpr auto Pow2Bias = 0;

static inline float Pow2Floor(float a)
{
    return float(1 << std::ilogbf(a));
}

static inline int BiasedIntLog2(float a)
{
    return std::ilogbf(a);
}

static inline float BiasedIntPow2(int a)
{
    if (a >= 0)
        return float(1 << a);
    else
        return 1.0f / float(1 << -a);
}

#elif (POV_PORTABLE_RADIOSITY == 3)

static constexpr auto Pow2Bias = 0;

static inline double Pow2Floor(double a)
{
    return std::pow(2.0, std::logb(a));
}

static inline int BiasedIntLog2(double a)
{
    return (int)std::logb(a);
}

static inline double BiasedIntPow2(int a)
{
    if (a >= 0)
        return double(1 << a);
    else
        return 1.0 / double(1 << -a);
}

#elif (POV_PORTABLE_RADIOSITY == 4)

static constexpr auto Pow2Bias = 0;

static inline double Pow2Floor(double a)
{
    return double(1 << std::ilogb(a));
}

static inline int BiasedIntLog2(double a)
{
    return std::ilogb(a);
}

static inline double BiasedIntPow2(int a)
{
    if (a >= 0)
        return double(1 << a);
    else
        return 1.0 / double(1 << -a);
}

#elif (POV_PORTABLE_RADIOSITY == 5)

static constexpr auto Pow2Bias = 1023;

union DoubleHack { double f; std::uint64_t i; };

static inline double Pow2Floor(double a)
{
    DoubleHack hack;
    hack.f = a;
    // Directly clear all mantissa bits (= 1.0)
    // while leaving sign and exponent intact.
    hack.i &= 0xFFF0000000000000;
    return hack.f;
}

static inline int BiasedIntLog2(double a)
{
    DoubleHack hack;
    hack.f = a;
    // Directly extract the (biased) float exponent as the final result.
    return ((hack.i & 0x7FF0000000000000u) >> 52);
}

static inline double BiasedIntPow2(int a)
{
    DoubleHack hack;
    // Directly copy `a` into the (biased) float exponent,
    // while clearing sign and mantissa bits (= positive, 1.0).
    hack.i = uint64_t(a) << 52;
    return hack.f;
}

#elif (POV_PORTABLE_RADIOSITY == 6)

static constexpr auto Pow2Bias = 0;

static inline float Pow2Floor(float a)
{
    int e;
    (void)std::frexpf(a, &e);
    return std::ldexpf(0.5f, e);
}

static inline int BiasedIntLog2(float a)
{
    int e;
    (void)std::frexpf(a, &e);
    return e - 1;
}

static inline float BiasedIntPow2(int a)
{
    return std::ldexpf(1.0f, a);
}

#elif (POV_PORTABLE_RADIOSITY == 7)

static constexpr auto Pow2Bias = 0;

static inline float Pow2Floor(float a)
{
    return std::ldexpf(1.0f, std::ilogbf(a));
}

static inline int BiasedIntLog2(float a)
{
    return std::ilogbf(a);
}

static inline float BiasedIntPow2(int a)
{
    return std::ldexpf(1.0f, a);
}

#elif (POV_PORTABLE_RADIOSITY == 8)

static constexpr auto Pow2Bias = 0;

static inline double Pow2Floor(double a)
{
    int e;
    (void)std::frexp(a, &e);
    return std::ldexp(0.5, e);
}
static inline int BiasedIntLog2(double a)
{
    int e;
    (void)std::frexp(a, &e);
    return e - 1;
}
static inline double BiasedIntPow2(int a)
{
    return std::ldexpf(1.0, a);
}

#elif (POV_PORTABLE_RADIOSITY == 9)

static constexpr auto Pow2Bias = 0;

static inline double Pow2Floor(double a)
{
    return std::ldexp(1.0, std::ilogb(a));
}

static inline int BiasedIntLog2(double a)
{
    return std::ilogb(a);
}

static inline double BiasedIntPow2(int a)
{
    return std::ldexp(1.0, a);
}

#else
#error "Unsupported POV_PORTABLE_RADIOSITY setting."
#endif

//******************************************************************************

bool ot_save_node (const Vector3d& point, OT_ID *node);
bool ot_traverse (OT_NODE *subtree, bool (*function)(OT_BLOCK *block, void * handle1), void * handle2);
bool ot_free_subtree (OT_NODE *node);

void ot_list_insert (OT_BLOCK **list_ptr, OT_BLOCK *item);
bool ot_point_in_node (const Vector3d& point, const OT_ID *node);

/*****************************************************************************
*
* FUNCTION
*
*   ot_ins
*
* INPUT
*   The octree
*   The data to store
*   The oct-tree node id at which to store
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Called with a pointer to the root pointer, because this routine can
*   create a new root block higher up.
*
* THREAD SAFETY
*
*   This function is *NOT* THREAD-SAFE regarding concurrent modifications to
*   the tree.
*
*   This function ensures that tree integrity is maintained at any time,
*   extending the tree (if necessary) via functions that maintain tree
*   integrity themselves, and hooking in the new block only after it has been
*   fully built.
*
*   NOTE:
*
*   To ensure tree integrity, it is *MANDATORY* that the new block already
*   contains valid data except for the "next" pointer.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/
void ot_ins(OT_NODE **root_ptr, OT_BLOCK *new_block, const OT_ID *new_id)
{
    int target_size, dx, dy, dz, index;
    OT_NODE *temp_node, *this_node;
    OT_ID temp_id;

#ifdef RADSTATS
    ot_inscount++;
#endif

    // If there is no root yet, create one.  This is a first-time-through

    if (*root_ptr == nullptr)
    {
        *root_ptr = new OT_NODE;

#ifdef RADSTATS
        ot_nodecount = 1;
#endif

        // Might as well make it the right size for our first data block

        (*root_ptr)->Id = *new_id;
    }

    // What if the thing we're inserting is bigger than the biggest node in the
    // existing tree?  Add a new top to the tree till it's big enough.

    while ((*root_ptr)->Id.Size < new_id->Size)
    {
        // root too small

        ot_newroot(root_ptr);
    }

    // What if the new block is the right size, but for an area of space which
    // does not overlap with the current tree?  New bigger root, until the
    // areas overlap.

    // Build a temp id, like a cursor to move around with

    temp_id = *new_id;

    // First, find the parent of our new node which is as big as root

    while (temp_id.Size < (*root_ptr)->Id.Size)
    {
        ot_parent(&temp_id, &temp_id);
    }

    while((temp_id.x != (*root_ptr)->Id.x) ||
          (temp_id.y != (*root_ptr)->Id.y) ||
          (temp_id.z != (*root_ptr)->Id.z))
    {
        // while separate subtrees...

        ot_newroot(root_ptr);       // create bigger root

        ot_parent(&temp_id, &temp_id);      // and move cursor up one, too
    }

    // At this point, the new node is known to fit under the current tree
    // somewhere.  Go back down the tree to the right level, making new nodes
    // as you go.

    this_node = *root_ptr;        // start at the root

    while (this_node->Id.Size > new_id->Size)
    {
        // First, pick the node id of the child we are talking about

        target_size = this_node->Id.Size - 1;       // this is the size we want

        temp_id = *new_id;  // start with the new one

        while (temp_id.Size < target_size)
        {
            ot_parent(&temp_id, &temp_id);    // climb up till one below here
        }

        // Now we have to pick which child number we are talking about

        dx = (temp_id.x & 1) * 4;
        dy = (temp_id.y & 1) * 2;
        dz = (temp_id.z & 1);

        index = dx + dy + dz;

        if (this_node->Kids[index] == nullptr)
        {
            // Next level down doesn't exist yet, so create it
            temp_node = new OT_NODE;

#ifdef RADSTATS
            ot_nodecount++;
#endif

            // Fill in the data
            temp_node->Id = temp_id;
            // (all other data fields are automatically zeroed by the allocation function)

            // Add it onto the tree
            this_node->Kids[index] = temp_node;
        }

        // Now follow it down and repeat
        this_node = this_node->Kids[index];
    }

    // Finally, we're in the right place, so insert the new value
    ot_list_insert(&(this_node->Values), new_block);
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_list_insert
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   -
*
* THREAD SAFETY
*
*   This function is *NOT* THREAD-SAFE regarding concurrent modifications to
*   the tree.
*
*   This function ensures that tree integrity is maintained at any time,
*   hooking in the new block only after it has been fully built.
*
*   NOTE:
*
*   To ensure tree integrity, it is *MANDATORY* that the new block already
*   contains valid data except for the "next" pointer.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_list_insert(OT_BLOCK **list_head, OT_BLOCK *new_block)
{
    new_block->next = *list_head; // copy addr of old first block

    *list_head = new_block;
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_newroot
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Modify a tree so that it has a bigger root, owning the old root passed in.
*   Note that this function is called with a POINTER TO the root pointer,
*   since the root pointer will be changed.
*
* THREAD SAFETY
*
*   This function is *NOT* THREAD-SAFE regarding concurrent modifications to
*   the tree.
*
*   This function ensures that tree integrity is maintained at any time,
*   hooking in the new node only after it has been fully built.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_newroot(OT_NODE **root_ptr)
{
    OT_NODE *newroot;
    int dx, dy, dz, index;

    newroot = new OT_NODE;

#ifdef RADSTATS
    ot_nodecount++;
#endif
    ot_parent(&newroot->Id, &((*root_ptr)->Id));  // sets the x/y/z/size id

    // Function:  decide which child of the new root the old root is. Theory:
    // x,y,z values are measured in block sizes, and are a factor of 2 smaller
    // at each level higher.  The parent of both (3,4,5,k) and (2,5,4,k) is
    // (1,2,2,k+1), so the oddness of the child's ordinates determines which
    // child it is, and hence the value of the index into the parent's array of
    // children.  First half of array (4 entries) is kids with low/even x;
    // First half of those is kids with low/even y (2 entries), and the very
    // first entry is the one with low/even everything.
    dx = ((*root_ptr)->Id.x & 1) * 4;
    dy = ((*root_ptr)->Id.y & 1) * 2;
    dz = ((*root_ptr)->Id.z & 1);
    index = dx + dy + dz;
    newroot->Kids[index] = *root_ptr;
    *root_ptr = newroot;
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_dist_traverse
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Call "function(&node, handle)" for every node which is less than a node
*   width from the test point. Post traverse = small stuff first = the kids
*   before this node. "function(*node, handle)" must return true/false on
*   whether or not to continue with further processing.  Returns false if
*   execution was halted this way, true otherwise;
*
* THREAD SAFETY
*
*   This function is robust regarding the following modifications to the tree by
*   other threads:
*   - inserting a new parent node, provided the new parent node already
*     contains valid data, and the proper "Kids[n]" pointer already references
*     the old root node
*   - adding a new child node anywhere in the tree, provided the new node
*     already contains valid data
*   - inserting a new block anywhere in a block list (including the head or
*     tail), provided the new block already contains valid data, and the
*     "next" pointer already references the block before which is to be
*     inserted
*
*   In essence, this means that the code is robust regarding any additions
*   to the tree by other threads, provided that they are done according to
*   best practice.
*
*   This function may - or may not - ignore elements currently being added to
*   the tree by other threads.
*
*   NOTE:
*
*   This function is *NOT* THREAD-SAFE regarding modifications to existing tree
*   data, except as necessary to add new elements.
*
*   Statistics activated by the RADSTATS macro are *NOT* THREAD-SAFE by design.
*
*   This function is *NOT* THREAD-SAFE on machines where pointer copying is a
*   non-atomic operation.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

bool ot_dist_traverse(OT_NODE *subtree, const Vector3d& point, int bounce_depth, bool (*function)(OT_BLOCK *block, void *handle1), void *handle)
// only those blocks with this recur depth
{
#ifdef RADSTATS
    extern long ot_seenodecount, ot_seeblockcount;
#endif

    int i;
    OT_NODE *this_node;
    OT_BLOCK *this_block;

#ifdef RADSTATS
    ot_seenodecount++;
#endif

    // First, recurse to the child nodes
    for (i = 0; i < 8 ; i++)
    {   // for each potential kid
        this_node = subtree->Kids[i];
        if (this_node != nullptr)
        {   // ... which exists
            if (ot_point_in_node(point, &this_node->Id))
            {   // ... and is in range
                if(!ot_dist_traverse(this_node, point, bounce_depth, function, handle))
                    return false;
            }
        }
    }

    // Now, call the specified routine for each data block hung off this tree
    // node

    // if ( ot_point_in_node(point, &subtree->Id) )
    {
        this_block = subtree->Values;
        while (this_block != nullptr)
        {
#ifdef RADSTATS
            if ((subtree->Id.Size < Pow2Bias - 27) || (subtree->Id.Size > Pow2Bias + 13))
            {
                Debug_Info("bounds error, unreasonable size %d\n", subtree->Id.Size);
            }
            ot_seeblockcount++;
#endif
            if ((int)this_block->Bounce_Depth == bounce_depth)
            {
                //oksofar = (*function) (this_block, handle);
                if (!( (*function) (this_block, handle)))
                    return false;
            }
            this_block = this_block->next;
        }
    }

    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   ot_traverse - call a function for every block in the tree.
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
* Call "function(&block, handle)" for every block hanging off every node.
*   Post traverse = small stuff first = the kids before this node.
*   "function(*node, handle)" must return true/false on whether or not to
*   Continue with further processing.  Returns false if execution
*   was halted this way, true otherwise;
*
* THREAD SAFETY
*
*   This function is robust regarding the following modifications to the tree by
*   other threads:
*   - inserting a new parent node, provided the new parent node already
*     contains valid data, and the proper "Kids[n]" pointer already references
*     the old root node
*   - adding a new child node anywhere in the tree, provided the new node
*     already contains valid data
*   - inserting a new block anywhere in a block list (including the head or
*     tail), provided the new block already contains valid data, and the
*     "next" pointer already references the block before which is to be
*     inserted
*
*   In essence, this means that the code is robust regarding any additions
*   to the tree by other threads, provided that they are done according to
*   best practice.
*
*   This function may - or may not - ignore elements currently being added to
*   the tree by other threads.
*
*   NOTE:
*
*   This function is *NOT* THREAD-SAFE regarding modifications to existing tree
*   data, except as necessary to add new elements.
*
*   Statistics activated by the RADSTATS macro are *NOT* THREAD-SAFE by design.
*
*   This function is *NOT* THREAD-SAFE on machines where pointer copying is a
*   non-atomic operation.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

bool ot_traverse(OT_NODE *subtree, bool (*function)(OT_BLOCK * bl, void * handle1), void *handle)
{
    int i = 0;
    bool oksofar = true;
    OT_NODE *this_node = nullptr;
    OT_BLOCK *this_block = nullptr;


    // First, recurse to the child nodes
    if (subtree != nullptr)
    {
        for (i=0; i<8 && oksofar; i++ )     // for each potential kid
        {
            this_node = subtree->Kids[i];
            if (this_node != nullptr)          // ...which exists
            {
                oksofar = ot_traverse(this_node, function, handle);
            }
        }

        // Now, call the specified routine for each data block hung off this tree node
        this_block = subtree->Values;
        while (oksofar && (this_block != nullptr))
        {
            oksofar = (*function)(this_block, handle);
            this_block = this_block->next;
        }
    }

    return oksofar;
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_point_in_node
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Returns true if the specified point is inside the max extent of the node
*   with the specified ID.
*
* THREAD SAFETY
*
*   This function is thread-safe regarding any modifications to the tree,
*   provided they do not change the OT_ID referenced by the id parameter.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

inline bool ot_point_in_node(const Vector3d& point, const OT_ID *id)
{
    DBL sized;

    // sized = 2.0^(size-Pow2Bias)
    sized = BiasedIntPow2(id->Size);

    if (fabs(point.x() + OT_BIAS - ((DBL) id->x + 0.5) * sized) >= sized) return false;
    if (fabs(point.y() + OT_BIAS - ((DBL) id->y + 0.5) * sized) >= sized) return false;
    if (fabs(point.z() + OT_BIAS - ((DBL) id->z + 0.5) * sized) >= sized) return false;

    return true;
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_index_sphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Return the oct-tree index for an object with the specified bounding
*   sphere. This is the smallest box in the tree that this object fits in with
*   a maximum 50% hand-over in any (or all) directions. For example, an object
*   at (.49, .49, 49) of radius 1 fits in the box (0,0,0) size Pow2Bias (length 1).
*
* THREAD SAFETY
*
*   This function is thread-safe regarding any modifications to the tree,
*   provided they do not change the OT_ID referenced by the id parameter.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_index_sphere(const Vector3d& point, DBL radius, OT_ID *id)
{
    Vector3d min_point, max_point;

    // TODO FIXME - This transformation from center/radius to min/max is a waste of effort,
    //              as `ot_index_box` will actually convert it back to center/radius anyway.

    min_point = point - radius;
    max_point = point + radius;

    ot_index_box(min_point, max_point, id);

#ifdef RADSTATS
    if (id->Size < ot_minsize)
    {
        ot_minsize = id->Size;
    }
    if (id->Size > ot_maxsize)
    {
        ot_maxsize = id->Size;
    }
#endif
}




/*****************************************************************************
*
* FUNCTION
*
*   ot_index_box
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Return the oct-tree index for an object with the specified bounding box.
*   min_point is lox, loy, loz; max_point is hix, hiy, hiz. This is the
*   smallest box in the tree that this object fits in with a maximum 50%
*   hang-over in any (or all) directions. For example, an object with extent
*   (-.49, -.49, -49) to (1.49, 1.49, 1.49) is the largest that fits in the
*   box (0,0,0) with size Pow2Bias (length 1).
*
* THREAD SAFETY
*
*   This function is thread-safe regarding any modifications to the tree,
*   provided they do not change the OT_ID referenced by the id parameter.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_index_box(const Vector3d& min_point, const Vector3d& max_point, OT_ID *id)
{
// TODO OPTIMIZE

    DBL dx, dy, dz, desiredSize;
    DBL bsized, maxord;

    // Calculate the absolute minimum required size of the node, assuming it is perfectly centered within the node;
    // Node size must be a power of 2, and be large enough to accommodate box's biggest dimensions with maximum overhang to all sides

    // compute ideal size of the node for a perfect fit without any overhang
    dx = max_point.x() - min_point.x();
    dy = max_point.y() - min_point.y();
    dz = max_point.z() - min_point.z();
    desiredSize = max3(dx, dy, dz);

    // compute ideal size of the node for a perfect fit with full overhang to all sides
    // desiredSize /= (1 + 2 * 0.5);

    // compute best-matching power-of-two size for a perfect fit with overhang
    // (Note: theoretically this might pick a size larger than required if desiredSize is already a power of two)
    // desiredSize *= 2.0;
    bsized = Pow2Floor(desiredSize);

    // avoid divisions by zero
    if(bsized == 0.0)
        bsized = 1.0;

#ifdef SAFE_METHOD

    // This block checks for the case where the node id would cause integer
    // overflow, since it is a small buffer far away
    maxord = max3(fabs(min_point[X]), fabs(min_point[Y]), fabs(min_point[Z]));
    maxord += OT_BIAS;
    while (maxord / bsized > 1000000000.0)
    {
#ifdef RADSTATS
        overflows++;
#endif
        bsized *= 2.0;
    }
#endif // SAFE_METHOD

    // The node we chose so far would be ideal for a box of identical size positioned at the node's center,
    // but the actual box is probably somewhat off-center and therefore may have excessive overhang in some directions;
    // check and possibly fix this.

    Vector3d center = (min_point + max_point) / 2;
    id->x = (int) floor((center[X] + OT_BIAS) / bsized);
    id->y = (int) floor((center[Y] + OT_BIAS) / bsized);
    id->z = (int) floor((center[Z] + OT_BIAS) / bsized);
    id->Size = BiasedIntLog2(bsized);

#ifdef RADSTATS
    thisloops = 0;
#endif
    while (!ot_point_in_node(min_point, id) || !ot_point_in_node(max_point, id))
    {
        // Debug_Info("looping %d,%d,%d,%d  min=%d, max=%d\n", test_id.x, test_id.y,
        // test_id.z, test_id.Size, ot_point_in_node(min_point, &test_id),
        // ot_point_in_node(max_point, &test_id));
        ot_parent(id, id);
#ifdef RADSTATS
        totloops++;
        thisloops++;
#endif
    }
#ifdef RADSTATS
    if (thisloops < minloops)
        minloops = thisloops;
    if (thisloops > maxloops)
        maxloops = thisloops;
#endif

#ifdef OT_DEBUG
    if (id->Size > Pow2Bias + 12)
    {
        Debug_Info("unusually large id, maxdel=%.4f, bsized=%.4f, isize=%d\n",
                   maxdel, bsized, id->Size);
    }
#endif
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_parent
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Set the x/y/z/size block ID info of dad = the parent ID of kid
*
* THREAD SAFETY
*
*   This function is thread-safe regarding any modifications to the tree,
*   provided they do not change the OT_ID referenced by the kid_id parameter.
*
*   NOTE:
*
*   This function changes the OT_ID referenced by the dad_id parameter.
*   It must therefore be used *ONLY* on nodes not hooked into the tree yet.
*
* CHANGES
*
*   --- 1994 : Creation.
*   Apr 2000 : changed (kid_id->? - 1) to (kid_id->? + 1)
*
******************************************************************************/

void ot_parent(OT_ID *dad_id, OT_ID  *kid_id)
{
    dad_id->Size = kid_id->Size + 1;
    // Theoretically, (0:1) should be parented by (0), while (-2:-1) should be parented by (-1).
    // In practice, parenting both by (0) makes the code more robust in case we ever encounter
    // that region, because otherwise we would enter an infinite loop trying to find a common parent.
    // (That doesn't mean that all is well in that region; we're just avoiding a catastrophe.)
#if 1
    //  note that it parents (-2:-1) by (0)
    dad_id->x = (kid_id->x >= 0) ? (kid_id->x >> 1) : (kid_id->x + 1) / 2;
    dad_id->y = (kid_id->y >= 0) ? (kid_id->y >> 1) : (kid_id->y + 1) / 2;
    dad_id->z = (kid_id->z >= 0) ? (kid_id->z >> 1) : (kid_id->z + 1) / 2;
#else
    //  To parent (-2:-1) by (-1), this code would be used:
    dad_id->x = (kid_id->x >= 0) ? (kid_id->x >> 1) : (kid_id->x - 1) / 2;
    dad_id->y = (kid_id->y >= 0) ? (kid_id->y >> 1) : (kid_id->y - 1) / 2;
    dad_id->z = (kid_id->z >= 0) ? (kid_id->z >> 1) : (kid_id->z - 1) / 2;
#endif
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_save_tree
*
* INPUT
*
* OUTPUT
*
* RETURNS 1 for success, 0 for failure.
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Given the root pointer of the in-memory cache tree, and a file descriptor
*   of a file you want to write to, write the whole tree to that file.
*
* CHANGES
*
*   Jan 1996 : Creation by JDM.
*
* THREAD SAFETY
*
*   This function is robust regarding the following modifications to the tree by
*   other threads:
*   - inserting a new parent node, provided the new parent node already
*     contains valid data, and the proper "Kids[n]" pointer already references
*     the old root node
*   - adding a new child node anywhere in the tree, provided the new node
*     already contains valid data
*   - inserting a new block anywhere in a block list (including the head or
*     tail), provided the new block already contains valid data, and the
*     "next" pointer already references the block before which is to be
*     inserted
*
*   In essence, this means that the code is robust regarding any additions
*   to the tree by other threads, provided that they are done according to
*   best practice.
*
*   This function may - or may not - ignore elements currently being added to
*   the tree by other threads.
*
*   NOTE:
*
*   This function is *NOT* THREAD-SAFE regarding modifications to existing tree
*   data, except as necessary to add new elements.
*
*   Statistics activated by the RADSTATS macro are *NOT* THREAD-SAFE by design.
*
*   This function is *NOT* THREAD-SAFE on machines where pointer copying is a
*   non-atomic operation.
*
* TO DO
*
*  Code must be written which turns Radiosity_File_*  flags on and off.
*  These flags should be in the opts structure.
*
******************************************************************************/

bool ot_save_tree(OT_NODE *root, OStream *fd)
{
    bool retval = false;

    if (fd != nullptr)
        retval = ot_traverse(root, ot_write_block, reinterpret_cast<void *>(fd));
    else
;// TODO MESSAGE    Warning("Bad radiosity cache file handle");

    return retval;
}



/*****************************************************************************
*
* FUNCTION
*
*   ot_write_block
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Write one block (not a node) from the memory cache to the cache file.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

bool ot_write_block(OT_BLOCK *bl, void *fd) // must be passed as void * for compatibility
{
    (reinterpret_cast<OStream *>(fd))->printf("C%d\t%g\t%g\t%g\t%02x%02x%02x\t%.4f\t%.4f\t%.4f\t%g\t%g\t%02x%02x%02x\n", // tw
        (int)(bl->Bounce_Depth + 1), // file format still uses 1-based bounce depth counting

        bl->Point[X], bl->Point[Y], bl->Point[Z],
        (int)((bl->S_Normal[X]+1.)*.5*254.+.499999),
        (int)((bl->S_Normal[Y]+1.)*.5*254.+.499999),
        (int)((bl->S_Normal[Z]+1.)*.5*254.+.499999),

#if (NUM_COLOUR_CHANNELS == 3)
        bl->Illuminance.Red(), bl->Illuminance.Green(), bl->Illuminance.Blue(),
#else
        #error "TODO!"
#endif
        bl->Harmonic_Mean_Distance,

        bl->Nearest_Distance,
        (int)((bl->To_Nearest_Surface[X]+1.)*.5*254.+.499999),
        (int)((bl->To_Nearest_Surface[Y]+1.)*.5*254.+.499999),
        (int)((bl->To_Nearest_Surface[Z]+1.)*.5*254.+.499999)

        // TODO - write Quality and Brilliance
    );
    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   ot_free_tree() - get rid of the entire in-memory radiosity cache tree,
*   and zero the pointer to its root.
*
* INPUT - pointer to the tree root pointer.
*
* RETURNS - success 1, failure 0
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Free a complete radiosity cache tree, and all of its nodes and blocks.
*   NOTE parameter is a pointer to the tree pointer...tree pointer will get zeroed.
*   Example call:
*      ot_free_tree(&ot_root);
*   Returns true for success, false for failure.
*
* THREAD SAFETY
*
*   This function is *NOT THREAD-SAFE*.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

bool ot_free_tree(OT_NODE **ppRoot)
{
    bool all_ok = ot_free_subtree(*ppRoot);

    *ppRoot = nullptr;

    return all_ok;
}


/*****************************************************************************
*
* FUNCTION
*
*   ot_free_subtree - free every node from this node downwards, and all blocks
*   hanging off those nodes, and then free the node which was passed.
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Free this subtree.  That is, free all of its daughters, then
*   free all of the blocks hanging off this node, then free this node itself.
*
*   Returns false if problems were encountered anywhere in the tree.
*   Currently, this code assumes success.  If called with an invalid tree pointer,
*   it would probably crash with a memory protection error.
*
* THREAD SAFETY
*
*   This function is *NOT THREAD-SAFE*.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

bool ot_free_subtree(OT_NODE *subtree)
{
    int i;
    OT_NODE *this_node;

    // First, recurse to the child nodes
    for (i = 0; i < 8; i++)   // for each potential kid
    {
        this_node = subtree->Kids[i];
        if (this_node != nullptr) {        // ...which exists
            ot_free_subtree(this_node);
        }
    }

    // Finally, free this block itself
    delete subtree;

    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   ot_read_file
*
* INPUT
*   file descriptor handle of file (already opened) to read into memory.
*
* OUTPUT
*
* RETURNS - Success 1 / failure 0
*
* AUTHOUR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   Read in a radiosity cache file, building a tree from its values.
*   If there is an existing tree, these values are added to it.
*
* THREAD SAFETY
*
*   This function is *NOT THREAD-SAFE*.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/

bool ot_read_file(OT_NODE **root, IStream *fd, const OT_READ_PARAM* param, OT_READ_INFO* info)
{
    bool retval, got_eof;
    int line_num = 0;
    int tempdepth, tx, ty, tz;
    int goodreads = 0;
    int count;
    bool goodparse = true;
    DBL brightness;
    OT_BLOCK bl;
    OT_BLOCK *new_block;
    OT_ID id;
    char normal_string[30], to_nearest_string[30];
    char line[101];

    memset(&bl, 0, sizeof(OT_BLOCK));

    if (fd != nullptr)
    {
        info->Gather_Total.Clear();
        info->Gather_Total_Count = 0;

        while (!(got_eof = !fd->getline (line, 99)) && goodparse)
        {
            switch ( line[0] )
            {
                case 'B':    // the file contains the old radiosity_brightness value
                {
                    if ( sscanf(line, "B%lf\n", &brightness) == 1 )
                    {
                        info->Brightness = brightness;
                    }
                    break;
                }
                case 'P':    // the file made it to the point that the Preview was done
                {
                    info->FirstRadiosityPass = true;
                    break;
                }
                case 'C':
                {
#if (NUM_COLOUR_CHANNELS == 3)
                    RGBColour tempCol;
                    count = sscanf(line, "C%d %lf %lf %lf %s %f %f %f %f %f %s\n", // tw
                                   &tempdepth,      // since you can't scan a short
                                   &bl.Point[X], &bl.Point[Y], &bl.Point[Z],
                                   normal_string,
                                   &tempCol.red(), &tempCol.green(), &tempCol.blue(),
                                   &bl.Harmonic_Mean_Distance,
                                   &bl.Nearest_Distance, to_nearest_string );
                    bl.Illuminance = ToMathColour(tempCol);
#else
                    #error "TODO!"
#endif

                    // TODO FIXME - read Quality and Brilliance

                    if ( count == 11 )
                    {
                        bl.Bounce_Depth = (short)tempdepth - 1;

                        // normals aren't very critical for direction precision, so they are packed
                        sscanf(normal_string, "%02x%02x%02x", &tx, &ty, &tz);
                        bl.S_Normal[X] = ((double)tx * (1./ 254.))*2.-1.;
                        bl.S_Normal[Y] = ((double)ty * (1./ 254.))*2.-1.;
                        bl.S_Normal[Z] = ((double)tz * (1./ 254.))*2.-1.;
                        bl.S_Normal.normalize();

                        sscanf(to_nearest_string, "%02x%02x%02x", &tx, &ty, &tz);
                        bl.To_Nearest_Surface[X] = ((double)tx * (1./ 254.))*2.-1.;
                        bl.To_Nearest_Surface[Y] = ((double)ty * (1./ 254.))*2.-1.;
                        bl.To_Nearest_Surface[Z] = ((double)tz * (1./ 254.))*2.-1.;
                        bl.To_Nearest_Surface.normalize();

                        line_num++;

                        new_block = reinterpret_cast<OT_BLOCK *>(POV_MALLOC(sizeof (OT_BLOCK), "octree node from file"));
                        if (new_block != nullptr)
                        {
                            std::memcpy(new_block, &bl, sizeof (OT_BLOCK));

                            ot_index_sphere(bl.Point, bl.Harmonic_Mean_Distance * param->RealErrorBound, &id);
                            ot_ins(root, new_block, &id);
                            goodreads++;
                        }
                        else
                        {
                            goodparse = false;    // allocation error, better stop now
                        }
                    }
                    break;
                }

                default:
                {
                    // wrong leading character on line, just try again on next line
                }

            }   // end switch
        } // end while-reading loop

        if ( !got_eof  ||  !goodparse ) {
;// TODO MESSAGE      PossibleError("Cannot process radiosity cache file at line %d.", (int)line_num);
            retval = false;
        }
        else
        {
            if ( goodreads > 0 )
;// TODO MESSAGE         Debug_Info("Reloaded %d values from radiosity cache file.\n", goodreads);
            else
;// TODO MESSAGE         PossibleError("Unable to read any values from the radiosity cache file.");
            retval = true;
        }
    }
    else
    {
        retval = false;
    }

    return retval;
}

}
// end of namespace pov
