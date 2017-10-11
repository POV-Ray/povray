/* ---------------------------------------------------------------------------
*  BINFILE.C
*
*
*
*  from POVLAB 3D Modeller
*  Copyright 1994-1999 POVLAB Authors.
*  ---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POVLAB and to port the software to platforms other
*  than those supported by the POVLAB authors. There are strict rules under
*  which you are permitted to use this file. The rules are in the file
*  named LEGAL.TXT which should be distributed with this file.
*  If LEGAL.TXT is not available or for more info please contact the POVLAB
*  primary author by leaving a message on http://www.povlab.org
*  The latest and official version of POVLAB may be found at this site.
*
*  POVLAB was originally written by Denis Olivier.
*
*  ---------------------------------------------------------------------------*/
#include <SYS\STAT.H>
#include <IO.H>
#include <FCNTL.H>
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <STRING.H>
#include <TIME.H>
#include <STDIO.H>
#include <DOS.H>
#include <DIRECT.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"
#if 1==0
#define BIN_FILE_VERSION       "SCN00"

#define BACK_TEXT_COLOR        0x0000
#define INTERFACE_COLOR        0x0001

#define VIEWPORT_COLOR         0x0100
#define VIEWPORT_CURRENT       0x0101
#define VIEWPORT_X4            0x0102
#define VIEWPORT_SCALE_XY      0x0103
#define VIEWPORT_SHIFT_X       0x0104
#define VIEWPORT_SHIFT_Y       0x0105

#define CAMERA_COLOR           0x0200
#define CAMERA_CURRENT         0x0201
#define CAMERA_OEIL            0x0202
#define CAMERA_CIBLE           0x0203
#define CAMERA_OUVERTURE       0x0204
#define CAMERA_PROCHAMP        0x0205
#define CAMERA_ONOFF           0x0206
#define CAMERA_F_BLUR          0x0207
#define CAMERA_SAMPLES         0x0208
#define CAMERA_APERTURE        0x0209
#define CAMERA_VARIANCE        0x0210
#define CAMERA_CONFIDENCE      0x0211
#define CAMERA_DISPLAY         0x0212

#define AXIS_COLOR             0x0300
#define AXIS_DISPLAY           0x0301

#define GRID_COLOR             0x0400
#define GRID_DISPLAY           0x0401

#define SELECTION_COLOR        0x0500
#define SELECTION_USE          0x0501

#define TEXTURE_CURRENT        0x0600

#define RENDER_BACKGROUND      0x0700

#define FOG_RGB                0x0800
#define FOG_OK                 0x0801
#define FOG_DISTANCE           0x0802
#define FOG_TYPE               0x0803
#define FOG_OFFSET             0x0804
#define FOG_ALT                0x0805
#define FOG_TURBD              0x0806
#define FOG_TURBULENCE         0x0807

#define ATMOS_ONOFF            0x0900
#define ATMOS_TYPE             0x0901
#define ATMOS_DIST             0x0902
#define ATMOS_SCATT            0x0903
#define ATMOS_ECCENT           0x0904
#define ATMOS_SAMPLES          0x0905
#define ATMOS_JITTER           0x0906
#define ATMOS_AA_T             0x0907
#define ATMOS_AA_L             0x0908
#define ATMOS_RGB              0x0909

#define GLOBAL_ADC             0x1000
#define GLOBAL_A_GAMMA         0x1001
#define GLOBAL_MAXTRACE        0x1002
#define GLOBAL_MAXINTER        0x1003
#define GLOBAL_NBWAVE          0x1004
#define GLOBAL_IRID            0x1005

#define OMNI_POINT             0x1100
#define OMNI_RVB               0x1101
#define OMNI_TAILLE            0x1102
#define OMNI_ONOFF             0x1103
#define OMNI_OMBRE             0x1104
#define OMNI_F_DIST            0x1105
#define OMNI_F_POWER           0x1106
#define OMNI_FADE              0x1107
#define OMNI_ATMOS             0x1108
#define OMNI_ATMOS_ATT         0x1109
#define OMNI_CACHE             0x1110
#define OMNI_DISPLAY           0x1111

#define OBJET_TYPE             0x1200
#define OBJET_POINT            0x1201
#define OBJET_SCALE            0x1202
#define OBJET_ROTATE           0x1203
#define OBJET_TRANSLATE        0x1204
#define OBJET_COULEUR          0x1205
#define OBJET_MATIERE          0x1206
#define OBJET_SELECTION        0x1207
#define OBJET_CACHE            0x1208
#define OBJET_NOM              0x1209
#define OBJET_CHEMINRAW        0x1210
#define OBJET_POLY             0x1211
#define OBJET_RAPIDE           0x1212
#define OBJET_MSCALE           0x1213
#define OBJET_MROTATE          0x1214
#define OBJET_MTRANSLATE       0x1215
#define OBJET_SMOOTH           0x1216
#define OBJET_FREEZE           0x1217
#define OBJET_IGNORE           0x1218
#define OBJET_OPERATOR         0x1219
#define OBJET_CSG              0x1220
#define OBJET_OMBRE            0x1221
#define OBJET_SCALEOM          0x1222
#define OBJET_IOR              0x1223
#define OBJET_REFRACTION       0x1224
#define OBJET_REFLEXION        0x1225
#define OBJET_DIFFUSION        0x1226
#define OBJET_AMBIANCE         0x1227
#define OBJET_CRAND            0x1228
#define OBJET_PHONG            0x1229
#define OBJET_PSIZE            0x1230
#define OBJET_CAUSTICS         0x1231
#define OBJET_FADE_D           0x1232
#define OBJET_FADE_P           0x1233
#define OBJET_ROUGH            0x1234
#define OBJET_BRILLI           0x1235
#define OBJET_SPECULAR         0x1236
#define OBJET_BITTEXTURE       0x1237
#define OBJET_HALO             0x1238
#define OBJET_WLEVEL           0x1239
#define OBJET_MAP_NAME         0x1240
#define OBJET_MAP_ON           0x1241
#define OBJET_MAP_TYPE         0x1242
#define OBJET_MAP_ONCE         0x1243
#define OBJET_MAP_INTERPOLATE  0x1244
#define OBJET_MAP_AMOUNT       0x1245
#define OBJET_MAP_ALPHA        0x1246
#define OBJET_MAP_FILTER       0x1247
#define OBJET_MAP_COLOR        0x1248
#define OBJET_LOOKSLIKE_NB     0x1249
#define OBJET_LOOKSLIKE_LIGHT  0x1250
#define OBJET_NOMBRE           0x1251


#define SPLINE_TYPE            0x1300
#define SPLINE_DEGREE          0x1301
#define SPLINE_SUBDI           0x1302
#define SPLINE_NOMBRE          0x1303
  
#define TRIANGLE_NOMBRE        0x1403

#define SPOT_OEIL              0x1500
#define SPOT_CIBLE             0x1501
#define SPOT_RVB               0x1502
#define SPOT_TAILLE            0x1503
#define SPOT_ANGLE1            0x1504
#define SPOT_ANGLE2            0x1505
#define SPOT_ONOFF             0x1506
#define SPOT_OMBRE             0x1507
#define SPOT_F_DIST            0x1508
#define SPOT_F_POWER           0x1509
#define SPOT_FADE              0x1510
#define SPOT_ATMOS             0x1511
#define SPOT_ATMOS_ATT         0x1512
#define SPOT_CONE              0x1513
#define SPOT_DISPLAY           0x1514

#define CYLLIGHT_OEIL          0x1600
#define CYLLIGHT_CIBLE         0x1601
#define CYLLIGHT_RVB           0x1602
#define CYLLIGHT_TAILLE        0x1603
#define CYLLIGHT_ANGLE1        0x1604
#define CYLLIGHT_ANGLE2        0x1605
#define CYLLIGHT_ONOFF         0x1606
#define CYLLIGHT_OMBRE         0x1607
#define CYLLIGHT_F_DIST        0x1608
#define CYLLIGHT_F_POWER       0x1609
#define CYLLIGHT_FADE          0x1610
#define CYLLIGHT_ATMOS         0x1611
#define CYLLIGHT_ATMOS_ATT     0x1612
#define CYLLIGHT_CONE          0x1613
#define CYLLIGHT_DISPLAY       0x1614

#define AMPOULE_POINT          0x1700
#define AMPOULE_RVB            0x1701
#define AMPOULE_TAILLE         0x1702
#define AMPOULE_DOUCEUR        0x1703
#define AMPOULE_RAYON          0x1704
#define AMPOULE_ONOFF          0x1705
#define AMPOULE_OMBRE          0x1706
#define AMPOULE_F_DIST         0x1707
#define AMPOULE_F_POWER        0x1708
#define AMPOULE_FADE           0x1709
#define AMPOULE_ATMOS          0x1710
#define AMPOULE_ATMOS_ATT      0x1711
#define AMPOULE_DISPLAY        0x1712

#define END_INIT_SECTION       0x1800

// -------------------------------------------------------------------------
// -- FONCTION POUR LIRE LES DONNEES BINAIRES ------------------------------
// -------------------------------------------------------------------------
byte read_byte(FILE *F) { byte data; data=fgetc(F); return data; }
word read_word(FILE *F) { word data; fread(&data,2,1,F); return data; }
DBL  read_dbl(FILE *F) { DBL data; fread(&data,sizeof(DBL),1,F); return data; }
int  read_int(FILE *F) { int data; fread(&data,sizeof(int),1,F); return data; }

// -------------------------------------------------------------------------
// -- FONCTION POUR LIRE UN VECTEUR EN BINAIRE -----------------------------
// -------------------------------------------------------------------------
void read_vecteur(FILE *F,Vecteur V) {
  V[_X]=read_dbl(F);
  V[_Y]=read_dbl(F);
  V[_Z]=read_dbl(F);
}

// -------------------------------------------------------------------------
// -- FONCTION POUR LIRE UNE CHAINE EN BINAIRE -----------------------------
// -------------------------------------------------------------------------
void read_string(FILE *F,char *String) {
  register int i=0;

  while ((String[i]=read_byte(F))) { i++; }
}

// -------------------------------------------------------------------------
// -- FONCTION POUR ECRIRE UN CHUNK ----------------------------------------
// -------------------------------------------------------------------------
void write_chunk(FILE *F,int C) {
  fwrite(&C,sizeof(int),1,F);
}

// -------------------------------------------------------------------------
// -- SAUVE LES TRIANGLES DANS LE FICHIER .SCN BINAIRE ---------------------
// -------------------------------------------------------------------------
void sauve_triangle_bin(FILE *Fichier,int i) {
  int j,NumP;
  
  NumP=Objet[i]->Poly;
  j=Poly[NumP]->Nombre;

  if (Poly[NumP]->Buffer) return;
  Poly[NumP]->Buffer=1;
  write_chunk(Fichier,TRIANGLE_NOMBRE);
    fwrite(&j,sizeof(int),1,Fichier);
    fwrite(&Poly[NumP]->T1,sizeof(DBL)*3*j,1,Fichier);
    fwrite(&Poly[NumP]->T2,sizeof(DBL)*3*j,1,Fichier);
    fwrite(&Poly[NumP]->T3,sizeof(DBL)*3*j,1,Fichier);
}

// -------------------------------------------------------------------------
// -- SAUVE LES POINTS POUR UNE SPLINE .SCN BINAIRE ------------------------
// -------------------------------------------------------------------------
void sauve_spline_bin(FILE *Fichier,int N) {
  int j,NSP;

  NSP=Objet[N]->Poly;
  j=Spline[NSP]->Nombre;

  if (Spline[NSP]->Buffer) return;
  Spline[NSP]->Buffer=1;

  write_chunk(Fichier,SPLINE_TYPE);
    fwrite(&Spline[NSP]->Type,sizeof(int),1,Fichier);
  write_chunk(Fichier,SPLINE_DEGREE);
    fwrite(&Spline[NSP]->Degree,sizeof(int),1,Fichier);
  write_chunk(Fichier,SPLINE_SUBDI);
    fwrite(&Spline[NSP]->Subdi,sizeof(int),1,Fichier);

  write_chunk(Fichier,SPLINE_NOMBRE);
    fwrite(&j,sizeof(int),1,Fichier);
    fwrite(&Spline[NSP]->dots,sizeof(DBL)*4*j,1,Fichier);
}

// -------------------------------------------------------------------------
// -- SAUVE UN FICHIER DE DESCRIPTION DE SCENE EN BINAIRE ------------------
// -------------------------------------------------------------------------
byte sauve_fichier_bin(byte Ecrase,byte Partiel) {
  FILE *Fichier;
  char *FBuffer;
  int i,j;
  int Nb_Bit_Texture=MAX_BIT_TEXTURE;
  char TmpFichierSCN[MAXFILE+MAXEXT];
  char TmpCheminSCN[MAXPATH];
  char Buffer[MAXPATH];

  if (!Partiel) if (!test_si_nouveau_scn()) return 0;

  if (Partiel) {
    if (!Selection) { f_erreur("None selected !"); return 0; }
    strcpy(TmpFichierSCN,FichierSCN);
    strcpy(TmpCheminSCN,CheminSCN);
    if (!renomme_scene("SAVE AS ...")) return 0;
  }

  if (CheminSCN[0]==NULLC) {
    strcpy(CheminSCN,NewLecteur);
    strcat(CheminSCN,NewChemin);
    strcat(CheminSCN,"\\SCENE\\");
  }

  strcpy(Buffer,CheminSCN);
  strcat(Buffer,FichierSCN);

  if (!Ecrase && test_fichier(Buffer)) {
    strcpy(StrBoite[0],"Save scene");
    strcpy(StrBoite[1],"Overwrite scene on disk");
    sprintf(StrBoite[2],"with the current scene %s ?",FichierSCN);
    if (g_boite_ONA(CentX,CentY,2,CENTRE,1)!=0) return 0;
  }

  Fichier=fopen("TEST.BIN","wb");
  FBuffer=malloc(0xFFFF);
  setvbuf(Fichier,FBuffer,_IOFBF,0xFFFF);

  fwrite(BIN_FILE_VERSION,5,1,Fichier);

  // -------------------------------- Cameras
  write_chunk(Fichier,CAMERA_COLOR);
    fwrite(&CCAMERA,sizeof(int),1,Fichier);
  write_chunk(Fichier,CAMERA_CURRENT);
    fwrite(&NumCam,sizeof(int),1,Fichier);
  write_chunk(Fichier,CAMERA_DISPLAY);
    fwrite(&Camera[0].Cache,sizeof(int),1,Fichier);
  for (i=1;i<=NbCamera;i++) {
    write_chunk(Fichier,CAMERA_OEIL);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].OX,sizeof(DBL),1,Fichier);
      fwrite(&Camera[i].OY,sizeof(DBL),1,Fichier);
      fwrite(&Camera[i].OZ,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_CIBLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].CX,sizeof(DBL),1,Fichier);
      fwrite(&Camera[i].CY,sizeof(DBL),1,Fichier);
      fwrite(&Camera[i].CZ,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_OUVERTURE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].Ouverture,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_PROCHAMP);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].ProChamp,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_ONOFF);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].OnOff,sizeof(int),1,Fichier);
    write_chunk(Fichier,CAMERA_F_BLUR);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].F_Blur,sizeof(int),1,Fichier);
    write_chunk(Fichier,CAMERA_SAMPLES);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].Samples,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_APERTURE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].Aperture,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_VARIANCE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].Variance,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CAMERA_CONFIDENCE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Camera[i].Confidence,sizeof(DBL),1,Fichier);
  }

  // -------------------------------- Text zone color
  write_chunk(Fichier,BACK_TEXT_COLOR);
  fwrite(&ZFOND,sizeof(int),1,Fichier);

  // -------------------------------- Selection color
  write_chunk(Fichier,SELECTION_COLOR);
  fwrite(&CSELECTION,sizeof(int),1,Fichier);

  // -------------------------------- Selection use
  write_chunk(Fichier,SELECTION_USE);
  fwrite(&Cc[0].Croix,sizeof(int),1,Fichier);

  // -------------------------------- Interface color
  write_chunk(Fichier,INTERFACE_COLOR);
  fwrite(&FOND,sizeof(int),1,Fichier);

  // -------------------------------- Axis color
  write_chunk(Fichier,AXIS_COLOR);
  fwrite(&CAXE,sizeof(int),1,Fichier);

  // -------------------------------- Grid color
  write_chunk(Fichier,GRID_COLOR);
  fwrite(&CGRILLE,sizeof(int),1,Fichier);

  // -------------------------------- Viewports
  write_chunk(Fichier,GRID_DISPLAY);
  fwrite(&OptGrille,sizeof(int),1,Fichier);
  write_chunk(Fichier,VIEWPORT_COLOR);
  fwrite(&FFOND,sizeof(int),1,Fichier);
  write_chunk(Fichier,VIEWPORT_CURRENT);
  fwrite(&NF,sizeof(int),1,Fichier);
  write_chunk(Fichier,VIEWPORT_X4);
  fwrite(&Fx4,sizeof(int),1,Fichier);
  write_chunk(Fichier,AXIS_DISPLAY);
  fwrite(&OptAxe,sizeof(int),1,Fichier);
  for (i=0;i<=2;i++) {
    write_chunk(Fichier,VIEWPORT_SCALE_XY);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Vid[i].Echelle,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,VIEWPORT_SHIFT_X);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Vid[i].Depla.X,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,VIEWPORT_SHIFT_Y);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Vid[i].Depla.Y,sizeof(DBL),1,Fichier);
  }

  // -------------------------------- Current texture
  write_chunk(Fichier,TEXTURE_CURRENT);
  fwrite(MatiereCourante,strlen(MatiereCourante),1,Fichier);

  // -------------------------------- Render background color
  write_chunk(Fichier,RENDER_BACKGROUND);
  fwrite(&Fond_RVB,sizeof(int)*3,1,Fichier);

  // -------------------------------- Fog
  write_chunk(Fichier,FOG_RGB);
    fwrite(&Fog_RGB,sizeof(int)*3,1,Fichier);
  write_chunk(Fichier,FOG_OK);
    fwrite(&Fog_Ok,sizeof(int),1,Fichier);
  write_chunk(Fichier,FOG_DISTANCE);
    fwrite(&Fog_Distance,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,FOG_TYPE);
    fwrite(&Fog_Type,sizeof(int),1,Fichier);
  write_chunk(Fichier,FOG_OFFSET);
    fwrite(&Fog_Offset,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,FOG_ALT);
    fwrite(&Fog_Alt,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,FOG_TURBD);
    fwrite(&Fog_TurbD,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,FOG_TURBULENCE);
    fwrite(&Fog_Turbulence,sizeof(DBL),1,Fichier);

  // -------------------------------- Atmosphere
  write_chunk(Fichier,ATMOS_ONOFF);
    fwrite(&Atmos_OnOff,sizeof(int),1,Fichier);
  write_chunk(Fichier,ATMOS_TYPE);
    fwrite(&Atmos_Type,sizeof(int),1,Fichier);
  write_chunk(Fichier,ATMOS_DIST);
    fwrite(&Atmos_Dist,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,ATMOS_SCATT);
   fwrite(&Atmos_Scatt,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,ATMOS_ECCENT);
    fwrite(&Atmos_Eccent,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,ATMOS_SAMPLES);
    fwrite(&Atmos_Samples,sizeof(int),1,Fichier);
  write_chunk(Fichier,ATMOS_JITTER);
    fwrite(&Atmos_Jitter,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,ATMOS_AA_T);
    fwrite(&Atmos_aa_t,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,ATMOS_AA_L);
    fwrite(&Atmos_aa_l,sizeof(int),1,Fichier);
  write_chunk(Fichier,ATMOS_RGB);
    fwrite(&Atmos_RGB,sizeof(int)*3,1,Fichier);

  // -------------------------------- Render
  write_chunk(Fichier,GLOBAL_ADC);
    fwrite(&Global_ADC,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,GLOBAL_A_GAMMA);
    fwrite(&Global_A_Gamma,sizeof(DBL),1,Fichier);
  write_chunk(Fichier,GLOBAL_MAXTRACE);
    fwrite(&Global_MaxTrace,sizeof(int),1,Fichier);
  write_chunk(Fichier,GLOBAL_MAXINTER);
    fwrite(&Global_MaxInter,sizeof(int),1,Fichier);
  write_chunk(Fichier,GLOBAL_NBWAVE);
    fwrite(&Global_NbWave,sizeof(int),1,Fichier);
  write_chunk(Fichier,GLOBAL_IRID);
    fwrite(&Global_Irid,sizeof(DBL)*3,1,Fichier);
  
  // -------------------------------- Omni
  write_chunk(Fichier,OMNI_DISPLAY);
    fwrite(&Omni[0].Cache,sizeof(int),1,Fichier);
  for (i=1;i<=NbOmni;i++) {
    write_chunk(Fichier,OMNI_POINT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].Point,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OMNI_RVB);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].RVB[_R],sizeof(int)*3,1,Fichier);
    write_chunk(Fichier,OMNI_TAILLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].Taille,sizeof(int),1,Fichier);
    write_chunk(Fichier,OMNI_ONOFF);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].OnOff,sizeof(int),1,Fichier);
    write_chunk(Fichier,OMNI_OMBRE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].Ombre,sizeof(int),1,Fichier);
    write_chunk(Fichier,OMNI_F_DIST);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].F_Dist,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,OMNI_F_POWER);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].F_Power,sizeof(int),1,Fichier);
    write_chunk(Fichier,OMNI_FADE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].Fade,sizeof(int),1,Fichier);
    write_chunk(Fichier,OMNI_ATMOS);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].Atmos,sizeof(int),1,Fichier);
    write_chunk(Fichier,OMNI_ATMOS_ATT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Omni[i].Atmos_Att,sizeof(int),1,Fichier);
  }

  // -------------------------------- Spot
  write_chunk(Fichier,SPOT_DISPLAY);
    fwrite(&Spot[0].Cache,sizeof(int),1,Fichier);
  for (i=1;i<=NbSpot;i++) {
    write_chunk(Fichier,SPOT_OEIL);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].OX,sizeof(DBL),1,Fichier);
      fwrite(&Spot[i].OY,sizeof(DBL),1,Fichier);
      fwrite(&Spot[i].OZ,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,SPOT_CIBLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].CX,sizeof(DBL),1,Fichier);
      fwrite(&Spot[i].CY,sizeof(DBL),1,Fichier);
      fwrite(&Spot[i].CZ,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,SPOT_RVB);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].RVB,sizeof(int)*3,1,Fichier);
    write_chunk(Fichier,SPOT_TAILLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Taille,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_ANGLE1);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Angle1,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,SPOT_ANGLE2);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Angle2,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,SPOT_ONOFF);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].OnOff,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_OMBRE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Ombre,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_F_DIST);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].F_Dist,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,SPOT_F_POWER);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].F_Power,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_FADE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Fade,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_ATMOS);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Atmos,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_ATMOS_ATT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Atmos_Att,sizeof(int),1,Fichier);
    write_chunk(Fichier,SPOT_CONE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Spot[i].Cone,sizeof(int),1,Fichier);
  }

  // -------------------------------- CylLight
  write_chunk(Fichier,CYLLIGHT_DISPLAY);
    fwrite(&CylLight[0].Cache,sizeof(int),1,Fichier);
  for (i=1;i<=NbCylLight;i++) {
    write_chunk(Fichier,CYLLIGHT_OEIL);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].OX,sizeof(DBL),1,Fichier);
      fwrite(&CylLight[i].OY,sizeof(DBL),1,Fichier);
      fwrite(&CylLight[i].OZ,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_CIBLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].CX,sizeof(DBL),1,Fichier);
      fwrite(&CylLight[i].CY,sizeof(DBL),1,Fichier);
      fwrite(&CylLight[i].CZ,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_RVB);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].RVB,sizeof(int)*3,1,Fichier);
    write_chunk(Fichier,CYLLIGHT_TAILLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Taille,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_ANGLE1);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Angle1,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_ANGLE2);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Angle2,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_ONOFF);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].OnOff,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_OMBRE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Ombre,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_F_DIST);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].F_Dist,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_F_POWER);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].F_Power,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_FADE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Fade,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_ATMOS);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Atmos,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_ATMOS_ATT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Atmos_Att,sizeof(int),1,Fichier);
    write_chunk(Fichier,CYLLIGHT_CONE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&CylLight[i].Cone,sizeof(int),1,Fichier);
  }

  // -------------------------------- Ampoule
  write_chunk(Fichier,AMPOULE_DISPLAY);
    fwrite(&Ampoule[0].Cache,sizeof(int),1,Fichier);
  for (i=1;i<=NbAmpoule;i++) {
    write_chunk(Fichier,AMPOULE_POINT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Point,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,AMPOULE_RVB);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].RVB,sizeof(int)*3,1,Fichier);
    write_chunk(Fichier,AMPOULE_TAILLE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Taille,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_DOUCEUR);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Douceur,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_RAYON);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Rayon,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,AMPOULE_ONOFF);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].OnOff,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_OMBRE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Ombre,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_F_DIST);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].F_Dist,sizeof(DBL),1,Fichier);
    write_chunk(Fichier,AMPOULE_F_POWER);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].F_Power,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_FADE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Fade,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_ATMOS);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Atmos,sizeof(int),1,Fichier);
    write_chunk(Fichier,AMPOULE_ATMOS_ATT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Ampoule[i].Atmos_Att,sizeof(int),1,Fichier);
  }

  write_chunk(Fichier,OBJET_NOMBRE);
  fwrite((Partiel ? &Selection:&NbObjet),sizeof(int),1,Fichier);

  write_chunk(Fichier,END_INIT_SECTION);

  reinit_buffer_poly();
  reinit_buffer_spline();
  GMouseOff();

  // -------------------------------- Objets
  for (i=1;i<=NbObjet;i++) {
    if (Partiel && (!Objet[i]->Selection || Objet[i]->Cache)) goto SAUTE_OBJET;
    write_chunk(Fichier,OBJET_TYPE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Type,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_POINT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->P,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_SCALE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->S,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_ROTATE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->R,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_TRANSLATE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->T,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_MSCALE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->MS,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_MROTATE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->MR,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_MTRANSLATE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->MT,sizeof(DBL)*3,1,Fichier);
    write_chunk(Fichier,OBJET_COULEUR);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Couleur,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_SELECTION);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Selection,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_CACHE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Cache,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_RAPIDE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Rapide,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_SMOOTH);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Smooth,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_FREEZE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Freeze,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_IGNORE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Ignore,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_OPERATOR);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Operator,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_CSG);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->CSG,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_OMBRE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Ombre,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_SCALEOM);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->ScaleOM,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_IOR);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Ior,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_REFRACTION);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Refraction,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_REFLEXION);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Reflexion,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_DIFFUSION);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Diffusion,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_AMBIANCE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Ambiance,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_CRAND);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Crand,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_PHONG);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Phong,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_PSIZE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->PSize,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_CAUSTICS);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Caustics,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_FADE_D);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Fade_D,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_FADE_P);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Fade_P,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_ROUGH);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Rough,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_BRILLI);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Brilli,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_SPECULAR);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Specular,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_LOOKSLIKE_NB);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->LooksLike.Nb,sizeof(int),1,Fichier);
    write_chunk(Fichier,OBJET_LOOKSLIKE_LIGHT);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->LooksLike.Light,sizeof(int),1,Fichier);

    write_chunk(Fichier,OBJET_MATIERE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Matiere,strlen(Objet[i]->Matiere),1,Fichier);
    write_chunk(Fichier,OBJET_NOM);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Nom,strlen(Objet[i]->Nom),1,Fichier);

    for (j=0;j<=1;j++) {
      write_chunk(Fichier,OBJET_MAP_NAME);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Name,sizeof(int),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_ON);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].On,sizeof(int),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_TYPE);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Type,sizeof(int),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_ONCE);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Once,sizeof(int),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_INTERPOLATE);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Interpolate,sizeof(int),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_AMOUNT);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Amount,sizeof(int),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_ALPHA);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Alpha,sizeof(DBL),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_FILTER);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Filter,sizeof(DBL),1,Fichier);
      write_chunk(Fichier,OBJET_MAP_COLOR);
        fwrite(&i,sizeof(int),1,Fichier);
        fwrite(&j,sizeof(int),1,Fichier);
        fwrite(&Objet[i]->Map[j].Color,sizeof(int),1,Fichier);
    }

    write_chunk(Fichier,OBJET_HALO);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Halo,sizeof(int),1,Fichier);

    write_chunk(Fichier,OBJET_WLEVEL);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->WLevel,sizeof(DBL),1,Fichier);

    write_chunk(Fichier,OBJET_CHEMINRAW);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->CheminRaw,strlen(Objet[i]->CheminRaw),1,Fichier);

    write_chunk(Fichier,OBJET_POLY);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Objet[i]->Poly,sizeof(int),1,Fichier);

    if (Objet[i]->Type==TRIANGLE && Objet[i]->CheminRaw[0]=='@') {
      sauve_triangle_bin(Fichier,i);
    }

    if (Objet[i]->Type==SPLINE) sauve_spline_bin(Fichier,i);

    write_chunk(Fichier,OBJET_BITTEXTURE);
      fwrite(&i,sizeof(int),1,Fichier);
      fwrite(&Nb_Bit_Texture,sizeof(int),1,Fichier);
      for (j=0;j<=MAX_BIT_TEXTURE;j++) {
        fwrite(&Objet[i]->BitTexture[j],sizeof(int),1,Fichier);
      }

    SAUTE_OBJET:
    if (i%10==0) message("Saving %.0f%c",((DBL)i/NbObjet)*100,'%');
  }

  fclose(Fichier);
  free(FBuffer);

  message_termine(0,"File %s%s saved.",CheminSCN,FichierSCN);
  if (Partiel) {
    strcpy(FichierSCN,TmpFichierSCN);
    strcpy(CheminSCN,TmpCheminSCN);
  }
  return 1;
}

// -------------------------------------------------------------------------
// -- LECTURE D'UN FICHIER DE DESCRIPTION DE SCENE -------------------------
// -------------------------------------------------------------------------
byte lecture_fichier_bin(char *Nom,byte Selecteur,byte Fusion,byte Config) {
  FILE *Fichier;
  char Buffer[256];
  char *FBuffer;
  char MarqueObjet[10];
  time_t Tps1,Tps2;
  register int i,NumObjetFusion,j;
  static int NbObjetTmp;
  char TmpFichierSCN[MAXFILE+MAXEXT];
  char TmpCheminSCN[MAXPATH];
  int NbAvantLoad=NbObjet;
  byte NomAuto=0;
  char *Spec[2]={"1","*.SCN"};
  int PolyAvantLoad=NbPoly;
  int Chunk;

  if (Fusion==1) {
    if (Nom) {
      strcpy(TmpFichierSCN,FichierSCN);
      strcpy(TmpCheminSCN,CheminSCN);
      strcpy(FichierSCN,"");
      strcpy(CheminSCN,Nom);
    } else {
      strcpy(TmpFichierSCN,FichierSCN);
      strcpy(TmpCheminSCN,CheminSCN);
    }
    reinit_buffer_objet();
  }

  if (Selecteur) {
    chdir("SCENE");
    strcpy(Buffer,selection_fichier(200,90,"Loading a scene",Spec));
    if (Buffer[0]==27 || Buffer[0]==0) return 0;
    split_chemin(CheminSCN,Buffer,4);
    split_chemin(FichierSCN,Buffer,5);
  }

  if ((NbObjet || NbOmni || NbSpot || NbAmpoule || NbCylLight || NbCamera) && Fusion==0 && Config==0 && Selecteur) {
    strcpy(StrBoite[0],"Loading scene");
    strcpy(StrBoite[1],"Overwrite current scene in memory");
    strcpy(StrBoite[2],"with the scene file");
    sprintf(StrBoite[3],"%s%s ?",CheminSCN,FichierSCN);
    if (g_boite_ONA(CentX,CentY,3,CENTRE,1)) goto FIN_LECTURE_FICHIER;
    bouton_nouveau(0);
  }

  log_out(0,"Read file: %s-%s, merge=%d",CheminSCN,FichierSCN,Fusion);

  Tps1=time(NULL);

  if (Fusion==0 && Config==1) {
    NbObjet=0;
    NbOmni=0;
    NbAmpoule=0;
    NbSpot=0;
    NbCylLight=0;
    NbCamera=0;
    NumCam=0;
    Selection=0;
    reinit_buffer_poly();
    reinit_buffer_spline();
    NbSpline=-1;
    NbPoly=-1;
  }

  if (strinstr(0,FichierSCN,"NONAME.SCN")==0) {
    redessine_fenetre(5,1);
    goto FIN_LECTURE_FICHIER;
  }

  strcpy(Buffer,CheminSCN);
  strcat(Buffer,FichierSCN);

  if (!test_fichier(Buffer)) { redessine_fenetre(5,1); goto FIN_LECTURE_FICHIER; }
  
  Fichier=fopen(Buffer,"rb");
  FBuffer=malloc(0xFFFF);
  setvbuf(Fichier,FBuffer,_IOFBF,0xFFFF);

  GMouseOff();

  while (!feof(Fichier)) {
    Chunk=read_word(Fichier);
    if (Config==1 && !Fusion) {
      switch(Chunk) {

        case CAMERA_COLOR:
          CCAMERA=read_int(Fichier);
          break;

        case VIEWPORT_COLOR:
          FFOND=read_int(Fichier);
          break;

        case BACK_TEXT_COLOR:
          ZFOND=read_int(Fichier);
          break;

        case SELECTION_COLOR:
          CSELECTION=read_int(Fichier);
          break;

        case INTERFACE_COLOR:
          FOND=read_int(Fichier);
          break;

        case AXIS_COLOR:
          CAXE=read_int(Fichier);
          break;

        case GRID_COLOR:
          CGRILLE=read_int(Fichier);
          break;

        case VIEWPORT_CURRENT:
          NF=read_int(Fichier);
          break;

        case VIEWPORT_X4:
          Fx4=read_int(Fichier);
          bouton_fenetre(0);
          break;

        case AXIS_DISPLAY:
          OptAxe=read_int(Fichier);
          break;

        case GRID_DISPLAY:
          OptGrille=read_int(Fichier);
          break;

        case TEXTURE_CURRENT:
          read_string(Fichier,MatiereCourante);
          break;

        case CAMERA_CURRENT:
          NumCam=read_int(Fichier);
          break;

        case VIEWPORT_SCALE_XY:
          Vid[read_int(Fichier)].Echelle=read_dbl(Fichier);
          break;

        case RENDER_BACKGROUND:
          Fond_RVB[_R]=read_int(Fichier);
          Fond_RVB[_V]=read_int(Fichier);
          Fond_RVB[_B]=read_int(Fichier);
          break;

        case FOG_RGB:
          Fog_RGB[_R]=read_int(Fichier);
          Fog_RGB[_V]=read_int(Fichier);
          Fog_RGB[_B]=read_int(Fichier);
          break;

        case FOG_OK:
          Fog_Ok=read_int(Fichier);
          break;

        case FOG_DISTANCE:
          Fog_Distance=read_dbl(Fichier);
          break;

        case FOG_TYPE:
          Fog_Type=read_int(Fichier);
          Fog_Type=(Fog_Type>=1 && Fog_Type<=2 ? Fog_Type:1);
          break;

        case FOG_OFFSET:
          Fog_Offset=read_dbl(Fichier);
          break;

        case FOG_ALT:
          Fog_Alt=read_dbl(Fichier);
          break;

        case FOG_TURBD:
          Fog_TurbD=read_dbl(Fichier);
          break;

        case FOG_TURBULENCE:
          Fog_Turbulence=read_dbl(Fichier);
          break;

        case GLOBAL_ADC:
          Global_ADC=read_dbl(Fichier);
          break;

        case GLOBAL_A_GAMMA:
          Global_A_Gamma=read_dbl(Fichier);
          break;

        case GLOBAL_MAXTRACE:
          Global_MaxTrace=read_int(Fichier);
          break;

        case GLOBAL_MAXINTER:
          Global_MaxInter=read_int(Fichier);
          break;

        case GLOBAL_NBWAVE:
          Global_NbWave=read_int(Fichier);
          break;

        case GLOBAL_IRID:
          read_vecteur(Fichier,Global_Irid);
          break;
        }

      if (!strinstr(0,Buffer,"Atmosphere")) {
        analyse_ligne(Buffer,32);
        Atmos_OnOff  =atoi(Argu[ 1]);
        Atmos_Type   =atoi(Argu[ 2]);
        Atmos_Dist   =atof(Argu[ 3]);
        Atmos_Scatt  =atof(Argu[ 4]);
        Atmos_Eccent =atof(Argu[ 5]);
        Atmos_Samples=atoi(Argu[ 6]);
        Atmos_Jitter =atof(Argu[ 7]);
        Atmos_aa_t   =atof(Argu[ 8]);
        Atmos_aa_l   =atoi(Argu[ 9]);
        Atmos_RGB[_R]=atoi(Argu[10]);
        Atmos_RGB[_V]=atoi(Argu[11]);
        Atmos_RGB[_B]=atoi(Argu[12]);
      }

      if (!strinstr(0,Buffer,"Shift-X")) {
        analyse_ligne(Buffer,32);
        if (!strinstr(0,Argu[1],"F0")) Vid[0].Depla.X=atof(Argu[2]);
        if (!strinstr(0,Argu[1],"F1")) Vid[1].Depla.X=atof(Argu[2]);
        if (!strinstr(0,Argu[1],"F2")) Vid[2].Depla.X=atof(Argu[2]);
      }

      if (!strinstr(0,Buffer,"Shift-Y")) {
        analyse_ligne(Buffer,32);
        if (!strinstr(0,Argu[1],"F0")) Vid[0].Depla.Y=atof(Argu[2]);
        if (!strinstr(0,Argu[1],"F1")) Vid[1].Depla.Y=atof(Argu[2]);
        if (!strinstr(0,Argu[1],"F2")) Vid[2].Depla.Y=atof(Argu[2]);
      }

      if (!strinstr(0,Buffer,"UseSelection")) {
        analyse_ligne(Buffer,32);
        OkSelect=Argu[1][0]-'0';
        coche_case(0,OkSelect);
      }

      if (!strinstr(0,Buffer,"Omni") && incr_NbOmni(0)) {
        analyse_ligne(Buffer,32);
        Omni[NbOmni].Point[_X]=atof(Argu[2]);
        Omni[NbOmni].Point[_Y]=atof(Argu[3]);
        Omni[NbOmni].Point[_Z]=atof(Argu[4]);
        Omni[NbOmni].RVB[_R]=atoi(Argu[5]);
        Omni[NbOmni].RVB[_V]=atoi(Argu[6]);
        Omni[NbOmni].RVB[_B]=atoi(Argu[7]);
        Omni[NbOmni].Taille=atoi(Argu[8]);
        Omni[NbOmni].OnOff=atoi(Argu[9]);
        Omni[NbOmni].Ombre=atoi(Argu[10]);
        Omni[NbOmni].F_Dist=atoi(Argu[11]);
        Omni[NbOmni].F_Power=atof(Argu[12]);
        Omni[NbOmni].Fade=atoi(Argu[13]);
        Omni[NbOmni].Atmos=atoi(Argu[14]);
        Omni[NbOmni].Atmos_Att=atoi(Argu[15]);
        test_limite_omni(NbOmni);
      }

      if (!strinstr(0,Buffer,"Area") && incr_NbAmpoule(0)) {
        analyse_ligne(Buffer,32);
        Ampoule[NbAmpoule].Point[_X]=atof(Argu[2]);
        Ampoule[NbAmpoule].Point[_Y]=atof(Argu[3]);
        Ampoule[NbAmpoule].Point[_Z]=atof(Argu[4]);
        Ampoule[NbAmpoule].RVB[_R]=atoi(Argu[5]);
        Ampoule[NbAmpoule].RVB[_V]=atoi(Argu[6]);
        Ampoule[NbAmpoule].RVB[_B]=atoi(Argu[7]);
        Ampoule[NbAmpoule].Taille=atoi(Argu[8]);
        Ampoule[NbAmpoule].Douceur=atoi(Argu[9]);
        Ampoule[NbAmpoule].Rayon=atof(Argu[10]);
        Ampoule[NbAmpoule].OnOff=atoi(Argu[11]);
        Ampoule[NbAmpoule].Ombre=atoi(Argu[12]);
        Ampoule[NbAmpoule].F_Dist=atoi(Argu[13]);
        Ampoule[NbAmpoule].F_Power=atof(Argu[14]);
        Ampoule[NbAmpoule].Fade=atoi(Argu[15]);
        Ampoule[NbAmpoule].Atmos=atoi(Argu[16]);
        Ampoule[NbAmpoule].Atmos_Att=atoi(Argu[17]);
        test_limite_ampoule(NbAmpoule);
      }

      if (!strinstr(0,Buffer,"Spot") && incr_NbSpot(0)) {
        analyse_ligne(Buffer,32);
        Spot[NbSpot].OX=atof(Argu[2]);
        Spot[NbSpot].OY=atof(Argu[3]);
        Spot[NbSpot].OZ=atof(Argu[4]);
        Spot[NbSpot].CX=atof(Argu[5]);
        Spot[NbSpot].CY=atof(Argu[6]);
        Spot[NbSpot].CZ=atof(Argu[7]);
        Spot[NbSpot].RVB[_R]=atoi(Argu[8]);
        Spot[NbSpot].RVB[_V]=atoi(Argu[9]);
        Spot[NbSpot].RVB[_B]=atoi(Argu[10]);
        Spot[NbSpot].Taille=atoi(Argu[11]);
        Spot[NbSpot].Angle1=atof(Argu[12]);
        Spot[NbSpot].Angle2=atof(Argu[13]);
        Spot[NbSpot].OnOff=atoi(Argu[14]);
        Spot[NbSpot].Ombre=atoi(Argu[15]);
        Spot[NbSpot].F_Dist=atoi(Argu[16]);
        Spot[NbSpot].F_Power=atof(Argu[17]);
        Spot[NbSpot].Fade=atoi(Argu[18]);
        Spot[NbSpot].Atmos=atoi(Argu[19]);
        Spot[NbSpot].Atmos_Att=atoi(Argu[20]);
        Spot[NbSpot].Cone=atoi(Argu[21]);
        test_limite_ampoule(NbSpot);
      }

      if (!strinstr(0,Buffer,"CylLight") && incr_NbCylLight(0)) {
        analyse_ligne(Buffer,32);
        CylLight[NbCylLight].OX=atof(Argu[2]);
        CylLight[NbCylLight].OY=atof(Argu[3]);
        CylLight[NbCylLight].OZ=atof(Argu[4]);
        CylLight[NbCylLight].CX=atof(Argu[5]);
        CylLight[NbCylLight].CY=atof(Argu[6]);
        CylLight[NbCylLight].CZ=atof(Argu[7]);
        CylLight[NbCylLight].RVB[_R]=atoi(Argu[8]);
        CylLight[NbCylLight].RVB[_V]=atoi(Argu[9]);
        CylLight[NbCylLight].RVB[_B]=atoi(Argu[10]);
        CylLight[NbCylLight].Taille=atoi(Argu[11]);
        CylLight[NbCylLight].Angle1=atof(Argu[12]);
        CylLight[NbCylLight].Angle2=atof(Argu[13]);
        CylLight[NbCylLight].OnOff=atoi(Argu[14]);
        CylLight[NbCylLight].Ombre=atoi(Argu[15]);
        CylLight[NbCylLight].F_Dist=atoi(Argu[16]);
        CylLight[NbCylLight].F_Power=atof(Argu[17]);
        CylLight[NbCylLight].Fade=atoi(Argu[18]);
        CylLight[NbCylLight].Atmos=atoi(Argu[19]);
        CylLight[NbCylLight].Atmos_Att=atoi(Argu[20]);
        CylLight[NbCylLight].Cone=atoi(Argu[21]);
        test_limite_ampoule(NbCylLight);
      }

      if (!strinstr(0,Buffer,"Camera:") && incr_NbCamera(0)) {
        analyse_ligne(Buffer,32);
        Camera[NbCamera].OX=atof(Argu[1]);
        Camera[NbCamera].OY=atof(Argu[2]);
        Camera[NbCamera].OZ=atof(Argu[3]);
        Camera[NbCamera].CX=atof(Argu[4]);
        Camera[NbCamera].CY=atof(Argu[5]);
        Camera[NbCamera].CZ=atof(Argu[6]);
        Camera[NbCamera].Ouverture=atof(Argu[7]);
        Camera[NbCamera].ProChamp=atof(Argu[8]);
        Camera[NbCamera].OnOff=atoi(Argu[9]);
        Camera[NbCamera].F_Blur=atoi(Argu[10]);
        Camera[NbCamera].Samples=atof(Argu[11]);
        Camera[NbCamera].Aperture=atof(Argu[12]);
        Camera[NbCamera].Variance=atof(Argu[13]);
        Camera[NbCamera].Confidence=atof(Argu[14]);
      }

      if (!strinstr(0,Buffer,"Display_Omni")) {
        analyse_ligne(Buffer,32);
        Omni[0].Cache=!atoi(Argu[1]);
      }

      if (!strinstr(0,Buffer,"Display_Spot")) {
        analyse_ligne(Buffer,32);
        Spot[0].Cache=!atoi(Argu[1]);
      }

      if (!strinstr(0,Buffer,"Display_CylLight")) {
        analyse_ligne(Buffer,32);
        CylLight[0].Cache=!atoi(Argu[1]);
      }

      if (!strinstr(0,Buffer,"Display_Area")) {
        analyse_ligne(Buffer,32);
        Ampoule[0].Cache=!atoi(Argu[1]);
      }

      if (!strinstr(0,Buffer,"Display_Camera")) {
        analyse_ligne(Buffer,32);
        Camera[0].Cache=!atoi(Argu[1]);
      }
    }

    // -------------------------------- GÇnÇral

    if (!strinstr(0,Buffer,"Nb-Object")) {
      analyse_ligne(Buffer,32);
      NbObjetTmp=atoi(Argu[1]);
    }

    // --------------------------------- lecture objets

    if (!strinstr(0,Buffer,"Object") && Config==0) {

      if (incr_NbObjet(0)==0) break;
      if (alloc_mem_objet(NbObjet)==0) break;

      if (!Fusion && NbObjet%10==0) {
        message("Loading %.0f%c",((DBL)(NbObjet-NbAvantLoad)/NbObjetTmp)*100,'%');
      }

      analyse_ligne(Buffer,32);
      strcpy(MarqueObjet,Argu[1]);
      NumObjetFusion=atoi(Argu[1]);

      // ------------------ initialise certains paramätres objet

      Objet[NbObjet]->Type=atoi(Argu[2]);

      strcpy(Objet[NbObjet]->Nom,"Object");
      strcat(Objet[NbObjet]->Nom,MarqueObjet);
      Objet[NbObjet]->Selection=0;
      Objet[NbObjet]->Nom[strlen(Objet[NbObjet]->Nom)-1]=NULLC;
      Objet[NbObjet]->Poly=-1;
      Objet[NbObjet]->Cache=0;
      Objet[NbObjet]->MS[0]=Objet[NbObjet]->MS[1]=Objet[NbObjet]->MS[2]=1;
      Objet[NbObjet]->MR[0]=Objet[NbObjet]->MR[1]=Objet[NbObjet]->MR[2]=0;
      Objet[NbObjet]->MT[0]=Objet[NbObjet]->MT[1]=Objet[NbObjet]->MT[2]=0;
      Objet[NbObjet]->Smooth=1;
      strcpy(Objet[NbObjet]->CheminRaw,"@");
      Objet[NbObjet]->Ignore=0;
      Objet[NbObjet]->Freeze=0;
      Objet[NbObjet]->Rapide=0;
      Objet[NbObjet]->Operator=PAS_CSG;
      Objet[NbObjet]->CSG=PAS_CSG;
      Objet[NbObjet]->Ombre=1;
      Objet[NbObjet]->Ior=0.0;       Objet[NbObjet]->BitTexture[ 0]=0;
      Objet[NbObjet]->Refraction=0;  Objet[NbObjet]->BitTexture[ 1]=0;
      Objet[NbObjet]->Reflexion=0;   Objet[NbObjet]->BitTexture[ 2]=0;
      Objet[NbObjet]->Diffusion=0;   Objet[NbObjet]->BitTexture[ 3]=0;
      Objet[NbObjet]->Ambiance=0;    Objet[NbObjet]->BitTexture[ 4]=0;
      Objet[NbObjet]->Crand=0;       Objet[NbObjet]->BitTexture[ 5]=0;
      Objet[NbObjet]->Phong=0;       Objet[NbObjet]->BitTexture[ 6]=0;
      Objet[NbObjet]->PSize=0;       Objet[NbObjet]->BitTexture[ 7]=0;
      Objet[NbObjet]->Caustics=0;    Objet[NbObjet]->BitTexture[ 8]=0;
      Objet[NbObjet]->Fade_D=0;      Objet[NbObjet]->BitTexture[ 9]=0;
      Objet[NbObjet]->Fade_P=0;      Objet[NbObjet]->BitTexture[10]=0;
      Objet[NbObjet]->Rough=0;       Objet[NbObjet]->BitTexture[11]=0;
      Objet[NbObjet]->Brilli=0;      Objet[NbObjet]->BitTexture[12]=0;
      Objet[NbObjet]->Specular=0;    Objet[NbObjet]->BitTexture[13]=0;
      Objet[NbObjet]->HautHF=1;
      Objet[NbObjet]->Halo=0;
      Objet[NbObjet]->WLevel=0;
      for (j=0;j<=1;j++) {
        Objet[NbObjet]->Map[j].Name[0]=NULLC;
        Objet[NbObjet]->Map[j].On=0;
        Objet[NbObjet]->Map[j].Type=0;
        Objet[NbObjet]->Map[j].Once=0;
        Objet[NbObjet]->Map[j].Interpolate=0;
        Objet[NbObjet]->Map[j].Amount=0;
        Objet[NbObjet]->Map[j].Alpha=0;
        Objet[NbObjet]->Map[j].Filter=0;
        Objet[NbObjet]->Map[j].Color=0;
      }
      Objet[NbObjet]->LooksLike.Nb=0;
      Objet[NbObjet]->LooksLike.Light=0;

      // ------------------ lecture paramätres objets

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);

      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->P[0]=atof(Argu[2]);
        Objet[NbObjet]->P[1]=atof(Argu[3]);
        Objet[NbObjet]->P[2]=atof(Argu[4]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->S[0]=atof(Argu[2]);
        Objet[NbObjet]->S[1]=atof(Argu[3]);
        Objet[NbObjet]->S[2]=atof(Argu[4]);

        if (Objet[NbObjet]->S[0]==0) Objet[NbObjet]->S[0]=1;
        if (Objet[NbObjet]->S[1]==0) Objet[NbObjet]->S[1]=1;
        if (Objet[NbObjet]->S[2]==0) Objet[NbObjet]->S[2]=1;
        if (Objet[NbObjet]->S[0]<0) Objet[NbObjet]->S[0]*=-1;
        if (Objet[NbObjet]->S[1]<0) Objet[NbObjet]->S[1]*=-1;
        if (Objet[NbObjet]->S[2]<0) Objet[NbObjet]->S[2]*=-1;
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->R[0]=atof(Argu[2]);
        Objet[NbObjet]->R[1]=atof(Argu[3]);
        Objet[NbObjet]->R[2]=atof(Argu[4]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->T[0]=atof(Argu[2]);
        Objet[NbObjet]->T[1]=atof(Argu[3]);
        Objet[NbObjet]->T[2]=atof(Argu[4]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Couleur=(byte) atoi(Argu[2]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        strcpy(Objet[NbObjet]->Matiere,Argu[2]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Selection=(byte) atoi(Argu[2]);
        if (Objet[NbObjet]->Selection==1) Selection++;
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Cache=(byte) atoi(Argu[2]);
        if (Objet[NbObjet]->Cache) Objet[NbObjet]->Selection=0;
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        if (strlen(Argu[2])>12) Argu[2][12]=NULLC;
        strcpy(Objet[NbObjet]->Nom,Argu[2]);
        if (nom_objet_existe(Argu[2],NbObjet) && NomAuto!=2) {
        if (NomAuto==2 || NomAuto==0) message("One object '%s' already exist ! Give another name or accept",Argu[2]);
          if (NomAuto==0) {
            NomAuto=nom_objet_auto();
            NomAuto=(NomAuto==1 ? 1:2);
          }
          if (NomAuto==1) creer_nom_objet(NbObjet);
          if (NomAuto==2) renomme_objet(0,0);
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }
      
      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        if ((Argu[2][0]=='#') && test_fichier(Argu[2])+1) {
          sprintf(Objet[NbObjet]->CheminRaw,"%s",Argu[2]+1);
          if (Fusion) {
            Objet[NbObjet]->Poly=PolyAvantLoad+1+atoi(Argu[3]);
          } else {
            Objet[NbObjet]->Poly=atoi(Argu[3]);
          }
          Objet[NbObjet]->P[_X]=Objet[NbObjet]->S[_X];
          Objet[NbObjet]->P[_Y]=Objet[NbObjet]->S[_Y];
          Objet[NbObjet]->P[_Z]=Objet[NbObjet]->S[_Z];
          switch (Objet[NbObjet]->Type) {
            case TRIANGLE:
              j=lecture_triangle_raw(Objet[NbObjet]->CheminRaw,0);
              break;
            case HFIELD:
              j=lecture_hfield(Objet[NbObjet]->CheminRaw,0);
              break;
          }
          if (!j) {
            if (Objet[NbObjet]->Selection) Selection--;
            free_mem_objet(NbObjet);
            NbObjet--;
            do {
              fgets(Buffer,256,Fichier);
              analyse_ligne(Buffer,32);
            } while (!strinstr(0,MarqueObjet,Argu[1]));
            goto LABEL_FIN_CHARGE_OBJET;
          } else {
            Objet[NbObjet]->S[_X]=Objet[NbObjet]->P[_X];
            Objet[NbObjet]->S[_Y]=Objet[NbObjet]->P[_Y];
            Objet[NbObjet]->S[_Z]=Objet[NbObjet]->P[_Z];
            Poly[Objet[NbObjet]->Poly]->Smooth=Objet[NbObjet]->Smooth;
          }
        } else {
          if (Fusion) {
            Objet[NbObjet]->Poly=PolyAvantLoad+1+atoi(Argu[2]);
          } else {
            Objet[NbObjet]->Poly=atoi(Argu[2]);
          }
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      if (Objet[NbObjet]->Type==TRIANGLE && Objet[NbObjet]->CheminRaw[0]=='@') {
        j=lecture_triangle_scn(Fichier,MarqueObjet);
        if (!j) {
          if (Objet[NbObjet]->Selection) Selection--;
          free_mem_objet(NbObjet);
          NbObjet--;
          do {
            fgets(Buffer,256,Fichier);
            analyse_ligne(Buffer,32);
          } while (!strinstr(0,MarqueObjet,Argu[1]));
          goto LABEL_FIN_CHARGE_OBJET;
        }
        Poly[Objet[NbObjet]->Poly]->Smooth=Objet[NbObjet]->Smooth;
      }

      if (Objet[NbObjet]->Type==SPLINE) {
        j=lecture_spline_scn(Fichier,MarqueObjet);
        if (!j) {
          if (Objet[NbObjet]->Selection) Selection--;
          free_mem_objet(NbObjet);
          NbObjet--;
          do {
            fgets(Buffer,256,Fichier);
            analyse_ligne(Buffer,32);
          } while (!strinstr(0,MarqueObjet,Argu[1]));
          goto LABEL_FIN_CHARGE_OBJET;
        }
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Rapide=(byte) atoi(Argu[2]);  // ------------- Rapide
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {  // ------------- M_Scale
        Objet[NbObjet]->MS[0]=atof(Argu[2]);
        Objet[NbObjet]->MS[1]=atof(Argu[3]);
        Objet[NbObjet]->MS[2]=atof(Argu[4]);

        if (Objet[NbObjet]->MS[0]==0) Objet[NbObjet]->MS[0]=1;
        if (Objet[NbObjet]->MS[1]==0) Objet[NbObjet]->MS[1]=1;
        if (Objet[NbObjet]->MS[2]==0) Objet[NbObjet]->MS[2]=1;
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {  // ----------------- M_Rotate
        Objet[NbObjet]->MR[0]=atof(Argu[2]);
        Objet[NbObjet]->MR[1]=atof(Argu[3]);
        Objet[NbObjet]->MR[2]=atof(Argu[4]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) { // ----------------- M_Translate
        Objet[NbObjet]->MT[0]=atof(Argu[2]);
        Objet[NbObjet]->MT[1]=atof(Argu[3]);
        Objet[NbObjet]->MT[2]=atof(Argu[4]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);  // --------------------------- Smooth
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Smooth=(byte) atoi(Argu[2]);
        if (Objet[NbObjet]->Poly>-1) {
          Poly[Objet[NbObjet]->Poly]->Smooth=Objet[NbObjet]->Smooth;
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Freeze
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Freeze=(byte) atoi(Argu[2]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Ignore
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Ignore=(byte) atoi(Argu[2]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Operator
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Operator=atoi(Argu[2]);
        if (Objet[NbObjet]->Operator==OK_CSG && Fusion) {
          Objet[NbObjet]->Buffer=-NumObjetFusion;
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Objet Operator
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->CSG=atoi(Argu[2]);
        if (Objet[NbObjet]->CSG!=PAS_CSG && Fusion) {
          Objet[NbObjet]->Buffer=Objet[NbObjet]->CSG;
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Objet Ombre
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        if (Argu[2]==NULL) {
          Objet[NbObjet]->Ombre=1;
        } else {
          Objet[NbObjet]->Ombre=atoi(Argu[2]);
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Scale Objet/Matiere
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        if (Argu[2]==NULL) {
          Objet[NbObjet]->ScaleOM=1;
        } else {
          Objet[NbObjet]->ScaleOM=atoi(Argu[2]);
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // -------- Autres paramätres texture
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        if (Argu[2]!=NULL) {
          Objet[NbObjet]->Ior=atof(Argu[2]);
          Objet[NbObjet]->Refraction=atoi(Argu[3]);
          Objet[NbObjet]->Reflexion=atoi(Argu[4]);
          Objet[NbObjet]->Diffusion=atoi(Argu[5]);
          Objet[NbObjet]->Ambiance=atoi(Argu[6]);
          Objet[NbObjet]->Crand=atoi(Argu[7]);
          Objet[NbObjet]->Phong=atoi(Argu[8]);
          Objet[NbObjet]->PSize=atoi(Argu[9]);
          Objet[NbObjet]->Caustics=atoi(Argu[10]);
          Objet[NbObjet]->Fade_D=atoi(Argu[11]);
          Objet[NbObjet]->Fade_P=atoi(Argu[12]);
          Objet[NbObjet]->Rough=atoi(Argu[13]);
          Objet[NbObjet]->Brilli=atoi(Argu[14]);
          Objet[NbObjet]->Specular=atoi(Argu[15]);
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // -------- Bits Autre paramätres texture
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        for (j=0;j<strlen(Argu[2]);j++) {
          if (Argu[2]==NULL) {
            Objet[NbObjet]->BitTexture[j]=0;
          } else {
            if (Argu[2][j]=='1' || Argu[2][j]=='0') {
              Objet[NbObjet]->BitTexture[j]=(Argu[2][j]>'1' ? 0:Argu[2][j]-'0');
            }
          }
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Halo/water_level
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Halo=(byte) atoi(Argu[2]);
        if (Objet[NbObjet]->Type==HFIELD) {
          Objet[NbObjet]->WLevel=(DBL) atof(Argu[3]);
        }
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);    // -------------- Image mapping
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        strcpy(Objet[NbObjet]->Map[0].Name,Argu[2]+1);
        Objet[NbObjet]->Map[0].On=atoi(Argu[3]);
        Objet[NbObjet]->Map[0].Type=atoi(Argu[4]);
        Objet[NbObjet]->Map[0].Once=atoi(Argu[5]);
        Objet[NbObjet]->Map[0].Interpolate=atoi(Argu[6]);
        Objet[NbObjet]->Map[0].Amount=atof(Argu[7]);
        Objet[NbObjet]->Map[0].Alpha=atoi(Argu[8]);
        Objet[NbObjet]->Map[0].Filter=atof(Argu[9]);
        Objet[NbObjet]->Map[0].Color=atoi(Argu[10]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);       // -------------- Image bumpping
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        strcpy(Objet[NbObjet]->Map[1].Name,Argu[2]+1);
        Objet[NbObjet]->Map[1].On=atoi(Argu[3]);
        Objet[NbObjet]->Map[1].Type=atoi(Argu[4]);
        Objet[NbObjet]->Map[1].Once=atoi(Argu[5]);
        Objet[NbObjet]->Map[1].Interpolate=atoi(Argu[6]);
        Objet[NbObjet]->Map[1].Amount=atof(Argu[7]);
        Objet[NbObjet]->Map[1].Alpha=atoi(Argu[8]);
        Objet[NbObjet]->Map[1].Filter=atof(Argu[9]);
        Objet[NbObjet]->Map[1].Color=atoi(Argu[10]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier);       // -------------- Looks like
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->LooksLike.Nb=atoi(Argu[2]);
        Objet[NbObjet]->LooksLike.Light=atoi(Argu[3]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      LABEL_FIN_CHARGE_OBJET:
      i=i; // --------- simplement pour Mr WATCOM !!!!!!!!!!!
    }
  }

  LABEL_FIN_INIT:

  fclose(Fichier);
  free(FBuffer);
  utilise_selection(1);
  redessine_fenetre(5,1);
  Tps2=time(NULL);
  message("Time=%.0f seconde(s)",difftime(Tps2,Tps1));
  if (!NumCam && NbCamera && Config) NumCam=1;

  FIN_LECTURE_FICHIER:

  if (Fusion==1) {
    strcpy(FichierSCN,TmpFichierSCN);
    strcpy(CheminSCN,TmpCheminSCN);
    analyse_csg_partiel(NbAvantLoad);
  }

  affiche_donnees();
  return 1;
}
#endif
