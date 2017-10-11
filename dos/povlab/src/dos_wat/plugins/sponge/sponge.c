#include <MATH.H>
#include <FLOAT.H>
#include <STDIO.H>
#include <BIOS.H>
#include <STDLIB.H>
#include <STRING.H>

#include "PLUGINS.H"
#include "PLUGINS.C"

int max_level=2;
char Buffer[20];
VECTOR VP,VS,VR,VT;
FILE *File;
double S;
int O;

union REGS regs;

// ---------------------------------------------------------------------------
// -- DATA SCRIPT FOR POVLAB -------------------------------------------------
// ---------------------------------------------------------------------------
int out_data_interface(void) {
  // ------------- Don't forget to name the .PLG like the .EXE
  // ------------- file you're running ;) !
  // ------------- The command line parameters are returned in the
  // ------------- Order you generate the graphic datas.

  if (!(File=fopen("SPONGE.PLG","wt"))) return 0;

  fprintf(File,"TITLE: SPONGE_1.0\n");
  fprintf(File,"COPYRIGHT: (C)_Stephen_Coy,_1996.\n");
  fprintf(File,"WINDOW: 190 160\n");
  fprintf(File,"TEXTZONE: 85 43 Scaling_Factor 1.0 Scaling_factor_for_spheres\n");
  fprintf(File,"RADIO: 14  67 Cylinder 0 Generate_cylinders 1\n");
  fprintf(File,"RADIO: 14  87 Sphere 0 Generate_spheres 1\n");
  fprintf(File,"RADIO: 14 107 Cube 1 Generate_cubes 1\n");
  fprintf(File,"END:\n");

  fclose(File);
  return 1;
}

// ---------------------------------------------------------------------------
// -- OBJECT GENERATOR -------------------------------------------------------
// ---------------------------------------------------------------------------
void object(double x,double y,double z,double rad,int level) {
  static int Nb=0;
  double  offset;
  int i,j,k;

  make_vector(VS,rad,rad,rad);
  make_vector(VT,x*S,y*S,z*S);
  make_vector(VR,0,0,0);
  make_vector(VP,0,0,0);
  sprintf(Buffer,"MNG1%04d",Nb);
  write_object(File,Nb,O,VP,VS,VR,VT,7,"Default",0,0,Buffer);

  Nb++;
  level++;
  if (level>max_level) return;

  offset = rad * 2.0;
  rad /= 3.0;

  for (i=-1;i<2;i++) {
    for (j=-1;j<2;j++) {
      for (k=-1;k<2;k++) {
        if (i || j || k) {
          object(x+i*offset,y+j*offset,z+k*offset,rad,level);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// -- MAIN PART OF THE PROGRAM -----------------------------------------------
// ---------------------------------------------------------------------------
void main(char argc,char *argv[]) {
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

  S=atof(argv[1]);
  O=atoi(argv[2]);

  File=fopen("SPONGE.INC","w+t");
  object(0.0,0.0,0.0,1.0,0);
  fclose(File);
}

