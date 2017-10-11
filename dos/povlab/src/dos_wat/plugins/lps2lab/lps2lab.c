// ---------------------------------------------------------------------------
// LPARSER - L-System Parser/Mutator - Copyright (C) RenderStar Technology BV.
// ---------------------------------------------------------------------------
// POVLAB  - 3d modeller for POV-Ray - Copyright (C) Denis Olivier.
// ---------------------------------------------------------------------------
// LPS2LAB - Lparser to Povlab converter - Copyright (C) Denis Olivier.
// ---------------------------------------------------------------------------
// You can modify the code of this program, unless you let the copyright 
// notices without any changes.
// If you make some great modifications, let me know.
// Denis Olivier : email : dolivier@cyberstation.fr
// ---------------------------------------------------------------------------

#include <IO.H>
#include <FCNTL.H>
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <STRING.H>
#include <TIME.H>
#include <STDIO.H>
#include <DOS.H>
#include <DIRECT.H>
#include "..\PLUGINS\PLUGINS.H"
#include "..\PLUGINS\PLUGINS.C"

#define ARGU_MAX 30
char Argu[ARGU_MAX][128];
char NomLogiciel[]="LPS2LAB";
char VerLogiciel[]="1.0";

// -----------------------------------------------------------------------
// -- RETOURNE LA NIEME POSITION DE CHAINE2 DANS CHAINE1 -----------------
// -----------------------------------------------------------------------
long strinstr(long decalage,char *ch1,char *ch2) {
  register long i=0,j,k=0;

  if (decalage<0) decalage=0;
  if (strlen(ch2)<=0) return -2;

  for (i=decalage;ch1[i]!=NULL;i++) {
    k=0;j=i;
    while (ch1[i]==ch2[k]) {
      i++;k++;
      if (k==strlen(ch2)) return j;
    }
  }

  return -1;
}

// -------------------------------------------------------------------------
// -- ANALYSE UNE LIGNE D'UN FICHIER ---------------------------------------
// -------------------------------------------------------------------------
int analyse_ligne(char *TempChar,int Separateur) {
  register int k,i;
  char Marque[2];
  char *Pointeur;

  Marque[0]=Separateur;
  Marque[1]=NULL;

  for (k=0;k<ARGU_MAX;k++) Argu[k][0]=NULL;
  k=0;

  Pointeur=strtok(TempChar,Marque);

  if (Pointeur[0]=='\n') return 0;

  while (Pointeur) {
    strcpy(Argu[k],Pointeur);
    i=strinstr(0,Argu[k],"\n");
    if (i) Argu[k][i]=NULL;
    Pointeur=strtok(NULL,Marque);
    k++;
  }

  return k;
}

// -----------------------------------------------------------------------
// ------------- MESSAGE COPYRIGHT DU LOGICIEL ---------------------------
// -----------------------------------------------------------------------
void message_dos(void) {
  printf("\n");
  printf("%s v.%s, (C) Copyright ChromaGraphics, 1996-1997.\n",NomLogiciel,VerLogiciel);
  printf("L-Parser blob output to Povlab scene file converter.\n");
  printf("All rights reserved, (R) Denis Olivier - %s.\n",__DATE__);
  printf("\n");
}

// -------------------------------------------------------------------------
// -- MAIN PROGRAM AND LOOP ------------------------------------------------
// -------------------------------------------------------------------------
void main(int argc,char *argv[]) {
  FILE *f;
  FILE *File;
  char Buffer[1000];
  char Buffer2[1000];
  int i,j=0;
  VECTOR VP,VT,VS,VR;

  message_dos();

  if (!(f=fopen(argv[1],"rt"))) {
    printf("Can't find \"%s\"",argv[1]);
    exit(0);
  }

  File=fopen("OUTPUT.SCN","wt");

  
  fprintf(File,"[EndInitSection]\n");

  while (!feof(f)) {
    fgets(Buffer,256,f);
    strcpy(Buffer2,Buffer);
    analyse_ligne(Buffer,32);
    if (!strcmp(Argu[0],"component")) {
      strcpy(Buffer,Buffer2);
      for (i=0;i<strlen(Buffer);i++) {
        Buffer[i]=(Buffer[i]==',' ? 32:Buffer[i]);
        Buffer[i]=(Buffer[i]=='<' ? 32:Buffer[i]);
        Buffer[i]=(Buffer[i]=='>' ? 32:Buffer[i]);
      }
      analyse_ligne(Buffer,32);
      make_vector(VP,atof(Argu[1]),0.6,1);
      make_vector(VS,atof(Argu[2]),atof(Argu[2]),atof(Argu[2]));
      make_vector(VT,atof(Argu[3]),atof(Argu[4]),atof(Argu[5]));
      make_vector(VR,0,0,0);
      printf("\rProcessing object #%04d",j);
      sprintf(Buffer,"BLOB%04d",(int)j++);
      write_object(File,1500+j,BLOB,VP,VS,VR,VT,7,"Default",0,0,Buffer);
    }
  }

  fprintf(File,"Nb-Object: %s\n",j-1);

  fclose(File);
  fclose(f);
  puts(" ");
}
