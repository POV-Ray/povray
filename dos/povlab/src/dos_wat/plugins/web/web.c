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
// --                        T ‚ l :  4 6  0 5  4 9  4 4
// --                        F a x :  4 6  0 5  6 7  4 3
// --
// --   Compiled and linked under :
// --
// --   *  Watcom 10.a 32 C/C++ : wcl ... (no options)
// --   *  Borland C/C++ 3.1    : bcc ... (no options)
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
#include <MATH.H>
#include <IO.H>

typedef struct {
  float Long;
  float Angle;
  float X,Y;
} CYL;

#define PI 3.1415926535

// ---------------------------------------------------------------------------
// -- CREATION DU SCRIPT POUR POVLAB -----------------------------------------
// ---------------------------------------------------------------------------
int out_data_interface(void) {
  FILE *Fichier;

  // ------------- Don't forget to name the .PLG like the .EXE
  // ------------- file you're running |-) !
  // ------------- The command line parameters are returned in the
  // ------------- Order you generate the graphic datas.

  if (!(Fichier=fopen("WEB.PLG","wt"))) return 0;

  fprintf(Fichier,"TITLE: SPIDER'S_WEB_2D_1.0\n");
  fprintf(Fichier,"MESSAGE: Copyright_Denis_Olivier_1995_-_All_rights_reserved.\n");
  fprintf(Fichier,"WINDOW: 180 150\n");
  fprintf(Fichier,"TEXTZONE: 75  40 Nb_vertices 10 Vertices_around_the_web\n");
  fprintf(Fichier,"TEXTZONE: 75  60 Radius 1.0 Web's radius\n");
  fprintf(Fichier,"TEXTZONE: 75  80 Cyl_Radius 0.01 Cylinder's_radius_for_connection\n");
  fprintf(Fichier,"END:\n");

  fclose(Fichier);
  return 1;
}

// ---------------------------------------------------------------------------
// -- ALGORITHME CALCUL POSITION/ANGLE/LONGUEUR D'1 CYL EN FCTø 2 PTS --------
// ---------------------------------------------------------------------------
CYL calcul_cylindre(float X1,float Y1,float X2,float Y2) {
  CYL C;

  C.Long=sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2))/2;
  C.Angle=90+atan2((Y1-Y2),(X1-X2))*(180/PI);
  C.X=(X1+X2)/2;
  C.Y=(Y1+Y2)/2;

  return C;
}

// ---------------------------------------------------------------------------
// -- CREATION DE GRILLE D'OBJET ---------------------------------------------
// ---------------------------------------------------------------------------
void main (int argc,char *argv[]) {
  FILE *Fichier;
  int Nb=1000; // ----- Start at object nb 1000 (why, I dunno !)
  char NomLogiciel[]={"NAPPERON"};
  char VerLogiciel[]={"1.0"};
  union REGS regs;
  
  float R=1.0;
  float N=10;
  float i,j;
  float TI,TJ;
  float E;
  CYL C;

  // ----------- The command line parameter /ASK tell the program
  // ----------- to put out the graphic data script for the
  // ----------- interface under POVLAB.

  if (argc>1) {
    if (!strcmp(strupr(argv[1]),"/ASK")) {
      out_data_interface();
      exit(0);
    }
  } else {
    exit(0);
  }

  // -------------- Don't clear the screen if you dont want, there's
  // -------------- not reason except to display some
  // -------------- Informations about the work it's doing.

  /*
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
  */

  // ------------- Don't forget to name the .INC like the .EXE
  // ------------- file you're running |-) !
  // ------------- Here follow the code for your own process :
  // ------------- Coding what you want to do with the object.

  Fichier=fopen("WEB.INC","w+t");

  N=atoi(argv[1]);
  R=atof(argv[2]);
  E=atof(argv[3]);

  for (i=0;i<=N-2;i+=1) {
    TI=(float) 2*PI*i/N;
    for (j=i+1;j<=N-1;j+=1) {
      TJ=(float) 2*PI*j/N;
      C=calcul_cylindre(R*cos(TI),R*sin(TI),
                        R*cos(TJ),R*sin(TJ));
      fprintf(Fichier,"\n");
      fprintf(Fichier,"Object %05d: 0\n",Nb);
      fprintf(Fichier,"Object %05d: 0 0 0\n",Nb);
      fprintf(Fichier,"Object %05d: %.4f %.4f %.4f\n",Nb,E,C.Long,E);
      fprintf(Fichier,"Object %05d: 0 0 %.3fd\n",Nb,C.Angle);
      fprintf(Fichier,"Object %05d: %.3f %.3f 0\n",Nb,C.X,-C.Y);
      fprintf(Fichier,"Object %05d: 7\n",Nb);
      fprintf(Fichier,"Object %05d: Default\n",Nb);
      fprintf(Fichier,"Object %05d: 0\n",Nb); // selection 1=true
      fprintf(Fichier,"Object %05d: 0\n",Nb); // hidden 1=true
      fprintf(Fichier,"Object %05d: GRD%05d\n",Nb,Nb);
      Nb=Nb+1;
      //printf("\rWidth %d height %d depth %d object %d",i,j,k,Nb-1000);
      if ((kbhit()) && getch()==27) {
        fclose(Fichier);
        remove("WEB.INC");
        exit(0);
      }
    }
  }

  fclose(Fichier);
}

