/* ---------------------------------------------------------------------------
*  FICHIER.C
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

#define NB_FILE_MAX 500

extern DBL Ratio;

char Argu[ARGU_MAX][128];
char FichierSCN[MAXFILE+MAXEXT]={NULLC};
char CheminSCN[MAXPATH]={NULLC};
char CheminLastSCN[MAXPATH]={NULLC};
char CheminRAYTRACER[MAXPATH];
char Buffer[MAXPATH+MAXFILE+MAXEXT],Heure[9],Date[40];
char Nom_Objet[256];
char ConfigRaytracer[40];

byte EXEC1=0; // premiäre exÇcution avec le modeleur (pour a propos de)
byte OptSauve=1;

// -------------------------------------------------------------------------
// -- CREATE BACKUP --------------------------------------------------------
// -------------------------------------------------------------------------
void create_backup(char *Fichier1) {
  char Fichier2[MAXPATH];
 
  strcpy(Fichier2,Fichier1);
  strcpy(strrchr(Fichier2,'.'),".BAK");

  remove(Fichier2);
  rename(Fichier1,Fichier2);  
}

// -------------------------------------------------------------------------
// -- ANALYSE UNE LIGNE D'UN FICHIER ---------------------------------------
// -------------------------------------------------------------------------
byte analyse_ligne(char *TempChar,byte Separateur) {
  register int k,i;
  char Marque[2];
  char *Pointeur;
  char *Temp;
  
  Temp=(char *) malloc(strlen(TempChar)+1);
  strcpy(Temp,TempChar);

  supprime_tab(TempChar);
  Marque[0]=Separateur;
  Marque[1]=NULLC;

  for (k=0;k<ARGU_MAX;k++) Argu[k][0]=NULLC;
  k=0;

  Pointeur=strtok(TempChar,Marque);

  if (Pointeur[0]=='\n') {
    k=0;
  } else {
    while (Pointeur) {
      strcpy(Argu[k],Pointeur);
      i=strinstr(0,Argu[k],"\n");
      if (i) Argu[k][i]=NULLC;
      Pointeur=strtok(NULL,Marque);
      k++;
    }
  }
  
  strcpy(TempChar,Temp);
  free((char *) Temp);
  
  return k;
}

// -------------------------------------------------------------------------
// -- SAUVE UN FICHIER DE DESCRIPTION DE SCENE -----------------------------
// -------------------------------------------------------------------------
byte sauve_fichier(byte Ecrase,byte Partiel) {
  FILE *Fichier;
  register int i,j;
  char TmpFichierSCN[MAXFILE+MAXEXT];
  char TmpCheminSCN[MAXPATH];

  // sauve_fichier_bin(Ecrase,Partiel);

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

  get_date(Date);
  get_heure(Heure);

  create_backup(Buffer);

  Fichier=fopen(Buffer,"wt");

  fprintf(Fichier,";%s version %s - ascii scene for 3D modeller.\n",NomLogiciel,VerLogiciel);
  fprintf(Fichier,";(C) %s, Denis Olivier.\n",RES_COPY[4]);
  fprintf(Fichier,";All rights reserved.\n");
  fprintf(Fichier,";Date=%-s.\n;Time=%s.\n",Date,Heure);
  fprintf(Fichier,";user_name=%s\n",NomUtilisateur);

  fprintf(Fichier,"NbRendu=%d\n",NbRendu);
  fprintf(Fichier,"ColorCamera=%d\n",CCAMERA);
  fprintf(Fichier,"ColorViewport=%d\n",FFOND);
  fprintf(Fichier,"ColorTextBackground=%d\n",ZFOND);
  fprintf(Fichier,"ColorSelection=%d\n",CSELECTION);
  fprintf(Fichier,"ColorInterface=%d\n",FOND);
  fprintf(Fichier,"ColorAxis=%d\n",CAXE);
  fprintf(Fichier,"ColorGrid=%d\n",CGRILLE);
  fprintf(Fichier,"GrilleType=%d\n",GrilleType);
  fprintf(Fichier,"CurrentViewport=%d\n",NF);
  fprintf(Fichier,"Viewportx4=%d\n",Fx4);
  fprintf(Fichier,"DisplayAxis=%d\n",OptAxe);
  fprintf(Fichier,"DisplayGrid=%d\n",OptGrille);
  fprintf(Fichier,"CurrentTexture=%s\n",MatiereCourante);
  fprintf(Fichier,"CurrentCamera=%d\n",NumCam);
  fprintf(Fichier,"BackgroundImageRGB: %d %d %d\n",Fond_RVB[0],Fond_RVB[1],Fond_RVB[2]);
  for (i=0;i<=4;i++) {
    fprintf(Fichier,"FogLayer: %d %d %d %d %d %.4g %d %.4g %.4g %.4g %.4g\n",
                i,
                Fog[i].RGB[0],
                Fog[i].RGB[1],
                Fog[i].RGB[2],
                Fog[i].Ok,
                Fog[i].Distance,
                Fog[i].Type,
                Fog[i].Offset,
                Fog[i].Alt,
                Fog[i].TurbD,
                Fog[i].Turbulence);
  }
  fprintf(Fichier,"Atmosphere: %d %d %.4g %.4g %.4g %d %.4g %.4g %d %d %d %d\n",
                Atmos_OnOff,
                Atmos_Type,
                Atmos_Dist,
                Atmos_Scatt,
                Atmos_Eccent,
                Atmos_Samples,
                Atmos_Jitter,
                Atmos_aa_t,
                Atmos_aa_l,
                Atmos_RGB[_R],
                Atmos_RGB[_V],
                Atmos_RGB[_B]);
  fprintf(Fichier,"UseSelection: %d\n",(Cc[0].Croix ? 1:0));
  fprintf(Fichier,"RayGlobal: %.6f %.4g %d %d %d %.2f %.2f %.2f\n",
            Global_ADC,
            Global_A_Gamma,
            Global_MaxTrace,
            Global_MaxInter,
            Global_NbWave,
            Global_Irid[_R],
            Global_Irid[_V],
            Global_Irid[_B]);

  fprintf(Fichier,"Radiosity: %d %d %.4g %d %.4g %.4g %.4g %.4g %.4g %d %d\n",
            Rad_OnOff,
            Rad_Type,
            Rad_Brightness,
            Rad_Count,
            Rad_Dist_Max,
            Rad_Err_Bound,
            Rad_Gray_Threshold,
            Rad_Low_Err_Fact,
            Rad_Min_Reuse,
            Rad_Near_Count,
            Rad_Rec_Lim);

  for (i=0;i<=3;i++) {
    fprintf(Fichier,"Scale-X/Y F%d: %.4g\n",i,Vid[i].Echelle);
    fprintf(Fichier,"Shift-X F%d: %.4g\n",i,Vid[i].Depla.X);
    fprintf(Fichier,"Shift-Y F%d: %.4g\n",i,Vid[i].Depla.Y);
  }

  for (i=1;i<=NbCamera;i++) {
    fprintf(Fichier,"Camera: %.4g %.4g %.4g %.4g %.4g %.4g %.4g %.4g %d %d %.4g %.4g %.4g %.4g %.4g\n",
      Camera[i].OX,Camera[i].OY,Camera[i].OZ,
      Camera[i].CX,Camera[i].CY,Camera[i].CZ,
      Camera[i].Ouverture,Camera[i].ProChamp,Camera[i].OnOff,
      Camera[i].F_Blur,Camera[i].Samples,Camera[i].Aperture,
      Camera[i].Variance,Camera[i].Confidence,Camera[i].Roll);
  }
  if (NbCamera) fprintf(Fichier,"Display_Camera: %d\n\n",!Camera[0].Cache);

  for (i=1;i<=NbOmni;i++) {
    fprintf(Fichier,"Omni %03d: %.4g %.4g %.4g %d %d %d %d %d %d %.4g %d %d %d %d %s\n",
      i,
      Omni[i].Point[_X],Omni[i].Point[_Y],Omni[i].Point[_Z],
      Omni[i].RVB[_R],Omni[i].RVB[_V],Omni[i].RVB[_B],
      Omni[i].Taille,Omni[i].OnOff,Omni[i].Ombre,Omni[i].F_Dist,
      Omni[i].F_Power,Omni[i].Fade,Omni[i].Atmos,Omni[i].Atmos_Att,Omni[i].Flare);
  }
  if (NbOmni) fprintf(Fichier,"Display_Omni: %d\n\n",!Omni[0].Cache);

  for (i=1;i<=NbSpot;i++) {
    fprintf(Fichier,"Spot %03d: %.4g %.4g %.4g %.4g %.4g %.4g %d %d %d %d %.4g %.4g %d %d %.4g %d %d %d %d %d %d %.4g %.4g %.4g %.4g %.4g %.4g %d %d %d %.4g %s\n",
      i,
      Spot[i].OX,Spot[i].OY,Spot[i].OZ,
      Spot[i].CX,Spot[i].CY,Spot[i].CZ,
      Spot[i].RVB[_R],Spot[i].RVB[_V],Spot[i].RVB[_B],
      Spot[i].Taille,Spot[i].Angle1,Spot[i].Angle2,
      Spot[i].OnOff,Spot[i].Ombre,Spot[i].F_Dist,
      Spot[i].F_Power,Spot[i].Fade,Spot[i].Atmos,Spot[i].Atmos_Att,Spot[i].Cone,
      Spot[i].Area,
      Spot[i].Axis1[_X],
      Spot[i].Axis1[_Y],
      Spot[i].Axis1[_Z],
      Spot[i].Axis2[_X],
      Spot[i].Axis2[_Y],
      Spot[i].Axis2[_Z],
      Spot[i].Size1,
      Spot[i].Size2,
      Spot[i].Jitter,
      Spot[i].Adaptive,
      Spot[i].Flare
    );
  }
  if (NbSpot) fprintf(Fichier,"Display_Spot: %d\n\n",!Spot[0].Cache);

  for (i=1;i<=NbCylLight;i++) {
    fprintf(Fichier,"CylLight %03d: %.4g %.4g %.4g %.4g %.4g %.4g %d %d %d %d %.4g %.4g %d %d %.4g %d %d %d %d %d %s\n",
      i,
      CylLight[i].OX,CylLight[i].OY,CylLight[i].OZ,
      CylLight[i].CX,CylLight[i].CY,CylLight[i].CZ,
      CylLight[i].RVB[_R],CylLight[i].RVB[_V],CylLight[i].RVB[_B],
      CylLight[i].Taille,CylLight[i].Angle1,CylLight[i].Angle2,
      CylLight[i].OnOff,CylLight[i].Ombre,CylLight[i].F_Dist,
      CylLight[i].F_Power,CylLight[i].Fade,CylLight[i].Atmos,
      CylLight[i].Atmos_Att,CylLight[i].Cone,CylLight[i].Flare);
  }
  if (NbCylLight) fprintf(Fichier,"Display_CylLight: %d\n\n",!CylLight[0].Cache);

  for (i=1;i<=NbArea;i++) {
    fprintf(Fichier,"Area %03d: %.4g %.4g %.4g %d %d %d %d %.4g %d %d %.4g %d %d %d %d %.4g %.4g %.4g %.4g %.4g %.4g %d %d %d %.4g %s\n",
      i,
      Area[i].Point[_X],Area[i].Point[_Y],Area[i].Point[_Z],
      Area[i].RVB[_R],Area[i].RVB[_V],Area[i].RVB[_B],
      Area[i].Taille,Area[i].Rayon,
      Area[i].OnOff,Area[i].Ombre,Area[i].F_Dist,
      Area[i].F_Power,Area[i].Fade,Area[i].Atmos,Area[i].Atmos_Att,
      Area[i].Axis1[_X],
      Area[i].Axis1[_Y],
      Area[i].Axis1[_Z],
      Area[i].Axis2[_X],
      Area[i].Axis2[_Y],
      Area[i].Axis2[_Z],
      Area[i].Size1,
      Area[i].Size2,
      Area[i].Jitter,
      Area[i].Adaptive,
      Area[i].Flare
    );
  }
  if (NbArea) fprintf(Fichier,"Display_Area: %d\n\n",!Area[0].Cache);

  fprintf(Fichier,"Nb-Object: %d\n\n",Partiel ? Selection:NbObjet);

  fprintf(Fichier,"[EndInitSection]\n\n");

  reinit_buffer_poly();
  reinit_buffer_spline();
  GMouseOff();

  for (i=1;i<=NbObjet;i++) {
    if (Partiel && (!Objet[i]->Selection || Objet[i]->Cache)) goto SAUTE_OBJET;
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Type);
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->P[0],Objet[i]->P[1],Objet[i]->P[2]) ;
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2]);
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2]);
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2]);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Couleur);
    fprintf(Fichier,"Object %05d: %s\n",i,Objet[i]->Matiere);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Selection);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Cache);
    fprintf(Fichier,"Object %05d: %s\n",i,Objet[i]->Nom);
    if (Objet[i]->CheminRaw[0]!='@') {
      fprintf(Fichier,"Object %05d: #%s %d\n",i,Objet[i]->CheminRaw,Objet[i]->Poly);
    } else {
      fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Poly);
    }
    if (Objet[i]->Type==TRIANGLE && Objet[i]->CheminRaw[0]=='@') {
      sauve_triangle(Fichier,i);
    }
    if (Objet[i]->Type==SPLINE) sauve_spline(Fichier,i);
    if (Objet[i]->Type==SOR ||
        Objet[i]->Type==LATHE ||
        Objet[i]->Type==EXTRUDE ||
        Objet[i]->Type==BEZIER) sauve_special(Fichier,i);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Rapide);
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->MS[0],Objet[i]->MS[1],Objet[i]->MS[2]);
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->MR[0],Objet[i]->MR[1],Objet[i]->MR[2]);
    fprintf(Fichier,"Object %05d: %.4g %.4g %.4g\n",i,Objet[i]->MT[0],Objet[i]->MT[1],Objet[i]->MT[2]);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Smooth);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Freeze);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Ignore);
    fprintf(Fichier,"Object %05d: %d %d\n",i,Objet[i]->Operator,Objet[i]->Inverse);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->CSG);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->Ombre);
    fprintf(Fichier,"Object %05d: %d\n",i,Objet[i]->ScaleOM);
    fprintf(Fichier,"Object %05d: %.2f %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                     i,
                     Objet[i]->Ior,
                     Objet[i]->Refraction,
                     Objet[i]->Reflexion,
                     Objet[i]->Diffusion,
                     Objet[i]->Ambiance,
                     Objet[i]->Crand,
                     Objet[i]->Phong,
                     Objet[i]->PSize,
                     Objet[i]->Caustics,
                     Objet[i]->Fade_D,
                     Objet[i]->Fade_P,
                     Objet[i]->Rough,
                     Objet[i]->Brilli,
                     Objet[i]->Specular);

    fprintf(Fichier,"Object %05d: %d",i,Objet[i]->BitTexture[0]);
    for (j=1;j<=12;j++) fprintf(Fichier,"%d",Objet[i]->BitTexture[j]);
    fprintf(Fichier,"\nObject %05d: %d %.4g\n",i,Objet[i]->Halo,Objet[i]->WLevel);
    for (j=0;j<=1;j++) {
      fprintf(Fichier,"Object %05d: #%s %d %d %d %d %.4g %d %.4g %d\n",i,
                       Objet[i]->Map[j].Name,
                       Objet[i]->Map[j].On,
                       Objet[i]->Map[j].Type,
                       Objet[i]->Map[j].Once,
                       Objet[i]->Map[j].Interpolate,
                       Objet[i]->Map[j].Amount,
                       Objet[i]->Map[j].Alpha,
                       Objet[i]->Map[j].Filter,
                       Objet[i]->Map[j].Color);
    }
    fprintf(Fichier,"Object %05d: %d %d\n",i,Objet[i]->LooksLike.Nb,
                                            Objet[i]->LooksLike.Light);
    fprintf(Fichier,"\n");
    SAUTE_OBJET:
    if (i%10==0) message("Saving %.0f%c",((DBL)i/NbObjet)*100,'%');
  }

  fprintf(Fichier,"End of file: %s\n",FichierSCN);
  fclose(Fichier);
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
byte lecture_fichier(char *Nom,byte Selecteur,byte Fusion,byte Config) {
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
    if (Buffer[0]==27 || Buffer[0]==NULLC) return 0;
    split_chemin(CheminSCN,Buffer,4);
    split_chemin(FichierSCN,Buffer,5);
  }

  if ((NbObjet || NbOmni || NbSpot || NbArea || NbCylLight || NbCamera) && Fusion==0 && Config==0 && Selecteur) {
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
    NbArea=0;
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
  
  Fichier=fopen(Buffer,"rt");
  FBuffer=malloc(0xFFFF);
  setvbuf(Fichier,FBuffer,_IOFBF,0xFFFF);

  GMouseOff();

  fgets(Buffer,256,Fichier);
  analyse_ligne(Buffer,32);

  if (strinstr(0,Buffer,NomLogiciel)!=1 && EXEC1 && Config==0 && !Fusion) {
    strcpy(StrBoite[0],"Loading a scene");
    sprintf(StrBoite[1],"This scene wasn't created with %s,",NomLogiciel);
    sprintf(StrBoite[2],"but with %s modeller. Ok for objects,",Argu[0]+1);
    strcpy(StrBoite[3],"but can't be sure for textures.");
    g_boite_ONA(CentX,CentY,3,CENTRE,0);
  }

  GMouseOff();

  while (!feof(Fichier)) {
    do {
      fgets(Buffer,256,Fichier);
      if (feof(Fichier)) break;
    } while (Buffer[0]==';' || Buffer[0]==NULLC || Buffer[0]=='\n');
    Buffer[strinstr(0,Buffer,"\n")]=NULLC;
    if (Config==1 && !Fusion) {
      if (!strinstr(0,Buffer,"[EndInitSection]")) goto LABEL_FIN_INIT;
      if (!strinstr(0,Buffer,"NbRendu")) NbRendu=atoi(Buffer+8);
      if (!strinstr(0,Buffer,"ColorCamera")) CCAMERA=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"ColorViewport")) FFOND=NbCouleurs==256 ? atoi(Buffer+14):NOIR;
      if (!strinstr(0,Buffer,"ColorTextBackground")) ZFOND=atoi(Buffer+20);
      if (!strinstr(0,Buffer,"ColorSelection")) CSELECTION=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"ColorInterface")) FOND=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"ColorAxis")) CAXE=atoi(Buffer+10);
      if (!strinstr(0,Buffer,"ColorGrid")) CGRILLE=atoi(Buffer+10);
      if (!strinstr(0,Buffer,"GrilleType")) GrilleType=atoi(Buffer+11);
      if (!strinstr(0,Buffer,"CurrentViewport")) NF=atoi(Buffer+16);
      if (!strinstr(0,Buffer,"Viewportx4")) {
        Fx4=!atoi(Buffer+11);
        bouton_fenetre(0);
      }
      if (!strinstr(0,Buffer,"DiplayAxis")) OptAxe=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"DisplayGrid")) OptGrille=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"CurrentTexture")) {
        strcpy(MatiereCourante,Buffer+15);
        nom_gif_matiere_courante();
      }

      if (!strinstr(0,Buffer,"CurrentCamera")) NumCam=atoi(Buffer+14);

      if (!strinstr(0,Buffer,"Scale-X/Y")) {
        analyse_ligne(Buffer,32);
        if (!strinstr(0,Argu[1],"F0")) Vid[0].Echelle=atof(Argu[2]);
        if (!strinstr(0,Argu[1],"F1")) Vid[1].Echelle=atof(Argu[2]);
        if (!strinstr(0,Argu[1],"F2")) Vid[2].Echelle=atof(Argu[2]);
      }

      if (!strinstr(0,Buffer,"BackgroundImageRGB")) {
        analyse_ligne(Buffer,32);
        Fond_RVB[0]=atoi(Argu[1]);
        Fond_RVB[1]=atoi(Argu[2]);
        Fond_RVB[2]=atoi(Argu[3]);
      }

      if (!strinstr(0,Buffer,"FogLayer")) {
        analyse_ligne(Buffer,32);
        j=atoi(Argu[1]);
        Fog[j].RGB[0]=atoi(Argu[2]);
        Fog[j].RGB[1]=atoi(Argu[3]);
        Fog[j].RGB[2]=atoi(Argu[4]);
        Fog[j].Ok=atoi(Argu[5]);
        Fog[j].Distance=atof(Argu[6]);
        Fog[j].Type=atoi(Argu[7]);
        Fog[j].Type=(Fog[j].Type>=1 && Fog[j].Type<=2 ? Fog[j].Type:1);
        Fog[j].Offset=atof(Argu[8]);
        Fog[j].Alt=atof(Argu[9]);
        Fog[j].TurbD=atof(Argu[10]);
        Fog[j].Turbulence=atof(Argu[11]);
      }

      if (!strinstr(0,Buffer,"Fog:")) {
        analyse_ligne(Buffer,32);
        Fog[0].RGB[0]=atoi(Argu[1]);
        Fog[0].RGB[1]=atoi(Argu[2]);
        Fog[0].RGB[2]=atoi(Argu[3]);
        Fog[0].Ok=atoi(Argu[4]);
        Fog[0].Distance=atof(Argu[5]);
        Fog[0].Type=atoi(Argu[6]);
        Fog[0].Type=(Fog[0].Type>=1 && Fog[0].Type<=2 ? Fog[0].Type:1);
        Fog[0].Offset=atof(Argu[7]);
        Fog[0].Alt=atof(Argu[8]);
        Fog[0].TurbD=atof(Argu[9]);
        Fog[0].Turbulence=atof(Argu[10]);
      }

      if (!strinstr(0,Buffer,"RayGlobal")) {
        analyse_ligne(Buffer,32);
        Global_ADC=      atof(Argu[1]);
        Global_A_Gamma=  atof(Argu[2]);
        Global_MaxTrace= atoi(Argu[3]);
        Global_MaxInter= atoi(Argu[4]);
        Global_NbWave=   atoi(Argu[5]);
        Global_Irid[_R]= atof(Argu[6]);
        Global_Irid[_V]= atof(Argu[7]);
        Global_Irid[_B]= atof(Argu[8]);
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
        strcpy(Omni[NbOmni].Flare,Argu[16]);
        test_limite_omni(NbOmni);
      }

      if (!strinstr(0,Buffer,"Area") && incr_NbArea(0)) {
        analyse_ligne(Buffer,32);
        Area[NbArea].Point[_X]=atof(Argu[2]);
        Area[NbArea].Point[_Y]=atof(Argu[3]);
        Area[NbArea].Point[_Z]=atof(Argu[4]);
        Area[NbArea].RVB[_R]=atoi(Argu[5]);
        Area[NbArea].RVB[_V]=atoi(Argu[6]);
        Area[NbArea].RVB[_B]=atoi(Argu[7]);
        Area[NbArea].Taille=atoi(Argu[8]);
        Area[NbArea].Rayon=atof(Argu[9]);
        Area[NbArea].OnOff=atoi(Argu[10]);
        Area[NbArea].Ombre=atoi(Argu[11]);
        Area[NbArea].F_Dist=atoi(Argu[12]);
        Area[NbArea].F_Power=atof(Argu[13]);
        Area[NbArea].Fade=atoi(Argu[14]);
        Area[NbArea].Atmos=atoi(Argu[15]);
        Area[NbArea].Atmos_Att=atoi(Argu[16]);
        Area[NbArea].Axis1[_X]=atof(Argu[17]);
        Area[NbArea].Axis1[_Y]=atof(Argu[18]);
        Area[NbArea].Axis1[_Z]=atof(Argu[19]);
        Area[NbArea].Axis2[_X]=atof(Argu[20]);
        Area[NbArea].Axis2[_Y]=atof(Argu[21]);
        Area[NbArea].Axis2[_Z]=atof(Argu[22]);
        Area[NbArea].Size1=atoi(Argu[23]);
        Area[NbArea].Size2=atoi(Argu[24]);
        Area[NbArea].Jitter=atoi(Argu[25]);
        Area[NbArea].Adaptive=atof(Argu[26]);
        strcpy(Area[NbArea].Flare,Argu[27]);
        test_limite_area(NbArea);
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
        Spot[NbSpot].Area=atoi(Argu[22]);
        Spot[NbSpot].Axis1[_X]=atof(Argu[23]);
        Spot[NbSpot].Axis1[_Y]=atof(Argu[24]);
        Spot[NbSpot].Axis1[_Z]=atof(Argu[25]);
        Spot[NbSpot].Axis2[_X]=atof(Argu[26]);
        Spot[NbSpot].Axis2[_Y]=atof(Argu[27]);
        Spot[NbSpot].Axis2[_Z]=atof(Argu[28]);
        Spot[NbSpot].Size1=atoi(Argu[29]);
        Spot[NbSpot].Size2=atoi(Argu[30]);
        Spot[NbSpot].Jitter=atoi(Argu[31]);
        Spot[NbSpot].Adaptive=atof(Argu[32]);
        strcpy(Spot[NbSpot].Flare,Argu[33]);
        test_limite_spot(NbSpot);
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
        strcpy(CylLight[NbCylLight].Flare,Argu[22]);
        test_limite_cyllight(NbCylLight);
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
        Camera[NbCamera].Roll=atof(Argu[15]);
      }

      if (!strinstr(0,Buffer,"Radiosity:")) {
        analyse_ligne(Buffer,32);
        Rad_OnOff=         atoi(Argu[1]);
        Rad_Type=          atoi(Argu[2]);
        Rad_Brightness=    atof(Argu[3]);
        Rad_Count=         atoi(Argu[4]);
        Rad_Dist_Max=      atof(Argu[5]);
        Rad_Err_Bound=     atof(Argu[6]);
        Rad_Gray_Threshold=atof(Argu[7]);
        Rad_Low_Err_Fact=  atof(Argu[8]);
        Rad_Min_Reuse=     atof(Argu[9]);
        Rad_Near_Count=    atoi(Argu[10]);
        Rad_Rec_Lim=       atoi(Argu[11]);
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
        Area[0].Cache=!atoi(Argu[1]);
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

    if (strinstr(0,Buffer,"Object ")==0 && Config==0) {

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
      Objet[NbObjet]->Inverse=0;
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
      Objet[NbObjet]->Special.Root=(VERTEX *) NULL;
      Objet[NbObjet]->Special.Nombre=0;

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

      if (Objet[NbObjet]->Type==SPLINE) {  // -------------- Read SPLINE
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

      if (Objet[NbObjet]->Type==SOR ||     // -------------- Read SOR
          Objet[NbObjet]->Type==LATHE ||
          Objet[NbObjet]->Type==EXTRUDE ||
          Objet[NbObjet]->Type==BEZIER) {
        j=lecture_special_scn(Fichier,MarqueObjet,NbObjet);
        if (j==0) {
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

      fgets(Buffer,256,Fichier);   // ------------- Rapide
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Rapide=(byte) atoi(Argu[2]); 
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

      fgets(Buffer,256,Fichier); // ---------------- CSG Operator
      analyse_ligne(Buffer,32);
      if (!strinstr(0,MarqueObjet,Argu[1])) {
        Objet[NbObjet]->Operator=atoi(Argu[2]);
        if (Objet[NbObjet]->Operator==OK_CSG && Fusion) {
          Objet[NbObjet]->Buffer=-NumObjetFusion;
        }
        Objet[NbObjet]->Inverse=atoi(Argu[3]);
      } else {
        goto LABEL_FIN_CHARGE_OBJET;
      }

      fgets(Buffer,256,Fichier); // ---------------- Objet
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

// -------------------------------------------------------------------------
// -- MODIF CONFIG RAYTRACER (BINAIRE) -------------------------------------
// -------------------------------------------------------------------------
void modif_cfg_raytracer(byte Num,byte Valeur) {
  Valeur=((Valeur!=0 && Valeur!=1) ? 0:Valeur);
  ConfigRaytracer[Num]=Valeur;
}

// -------------------------------------------------------------------------
// -- GENERE UN SCRIPT POUR UN RAYTRACER -----------------------------------
// -------------------------------------------------------------------------
int genere_script_raytracer(byte Niveau) {
  if (test_si_nouveau_scn()==0) return 0;
  if (pas_objet(0)) return 0;
  if (pas_lumiere(0)) {
    if (x_fenetre(CentX,CentY,GAUCHE,1,"Error|There no light(s). Continue anyway ?")) return 0;
  }
  if (pas_camera(0)) {
    if (x_fenetre(CentX,CentY,GAUCHE,1,"Error|There no camera. Continue anyway ?")) return 0;
  }
  if (!analyse_matiere_scene(NULL)) return 0;
  if (!genere_include_matiere()) return 0;
  if (!test_couleur_omni(1,NbOmni)) return 0;
  if (!test_couleur_area(1,NbArea)) return 0;
  if (!test_couleur_spot(1,NbSpot)) return 0;
  if (!test_couleur_spot(1,NbCylLight)) return 0;
  if (!genere_povray(Niveau)) return 0;
  return 1;
}

// -------------------------------------------------------------------------
// -- ANALYSE LA LIGNE DE COMMANDE DU DOS EN DEMARRAGE ---------------------
// -------------------------------------------------------------------------
void analyse_arguments (void) {
  int i;
  
  strcpy(CheminSCN,"\0");
  strcpy(FichierSCN,"NONAME.SCN");

  for (i=1;i<NbArg;i++) {
    if (strinstr(0,Arg[i],"/Ok")>=0) { EXEC1=1; NbArg--; break; }
  }

  if (DEBUG) {
    log_out(0,"Nb Args : %d",NbArg);
    for (i=0;i<NbArg;i++) {
      log_out(i,"Arg [%d] : <%s>",i,Arg[i]);
    }
  }

  if (EXEC1==0 && access(CheminLastSCN,0)==0) {
    strcpy(Arg[1],CheminLastSCN);
    NbArg=2;
  }

  if (DEBUG) {
    log_out(0,"DEBUG mode : %s",DEBUG==0 ? "NO":"YES");
    log_out(0,"OldChemin : %s%s",OldLecteur,OldChemin);
    log_out(0,"NewChemin : %s%s",NewLecteur,NewChemin);
  }

  if (NbArg>1) {
    strupr(Arg[1]);
    split_chemin(CheminSCN,Arg[1],4);
    split_chemin(FichierSCN,Arg[1],5);
    if ((i=strinstr(0,FichierSCN,"."))!=-1) FichierSCN[i]=NULLC;
    FichierSCN[9]=NULLC;
    if (CheminSCN[0]==NULLC) {
      getcwd(CheminSCN,MAXPATH);
      strcat(CheminSCN,"\\");
    }
    strcpy(Buffer,CheminSCN);
    strcat(FichierSCN,".SCN");
    strcat(Buffer,FichierSCN);
  } else {
    if (test_fichier("scene\\povlab.scn") && OptLabScn && EXEC1==1) {
      strcpy(FichierSCN,"POVLAB.SCN");
      strcpy(CheminSCN,"SCENE\\");
    }
  }

  efface_fichier_shareware();

  if (EXEC1) printf("  - Loaded scene : %s%s\n",CheminSCN,FichierSCN);
}

// -----------------------------------------------------------------------
// --------- EFFACE FICHIERS POV/INC DANS LA VERSION SHAREWARE -----------
// -----------------------------------------------------------------------
void efface_fichier_shareware(void) {
  return;
}

// -----------------------------------------------------------------------
// --------- REQUESTER SELECTION D'UN FICHIER ----------------------------
// -----------------------------------------------------------------------
char *selection_fichier(int X,int Y,char *Titre,char *Spec[]) {
  int NbSpec;
  struct find_t find_t;
  int NbF;
  int Flag=1;
  int i,Disk,Save,CurDisk;
  char tmp[MAXPATH];
  byte Champ=MAXFILE+MAXEXT;
  char *File[NB_FILE_MAX];
  struct retour_selecteur Val;

  NbSpec=atoi(Spec[0]);

  g_fenetre(X-5,Y,X+155,Y+255,Titre,AFFICHE);

  while (Flag) {
    NbF=-1;

    // ----------------------- RÇcupäre les lecteurs

    message("Searching files...");

    Save=get_disk();

    for (Disk=2;Disk<26;++Disk) {
      set_disk(Disk);

      if (Disk==get_disk()) {
        NbF++;
        File[NbF]=(char *) mem_alloc(Champ);
        strcpy(File[NbF],"[ :]");
        File[NbF][1]=Disk+65;
        message("Searching files... %d",NbF);
      }
    }

    set_disk(Save);

    // ----------------------- RÇcupäre les rÇpertoires

    if (!_dos_findfirst("*.*",0x10,&find_t)) {
      do {
        if (strcmp(find_t.name,".\0")) {
          if (NbF==NB_FILE_MAX) break;
          if (find_t.attrib==16) {
            NbF++;
            File[NbF]=(char *) mem_alloc(Champ);
            strcpy(File[NbF],"[\\");
            strcat(File[NbF],find_t.name);
            strcat(File[NbF],"]");
            message("Searching files... %d",NbF);
          }
        }
      } while(!_dos_findnext(&find_t));
    }

    //_dos_findclose(&find_t);

    // ----------------------- RÇcupäre les noms de fichiers

    for (i=1;i<=NbSpec;i++) {
      if (!_dos_findfirst(Spec[i],0x20,&find_t)) {
        do {
          if (strcmp(find_t.name,".\0")) {
            if (NbF==NB_FILE_MAX) break;
            if (find_t.attrib!=16) {
              NbF++;
              File[NbF]=(char *) mem_alloc(Champ);
              strcpy(File[NbF],strlwr(find_t.name));
            }
          }
        } while(!_dos_findnext(&find_t));
      }
    }

    _dos_findclose(&find_t);

    tri_tableau(File,NbF+1,sizeof(File[0]));

    affiche_donnees();

    strcpy(Buffer,"");
    for (i=0;i<=NbSpec;i++) {
      strcat(Buffer,Spec[i]);
      strcat(Buffer," ");
    }

    message("%d file(s) found [%s] in %s",NbF,Buffer,getcwd(NULL,0));

    init_selecteur(0,X,Y,12,NbF+1,File,12);
    affiche_selecteur(0);

    bouton_dialog(X,X+155,Y+255,1,1);

    while (1) {
      Val=test_selecteur(0);
      Flag=Val.Num;
      if (Val.Ok==13) { i=0; break; }
      if (Val.Ok==27) { i=1; break; }
      if ((i=bouton_dialog(X,X+155,Y+255,0,1))!=-1) break;
    }

    if (i!=0) { Flag=-4; break; }

    if ((i=File[Flag][0])=='[') {
       switch(File[Flag][1]) {
         case '\\':
           File[Flag][strlen(File[Flag])-1]=NULLC;
           chdir(File[Flag]+2);
           break;
         default:
           CurDisk=get_disk();
           set_disk(File[Flag][1]-65);

           if (get_disk()!=(File[Flag][1]-65) || DiskError) {
             f_erreur("Disk i/o error : POVLAB can't access to drive %c: !",File[Flag][1]);
             set_disk(CurDisk);
           }

           DiskError=0;
           break;
       }
       Flag=1;
       for (i=0;i<=NbF;i++) {
         mem_free(File[i],Champ);
       }
    } else break;
  }
                   
  if (Flag==-4) {
    strcpy(tmp," ");
    tmp[0]=27;
    tmp[1]=27;
    tmp[2]=NULLC;
  } else {
    getcwd(tmp,MAXPATH);
    strcat(tmp,"\\");
    strcat(tmp,File[Flag]);
  }

  g_fenetre(X-5,Y,X+160,Y+225,Titre,EFFACE);

  for (i=0;i<=NbF;i++) {
    mem_free(File[i],Champ);
  }

  affiche_donnees();
  
  set_disk(NewLecteur[0]-65);
  chdir(NewChemin);
  return (char *) strupr(tmp);
}

// -------------------------------------------------------------------------
// -- SAUVE LA SCENE EN QUITTANT -------------------------------------------
// -------------------------------------------------------------------------
int sauve_scene_en_quittant(void) {
  int i;

  if (pas_objet(0) && pas_lumiere(0) && pas_camera(0)) return 1;
  if (OptSauve) { if (!sauve_fichier(1,0)) return 0; else return 1; }
  if (OptBeep) beep();
  strcpy(StrBoite[0],"SAVE THE CURRENT SCENE");
  sprintf(StrBoite[1],"Do you want to save the scene %s",FichierSCN);
  sprintf(StrBoite[2],"before exiting %s.%s ?",LOGICIEL,VERSION);
  i=g_boite_ONA(CentX,CentY,2,CENTRE,2);
  if (i==0) if (!sauve_fichier(1,0)) return 0;
  if (i==2) return 0;
  return 1;
}

// -------------------------------------------------------------------------
// -- TEST SI FICHIER = NOUVEAU.SCN ----------------------------------------
// -------------------------------------------------------------------------
int test_si_nouveau_scn(void) {
  while (strinstr(0,FichierSCN,"NONAME.SCN")==0) {
    if (!renomme_scene("RENAME SCENE")) return 0;
  }
  return 1;
}

// -------------------------------------------------------------------------
// -- SAUVEGARDE TEMPORAIRE DES DONNEES ------------------------------------
// -------------------------------------------------------------------------
byte sauve_temporaire(void) {
  /*
  register int i;
  FILE *Fichier;

  Fichier=fopen("MODELEUR.BCK","w");

  for (i=1;i<=NbCamera;i++) fwrite(&Camera[i],sizeof(Camera[i]),1,Fichier);
  for (i=1;i<=NbOmni;i++) fwrite(&Omni[i],sizeof(Omni[i]),1,Fichier);
  for (i=1;i<=NbSpot;i++) fwrite(&Spot[i],sizeof(Spot[i]),1,Fichier);
  for (i=1;i<=NbCylLight;i++) fwrite(&CylLight[i],sizeof(CylLight[i]),1,Fichier);
  for (i=1;i<=NbArea;i++) fwrite(&Area[i],sizeof(Area[i]),1,Fichier);
  for (i=1;i<=NbObjet;i++) fwrite(&Objet[i],sizeof(VOLUME),1,Fichier);

  fclose(Fichier);
  */
  return 1;
}


