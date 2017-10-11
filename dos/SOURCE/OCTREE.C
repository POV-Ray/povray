/****************************************************************************
*                   octree.c
*
*  This module contains all oct-tree functions for radiosity.
*
*  This file was written by Jim McElhiney.
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
* Modifications by Thomas Willhalm, March 1999, used with permission
*
*****************************************************************************/

/************************************************************************
*  Oct-tree routines.  Used by Radiosity calculation routies.
*
*  To understand the relationship between an ot_id (x,y,z,size) and
*  a place in model space, you have to scale the integer values:
*  The nominal space occupied is given as follows:
*      fsize = pow(2,size-127);
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

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "povray.h"
#include "octree.h"
#include "radiosit.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define SAFE_METHOD 1
/* #define OT_DEBUG 1 */



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/

#ifdef RADSTATS
long ot_inscount = 0;
long ot_nodecount = 0;
long ot_blockcount = 0;
long ot_minsize = 1000;
long ot_maxsize = 0;
#endif

#ifdef RADSTATS
long overflows = 0;
long thisloops = 0;
long totloops = 0;
long minloops = 1000;
long maxloops = 0;
#endif



/*****************************************************************************
* Static functions
******************************************************************************/

long ot_save_node (VECTOR point, OT_ID *node);
long ot_traverse (OT_NODE *subtree, long (*function)(OT_BLOCK *block, void * handle1), void * handle2);
long ot_free_subtree (OT_NODE *node);





/*****************************************************************************
*
* FUNCTION
*
*   ot_ins
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
*   Called with a pointer to the root pointer, because this routine can
*   create a new root block higher up.
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/
/* The data to store */
/* The oct-tree node id at which to store */
void ot_ins(OT_NODE **root_ptr, OT_BLOCK *new_block, OT_ID *new_id)
{
  long target_size, dx, dy, dz, index;
  OT_NODE *temp_node, *this_node;
  OT_ID temp_id;

#ifdef RADSTATS
  ot_inscount++;
#endif

  /* If there is no root yet, create one.  This is a first-time-through */

  if (*root_ptr == NULL)
  {
    *root_ptr = (OT_NODE *)POV_CALLOC(1, sizeof(OT_NODE), "octree node");

#ifdef RADSTATS
    ot_nodecount = 1;
#endif

    /* Might as well make it the right size for our first data block */

    (*root_ptr)->Id = *new_id;
  }

  /*
   * What if the thing we're inserting is bigger than the biggest node in the
   * existing tree?  Add a new top to the tree till it's big enough.
   */

  while ((*root_ptr)->Id.Size < new_id->Size)
  {
    /* root too small */

    ot_newroot(root_ptr);
  }

  /*
   * What if the new block is the right size, but for an area of space which
   * does not overlap with the current tree?  New bigger root, until the
   * areas overlap.
   */

  /* Build a temp id, like a cursor to move around with */

  temp_id = *new_id;

  /* First, find the parent of our new node which is as big as root */

  while (temp_id.Size < (*root_ptr)->Id.Size)
  {
    ot_parent(&temp_id, &temp_id);
  }

  while((temp_id.x != (*root_ptr)->Id.x) ||
        (temp_id.y != (*root_ptr)->Id.y) ||
        (temp_id.z != (*root_ptr)->Id.z))
  {
    /* while separate subtrees... */

    ot_newroot(root_ptr);       /* create bigger root */

    ot_parent(&temp_id, &temp_id);      /* and move cursor up one, too */
  }

  /*
   * At this point, the new node is known to fit under the current tree
   * somewhere.  Go back down the tree to the right level, making new nodes
   * as you go.
   */

  this_node = *root_ptr;        /* start at the root */

  while (this_node->Id.Size > new_id->Size)
  {
    /* First, pick the node id of the child we are talking about */

    target_size = this_node->Id.Size - 1;       /* this is the size we want */

    temp_id = *new_id;  /* start with the new one */

    while (temp_id.Size < target_size)
    {
      ot_parent(&temp_id, &temp_id);    /* climb up till one below here */
    }

    /* Now we have to pick which child number we are talking about */

    dx = (temp_id.x & 1) * 4;
    dy = (temp_id.y & 1) * 2;
    dz = (temp_id.z & 1);

    index = dx + dy + dz;

    if (this_node->Kids[index] == NULL)
    {
      /* Next level down doesn't exist yet, so create it */
      temp_node = (OT_NODE *)POV_CALLOC(1, sizeof(OT_NODE), "octree node");

#ifdef RADSTATS
      ot_nodecount++;
#endif

      /* Fill in the data */
      temp_node->Id = temp_id;

      /* Add it onto the tree */
      this_node->Kids[index] = temp_node;
    }

    /* Now follow it down and repeat */
    this_node = this_node->Kids[index];
  }

  /* Finally, we're in the right place, so insert the new value */
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
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_list_insert(OT_BLOCK **list_head, OT_BLOCK *new_block)
{
  new_block->next = *list_head; /* copy addr of old first block */

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
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_newroot(OT_NODE **root_ptr)
{
  OT_NODE *newroot;
  long dx, dy, dz, index;

  newroot = (OT_NODE *)POV_CALLOC(1, sizeof(OT_NODE), "octree node");

#ifdef RADSTATS
  ot_nodecount++;
#endif
  ot_parent(&newroot->Id, &((*root_ptr)->Id));  /* sets the x/y/z/size id */

  /*
   * Function:  decide which child of the new root the old root is. Theory:
   * x,y,z values are measured in block sizes, and are a factor of 2 smaller
   * at each level higher.  The parent of both (3,4,5,k) and (2,5,4,k) is
   * (1,2,2,k+1), so the oddness of the child's ordinates determines which
   * child it is, and hence the value of the index into the parent's array of
   * children.  First half of array (4 entries) is kids with low/even x;
   * First half of those is kids with low/even y (2 entries), and the very
   * first entry is the one with low/even everything.
   */
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
*   whether or not to continue with further processing.  Returns 0 if
*   execution was halted this way, 1 otherwise;
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

long ot_dist_traverse(OT_NODE *subtree, VECTOR point, int bounce_depth, long (*function)(OT_BLOCK *block, void *handle1), void *handle)
/* only those nodes with this recur depth */
{
#ifdef RADSTATS
  extern long ot_seenodecount, ot_seeblockcount;

#endif
  long i, oksofar;
  OT_NODE *this_node;
  OT_BLOCK *this_block;

#ifdef RADSTATS
  ot_seenodecount++;
#endif

  /* First, recurse to the child nodes */
  oksofar = 1;
  for (i = 0; i < 8 && oksofar; i++)
  {     /* for each potential kid */
    this_node = subtree->Kids[i];
    if (this_node != NULL)
    {   /* ...which exists */
      if (ot_point_in_node(point, &this_node->Id))
      { /* ...and in range */
        oksofar = ot_dist_traverse(this_node, point, bounce_depth,
                                   function, handle);
      }
    }
  }

  /*
   * Now, call the specified routine for each data block hung off this tree
   * node
   */

  /* if ( ot_point_in_node(point, &subtree->Id) ) { */
  {
    this_block = subtree->Values;
    while (oksofar && (this_block != NULL))
    {
#ifdef RADSTATS
      if (subtree->Id.Size < 100 || subtree->Id.Size > 140 )
      {
        Debug_Info("bounds error, unreasonable size %d\n", subtree->Id.Size);
      }
      ot_seeblockcount++;
#endif
      if ((int)this_block->Bounce_Depth == bounce_depth)
      {
        oksofar = (*function) (this_block, handle);
      }
      this_block = this_block->next;
    }
  }

  return oksofar;
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
*   Continue with further processing.  Returns 0 if execution
*   was halted this way, 1 otherwise;
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
long
ot_traverse(OT_NODE *subtree, long (*function)(OT_BLOCK * bl, void * handle1), void *handle)
/* Call "function(&block, handle)" for every block hanging off every node.
   Post traverse = small stuff first = the kids before this node.
   "function(*node, handle)" must return true/false on whether or not to
   Continue with further processing.  Returns 0 if execution
   was halted this way, 1 otherwise;
*/
{
  long i, oksofar;
  OT_NODE *this_node;
  OT_BLOCK *this_block;


  /* First, recurse to the child nodes */
  oksofar = 1;
  if (subtree!=NULL)
  {
    
    for (i=0; i<8 && oksofar; i++ )     /* for each potential kid */
    {
      this_node = subtree->Kids[i];
      if ( this_node != NULL )          /* ...which exists */
      {
        oksofar = ot_traverse(this_node, function, handle);
      }
    }

    /* Now, call the specified routine for each data block hung off
       this tree node */
    this_block = subtree->Values;
    while ( oksofar  &&  (this_block != NULL) )
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
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

long ot_point_in_node(VECTOR point, OT_ID *id)
{
  DBL sized, minx, miny, minz, lox, loy, loz, hix, hiy, hiz;

  union
  {
    float f;
    long l;
  }
  size;                         /* MUST be float, NOT DBL */

  size.l = id->Size << 23;
  sized = (DBL) size.f;
  minx = (DBL) id->x * sized - OT_BIAS;
  miny = (DBL) id->y * sized - OT_BIAS;
  minz = (DBL) id->z * sized - OT_BIAS;

  lox = minx - sized * .5;
  hix = minx + sized * 1.5;
  loy = miny - sized * .5;
  hiy = miny + sized * 1.5;
  loz = minz - sized * .5;
  hiz = minz + sized * 1.5;

  return(point[X] >= lox && point[X] < hix &&
         point[Y] >= loy && point[Y] < hiy &&
         point[Z] >= loz && point[Z] < hiz);
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
*   at (.49, .49, 49) of radius 1 fits in the box (0,0,0) size 127 (length 1).
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_index_sphere(VECTOR point, DBL radius, OT_ID *id)
{
  VECTOR min_point, max_point;

  min_point[X] = point[X] - radius;
  min_point[Y] = point[Y] - radius;
  min_point[Z] = point[Z] - radius;
  max_point[X] = point[X] + radius;
  max_point[Y] = point[Y] + radius;
  max_point[Z] = point[Z] + radius;

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
*   near_point is lox, loy, loz; far_point is hix, hiy, hiz. This is the
*   smallest box in the tree that this object fits in with a maximum 50%
*   hang-over in any (or all) directions. For example, an object with extent
*   (-.49, -.49, -49) to (1.49, 1.49, 1.49) is the largest that fits in the
*   box (0,0,0) with size 127 (length 1).
*
*   PORTABILITY WARNING:  this function REQUIRES IEEE single precision floating
*   point format to work.  This is true of most common systems except VAXen,
*   Crays, and Alpha AXP in VAX compatibility mode.  Local "float" variables
*   can NOT be made double precision "double" or "DBL".
*
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_index_box(VECTOR min_point, VECTOR max_point, OT_ID *id)
{
  long done, idx, idy, idz;
  float dx, dy, dz, maxdel;     /* MUST BE "float" NOT "DBL" */
  DBL bsized, maxord;
  union
  {
    float f;
    long l;
  }
  convert;
  OT_ID base_id, test_id;

  dx = (float) (max_point[X] - min_point[X]);
  dy = (float) (max_point[Y] - min_point[Y]);
  dz = (float) (max_point[Z] - min_point[Z]);
  maxdel = MAX3(dx, dy, dz);

  /*
   * This hex operation does a floor to next lower power of 2, by clearing
   * all of the mantissa bits.  Works only on IEEE single precision floats
   */
  convert.f = maxdel;
  convert.l &= 0xff800000;
  bsized = (DBL) convert.f;

#ifdef SAFE_METHOD

  /*
   * This block checks for the case where the node id would cause integer
   * overflow, since it is a small buffer far away
   */
  maxord = MAX3(fabs(min_point[X]), fabs(min_point[Y]), fabs(min_point[Z]));
  maxord += OT_BIAS;
  while (maxord / bsized > 1000000000.)
  {
#ifdef RADSTATS
    overflows++;
#endif
    bsized *= 2.;
  }
#endif

  /* calculate the smallest possible node that the item might fit into */
  base_id.x = (long) floor((min_point[X] + OT_BIAS) / bsized);
  base_id.y = (long) floor((min_point[Y] + OT_BIAS) / bsized);
  base_id.z = (long) floor((min_point[Z] + OT_BIAS) / bsized);

  /*
   * This magic hex operation extracts the exponent, which gives us an
   * integer number suitable for labelling a range of a power of 2.  In IEEE
   * format, value = pow(2,exponent-127). Therefore, if our index is, say,
   * 129, then the item has a maximum extent of (2 to the (129-127)), or
   * about 4 space units.
   */
  convert.f = (float) bsized;
  base_id.Size = (convert.l & 0x7f800000) >> 23;

  /* Now increase the node size until it fits for sure */
#ifdef RADSTATS
  thisloops = 0;
#endif
  done = 0;
  while (!done)
  {
    test_id.Size = base_id.Size;
    for (idx = 0; idx < 2 && !done; idx++)
    {
      for (idy = 0; idy < 2 && !done; idy++)
      {
        for (idz = 0; idz < 2 && !done; idz++)
        {
          test_id.x = base_id.x + idx;
          test_id.y = base_id.y + idy;
          test_id.z = base_id.z + idz;
          if (ot_point_in_node(min_point, &test_id) &&
              ot_point_in_node(max_point, &test_id))
          {
            done = 1;
          }
        }
      }
    }

    /*
     * Debug_Info("looping %d,%d,%d,%d  min=%d, max=%d\n", test_id.x, test_id.y,
     * test_id.z, test_id.Size, ot_point_in_node(min_point, &test_id),
     * ot_point_in_node(max_point, &test_id));
     */
    ot_parent(&base_id, &base_id);
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
  *id = test_id;

#ifdef OT_DEBUG
  if (id->Size > 139)
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
* CHANGES
*
*   --- 1994 : Creation.
*
******************************************************************************/

void ot_parent(OT_ID *dad_id, OT_ID  *kid_id)
{
  dad_id->Size = kid_id->Size + 1;
  dad_id->x = (kid_id->x > 0) ? (kid_id->x >> 1) : (kid_id->x - 1) / 2;
  dad_id->y = (kid_id->y > 0) ? (kid_id->y >> 1) : (kid_id->y - 1) / 2;
  dad_id->z = (kid_id->z > 0) ? (kid_id->z >> 1) : (kid_id->z - 1) / 2;
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
* TO DO
*
*  Code must be written which turns Radiosity_File_*  flags on and off.
*  These flags should be in the opts structure.
*
******************************************************************************/
long
ot_save_tree(OT_NODE *root, FILE *fd)
{
  long retval = 0;

  if ( fd != NULL ) {
    retval = ot_traverse(root, ot_write_block, (void *)fd);
  }
  else
  {
    Warning(0.0, "bad radiosity cache file handle\n");
  }

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
long
ot_write_block/* must be passed as void * for compatibility */(OT_BLOCK *bl, void *fd)
{

  if ( bl->Bounce_Depth == 1 )
  {
    fprintf((FILE *)fd, "C%ld\t%g\t%g\t%g\t%02lx%02lx%02lx\t%.4f\t%.4f\t%.4f\t%g\t%g\t%02lx%02lx%02lx\n", /* tw */
      (long)bl->Bounce_Depth,

      bl->Point[X], bl->Point[Y], bl->Point[Z],
      (long)((bl->S_Normal[X]+1.)*.5*254.+.499999),
      (long)((bl->S_Normal[Y]+1.)*.5*254.+.499999),
      (long)((bl->S_Normal[Z]+1.)*.5*254.+.499999),

      bl->Illuminance[X], bl->Illuminance[Y], bl->Illuminance[Z],
      bl->Harmonic_Mean_Distance,
      
      bl->Nearest_Distance,
      (long)((bl->To_Nearest_Surface[X]+1.)*.5*254.+.499999),
      (long)((bl->To_Nearest_Surface[Y]+1.)*.5*254.+.499999),
      (long)((bl->To_Nearest_Surface[Z]+1.)*.5*254.+.499999)
    );
  }
  return 1;
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
*   Returns 1 for success, 0 for failure.
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
long
ot_free_tree(OT_NODE **ppRoot)
/* Free a complete radiosity cache tree, and all of its nodes and blocks.
   Note parameter is a pointer to the tree pointer...tree pointer will get zeroed.
   Example call:
      ot_free_tree(&ot_root);
   Returns 1 for success, 0 for failure.
*/
{
  long all_ok;

  all_ok = ot_free_subtree(*ppRoot);
  *ppRoot = NULL;

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
*   Set the x/y/z/size block ID info of dad = the parent ID of kid
*
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
long
ot_free_subtree(OT_NODE *subtree)
/* Free this subtree.  That is, free all of its daughters, then 
   free all of the blocks hanging off this node, then free this node itself.

   Returns 0 if problems were encountered anywhere in the tree.
   Currently, this code assumes success.  If called with an invalid tree pointer,
   it would probably crash with a memory protection error.
*/
{
  long i, oksofar;
  OT_NODE *this_node;
  OT_BLOCK *this_block, *next_block;

  /* First, recurse to the child nodes */
  oksofar = 1;
  for (i=0; i<8 && oksofar; i++ )   /* for each potential kid */
  {
    this_node = subtree->Kids[i];
    if ( this_node != NULL ) {      /* ...which exists */
      oksofar &= ot_free_subtree(this_node);
    }
  }

  /* Now, free each block hanging off this node. */
  this_block = subtree->Values;
  while ( this_block != NULL )
  {
    next_block = this_block->next;
    POV_FREE(this_block);
    this_block = next_block;
  }

  /* Finally, free this block itself */
  POV_FREE(subtree);

  return oksofar;
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
* CHANGES
*
*   --- Jan 1996 : Creation.
*
******************************************************************************/
long
ot_read_file(FILE *fd)
/* Read in a radiosity cache file, building a tree from its values.
   If there is an existing tree, these values are added to it.
*/
{
  long retval, line_num, tempdepth, tx, ty, tz, goodreads;
  int count, goodparse, readret;
  DBL brightness;
  OT_BLOCK bl, *new_block;
  OT_ID id;
  char normal_string[30], to_nearest_string[30], line[101];

  memset(&bl, 0, sizeof(OT_BLOCK));

  if ( fd != NULL )
  {
    line_num = 0;

    Make_Colour(Radiosity_Gather_Total, 0., 0., 0.);
    Radiosity_Gather_Total_Count = 0;

    goodparse = 1;
    goodreads = 0;

    while ( ((readret = fscanf(fd, "%99[^\n]\n", line)) == 1) && goodparse )
    {
      switch ( line[0] )
      {
        case 'B':    /* the file contains the old radiosity_brightness value */
        {
          if ( sscanf(line, "B%lf\n", &brightness) == 1 )
          {
            opts.Radiosity_Brightness = brightness;
          }
          break;
        }
        case 'P':    /* the file made it to the point that the Preview was done */
        {
          opts.Radiosity_Preview_Done = 1;
          break;
        }    
        case 'C':
        {
          count = sscanf(line, "C%ld %lf %lf %lf %s %f %f %f %f %f %s\n", /* tw */
		     &tempdepth,      /* since you can't scan a short */
                     &bl.Point[X], &bl.Point[Y], &bl.Point[Z],
                     normal_string,
                     &bl.Illuminance[X], &bl.Illuminance[Y], &bl.Illuminance[Z],
                     &bl.Harmonic_Mean_Distance,
                     &bl.Nearest_Distance, to_nearest_string );
          if ( count == 11 )
          {
            bl.Bounce_Depth = (short)tempdepth;

            /* normals aren't very critical for direction precision, so they are packed */
            sscanf(normal_string, "%02lx%02lx%02lx", &tx, &ty, &tz);
            bl.S_Normal[X] = ((double)tx * (1./ 254.))*2.-1.;
            bl.S_Normal[Y] = ((double)ty * (1./ 254.))*2.-1.;
            bl.S_Normal[Z] = ((double)tz * (1./ 254.))*2.-1.;
            VNormalizeEq(bl.S_Normal);

            sscanf(to_nearest_string, "%02lx%02lx%02lx", &tx, &ty, &tz);
            bl.To_Nearest_Surface[X] = ((double)tx * (1./ 254.))*2.-1.;
            bl.To_Nearest_Surface[Y] = ((double)ty * (1./ 254.))*2.-1.;
            bl.To_Nearest_Surface[Z] = ((double)tz * (1./ 254.))*2.-1.;
            VNormalizeEq(bl.To_Nearest_Surface);

            line_num++;

            new_block = (OT_BLOCK *)POV_MALLOC(sizeof (OT_BLOCK), "octree node from file");
            if ( new_block != NULL )
            {
              memcpy(new_block, &bl, sizeof (OT_BLOCK));

              ot_index_sphere(bl.Point, bl.Harmonic_Mean_Distance * opts.Radiosity_Error_Bound, &id);
              ot_ins(&ot_root, new_block, &id);
              goodreads++;
            }
            else
            {
              goodparse = 0;    /* allocation error, better stop now */
            }
          }
          break;
        }

        default:
        {
          /* wrong leading character on line, just try again on next line */
        }

      }   /* end switch */
    }     /* end while-reading loop */

    if ( (readret != EOF)  ||  !goodparse ) {
      Status_Info("\nError processing the radiosity cache file at line %ld", (long)line);
      retval = 0;
    }
    else
    {
      if ( goodreads > 0 )
      {
        Status_Info("\nReloaded %ld values from radiosity cache file.", goodreads);
      }
      else
      {
        Status_Info("\nUnable to read any values from the radiosity cache file.");
      }
      retval = 1;
    }
  }
  else
  {
    retval = 0;
  }

  return retval;
}
