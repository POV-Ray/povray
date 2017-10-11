/*****************************************************************************
*                bbox.c
*
*  This module implements the bounding box calculations.
*  This file was written by Alexander Enzmann.    He wrote the code for
*  POV-Ray's bounding boxes and generously provided us these enhancements.
*  The box intersection code was further hacked by Eric Haines to speed it up.
*
*  Just so everyone knows where this came from, the code is VERY heavily
*  based on the slab code from Mark VandeWettering's MTV raytracer.
*  POV-Ray is just joining the crowd of admirers of Mark's contribution to
*  the public domain. [ARE]
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
*****************************************************************************/

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "bbox.h"
#include "matrices.h"
#include "objects.h"
#include "povray.h"
#include "render.h"



/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define BUNCHING_FACTOR 4

/* Initial number of entries in a priority queue. */

#define INITIAL_PRIORITY_QUEUE_SIZE 256



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static BBOX_TREE *create_bbox_node (int size);

static int find_axis (BBOX_TREE **Finite, long first, long last);
static void calc_bbox (BBOX *BBox, BBOX_TREE **Finite, long first, long last);
static void build_area_table (BBOX_TREE **Finite, long a, long b, DBL *areas);
static int sort_and_split (BBOX_TREE **Root, BBOX_TREE **Finite, long *nFinite, long first, long last);

static void priority_queue_insert (PRIORITY_QUEUE *Queue, DBL Depth, BBOX_TREE *Node);

static int CDECL compboxes (CONST void *in_a, CONST void *in_b);


/*****************************************************************************
* Local variables
******************************************************************************/

/* Current axis to sort along. */

static int Axis = 0;

/* Number of finite elements. */

static long maxfinitecount = 0;

/* Priority queue used for frame level bouning box hierarchy. */

static PRIORITY_QUEUE *Frame_Queue;

/* Top node of bounding hierarchy. */

BBOX_TREE *Root_Object;



/*****************************************************************************
*
* FUNCTION
*
*   Initialize_BBox_Code
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
*   Initialize bbox specific variables.
*
* CHANGES
*
*   Jul 1995 : Creation.
*
******************************************************************************/

void Initialize_BBox_Code()
{
  Frame_Queue = Create_Priority_Queue(INITIAL_PRIORITY_QUEUE_SIZE);
}



/*****************************************************************************
*
* FUNCTION
*
*   Deinitialize_BBox_Code
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
*   Deinitialize bbox specific variables.
*
* CHANGES
*
*   Jul 1995 : Creation.
*
******************************************************************************/

void Deinitialize_BBox_Code()
{
  if (Frame_Queue != NULL)
  {
    Destroy_Priority_Queue(Frame_Queue);
  }

  Frame_Queue = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Priority_Queue
*
* INPUT
*
*   QSize - initial size of priority queue
*
* OUTPUT
*
* RETURNS
*
*   PRIORITY_QUEUE * - priority queue
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a priority queue.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

PRIORITY_QUEUE *Create_Priority_Queue(unsigned QSize)
{
  PRIORITY_QUEUE *New;

  New = (PRIORITY_QUEUE *)POV_MALLOC(sizeof(PRIORITY_QUEUE), "priority queue");

  New->Queue = (QELEM *)POV_MALLOC(QSize*sizeof(QELEM), "priority queue");

  New->QSize = 0;

  New->Max_QSize = QSize;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Priority_Queue
*
* INPUT
*
*   Queue - Priority queue
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
*   Destroy a priority queue.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void Destroy_Priority_Queue(PRIORITY_QUEUE *Queue)
{
  if (Queue != NULL)
  {
    POV_FREE(Queue->Queue);

    POV_FREE(Queue);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_BBox_Tree
*
* INPUT
*
*   Node - Node to destroy
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
*   Recursively destroy a bounding box tree.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Destroy_BBox_Tree(BBOX_TREE *Node)
{
  short i;

  if (Node != NULL)
  {
    if (Node->Entries > 0)
    {
      for (i = 0; i < Node->Entries; i++)
      {
        Destroy_BBox_Tree(Node->Node[i]);
      }

      POV_FREE(Node->Node);

      Node->Entries = 0;

      Node->Node = NULL;
    }

    POV_FREE(Node);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Recompute_BBox
*
* INPUT
*
*   trans - Transformation
*
* OUTPUT
*
*   bbox  - Bounding box
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Recalculate a bounding box after a transformation.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Recompute_BBox(BBOX *bbox, TRANSFORM *trans)
{
  int i;
  VECTOR lower_left, lengths, corner;
  VECTOR mins, maxs;

  if (trans == NULL)
  {
    return;
  }

  Assign_BBox_Vect(lower_left, bbox->Lower_Left);
  Assign_BBox_Vect(lengths, bbox->Lengths);

  Make_Vector(mins, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(maxs, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = 1; i <= 8; i++)
  {
    Assign_Vector(corner, lower_left);

    corner[X] += ((i & 1) ? lengths[X] : 0.0);
    corner[Y] += ((i & 2) ? lengths[Y] : 0.0);
    corner[Z] += ((i & 4) ? lengths[Z] : 0.0);

    MTransPoint(corner, corner, trans);

    if (corner[X] < mins[X]) { mins[X] = corner[X]; }
    if (corner[X] > maxs[X]) { maxs[X] = corner[X]; }
    if (corner[Y] < mins[Y]) { mins[Y] = corner[Y]; }
    if (corner[Y] > maxs[Y]) { maxs[Y] = corner[Y]; }
    if (corner[Z] < mins[Z]) { mins[Z] = corner[Z]; }
    if (corner[Z] > maxs[Z]) { maxs[Z] = corner[Z]; }
  }

  /* Clip bounding box at the largest allowed bounding box. */

  if (mins[X] < -BOUND_HUGE / 2) { mins[X] = -BOUND_HUGE / 2; }
  if (mins[Y] < -BOUND_HUGE / 2) { mins[Y] = -BOUND_HUGE / 2; }
  if (mins[Z] < -BOUND_HUGE / 2) { mins[Z] = -BOUND_HUGE / 2; }
  if (maxs[X] >  BOUND_HUGE / 2) { maxs[X] =  BOUND_HUGE / 2; }
  if (maxs[Y] >  BOUND_HUGE / 2) { maxs[Y] =  BOUND_HUGE / 2; }
  if (maxs[Z] >  BOUND_HUGE / 2) { maxs[Z] =  BOUND_HUGE / 2; }

  Make_BBox_from_min_max(*bbox, mins, maxs);
}



/*****************************************************************************
*
* FUNCTION
*
*   Recompute_Inverse_BBox
*
* INPUT
*
*   trans - Transformation
*
* OUTPUT
*
*   bbox  - Bounding box
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Recalculate a bounding box after a transformation.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Recompute_Inverse_BBox(BBOX *bbox, TRANSFORM *trans)
{
  int i;
  VECTOR lower_left, lengths, corner;
  VECTOR mins, maxs;

  if (trans == NULL)
  {
    return;
  }

  Assign_BBox_Vect(lower_left, bbox->Lower_Left);
  Assign_BBox_Vect(lengths, bbox->Lengths);

  Make_Vector(mins, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(maxs, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = 1; i <= 8; i++)
  {
    Assign_Vector(corner, lower_left);

    corner[X] += ((i & 1) ? lengths[X] : 0.0);
    corner[Y] += ((i & 2) ? lengths[Y] : 0.0);
    corner[Z] += ((i & 4) ? lengths[Z] : 0.0);

    MInvTransPoint(corner, corner, trans);

    if (corner[X] < mins[X]) { mins[X] = corner[X]; }
    if (corner[X] > maxs[X]) { maxs[X] = corner[X]; }
    if (corner[Y] < mins[Y]) { mins[Y] = corner[Y]; }
    if (corner[Y] > maxs[Y]) { maxs[Y] = corner[Y]; }
    if (corner[Z] < mins[Z]) { mins[Z] = corner[Z]; }
    if (corner[Z] > maxs[Z]) { maxs[Z] = corner[Z]; }
  }

  /* Clip bounding box at the largest allowed bounding box. */

  if (mins[X] < -BOUND_HUGE / 2) { mins[X] = -BOUND_HUGE / 2; }
  if (mins[Y] < -BOUND_HUGE / 2) { mins[Y] = -BOUND_HUGE / 2; }
  if (mins[Z] < -BOUND_HUGE / 2) { mins[Z] = -BOUND_HUGE / 2; }
  if (maxs[X] >  BOUND_HUGE / 2) { maxs[X] =  BOUND_HUGE / 2; }
  if (maxs[Y] >  BOUND_HUGE / 2) { maxs[Y] =  BOUND_HUGE / 2; }
  if (maxs[Z] >  BOUND_HUGE / 2) { maxs[Z] =  BOUND_HUGE / 2; }

  Make_BBox_from_min_max(*bbox, mins, maxs);
}



/*****************************************************************************
*
* FUNCTION
*
*   Build_BBox_Tree
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
*   Create a bounding box hierarchy from a given list of finite and
*   infinite elements. Each element consists of
*
*     - an infinite flag
*     - a bounding box enclosing the element
*     - a pointer to the structure representing the element (e.g an object)
*
* CHANGES
*
*   Feb 1995 : Creation. (Extracted from Build_Bounding_Slabs)
*   Sep 1995 : Changed to allow use of memcpy if memmove isn't available. [AED]
*   Jul 1996 : Changed to use POV_MEMMOVE, which can be memmove or pov_memmove
*
******************************************************************************/

void Build_BBox_Tree(BBOX_TREE **Root, long nFinite, BBOX_TREE **Finite, long  nInfinite, BBOX_TREE  **Infinite)
{
  short i;
  long low, high;
  BBOX_TREE *cd, *root;

  /*
   * This is a resonable guess at the number of finites needed.
   * This array will be reallocated as needed if it isn't.
   */

  maxfinitecount = 2 * nFinite;

  /*
   * Now do a sort on the objects, with the end result being
   * a tree of objects sorted along the x, y, and z axes.
   */

  if (nFinite > 0)
  {
    low = 0;
    high = nFinite;

    while (sort_and_split(Root, Finite, &nFinite, low, high) == 0)
    {
      low = high;
      high = nFinite;
    }

    /* Move infinite objects in the first leaf of Root. */

    if (nInfinite > 0)
    {
      root = (BBOX_TREE *)(*Root);

      root->Node = (BBOX_TREE **)POV_REALLOC(root->Node, (root->Entries + 1) * sizeof(BBOX_TREE *), "composite");

      POV_MEMMOVE(&(root->Node[1]), &(root->Node[0]), root->Entries * sizeof(BBOX_TREE *));

      root->Entries++;

      cd = create_bbox_node(nInfinite);

      for (i = 0; i < nInfinite; i++)
      {
        cd->Node[i] = Infinite[i];
      }

      calc_bbox(&(cd->BBox), Infinite, 0, nInfinite);

      root->Node[0] = (BBOX_TREE *)cd;

      calc_bbox(&(root->BBox), root->Node, 0, root->Entries);

      /* Root and first node are infinite. */

      root->Infinite = TRUE;

      root->Node[0]->Infinite = TRUE;
    }
  }
  else
  {
    /*
     * There are no finite objects and no Root was created.
     * Create it now and put all infinite objects into it.
     */

    if (nInfinite > 0)
    {
      cd = create_bbox_node(nInfinite);

      for (i = 0; i < nInfinite; i++)
      {
        cd->Node[i] = Infinite[i];
      }

      calc_bbox(&(cd->BBox), Infinite, 0, nInfinite);

      *Root = (BBOX_TREE *)cd;

      (*Root)->Infinite = TRUE;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Build_Bounding_Slabs
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Create the bounding box hierarchy for all objects in the scene.
*
* CHANGES
*
*   Sep 1994 : Added allocation of priority queue. [DB]
*
*   Sep 1994 : Added code to put all infinite objects in the first node
*              of the root. Thus the hierarchy won't be messed up. [DB]
*
*   Sep 1994 : Fixed nIfinite=0 bug. [DB]
*
*   Feb 1995 : Moved code to actually build the hierarchy. [DB]
*
******************************************************************************/

void Build_Bounding_Slabs(BBOX_TREE **Root)
{
  long i, nFinite, nInfinite, iFinite, iInfinite;
  BBOX_TREE **Finite, **Infinite;
  OBJECT *Object, *Temp;

  /* Count frame level and infinite objects. */

  Object = Frame.Objects;

  nFinite = nInfinite = 0;

  while (Object != NULL)
  {
    if (Object->Type & LIGHT_SOURCE_OBJECT)
    {
      Temp = ((LIGHT_SOURCE *)Object)->Children;
    }
    else
    {
      Temp = Object;
    }

    if (Temp != NULL)
    {
      if (Test_Flag(Temp, INFINITE_FLAG))
      {
        nInfinite++;
      }
      else
      {
        nFinite++;
      }
    }

    Object = Object->Sibling;
  }

  /* We want to know how many objects there are. */

  Render_Info("\nScene contains %ld frame level objects; %ld infinite.\n",
      nFinite + nInfinite, nInfinite);

  /* If bounding boxes aren't used we can return. */

  if (!opts.Use_Slabs || !(nFinite + nInfinite >= opts.BBox_Threshold))
  {
    opts.Use_Slabs = FALSE; 

    return;
  }

  opts.Use_Slabs = TRUE;

  /*
   * This is a resonable guess at the number of finites needed.
   * This array will be reallocated as needed if it isn't.
   */

  maxfinitecount = 2 * nFinite;

  /*
   * Now allocate an array to hold references to these finites and
   * any new composite objects we may generate.
   */

  Finite = Infinite = NULL;

  if (nFinite > 0)
  {
    Finite = (BBOX_TREE **)POV_MALLOC(maxfinitecount*sizeof(BBOX_TREE *), "bounding boxes");
  }

  /* Create array to hold pointers to infinite objects. */

  if (nInfinite > 0)
  {
    Infinite = (BBOX_TREE **)POV_MALLOC(nInfinite*sizeof(BBOX_TREE *), "bounding boxes");
  }

  /* Init lists. */

  for (i = 0; i < nFinite; i++)
  {
    Finite[i] = create_bbox_node(0);
  }

  for (i = 0; i < nInfinite; i++)
  {
    Infinite[i] = create_bbox_node(0);
  }

  /* Set up finite and infinite object lists. */

  iFinite = iInfinite = 0;

  for (Object = Frame.Objects; Object != NULL; Object = Object->Sibling)
  {
    if (Object->Type & LIGHT_SOURCE_OBJECT)
    {
      Temp = ((LIGHT_SOURCE *)Object)->Children;
    }
    else
    {
      Temp = Object;
    }

    if (Temp != NULL)
    {
      /* Add object to the appropriate list. */

      if (Test_Flag(Temp, INFINITE_FLAG))
      {
        Infinite[iInfinite]->Infinite = TRUE;
        Infinite[iInfinite]->BBox     = Temp->BBox;
        Infinite[iInfinite]->Node     = (BBOX_TREE **)Temp;

        iInfinite++;
      }
      else
      {
        Finite[iFinite]->BBox = Temp->BBox;
        Finite[iFinite]->Node = (BBOX_TREE **)Temp;

        iFinite++;
      }
    }
  }

  /*
   * Now build the bounding box tree.
   */

  Build_BBox_Tree(Root, nFinite, Finite, nInfinite, Infinite);

  /* Get rid of the Finite and Infinite arrays and just use Root. */

  if (Finite != NULL)
  {
    POV_FREE(Finite);
  }

  if (Infinite != NULL)
  {
    POV_FREE(Infinite);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Bounding_Slabs
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Destroy the bounding box hierarchy and the priority queue.
*
* CHANGES
*
*   Sep 1994 : Added freeing of priority queue. [DB]
*
******************************************************************************/

void Destroy_Bounding_Slabs()
{
  if (Root_Object != NULL)
  {
    Destroy_BBox_Tree(Root_Object);

    Root_Object = NULL;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_BBox_Tree
*
* INPUT
*
*   Root - Root node of the bbox tree
*   Ray  - Current ray
*
* OUTPUT
*
*   Best_Intersection - Nearest intersection found
*   Best_Object       - Nearest object found
*
* RETURNS
*
*   int - TRUE if an intersection was found
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Intersect a ray with a bounding box tree.
*
* CHANGES
*
*   Sep 1994 : Moved priority queue allocation/freeing out of here. [DB]
*
******************************************************************************/

int Intersect_BBox_Tree(BBOX_TREE *Root, RAY *Ray, INTERSECTION *Best_Intersection, OBJECT **Best_Object)
{
  int i, found;
  DBL Depth;
  BBOX_TREE *Node;
  RAYINFO rayinfo;
  INTERSECTION New_Intersection;

  /* Create the direction vectors for this ray. */

  Create_Rayinfo(Ray, &rayinfo);

  /* Start with an empty priority queue. */

  found = FALSE;

  Frame_Queue->QSize = 0;

#ifdef BBOX_EXTRA_STATS
  Increase_Counter(stats[totalQueueResets]);
#endif

  /* Check top node. */

  Check_And_Enqueue(Frame_Queue, Root, &Root->BBox, &rayinfo);

  /* Check elements in the priority queue. */

  while (Frame_Queue->QSize)
  {
    Priority_Queue_Delete(Frame_Queue, &Depth, &Node);

    /*
     * If current intersection is larger than the best intersection found
     * so far our task is finished, because all other bounding boxes in
     * the priority queue are further away.
     */

    if (Depth > Best_Intersection->Depth)
    {
      break;
    }

    /* Check current node. */

    if (Node->Entries)
    {
      /* This is a node containing leaves to be checked. */

      for (i = 0; i < Node->Entries; i++)
      {
        Check_And_Enqueue(Frame_Queue, Node->Node[i], &Node->Node[i]->BBox, &rayinfo);
      }
    }
    else
    {
      /* This is a leaf so test contained object. */

      if (Intersection(&New_Intersection, (OBJECT *)Node->Node, Ray))
      {
        if (New_Intersection.Depth < Best_Intersection->Depth)
        {
          *Best_Intersection = New_Intersection;

          *Best_Object = (OBJECT *)Node->Node;

          found = TRUE;
        }
      }
    }
  }

  return (found);
}



/*****************************************************************************
*
* FUNCTION
*
*   priority_queue_insert
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Insert an element in the priority queue.
*
* CHANGES
*
*   Sep 1994 : Added code for resizing the priority queue. [DB]
*
******************************************************************************/

static void priority_queue_insert(PRIORITY_QUEUE *Queue, DBL Depth, BBOX_TREE *Node)
{
  unsigned size;
  int i;
  QELEM tmp, *List;

#ifdef BBOX_EXTRA_STATS
  Increase_Counter(stats[totalQueues]);
#endif

  Queue->QSize++;

  size = Queue->QSize;

  /* Reallocate priority queue if it's too small. */

  if (size >= Queue->Max_QSize)
  {
    if (size >= INT_MAX/2)
    {
      Error("Priority queue overflow.\n");
    }

#ifdef BBOX_EXTRA_STATS
    Increase_Counter(stats[totalQueueResizes]);
#endif

    Queue->Max_QSize *= 2;

    Queue->Queue = (QELEM *)POV_REALLOC(Queue->Queue, Queue->Max_QSize*sizeof(QELEM), "priority queue");
  }

  List = Queue->Queue;
  
  List[size].Depth = Depth;
  List[size].Node  = Node;
  
  i = size;
  
  while (i > 1 && List[i].Depth < List[i / 2].Depth)
  {
    tmp = List[i];

    List[i] = List[i / 2];

    List[i / 2] = tmp;

    i = i / 2;
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Priority_Queue_Delete
*
* INPUT
*
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Get an element from the priority queue.
*
*   NOTE: This element will always be the one closest to the ray origin.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Priority_Queue_Delete(PRIORITY_QUEUE *Queue, DBL *Depth, BBOX_TREE **Node)
{
  QELEM tmp, *List;
  int i, j;
  unsigned size;

  if (Queue->QSize == 0)
  {
    Error("priority queue is empty.\n");
  }

  List = Queue->Queue;

  *Depth = List[1].Depth;
  *Node  = List[1].Node;

  List[1] = List[Queue->QSize];

  Queue->QSize--;

  size = Queue->QSize;

  i = 1;

  while (2 * i <= (int)size)
  {
    if (2 * i == (int)size)
    {
      j = 2 * i;
    }
    else
    {
      if (List[2*i].Depth < List[2*i+1].Depth)
      {
        j = 2 * i;
      }
      else
      {
        j = 2 * i + 1;
      }
    }

    if (List[i].Depth > List[j].Depth)
    {
      tmp = List[i];

      List[i] = List[j];

      List[j] = tmp;

      i = j;
    }
    else
    {
      break;
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Check_And_Enqueue
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   If a given ray intersect the object's bounding box then add it
*   to the priority queue.
*
* CHANGES
*
*   Sep 1994 : Pass bounding box seperately.
*              This is needed for the vista/light buffer. [DB]
*
*   Sep 1994 : Added code to insert infinte objects without testing. [DB]
*
******************************************************************************/

void Check_And_Enqueue(PRIORITY_QUEUE *Queue, BBOX_TREE *Node, BBOX *BBox, RAYINFO *rayinfo)
{
  DBL tmin, tmax;
  DBL dmin, dmax;

  if (Node->Infinite)
  {
    /* Set intersection depth to -Max_Distance. */

    dmin = -Max_Distance;
  }
  else
  {
    Increase_Counter(stats[nChecked]);

    if (rayinfo->nonzero[X])
    {
      if (rayinfo->positive[X])
      {
        dmin = (BBox->Lower_Left[X] - rayinfo->slab_num[X]) *  rayinfo->slab_den[X];

        dmax = dmin + (BBox->Lengths[X]  * rayinfo->slab_den[X]);

        if (dmax < EPSILON) return;
      }
      else
      {
        dmax = (BBox->Lower_Left[X] - rayinfo->slab_num[X]) * rayinfo->slab_den[X];

        if (dmax < EPSILON) return;

        dmin = dmax + (BBox->Lengths[X]  * rayinfo->slab_den[X]);
      }

      if (dmin > dmax) return;
    }
    else
    {
      if ((rayinfo->slab_num[X] < BBox->Lower_Left[X]) ||
          (rayinfo->slab_num[X] > BBox->Lengths[X] + BBox->Lower_Left[X]))
      {
        return;
      }

      dmin = -BOUND_HUGE;
      dmax = BOUND_HUGE;
    }

    if (rayinfo->nonzero[Y])
    {
      if (rayinfo->positive[Y])
      {
        tmin = (BBox->Lower_Left[Y] - rayinfo->slab_num[Y]) * rayinfo->slab_den[Y];

        tmax = tmin + (BBox->Lengths[Y]  * rayinfo->slab_den[Y]);
      }
      else
      {
        tmax = (BBox->Lower_Left[Y] - rayinfo->slab_num[Y]) * rayinfo->slab_den[Y];

        tmin = tmax + (BBox->Lengths[Y]  * rayinfo->slab_den[Y]);
      }

      /*
       * Unwrap the logic - do the dmin and dmax checks only when tmin and
       * tmax actually affect anything, also try to escape ASAP. Better
       * yet, fold the logic below into the two branches above so as to
       *  compute only what is needed.
       */

      /*
       * You might even try tmax < EPSILON first (instead of second) for an
       * early quick out.
       */

      if (tmax < dmax)
      {
        if (tmax < EPSILON) return;

        /* check bbox only if tmax changes dmax */

        if (tmin > dmin)
        {
          if (tmin > tmax) return;

          /* do this last in case it's not needed! */

          dmin = tmin;
        }
        else
        {
          if (dmin > tmax) return;
        }

        /* do this last in case it's not needed! */

        dmax = tmax;
      }
      else
      {
        if (tmin > dmin)
        {
          if (tmin > dmax) return;
          
          /* do this last in case it's not needed! */
          
          dmin = tmin;
        }

        /* else nothing needs to happen, since dmin and dmax did not change! */
      }
    }
    else
    {
      if ((rayinfo->slab_num[Y] < BBox->Lower_Left[Y]) ||
          (rayinfo->slab_num[Y] > BBox->Lengths[Y] + BBox->Lower_Left[Y]))
      {
        return;
      }
    }
    
    if (rayinfo->nonzero[Z])
    {
      if (rayinfo->positive[Z])
      {
        tmin = (BBox->Lower_Left[Z] - rayinfo->slab_num[Z]) * rayinfo->slab_den[Z];
        
        tmax = tmin + (BBox->Lengths[Z]  * rayinfo->slab_den[Z]);
      }
      else
      {
        tmax = (BBox->Lower_Left[Z] - rayinfo->slab_num[Z]) * rayinfo->slab_den[Z];

        tmin = tmax + (BBox->Lengths[Z]  * rayinfo->slab_den[Z]);
      }

      if (tmax < dmax)
      {
        if (tmax < EPSILON) return;

        /* check bbox only if tmax changes dmax */

        if (tmin > dmin)
        {
          if (tmin > tmax) return;

          /* do this last in case it's not needed! */

          dmin = tmin;
        }
        else
        {
          if (dmin > tmax) return;
        }
      }
      else
      {
        if (tmin > dmin)
        {
          if (tmin > dmax) return;

          /* do this last in case it's not needed! */

          dmin = tmin;
        }

        /* else nothing needs to happen, since dmin and dmax did not change! */
      }
    }
    else
    {
      if ((rayinfo->slab_num[Z] < BBox->Lower_Left[Z]) ||
          (rayinfo->slab_num[Z] > BBox->Lengths[Z] + BBox->Lower_Left[Z]))
      {
        return;
      }
    }

    Increase_Counter(stats[nEnqueued]);
  }

  priority_queue_insert(Queue, dmin, Node);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Rayinfo
*
* INPUT
*
*   Ray     - Current ray
*   rayinfo - Rayinfo structure
*   
* OUTPUT
*
*   rayinfo
*   
* RETURNS
*   
* AUTHOR
*
*   Dieter Bayer
*   
* DESCRIPTION
*
*   Calculate the rayinfo structure for a given ray. It's need for
*   intersection the ray with bounding boxes.
*
* CHANGES
*
*   May 1994 : Creation. (Extracted from Intersect_BBox_Tree)
*
******************************************************************************/

void Create_Rayinfo(RAY *Ray, RAYINFO *rayinfo)
{
  DBL t;

  /* Create the direction vectors for this ray */

  Assign_Vector(rayinfo->slab_num, Ray->Initial);

  if ((rayinfo->nonzero[X] = ((t = Ray->Direction[X]) != 0.0)) != 0)
  {
    rayinfo->slab_den[X] = 1.0 / t;

    rayinfo->positive[X] = (Ray->Direction[X] > 0.0);
  }

  if ((rayinfo->nonzero[Y] = ((t = Ray->Direction[Y]) != 0.0)) != 0)
  {
    rayinfo->slab_den[Y] = 1.0 / t;

    rayinfo->positive[Y] = (Ray->Direction[Y] > 0.0);
  }

  if ((rayinfo->nonzero[Z] = ((t = Ray->Direction[Z]) != 0.0)) != 0)
  {
    rayinfo->slab_den[Z] = 1.0 / t;

    rayinfo->positive[Z] = (Ray->Direction[Z] > 0.0);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   create_bbox_node
*
* INPUT
*
*   size - Number of subnodes
*
* OUTPUT
*
* RETURNS
*
*   BBOX_TREE * - New node
*
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static BBOX_TREE *create_bbox_node(int size)
{
  BBOX_TREE *New;

  New = (BBOX_TREE *)POV_MALLOC(sizeof(BBOX_TREE), "bounding box node");

  New->Infinite = FALSE;

  New->Entries = size;

  Make_BBox(New->BBox, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  if (size)
  {
    New->Node = (BBOX_TREE **)POV_MALLOC(size*sizeof(BBOX_TREE *), "bounding box node");
  }
  else
  {
    New->Node = NULL;
  }

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   compboxes
*
* INPUT
*
*   in_a, in_b - Elements to compare
*
* OUTPUT
*
* RETURNS
*
*   int - result of comparison
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Sep 1994 : Removed test for infinite objects because it's obsolete. [DB]
*
******************************************************************************/

static int CDECL compboxes(CONST void *in_a, CONST void *in_b)
{
  BBOX *a, *b;
  BBOX_VAL am, bm;

  a = &((*(BBOX_TREE **)in_a)->BBox);
  b = &((*(BBOX_TREE **)in_b)->BBox);

  am = 2.0 * a->Lower_Left[Axis] + a->Lengths[Axis];
  bm = 2.0 * b->Lower_Left[Axis] + b->Lengths[Axis];

  if (am < bm)
  {
    return (-1);
  }
  else
  {
    if (am == bm)
    {
      return (0);
    }
    else
    {
      return (1);
    }
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   find_axis
*
* INPUT
*
*   Finite      - Array of finite elements
*   first, last - Position of elements
*
* OUTPUT
*   
* RETURNS
*
*   int - Axis to sort along
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Find the axis along which the elements will be sorted.
*
* CHANGES
*
*   Sep 1994 : Initialize local variable which. [DB]
*
******************************************************************************/

static int find_axis(BBOX_TREE **Finite, long first, long  last)
{
  int which = X;
  long i;
  DBL e, d = -BOUND_HUGE;
  VECTOR mins, maxs;
  BBOX *bbox;

  Make_Vector(mins, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(maxs, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = first; i < last; i++)
  {
    bbox = &(Finite[i]->BBox);

    if (bbox->Lower_Left[X] < mins[X])
    {
      mins[X] = bbox->Lower_Left[X];
    }

    if (bbox->Lower_Left[X] + bbox->Lengths[X] > maxs[X])
    {
      maxs[X] = bbox->Lower_Left[X];
    }

    if (bbox->Lower_Left[Y] < mins[Y])
    {
      mins[Y] = bbox->Lower_Left[Y];
    }

    if (bbox->Lower_Left[Y] + bbox->Lengths[Y] > maxs[Y])
    {
      maxs[Y] = bbox->Lower_Left[Y];
    }

    if (bbox->Lower_Left[Z] < mins[Z])
    {
      mins[Z] = bbox->Lower_Left[Z];
    }

    if (bbox->Lower_Left[Z] + bbox->Lengths[Z] > maxs[Z])
    {
      maxs[Z] = bbox->Lower_Left[Z];
    }
  }

  e = maxs[X] - mins[X];

  if (e > d)
  {
    d = e;  which = X;
  }

  e = maxs[Y] - mins[Y];

  if (e > d)
  {
    d = e;  which = Y;
  }

  e = maxs[Z] - mins[Z];

  if (e > d)
  {
    which = Z;
  }

  return (which);
}



/*****************************************************************************
*
* FUNCTION
*
*   calc_bbox
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Calculate the bounding box containing Finite[first] through Finite[last-1].
*
* CHANGES
*
*   -
*
******************************************************************************/

static void calc_bbox(BBOX *BBox, BBOX_TREE **Finite, long first, long  last)
{
  long i;
  DBL tmin, tmax;
  VECTOR bmin, bmax;
  BBOX *bbox;

  COOPERATE_1

  Make_Vector(bmin, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(bmax, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = first; i < last; i++)
  {
    bbox = &(Finite[i]->BBox);

    tmin = bbox->Lower_Left[X];
    tmax = tmin + bbox->Lengths[X];

    if (tmin < bmin[X]) { bmin[X] = tmin; }
    if (tmax > bmax[X]) { bmax[X] = tmax; }

    tmin = bbox->Lower_Left[Y];
    tmax = tmin + bbox->Lengths[Y];

    if (tmin < bmin[Y]) { bmin[Y] = tmin; }
    if (tmax > bmax[Y]) { bmax[Y] = tmax; }

    tmin = bbox->Lower_Left[Z];
    tmax = tmin + bbox->Lengths[Z];

    if (tmin < bmin[Z]) { bmin[Z] = tmin; }
    if (tmax > bmax[Z]) { bmax[Z] = tmax; }
  }

  Make_BBox_from_min_max(*BBox, bmin, bmax);
}



/*****************************************************************************
*
* FUNCTION
*
*   build_area_table
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Alexander Enzmann
*   
* DESCRIPTION
*
*   Generates a table of bound box surface areas.
*
* CHANGES
*
*   -
*
******************************************************************************/

static void build_area_table(BBOX_TREE **Finite, long a, long  b, DBL *areas)
{
  long i, imin, dir;
  DBL tmin, tmax;
  VECTOR bmin, bmax, len;
  BBOX *bbox;

  if (a < b)
  {
    imin = a;  dir =  1;
  }
  else
  {
    imin = b;  dir = -1;
  }

  Make_Vector(bmin, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);
  Make_Vector(bmax, -BOUND_HUGE, -BOUND_HUGE, -BOUND_HUGE);

  for (i = a; i != (b + dir); i += dir)
  {
    bbox = &(Finite[i]->BBox);

    tmin = bbox->Lower_Left[X];
    tmax = tmin + bbox->Lengths[X];

    if (tmin < bmin[X]) { bmin[X] = tmin; }
    if (tmax > bmax[X]) { bmax[X] = tmax; }

    tmin = bbox->Lower_Left[Y];
    tmax = tmin + bbox->Lengths[Y];

    if (tmin < bmin[Y]) { bmin[Y] = tmin; }
    if (tmax > bmax[Y]) { bmax[Y] = tmax; }

    tmin = bbox->Lower_Left[Z];
    tmax = tmin + bbox->Lengths[Z];

    if (tmin < bmin[Z]) { bmin[Z] = tmin; }
    if (tmax > bmax[Z]) { bmax[Z] = tmax; }

    VSub(len, bmax, bmin);

    areas[i - imin] = len[X] * (len[Y] + len[Z]) + len[Y] * len[Z];
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   sort_and_split
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

static int sort_and_split(BBOX_TREE **Root, BBOX_TREE **Finite, long *nFinite, long  first, long  last)
{
  BBOX_TREE *cd;
  long size, i, best_loc;
  DBL *area_left, *area_right;
  DBL best_index, new_index;

  Axis = find_axis(Finite, first, last);

  size = last - first;

  if (size <= 0)
  {
    return (1);
  }

  /*
   * Actually, we could do this faster in several ways. We could use a
   * logn algorithm to find the median along the given axis, and then a
   * linear algorithm to partition along the axis. Oh well.
   */

  QSORT((void *)(&Finite[first]), (unsigned long)size, sizeof(BBOX_TREE *), compboxes);

  /*
   * area_left[] and area_right[] hold the surface areas of the bounding
   * boxes to the left and right of any given point. E.g. area_left[i] holds
   * the surface area of the bounding box containing Finite 0 through i and
   * area_right[i] holds the surface area of the box containing Finite
   * i through size-1.
   */

  area_left = (DBL *)POV_MALLOC(size * sizeof(DBL), "bounding boxes");
  area_right = (DBL *)POV_MALLOC(size * sizeof(DBL), "bounding boxes");

  /* Precalculate the areas for speed. */

  build_area_table(Finite, first, last - 1, area_left);
  build_area_table(Finite, last - 1, first, area_right);

  best_index = area_right[0] * (size - 3.0);

  best_loc = -1;

  /*
   * Find the most effective point to split. The best location will be
   * the one that minimizes the function N1*A1 + N2*A2 where N1 and N2
   * are the number of objects in the two groups and A1 and A2 are the
   * surface areas of the bounding boxes of the two groups.
   */

  for (i = 0; i < size - 1; i++)
  {
    new_index = (i + 1) * area_left[i] + (size - 1 - i) * area_right[i + 1];

    if (new_index < best_index)
    {
      best_index = new_index;
      best_loc = i + first;
    }
  }

  POV_FREE(area_left);
  POV_FREE(area_right);

  /*
   * Stop splitting if the BUNCHING_FACTOR is reached or
   * if splitting stops being effective.
   */

  if ((size <= BUNCHING_FACTOR) || (best_loc < 0))
  {
    cd = create_bbox_node(size);
      
    for (i = 0; i < size; i++)
    {
      cd->Node[i] = Finite[first+i];
    }

    calc_bbox(&(cd->BBox), Finite, first, last);

    *Root = (BBOX_TREE *)cd;

    if (*nFinite > maxfinitecount)
    {
      /* Prim array overrun, increase array by 50%. */

      maxfinitecount = 1.5 * maxfinitecount;

      /* For debugging only. */

      Debug_Info("Reallocing Finite to %d\n", maxfinitecount);

      Finite = (BBOX_TREE **)POV_REALLOC(Finite, maxfinitecount * sizeof(BBOX_TREE *), "bounding boxes");
    }

    Finite[*nFinite] = cd;

    (*nFinite)++;

    return (1);
  }
  else
  {
    sort_and_split(Root, Finite, nFinite, first, best_loc + 1);

    sort_and_split(Root, Finite, nFinite, best_loc + 1, last);

    return (0);
  }
}

