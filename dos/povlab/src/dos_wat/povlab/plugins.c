/* ---------------------------------------------------------------------------
*  PLUGINS.C
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

#include <DIRECT.H>
#include <MATH.H>
#include <FLOAT.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <STRING.H>
#include <PROCESS.H>
#include <DOS.H>
#include <TIME.H>
#include <GRAPH.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"

#define Nb_Radio 8

struct {
  char Titre[60];
  char Copyright[100];
  int WX;
  int WY;
  int NbCmd;
  struct {
    byte Type;
    byte Grp;
  } Cmd[40];
  struct {
    int D,F;
    int G;
  } Radio[Nb_Radio];
} Plg;

// ----------------------------------------------------------------------------
// --------- CHARGE LES PLUGINS EXISTANTS -------------------------------------
// ----------------------------------------------------------------------------
byte choix_plugins(void) {
  char NomFichier[MAXPATH];
  char Buffer[MAXPATH];
  char *Spec[2]={"1","*.EXE"};

  chdir("PLUGINS");

  strcpy(Buffer,selection_fichier(100,100,"PLUGINS",Spec));
  if (Buffer[0]==27) return 0;
  strcpy(NomFichier,Buffer);
  strupr(NomFichier);
  NomFichier[strinstr(0,NomFichier,".EXE")]=NULLC;
  execute_plugins(NomFichier);
  return 1;
}

// ----------------------------------------------------------------------------
// --------- INTIALISE LA STRUCTURE POUR LES PLUGINS --------------------------
// ----------------------------------------------------------------------------
void init_structure_plg(void) {
  register int i;

  Plg.NbCmd=0;
  for (i=0;i<=2;i++) {
    Plg.Radio[i].D=+100;
    Plg.Radio[i].F=-100;
    Plg.Radio[i].G=0;
  }
  strcpy(Plg.Copyright,"");
  strcpy(Plg.Titre,"");
}

// ----------------------------------------------------------------------------
// --------- EXECUTE UN SCRIPT PLUGINS ----------------------------------------
// ----------------------------------------------------------------------------
byte execute_plugins(char *Process) {
  int i,j,k,X1,Y1,X2,Y2;
  FILE *Fichier;
  char Buffer[256];
  char Tmp[256];
  byte Ok=1;
  char EXE[MAXPATH];
  char PLG[MAXPATH];
  char INC[MAXPATH];
  byte OldVideo;

  Plg.NbCmd=0;
  init_structure_plg();
  chdir("PLUGINS");

  strcpy(PLG,Process);
  strcpy(EXE,Process);
  strcpy(INC,Process);
  strcat(PLG,".PLG");
  strcat(EXE,".EXE");
  strcat(INC,".INC");

  #if !WINDOWS
  spawnl(P_WAIT,EXE,EXE,"/ASK",NULL);
  #endif

  if ((Fichier=fopen(PLG,"r+t"))==NULL) {
    f_erreur("Can't open plugins script %s",PLG);
    Ok=0;
    goto LABEL_FIN_PLG;
  }

  while (!feof(Fichier)) {
    fgets(Buffer,256,Fichier);
    Buffer[strinstr(0,Buffer,"\n")]=NULLC;
    if (Buffer[0]!=';'&& Buffer[0]!=NULLC && Buffer[0]!='\n') {
      k=analyse_ligne(Buffer,32);
      for (j=0;j<=k;j++) {
        for (i=0;i<=strlen(Argu[j]);i++) {
          if (Argu[j][i]=='_') Argu[j][i]=32;
        }
      }
      if (!strinstr(0,Buffer,"TITLE:")) strcpy(Plg.Titre,Argu[1]);
      if (!strinstr(0,Buffer,"COPYRIGHT:")) strcpy(Plg.Copyright,Argu[1]);
      if (!strinstr(0,Buffer,"WINDOW:")) {
        Plg.WX=atoi(Argu[1])/2;
        Plg.WY=atoi(Argu[2])/2;
        X1=CentX-Plg.WX;
        Y1=CentY-Plg.WY;
        X2=CentX+Plg.WX;
        Y2=CentY+Plg.WY;
      }
      if (!strinstr(0,Buffer,"TEXTZONE:")) {
        Plg.NbCmd++;
        init_texte(Plg.NbCmd,X1+atoi(Argu[1]),Y1+atoi(Argu[2]),Argu[3],Argu[4],10,Argu[5]);
        Plg.Cmd[Plg.NbCmd].Type='T';
      }
      if (!strinstr(0,Buffer,"CASE:")) {
        Plg.NbCmd++;
        init_case(Plg.NbCmd+10,X1+atoi(Argu[1]),Y1+atoi(Argu[2]),Argu[3],atoi(Argu[4]),Argu[5]);
        Plg.Cmd[Plg.NbCmd].Type='C';
      }
      if (!strinstr(0,Buffer,"MESSAGE:")) message(Argu[1]);
      if (!strinstr(0,Buffer,"RADIO:")) {
        Plg.NbCmd++;
        init_pastille(Plg.NbCmd,X1+atoi(Argu[1]),Y1+atoi(Argu[2]),Argu[3],atoi(Argu[4]),Argu[5]);
        Plg.Cmd[Plg.NbCmd].Type='P';
        Plg.Cmd[Plg.NbCmd].Grp=atoi(Argu[6])-1;
        Plg.Radio[atoi(Argu[6])-1].G=1;
      }
    }
  }

  fclose(Fichier);

  forme_mouse(MS_FLECHE);
  GMouseOn();

  parse_ascii_watcom(Plg.Titre);
  parse_ascii_watcom(Plg.Copyright);
  g_fenetre(X1,Y1,X2,Y2,Plg.Titre,AFFICHE);
  text_xy(X1+9,Y1+20,Plg.Copyright,NOIR);

  for (i=1;i<=Plg.NbCmd;i++) {
    if (Plg.Cmd[i].Type=='P') {
       j=Plg.Cmd[i].Grp;
       if (i<Plg.Radio[j].D) Plg.Radio[j].D=i;
       if (i>Plg.Radio[j].F) Plg.Radio[j].F=i;
    }
  }

  for (i=1;i<=Plg.NbCmd;i++) {
    switch (Plg.Cmd[i].Type) {
      case 'T': place_zone_texte(i); break;
      case 'C': affiche_case(i+10); break;
      case 'P': affiche_pastille(i); break;
    }
  }

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    if (Plg.Radio[0].G) test_groupe_pastille(Plg.Radio[0].D,Plg.Radio[0].F);
    if (Plg.Radio[1].G) test_groupe_pastille(Plg.Radio[1].D,Plg.Radio[1].F);
    if (Plg.Radio[2].G) test_groupe_pastille(Plg.Radio[2].D,Plg.Radio[2].F);
    for (i=1;i<=Plg.NbCmd;i++) {
      switch (Plg.Cmd[i].Type) {
        case 'T': test_texte(i,i); break;
        case 'C': test_case(i+10,i+10); break;
      }
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
    if (sequence_sortie()) { i=1; break; }
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i) goto LABEL_FIN_PLG;

  GMouseOff();
  set_port(0,0,XMax,YMax);
  g_bitmap(0,0,XMax,YMax,SAUVE,4);

  for (i=0;i<=255;i++) Buffer[i]=32;
  strcpy(Buffer,"");

  for (i=1;i<=Plg.NbCmd;i++) {
    switch(Plg.Cmd[i].Type) {
      case 'T':
        sprintf(Tmp,"%.5g",atof(ZTexte[i].Variable));
        strcat(Buffer,Tmp);
        strcat(Buffer," ");
        break;
      case 'C':
        sprintf(Tmp,"%d",Cc[i+10].Croix);
        strcat(Buffer,Tmp);
        strcat(Buffer," ");
        break;
      case 'P':
        j=Plg.Cmd[i].Grp;
        if (Plg.Radio[j].G) {
          k=quelle_pastille_dans_groupe(Plg.Radio[j].D,Plg.Radio[j].F);
          k-=Plg.Radio[j].D;
          sprintf(Tmp,"%d",k);
          strcat(Buffer,Tmp);
          strcat(Buffer," ");
          Plg.Radio[j].G=0;
        }
        break;
    }
  }
  

  #if !WINDOWS
  log_out(0,"Plugin %s -> %s",EXE,Buffer);
  log_out(1,"Plugin %s -> %s",EXE,INC);

  OldVideo=get_video_mode();
  spawnl(P_WAIT,EXE,EXE,Buffer,NULL);

  if (OldVideo!=get_video_mode()) {
    palette_noire();
    init_gmode(1);
    palette_noire();
    interface(0);

    GMouseOff();
    g_bitmap(0,0,XMax,YMax,AFFICHE,4);
    g_bitmap(0,0,XMax,YMax,EFFACE,4);
    put_palette();
  }

  lecture_fichier(INC,0,1,0);
  #endif
  
  LABEL_FIN_PLG:

  if (!DEBUG) remove(INC);
  if (!DEBUG) remove(PLG);
  set_disk(NewLecteur[0]-65);
  chdir(NewChemin);
  OkSelect=0;
  return Ok;
}
