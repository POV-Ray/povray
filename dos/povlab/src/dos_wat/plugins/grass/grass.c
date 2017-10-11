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
#include <FLOAT.H>
#include <MATH.H>
#include <IO.H>

// ----------------------- Here is included the PLUGINS C and H files

#include "PLUGINS.H"
#include "PLUGINS.C"

#define PI 3.14159265358979323846264338327950288419716939937511

// ---------------------------------------------------------------------------
// -- DATA SCRIPT FOR POVLAB -------------------------------------------------
// ---------------------------------------------------------------------------
int out_data_interface(void) {
  FILE *File;

  // ------------- Don't forget to name the .PLG like the .EXE
  // ------------- file you're running |-) !
  // ------------- The command line parameters are returned in the
  // ------------- Order you generate the graphic datas.

  if (!(File=fopen("GRASS.PLG","wt"))) return 0;

  fprintf(File,"TITLE: GRASS_1.01\n");
  fprintf(File,"COPYRIGHT: (C)_Denis_Olivier_1996.\n");
  fprintf(File,"WINDOW: 190 135\n");
  fprintf(File,"TEXTZONE: 85  40 Nb_grass 20 Nb_of_grass_in_the_box\n");
  fprintf(File,"TEXTZONE: 85  60 Box_width_(x) 2 Box_width\n");
  fprintf(File,"TEXTZONE: 85 80 Box_lenght_(z) 2 Box_lenght\n");
  fprintf(File,"RADIO: 10  100 Cone 1 Generate_cones 1\n");
  fprintf(File,"RADIO: 10 115 Cylinder 0 Generate_cylinders 1\n");
  fprintf(File,"END:\n");

  fclose(File);
  return 1;
}

// ---------------------------------------------------------------------------
// -- MAIN PROGRAM -----------------------------------------------------------
// ---------------------------------------------------------------------------
void main (int argc,char *argv[]) {
  FILE *File;
  int Start=1000; // ----- Start at object nb 1000 (why, I dunno !)
  char NomLogiciel[]={"GRASS"};
  char VerLogiciel[]={"1.0"};
  union REGS regs;
  VECTOR VP,VS,VR,VT;
  char Buffer[20];
  #define Rd (double) (rand()%100)/100

  int i,j;
  int Obj=CONE;
  double X1,X0,Y1,Y0;
  double X,Y,Z,T;
  double BMax,BMin,Nb;
  double A,B,C;

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

  // ------------- Don't forget to name the .INC like the .EXE
  // ------------- file you're running :) !
  // ------------- Here follow the code for your own process :
  // ------------- Coding what you want to do with the object.

  File=fopen("GRASS.INC","w+t");

  Nb=atof(argv[1]);
  X=atof(argv[2]);
  Z=atof(argv[3]);

  Obj=atoi(argv[4]) ? CYLINDRE:CONE;
  BMax=1.0;
  BMin=0.5;

  for (i=1;i<=(int)Nb;i++) {
    T=((BMax-BMin)*Rd)+BMin;
    make_vector(VP,0,0,0);
    make_vector(VS,0.04,T,0.04);
    make_vector(VR,Rd*4-2,0,Rd*4-2);
    A=((X-T*2)*Rd)-((X/2)-T);
    C=((Z-T*2)*Rd)-((Z/2)-T);
    make_vector(VT,A,-T,C);
    sprintf(Buffer,"GRASS%04d",(int)i);
    write_object(File,Start+i,Obj,VP,VS,VR,VT,7,"Default",0,0,Buffer);
    if ((kbhit()) && getch()==27) {
      fclose(File);
      remove("GRASS.INC");
      exit(0);
    }
  }

  fclose(File);
}

