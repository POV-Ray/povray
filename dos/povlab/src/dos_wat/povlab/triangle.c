/* ---------------------------------------------------------------------------
*  TRIANGLE.C
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
#include <DIRECT.H>
#include <STRING.H>
#include <STDIO.H>
#include <IO.H>
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <DOS.H>
#include <DOSFUNC.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

int NbPoly=-1;

POLYGONE *Poly[NB_POLY_MAX];

// -------------------------------------------------------------------------
// -- RETOURNE LES POINTS DES TRIANGLES POUR l'AFFICHAGE  -----------------
// -------------------------------------------------------------------------
int data_triangle (int N) {
  register int i,Nb=-1;

  N=Objet[N]->Poly;

  for (i=0;i<=Poly[N]->Nombre;i++) {
    Nb++;
    charge_xyz(Nb,Poly[N]->T1[i][_X],Poly[N]->T1[i][_Y],Poly[N]->T1[i][_Z],1,-1);
    Nb++;
    charge_xyz(Nb,Poly[N]->T2[i][_X],Poly[N]->T2[i][_Y],Poly[N]->T2[i][_Z],0,-1);
    Nb++;
    charge_xyz(Nb,Poly[N]->T3[i][_X],Poly[N]->T3[i][_Y],Poly[N]->T3[i][_Z],0,-1);
    Nb++;
    charge_xyz(Nb,Poly[N]->T1[i][_X],Poly[N]->T1[i][_Y],Poly[N]->T1[i][_Z],0,-1);
  }

  return Nb;
}

// -------------------------------------------------------------------------
// -- AJOUTE UN NOUVEAU VOLUME POLYGONAL -----------------------------------
// -------------------------------------------------------------------------
int new_volume_polygone(void) {
  NbPoly++;
  if (NbPoly>NB_POLY_MAX-1) {
    forme_mouse(MS_FLECHE);
    f_erreur("The max number of polygonal objects is %d",NB_POLY_MAX);
    NbPoly--;
    affiche_donnees();
    return 0;
  }
  return 1;
}

// -------------------------------------------------------------------------
// -- EFFACE UN VOLUME POLYGONAL EN MEMOIRE --------------------------------
// -------------------------------------------------------------------------
void free_mem_poly(int Num) {
  mem_free(Poly[Num],sizeof(POLYGONE));
}

// -------------------------------------------------------------------------
// -- EFFACE UN VOLUME POLYGONAL EN TRAITANT AUSSI LES OBJETS --------------
// -------------------------------------------------------------------------
void free_mem_poly_objet(int Num) {
  register int i,Nb=0;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Poly==Num) Nb++;
  }

  if (Nb==1) {
    for (i=Num;i<NbPoly;i++) {
      *Poly[i]=*Poly[i+1];
    }
    free_mem_poly(NbPoly);
    NbPoly--;
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Poly>=Num) Objet[i]->Poly--;
    }
  }
}

// -------------------------------------------------------------------------
// -- ALLOUE DE LA PLACE MEMOIRE POUR UN OBJET POLYGONAL -------------------
// -------------------------------------------------------------------------
void alloc_mem_triangle(DBL TX1,DBL TY1,DBL TZ1,
                        DBL TX2,DBL TY2,DBL TZ2,
                        DBL TX3,DBL TY3,DBL TZ3,
                        int N) {

  register int NumP=NbPoly;

  if (N==0) {
    Poly[NumP]=(POLYGONE *) mem_alloc(sizeof(POLYGONE));
    Objet[NbObjet]->Poly=NumP;
    Poly[NumP]->Smooth=0;
    log_out(0,"Alloc mem triangle : %lu %s",(long) sizeof(POLYGONE),Objet[NbObjet]->Nom);
  }

  Poly[NumP]->Nombre=N;

  Poly[NumP]->T1[N][_X]=TX1;
  Poly[NumP]->T1[N][_Y]=TY1;
  Poly[NumP]->T1[N][_Z]=TZ1;

  Poly[NumP]->T2[N][_X]=TX2;
  Poly[NumP]->T2[N][_Y]=TY2;
  Poly[NumP]->T2[N][_Z]=TZ2;

  Poly[NumP]->T3[N][_X]=TX3;
  Poly[NumP]->T3[N][_Y]=TY3;
  Poly[NumP]->T3[N][_Z]=TZ3;
}

// -------------------------------------------------------------------------
// -- SAUVE LES TRIANGLES DANS LE FICHIER .SCN -----------------------------
// -------------------------------------------------------------------------
void sauve_triangle(FILE *Fichier,int i) {
  register int j,NumP;
  register int NumT;
  char Buffer[30];

  NumP=Objet[i]->Poly;
  j=Poly[NumP]->Nombre;

  if (Poly[NumP]->Buffer) return;
  Poly[NumP]->Buffer=1;

  sprintf(Buffer,"SAVING TRIANGLES: %s",Objet[i]->Nom);
  f_jauge(1,AFFICHE,0,0,Buffer);
  fprintf(Fichier,"Object %05d: %d\n",i,j+1);

  for (NumT=0;NumT<=j;NumT++) {
    fprintf(Fichier,"Object %05d: %.7g %.7g %.7g %.7g %.7g %.7g %.7g %.7g %.7g\n",
      i,
      Poly[NumP]->T1[NumT][_X],Poly[NumP]->T1[NumT][_Y],Poly[NumP]->T1[NumT][_Z],
      Poly[NumP]->T2[NumT][_X],Poly[NumP]->T2[NumT][_Y],Poly[NumP]->T2[NumT][_Z],
      Poly[NumP]->T3[NumT][_X],Poly[NumP]->T3[NumT][_Y],Poly[NumP]->T3[NumT][_Z]
    );
    if (NumT%20==0) f_jauge(1,MODIF,NumT,j,NULL);
  }

  f_jauge(1,EFFACE,0,0,NULL);
}

// -------------------------------------------------------------------------
// -- LECTURE DES TRIANGLES DANS LE FICHIER .SCN ASCII ---------------------
// -------------------------------------------------------------------------
byte lecture_triangle_scn(FILE *Fichier,char *MarqueObjet) {
  register int i;
  char Buffer[256];
  int NbT;

  if (NbPoly<Objet[NbObjet]->Poly) {
    Poly[Objet[NbObjet]->Poly]->Buffer=1;
  } else {
    if (Poly[Objet[NbObjet]->Poly]->Buffer) return 1;
  }

  if (!new_volume_polygone()) return 0;

  sprintf(Buffer,"READING TRIANGLES: %s",Objet[NbObjet]->Nom);
  f_jauge(1,AFFICHE,0,0,Buffer);
  message("Reading triangles for object %s",Objet[NbObjet]->Nom);

  fgets(Buffer,256,Fichier);
  analyse_ligne(Buffer,32);
  NbT=atoi(Argu[2]);

  for (i=0;i<NbT;i++) {
    if (feof(Fichier)) break;
    fgets(Buffer,256,Fichier);
    analyse_ligne(Buffer,32);
    if (!strinstr(0,MarqueObjet,Argu[1])) {
      alloc_mem_triangle(atof(Argu[2]),atof(Argu[3]),atof(Argu[4]),
                         atof(Argu[5]),atof(Argu[6]),atof(Argu[7]),
                         atof(Argu[8]),atof(Argu[9]),atof(Argu[10]),
                         i);
      if (i%20==0) f_jauge(1,MODIF,i,NbT,NULL);
    } else {
      break;
    }
  }

  f_jauge(1,EFFACE,0,0,NULL);
  return 1;
}

// -------------------------------------------------------------------------
// ---- RENCENTRAGE DES TRIANGLES SUR <1,1,1> ECHELLE <0,0,0> POSITION -----
// -------------------------------------------------------------------------
#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#define MAX(A,B) (((A) > (B)) ? (A) : (B))
void adapte_polygone(int Num,int NbObjet,byte TelQuel) {
  register DBL MaxX,MaxY,MaxZ;
  register DBL MinX,MinY,MinZ;
  register int Nb=Poly[Num]->Nombre,i;
  register DBL X,Y,Z,T,T1,T2,T3;

  MaxX=MaxY=MaxZ=DBL_MIN_;
  MinX=MinY=MinZ=DBL_MAX_;

  f_jauge(1,AFFICHE,0,0,"SHEARCHING MIN/MAX"); // -------- analyse min/max axe
  for (i=0;i<=Nb;i++) {
    MaxX=(Poly[Num]->T1[i][_X]>MaxX ? Poly[Num]->T1[i][_X]:MaxX);
    MaxX=(Poly[Num]->T2[i][_X]>MaxX ? Poly[Num]->T2[i][_X]:MaxX);
    MaxX=(Poly[Num]->T3[i][_X]>MaxX ? Poly[Num]->T3[i][_X]:MaxX);
    MinX=(Poly[Num]->T1[i][_X]<MinX ? Poly[Num]->T1[i][_X]:MinX);
    MinX=(Poly[Num]->T2[i][_X]<MinX ? Poly[Num]->T2[i][_X]:MinX);
    MinX=(Poly[Num]->T3[i][_X]<MinX ? Poly[Num]->T3[i][_X]:MinX);

    MaxY=(Poly[Num]->T1[i][_Y]>MaxY ? Poly[Num]->T1[i][_Y]:MaxY);
    MaxY=(Poly[Num]->T2[i][_Y]>MaxY ? Poly[Num]->T2[i][_Y]:MaxY);
    MaxY=(Poly[Num]->T3[i][_Y]>MaxY ? Poly[Num]->T3[i][_Y]:MaxY);
    MinY=(Poly[Num]->T1[i][_Y]<MinY ? Poly[Num]->T1[i][_Y]:MinY);
    MinY=(Poly[Num]->T2[i][_Y]<MinY ? Poly[Num]->T2[i][_Y]:MinY);
    MinY=(Poly[Num]->T3[i][_Y]<MinY ? Poly[Num]->T3[i][_Y]:MinY);

    MaxZ=(Poly[Num]->T1[i][_Z]>MaxZ ? Poly[Num]->T1[i][_Z]:MaxZ);
    MaxZ=(Poly[Num]->T2[i][_Z]>MaxZ ? Poly[Num]->T2[i][_Z]:MaxZ);
    MaxZ=(Poly[Num]->T3[i][_Z]>MaxZ ? Poly[Num]->T3[i][_Z]:MaxZ);
    MinZ=(Poly[Num]->T1[i][_Z]<MinZ ? Poly[Num]->T1[i][_Z]:MinZ);
    MinZ=(Poly[Num]->T2[i][_Z]<MinZ ? Poly[Num]->T2[i][_Z]:MinZ);
    MinZ=(Poly[Num]->T3[i][_Z]<MinZ ? Poly[Num]->T3[i][_Z]:MinZ); 
  }
  f_jauge(1,EFFACE,0,0,NULL);

  T1=X=(MaxX+MinX)/(DBL)2.0;
  T2=Y=(MaxY+MinY)/(DBL)2.0;
  T3=Z=(MaxZ+MinZ)/(DBL)2.0;

  log_out(1,"X=%.4f Y=%.4f Z=%.4f MAX=%.4f MAY=%.4f MAZ=%.4f MIX=%.4f MIY=%.4f MIZ=%.4f",X,Y,Z,MaxX,MaxY,MaxZ,MinX,MinY,MinZ);

  f_jauge(1,AFFICHE,0,0,"RECENTERING ALONG XYZ AXES"); // -------- recentrage

  for (i=0;i<=Nb;i++) {
    Poly[Num]->T1[i][_X]-=X;
    Poly[Num]->T1[i][_Y]-=Y;
    Poly[Num]->T1[i][_Z]-=Z;

    Poly[Num]->T2[i][_X]-=X;
    Poly[Num]->T2[i][_Y]-=Y;
    Poly[Num]->T2[i][_Z]-=Z;

    Poly[Num]->T3[i][_X]-=X;
    Poly[Num]->T3[i][_Y]-=Y;
    Poly[Num]->T3[i][_Z]-=Z;

    if (i%10==0) f_jauge(1,MODIF,i,Nb,NULL);
  }
  f_jauge(1,EFFACE,0,0,NULL);

  message("Center X:%.2f Y:%.2f Z:%.2f",X,Y,Z);

  X=(fabs(MaxX-X)+fabs(MinX-X))/2;
  Y=(fabs(MaxY-Y)+fabs(MinY-Y))/2;
  Z=(fabs(MaxZ-Z)+fabs(MinZ-Z))/2;

  X=(X<=0 ? 1:X);
  Y=(Y<=0 ? 1:Y);
  Z=(Z<=0 ? 1:Z);

  T=X;
  T=(Y>T ? Y:T);
  T=(Z>T ? Z:T);
  T=(T<0 ? fabs(T):T);
  T=(T==0 ? 1:T);

  message("Ratio X:%.2f Y:%.2f Z:%.2f T:%.2f",X,Y,Z,T);

  f_jauge(1,AFFICHE,0,0,"SCALING"); // -------- mise … l'‚chelle
  for (i=0;i<=Nb;i++) {
    Poly[Num]->T1[i][_X]/=X;
    Poly[Num]->T2[i][_X]/=X;
    Poly[Num]->T3[i][_X]/=X;

    Poly[Num]->T1[i][_Y]/=Y;
    Poly[Num]->T2[i][_Y]/=Y;
    Poly[Num]->T3[i][_Y]/=Y;

    Poly[Num]->T1[i][_Z]/=Z;
    Poly[Num]->T2[i][_Z]/=Z;
    Poly[Num]->T3[i][_Z]/=Z;

    if (i%10==0) f_jauge(1,MODIF,i,Nb,NULL);
  }
  f_jauge(1,EFFACE,0,0,NULL);

  message("Largest X:%.2f Y:%.2f Z:%.2f",X,Y,Z);

  if (TelQuel) {
    Objet[NbObjet]->S[_X]=fabs(X);
    Objet[NbObjet]->S[_Y]=fabs(Y);
    Objet[NbObjet]->S[_Z]=fabs(Z);

    Objet[NbObjet]->T[_X]=T1;
    Objet[NbObjet]->T[_Y]=T2;
    Objet[NbObjet]->T[_Z]=-T3;
  } else {
    Objet[NbObjet]->S[_X]=(DBL) fabs(X/T);
    Objet[NbObjet]->S[_Y]=(DBL) fabs(Y/T);
    Objet[NbObjet]->S[_Z]=(DBL) fabs(Z/T);

    if (Objet[NbObjet]->S[_X]<=0) Objet[NbObjet]->S[_X]=1;
    if (Objet[NbObjet]->S[_Y]<=0) Objet[NbObjet]->S[_Y]=1;
    if (Objet[NbObjet]->S[_Z]<=0) Objet[NbObjet]->S[_Z]=1;
  }
}

// -------------------------------------------------------------------------
// ----------------- LISSE LES TRIANGLES POUR SORTIE RAYTRACER -------------
// -------------------------------------------------------------------------
byte lisse_triangle(int NumPoly,FILE *Fichier,byte Niveau,int N) {
  register int i;

  for (i=0;i<=Poly[NumPoly]->Nombre;i++) {
    opt_add_tri (
       Poly[NumPoly]->T1[i][_X],
      -Poly[NumPoly]->T1[i][_Y],
       Poly[NumPoly]->T1[i][_Z],
       Poly[NumPoly]->T2[i][_X],
      -Poly[NumPoly]->T2[i][_Y],
       Poly[NumPoly]->T2[i][_Z],
       Poly[NumPoly]->T3[i][_X],
      -Poly[NumPoly]->T3[i][_Y],
       Poly[NumPoly]->T3[i][_Z]
    );
  }
  opt_write_pov(Fichier,Niveau,N);
  return 0;
}

// --------------------------------------------------------------------------
// ----- REINITIALISE LE CHAMP BUFFER DE LA STRUCT POLY ---------------------
// --------------------------------------------------------------------------
void reinit_buffer_poly(void) {
  register int i;

  for (i=0;i<=NbPoly;i++) Poly[i]->Buffer=0;
}

// -------------------------------------------------------------------------
// -- LECTURE DES TRIANGLES DANS UN FICHIER RAW ----------------------------
// -------------------------------------------------------------------------
int lecture_triangle_raw(char *NomFichierRaw,byte NouveauObj) {
  struct find_t find;
  FILE *Fichier;
  char Buffer[256],NomFichier[MAXPATH];
  int i,Ok=1,Nb,NewObjet;
  int FileLen;
  char *Spec[2]={"1","*.RAW"};

  if (NbObjet) {
    if (NbPoly<Objet[NbObjet]->Poly && NouveauObj==0) {
      Poly[Objet[NbObjet]->Poly]->Buffer=1;
    } else {
      if (Poly[Objet[NbObjet]->Poly]->Buffer && NouveauObj==0) return 1;
    }
  }

  LECTURE_ERREUR_RAW:

  if (NomFichierRaw[0]=='?') {
    chdir("RAW");
    strcpy(NomFichier,selection_fichier(100,100,"RAW TRIANGLES",Spec));
    if (NomFichier[0]==27) return 0;
  } else {
    strcpy(NomFichier,NomFichierRaw);
  }

  if (!test_fichier(NomFichier)) {
    strcpy(StrBoite[0],"READING RAW FILE");
    sprintf(StrBoite[1],"Can't find raw file %s",NomFichier);
    strcpy(StrBoite[2],"\"Ok\" to specify another path,");
    strcpy(StrBoite[3],"\"Cancel\" to delete object...");
    if (g_boite_ONA(CentX,CentY,3,CENTRE,1)) return 0;
    strcpy(NomFichierRaw,"?");
    goto LECTURE_ERREUR_RAW;
  }

  _dos_findfirst(NomFichier,0,&find);
  FileLen=find.size;

  Fichier=fopen(NomFichier,"r+t");
    fgets(Buffer,255,Fichier);
    Ok=analyse_ligne(Buffer,32);
  fclose(Fichier);

  for (i=0;i<=strlen(Buffer);i++) {
    if (Buffer[i]>127 || Ok!=1) {
      f_erreur("This is not a valid file|"\
               "because there's no object's name|"\
               "on the first line of the file");
      Ok=0;
      break;
    }
  }

  if (Ok) {
    f_jauge(1,AFFICHE,0,0,"READING RAW TRIANGLES");
    Fichier=fopen(NomFichier,"r+t");
    NewObjet=-1;

    while (!feof(Fichier)) {
      fgets(Buffer,255,Fichier);
      Buffer[255]=NULLC;
      i=analyse_ligne(Buffer,32);
      if (i<1) goto LECTURE_RAW;
      if (feof(Fichier)) break;
      if (i==1) {
        if (NouveauObj) if (!new_objet(TRIANGLE,0)) { Ok=-1; break; }
        if (!new_volume_polygone()) { NbObjet--; break; }
        if (strlen(Argu[0])>12) Argu[0][12]=NULLC;
        strcpy(Objet[NbObjet]->Nom,Argu[0]);
        strcpy(Objet[NbObjet]->CheminRaw,NomFichier);
        NewObjet++;
        message("Reading object nø%d : %s",NbObjet,Argu[0]);
        Nb=0;
        goto LECTURE_RAW;
      }
      if (Nb>=NB_TRIANGLE_MAX-1) {
        free_mem_poly(NbPoly);
        NbPoly--;
        f_erreur("Too many triangles in the file !|Limited to %d triangles.",NB_TRIANGLE_MAX);
        log_out(0,"ERROR : too many triangles in %s",Objet[NbObjet]->Nom);
        Ok=-1;
        f_jauge(1,EFFACE,0,0,NULL);
        return 0;
      }
      alloc_mem_triangle(atof(Argu[0]),-atof(Argu[1]),atof(Argu[2]),
                         atof(Argu[3]),-atof(Argu[4]),atof(Argu[5]),
                         atof(Argu[6]),-atof(Argu[7]),atof(Argu[8]),
                         Nb);
      if (ftell(Fichier)%10==0) f_jauge(1,MODIF,ftell(Fichier),FileLen,NULL);
      Nb++;
      LECTURE_RAW:
      i=i;
    }
  }

  if (NouveauObj) if (Ok==-1) { free_mem_objet(NbObjet); NbObjet--; }
  if (Ok==-2) { free_mem_poly(NbPoly); NbPoly--; }

  fclose(Fichier);

  f_jauge(1,EFFACE,0,0,NULL);

  if (Ok>0) {
    if (NewObjet!=-1) {
      for (i=NbObjet-NewObjet;i<=NbObjet;i++) {
        if (NouveauObj) {
          if (x_fenetre(0,0,GAUCHE,1,"Rescaling shape|"\
                "POVLAB will rescale the shape %s in a unit box ?",Objet[i]->Nom)==0) {
            adapte_polygone(Objet[i]->Poly,i,0);
          } else {
            adapte_polygone(Objet[i]->Poly,i,1);
          }
        } else {
          adapte_polygone(Objet[i]->Poly,i,0);
        }
      }
    }
    message("Nb triangles %d",Poly[NbPoly]->Nombre);
    affiche_donnees();
    if (NouveauObj) trace_volume_all(NbObjet-NewObjet,NbObjet);
    return 1;
  }

  return 0;
}
