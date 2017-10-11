// ----------------------------------------------------------------------------
// --
// --  Ce source est livr‚ tel quel, les modifications encourues ‚tant sous
// --  la responsabilit‚ du programmeur effectuant ces derniŠres.
// --  Respecter le format, afin que la lisibilit‚ soit conforme et homogŠne.
// --  Si vous pensez avoir r‚alis‚ des am‚lioration, contactez :
// --
// --                        C H R O M A G R A P H I C S
// --                        D E N I S     O L I V I E R
// --                        5,  boulevard  Franck  Lamy
// --                        1 7 2 0 0     R  O  Y  A  N
// --                        F    R    A     N    C    E
// --                        ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
// --                        T ‚ l :  5 6  4 7  2 9  5 4
// --                        F a x :  4 6  0 5  6 7  4 3
// --
// --   Compiled and linked under :
// --
// --   *  Watcom 10.a 32 C/C++ : wcl ... (no options)
// --   *  Borland C/C++ 3.1    : bcc ... (no options)
// --
// --   This source is release to the public domain. Do what you want with
// --   it, and especially good things !
// --
// ----------------------------------------------------------------------------

#include <STDIO.H>
#if __WATCOMC__
  #include <I86.H>
#else
  #include <DOS.H>
#endif
#include <STRING.H>
#include <CONIO.H>
#include <STDLIB.H>
#include <IO.H>

// ---------------------------------------------------------------------------
// -- CREATION DU SCRIPT POUR POVLAB -----------------------------------------
// ---------------------------------------------------------------------------
int out_data_interface(void) {
  FILE *Fichier;

  // ------------- Don't forget to name the .PLG like the .EXE
  // ------------- file you're running |-) !
  // ------------- The command line parameters are returned in the
  // ------------- Order you generate the graphic datas.

  if (!(Fichier=fopen("GRID3D.PLG","wt"))) return 0;

  fprintf(Fichier,"TITLE: GRID_3D_1.0\n");
  fprintf(Fichier,"COPYRIGHT: Copyright_Denis_Olivier_1995_-_All_rights_reserved.\n");
  fprintf(Fichier,"WINDOW: 410 180\n");
  fprintf(Fichier,"TEXTZONE:  83  60 Width_of_Array 5 Width_of_the_object's_array\n");
  fprintf(Fichier,"TEXTZONE:  83  80 Height_of_Array 5 Height_of_the_object's_array\n");
  fprintf(Fichier,"TEXTZONE:  83 100 Depth_of_Array 5 Depth_of_the_object's_array\n");
  fprintf(Fichier,"TEXTZONE: 240  60 Nb_Width 5 Nb_of_objects_in_Width\n");
  fprintf(Fichier,"TEXTZONE: 240  80 Nb_Height 5 Nb_of_objects_in_Height\n");
  fprintf(Fichier,"TEXTZONE: 240 100 Nb_Depth 5 Nb_of_objects_in_Depth\n");
  fprintf(Fichier,"TEXTZONE:  83 130 Object_size 0.2 Scale_for_object_(X-Y-Z)\n");
  fprintf(Fichier,"RADIO: 345  60 Cube 1 Generate_cubes 1\n");
  fprintf(Fichier,"RADIO: 345  80 Sphere 0 Generate_spheres 1\n");
  fprintf(Fichier,"RADIO: 345 100 Cylinder 0 Generate_cylinders 1\n");
  fprintf(Fichier,"CASE: 240 125 Pause 1 Pause_after_computed\n");
  fprintf(Fichier,"MESSAGE: The_array_begin_in_<0,0,0>\n");
  fprintf(Fichier,"END:\n");

  fclose(Fichier);
  return 1;
}

// ---------------------------------------------------------------------------
// -- CREATION DE GRILLE D'OBJET ---------------------------------------------
// ---------------------------------------------------------------------------
void main (int argc,char *argv[]) {
  FILE *Fichier;
  int Nb=1000; // ----- Start at object nb 1000 (why, I dunno !)
  int i,j,k;
  char NomLogiciel[]={"GRID MAKER"};
  char VerLogiciel[]={"1.0"};
  union REGS regs;
  float X,PX;
  float Y,PY;
  float Z,PZ;
  float S;
  int O;

  // ----------- The command line parameter /ASK tell the program
  // ----------- to put out the graphic data script for the
  // ----------- interface under POVLAB.

  if (argc>1) {
    if (strcmp(strupr(argv[1]),"/ASK")==NULL) {
      out_data_interface();
      exit(0);
    }
  } else {
    exit(0);
  }

  // -------------- Don't clear the screen if you dont want, there's
  // -------------- not reason except to display some
  // -------------- Informations about the work it's doing.

  #if __WATCOMC__
    regs.w.ax=3;
  #else
    regs.x.ax=3;
  #endif
  int86(0x10,&regs,&regs); // Textmode with int 10H

  puts("");
  printf("%s release %s, (C) Copyright ChromaGraphics, 1994-1995.\n",NomLogiciel,VerLogiciel);
  printf("External process for POVLAB - Generate a grid of objects.\n");
  printf("All rights reserved, (R) Denis Olivier - %s.\n",__DATE__);
  puts("");

  // ------------- Don't forget to name the .INC like the .EXE
  // ------------- file you're running |-) !
  // ------------- Here follow the code for your own process :
  // ------------- Coding what you want to do with the object.

  Fichier=fopen("GRID3D.INC","w+t");

  X=atof(argv[1]);
  Y=atof(argv[2]);
  Z=atof(argv[3]);

  PX=X/atof(argv[4]);
  PY=Y/atof(argv[5]);
  PZ=Z/atof(argv[6]);

  S=atof(argv[7]);

  switch(atoi(argv[8])) {
    case 0: O=2; break;      // Cube
    case 1: O=1; break;      // Sphere
    case 2: O=0; break;      // Cylindre
  }

  for (i=0;i<=X;i+=PX) {
    for (j=0;j<=Y;j+=PY) {
      for (k=0;k<=Z;k+=PZ) {
        fprintf(Fichier,"\n"); // You need this line first
        fprintf(Fichier,"Object %05d: %d\n",Nb,O);
        fprintf(Fichier,"Object %05d: 0 0 0\n",Nb);
        fprintf(Fichier,"Object %05d: %.4f %.4f %.4f\n",Nb,S,S,S);
        fprintf(Fichier,"Object %05d: 0 0 0\n",Nb);
        fprintf(Fichier,"Object %05d: %d %d %d\n",Nb,i,j,k);
        fprintf(Fichier,"Object %05d: 7\n",Nb);
        fprintf(Fichier,"Object %05d: Default\n",Nb);
        fprintf(Fichier,"Object %05d: 0\n",Nb); // selection 1=true
        fprintf(Fichier,"Object %05d: 0\n",Nb); // hidden 1=true
        fprintf(Fichier,"Object %05d: GRD%05d\n",Nb,Nb);
        Nb=Nb+1;
        printf("\rWidth %d height %d depth %d object %d",i,j,k,Nb-1000);
        if ((kbhit()) && getch()==27) {
          fclose(Fichier);
          remove("GRID3D.INC");
          exit(0);
        }
      }
    }
  }

  fclose(Fichier);
  printf("\nTerminate with success.\n");
  if (atoi(argv[9])) {
    printf("\nHit a key to continue...\n");
    getch();
  }
}

