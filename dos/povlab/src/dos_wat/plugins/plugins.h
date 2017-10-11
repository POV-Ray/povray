#ifndef PLUGINS_H_INCLUDED

#include <STDIO.H>

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

#define PI 3.14159265358979323846264338327950288419716939937511
#define M_PI PI

void make_vector(VECTOR V,float X,float Y,float Z);

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
                 char *Name);      // String for object's name

#define PLUGINS_H_INCLUDED
#endif

