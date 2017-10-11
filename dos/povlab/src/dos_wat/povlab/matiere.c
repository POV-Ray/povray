/* ---------------------------------------------------------------------------
*  MATIERE.C
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
#include <MATH.H>
#include <FLOAT.H>
#include <STRING.H>
#include <IO.H>
#include <DOS.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

char *NomMat[NB_MAX_MATIERE];
char *NomGif[NB_MAX_MATIERE];

char CheminMTEX[MAXPATH];
char MatiereCourante[MAXPATH]={"Default"};
char MatiereCouranteGif[MAXPATH]={NULLC};
Vecteur MS={1,1,1};
Vecteur MR={0,0,0};
Vecteur MT={0,0,0};
int NumMatEnCours=0;
byte AnalyseTexture=1;

// -------------------------------------------------------------------------
// -- RECUPERE LE FICHIER INCLUDE DANS LA LIBRARIE DE TEXTURE --------------
// -------------------------------------------------------------------------
byte retourne_include(char* Include) {
  FILE *Fichier;
  char Buffer[MAXPATH];

  if ((Fichier=fopen(CheminMTEX,"r+t"))==NULL) {
    f_erreur("Can't open the file %s",CheminMTEX);
    return 0;
  }
  if (fgets(Buffer,256,Fichier)==NULL) {
    fclose(Fichier);
    f_erreur("Can't open the file %s",CheminMTEX);
    return 0;
  }
  fclose(Fichier);
  strcpy(Include,Buffer);
  strupr(Include);
  Include[strinstr(0,Include,"\n")]=NULLC;
  return 1;
}

// -------------------------------------------------------------------------
// -- LECTURE DU FICHIER DE DESCRIPTION DE MATIERES ------------------------
// -------------------------------------------------------------------------
void lecture_matiere (void) {
  int X1=CentX-155;
  int X2=CentX+155;
  int Y1=CentY-125;
  int Y2=CentY+125;
  FILE *Fichier;
  char Buffer[256],Include[MAXPATH];
  register int i,j;
  int NbM=0;
  int Vu=-1,N,X,Y;
  char *Image;
  struct retour_selecteur Val;
  static int NElem=0,NNow=0,NPosi=1;
  char *RemMat[NB_MAX_MATIERE];

  if (test_fichier(CheminMTEX)==0) {
    beep_erreur();
    strcpy(StrBoite[0],"Invalid path");
    strcpy(StrBoite[1],"Texture description file is missing");
    strcpy(StrBoite[2],"Please check your config file");
    strcpy(StrBoite[3],"in the SYSTEM path.");
    g_boite_ONA(CentX,CentY,3,CENTRE,0);
    return;
  }

  message("Reading texture description file");

  if (!retourne_include(Include)) return;

  NomMat[0]=(char *) malloc(23);
  strcpy(NomMat[0],"[None (Object color)]");
  NomGif[0]=(char *) malloc(2);
  strcpy(NomGif[0]," ");
  RemMat[0]=(char *) malloc(40);
  strcpy(RemMat[0],"Use object color if no mapping/texture");

  if ((Fichier=fopen(CheminMTEX,"rt"))==NULL) {
    f_erreur("Can't open the file %s",CheminMTEX);
    goto LABEL_ERASE_POINTEURS;
  }
  fgets(Buffer,256,Fichier);

  while (!feof(Fichier)) {
    Buffer[0]=NULLC;
    fgets(Buffer,256,Fichier);
    if (feof(Fichier)) break;
    if (Buffer[0]!=32 && Buffer[0]!=NULLC && Buffer[0]!='\n') {
      analyse_ligne(Buffer,32);
      NbM++;
      if (Argu[0][0]=='#') {
        for (i=0;i<=strlen(Argu[0]);i++) if (Argu[0][i]=='#') Argu[0][i]=32;
      }
      for (i=0;i<=strlen(Argu[2]);i++) if (Argu[2][i]=='#') Argu[2][i]=32;
      NomMat[NbM]=(char *) NULL;
      NomGif[NbM]=(char *) NULL;
      RemMat[NbM]=(char *) NULL;
      NomMat[NbM]=(char *) malloc(strlen(Argu[0])+1);
      NomGif[NbM]=(char *) malloc(strlen(Argu[1])+1);
      RemMat[NbM]=(char *) malloc(strlen(Argu[2])+1);
      strcpy(NomMat[NbM],Argu[0]);
      strcpy(NomGif[NbM],Argu[1]);
      strcpy(RemMat[NbM],Argu[2]);
      if (NbM==NB_MAX_MATIERE-1) break;
    }
  }

  fclose(Fichier);
  sprintf(Buffer,"%d textures loaded",NbM+1);
  message_aide(0,YMenuD+7,JAUNE,13,Buffer,MODIF);

  g_fenetre(X1,Y1,X2,Y2,"TEXTURES SELECTOR",AFFICHE);
  windows(X1+10,Y2-18,X2-10,Y2-5,0,ZFOND);  // fichier include
  bouton_dialog(X1,X2,Y2-20,1,0);

  init_selecteur(0,X1,Y1,12,NbM+1,NomMat,18);
  Mn[0].NumElem=NElem;
  Mn[0].NumElemNow=NNow;
  Mn[0].PosiF=NPosi;
  affiche_selecteur(0);
  text_xy(X1+12,Y2-18,Include,BLANC);

  windows(X2-110,Y1+35,X2-10,Y1+135,0,FOND); // cadre GIF

  while (1) {
    Val=test_selecteur(0);
    N=Val.Num;
    if ((i=bouton_dialog(X1,X2,Y2-20,0,0))!=-1) break;
    if (strinstr(0,NomGif[N],".")==-1) N=0;
    if (NbCouleurs==16 && Vu!=N && test_fichier(NomGif[N])) {
      Vu=N;
      i=0;
      message("Thumbnail: %s",NomGif[N]);
    }
    if (NbCouleurs==256 && Vu!=N) {
      i=0;
      message("%s",RemMat[N]);
      if (test_fichier(NomGif[N]) &&
          decompresse_gif(&X,&Y,NomGif[N],"",1)) {
        Image=(char *) malloc((size_t) 6+X*Y);
        decompresse_gif(&X,&Y,NomGif[N],Image,0);
        GMouseOff();
        affiche_gif(X2-110,Y1+35,X,Y,Image,1,1,0);
        message("Thumbnail: %s",NomGif[N]);
        GMouseOn();
        free(Image);
      } else {
        GMouseOff();
        affiche_icone(X2-110,Y1+35,0,PasDeTexture);
        GMouseOn();
      }
      Vu=N;
    }
  }

  NElem=Mn[0].NumElem;
  NNow=Mn[0].NumElemNow;
  NPosi=Mn[0].PosiF;

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    if (N==0) {
      strcpy(MatiereCourante,"Default");
      strcpy(MatiereCouranteGif,"\0");
    } else {
      strcpy(MatiereCourante,NomMat[N]);
      strcpy(MatiereCouranteGif,NomGif[N]);
    }
    if (i==0) assigne_matiere();
    affiche_donnees();
  }

  LABEL_ERASE_POINTEURS:

  for (j=0;j<=NbM;j++) {
    if (NomMat[j]) free(NomMat[j]);
    if (RemMat[j]) free(RemMat[j]);
    if (NomGif[j]) free(NomGif[j]);
  }
}

// -------------------------------------------------------------------------
// -- ACQUERIR UNE MATIERE D'UN OBJET --------------------------------------
// -------------------------------------------------------------------------
void give_me_your_matiere (void) {
  int i,j;

  message("Select object to get texture...");

  LABEL_ACQUERIR:
  forme_mouse(MS_SELECTEUR);
  if ((i=trouve_volume(0,3,1))==FAUX) return;

  strcpy(MatiereCourante,Objet[i]->Matiere);
  nom_gif_matiere_courante();

  for (j=0;j<=2;j++) {
    MS[j]=Objet[i]->MS[j];
    MR[j]=Objet[i]->MR[j];
    MT[j]=Objet[i]->MT[j];
  }

  message("Current texture: %s",MatiereCourante);
  affiche_donnees();

  while (MouseB());
  forme_mouse(MS_FLECHE);
  GMouseOn();
  goto LABEL_ACQUERIR;
}

// -------------------------------------------------------------------------
// -- ASSIGNER UNE MATIERE SUR UN OBJET ------------------------------------
// -------------------------------------------------------------------------
void assigne_matiere (void) {
  int i,j,X,Y;

  if (!test_si_selection_et_coche()) return;

  LABEL_MATIERE:

  {
    char Tmp[MAXPATH];

    if (MatiereCourante[0]=='#') {
      split_chemin(Tmp,MatiereCourante+1,5);
    } else {
      strcpy(Tmp,MatiereCourante);
    }
    message("Select object to assign texture '%s'",Tmp);
  }

  forme_mouse(MS_SELECTEUR);
  GMouseOn();

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
  } else {
    if ((NumObjet=trouve_volume(0,3,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);
  GMouseOff();

  strcpy(StrBoite[0],"Assign texture");
  strcpy(StrBoite[1],"Assign current texture :");
  sprintf(StrBoite[2],"\"%s\"",MatiereCourante);


  if (Selection && OkSelect==1) {
    strcpy(StrBoite[3],"to all selected objects");
    sprintf(StrBoite[4],"in the current scene %s ?",FichierSCN);
  } else {
    sprintf(StrBoite[3],"instead of \"%s\"",Objet[NumObjet]->Matiere);
    sprintf(StrBoite[4],"for object nø%d \"%s\" ?",NumObjet,Objet[NumObjet]->Nom);
  }

  if (g_boite_ONA(CentX,CentY,4,CENTRE,1)==0) {
    if (Selection && OkSelect==1) {
      for (i=1;i<=NbObjet;i++) {
        if (Objet[i]->Selection && !Objet[i]->Cache) {
          strcpy(Objet[i]->Matiere,MatiereCourante);
          for (j=0;j<=2;j++) {
            Objet[i]->MS[j]=MS[j];
            Objet[i]->MR[j]=MR[j];
            Objet[i]->MT[j]=MT[j];
          }
          if (Objet[i]->Matiere[0]=='#') {
            extract_xy_mapping(Objet[i]->Matiere+1,&X,&Y);
            if (X>=Y) {
              Objet[i]->MS[_X]=(DBL) 1.0;
              Objet[i]->MS[_Y]=(DBL) Y/X;
              
            } else {
              Objet[i]->MS[_X]=(DBL) X/Y;
              Objet[i]->MS[_Y]=(DBL) 1.0;
            }
            Objet[i]->MS[_Z]=(DBL) 1.0;
          }
        }
      }
    } else {
      strcpy(Objet[NumObjet]->Matiere,MatiereCourante);
      for (j=0;j<=2;j++) {
        Objet[NumObjet]->MS[j]=MS[j];
        Objet[NumObjet]->MR[j]=MR[j];
        Objet[NumObjet]->MT[j]=MT[j];
      }
      if (Objet[NumObjet]->Matiere[0]=='#') {
        extract_xy_mapping(Objet[NumObjet]->Matiere+1,&X,&Y);
        if (X>=Y) {
          Objet[NumObjet]->MS[_X]=(DBL) 1.0;
          Objet[NumObjet]->MS[_Y]=(DBL) Y/X;
        } else {
          Objet[NumObjet]->MS[_X]=(DBL) X/Y;
          Objet[NumObjet]->MS[_Y]=(DBL) 1.0;
        }
        Objet[NumObjet]->MS[_Z]=(DBL) 1.0;
      }
    }
  }

  forme_mouse(MS_FLECHE);
  GMouseOn();
  if (OkSelect==0) goto LABEL_MATIERE;
}

// -------------------------------------------------------------------------
// -- LECTURE DU FICHIER POUR COMPARAISON AVEC MATIERE SCENE ---------------
// -------------------------------------------------------------------------
int analyse_matiere_scene(int Num) {
  int X1=CentX-104;
  int X2=CentX+104;
  int Y1=CentY-160;
  int Y2=CentY+160;
  FILE *Fichier;
  char Buffer[256];
  char *NomObj[NB_OBJET_MAX];
  register int i,j,Ok=0;
  int NbM=0;
  int Manque=0;
  struct retour_selecteur Val;

  if (!AnalyseTexture) return 1;

  if (Num==0) {
    if (test_fichier(CheminMTEX)==0) {
      beep_erreur();
      strcpy(StrBoite[0],"Invalid path");
      strcpy(StrBoite[1],"Texture description file cannot be");
      strcpy(StrBoite[2],"found. Check your config file");
      strcpy(StrBoite[3],"in the SYSTEM path.");
      g_boite_ONA(CentX,CentY,3,CENTRE,0);
      return 0;
    }
    message("Reading texture description");
  } else {
    if (Objet[Num]->Matiere[0]=='#') return 1;
  }

  NomMat[0]=(char *) mem_alloc(17);
  strcpy(NomMat[0],"Default");

  if ((Fichier=fopen(CheminMTEX,"r+t"))==NULL) {
    f_erreur("Can't open the file %s",CheminMTEX);
    return 0;
  }

  fgets(Buffer,256,Fichier);

  do {
    Buffer[0]=NULLC;
    fgets(Buffer,256,Fichier);
    if (feof(Fichier)) break;
    if (Buffer[0]!=32 && Buffer[0]!=NULLC) {
      analyse_ligne(Buffer,32);
      NbM++;
      NomMat[NbM]=(char *) mem_alloc(strlen(Argu[0])+1);
      strcpy(NomMat[NbM],Argu[0]);
      if (NbM==NB_MAX_MATIERE) break;
    }
  } while (!feof(Fichier));

  NbM++;

  fclose(Fichier);

  if (Num==0) {
    message("%d textures loaded",NbM+1);
    f_jauge(0,AFFICHE,0,0,"SCANNING TEXTURE/OBJECT");
    for (i=1;i<=NbObjet;i++) {
      f_jauge(0,MODIF,i,NbObjet,NULL);
      if (!Objet[i]->Cache && Objet[i]->Matiere[0]!='#') {
        Ok=1;
        for (j=0;j<=NbM;j++) {
          if (strinstr(0,NomMat[j],Objet[i]->Matiere)==0 &&
              strlen(NomMat[j])==strlen(Objet[i]->Matiere)) { Ok=2; break; }
        }
        if (Ok==1) {
          NomObj[Manque]=(char *) mem_alloc(strlen(Objet[i]->Nom)+strlen(Objet[i]->Matiere)+4);
          sprintf(NomObj[Manque],"%s [%s]",Objet[i]->Nom,Objet[i]->Matiere);
          if (strlen(NomObj[Manque])>27) NomObj[Manque][27]=NULLC;
          Manque++;
        }
      }
    }
    f_jauge(0,EFFACE,0,0,NULL);
  } else {
    for (j=0;j<=NbM;j++) {
      if (strinstr(0,Objet[Num]->Matiere,NomMat[j])==0 &&
      strlen(NomMat[j])==strlen(Objet[Num]->Matiere)) break;
    }
    if (j>=NbM) Ok=0; else Ok=1;
  }

  if (Manque && Num==0) {
    g_fenetre(X1,Y1,X2,Y2,"ANALYZING TEXTURES",AFFICHE); // fenˆtre
    bouton_dialog(X1,X2,Y2,1,1);
    init_selecteur(0,X1+2,Y1,12,Manque,NomObj,18);
    affiche_selecteur(0);
    sprintf(StrBoite[1],"%04d objects have a texture that",Manque);
    strcpy(StrBoite[2],"don't exist in the library. \"Yes\"");
    strcpy(StrBoite[3],"to continue and render the scene with");
    strcpy(StrBoite[4],"default texture instead.");
    for (i=1;i<=4;i++) {
      parse_ascii_watcom(StrBoite[i]);
      text_xy(X1+10,Y2-93+(i-1)*15,StrBoite[i],BLANC);
    }
    while (1) {
      Val=test_selecteur(0);
      if (Val.Ok==13) { i=0; break; }
      if (Val.Ok==27) { i=1; break; }
      if ((i=bouton_dialog(X1,X2,Y2,0,1))>=0) break;
    }
    g_fenetre(X1,Y1,X2,Y2,"",EFFACE); // fenˆtre
    if (i==0) Ok=1; else Ok=0;
  } else {
    if (Num==0) Ok=1;
  }

  if (NbM>0) {
    for (j=0;j<=NbM;j++) {
      mem_free(NomMat[j],strlen(NomMat[j])+1);
    }
  }

  if (Manque && Num==0) {
    for (j=0;j<Manque;j++) {
      mem_free(NomObj[j],strlen(NomObj[j])+1);
    }
  }

  return Ok;
}

// -------------------------------------------------------------------------
// -- MODIFIE LES PARAMETERES DE LA TEXTURE SCALE/ROTATE/TRANSLATE ---------
// -------------------------------------------------------------------------
byte modif_matiere(byte Travail) {
  int i,NO,B;

  if (pas_objet(1)) return 0;

  while (1) {
    B=0;
    forme_mouse(((Travail==ROTATE || Travail==DIVERS || Travail==4) ? MS_SELECTEUR:Sens));
    message("Select an object");
    if ((NO=trouve_volume(0,2,1))==FAUX) break;
    if (sequence_sortie()) break;
    NumObjet=0;

    Objet[0]->Type=MAPPING;
    modif_objet(0,Objet[NO]->MS[0],Objet[NO]->MS[1],Objet[NO]->MS[2],SCALE);
    modif_objet(0,Objet[NO]->MR[0],Objet[NO]->MR[1],Objet[NO]->MR[2],ROTATE);
    modif_objet(0,Objet[NO]->MT[0],Objet[NO]->MT[1],Objet[NO]->MT[2],TRANSLATE);
    Objet[0]->Cache=0;
    Objet[0]->Selection=0;
    Objet[0]->Rapide=0;
    Objet[0]->Buffer=0;
    strcpy(Objet[0]->Nom,"Texture");

    NumObjet=0;

    switch(Travail) {
      case SCALE:     deformation2D(1,0); break;
      case DIVERS:    deformation3D(1,0); break; // scale 3D
      case ROTATE:    rotation(1,0);      break;
      case TRANSLATE: translation(1,0);   break;
    }

    for (i=0;i<3;i++) {
      if (Travail==SCALE || Travail==DIVERS) {
        if (Objet[0]->S[i]!=Objet[NO]->MS[i]) { B=1; break; }
      }
      if (Travail==ROTATE) {
        if (Objet[0]->R[i]!=Objet[NO]->MR[i]) { B=1; break; }
      }
      if (Travail==TRANSLATE) {
        if (Objet[0]->T[i]!=Objet[NO]->MT[i]) { B=1; break; }
      }
    }

    if (B) {
      for (i=0;i<3;i++) {
        switch (Travail) {
          case SCALE:     Objet[NO]->MS[i]=Objet[0]->S[i]; break;
          case DIVERS:    Objet[NO]->MS[i]=Objet[0]->S[i]; break;
          case ROTATE:    Objet[NO]->MR[i]=Objet[0]->R[i]; break;
          case TRANSLATE: Objet[NO]->MT[i]=Objet[0]->T[i]; break;
        }
      }
    }

    if (Travail==4) {
      strcpy(StrBoite[0],"REINIT TEXTURE");
      strcpy(StrBoite[1],"Do you really want to reinit");
      strcpy(StrBoite[2],"textures parameters for selected object");
      sprintf(StrBoite[3],"nø%d \"%s\" ?",NO,Objet[NO]->Nom);
      if (g_boite_ONA(CentX,CentY,3,CENTRE,1)==0) {
        vect_init(Objet[NO]->MS,1.0,1.0,1.0);
        vect_init(Objet[NO]->MR,0.0,0.0,0.0);
        vect_init(Objet[NO]->MT,0.0,0.0,0.0);
      }
    }
  }

  return 1;
}

// -------------------------------------------------------------------------
// -- MODIFIE LES PARAMETERES DE LA TEXTURE NON FIGES ----------------------
// -------------------------------------------------------------------------
byte modif_parametre_texture(int N) {
  int X1=CentX-180;
  int X2=CentX+180;
  int Y1=CentY-170;
  int Y2=CentY+180;
  int i,j,k;
  int D,F;

  if (pas_objet(1)) return 0;
  if (!test_si_selection_et_coche()) return 0;
  message("Modify some texture finish properties");

  forme_mouse(MS_SELECTEUR);

  D=F=N;
  if (N==0) {
    if (Selection && OkSelect==1) {
      if (cherche_fenetre()==FAUX) return 0;
      N=0;
      D=1;
      F=NbObjet;
    } else {
      if ((N=trouve_volume(0,3,1))==FAUX) return 0;
      D=F=N;
    }
  }

  if (N==0) {
    Objet[0]->Ior       =0;
    Objet[0]->Refraction=0;
    Objet[0]->Reflexion =0;
    Objet[0]->Diffusion =0;
    Objet[0]->Ambiance  =0;
    Objet[0]->Crand     =0;
    Objet[0]->Phong     =0;
    Objet[0]->PSize     =0;
    Objet[0]->Caustics  =0;
    Objet[0]->Fade_D    =0;
    Objet[0]->Fade_P    =0;
    Objet[0]->Rough     =0;
    Objet[0]->Brilli    =0;
    Objet[0]->Specular  =0;
    for (j=0;j<=13;j++) {
      Objet[0]->BitTexture[j]=0;
    }
  }

  forme_mouse(MS_FLECHE);
  g_fenetre(X1,Y1,X2,Y2,"Texture parameters",AFFICHE);

  init_potar( 0,X1+35,Y1+ 40,100,0,  5,((DBL)Objet[N]->Ior)       ,0.01,"Indice of refraction");
  init_potar( 1,X1+35,Y1+ 60,100,0,100,((DBL)Objet[N]->Refraction),1.0,"Amonth of refraction");
  init_potar( 2,X1+35,Y1+ 80,100,0,100,((DBL)Objet[N]->Reflexion) ,1.0,"Amonth of reflection");
  init_potar( 3,X1+35,Y1+100,100,0,100,((DBL)Objet[N]->Diffusion) ,1.0,"Diffuse");
  init_potar( 4,X1+35,Y1+120,100,0,100,((DBL)Objet[N]->Ambiance)  ,1.0,"Ambient");
  init_potar( 5,X1+35,Y1+140,100,0,100,((DBL)Objet[N]->Crand)   ,1.0,"Crand");
  init_potar( 6,X1+35,Y1+160,100,0,100,((DBL)Objet[N]->Phong)  ,1.0,"Phong");
  init_potar( 7,X1+35,Y1+180,100,0,100,((DBL)Objet[N]->PSize)   ,1.0,"Phong size");
  init_potar( 8,X1+35,Y1+200,100,0,100,((DBL)Objet[N]->Caustics)   ,1.0,"Caustics");
  init_potar( 9,X1+35,Y1+220,100,0,1000,((DBL)Objet[N]->Fade_D)   ,1.0,"Fade_Distance");
  init_potar(10,X1+35,Y1+240,100,0,1000,((DBL)Objet[N]->Fade_P)   ,1.0,"Fade_Power");
  init_potar(11,X1+35,Y1+260,100,0,100,((DBL)Objet[N]->Rough)   ,1.0,"Roughness");
  init_potar(12,X1+35,Y1+280,100,0,100,((DBL)Objet[N]->Brilli)   ,1.0,"Brilliance");
  init_potar(13,X1+35,Y1+300,100,0,100,((DBL)Objet[N]->Specular)  ,1.0,"Specular");
  for (i=0;i<=13;i++) affiche_potar(i);

  init_case(11,X1+220,Y1+ 40-6,"Ior actived",Objet[N]->BitTexture[0],"Indice of refraction actived");
  init_case(12,X1+220,Y1+ 60-6,"Refraction actived",Objet[N]->BitTexture[1],"Amonth of refraction actived");
  init_case(13,X1+220,Y1+ 80-6,"Reflection actived",Objet[N]->BitTexture[2],"Amonth of reflection actived");
  init_case(14,X1+220,Y1+100-6,"Diffusion actived",Objet[N]->BitTexture[3],"Diffuse actived");
  init_case(15,X1+220,Y1+120-6,"Ambiance actived",Objet[N]->BitTexture[4],"Ambient actived");
  init_case(16,X1+220,Y1+140-6,"Crand actived",Objet[N]->BitTexture[5],"Crand actived");
  init_case(17,X1+220,Y1+160-6,"Phong actived",Objet[N]->BitTexture[6],"Phong actived");
  init_case(18,X1+220,Y1+180-6,"Phong size actived",Objet[N]->BitTexture[7],"Phong size actived");
  init_case(19,X1+220,Y1+200-6,"Caustics actived",Objet[N]->BitTexture[8],"Caustics actived");
  init_case(20,X1+220,Y1+220-6,"Fade_Distance actived",Objet[N]->BitTexture[9],"Fade distance size actived");
  init_case(21,X1+220,Y1+240-6,"Fade_Power actived",Objet[N]->BitTexture[10],"Fade power actived");
  init_case(22,X1+220,Y1+260-6,"Roughness actived",Objet[N]->BitTexture[11],"Roughness actived");
  init_case(23,X1+220,Y1+280-6,"Brilliance actived",Objet[N]->BitTexture[12],"Brilliance actived");
  init_case(24,X1+220,Y1+300-6,"Specular actived",Objet[N]->BitTexture[13],"Specular actived");
  for (i=11;i<=24;i++) affiche_case(i);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_potar(0,13);
    test_case(11,24);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    if (!N) {
      if (Selection && OkSelect==1) {
        if (x_fenetre(CentX,CentY,GAUCHE,1,"Change finish statements|"\
          "Do you really want to change the finish|"\
          "statement of all %d selected objects ?|",Selection)==1) return 0;
      }
    }
    for (k=D;k<=F;k++) {
      if (((Objet[k]->Selection && OkSelect) || k==N) && !Objet[k]->Cache) {
        if (Cc[11+ 0].Croix) Objet[k]->Ior       =Potar[ 0].Val;
        if (Cc[11+ 1].Croix) Objet[k]->Refraction=Potar[ 1].Val;
        if (Cc[11+ 2].Croix) Objet[k]->Reflexion =Potar[ 2].Val;
        if (Cc[11+ 3].Croix) Objet[k]->Diffusion =Potar[ 3].Val;
        if (Cc[11+ 4].Croix) Objet[k]->Ambiance  =Potar[ 4].Val;
        if (Cc[11+ 5].Croix) Objet[k]->Crand     =Potar[ 5].Val;
        if (Cc[11+ 6].Croix) Objet[k]->Phong     =Potar[ 6].Val;
        if (Cc[11+ 7].Croix) Objet[k]->PSize     =Potar[ 7].Val;
        if (Cc[11+ 8].Croix) Objet[k]->Caustics  =Potar[ 8].Val;
        if (Cc[11+ 9].Croix) Objet[k]->Fade_D    =Potar[ 9].Val;
        if (Cc[11+10].Croix) Objet[k]->Fade_P    =Potar[10].Val;
        if (Cc[11+11].Croix) Objet[k]->Rough     =Potar[11].Val;
        if (Cc[11+12].Croix) Objet[k]->Brilli    =Potar[12].Val;
        if (Cc[11+13].Croix) Objet[k]->Specular  =Potar[13].Val;

        for (j=0;j<=13;j++) {
          if (Objet[k]->Selection && OkSelect && N==0) {
            if (Cc[11+j].Croix) Objet[k]->BitTexture[j]=Cc[11+j].Croix;
          } else {
            Objet[k]->BitTexture[j]=Cc[11+j].Croix;
          }
        }
      }
    }
  }

  return (byte) !i;
}

// -------------------------------------------------------------------------
// -- EXTRAIT LE NOM D'UNE TEXTURE D'UNE LIGNE -----------------------------
// -------------------------------------------------------------------------
byte extract_nom_texture(char *Nom,char *Buffer) {
  register int i;
  char Buffer2[256];
  strcpy(Buffer2,Buffer);

  i=strinstr(0,Buffer2,"#declare");
  if (i<0) return 0;

  for (i=0;i<strlen(Buffer2);i++) {
    if (Buffer2[i]=='{' || Buffer2[i]=='=') {
      Buffer2[i]=32;
    }
  }

  analyse_ligne(Buffer2,32);

  if (strinstr(0,Nom,Argu[1])>=0 && strlen(Nom)==strlen(Argu[1])) return 1;
  
  return 0;
}

// -------------------------------------------------------------------------
// -- GENERATION DU FICHIER DE TEXTURE INCLUDE -----------------------------
// -------------------------------------------------------------------------
int genere_include_matiere(void) {
  FILE *FileIn;
  FILE *FileOut;
  char Buffer[256];
  register int i,j,k;
  int NbM=-1;
  char FichierIn[MAXPATH];
  char FichierOut[MAXPATH];
  char Heure[9],Date[40];

  get_date(Date);
  get_heure(Heure);

  if (!retourne_include(FichierIn)) return 0;

  strcpy(FichierOut,CheminPOVSCN);
  strcat(FichierOut,"\\");
  strcat(FichierOut,FichierSCN);
  i=strinstr(0,FichierOut,".SCN");
  FichierOut[i]=NULLC;
  strcat(FichierOut,".INC");

  for (i=1;i<=NbObjet;i++) {
    k=0;
    for (j=0;j<=NbM;j++) {
      if (strinstr(0,NomMat[j],Objet[i]->Matiere)>-1 &&
      strlen(NomMat[j])==strlen(Objet[i]->Matiere)) { k=1; break; }
    }
    if (k==0) {
      NbM++;
      NomMat[NbM]=(char *) mem_alloc(strlen(Objet[i]->Matiere)+1);
      strcpy(NomMat[NbM],Objet[i]->Matiere);
    }
  }

  if ((FileIn=fopen(FichierIn,"r+t"))==NULL) {
    f_erreur("Can't open the description file %s|Check the first line path in TEXTURE\\POVLAB.TEX",FichierIn);
    return 0;
  }
  if ((FileOut=fopen(FichierOut,"w+t"))==NULL) {
    f_erreur("Can't open the file %s",FichierOut);
    return 0;
  }

  fprintf(FileOut,"// %s version %s - Included texture file.\n",NomLogiciel,VerLogiciel);
  fprintf(FileOut,"// (C) 1994-%s, Denis Olivier.\n",RES_COPY[4]);
  fprintf(FileOut,"// All rights reserved.\n");
  fprintf(FileOut,"// Generated for POV-Ray (C) POV-Team, USA.\n");
  fprintf(FileOut,"// Date=%s.\n// Time=%s.\n",Date,Heure);
  fprintf(FileOut,"// user_name=%s\n",NomUtilisateur);
  fprintf(FileOut,"// From=%s\n",FichierIn);
  fprintf(FileOut,"// For=%s%s\n",CheminSCN,FichierSCN);
  fprintf(FileOut,"// Textures found=%d\n\n",NbM+1);

  for (i=0;i<=NbM;i++) {
    log_out(i,"Extract mat : %s",NomMat[i]);
    fseek(FileIn,0,0);
    k=0;
    message("Searching texture : %s",NomMat[i]);
    while (!feof(FileIn)) {
      fgets(Buffer,256,FileIn);
      if (extract_nom_texture(NomMat[i],Buffer)) {
        k=1;
        while (!feof(FileIn)) {
          if (Buffer[0]!='/') fprintf(FileOut,"%s",Buffer);
          fgets(Buffer,256,FileIn);
          if (strinstr(0,Buffer,"#declare")>-1) break;
        }
      }
    }
    message("Texture %s %s",NomMat[i],(k ? "found":"not found"));
  }

  fclose(FileIn);
  fclose(FileOut);


  if (NbM>-1) {
    for (j=0;j<=NbM;j++) {
      mem_free(NomMat[j],strlen(NomMat[j])+1);
    }
  }

  return 1;
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP POUR LES TEXTURES ---------------------------------
// -------------------------------------------------------------------------
void setup_texture(void) {
  int X1=CentX-95;
  int X2=CentX+95;
  int Y1=CentY-40;
  int Y2=CentY+40;
  int i;

  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"Texture setup",AFFICHE);

  init_case(11,X1+10,Y1+30,"Check for missing textures",AnalyseTexture,"Check textures before render");

  affiche_case(11);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_case(11,11);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  if (i==0) AnalyseTexture=Cc[11].Croix;

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- EXTRAIT LE NOM DU FICHIER GIF DE LA MATIERE COURANTE -----------------
// -------------------------------------------------------------------------
void nom_gif_matiere_courante(void) {
  FILE *Fichier;
  char *Buffer=(char *) malloc(256);
  char *Include=(char *) malloc(MAXPATH);

  if (test_fichier(CheminMTEX)==0) {
    free(Include);
    free(Buffer);
    return;
  }

  if (!retourne_include(Include)) {
    free(Include);
    free(Buffer);
    return;
  }

  if ((Fichier=fopen(CheminMTEX,"rt"))==NULL) {
    free(Include);
    free(Buffer);
    return;
  }

  while (!feof(Fichier)) {
    Buffer[0]=NULLC;
    fgets(Buffer,256,Fichier);
    analyse_ligne(Buffer,32);
    if (!strcmp(Argu[0],MatiereCourante)) {
      strcpy(MatiereCouranteGif,Argu[1]);
      break;
    }
  }

  message("Current texture : %s %s",MatiereCourante,MatiereCouranteGif);

  fclose(Fichier);

  free(Include);
  free(Buffer);
}
