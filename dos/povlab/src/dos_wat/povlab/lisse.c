/* ---------------------------------------------------------------------------
*  LISSE.C
*
*  Some parts from 3DS2POV utility.
*
*  from POVLAB 3D Modeller
*  Copyright 1994-1999 POVLAB Authors.
*  ---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POVLAB and to port the software to platforms other
*  than those supported by the POVLAB authors. There are strict rules under
*  which you are permitted to use this file. The rules are in the file
*  named LEGAL.TXT which should be distributed with this file.
*  If LEGAL.TXT is not available or for more info please contact the POVLAB
*  primary author by leaving a message on http://www.povlab.org
*  The latest and official version of POVLAB may be found at this site.
*
*  POVLAB was originally written by Denis Olivier.
*
*  ---------------------------------------------------------------------------*/

#include <MATH.H>
#include <FLOAT.H>
#include <STRING.H>
#include <MALLOC.H>
#include <CTYPE.H>
#include "GLIB.H"
#include "GLOBAL.H"

static _Palette   *ptable;      /* _Palette table */
static unsigned  pmax;         /* Maximum size of table */
static unsigned  psize;        /* Current size */

static Texture   *ttable;      /* Named texture table */
static unsigned  tmax;         /* Maximum size of table */
static unsigned  tsize;        /* Current size */

static Vecteur    *vtable;      /* Vertice table */
static unsigned  vmax;         /* Maximum size of table */
static unsigned  vsize;        /* Current size */

static Vecteur    gmin = {+MAXFLOAT,+MAXFLOAT,+MAXFLOAT};
static Vecteur    gmax = {-MAXFLOAT,-MAXFLOAT,-MAXFLOAT};

static MATRIX    trans_matrix;
static int       use_transform = 0;

static VertList  **vert_hash;    /* Hash table for looking up vertices */
static TriList   **tri_index;    /* Index for smooth triangle generation */

static GroupTree *groot;         /* Tree representing the object hierarchy */

static int       initialized  = 0;
//static int       quiet_mode   = 0;
static int       bound_mode   = 0;
static DBL       smooth_angle = 70.0;
static unsigned  vert_init    = 0;
static int       dec_point    = 9;
//static int       out_format   = 0;

static unsigned  tot_bounds   = 0;
static unsigned  object_cnt   = 0;

static Vecteur    last_vmin = {0.0,0.0,0.0};
static Vecteur    last_vmax = {0.0,0.0,0.0};
static unsigned  last_vert_cnt = 0;
static unsigned  last_tri_cnt = 0;
static DBL       last_index = 0.0;
static unsigned  last_bounds = 0;

static _Palette   last_pal;
static char      last_texture[64] = "";
static unsigned  texture_index;
static char      texture_type;
// static char      object_name[64] = "";

static DBL     orig_tpr;    /* Number of Tests Per Ray before optimization */
static DBL     final_tpr;   /*    "   "    "    "   "  after optimization */
static DBL     bound_cost;  /* Cost of testing a bound relative to testing */

// ---------------------------------------------------------------------------
// ------------------ ADD A NEW TRIANGLE TO THE DATABASE ---------------------
// ---------------------------------------------------------------------------
int opt_add_tri (DBL ax,DBL ay,DBL az,DBL bx,DBL by,DBL bz,DBL cx,DBL cy,DBL cz) {
  TriList2 *new_node;
  _Triangle *new_tri;
  register int      i;

  // Check if the triangle is degenerate (zero area),if so return -1

  if (degen_tri (ax,ay,az,bx,by,bz,cx,cy,cz)) return -1;

  if (!initialized) init_object();

  // Allocate memory for the new triangle

  new_tri=malloc(sizeof(_Triangle));
  if (new_tri==NULL) {
    f_erreur("No memory for triangles.",1);
    return 1;
  }

  // Look up the vertex and texture indexes

  new_tri->vert[0]=vert_lookup (ax,ay,az);
  new_tri->vert[1]=vert_lookup (bx,by,bz);
  new_tri->vert[2]=vert_lookup (cx,cy,cz);

  new_tri->text_index=texture_index;
  new_tri->text_type =texture_type;

  new_tri->flag=0;

  for (i=0; i<3; i++) {
    // Create a new index node

    new_node=malloc (sizeof(TriList2));
    if (new_node==NULL) {
      f_erreur("No memory for triangles.");
      return 1;
    }

    // Point the index entry to the new triangle

    new_node->tri=new_tri;

    // Insert the new index node into the list

    new_node->next=groot->index[i];
    new_node->prev=groot->index[i]->prev;
    groot->index[i]->prev->next=new_node;
    groot->index[i]->prev=new_node;
  }

  ++(groot->obj_cnt);

  return 0;
}

void opt_write_pov (FILE *out_file,byte Niveau,int N) {
  opt_write_file(out_file,Niveau,N);
}

// ----------------------------------------------------------------------
// -------------------- OPTIMIZE AND WRITE FILE -------------------------
// ----------------------------------------------------------------------
void opt_write_file (FILE *out_file,byte Niveau,int N) {
  register unsigned int X1=CentX-120;
  register unsigned int X2=CentX+120;
  register unsigned int Y1=CentY-30;
  register unsigned int Y2=CentY+30;
  VertList *temp;
  register int i;

  if (!initialized || groot->obj_cnt==0) {
    orig_tpr=1.0;
    final_tpr=0.0;
    tot_bounds=0;
    return;   // No triangles where ever added,nothing to write
  }

  ++object_cnt;

  // Dump the hash table,don't need it any more

  f_jauge(1,AFFICHE,0,0,"ORDERING VERTICES");
  for (i=0;i<HASHSIZE;i++) {
    while (vert_hash[i]!=NULL) {
      temp=vert_hash[i];
      vert_hash[i]=vert_hash[i]->next;
      free (temp);
      f_jauge(1,MODIF,i,HASHSIZE,NULL);
    }
  }
  f_jauge(1,EFFACE,0,0,NULL);

  build_tri_index(); // Build the vertice index

  if (bound_mode!=2) //sort_indexes (groot);

  update_node (groot);

  orig_tpr=calc_tpr(groot); // Optimize the tree

  if (bound_mode!=2) {
    select_vue(5,CLIP_ON);
    g_fenetre(X1,Y1,X2,Y2,"BUILD GROUP",AFFICHE);
    gprintf(X1+20,Y1+30,15,FOND,1,18,"SUBDIVIDE OBJECT GROUPS:");
    optimize_tree(groot);
    g_fenetre(X1,Y1,X2,Y2,NULL,EFFACE);
  }

  final_tpr=calc_tpr(groot);
  write_file(out_file,Niveau,N); // Write the file

  dump_tri_index(); // Free up the vertex index

  cleanup_object();
}

void opt_get_limits (DBL *min_x,DBL *min_y,DBL *min_z,DBL *max_x,DBL *max_y,DBL *max_z) {
  *min_x=last_vmin[_X];
  *min_y=last_vmin[_Y];
  *min_z=last_vmin[_Z];

  *max_x=last_vmax[_X];
  *max_y=last_vmax[_Y];
  *max_z=last_vmax[_Z];
}


void opt_get_glimits (DBL *min_x,DBL *min_y,DBL *min_z,DBL *max_x,DBL *max_y,DBL *max_z) {
  *min_x=gmin[_X];
  *min_y=gmin[_Y];
  *min_z=gmin[_Z];

  *max_x=gmax[_X];
  *max_y=gmax[_Y];
  *max_z=gmax[_Z];
}


unsigned opt_get_vert_cnt() { return last_vert_cnt; }

unsigned opt_get_tri_cnt() { return last_tri_cnt; }

DBL opt_get_index() { return last_index; }

unsigned opt_get_bounds() { return last_bounds; }

void init_object(void) {
  register int i;

  last_pal.red  =0.0;
  last_pal.green=0.0;
  last_pal.blue =0.0;

  strcpy (last_texture,"");

  bound_cost=1.6;

  // Allocate memory for palette lookup table

  pmax  =10;
  psize =0;
  ptable=malloc (pmax*sizeof(_Palette));

  if (ptable==NULL) {
    f_erreur("No more memory for palette");
    return;
  }

  // Allocate memory for texture table

  tmax  =10;
  tsize =0;
  ttable=malloc (tmax*sizeof(Texture));

  if (ttable==NULL) {
    f_erreur("No more memory for textures");
    return;
  }

  // Allocate memory for vertex lookup table

  vmax=(vert_init>0) ? vert_init:1000;
  vsize =0;
  vtable=malloc (vmax*sizeof(Vecteur));

  if (vtable==NULL) {
    f_erreur("No more memory for vertices");
    return;
  }

  // Allocate memory for vertex hash table

  vert_hash=malloc (sizeof(VertList*)*HASHSIZE);

  if (vert_hash==NULL) {
    f_erreur("No more memory for vertices in hash table.");
    return;
  }

  // Initialize the vertex lookup hash table

  for (i=0; i<HASHSIZE; i++)
  vert_hash[i]=NULL;

  // Start with an empty root node

  groot=create_group();

  tot_bounds=1;
  initialized=1;
}

void cleanup_object(void) {
  register int i;
  Vecteur corners[8];  /* Corners of box */

  last_vert_cnt=vsize;
  last_tri_cnt =groot->obj_cnt;
  last_index   =orig_tpr/final_tpr;
  last_bounds  =tot_bounds;

  vect_copy (last_vmin,groot->vmin);
  vect_copy (last_vmax,groot->vmax);

  // Calculate the corners of the bounding box

  corners[0][_X]= groot->vmin[_X];
  corners[0][_Y]= groot->vmin[_Y];
  corners[0][_Z]= groot->vmin[_Z];

  corners[1][_X]= groot->vmin[_X];
  corners[1][_Y]= groot->vmin[_Y];
  corners[1][_Z]= groot->vmax[_Z];

  corners[2][_X]= groot->vmax[_X];
  corners[2][_Y]= groot->vmin[_Y];
  corners[2][_Z]= groot->vmin[_Z];

  corners[3][_X]= groot->vmax[_X];
  corners[3][_Y]= groot->vmin[_Y];
  corners[3][_Z]= groot->vmax[_Z];

  corners[4][_X]= groot->vmin[_X];
  corners[4][_Y]= groot->vmax[_Y];
  corners[4][_Z]= groot->vmin[_Z];

  corners[5][_X]= groot->vmax[_X];
  corners[5][_Y]= groot->vmax[_Y];
  corners[5][_Z]= groot->vmin[_Z];

  corners[6][_X]= groot->vmin[_X];
  corners[6][_Y]= groot->vmax[_Y];
  corners[6][_Z]= groot->vmax[_Z];

  corners[7][_X]= groot->vmax[_X];
  corners[7][_Y]= groot->vmax[_Y];
  corners[7][_Z]= groot->vmax[_Z];

  // Include any transformation in the box calcs

  if (use_transform) {
    for (i=0;i<8;i++) vect_transform (corners[i],corners[i],trans_matrix);
  }

  for (i=0; i<8; i++) {
    gmin[_X]=(corners[i][_X]<gmin[_X]) ? corners[i][_X]:gmin[_X];
    gmin[_Y]=(corners[i][_Y]<gmin[_Y]) ? corners[i][_Y]:gmin[_Y];
    gmin[_Z]=(corners[i][_Z]<gmin[_Z]) ? corners[i][_Z]:gmin[_Z];

    gmax[_X]=(corners[i][_X]>gmax[_X]) ? corners[i][_X]:gmax[_X];
    gmax[_Y]=(corners[i][_Y]>gmax[_Y]) ? corners[i][_Y]:gmax[_Y];
    gmax[_Z]=(corners[i][_Z]>gmax[_Z]) ? corners[i][_Z]:gmax[_Z];
  }

  free (ptable);
  free (vtable);
  free (vert_hash);

  f_jauge(1,AFFICHE,0,0,"DELETE HASH TABLE");
  for (i=0;i<tsize;i++) {
    free (ttable[i]);
    if (i%20==0) f_jauge(1,MODIF,i,tsize,NULL);
  }
  f_jauge(1,EFFACE,0,0,NULL);

  free (ttable);

  delete_tree (groot);

  initialized=0;
}

// -------------------------------------------------------------------------
// -- CALCULATE THE NUMBER OF TESTS PER RAY (TPR) REQUIRED FOR THIS GROUP --
// -------------------------------------------------------------------------
DBL calc_tpr (GroupTree *gnode) {
  GroupTree *g;
  register DBL    tpr;

  if (gnode->child_cnt==0)
  return gnode->obj_cnt;

  tpr=bound_cost*gnode->child_cnt;

  for (g=gnode->child; g!=NULL; g=g->next)
  tpr=tpr+(g->area/gnode->area)*calc_tpr(g);

  return tpr;
}

// -------------------------------------------------------------------------
//  --------------------- CREATE AN EMPTY GROUP NODE -----------------------
// -------------------------------------------------------------------------
GroupTree *create_group() {
  GroupTree *new_group;
  register int       i;

  new_group=malloc (sizeof(GroupTree));

  if (new_group==NULL) {
    f_erreur("No more memory for the group list");
    return NULL;
  }

  for (i=0; i<3; i++) {
  new_group->index[i]=malloc (sizeof(TriList2));

  if (new_group->index[i]==NULL) {
    f_erreur("No memory for tree");
    return NULL;
  }

  new_group->index[i]->tri=NULL;
  new_group->index[i]->prev=new_group->index[i];
  new_group->index[i]->next=new_group->index[i];
  }

  vect_init (new_group->vmin,+MAXFLOAT,+MAXFLOAT,+MAXFLOAT);
  vect_init (new_group->vmax,-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);
  new_group->area     =0.0;
  new_group->obj_cnt  =0;
  new_group->child_cnt=0;
  new_group->split_cnt=0;
  new_group->parent   =NULL;
  new_group->next     =NULL;
  new_group->child    =NULL;

  return new_group;
}

// -------------------------------------------------------------------------
// --------------- DELETE THIS NODE AND ALL SUB-NODES OF TREE --------------
// -------------------------------------------------------------------------
void delete_tree (GroupTree *gnode) {
    GroupTree *g,*g_temp;
    TriList2  *t,*t_temp;
    register int       i;

    for (g=gnode->child; g!=NULL; ) {
    g_temp=g->next;
    delete_tree (g);
    g=g_temp;
    }

    // Free the indexes for this node (if any exist)

    for (i=0; i<3; i++) {
    if ((gnode->index[i]!=NULL) && (gnode->index[i]->prev!=NULL)) {

        // Drop a link so the list isn't circular any more

        gnode->index[i]->prev->next=NULL;

        // Delete the list

        for (t=gnode->index[i]; t!=NULL; ) {
        if (i==0 && (t->tri!=NULL))
            free (t->tri);

        t_temp=t;
        t=t->next;
        free (t_temp);
        }
    }
    }

    // And finally free the root node

    free (gnode);
}

// -------------------------------------------------------------------------
// ---------- OPTIMIZE THE BOUNDS FOR THIS SUB-TREE ------------------------
// -------------------------------------------------------------------------
void optimize_tree (GroupTree *gnode) {
  GroupTree *group_a,*group_b;
  register int axis,best_axis;
  DBL best_rtpr,new_rtpr;
  TriList2  *best_loc,*new_loc;

  best_rtpr=0.0;
  best_loc =NULL;
  best_axis=-1;

  // Try splitting the group in each of the three axis' (x,y,z)

  for (axis=0; axis<3; axis++) {
    test_split(gnode,axis,&new_rtpr,&new_loc);
    if (new_rtpr<best_rtpr) {
      best_rtpr=new_rtpr;
      best_loc =new_loc;
      best_axis=axis;
    }
  }

  if (best_axis!=-1) {

    // Split this node into two nodes */

    split_group (gnode,best_axis,best_loc,&group_a,&group_b);
    optimize_tree (group_a);
    optimize_tree (group_b);
  }
}

// ---------------------------------------------------------------------------
// --- TEST THE EFFECTIVENESS OF SPLITTING THIS GROUP (BUT DON'T DO IT YET) --
// ---------------------------------------------------------------------------
void test_split (GroupTree *gnode,int axis,DBL *best_rtpr,TriList2 **best_loc) {
  register DBL   dim1,dim2;
  register DBL   area1,area2,p_area;
  register DBL   new_min1,new_max1,new_min2,new_max2;
  register DBL   best_index,new_index;
  TriList2 *t;
  register int      cnt,best_cnt;

  *best_loc =NULL;
  best_index=+MAXFLOAT ;
  best_cnt  =0;
  cnt=0;

  dim1=gnode->vmax[(axis+1) % 3]-gnode->vmin[(axis+1) % 3];
  dim2=gnode->vmax[(axis+2) % 3]-gnode->vmin[(axis+2) % 3];

  for (t=gnode->index[axis]->next; t!=gnode->index[axis]; t=t->next) {
    if (t->next==gnode->index[axis]) break;

    ++cnt;

    // Make an estimate of the new min/max limits,doing the full
    // calculation is just tooooo slooowww.

    new_min1=gnode->vmin[axis];
    new_max1=max_vertex (t->tri,axis);
    new_min2=min_vertex (t->next->tri,axis);
    new_max2=gnode->vmax[axis];

    // Calculate the surface area of the new groups

    area1=surf_area (dim1,dim2,new_max1-new_min1);
    area2=surf_area (dim1,dim2,new_max2-new_min2);

    new_index=(cnt*area1)+((gnode->obj_cnt-cnt)*area2);

    // Keep track of the best one

    if (new_index<best_index) {
      best_index=new_index;
      *best_loc =t->next;
      best_cnt  =cnt;
    }
  }

  // The former was just an estimate,verify the numbers

  if (*best_loc!=NULL) {
    new_min1=gnode->vmin[axis];
    new_max1=-MAXFLOAT;
    new_min2=+MAXFLOAT;
    new_max2=gnode->vmax[axis];

    for (t=gnode->index[axis]->next; t!=*best_loc; t=t->next)
      new_max1=fmax (new_max1,max_vertex (t->tri,axis));

    for (t=*best_loc; t!=gnode->index[axis]; t=t->next)
      new_min2=fmin (new_min2,min_vertex (t->tri,axis));

    area1=surf_area (dim1,dim2,new_max1-new_min1);
    area2=surf_area (dim1,dim2,new_max2-new_min2);

    best_index=(best_cnt*area1)+((gnode->obj_cnt-best_cnt)*area2);
  }

  if (gnode->parent==NULL || gnode->split_cnt>=2) {
    p_area=gnode->area;

    *best_rtpr=-1.0*((gnode->area/p_area)*gnode->obj_cnt) +
           (gnode->area/p_area)*((best_index/p_area) +
           2.0*bound_cost);
  } else {
    p_area=gnode->parent->area;
    *best_rtpr=-1.0*((gnode->area/p_area)*gnode->obj_cnt) +
           (best_index/p_area)+bound_cost;
  }
}

// ---------------------------------------------------------------------------
// ------ SPLIT THE GROUP ALONG THE SPECIFIED AXIS INTO TWO SUB-GROUPS -------
// ---------------------------------------------------------------------------
void split_group(GroupTree *gnode,int axis,TriList2 *split_loc,GroupTree **group_a,GroupTree **group_b) {
  GroupTree *new_a,*new_b;
  TriList2 *t,*next_t,*new_index;
  char new_flag;
  register int i;
  register unsigned int X1=CentX-120;
  register unsigned int Y1=CentY-30;

  /* Mark the triangles as to which group they will belong */
  new_flag=0;
  for (t=gnode->index[axis]->next; t!=gnode->index[axis]; t=t->next) {
    if (t==split_loc) new_flag=1;
    t->tri->flag=new_flag;
  }

  new_a=create_group();
  new_b=create_group();

  for (i=0; i<3; i++) {
    t=gnode->index[i]->next;

    while (t!=gnode->index[i]) {
      next_t=t->next;

      if (t->tri->flag==0)
        new_index=new_a->index[i];
      else
        new_index=new_b->index[i];

      /* Remove this node from the list */
      t->prev->next=t->next;
      t->next->prev=t->prev;

      /* Insert node into its new group */
      t->prev=new_index->prev;
      t->next=new_index;
      new_index->prev->next=t;
      new_index->prev=t;

      t=next_t;
    }
  }

  for (i=0; i<3; i++) {
    free (gnode->index[i]);
    gnode->index[i]=NULL;
  }

  if (gnode->parent==NULL || gnode->split_cnt>=2) {
    /* Add the new groups as children of original */
    gnode->child =new_a;
    new_a->parent=gnode;
    new_a->next  =new_b;
    new_b->parent=gnode;

    new_a->split_cnt=0;
    new_b->split_cnt=0;

    tot_bounds=tot_bounds+2;
  }  else {
    /* Remove the original group and replace with the new groups */
    for (i=0; i<3; i++) gnode->index[i]=new_a->index[i];

    free (new_a);
    new_a=gnode;

    new_b->next=new_a->next;
    new_a->next=new_b;

    new_a->parent=gnode->parent;
    new_b->parent=gnode->parent;

    new_a->split_cnt=gnode->split_cnt+1;
    new_b->split_cnt=gnode->split_cnt+1;

    tot_bounds=tot_bounds+1;
  }

  update_node (new_a);
  update_node (new_b);

  if (new_a->parent!=NULL) update_node (new_a->parent);

  gprintf(X1+199,Y1+30,15,FOND,1,4,"%d",tot_bounds);

  *group_a=new_a;
  *group_b=new_b;
}

// -------------------------------------------------------------------------
// ----------------- WRITE THE OPTIMIZED POV FILE --------------------------
// -------------------------------------------------------------------------
void write_file(FILE *f,byte Niveau,int N) {
  f_jauge(1,AFFICHE,0,0,"COMPUTING SMOOTH TRIANGLES");
  write_pov20_tree(f,groot,1,Niveau,N);
  f_jauge(1,EFFACE,0,0,NULL);
}

// -------------------------------------------------------------------------
// ---------------------- WRITE A SUB-TREE TO FILE -------------------------
// -------------------------------------------------------------------------
void write_pov20_tree(FILE *f,GroupTree *gnode,int level,byte Niveau,int N) {
  GroupTree *g;
  TriList2 *t;
  _Triangle *first_tri;
  register int one_texture;

  if (gnode->child!=NULL) {
    for (g=gnode->child;g!=NULL;g=g->next) write_pov20_tree(f,g,level+1,Niveau,N);
  } else {
    first_tri=gnode->index[0]->next->tri;
    one_texture=1;

    for (t=gnode->index[0]->next; t!=gnode->index[0]; t=t->next) {
      if (t->tri->text_index!=first_tri->text_index || t->tri->text_type !=first_tri->text_type) {
        one_texture=0;
        break;
      }
    }

    for (t=gnode->index[0]->next; t!=gnode->index[0]; t=t->next) {
      write_pov20_triangle(f,t->tri,one_texture,Niveau,N);
    }
  }
}

// -----------------------------------------------------------------------
//  WRITES A TRANSFORMATION MATRIX AS SEPARATE POV-RAY SCALE< >,
//  ROTATE< >,AND TRANSLATE<>COMMANDS
// -----------------------------------------------------------------------
void write_pov20_transform (FILE *f,MATRIX matrix) {
  Vecteur scale,shear,rotate,transl;

  // Decode the matrix into separate operations

  mat_decode (matrix,scale,shear,rotate,transl);

  if (fabs(scale[_X]-1.0)>0.001 || fabs(scale[_Y]-1.0)>0.001 || fabs(scale[_Z]-1.0)>0.001)
  fprintf (f,"scale <%.3f,%.3f,%.3f>\n",scale[_X],scale[_Y],scale[_Z]);

  if (fabs(rotate[_X])>0.01 || fabs(rotate[_Y])>0.01 || fabs(rotate[_Z])>0.01)
  fprintf (f,"rotate <%.2f,%.2f,%.2f>\n",rotate[_X],rotate[_Y],rotate[_Z]);

  if (fabs(transl[_X])>0.0001 || fabs(transl[_Y])>0.0001 || fabs(transl[_Z])>0.0001)
  fprintf (f,"translate <%.4g,%.4g,%.4g>\n",transl[_X],transl[_Y],transl[_Z]);

  // Can't handle shear but warn if it's there

  /*
  if (fabs(shear[_X])>0.01 || fabs(shear[_Y])>0.01 || fabs(shear[_Z])>0.01) {
    message("Warning: Significant shear in transformation (ignored)\n");
  }
  */
}

// --------------------------------------------------------------------------
// -------------------- WRITE A TRIANGLE (SMOOTH OR REGULAR) ----------------
// --------------------------------------------------------------------------
void write_pov20_triangle (FILE *f,_Triangle *tri,int one_texture,byte Niveau,int N) {
  Vecteur norm[3];
  int no_smooth=!Objet[N]->Smooth; // =(Niveau<3 ? 1:0);
  one_texture=one_texture;
  Niveau=Niveau;
  N=N;

  if (smooth_angle>0.0) {
    vert_normal(tri,norm);
    if (vect_equal(norm[0],norm[1]) && vect_equal (norm[1],norm[2])) no_smooth=1;
  }

  f_jauge(1,MODIF,(long) tri->vert[0],(long) groot->obj_cnt/2,NULL);

  if (smooth_angle>0.0 && !no_smooth) {
    outl(f,3,"smooth_triangle { <");
    vect_print (f,vtable[tri->vert[0]],dec_point,',');
    fprintf (f,">,<");
    vect_print (f,norm[0],3,',');
    fprintf (f,">,<");
    vect_print (f,vtable[tri->vert[1]],dec_point,',');
    fprintf (f,">,<");
    vect_print (f,norm[1],3,',');
    fprintf (f,">,<");
    vect_print (f,vtable[tri->vert[2]],dec_point,',');
    fprintf (f,">,<");
    vect_print (f,norm[2],3,',');
    fprintf (f,"> ");
  } else {
    outl(f,3,"triangle { <");
    vect_print (f,vtable[tri->vert[0]],dec_point,',');
    fprintf (f,">,<");
    vect_print (f,vtable[tri->vert[1]],dec_point,',');
    fprintf (f,">,<");
    vect_print (f,vtable[tri->vert[2]],dec_point,',');
    fprintf (f,"> ");
  }
  fprintf (f,"}\n");
}

// ------------------------------------------------------------------------
// ---- UPDATE THE STATS (AREA,VMIN/VMAX,CHILD_CNT,ETC.) FOR THIS NODE ----
// ------------------------------------------------------------------------
void update_node (GroupTree *gnode) {
  GroupTree *g;
  TriList2  *t;
  register int i;

  vect_init (gnode->vmin,+MAXFLOAT,+MAXFLOAT,+MAXFLOAT);
  vect_init (gnode->vmax,-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

  gnode->obj_cnt  =0;
  gnode->child_cnt=0;

  if (gnode->index[0]==NULL) {

    // Not a leaf node,calc the info from the child nodes

    for (g=gnode->child; g!=NULL; g=g->next) {
      ++(gnode->child_cnt);

      gnode->obj_cnt += g->obj_cnt;

      for (i=0; i<3; i++) {
        gnode->vmin[i]=fmin (gnode->vmin[i],g->vmin[i]);
        gnode->vmax[i]=fmax (gnode->vmax[i],g->vmax[i]);
      }
    }
  } else {

  // A leaf node,calc the info from the triangle list

    for (t=gnode->index[0]->next; t!=gnode->index[0]; t=t->next) {
      ++(gnode->obj_cnt);
      for (i=0; i<3; i++) {
        gnode->vmin[i]=fmin (gnode->vmin[i],min_vertex (t->tri,i));
        gnode->vmax[i]=fmax (gnode->vmax[i],max_vertex (t->tri,i));
      }
    }
  }

  // Update total surface area of region

  gnode->area=surf_area (gnode->vmax[_X]-gnode->vmin[_X],gnode->vmax[_Y]-gnode->vmin[_Y],gnode->vmax[_Z]-gnode->vmin[_Z]);
}

void quick_sort (TriList2 *start,TriList2 *end,int axis,int n) {
  TriList2 *a,*b;
  _Triangle *temp;
  DBL middle;
  static int Toto;

  if (start==end) return;

  Toto=n;
  f_jauge(1,MODIF,(long)Toto++,(long)vsize,NULL);

  a=start;
  b=end;
  middle=avg_vertex(a->tri,axis);

  do {
    while (avg_vertex(b->tri,axis)>=middle && a!=b) b=b->prev;

    if (a!=b) {
      temp  =a->tri;
      a->tri=b->tri;
      b->tri=temp;

      while (avg_vertex (a->tri,axis)<=middle && a!=b) a=a->next;

      if (a!=b) {
        temp  =a->tri;
        a->tri=b->tri;
        b->tri=temp;
      }
    }
  } while (a!=b);

  if (a!=start) quick_sort (start,a->prev,axis,Toto);
  if (b!=end) quick_sort (b->next,end,axis,Toto);
}


// ------------------------------------------------------------------------
// ------------ CALCULATE THE SURFACE AREA OF A BOX -----------------------
// ------------------------------------------------------------------------
DBL surf_area (DBL a,DBL b,DBL c) {
  return 2.0*(a*b+b*c+c*a);
}

DBL max_vertex (_Triangle *tri,int axis) {
  DBL max_v,val;
  register int i;

  max_v=-MAXFLOAT;

  for (i=0;i<3;i++) {
    val=vtable[tri->vert[i]][axis];
    if (val>max_v) max_v=val;
  }

  return max_v;
}


DBL min_vertex (_Triangle *tri,int axis) {
  DBL min_v,val;
  register int i;

  min_v=+MAXFLOAT;

  for (i=0; i<3; i++) {
    val=vtable[tri->vert[i]][axis];
    if (val<min_v) min_v=val;
  }

  return min_v;
}


DBL avg_vertex (_Triangle *tri,int axis) {
  DBL avg;

  avg=(vtable[tri->vert[0]][axis]+vtable[tri->vert[1]][axis]+vtable[tri->vert[2]][axis])/3.0;
  return avg;
}

// -----------------------------------------------------------------------
// ----- BUILD AN INDEX OF WHICH TRIANGLES TOUCH EACH VERTEX.  USED TO ---
// ----- SPEED UP SMOOTH TRIANGLE NORMAL CALCULATIONS. -------------------
// -----------------------------------------------------------------------
void build_tri_index() {
  GroupTree *g;
  TriList   *temp;
  TriList2  *t;
  unsigned  i,vert_no;

  if (vsize==0)
  return;

  tri_index=malloc (vsize*sizeof(TriList));
  if (tri_index==NULL) {
    f_erreur ("Insufficient memory for smooth triangles");
    return;
  }

  for (i=0;i<vsize;i++) tri_index[i]=NULL;

  for (g=groot;g!=NULL;g=g->next) {
    for (t=g->index[0]->next; t!=g->index[0]; t=t->next) {
      for (i=0;i<3;i++) {
        vert_no=t->tri->vert[i];
        temp=tri_index[vert_no];
        tri_index[vert_no]=malloc (sizeof(TriList));
        if (tri_index[vert_no]==NULL) {
          f_erreur ("Insufficient memory for smooth triangles.\n");
          return;
        }
        tri_index[vert_no]->tri=t->tri;
        tri_index[vert_no]->next=temp;
      }
    }
  }
}

void dump_tri_index() {
  TriList *temp;
  register int i;

  f_jauge(1,AFFICHE,0,0,"DELETING ARRAY");
  for (i=0;i<vsize;i++) {
    while (tri_index[i]!=NULL) {
      temp=tri_index[i];
      tri_index[i]=tri_index[i]->next;
      free (temp);
    }
    if (i%20==0) f_jauge(1,MODIF,i,vsize,NULL);
  }
  free (tri_index);
  f_jauge(1,EFFACE,0,0,NULL);
}

// --------------------------------------------------------------------------
// --- CALCULATES THE SMOOTH TRIANGLE NORMAL FOR THIS VERTEX ----------------
// --------------------------------------------------------------------------
void vert_normal (_Triangle *t,Vecteur *norm) {
  Vecteur  curr_norm,new_norm;
  TriList *p;
  register int     i;

  tri_normal (t,curr_norm);

  if (smooth_angle<=0.0) {
  for (i=0; i<3; i++)
      vect_copy (norm[i],curr_norm);

  return;
  }

  for (i=0; i<3; i++) {
  vect_init (norm[i],0.0,0.0,0.0);

  for (p=tri_index[t->vert[i]]; p!=NULL; p=p->next) {
      tri_normal (p->tri,new_norm);
      if (vect_angle (curr_norm,new_norm)<smooth_angle)
      vect_add (norm[i],norm[i],new_norm);
  }

  vect_normalize (norm[i]);
  }
}


/* Calculates the normal to the specified triangle */
void tri_normal (_Triangle *t,Vecteur normal) {
  Vecteur ab,ac;

  vect_sub (ab,vtable[t->vert[1]],vtable[t->vert[0]]);
  vect_sub (ac,vtable[t->vert[2]],vtable[t->vert[0]]);
  vect_cross (normal,ac,ab);

  vect_normalize (normal);
}

// --------------------------------------------------------------------
// --------- FIND THE SPECIFIED RGB VALUES IN THE PALETTE TABLE -------
// --------------------------------------------------------------------
unsigned pal_lookup (DBL red,DBL green,DBL blue) {
  register int i;

  /* The palette table is usually small so just do a simple linear search */

  for (i=psize-1; i>=0; i--) {
  if (ptable[i].red  ==red &&
      ptable[i].green==green &&
      ptable[i].blue ==blue)
    break;
  }

  if (i>=0)
  return i;    /* found,return the table index */

  /* not found,insert the new palette into the table */

  ++psize;
  if (psize>pmax) {

  /* table not big enough,resize it */

  pmax=pmax+10;
  ptable=realloc (ptable,pmax*sizeof(_Palette));

  if (ptable==NULL)
    f_erreur ("Insufficient memory to expand palette table");
    return NULL;
  }

  ptable[psize-1].red  =red;
  ptable[psize-1].green=green;
  ptable[psize-1].blue =blue;

  return (psize-1);
}

// ------------------------------------------------------------------------
// ------ FIND THE SPECIFIED NAMED TEXTURE IN THE TEXTURE TABLE -----------
// ------------------------------------------------------------------------
unsigned texture_lookup (char *texture_name) {
    register int i;

    /* The texture table is usually small so just do a simple linear search */
    for (i=tsize-1; i>=0; i--) {
    if (strcmp (ttable[i],texture_name)==0)
        break;
    }

    if (i>=0)
    return i;    /* found,return the table index */

    /* not found,insert the new texture into the table */
    ++tsize;
    if (tsize>tmax) {
      /* table not big enough,resize it */
      tmax=tmax+10;
      ttable=realloc (ttable,tmax*sizeof(Texture));
      if (ttable==NULL) {
        f_erreur("Insufficient memory to expand palette table");
        return NULL;
      }
    }

    ttable[tsize-1]=malloc (strlen(texture_name)+1);
    if (ttable[tsize-1]==NULL) {
      f_erreur ("Insufficient memory for texture name");
      return NULL;
    }

    strcpy (ttable[tsize-1],texture_name);

    return (tsize-1);
}

// ---------------------------------------------------------------------------
// -- FIND THE SPECIFIED VERTEX IN THE VERTEX TABLE --------------------------
// ---------------------------------------------------------------------------
unsigned vert_lookup (DBL x,DBL y,DBL z) {
    VertList *p,*new_node;
    unsigned hash;

    /* Vertex table is usually very large,use hash lookup */
    hash=(unsigned)((int)(326.4*x) ^ (int)(694.7*y) ^ (int)(1423.6*z)) % HASHSIZE;

    for (p=vert_hash[hash]; p!=NULL; p=p->next) {
    if (vtable[p->vert][0]==x && vtable[p->vert][1]==y &&
        vtable[p->vert][2]==z) break;
    }

    if (p!=NULL)
    return (p->vert);   /* found,return the table index */

    /* not found,insert the new vertex into the table */
    ++vsize;
    if (vsize>vmax) {
    /* table not big enough,expand it */
    vmax=vmax+100;
    vtable=realloc (vtable,vmax*sizeof(Vecteur));
      if (vtable==NULL) {
        f_erreur ("Insufficient memory for vertices\n");
        return NULL;
      }
    }

    vect_init (vtable[vsize-1],x,y,z);

    new_node=malloc (sizeof(VertList));

    if (new_node==NULL) {
      f_erreur ("Insufficient memory for hash table");
      return NULL;
    }

    new_node->vert =vsize-1;
    new_node->next =vert_hash[hash];
    vert_hash[hash]=new_node;

    return (vsize-1);
}


/* Checks if triangle is degenerate (zero area) */
int  degen_tri (DBL ax,DBL ay,DBL az,
        DBL bx,DBL by,DBL bz,
        DBL cx,DBL cy,DBL cz)
{
    Vecteur  ab,ac,norm;
    DBL  mag,fact;

    fact=pow (10.0,dec_point);

    /* Round the coords off to the output precision before checking */
    ax=floor((ax*fact)+0.5)/fact;
    ay=floor((ay*fact)+0.5)/fact;
    az=floor((az*fact)+0.5)/fact;
    bx=floor((bx*fact)+0.5)/fact;
    by=floor((by*fact)+0.5)/fact;
    bz=floor((bz*fact)+0.5)/fact;
    cx=floor((cx*fact)+0.5)/fact;
    cy=floor((cy*fact)+0.5)/fact;
    cz=floor((cz*fact)+0.5)/fact;

    vect_init (ab,ax-bx,ay-by,az-bz);
    vect_init (ac,ax-cx,ay-cy,az-cz);
    vect_cross (norm,ab,ac);

    mag=vect_mag(norm);

    return (mag<DEGEN_TOL);
}

DBL fmin (DBL a,DBL b) {
  if (a<b) return a; else return b;
}


DBL fmax (DBL a,DBL b) {
  if (a>b) return a; else return b;
}

void add_ext (char *fname,char *ext,int force) {
  register int i;

  for (i=0; i<strlen(fname); i++)
  if (fname[i]=='.') break;

  if (fname[i]=='\0' || force) {
    if (strlen(ext)>0) fname[i++]='.';
    strcpy(&fname[i],ext);
  }
}

void cleanup_name (char *name) {
  char *tmp=malloc(strlen(name)+1);
  register int i;

  // Remove any leading blanks or quotes

  i=0;
  while ((name[i]==' ' || name[i]=='"') && name[i]!='\0') i++;

  strcpy (tmp,&name[i]);

  // Remove any trailing blanks or quotes

  for (i=strlen(tmp)-1; i>=0; i--) {
    if (isprint(tmp[i]) && !isspace(tmp[i]) && tmp[i]!='"')
      break;
    else
      tmp[i]='\0';
  }

  strcpy (name,tmp);

  // Prefix the letter 'N' to materials that begin with a digit

  if (!isdigit (name[0]))
     strcpy (tmp,name);
  else {
     tmp[0]='N';
     strcpy (&tmp[1],name);
  }

  // Replace all illegal charaters in name with underscores

  for (i=0; tmp[i]!='\0'; i++) {
     if (!isalnum(tmp[i]))
     tmp[i]='_';
  }

  strcpy (name,tmp);

  free (tmp);
}

void sort_indexes (GroupTree *gnode) {
  register int i;

  f_jauge(1,AFFICHE,0,0,"ORDERING INDEX");
  for (i=0;i<3;i++) quick_sort(gnode->index[i]->next,gnode->index[i]->prev,i,0);
  f_jauge(1,EFFACE,0,0,NULL);
}


