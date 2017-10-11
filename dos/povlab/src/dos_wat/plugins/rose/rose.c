#define VERSION "v2.0"

static char what[] = "Dos_version_by_Alex_Howansky";

/*
 *  rose.c
 *
 *  By Alex Howansky
 *     261 Boght Road
 *     Watervliet, NY 12189-1121
 *
 *  Please feel free to distribute and make changes to this code, as long
 *   as you include this header and maintain the version list.
 *
 *  Version 1.0 11/21/91 Alex Howansky
 *    initial version for DKBTrace v2.12
 *
 *  Version 1.1 8/23/93 Alex Howansky
 *    modified for use with POV-Ray v1
 *
 *  Version 1.2 2/7/94 Alex Howansky
 *    modified for use with POV-Ray v2
 */

#include <STDIO.H>
#include <MATH.H>
#include <STRING.H>
#include <FLOAT.H>

#include "PLUGINS.H"
#include "PLUGINS.C"

#define M_PI 3.14159265358979323846264338327950288419716939937511

int exit (int);

// ---------------------------------------------------------------------------
// -- DATA SCRIPT FOR POVLAB -------------------------------------------------
// ---------------------------------------------------------------------------
int out_data_interface(void) {
  FILE *File;

  // ------------- Don't forget to name the .PLG like the .EXE
  // ------------- file you're running |-) !
  // ------------- The command line parameters are returned in the
  // ------------- Order you generate the graphic datas.

  if (!(File=fopen("ROSE.PLG","wt"))) return 0;

  fprintf(File,"TITLE: ROSE_%s\n",VERSION);
  fprintf(File,"COPYRIGHT: %s.\n",what);
  fprintf(File,"WINDOW: 190 200\n");
  fprintf(File,"TEXTZONE: 85  40 Angle_begin 0 Nb_of_bubbles_in_the_box\n");
  fprintf(File,"TEXTZONE: 85  60 Angle_end 360 Box_lenght\n");
  fprintf(File,"TEXTZONE: 85  80 Angle_Step 3 Box_height\n");
  fprintf(File,"TEXTZONE: 85 100 Scale_factor 10 Box_depth\n");
  fprintf(File,"TEXTZONE: 85 120 Multi_value 2 Box_depth\n");
  fprintf(File,"TEXTZONE: 85 140 Sphere_Radius 0.2 ..._for_bubbles\n");
  fprintf(File,"END:\n");

  fclose(File);
  return 1;
}

// ---------------------------------------------------------------------------
// -- MAIN PROGRAM -----------------------------------------------------------
// ---------------------------------------------------------------------------
void main (int argc, char *argv[]) {
   FILE *File;
   int ch,i=0;
   float theta, r;
   float begin = 0.0 * M_PI, end = 2.0 * M_PI, step = M_PI / 60;
   float n = 2.0, radius = 0.5, scale = 10.0,  T;
   char Buffer[20];
   VECTOR VP,VS,VR,VT;

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

  begin = atof(argv[1]) * M_PI / 180;
  end = atof(argv[2]) * M_PI / 180;
  step = atof(argv[3]) * M_PI / 180;
  scale = atof(argv[4]);
  n = atof(argv[5]);
  radius = atof(argv[6]);

  File=fopen("ROSE.INC","w+t");

  theta=begin;

  while (theta<end) {
    r = scale * sin(theta * n);
    //if ((int)r == 0) continue;
    T=(scale == 0.0 ? radius : radius * fabs(r));
    make_vector(VS,T,T,T);
    make_vector(VT,r*cos(theta),r*sin(theta),0);
    make_vector(VR,0,0,0);
    make_vector(VP,0,0,0);
    sprintf(Buffer,"ROSE%04d",i);
    write_object(File,i++,SPHERE,VP,VS,VR,VT,7,"Default",0,0,Buffer);
    theta += step;
  }

  fclose(File);
 }
