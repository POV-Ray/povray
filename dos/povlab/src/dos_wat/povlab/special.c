/* ---------------------------------------------------------------------------
*  SPECIAL.C
*
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
#include <STDIO.H>
#include <IO.H>
#include <DOS.H>
#include <STDLIB.H>
#include <STRING.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

// -------------------------------------------------------------------------
// -- SUPPRESSION D'UN OBJET SPECIAL ---------------------------------------
// -------------------------------------------------------------------------
void free_mem_special(int N) {
  VERTEX *Ptr,*Tmp;

  if (Objet[N]->Special.Root!=(VERTEX *) NULL) {
    for (Ptr=Objet[N]->Special.Root;Ptr!=(VERTEX *) NULL;Ptr=Tmp) {
      Tmp=Ptr->Next;
      mem_free((VERTEX *) Ptr,sizeof(VERTEX));
    }

    Objet[N]->Special.Root=(VERTEX *) NULL;
    Objet[N]->Special.Pt=(VERTEX *) NULL;
    Objet[N]->Special.Nombre=0;
  }
}

// -------------------------------------------------------------------------
// -- MODIFICATION D'UN POINT DE LA CHAINE ---------------------------------
// -------------------------------------------------------------------------
void change_point_chaine(int N,int Num,VECTOR V) {
  VERTEX *Ptr;
  register int i=0;

  for (Ptr=Objet[N]->Special.Root;Ptr!=(VERTEX *) NULL;Ptr=Ptr->Next) {
    if (Num==i++) break;
  }

  Ptr->V[_X]=(DBL) V.x;
  Ptr->V[_Y]=(DBL) V.y;
  Ptr->V[_Z]=(DBL) V.z;
}

// -------------------------------------------------------------------------
// -- RETOURNE LE POINT N DANS LA LISTE CHAINEE ----------------------------
// -------------------------------------------------------------------------
VECTOR point_chaine(int N,int Num) {
  VERTEX *Ptr;
  register int i=0;
  VECTOR V;

  for (Ptr=Objet[N]->Special.Root;Ptr!=(VERTEX *) NULL;Ptr=Ptr->Next) {
    if (Num==i++) break;
  }

  V.x=(DBL) Ptr->V[_X];
  V.y=(DBL) Ptr->V[_Y];
  V.z=(DBL) Ptr->V[_Z];

  // log_out(Num,"%.4f %.4f %.4f",V.x,V.y,V.z);

  return V;
}

// -------------------------------------------------------------------------
// -- ALLOUE UN POINT POUR LA LISTE CHAINEE --------------------------------
// -------------------------------------------------------------------------
VERTEX *alloc_point_chaine(void) {
  VERTEX *Ptr;

  Ptr=(VERTEX *) mem_alloc(sizeof(VERTEX));
  Ptr->Next=(VERTEX *) NULL;

  return (VERTEX *) Ptr;
}

// -------------------------------------------------------------------------
// ---- RENCENTRAGE DES POINTS SUR <1,1,1> ECHELLE <0,0,0> POSITION -----
// -------------------------------------------------------------------------
void adapte_special(int N) {
  register DBL MaxX,MaxY,MaxZ;
  register DBL MinX,MinY,MinZ;
  register int Nb=Objet[N]->Special.Nombre,i;
  register DBL T1,T2,X,Y;
  VECTOR V;

  MaxX=MaxY=MaxZ=DBL_MIN_;
  MinX=MinY=MinZ=DBL_MAX_;

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    MaxX=(V.x>MaxX ? V.x:MaxX);
    MaxY=(V.y>MaxY ? V.y:MaxY);
    MinX=(V.x<MinX ? V.x:MinX);
    MinY=(V.y<MinY ? V.y:MinY);
  }

  T1=X=(DBL)(MaxX+MinX)/2;
  T2=Y=(DBL)(MaxY+MinY)/2;

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    V.x-=X;
    V.y-=Y;
    change_point_chaine(N,i,V);
  }

  X=(fabs(MaxX-X)+fabs(MinX-X))/2+(T1);
  Y=(fabs(MaxY-Y)+fabs(MinY-Y))/2;

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    V.x+=T1;
    V.x/=X;
    V.y/=Y;
    change_point_chaine(N,i,V);
  }

  Objet[N]->S[_X]=fabs(X);
  Objet[N]->S[_Y]=fabs(Y);
  Objet[N]->S[_Z]=fabs(X);

  Objet[N]->T[_X]=0;
  Objet[N]->T[_Y]=0;
  Objet[N]->T[_Z]=0;
}

// -------------------------------------------------------------------------
// -- LECTURE DES POINTS DE SPLINE DANS LE FICHIER .SCN --------------------
// -------------------------------------------------------------------------
byte lecture_special_scn(FILE *Fichier,char *MarqueObjet,int N) {
  register int NPT,i,Mark=0;
  char *Buffer=(char *) malloc(256);

  fgets(Buffer,256,Fichier);
  analyse_ligne(Buffer,32);
  if (strinstr(0,MarqueObjet,Argu[1])!=0) {
    free(Buffer);
    return 0;
  }

  if (!strcmp(MarqueObjet,"00000:")) Mark=1;

  Objet[N]->Special.Type=atoi(Argu[2]);

  fgets(Buffer,256,Fichier);
  analyse_ligne(Buffer,32);
  if (Objet[N]->Type==BEZIER) {
    NPT=Objet[N]->Special.Nombre=16;
    Objet[N]->P[_X]=atof(Argu[2]);
    Objet[N]->P[_Y]=atof(Argu[3]);
    Objet[N]->P[_Z]=atof(Argu[4]);
  } else {
    NPT=Objet[N]->Special.Nombre=atoi(Argu[2]);
  }

  message("Reading #%d vertices for object %s",NPT,Objet[N]->Nom);

  for (i=0;i<NPT;i++) {
    if (feof(Fichier)) break;
    fgets(Buffer,256,Fichier);
    analyse_ligne(Buffer,32);
    if (!strinstr(0,MarqueObjet,Argu[1])) {
      if (Objet[N]->Special.Root == (VERTEX *) NULL) {
        Objet[N]->Special.Root = alloc_point_chaine();
        Objet[N]->Special.Pt = Objet[N]->Special.Root;
      } else {
        Objet[N]->Special.Pt->Next = alloc_point_chaine();
        Objet[N]->Special.Pt = Objet[N]->Special.Pt->Next;
      }
      Objet[N]->Special.Pt->V[_X] = atof(Argu[2]);
      Objet[N]->Special.Pt->V[_Y] = atof(Argu[3]);
      Objet[N]->Special.Pt->V[_Z] = atof(Argu[4]);
    }
  }

  if (Mark) {
    if (Objet[N]->Type==BEZIER) {
      adapte_bezier(N);
    } else {
      adapte_special(N);
    }
  }

  free(Buffer);
  return 1;
}

// -------------------------------------------------------------------------
// -- SAUVE LES POINTS OBJET SPECIAL DANS .SCN ASCII -----------------------
// -------------------------------------------------------------------------
void sauve_special(FILE *Fichier,int N) {
  VERTEX *Ptr;

  fprintf(Fichier,"Object %05d: %d // Type\n",N,Objet[N]->Special.Type);
  if (Objet[N]->Type==BEZIER) {
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",N,
                                                    Objet[N]->P[_X],
                                                    Objet[N]->P[_Y],
                                                    Objet[N]->P[_Z]);
  } else {
    fprintf(Fichier,"Object %05d: %d // Nb\n",N,Objet[N]->Special.Nombre);
  }

  Ptr=Objet[N]->Special.Root;
  for (Ptr=Objet[N]->Special.Root;Ptr!=(VERTEX *) NULL;Ptr=Ptr->Next) {
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",N,Ptr->V[0],Ptr->V[1],Ptr->V[2]);
  }
}

// -------------------------------------------------------------------------
// -- ENREGISTRE LATHE AU FORMAT POV ---------------------------------------
// -------------------------------------------------------------------------
void enregistre_lathe_pov(FILE *Fichier,int N,int L) {
  register int i;
  VECTOR V;
  int Nb=Objet[N]->Special.Nombre;

  switch (Objet[N]->Special.Type) {
    case 0: outl(Fichier,1,"linear_spline\n"); break;
    case 1: outl(Fichier,1,"quadratic_spline\n"); break;
    case 2: outl(Fichier,1,"cubic_spline\n"); break;
  }

  outl(Fichier,1,"%d,\n",Nb);

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    outl(Fichier,1,"<%.4g,%.4g>%c\n",V.x,V.y,(i==Nb-1 ? ' ':','));
  }

  outl(Fichier,1,"sturm\n");
}

// -------------------------------------------------------------------------
// -- IMPORT .LTH OBJECT ---------------------------------------------------
// -------------------------------------------------------------------------
void import_lathe(void) {
  char *Spec[2]={"1","*.LTH"};
  char *Buffer=(char *) malloc(256);
  FILE *f;

  strcpy(Buffer,selection_fichier(100,100,".LTH FILES",Spec));
  if (Buffer[0]==27) { free(Buffer); return; }

  f=fopen(Buffer,"rt");
  fgets(Buffer,80,f);
  analyse_ligne(Buffer,32);
  if (atoi(Argu[2])!=LATHE) {
    f_erreur("This is not a lathe object !");
  } else {
    if (new_objet(LATHE,2)) {
      lecture_special_scn(f,"00000:",NbObjet);
      trace_volume_all(NbObjet,NbObjet);
    }
  }

  fclose(f);
  free(Buffer);
  affiche_donnees();
}


