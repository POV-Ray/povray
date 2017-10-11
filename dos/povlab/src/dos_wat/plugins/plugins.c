#include <MATH.H>
#include <FLOAT.H>
#include <STDIO.H>
#include "PLUGINS.H"

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
