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

typedef struct {
  double Long;
  double Angle;
  double X,Y;
} CYL;

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

  if (!(File=fopen("GATEWAY.PLG","wt"))) return 0;

  fprintf(File,"TITLE: GATEWAY_FORM_2D_1.0\n");
  fprintf(File,"MESSAGE: Copyright_Denis_Olivier_1996_-_All_rights_reserved.\n");
  fprintf(File,"WINDOW: 180 190\n");
  fprintf(File,"TEXTZONE: 75  40 Nb_vertices 4 Vertices_around_the_gateway\n");
  fprintf(File,"TEXTZONE: 75  60 Radius 1.0 gateway's radius\n");
  fprintf(File,"TEXTZONE: 75  80 Cyl_Radius 0.01 Cylinder's_radius_for_connection\n");
  fprintf(File,"TEXTZONE: 75 100 Offset 0.15 Offset_for_next_cylinder\n");
  fprintf(File,"TEXTZONE: 75 120 Nb_rotations 8 Nb_of_polygon_rotations\n");
  fprintf(File,"END:\n");

  fclose(File);
  return 1;
}

// ---------------------------------------------------------------------------
// -- COMPUTE THE SX,RX,TX OF A CYLINDER WITH JUST 2 POINTS ------------------
// ---------------------------------------------------------------------------
CYL calcul_cylinder(double X1,double Y1,double X2,double Y2) {
  CYL C;

  C.Long=sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2))/2;
  C.Angle=90+atan2((Y1-Y2),(X1-X2))*(180/PI);
  C.X=(X1+X2)/2;
  C.Y=(Y1+Y2)/2;

  return C;
}

// ---------------------------------------------------------------------------
// -- MAIN PROGRAM -----------------------------------------------------------
// ---------------------------------------------------------------------------
void main (int argc,char *argv[]) {
  FILE *File;
  int Nb=1000; // ----- Start at object nb 1000 (why, I dunno !)
  char NomLogiciel[]={"GATEWAY"};
  char VerLogiciel[]={"1.0"};
  union REGS regs;
  VECTOR VP,VS,VR,VT;
  char Buffer[20];
  
  double R=1.0;
  double N=4;
  int i,j;
  double T;
  double X1,X0,Y1,Y0;
  double E=0.01,D=.15,P=8;
  CYL C;

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
  // -------------- Put your copyright infos here

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
  // ------------- file you're running :) !
  // ------------- Here follow the code for your own process :
  // ------------- Coding what you want to do with the object.

  File=fopen("GATEWAY.INC","w+t");

  N=atof(argv[1]);
  R=atof(argv[2]);
  E=atof(argv[3]);
  D=atof(argv[4]);
  P=atof(argv[5]);

  X0=1.0;
  Y0=0.00000001;
  X1=Y1=0.0;
  T=2*PI/N;

  for (j=1;j<=(int) P;j++) {
    for (i=1;i<=(int) P;i++) {
      X1= X0*R*cos(T)+Y0*R*sin(T);
      Y1=-X0*R*sin(T)+Y0*R*cos(T);
      C=calcul_cylinder(X0,Y0,X1,Y1);

      if (i!=P) {
        X0=X1;
        Y0=Y1;
      }

      // ------------------------ Prepare the datas

      make_vector(VP,0,0,0);
      make_vector(VS,(double)   E,(double) C.Long,(double)       E);
      make_vector(VR,           0,              0,(double) C.Angle);
      make_vector(VT,(double) C.X,(double)   -C.Y,               0);
      sprintf(Buffer,"CYL%04d",Nb);

      // ------------------------ Here's the part to write the file

      write_object(File,Nb,CYLINDRE,VP,VS,VR,VT,7,"Default",0,0,Buffer);

      Nb=Nb+1;

      if ((kbhit()) && getch()==27) {
        fclose(File);
        remove("GATEWAY.INC");
        exit(0);
      }

      // ----------------------------------------------------------
    }

    X0=X0+(D*(X1-X0));
    Y0=Y0+(D*(Y1-Y0));
  }

  fclose(File);
}

