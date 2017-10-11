// ChemLab 1.0
// PDB to POVLAB conversion utility
// By Denis Olivier 98
// dolivier@povlab.org
// All right reserved.

#include <STDIO.H>
#include <STRING.H>
#include <IO.H>
#include <FLOAT.H>
#include <MATH.H>
#include <STDARG.H>
#include <STDLIB.H>
#include "PDBLAB.H"

typedef float VECTOR[3];

#define CYLINDRE     0
#define SPHERE		 1
#define CUBE		 2
#define CONE		 3
#define TORE         4
#define TUBE         5
#define PLANX		 6
#define PLANY		 7
#define PLANZ		 8
#define ANNEAU      14
#define DISQUE      15
#define PRISME      16
#define QTUBE       17
#define CONET       18
#define PYRAMIDE    19
#define DSPHERE     35
#define QTORE       36
#define BLOB        37
#define HFIELD      38

#define PAS_CSG 0
#define OK_CSG  1
#define UNION   2
#define INTERSE 3
#define DIFFERE 4
#define MERGE   5
#define INVERSE 6

#define _X 0
#define _Y 1
#define _Z 2

char Argu[65][256];

char Element[18][7][30]={
"Hydr","H","1.20","0.8 ", "0.8 ","0.8 ","White_Plastic",
"Lith","Li","1.2 ","0.7 ", "0.7 ","0.7 ","Red_Plastic",
"Boro","B","0.8 ","0.9 ", "0.4 ","0.0 ","Maroon_Plastic",
"Carb","C","1.70","0.3 ", "0.3 ","0.3 ","Black_Plastic",
"Nitr","N","1.55","0.2 ", "0.2 ","0.8 ","Blue_Plastic",
"Oxyg","O","1.52","0.8 ", "0.2 ","0.2 ","Red_Plastic",
"Fluo","F","1.47","0.7 ", "0.85","0.45","Green_Plastic",
"Sodi","Na","1.6 ","0.6 ", "0.6 ","0.6 ","Brown_Plastic",
"Magn","Mg","1.4 ","0.6 ", "0.6 ","0.7 ","LighBlue_Plastic",
"Alum","Al","1.3 ","0.0 ", "0.74","0.65","Magenta_Plastic",
"Sili","Si","2.10","0.7 ", "0.7 ","0.3 ","Navy_Plastic",
"Phos","P","1.80","0.1 ", "0.7 ","0.3 ","Orange_Plastic",
"Sulf","S","1.80","0.95", "0.9 ","0.2 ","Yellow_Plastic",
"Chlo","Cl","1.75","0.15", "0.5 ","0.1 ","Orchid_Plastic",
"Pota","K","2.0 ","0.5 ", "0.5 ","0.5 ","Pink_Plastic",
"Calc","Ca","1.7 ","0.8 ", "0.8 ","0.7 ","Skin_Plastic",
"Brom","Br","1.85","0.5 ", "0.08","0.12","Tan_Plastic",
"Iodi","I","1.98","0.5 ", "0.1 ","0.5 ","Violet_Plastic"
};

typedef struct {
  float Long;
  float Angle1,Angle2,Angle3;
  float X,Y,Z;
} CYL;

// ---------------------------------------------------------------------------
// -- MAKE A VECTOR ----------------------------------------------------------
// ---------------------------------------------------------------------------
void make_vector(VECTOR V,float X,float Y,float Z) {
  V[_X]=X;
  V[_Y]=Y;
  V[_Z]=Z;
}

// ---------------------------------------------------------------------------
// -- VOID TO OUTPUT OBJECT IN A .INC FILE -----------------------------------
// ---------------------------------------------------------------------------
int write_object(FILE *File,       // Handle to file
                 int Nb,           // Object number Nb
                 int Type,         // Object type Type
                 VECTOR P,         // Aux vector (for some objects)
                 VECTOR S,         // Vector scale <SX,SY,SZ>
                 VECTOR R,         // Vector rotate <RX,RY,RZ>
                 VECTOR T,         // Vector translate <TX,TY,TZ>
                 int C,            // Color of the object (in modeller)
                 char *Texture,    // String for texture's name
                 int Selection,    // Bool if object is selected
                 int Hide,         // Bool if object is hidden
                 char *Name) {       // String for object's name
  fprintf(File,"\n");
  fprintf(File,"Object %05d: %d\n",Nb,Type);
  fprintf(File,"Object %05d: %.4f %.4f %.4f\n",Nb,P[_X],P[_Y],P[_Z]);
  fprintf(File,"Object %05d: %.4f %.4f %.4f\n",Nb,S[_X],S[_Y],S[_Z]);
  fprintf(File,"Object %05d: %.4f %.4f %.4f\n",Nb,R[_X],R[_Y],R[_Z]);
  fprintf(File,"Object %05d: %.4f %.4f %.4f\n",Nb,T[_X],T[_Y],T[_Z]);
  fprintf(File,"Object %05d: %d\n",Nb,C);
  fprintf(File,"Object %05d: %s\n",Nb,Texture);
  fprintf(File,"Object %05d: %d\n",Nb,Selection);
  fprintf(File,"Object %05d: %d\n",Nb,Hide);
  fprintf(File,"Object %05d: %s\n",Nb,Name);
  return 1;
}

/* ----------------------------------------------------------------------- */
/* -- RETOURNE LA NIEME POSITION DE CHAINE2 DANS CHAINE1 ----------------- */
/* ----------------------------------------------------------------------- */
int strinstr(int decalage,char *ch1,char *ch2) {
  int i=0,j,k=0;

  if (decalage<0) decalage=0;
  if (strlen(ch2)<=0) return -2;

  for (i=decalage;ch1[i]!=NULL;i++) {
    k=0;
    j=i;
    while (ch1[i]==ch2[k]) {
      i++;
      k++;
      if (k==strlen(ch2)) return j;
    }
  }

  return -1;
}

/* ------------------------------------------------------------------------- */
/* -- SCAN A LINE OF TEXT AND CUT IT WITH SEPARATEUR ----------------------- */
/* ------------------------------------------------------------------------- */
int analyse_ligne(char *TempChar,int Separateur) {
  int k,i;
  char Marque[2];
  char *Pointeur;
  char *Temp;
  
  if (!TempChar[0] && strlen(TempChar)==0) return 0;
  
  Temp=TempChar;

  Marque[0]=Separateur;
  Marque[1]='\0';

  for (k=0;k<65;k++) Argu[k][0]='\0';
  k=0;

  Pointeur=strtok(TempChar,Marque);

  if (Pointeur[0]=='\n') {
    k=0;
  } else {
    while (Pointeur && k<65) {
      strncpy(Argu[k],Pointeur,256-1);
      i=strinstr(0,Argu[k],"\n");
      if (i) Argu[k][i]=NULL;
      Pointeur=strtok(NULL,Marque);
      k++;
    }
  }
  
  TempChar=Temp;
  return k;
}

#define Efface_Transform(x) if ((x)!=NULL) free(x)

// ---------------------------------------------------------------------------
// -- COMPUTE THE SX,RX,TX OF A CYLINDER WITH JUST 2 POINTS ------------------
// ---------------------------------------------------------------------------
CYL calcul_cylinder(float X1,float Y1,float Z1,float X2,float Y2,float Z2) {
  CYL C;
  
  float  dx, dy, dz;     // vector Pt1 to Pt2
  float  dx2, dy2;
  // float  len=0;            // length of vector
  float  xzlen;
  float  dt1[4], dt2[4];     // local dot's
  float  a1, a2;
  float  alpha;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;

  // Align cones so as to be tangential to spheres.
  // by Robin Luiten (luiten@trantor.nmsd.oz.au)
  //
  // dx,dy,dz is vector from Pt1 to Pt2 */

  dx = X2 - X1;
  dy = Y2 - Y1;
  dz = Z2 - Z1;
  // len = sqrt(dx*dx + dy*dy + dz*dz);
  xzlen = sqrt(dx*dx + dz*dz);

  //
  // Transform dx,dy,dz till dy and dz are zero.
  // Retain necessary rotations for inverse transform.
  // The transform is made up of two rotations
  //
  // 1. rotate around y-axis to reduce dz to zero.
  //  angle a1.
  // 2. rotate around z-axis to reduce dy to zero.
  //  angle a2.
  // atan2(y/x) - result -PI to PI
  //

  a1 = atan2(dz,dx);      // 1

  //
  // Transform the vector by rotation a1 around y-axis.
  //

  dx2 = xzlen;
  dy2 = dy;

  a2 = atan2(dy2, dx2);       // 2
  
  // convert angle a1 and angle a2 to degrees */
  a1=   (a1 * 180)/PI;
  a2=90+(a2 * 180)/PI;

  // printf("Y angle: %4.6f, Z angle: %4.6f\n", a1, a2);
  //
  // NOTE: The rotation about the Y axis is the negative
  // of the calculated value because POV is setup as a
  // left hand system rather than a right hand one.
  //
  
  Mat=Create_Transform();
  vect_init(RO,0,0,-a2);
  rotation_objet(Mat,RO,'+');
  vect_init(RO,0,-a1,0);
  rotation_objet(Mat,RO,'+');
  mat_decode(Mat->matrix,SC,SH,RO,TR);
  Efface_Transform(Mat);

  C.Long=sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)+(Z1-Z2)*(Z1-Z2))/2;
  C.Angle1=RO[0];
  C.Angle2=RO[1];
  C.Angle3=RO[2];
  C.X=(X1+X2)/2;
  C.Y=(Y1+Y2)/2;
  C.Z=(Z1+Z2)/2;
  return C;
}

/* ------------------------------------------------------------------------- */
/* -- VOID MAIN VOID ------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
void main(int argc,char *argv[]) {
  char FichierIn[256];
  char FichierOut[256];
  char Line[2048];
  char Buffer[2048];
  FILE *FileIn;
  FILE *FileOut;
  int Nb=0;
  VECTOR VP,VS,VR,VT,VSAVE;
  int i;
  int Stick=0;
  int TER=0;
  
  printf("\n");
  printf("PDBLAB v.1.0, (C) Copyright ChromaGraphics, 1998.\n");
  printf("PDB Molecule format to POVLAB SCN format converter.\n");
  printf("All rights reserved, (R) Denis Olivier - %s.\n",__DATE__);
  printf("\n");
  
  if (argc<3) {
    printf("Usage : pdblab molecul.pdb -s\n\n");
    printf("This will create automatically a molecul.scn POVLAB file.\n");
    printf("More PDB files at : http://www.pdb.bnl.gov\n");
    exit(0);
  }
  
  if (!strcmp(strlwr(argv[2]),"-s")) {
    Stick=1;
    printf("Stick mode.\n");
  }
  
  strcpy(FichierIn,argv[1]);
  strcpy(FichierOut,strlwr(argv[1]));
  
  FichierOut[strinstr(0,FichierOut,".pdb")]='\0';
  strcat(FichierOut,".scn");
    
  if ((FileIn=fopen(FichierIn,"rt"))==NULL) {
    printf("\nCan't open %s file. Aborting.\n",FichierIn);
    exit(0);
  }
  if ((FileOut=fopen(FichierOut,"wt"))==NULL) {
    printf("\nCan't open %s file. Aborting.\n",FichierOut);
    exit(0);
  }
  
  while(!feof(FileIn)) {
    fgets(Line,2047,FileIn);
    analyse_ligne(Line,32);
    if (!strcmp(Argu[0],"TER")) TER=1;
    if (!strcmp(Argu[0],"ATOM") || strcmp(Argu[0],"HETATM")==0) {
      for (i=0;i<=17;i++) {
        if (!strncmp(Argu[1],Element[i][1],strlen(Element[i][1]))) {
          if (Stick && Nb!=0) {
            if (TER==0) {
              CYL C;
              C=calcul_cylinder(atof(Argu[5]),atof(Argu[6]),atof(Argu[7]),VSAVE[_X],VSAVE[_Y],VSAVE[_Z]);
              make_vector(VP,0,0,0);
              make_vector(VS,0.2,C.Long,0.2);
              make_vector(VR,C.Angle1,C.Angle2,C.Angle3);
              make_vector(VT,C.X,C.Y,C.Z);
              sprintf(Buffer,"%s%04d","Conn",(int) Nb);
              write_object(FileOut,Nb,CYLINDRE,VP,VS,VR,VT,7,Element[i][6],0,0,Buffer);
              printf("\rConnection #%6d %s",Nb,Buffer);
            } else {
              TER=0;
            }
          }
          make_vector(VP,0,0,0);
          if (Stick) {
            make_vector(VS,0.2,0.2,0.2);
          } else {
            make_vector(VS,atof(Element[i][2])*1.123,atof(Element[i][2])*1.123,atof(Element[i][2])*1.123);
          }
          make_vector(VR,0,0,0);
          make_vector(VT,atof(Argu[5]),atof(Argu[6]),atof(Argu[7]));
          VSAVE[_X]=atof(Argu[5]);
          VSAVE[_Y]=atof(Argu[6]);
          VSAVE[_Z]=atof(Argu[7]);
          sprintf(Buffer,"%s%04d",Element[i][0],(int) Nb);
          write_object(FileOut,Nb,SPHERE,VP,VS,VR,VT,7,Element[i][6],0,0,Buffer);
          printf("\rAtom #%6d %s                      ",Nb,Buffer);
          break;
        }
      }
      Nb++;
    }
  }
  
  fcloseall();
  printf("\n");
}
  
