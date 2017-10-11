/*
 * ATTRACTEUR DE ROSSLER (avec affichage en 3D)
 * (c) 10/11/95, St‚phane MARTY
 * Source Borland C/C++ 3.1
 */

#include <STDIO.H>
#include <STDLIB.H>
#include <CONIO.H>
#include <MATH.H>
#if __WATCOMC__
  #include <I86.H>
#else
  #include <DOS.H>
#endif

#define Arrondi(a)      ((a)>0 ? (int)(a+0.5) : -(int)(0.5-a))
#define CosD(a)         (cos(a*0.017453925))
#define SinD(a)         (sin(a*0.017453925))

static float Mx, My, Mz;
static float Angle1,      // rotation Z
             Angle2,      // inclinaison plan XY
             Scale;
static float CosA, SinA, CosB, SinB;
static float CosACosB, SinASinB, CosASinB, SinACosB;

// ---------------------------------------------------------------------------
// -- CREATION DU SCRIPT POUR POVLAB -----------------------------------------
// ---------------------------------------------------------------------------
int out_data_interface(void) {
  FILE *Fichier;

  // ------------- Don't forget to name the .PLG like the .EXE
  // ------------- file you're running |-) !
  // ------------- The command line parameters are returned in the
  // ------------- Order you generate the graphic datas.

  if (!(Fichier=fopen("ROSSLER.PLG","wt"))) return 0;

  fprintf(Fichier,"TITLE: ROSSLER_3D_1.0\n");
  fprintf(Fichier,"COPYRIGHT: Copyright_St‚phane_Marty_1995_-_All_rights_reserved.\n");
  fprintf(Fichier,"WINDOW: 270 170\n");
  fprintf(Fichier,"TEXTZONE:  83 50 Angle_1 -45 Angle_1\n");
  fprintf(Fichier,"TEXTZONE:  83 70 Angle_2 35 Angle_2\n");
  fprintf(Fichier,"TEXTZONE:  83 90 Passes 3000 How_many_passes\n");
  fprintf(Fichier,"RADIO: 190 50 Cube 0 Generate_cubes 1\n");
  fprintf(Fichier,"RADIO: 190 70 Sphere 1 Generate_spheres 1\n");
  fprintf(Fichier,"RADIO: 190 90 Cylinders 0 Generate_cylinders 1\n");
  fprintf(Fichier,"TEXTZONE: 83 110 Object's_scale 0.2 Object's_scale\n");
  fprintf(Fichier,"CASE: 190 110 Pause 1 Pause_after_computed\n");
  fprintf(Fichier,"END:\n");

  fclose(Fichier);
  return 1;
}

// -------------------------------------------------------------------------
// -- INITIALISE LES VARIABLES UTILISEES DANS LE CALCUL DE LA PERSPECTIVE --
// -------------------------------------------------------------------------
void Init_3D(void) {
  CosA = CosD(Angle1);
  SinA = SinD(Angle1);
  CosB = CosD(Angle2);
  SinB = SinD(Angle2);
  CosACosB = CosA * CosB;
  SinASinB = SinA * SinB;
  CosASinB = CosA * SinB;
  SinACosB = SinA * CosB;
}

// -------------------------------------------------------------------------
// -- POINT D'ENTREE DU PROGRAMME ------------------------------------------
// -------------------------------------------------------------------------
void main(int argc,char *argv[]) {
  short i;
  float x, y, z, dt = 0.04, dx, dy, dz, A, B, C;
  int iterat = 3000;
  FILE *Fichier;
  int Nb=1000;
  int Objet,Pause;
  char NomLogiciel[]={"ROSSLER 3D"};
  char VerLogiciel[]={"1.0"};
  union REGS regs;

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
  printf("%s version %s, (C) Copyright ChromaGraphics, 1994-1995.\n",NomLogiciel,VerLogiciel);
  printf("External process for POVLAB - Generate a Rossler function.\n");
  printf("All rights reserved, (R) St‚phane Marty - %s.\n",__DATE__);
  puts("");

  Angle1=atof(argv[1]);  // d‚faut -45;
  Angle2=atof(argv[2]);  // d‚faut 35;
  iterat=atof(argv[3]);  // d‚faut 3000;
  switch(atoi(argv[4])) {
    case 0: Objet=2; break;  // Cube
    case 1: Objet=1; break;  // Sphere
    case 2: Objet=0; break;  // Cylindre
  }
  Scale=atof(argv[5]);   // d‚faut 0.2;
  Pause=atoi(argv[6]);   // d‚faut pause;

  Fichier=fopen("rossler.inc","wt");

  Mx = 0; My = -20; Mz = 80;    // position de l'observateur
  Init_3D();
  
  x = y = z = 1.0;
  A = B = 0.2;
  C = 5.7;         // valeur des paramŠtres

  do {
    //fprintf(Fichier, "sphere { centre(%f, %f, %f) rayon 0.3 }\n", x, y, z);
	
    fprintf(Fichier,"\n");
    fprintf(Fichier,"Object %05d: %d\n",Nb,Objet);
    fprintf(Fichier,"Object %05d: 0 0 0\n",Nb);
    fprintf(Fichier,"Object %05d: %.4f %.4f %.4f\n",Nb,Scale,Scale,Scale);
    fprintf(Fichier,"Object %05d: 0 0 0\n",Nb);
    fprintf(Fichier,"Object %05d: %.2f %.2f %.2f\n",Nb,x,y,z);
    fprintf(Fichier,"Object %05d: 7\n",Nb);
    fprintf(Fichier,"Object %05d: Default\n",Nb);
    fprintf(Fichier,"Object %05d: 0\n",Nb); // selection 1=true
    fprintf(Fichier,"Object %05d: 0\n",Nb); // hidden 1=true
    fprintf(Fichier,"Object %05d: RSS%05d\n",Nb,Nb);
    Nb=Nb+1;
    printf("\rPass %d object %d.",iterat,Nb-1000);

    dx = -(y + z);
    dy = x + y * A;
    dz = B + z * (x - C);
    x += dt * dx;
    y += dt * dy;
    z += dt * dz;
	iterat--;
    Nb++;
  } while(!(kbhit()) && iterat);

  fclose(Fichier);

  printf("\nSuccess.\n");

  if (Pause) {
    printf("\nHit a key...\n");
    getch();
  }
}
