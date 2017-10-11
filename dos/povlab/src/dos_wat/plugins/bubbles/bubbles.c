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

  if (!(File=fopen("BUBBLES.PLG","wt"))) return 0;

  fprintf(File,"TITLE: BUBBLES_1.0\n");
  fprintf(File,"COPYRIGHT: (C)_Denis_Olivier_1996.\n");
  fprintf(File,"WINDOW: 190 200\n");
  fprintf(File,"TEXTZONE: 85  40 Nb_bubbles 20 Nb_of_bubbles_in_the_box\n");
  fprintf(File,"TEXTZONE: 85  60 Box_lenght_(x) 2 Box_lenght\n");
  fprintf(File,"TEXTZONE: 85  80 Box_height_(y) 2 Box_height\n");
  fprintf(File,"TEXTZONE: 85 100 Box_depth_(z) 2 Box_depth\n");
  fprintf(File,"TEXTZONE: 85 120 Max_size_radius 0.2 ..._for_bubbles\n");
  fprintf(File,"TEXTZONE: 85 140 Min_size_radius 0.02 ..._for_bubbles\n");
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
  char NomLogiciel[]={"BUBBLES"};
  char VerLogiciel[]={"1.0"};
  union REGS regs;
  VECTOR VP,VS,VR,VT;
  char Buffer[20];
  #define Rd (double) (rand()%100)/100

  int i,j;
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

  File=fopen("BUBBLES.INC","w+t");

  Nb=atof(argv[1]);
  X=atof(argv[2]);
  Y=atof(argv[3]);
  Z=atof(argv[4]);
  BMax=atof(argv[5]);
  BMin=atof(argv[6]);

  for (i=1;i<=(int)Nb;i++) {
    T=((BMax-BMin)*Rd)+BMin;
    make_vector(VP,0,0,0);
    make_vector(VS,T,T,T);
    make_vector(VR,0,0,0);
    A=((X-T*2)*Rd)-((X/2)-T);
    B=((Y-T*2)*Rd)-((Y/2)-T);
    C=((Z-T*2)*Rd)-((Z/2)-T);
    make_vector(VT,A,B,C);
    sprintf(Buffer,"BUBB%04d",(int)i);
    write_object(File,Start+i,SPHERE,VP,VS,VR,VT,7,"Default",0,0,Buffer);
    if ((kbhit()) && getch()==27) {
      fclose(File);
      remove("BUBBLES.INC");
      exit(0);
    }
  }

  fclose(File);
}

