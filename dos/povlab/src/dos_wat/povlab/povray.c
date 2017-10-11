/* ---------------------------------------------------------------------------
*  POVRAY.C
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
#include "VERSIONC.H"
#include <SYS\STAT.H>
#include <IO.H>
#include <FCNTL.H>
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <STDARG.H>
#include <STRING.H>
#include <TIME.H>
#include <GRAPH.H>
#include <DIRECT.H>
#include <STDIO.H>
#include <DOS.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"

byte OptVideo='G';
byte NbTab=2;

// -------------------------------------------------------------------------
// -- RETOURNE UNE FORMATAGE DE TABULATION ---------------------------------
// -------------------------------------------------------------------------
char *tabs(byte n) {
  char String[50];
  register int i;

  for (i=0;i<=n;i++) String[i]=32;

  String[n]=NULLC;
  return (char *) String;
}

// -------------------------------------------------------------------------
// -- SORT UNE LIGNE DIRECTEMENT DANS LE FICHIER ---------------------------
// -------------------------------------------------------------------------
void outl(FILE *Fichier,int T,char *String,...) {
  va_list parametre;
  char Sortie[256];

  parse_ascii_watcom(String);

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  fprintf(Fichier,"%s%s",tabs(T*NbTab),Sortie);
}

// -------------------------------------------------------------------------
// -- ETUDIE Operator -----------------------------------------------------------
// -------------------------------------------------------------------------
void enregistre_csg(int N,FILE *Fichier,char *Nom) {
  if (Objet[N]->CSG!=PAS_CSG ||
      Objet[N]->Operator!=PAS_CSG ||
      Objet[N]->LooksLike.Nb) {
    outl(Fichier,0,"#declare OBJ%04d = %s {\n",N,Nom);
  } else {
    outl(Fichier,0,"%s {\n",Nom);
  }
}

// -------------------------------------------------------------------------
// -- CALCULE Operator ----------------------------------------------------------
// -------------------------------------------------------------------------
void calcule_csg(FILE *Fichier) {
  register int i,j,k,Last;
  char BOperator[5];
  char *Prefix[5]={"OBJ","DIF","UNI","INT","MER"};

  for (i=1;i<=NbObjet;i++) {
    strcpy(BOperator,"0000");
    Last=0;
    k=1;  // ----------------------------------- Difference
    if (Objet[i]->Operator==OK_CSG) {
      if ((Objet[i]->Cache && GenerateHidden) || Objet[i]->Cache==0) {
        for (j=1;j<=NbObjet;j++) {
          if (Objet[j]->Operator==DIFFERE
              && i!=j
              && Objet[j]->CSG==i
              && Objet[j]->Cache==0) {
            if (k) {
              fprintf(Fichier,"#declare DIF%04d = difference {\n",i);
              outl(Fichier,1,"object { OBJ%04d }\n",i);
              k=0;
            }
            if (Last) {
              outl(Fichier,1,"object { %s%04d }\n",Prefix[Last],i);
            } else {
              outl(Fichier,1,"object { OBJ%04d %s}\n",j,Objet[j]->Inverse ? "inverse ":"");
            }
          }
        }
      }
      if (!k) { fprintf(Fichier,"}\n\n"); BOperator[0]='1'; Last=1; }
    }

    k=1; // ----------------------------------- Union
    if (Objet[i]->Operator==OK_CSG) {
      if ((Objet[i]->Cache && GenerateHidden) || Objet[i]->Cache==0) {
        for (j=1;j<=NbObjet;j++) {
          if (Objet[j]->Operator==UNION
              && i!=j
              && Objet[j]->CSG==i
              && Objet[j]->Cache==0) {
            if (k) {
              fprintf(Fichier,"#declare UNI%04d = union {\n",i);
              if (Last) {
                outl(Fichier,1,"object { %s%04d }\n",Prefix[Last],i);
              } else {
                outl(Fichier,1,"object { OBJ%04d }\n",i);
              }
              k=0;
            }
            outl(Fichier,1,"object { OBJ%04d %s}\n",j,Objet[j]->Inverse ? "inverse ":"");
          }
        }
      }
      if (!k) { fprintf(Fichier,"}\n\n"); BOperator[1]='1'; Last=2; }
    }

    k=1;  // ----------------------------------- Intersection
    if (Objet[i]->Operator==OK_CSG) {
      if ((Objet[i]->Cache && GenerateHidden) || Objet[i]->Cache==0) {
        for (j=1;j<=NbObjet;j++) {
          if (Objet[j]->Operator==INTERSE
              && i!=j
              && Objet[j]->CSG==i
              && Objet[j]->Cache==0) {
            if (k) {
              fprintf(Fichier,"#declare INT%04d = intersection {\n",i);
              if (Last) {
                outl(Fichier,1,"object { %s%04d }\n",Prefix[Last],i);
              } else {
                outl(Fichier,1,"object { OBJ%04d }\n",i);
              }
              k=0;
            }
            outl(Fichier,1,"object { OBJ%04d %s}\n",j,Objet[j]->Inverse ? "inverse ":"");
          }
        }
      }
      if (!k) { fprintf(Fichier,"}\n\n"); BOperator[2]='1'; Last=3; }
    }

    k=1;  // ----------------------------------- Merge
    if (Objet[i]->Operator==OK_CSG && Objet[i]->Cache==0) {
      for (j=1;j<=NbObjet;j++) {
        if (Objet[j]->Operator==MERGE
            && i!=j
            && Objet[j]->CSG==i
            && Objet[j]->Cache==0) {
          if (k) {
            fprintf(Fichier,"#declare MER%04d = merge {\n",i);
            if (Last) {
              outl(Fichier,1,"object { %s%04d }\n",Prefix[Last],i);
            } else {
              outl(Fichier,1,"object { OBJ%04d }\n",i);
            }
            k=0;
          }
          outl(Fichier,1,"object { OBJ%04d %s}\n",j,Objet[j]->Inverse ? "inverse ":"");
        }
      }
      if (!k) { fprintf(Fichier,"}\n\n"); BOperator[3]='1'; Last=4; }
    }

    if (strinstr(0,BOperator,"0000")!=0) {
      for (k=3;k>=0;k--) {
        if (k==3 && BOperator[k]=='1') { fprintf(Fichier,"object { MER%04d }\n\n",i); break; }
        if (k==2 && BOperator[k]=='1') { fprintf(Fichier,"object { INT%04d }\n\n",i); break; }
        if (k==1 && BOperator[k]=='1') { fprintf(Fichier,"object { UNI%04d }\n\n",i); break; }
        if (k==0 && BOperator[k]=='1') { fprintf(Fichier,"object { DIF%04d }\n\n",i); break; }
      }
    }
  }
}

// -------------------------------------------------------------------------
// -- GENERE LA MATIERE AU FORMAT POVRAY -----------------------------------
// -------------------------------------------------------------------------
static void enregistre_matiere(int N,FILE *Fichier,int L) {
  register DBL SX,SY,SZ,RX,RY,RZ,TX,TY,TZ;
  byte i,n=0;

  if (analyse_matiere_scene(N)) {
    SX=Objet[N]->MS[0];
    SY=Objet[N]->MS[1];
    SZ=Objet[N]->MS[2];
    RX=Objet[N]->MR[0];
    RY=Objet[N]->MR[1];
    RZ=Objet[N]->MR[2];
    TX=Objet[N]->MT[0];
    TY=Objet[N]->MT[1];
    TZ=Objet[N]->MT[2];

    outl(Fichier,L,"texture {\n");

    if (Objet[N]->Map[0].On) {
      ecrit_mapping(Fichier,L+1,N);
    } else {
      if (strinstr(0,Objet[N]->Matiere,"Default")>=0) {
        outl(Fichier,L+1,"pigment { color rgb <%.2g,%.2g,%.2g> }\n",
        (DBL) Palette[Objet[N]->Couleur][0]/255,
        (DBL) Palette[Objet[N]->Couleur][1]/255,
        (DBL) Palette[Objet[N]->Couleur][2]/255);
      } else {
        outl(Fichier,L+1,"%s\n",Objet[N]->Matiere);
      }
    }

    ecrit_bumping(Fichier,L+1,N);

    if (SX!=1.0 || SY!=1.0 || SZ!=1.0) {
      outl(Fichier,L+1,"scale <%.4g,%.4g,%.4g>\n",fabs(SX),fabs(SY),fabs(SZ));
    }
    if (RX || RY || RZ) {
      outl(Fichier,L+1,"rotate <%.4g,%.4g,%.4g>\n",RX,-RY,-RZ);
    }
    if (TX || TY || TZ) {
      outl(Fichier,L+1,"translate <%.4g,%.4g,%.4g>\n",TX,-TY,-TZ);
    }
    
    for (i=0;i<=12;i++) if (Objet[N]->BitTexture[i]) { n=1; break; }
    if (n) {
      outl(Fichier,L+1,"finish {\n");
      if (Objet[N]->BitTexture[ 0]) outl(Fichier,L+2,"ior %.2f\n",Objet[N]->Ior);
      if (Objet[N]->BitTexture[ 1]) outl(Fichier,L+2,"refraction %.2f\n",(DBL)Objet[N]->Refraction/100);
      if (Objet[N]->BitTexture[ 2]) outl(Fichier,L+2,"reflection %.2f\n",(DBL)Objet[N]->Reflexion/100);
      if (Objet[N]->BitTexture[ 3]) outl(Fichier,L+2,"diffuse %.2f\n",(DBL)Objet[N]->Diffusion/100);
      if (Objet[N]->BitTexture[ 4]) outl(Fichier,L+2,"ambient %.2f\n",(DBL)Objet[N]->Ambiance/100);
      if (Objet[N]->BitTexture[ 5]) outl(Fichier,L+2,"crand %.2f\n",(DBL)Objet[N]->Crand/100);
      if (Objet[N]->BitTexture[ 6]) outl(Fichier,L+2,"phong %.2f\n",(DBL)Objet[N]->Phong/100);
      if (Objet[N]->BitTexture[ 7]) outl(Fichier,L+2,"phong_size %.2f\n",(DBL)Objet[N]->PSize);
      if (Objet[N]->BitTexture[ 8]) outl(Fichier,L+2,"caustics %.2f\n",(DBL)Objet[N]->Caustics/100);
      if (Objet[N]->BitTexture[ 9]) outl(Fichier,L+2,"fade_distance %.2f\n",(DBL)Objet[N]->Fade_D/100);
      if (Objet[N]->BitTexture[10]) outl(Fichier,L+2,"fade_power %.2f\n",(DBL)Objet[N]->Fade_P/100);
      if (Objet[N]->BitTexture[11]) outl(Fichier,L+2,"roughness %.2f\n",(DBL)Objet[N]->Rough/100);
      if (Objet[N]->BitTexture[12]) outl(Fichier,L+2,"brilliance %.2f\n",(DBL)Objet[N]->Brilli/100);
      if (Objet[N]->BitTexture[13]) outl(Fichier,L+2,"specular %.2f\n",(DBL)Objet[N]->Specular/100);
      outl(Fichier,L+1,"}\n");
    }
    outl(Fichier,L,"}\n");
  } else {
    outl(Fichier,L,"texture { Default }\n");
    if (!Objet[N]->Ombre) outl(Fichier,L,"no_shadow\n");
  }
  if (Objet[N]->Halo &&
      Objet[N]->Type!=BLOB &&
      Objet[N]->Type!=BLOBC) outl(Fichier,L,"hollow\n");
  if (!Objet[N]->Ombre) outl(Fichier,L,"no_shadow\n");
}

// -------------------------------------------------------------------------
// -- AFFICHE LES DONNEES POUR LE BLOC -------------------------------------
// -------------------------------------------------------------------------
void print_f_SRT(FILE *Fichier,int N,DBL SX,DBL SY,DBL SZ,DBL RX,DBL RY,DBL RZ,DBL TX,DBL TY,DBL TZ,byte L) {

  if (Objet[N]->ScaleOM) enregistre_matiere(N,Fichier,L);

  if (SX!=1.0 || SY!=1.0 || SZ!=1.0) {
    SX=SX<0.0001 ? 0.0001:SX;
    SY=SY<0.0001 ? 0.0001:SY;
    SZ=SZ<0.0001 ? 0.0001:SZ;
    outl(Fichier,L,"scale <%.4g,%.4g,%.4g>\n",SX,SY,SZ);
  }
  if (RX || RY || RZ) {
    outl(Fichier,L,"rotate <%.4g,%.4g,%.4g>\n",-RX,-RY,RZ);
  }
  if (TX || TY || TZ) {
    outl(Fichier,L,"translate <%.4g,%.4g,%.4g>\n",TX,-TY,-TZ);
  }

  if (!Objet[N]->ScaleOM) enregistre_matiere(N,Fichier,L);
}

// -------------------------------------------------------------------------
// -- GENERE LES SOURCES DE LUMIERE ----------------------------------------
// -------------------------------------------------------------------------
void genere_lumieres(FILE *Fichier,byte Type,byte LLike) {
  register int i,j;

  message("Writing light sources");

  if (Type==OMNI) {
    for (i=1;i<=NbOmni;i++) {
      if (((Omni[i].OnOff && GenerateHidden) || Omni[i].OnOff==1) && si_omni_looks_like(i)==LLike) {
        fprintf(Fichier,"light_source { // #%02d\n",i);
        outl(Fichier,1,"<%.4g,%.4g,%.4g>\n",Omni[i].Point[_X],-Omni[i].Point[_Y],-Omni[i].Point[_Z]);
        outl(Fichier,1,"color rgb <%.4g,%.4g,%.4g>\n",(DBL) Omni[i].RVB[0]/255,
                                                      (DBL) Omni[i].RVB[1]/255,
                                                      (DBL) Omni[i].RVB[2]/255);
        if (Omni[i].Fade) {
          outl(Fichier,1,"fade_distance %.4g\n",Omni[i].F_Dist);
          outl(Fichier,1,"fade_power %d\n",Omni[i].F_Power);
        }
        if (Omni[i].Atmos_Att) outl(Fichier,1,"atmospheric_attenuation 1\n");
        if (!Omni[i].Atmos) outl(Fichier,1,"atmosphere off\n");
        if (!Omni[i].Ombre) outl(Fichier,1,"shadowless\n");
        if (LLike) {
          outl(Fichier,1,"looks_like {\n");
          for (j=1;j<=NbObjet;j++) {
            if (Objet[j]->LooksLike.Nb==i && Objet[j]->LooksLike.Light==OMNI) {
              outl(Fichier,2,"object { OBJ%04d translate <%.4g,%.4g,%.4g> }\n",
                   j,
                   -Omni[i].Point[_X],
                   Omni[i].Point[_Y],
                   Omni[i].Point[_Z]);
            }
          }
          outl(Fichier,1,"}\n");
        }
        fprintf(Fichier,"}\n\n");
        if (strinstr(0,Omni[i].Flare,".FLR")>0) {
          write_lens_flare_code(Fichier,
                                Omni[i].Point[_X],
                                -Omni[i].Point[_Y],
                                -Omni[i].Point[_Z],
                                Omni[i].Flare);
        }
      }
    }
  }

  if (Type==AREA) {
    for (i=1;i<=NbArea;i++) {
      if (((Area[i].OnOff && GenerateHidden) || Area[i].OnOff==1) && si_area_looks_like(i)==LLike) {
        fprintf(Fichier,"light_source {\n");
        outl(Fichier,1,"<%.4g,%.4g,%.4g>\n",Area[i].Point[_X],-Area[i].Point[_Y],-Area[i].Point[_Z]);
        outl(Fichier,1,"color rgb <%.4g,%.4g,%.4g>\n",(DBL) Area[i].RVB[0]/255,
                                                      (DBL) Area[i].RVB[1]/255,
                                                      (DBL) Area[i].RVB[2]/255);
        outl(Fichier,1,"area_light <%.3f,%.3f,%.3f>,<%.3f,%.3f,%.3f>,%d,%d\n",
                           Area[i].Axis1[_X],Area[i].Axis1[_Y],Area[i].Axis1[_Z],
                           Area[i].Axis2[_X],Area[i].Axis2[_Y],Area[i].Axis2[_Z],
                           Area[i].Size1,Area[i].Size2);
        outl(Fichier,1,"adaptive %.4g\n",Area[i].Adaptive);
        if (Area[i].Jitter) outl(Fichier,1,"jitter\n");
        if (Area[i].Fade) {
          outl(Fichier,1,"fade_distance %.4g\n",Area[i].F_Dist);
          outl(Fichier,1,"fade_power %d\n",Area[i].F_Power);
        }
        if (Area[i].Atmos_Att) outl(Fichier,1,"atmospheric_attenuation 1\n");
        if (!Area[i].Atmos) outl(Fichier,1,"atmosphere off\n");
        if (!Area[i].Ombre) outl(Fichier,1,"shadowless\n");
        if (LLike) {
          outl(Fichier,1,"looks_like {\n");
          for (j=1;j<=NbObjet;j++) {
            if (Objet[j]->LooksLike.Nb==i && Objet[j]->LooksLike.Light==AREA) {
              outl(Fichier,2,"object { OBJ%04d translate <%.4g,%.4g,%.4g> }\n",
                   j,
                   -Area[i].Point[_X],
                   Area[i].Point[_Y],
                   Area[i].Point[_Z]);
            }
          }
          outl(Fichier,1,"}\n");
        }
        fprintf(Fichier,"}\n\n");
        if (strinstr(0,Area[i].Flare,".FLR")>0) {
          write_lens_flare_code(Fichier,
                                Area[i].Point[_X],
                                -Area[i].Point[_Y],
                                -Area[i].Point[_Z],
                                Area[i].Flare);
        }
      }
    }
  }

  if (Type==SPOT) {
    for (i=1;i<=NbSpot;i++) {
      if ((Spot[i].OnOff && GenerateHidden) || Spot[i].OnOff==1) {
        fprintf(Fichier,"light_source { // #%02d\n",i);
        outl(Fichier,1,"<%.4g,%.4g,%.4g>\n",Spot[i].OX,-Spot[i].OY,-Spot[i].OZ);
        outl(Fichier,1,"color rgb <%.4g,%.4g,%.4g>\n",(DBL) Spot[i].RVB[0]/255,
                                                      (DBL) Spot[i].RVB[1]/255,
                                                      (DBL) Spot[i].RVB[2]/255);
        outl(Fichier,1,"spotlight // #%02d\n",i);
        outl(Fichier,1,"point_at <%.4g,%.4g,%.4g>\n",Spot[i].CX,-Spot[i].CY,-Spot[i].CZ);
        outl(Fichier,1,"falloff %.4g\n",Spot[i].Angle1);
        outl(Fichier,1,"radius %.4g\n",Spot[i].Angle2);
        if (Spot[i].Fade) {
          outl(Fichier,1,"fade_distance %.4g\n",Spot[i].F_Dist);
          outl(Fichier,1,"fade_power %d\n",Spot[i].F_Power);
        }
        if (Spot[i].Atmos_Att) outl(Fichier,1,"atmospheric_attenuation 1\n");
        if (!Spot[i].Atmos) outl(Fichier,1,"atmosphere off\n");
        if (!Spot[i].Ombre) outl(Fichier,1,"shadowless\n");
        if (Spot[i].Area) {
          outl(Fichier,1,"area_light <%.3f,%.3f,%.3f>,<%.3f,%.3f,%.3f>,%d,%d\n",
                             Spot[i].Axis1[_X],Spot[i].Axis1[_Y],Spot[i].Axis1[_Z],
                             Spot[i].Axis2[_X],Spot[i].Axis2[_Y],Spot[i].Axis2[_Z],
                             Spot[i].Size1,Spot[i].Size2);
          outl(Fichier,1,"adaptive %.4g\n",Spot[i].Adaptive);
          if (Spot[i].Jitter) outl(Fichier,1,"jitter\n");
        }
        fprintf(Fichier,"}\n\n");
        if (strinstr(0,Spot[i].Flare,".FLR")>0) {
          write_lens_flare_code(Fichier,
                                Spot[i].OX,
                                -Spot[i].OY,
                                -Spot[i].OZ,
                                Spot[i].Flare);
        }
      }
    }
  }


  if (Type==CYLLIGHT) {
    for (i=1;i<=NbCylLight;i++) {
      if ((CylLight[i].OnOff && GenerateHidden) || CylLight[i].OnOff==1) {
        fprintf(Fichier,"light_source { // #%02d\n",i);
        outl(Fichier,1,"<%.4g,%.4g,%.4g>\n",CylLight[i].OX,-CylLight[i].OY,-CylLight[i].OZ);
        outl(Fichier,1,"color rgb <%.4g,%.4g,%.4g>\n",(DBL) CylLight[i].RVB[0]/255,
                                                      (DBL) CylLight[i].RVB[1]/255,
                                                      (DBL) CylLight[i].RVB[2]/255);
        outl(Fichier,1,"cylinder // #%02d\n",i);
        outl(Fichier,1,"point_at <%.4g,%.4g,%.4g>\n",CylLight[i].CX,-CylLight[i].CY,-CylLight[i].CZ);
        outl(Fichier,1,"falloff %.4g\n",CylLight[i].Angle1);
        outl(Fichier,1,"radius %.4g\n",CylLight[i].Angle2);
        if (CylLight[i].Fade) {
          outl(Fichier,1,"fade_distance %.4g\n",CylLight[i].F_Dist);
          outl(Fichier,1,"fade_power %d\n",CylLight[i].F_Power);
        }
        if (CylLight[i].Atmos_Att) outl(Fichier,1,"atmospheric_attenuation 1\n");
        if (!CylLight[i].Atmos) outl(Fichier,1,"atmosphere off\n");
        if (!CylLight[i].Ombre) outl(Fichier,1,"shadowless\n");
        fprintf(Fichier,"}\n\n");
        if (strinstr(0,CylLight[i].Flare,".FLR")>0) {
          write_lens_flare_code(Fichier,
                                 CylLight[i].OZ,
                                -CylLight[i].OY,
                                -CylLight[i].OZ,
                                 CylLight[i].Flare);
        }
      }
    }
  }
}

// -------------------------------------------------------------------------
// -- GENERE UN FICHIER AU FORMAT POV-RAY ----------------------------------
// -------------------------------------------------------------------------
int genere_povray (byte Niveau) {
  register int i,Grp,j;
  FILE *Fichier;
  char Buffer[MAXPATH+MAXFILE+MAXEXT],Heure[9],Date[40];
  DBL X,Z;
  Vecteur F,V;
  char FichierINC[MAXPATH];

  NbRendu+=1;
  get_date(Date);
  get_heure(Heure);

  if (!test_fichier(CheminPOVSCN) || !test_fichier(CheminIMAGE)) {
    beep_erreur();
    strcpy(StrBoite[0],"Invalid path");
    strcpy(StrBoite[1],"The dir for POV-Ray output files isn't valid !");
    strcpy(StrBoite[2],"Please, check your config file");
    strcpy(StrBoite[3],"in the sub-path SYSTEM :");
    sprintf(StrBoite[4],"POV : %s",CheminPOVSCN);
    sprintf(StrBoite[5],"IMG : %s",CheminIMAGE);
    g_boite_ONA(CentX,CentY,5,CENTRE,0);
    return 0;
  }

  strcpy(Buffer,CheminPOVSCN);
  strcat(Buffer,"\\");
  strcat(Buffer,FichierSCN);
  i=strinstr(0,Buffer,".SCN");
  Buffer[i]=NULLC;
  strcat(Buffer,".POV");

  GMouseOff();
  message("Opening file %s",Buffer);

  Fichier=fopen(Buffer,"w+t");
  if (Fichier==NULL) {
    f_erreur("Can't open the file %s",Buffer);
    return 0;
  }

  f_jauge(9,AFFICHE,0,0,"Generating");

  fprintf(Fichier,"// %s version %s - Scene for raytracing.\n",NomLogiciel,VerLogiciel);
  fprintf(Fichier,"// (C) 1994-%s, Denis Olivier.\n",RES_COPY[4]);
  fprintf(Fichier,"// All rights reserved.\n");
  fprintf(Fichier,"// Generated for POV-Ray (C) POV-Team, USA.\n");
  fprintf(Fichier,"// Date=%s.\n// Time=%s.\n",Date,Heure);
  fprintf(Fichier,"// user_name=%s\n\n",NomUtilisateur);
  fprintf(Fichier,"// Render # %d\n\n",NbRendu);


  if (test_fichier(CheminUserINC) &&
      CheminUserINC[0]!=NULLC &&
      strlen(CheminUserINC)) {
     fprintf(Fichier,"#include \"%s\"\n",CheminUserINC);
  }

  strcpy(FichierINC,CheminPOVSCN);
  strcat(FichierINC,"\\");
  strcat(FichierINC,FichierSCN);
  i=strinstr(0,FichierINC,".SCN");
  FichierINC[i]=NULLC;
  strcat(FichierINC,".INC");

  fprintf(Fichier,"#include \"%s\"\n\n",FichierINC);

  fprintf(Fichier,"global_settings {\n");
    outl(Fichier,1,"adc_bailout %.4g\n",Global_ADC);
    outl(Fichier,1,"// ambient_light <%.4g,%.4g,%.4g>\n",1.0,1.0,1.0);
    outl(Fichier,1,"assumed_gamma %.4g\n",Global_A_Gamma);
    outl(Fichier,1,"irid_wavelength rgb <%.4g,%.4g,%.4g>\n",Global_Irid[_R],
                                                      Global_Irid[_V],
                                                      Global_Irid[_B]);
    outl(Fichier,1,"max_intersections %d\n",Global_MaxInter);
    outl(Fichier,1,"max_trace_level %d\n",Global_MaxTrace);
    outl(Fichier,1,"number_of_waves %d\n",Global_NbWave);
    if (Rad_OnOff) {
      outl(Fichier,1,"radiosity {\n");
      outl(Fichier,2,"count %d\n",Rad_Count);
      outl(Fichier,2,"error_bound %.4g\n",Rad_Err_Bound);
      outl(Fichier,2,"gray_threshold %.4g\n",Rad_Gray_Threshold);
      outl(Fichier,2,"distance_maximum %.4g\n",Rad_Dist_Max);
      outl(Fichier,2,"low_error_factor %.4g\n",Rad_Low_Err_Fact);
      outl(Fichier,2,"nearest_count %d\n",Rad_Near_Count);
      outl(Fichier,2,"minimum_reuse %.4g\n",Rad_Min_Reuse);
      outl(Fichier,2,"brightness %.4g\n",Rad_Brightness);
      outl(Fichier,2,"recursion_limit %d\n",Rad_Rec_Lim);
      outl(Fichier,1,"}\n\n");
    }
  fprintf(Fichier,"}\n\n");

  if (Atmos_OnOff) {
  fprintf(Fichier,"atmosphere {\n");
      outl(Fichier,1,"type %d\n",Atmos_Type);
      outl(Fichier,1,"distance %.4g\n",Atmos_Dist);
      outl(Fichier,1,"scattering %.4g\n",Atmos_Scatt);
      outl(Fichier,1,"eccentricity %.4g\n",Atmos_Eccent);
      outl(Fichier,1,"samples %d\n",Atmos_Samples);
      outl(Fichier,1,"jitter %.4g\n",Atmos_Jitter);
      outl(Fichier,1,"aa_threshold %.4g\n",Atmos_aa_t);
      outl(Fichier,1,"aa_level %d\n",Atmos_aa_l);
      outl(Fichier,1,"color rgb <%.4g,%.4g,%.4g>\n",(DBL) Atmos_RGB[_R]/0xFF,
                                                    (DBL) Atmos_RGB[_V]/0xFF,
                                                    (DBL) Atmos_RGB[_B]/0xFF);
    fprintf(Fichier,"}\n\n");
  }

  fprintf(Fichier,"// ---- Camera and environment.\n\n");

  fprintf(Fichier,"#declare location_vector = <%.4g,%.4g,%.4g>\n",Camera[NumCam].OX,-Camera[NumCam].OY,-Camera[NumCam].OZ);
  fprintf(Fichier,"#declare look_at_vector  = <%.4g,%.4g,%.4g>\n\n",Camera[NumCam].CX,-Camera[NumCam].CY,-Camera[NumCam].CZ);

  fprintf(Fichier,"// ---- if Lens Flare used.\n\n");

  fprintf(Fichier,"#declare cam_loc = location_vector\n");
  fprintf(Fichier,"#declare lookat = look_at_vector\n");
  fprintf(Fichier,"#declare sky_vect = <0,1,0>\n\n");
   
  fprintf(Fichier,"camera {\n");
  outl(Fichier,1,"location location_vector\n");
  outl(Fichier,1,"direction <0,0,%.4g>\n",calcule_fov(NumCam));
  if (ConfigRaytracer[17]) {
    outl(Fichier,1,"right <%s,0,0>\n","1.6666");
  }
  if (ConfigRaytracer[15]) {
    outl(Fichier,1,"right <%.4g,0,0>\n",(DBL) ResolutionX/ResolutionY);
  }

  vect_init(V,0,1,0);
  vect_rotate(F,V,_Z,-Camera[NumCam].Roll);
  outl(Fichier,1,"sky <%.4g,%.4g,%.4g>\n",F[_X],F[_Y],F[_Z]);

  if (!ConfigRaytracer[17] && !ConfigRaytracer[15]) {
    outl(Fichier,1,"right <1.333333,0,0>\n");
  }
  outl (Fichier,1,"look_at look_at_vector\n");
  
  if (Camera[NumCam].F_Blur) {
    outl(Fichier,1,"aperture %.4g\n",Camera[NumCam].Aperture);
    vect_init(F,Camera[NumCam].OX-(+Camera[NumCam].CX),
               -Camera[NumCam].OY-(-Camera[NumCam].CY),
               -Camera[NumCam].OZ-(-Camera[NumCam].CZ));
    vect_normalize(F);
    vect_scale(F,F,fabs(Camera[NumCam].ProChamp));
    outl(Fichier,1,"focal_point <%.4g,%.4g,%.4g>\n", Camera[NumCam].OX-F[_X],
                                                    -Camera[NumCam].OY-F[_Y],
                                                    -Camera[NumCam].OZ-F[_Z]);
    outl(Fichier,1,"blur_samples %.4g\n",Camera[NumCam].Samples);
    outl(Fichier,1,"variance %.4g\n",Camera[NumCam].Variance);
    outl(Fichier,1,"confidence %.4g\n",Camera[NumCam].Confidence);
  }

  fprintf(Fichier,"}\n\n");

  fprintf(Fichier,"background { color rgb <%.4g,%.4g,%.4g> }\n\n",(DBL) Fond_RVB[0]/255,
                                                                  (DBL) Fond_RVB[1]/255,
                                                                  (DBL) Fond_RVB[2]/255);
  genere_lumieres(Fichier,OMNI,0);
  genere_lumieres(Fichier,CYLLIGHT,0);
  genere_lumieres(Fichier,AREA,0);
  genere_lumieres(Fichier,SPOT,0);

  for (i=0;i<=4;i++) {
    if (Fog[i].Ok) {
      fprintf(Fichier,"fog { // ---- Layer #%d\n",i);
      outl(Fichier,1,"fog_type %d\n",Fog[i].Type);
      outl(Fichier,1,"color rgb <%.4g,%.4g,%.4g>\n",(DBL) Fog[i].RGB[0]/255,
                                                    (DBL) Fog[i].RGB[1]/255,
                                                    (DBL) Fog[i].RGB[2]/255);
      outl(Fichier,1,"distance %.4g\n",Fog[i].Distance);
      if (Fog[i].Type==2) {
        outl(Fichier,1,"fog_offset %.4g\n",Fog[i].Offset);
        outl(Fichier,1,"fog_alt %.4g\n",Fog[i].Alt);
        outl(Fichier,1,"turb_depth %.4g\n",Fog[i].TurbD);
        outl(Fichier,1,"turbulence %.4g\n",Fog[i].Turbulence);
      }
      fprintf(Fichier,"}\n\n");
    }
  }

  fprintf(Fichier,"#declare Default = texture {\n");
  outl(Fichier,1,"pigment { color rgb <1,1,1> }\n");
  outl(Fichier,1,"finish { phong 1.0 }\n");
  fprintf(Fichier,"}\n\n");

  // ---------------------------- Enregistrement blob
  Grp=0;
  while ((Grp=groupe_suivant(Grp))) {
    fprintf(Fichier,"blob {\n");
    outl(Fichier,1,"threshold %.4g\n",quel_seuil(Grp));
    for (i=1;i<=NbObjet;i++) {
      if (sequence_sortie()) { goto LABEL_FIN_POV; }
      if (Objet[i]->Type==BLOB && Objet[i]->P[2]==(DBL) Grp) {
        j=i;
        message("Generate blob for object nø%05d/%d : %s",i,NbObjet,Objet[i]->Nom);
        if ((Objet[i]->Cache && GenerateHidden) || Objet[i]->Cache==0) {
          outl(Fichier,1,"sphere {\n");
          outl(Fichier,2,"<0,0,0>,1,\n");
          outl(Fichier,2,"%.4g\n",Objet[i]->P[0]);
          print_f_SRT(Fichier,i,
                      Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                      Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                      Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],2);
          fprintf(Fichier,"  }\n\n");
        }
      }
      if (Objet[i]->Type==BLOBC && Objet[i]->P[2]==(DBL) Grp) {
        j=i;
        message("Generate blob for object nø%05d/%d : %s",i,NbObjet,Objet[i]->Nom);
        if ((Objet[i]->Cache && GenerateHidden) || Objet[i]->Cache==0) {
          outl(Fichier,1,"cylinder {\n");
          outl(Fichier,2,"<0, 1,0>,\n");
          outl(Fichier,2,"<0,-1,0>,\n");
          outl(Fichier,2,"1.0,\n");
          outl(Fichier,2,"%.4g\n",Objet[i]->P[0]);
          print_f_SRT(Fichier,i,
                      Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                      Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                      Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],2);
          fprintf(Fichier,"  }\n\n");
        }
      }
    }
    outl(Fichier,1,"sturm\n");
    fprintf(Fichier,"}\n\n");
  }

  reinit_buffer_poly();
  reinit_buffer_spline();

  for (i=1;i<=NbObjet;i++) {
    f_jauge(9,MODIF,(long) i,(long) NbObjet,NULL);
    if (sequence_sortie()) { NbRendu-=1; i=-1; efface_fichier_shareware(); break; }
    if (Objet[i]->Cache && GenerateHidden==0) goto LABEL_OBJET_INVISIBLE;
    analyse_rotation(i);
    message("Generating object nø%05d/%d : %s",i,NbObjet,Objet[i]->Nom);

    if (Objet[i]->Type!=BLOB && Objet[i]->Type!=BLOBC) fprintf(Fichier,"// Object %05d: %s\n\n",i,Objet[i]->Nom);

    switch (Objet[i]->Type) {
      case CYLINDRE :
        enregistre_csg(i,Fichier,"cylinder");
        outl(Fichier,1,"<0, 1,0>,\n");
        outl(Fichier,1,"<0,-1,0>,\n");
        outl(Fichier,1,"1.0000\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case SPHERE:
        enregistre_csg(i,Fichier,"sphere");
        outl(Fichier,1,"<0,0,0>,1\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case CONET:
        enregistre_csg(i,Fichier,"cone");
        outl(Fichier,1,"<0,0, 1>,%.4g\n",Objet[i]->P[0]);
        outl(Fichier,1,"<0,0,-1>,1\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case CONE:
        enregistre_csg(i,Fichier,"cone");
        outl(Fichier,1,"<0, 1,0>,0\n");
        outl(Fichier,1,"<0,-1,0>,1\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case CUBE:
        enregistre_csg(i,Fichier,"box");
        outl(Fichier,1,"<-1,-1,-1>,<1, 1, 1>\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case SUPEREL:
        enregistre_csg(i,Fichier,"superellipsoid");
        outl(Fichier,1,"<%.4g,%.4g>\n",Objet[i]->P[_X],Objet[i]->P[_Y]);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case PLANX:
        enregistre_csg(i,Fichier,"plane");
        outl(Fichier,1,"<1,0,0>,0\n");
        print_f_SRT(Fichier,i,
                    1,1,1,
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case PLANY:
        enregistre_csg(i,Fichier,"plane");
        outl(Fichier,1,"<0,1,0>,0\n");
        print_f_SRT(Fichier,i,
                    1,1,1,
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case PLANZ:
        enregistre_csg(i,Fichier,"plane");
        outl(Fichier,1,"<0,0,1>,0\n");
        print_f_SRT(Fichier,i,
                    1,1,1,
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case TORE:
        enregistre_csg(i,Fichier,"torus");
        outl(Fichier,1,"%.4g,%.4g\n",Objet[i]->P[0],Objet[i]->P[1]);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1]/Objet[i]->P[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case LATHE:
        enregistre_csg(i,Fichier,"lathe");
        enregistre_lathe_pov(Fichier,i,1);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case BEZIER:
        enregistre_csg(i,Fichier,"bicubic_patch");
        enregistre_bezier_patch_pov(Fichier,i,1);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case EXTRUDE:
        enregistre_csg(i,Fichier,"prism");
        enregistre_prism_pov(Fichier,i,1);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case TUBE:
        enregistre_csg(i,Fichier,"object");
        fprintf(Fichier,"difference {\n");
        outl(Fichier,1,"cylinder {\n");
        outl(Fichier,2,"<0,0, 1>,\n");
        outl(Fichier,2,"<0,0,-1>,\n");
        outl(Fichier,2,"1\n"); //"%.4g\n",Objet[i]->S[0]);
        outl(Fichier,1,"} \n");
        outl(Fichier,1,"cylinder {\n");
        outl(Fichier,2,"<0,0, 1.1>,\n");
        outl(Fichier,2,"<0,0,-1.1>,\n");
        outl(Fichier,2,"%4f\n",Objet[i]->P[0]/**Objet[i]->S[0]*/);
        outl(Fichier,1,"}\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n}\n\n");
        break;
      case TRIANGLE:
        if (Poly[Objet[i]->Poly]->Buffer==0 /*||
            Poly[Objet[i]->Poly]->Smooth!=Objet[i]->Smooth*/) {

          Poly[Objet[i]->Poly]->Buffer=1;
          fprintf(Fichier,"#declare OBJ%04d = object {\n",Objet[i]->Poly);
          outl(Fichier,1,"mesh {\n");
          lisse_triangle(Objet[i]->Poly,Fichier,Niveau,i);
          outl(Fichier,1,"}\n");
          fprintf(Fichier,"}\n\n");
          Poly[Objet[i]->Poly]->Smooth=!Poly[Objet[i]->Poly]->Smooth;
        }
        fprintf(Fichier,"object {\n");
        outl(Fichier,1,"OBJ%04d\n",Objet[i]->Poly);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case SPLINE:
        enregistre_csg(i,Fichier,"object");
        generate_output_spline(i,Fichier);
        print_f_SRT(Fichier,i,1,1,1,0,0,0,0,0,0,1);
        fprintf(Fichier,"}\n\n");
        break;
      case ANNEAU:
        enregistre_csg(i,Fichier,"disc");
        outl(Fichier,1,"<0,0,0>,\n");
        outl(Fichier,1,"<0,0,1>,\n");
        outl(Fichier,1,"1,\n");
        outl(Fichier,1,"%4f\n",Objet[i]->P[0]);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case DISQUE:
        enregistre_csg(i,Fichier,"disc");
        outl(Fichier,1,"<0,0,0>,\n");
        outl(Fichier,1,"<0,0,1>,\n");
        outl(Fichier,1,"1\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case DSPHERE:
        enregistre_csg(i,Fichier,"object");
        outl(Fichier,1,"difference {\n");
        outl(Fichier,2,"sphere {\n");
        outl(Fichier,3,"<0,0,0>,1\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,2,"plane {\n");
        outl(Fichier,3,"<0,1,0>,0\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,1,"}\n\n");
        outl(Fichier,1,"translate <0,-0.5,0>\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1]*2,Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case QTORE:
        X=Z=(Objet[i]->P[0]+Objet[i]->P[1])/2;
        enregistre_csg(i,Fichier,"object");
        outl(Fichier,1,"difference {\n");
        outl(Fichier,2,"torus {\n");
        outl(Fichier,3,"%.4g,%.4g\n",Objet[i]->P[0],Objet[i]->P[1]);
        outl(Fichier,2,"}\n");
        outl(Fichier,2,"plane {\n");
        outl(Fichier,3,"<0,0,-1>,0\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,2,"plane {\n");
        outl(Fichier,3,"<-1,0,0>,0\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,1,"}\n");
        outl(Fichier,1,"translate <%.4g,0,%.4g>\n",X,Z);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0]*2,Objet[i]->S[1]/Objet[i]->P[1],Objet[i]->S[2]*2,
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case QTUBE:
        enregistre_csg(i,Fichier,"object");
        outl(Fichier,1,"difference {\n");
        outl(Fichier,2,"cylinder {\n");
        outl(Fichier,3,"<0,0, 1>,\n");
        outl(Fichier,3,"<0,0,-1>,\n");
        outl(Fichier,3,"1\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,2,"cylinder {\n");
        outl(Fichier,3,"<0,0, 1.1>,\n");
        outl(Fichier,3,"<0,0,-1.1>,\n");
        outl(Fichier,3,"%4f\n",Objet[i]->P[0]);
        outl(Fichier,2,"}\n");
        outl(Fichier,2,"plane {\n");
        outl(Fichier,3,"<0,-1,0>,0\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,2,"plane {\n");
        outl(Fichier,3,"<-1,0,0>,0\n");
        outl(Fichier,2,"}\n");
        outl(Fichier,1,"}\n");
        outl(Fichier,1,"translate <0.5,0.5,0>\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0]*2,Objet[i]->S[1]*2,Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case PYRAMIDE:
        enregistre_csg(i,Fichier,"object");
        outl(Fichier,1,"intersection {\n");
        outl(Fichier,2,"plane { < 1, 0, 0>, 0  rotate <  0, 0,  26.56505> translate < 0.5,1, 0  > }\n");
        outl(Fichier,2,"plane { <-1, 0, 0>, 0  rotate <  0, 0, -26.56505> translate <-0.5,1, 0  > }\n");
        outl(Fichier,2,"plane { < 0, 0, 1>, 0  rotate <-26.56505, 0,   0> translate < 0  ,1, 0.5> }\n");
        outl(Fichier,2,"plane { < 0, 0,-1>, 0  rotate < 26.56505, 0,   0> translate < 0  ,1,-0.5> }\n");
        outl(Fichier,2,"plane { < 0,-1, 0>, 0 }\n");
        outl(Fichier,2,"translate <0,-1,0>\n");
        outl(Fichier,1,"}\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case PRISME:
        enregistre_csg(i,Fichier,"object");
        outl(Fichier,1,"intersection {\n");
        outl(Fichier,2,"plane { < 1, 0, 0>,  1 }\n");
        outl(Fichier,2,"plane { <-1, 0, 0>,  0 translate <-1,0,0> }\n");
        outl(Fichier,2,"plane { < 0, 0, 1>,  0 rotate <-26.56505, 0, 0> translate < 0,1, 0.5> }\n");
        outl(Fichier,2,"plane { < 0, 0,-1>,  0 rotate < 26.56505, 0, 0> translate < 0,1,-0.5> }\n");
        outl(Fichier,2,"plane { < 0,-1, 0>,  0 }\n");
        outl(Fichier,2,"translate <0,-1,0>\n");
        outl(Fichier,1,"}\n");
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
      case HFIELD:
        enregistre_csg(i,Fichier,"object");
        outl(Fichier,1,"height_field {\n");
        outl(Fichier,2,"%s\n",strinstr(0,Objet[i]->CheminRaw,".GIF")>=0 ? "gif":"tga");
        outl(Fichier,2,"\"%s\"\n",Objet[i]->CheminRaw);
        outl(Fichier,2,"%s\n",Objet[i]->Smooth ? "smooth":"// smooth");
        if (Objet[i]->P[_Z]) {
          outl(Fichier,2,"water_level %.4g\n",Objet[i]->WLevel);
        }
        outl(Fichier,1,"}\n");
        outl(Fichier,1,"translate <-0.5,-%.4g,-0.5>\n",(DBL) Objet[i]->HautHF/2);
        outl(Fichier,1,"scale <2,%.4g,2>\n",(DBL) 2/Objet[i]->HautHF);
        print_f_SRT(Fichier,i,
                    Objet[i]->S[0],Objet[i]->S[1],Objet[i]->S[2],
                    Objet[i]->R[0],Objet[i]->R[1],Objet[i]->R[2],
                    Objet[i]->T[0],Objet[i]->T[1],Objet[i]->T[2],1);
        fprintf(Fichier,"}\n\n");
        break;
    }
    LABEL_OBJET_INVISIBLE:
    i=i;  // Just for Mr WATCOM !!
  }

  calcule_csg(Fichier);
  genere_lumieres(Fichier,OMNI,1);
  genere_lumieres(Fichier,AREA,1);

  message("Closing render # %d file %s",NbRendu,Buffer);
  LABEL_FIN_POV:
  fclose(Fichier);
  f_jauge(9,EFFACE,0,0,NULL);
  return (i==-1 ? 0:1);
}

// -----------------------------------------------------------------------
// --------- PREPARE LES OPTIONS VIDEO POUR LES RENDUS POVRAY ------------
// -----------------------------------------------------------------------
void options_video(void) {
  int N=18;
  int i;
  register int X1,Y1,X2,Y2;
  char Cartes[]={"0123456789ABCDEFGHI"};

  X1=CentX-115;
  X2=CentX+115;
  Y1=CentY-(N/2*15+30);
  Y2=CentY+(N/2*15+60);

  message("Choose a graphic card for for rendering");

  strcpy(StrBoite[0],"Available graphic cards");
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  init_pastille( 0,X1+20,Y1+ 30,"Autodetect (S)VGA",(OptVideo=='0'),"");
  init_pastille( 1,X1+20,Y1+ 45,"Standard VGA 320x200",(OptVideo=='1'),"");
  init_pastille( 2,X1+20,Y1+ 60,"Standard VGA 360 x 480",(OptVideo=='2'),"");
  init_pastille( 3,X1+20,Y1+ 75,"Tseng Labs 3000 SVGA",(OptVideo=='3'),"");
  init_pastille( 4,X1+20,Y1+ 90,"Tseng Labs 4000 SVGA",(OptVideo=='4'),"");
  init_pastille( 5,X1+20,Y1+105,"AT&T VDC600 SVGA 640x400",(OptVideo=='5'),"");
  init_pastille( 6,X1+20,Y1+120,"Oak Technologies SVGA",(OptVideo=='6'),"");
  init_pastille( 7,X1+20,Y1+135,"Video 7 SVGA",(OptVideo=='7'),"");
  init_pastille( 8,X1+20,Y1+150,"Video 7 Vega (Cirrus) VGA 360x480",(OptVideo=='8'),"");
  init_pastille( 9,X1+20,Y1+165,"Paradise SVGA",(OptVideo=='9'),"");
  init_pastille(10,X1+20,Y1+180,"Ahead Systems Ver. A SVGA",(OptVideo=='A'),"");
  init_pastille(11,X1+20,Y1+195,"Ahead Systems Ver. B SVGA",(OptVideo=='B'),"");
  init_pastille(12,X1+20,Y1+210,"Chips & Technologies SVGA",(OptVideo=='C'),"");
  init_pastille(13,X1+20,Y1+225,"ATI SGVA",(OptVideo=='D'),"");
  init_pastille(14,X1+20,Y1+240,"Everex SVGA",(OptVideo=='E'),"");
  init_pastille(15,X1+20,Y1+255,"Trident SVGA",(OptVideo=='F'),"");
  init_pastille(16,X1+20,Y1+270,"VESA Standard SVGA Adapter",(OptVideo=='G'),"");
  init_pastille(17,X1+20,Y1+285,"ATI XL display card",(OptVideo=='H'),"");
  init_pastille(18,X1+20,Y1+300,"Diamond Computer Systems SpeedSTAR 24X",(OptVideo=='I'),"");

  for (i=0;i<=18;i++) affiche_pastille(i);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(0,18);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    i=quelle_pastille_dans_groupe(0,18);
    OptVideo=Cartes[i];
  } else {
    return;
  }
}

// -------------------------------------------------------------------------
// -- PREPARE LONGUEUR DES TABULATIONS -------------------------------------
// -------------------------------------------------------------------------
void set_indentation_output(void) {
  int X1=CentX-120;
  int X2=CentX+120;
  int Y1=CentY-40;
  int Y2=CentY+50;
  int i;

  init_potar(0,X1+30,Y1+40,120,1,5,NbTab,1,"Soft tabs output format");
  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"Format output",AFFICHE);
  affiche_potar(0);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_potar(0,0);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  if (i==0) NbTab=(int) Potar[0].Val;

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  forme_mouse(MS_FLECHE);
}
