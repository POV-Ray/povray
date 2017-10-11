/* ---------------------------------------------------------------------------
*  INTERFA.C
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
#include <GRAPH.H>
#include <STDLIB.H>
#include <STRING.H>
#include <I86.H>
#include <DOS.H>
#include <STDIO.H>
#include <IO.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"


int XMenuD,YMenuD;
int XMenuF,YMenuF;
int CentX,CentY;
int BarreD;
int BarreF;

char *ImageMotif;
int XMotif,YMotif;
byte MotifOk=0;
char CheminMotif[MAXPATH];
byte OptFade=1;

// -----------------------------------------------------------------------
// -- EFFACE BARRE DE BOUTONS DU BAS -------------------------------------
// -----------------------------------------------------------------------
void efface_barre_bouton_bas(int N) {
  g_rectangle(Bt[N].X,Bt[N].Y,Bt[N].X+Bt[N].L,Bt[N].Y+Bt[N].H,FOND,1);
}

// -----------------------------------------------------------------------
// ------- INITIALISE LES VUES FENETRES ----------------------------------
// -----------------------------------------------------------------------
void init_video_structure(void) {
  int i;

  for (i=0;i<=3;i++) {
    Vid[i].Echelle=45;
    Vid[i].Depla.X=0;
    Vid[i].Depla.Y=0;
    Vid[i].Enable=1;
    Vid[i].OptSnap=0;
    Vid[i].SnapX=Vid[i].SnapY=1.0;
    Vid[i].Coord=1;
  }
}

// -----------------------------------------------------------------------
// ------- INITIALISE LES FENETRES EN FCTø RESOLUTION --------------------
// -----------------------------------------------------------------------
void init_vues(byte Draw) {
  register int i;
  set_port(0,0,XMax,YMax);

  if (Draw && Fx4) goto LABEL_REDRAW;

  XMenuD=(OptBtD==1 ? XMax-80:4);    // Position d‚part en X menu droite
  XMenuF=XMenuD+75;                  // Position fin en X menu droite
  
  YMenuD=YMax-70;    // Position d‚part en Y menu bas
  YMenuF=YMax;       // Position fin en Y menu bas

  CentX=(OptBtD==1 ? XMenuD/2:XMenuF+(XMax-XMenuF)/2);
  CentY=YMenuD/2;

  if (Fx4==1) {
    for (i=0;i<=3;i++) {
      Vid[i].WX=(XMax-75-21)/2;  // X Taille d'une fenˆtre
      Vid[i].WY=(YMax-65-21)/2;  // Y Taille d'une fenˆtre
      Vid[i].WXs2=Vid[i].WX/2;
      Vid[i].WYs2=Vid[i].WY/2;
    }

    // Position X/Y fenˆtre 1 en Haut … Gauche
    Vid[0].XF=(OptBtD==1 ? 6:XMenuF+6);     
    Vid[0].YF=6;

    // Position X/Y fenˆtre 2 en Haut … Droite
    Vid[1].XF=(OptBtD==1 ? 12+Vid[1].WX:XMenuF+12+Vid[1].WX);
    Vid[1].YF=6;

    // Position X/Y fenˆtre 3 en Bas … Gauche
    Vid[2].XF=(OptBtD==1 ? 6:XMenuF+6);
    Vid[2].YF=12+Vid[2].WY;

    // Position X/Y fenˆtre 4 en Bas … Droite
    Vid[3].XF=(OptBtD==1 ? 12+Vid[3].WX:XMenuF+12+Vid[3].WX);
    Vid[3].YF=12+Vid[3].WY;

    LABEL_REDRAW:

    g_rectangle(Vid[0].XF- 1,
                Vid[0].WY+6,
                Vid[0].XF+Vid[0].WX+Vid[1].WX+6,
                Vid[0].WY+12,
                FOND,-1); // efface horz entre fenˆtres
    g_rectangle(Vid[0].XF+Vid[0].WX,
                5,
                Vid[0].XF+Vid[0].WX+6,
                Vid[0].WY+Vid[2].WY+12,
                FOND,-1); // efface vert entre fenˆtres
  } else {
    for (i=0;i<=3;i++) {
      Vid[i].WX=XMax-80-11;  // X Taille d'une fenˆtre
      Vid[i].WY=YMax-70-11;  // Y Taille d'une fenˆtre
      Vid[i].WXs2=Vid[i].WX/2;
      Vid[i].WYs2=Vid[i].WY/2;
    }

    // Position X/Y fenˆtre 1 en Haut … Gauche
    Vid[0].XF=(OptBtD==1 ? 6:XMenuF+6);
    Vid[0].YF=6;

    // Position X/Y fenˆtre 2 en Haut … Droite
    Vid[1].XF=(OptBtD==1 ? 6:XMenuF+6);
    Vid[1].YF=6;

    // Position X/Y fenˆtre 3 en Bas … Gauche
    Vid[2].XF=(OptBtD==1 ? 6:XMenuF+6);
    Vid[2].YF=6;

    // Position X/Y fenˆtre 4 en Bas … Droite
    Vid[3].XF=(OptBtD==1 ? 6:XMenuF+6);
    Vid[3].YF=6;
  }

  BarreD=(OptBtD==1 ? 0:Vid[0].XF);      // D‚but de la barre de menus
  BarreF=(OptBtD==1 ? XMenuD:XMax);  // Fin de la barre de menus

  windows(Vid[0].XF,
          Vid[0].YF,
          Vid[0].XF+Vid[0].WX,
          Vid[0].YF+Vid[0].WY,
          0,FFOND);  // gauche haut
  if (Fx4==0) return;

  windows(Vid[1].XF,Vid[1].YF,Vid[1].XF+Vid[1].WX,Vid[1].YF+Vid[1].WY,0,FFOND);  // droite haut
  windows(Vid[2].XF,Vid[2].YF,Vid[2].XF+Vid[2].WX,Vid[2].YF+Vid[2].WY,0,FFOND);  // gauche bas
  windows(Vid[3].XF,Vid[3].YF,Vid[3].XF+Vid[3].WX,Vid[3].YF+Vid[3].WY,0,FFOND);  // droite bas

  for (i=0;i<=3;i++) {
    log_out(i,"Windows %d %d %d %d %d %d %d",i,
               Vid[i].XF,
               Vid[i].YF,
               Vid[i].XF+Vid[i].WX,
               Vid[i].YF+Vid[i].WY,
               Vid[i].WX,
               Vid[i].WY);
  }

  trouve_fenetre(1);
}

// -----------------------------------------------------------------------
// --------- AFFICHE LE DESSIN DU BUREAU ---------------------------------
// -----------------------------------------------------------------------
void affiche_bureau(void) {
  windows(10,YMenuD+4,XMax-XDebutAide-5,YMenuD+17,0,ZFOND);  // message
  windows(XMax-XDebutAide,YMenuD+4,XMax-9,YMenuD+17,0,ZFOND);  // ‚v‚nements
  windows(10,YMax-13-9,XMax-9,YMax-9,0,ZFOND);  // status du bas
}

// -----------------------------------------------------------------------
// --------- AFFICHE CERTAINES DONNEES POUR LE DEBUGGING -----------------
// -----------------------------------------------------------------------
void affiche_donnees(void) {
  register int L;

  select_vue(5,CLIP_ON);
  gprintf( 12,YMax-21,BLANC,ZFOND,1, 6,"%05d",NbObjet);
  gprintf( 50,YMax-21,BLANC,ZFOND,1, 6,"%05d",Selection);
  gprintf( 95,YMax-21,BLANC,ZFOND,1, 2,"O%02d",NbOmni);
  gprintf(120,YMax-21,BLANC,ZFOND,1, 2,"S%02d",NbSpot);
  gprintf(145,YMax-21,BLANC,ZFOND,1, 2,"A%02d",NbArea);
  gprintf(170,YMax-21,BLANC,ZFOND,1, 2,"CL%02d",NbCylLight);
  gprintf(200,YMax-21,BLANC,ZFOND,1, 2,"C%02d",NbCamera);
  gprintf(230,YMax-21,BLANC,ZFOND,1, 5,"%05lukb",(long)((MemoireLibre-MemoirePrise)/1024));
  gprintf(285,YMax-21,BLANC,ZFOND,1, 9,"%dx%d",XMax+1,YMax+1);
  gprintf(345,YMax-21,BLANC,ZFOND,1,12,"%s",FichierSCN);
  gprintf(455,YMax-21,BLANC,ZFOND,1, 3,"%02d",NbPoly+1);

  {
    char Tmp[MAXPATH];
    char Buffer[MAXPATH];

    if (MatiereCourante[0]=='#') {
      split_chemin(Tmp,MatiereCourante+1,5);
    } else {
      strcpy(Tmp,MatiereCourante);
    }
    sprintf(Buffer,"[%s]",Tmp);
    L=XMax-32*6;
    g_rectangle(L,YMax-22,XMax-10,YMax-10,ZFOND,MOTIF);
    text_xy(XMax-largeur_text(Buffer)-15,YMax-23,Buffer,BLANC);
  }
}

// -----------------------------------------------------------------------
// --------- AFFICHE LES BOUTONS PRINCIPAUX ------------------------------
// -----------------------------------------------------------------------
void affiche_boutons_principaux(void) {
  init_bouton( 2,XMenuD+5, 10,66,20,RES_BOUT[1],DROITE,ATTEND,RES_AIDE[1]);
  affiche_bouton(2);

  init_bouton( 3,XMenuD+5, 30,66,20,RES_BOUT[2],DROITE,ATTEND,RES_AIDE[2]);
  affiche_bouton(3);

  init_bouton(10,XMenuD+5, 50,66,20,RES_BOUT[3],DROITE,ATTEND,RES_AIDE[3]);
  affiche_bouton(10);

  init_bouton( 9,XMenuD+5, 70,66,20,RES_BOUT[4],DROITE,ATTEND,RES_AIDE[4]);
  affiche_bouton(9);

  init_bouton(11,XMenuD+5, 90,66,20,RES_BOUT[5],DROITE,ATTEND,RES_AIDE[5]);
  affiche_bouton(11);

  init_bouton(13,XMenuD+5,110,66,20,RES_BOUT[6],DROITE,ATTEND,RES_AIDE[6]);
  affiche_bouton(13);

  init_bouton(15,XMenuD+5,130,66,20,RES_BOUT[7],DROITE,ATTEND,RES_AIDE[7]);
  affiche_bouton(15);

  init_bouton(18,XMenuD+5,150,66,20,RES_BOUT[12],DROITE,ATTEND,RES_AIDE[149]);
  affiche_bouton(18);

  coche_bouton_menu(0);
}

// -----------------------------------------------------------------------
// --------- AFFICHE LES BOUTONS FONCTIONS GRAPHIQUES --------------------
// -----------------------------------------------------------------------
void affiche_boutons_graphique(void) {
  // --------- Bouton r‚duire
  init_bouton(4,XMax-32,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[93]);
  affiche_bouton(4);
  g_rectangle(Bt[4].X+7,Bt[4].Y+11,Bt[4].X+17,Bt[4].Y+13,NOIR,0);  // horizontal
  g_rectangle(Bt[4].X+8,Bt[4].Y+12,Bt[4].X+16,Bt[4].Y+12,BLANC,0);

  // --------- Bouton grossir
  init_bouton(5,XMax-57,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[94]);
  affiche_bouton(5);
  g_rectangle(Bt[5].X+ 7,Bt[5].Y+11,Bt[5].X+17,Bt[5].Y+13,NOIR,0);  // horizontal
  g_rectangle(Bt[5].X+11,Bt[5].Y+ 7,Bt[5].X+13,Bt[5].Y+17,NOIR,0);  // vertical
  g_rectangle(Bt[5].X+ 8,Bt[5].Y+12,Bt[5].X+16,Bt[5].Y+12,BLANC,0);
  g_rectangle(Bt[5].X+12,Bt[5].Y+ 8,Bt[5].X+12,Bt[5].Y+16,BLANC,0);
  
  // --------- Bouton recentrer
  init_bouton(6,XMax-82,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[95]);
  affiche_bouton(6);
  g_ligne(Bt[6].X+12,Bt[6].Y+ 4,Bt[6].X+12,Bt[6].Y+20,15); // vertical
  g_ligne(Bt[6].X+ 4,Bt[6].Y+12,Bt[6].X+20,Bt[6].Y+12,15); // horizontal
  g_ligne(Bt[6].X+17,Bt[6].Y+ 7,Bt[6].X+ 7,Bt[6].Y+17,15); // diagonal
  
  // --------- Bouton s‚lection zone
  init_bouton(7,XMax-107,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[96]);
  affiche_bouton(7);
  type_motif_ligne(2);
  g_rectangle(Bt[7].X+4,Bt[7].Y+4,Bt[7].X+20,Bt[7].Y+20,15,0);
  type_motif_ligne(0);
  
  // --------- Bouton s‚lection d‚placement
  init_bouton(12,XMax-132,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[97]);
  affiche_bouton(12);

  move_to(Bt[12].X+12,Bt[12].Y+12);
  ligne_rel(7,-7,15);
  ligne_rel(0,2,15);
  ligne_rel(-2,-2,15);
  ligne_rel(1,0,15);
  move_to(Bt[12].X+12,Bt[12].Y+12);
  ligne_rel(-7,-7,15);
  ligne_rel(2,0,15);
  ligne_rel(-2,2,15);
  ligne_rel(0,-1,15);
  move_to(Bt[12].X+12,Bt[12].Y+12);
  ligne_rel(7,7,15);
  ligne_rel(-2,0,15);
  ligne_rel(2,-2,15);
  ligne_rel(0,1,15);
  move_to(Bt[12].X+12,Bt[12].Y+12);
  ligne_rel(-7,7,15);
  ligne_rel(0,-2,15);
  ligne_rel(2,2,15);
  ligne_rel(-1,0,15);
  move_to(Bt[12].X+12,Bt[12].Y+12+3);
  ligne_rel(0,5,15);
  ligne_rel(-1,-1,15);
  ligne_rel(2,0,15);
  move_to(Bt[12].X+12,Bt[12].Y+12-3);
  ligne_rel(0,-5,15);
  ligne_rel(-1,1,15);
  ligne_rel(2,0,15);
  move_to(Bt[12].X+12-3,Bt[12].Y+12);
  ligne_rel(-5,0,15);
  ligne_rel(1,-1,15);
  ligne_rel(0,2,15);
  move_to(Bt[12].X+12+3,Bt[12].Y+12);
  ligne_rel(5,0,15);
  ligne_rel(-1,-1,15);
  ligne_rel(0,2,15);
  g_rectangle(Bt[12].X+12-1,Bt[12].Y+12-1,Bt[12].X+12+1,Bt[12].Y+12+1,15,1);

  // --------- Bouton agrandissement de fenˆtre
  init_bouton(14,XMax-157,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[98]);
  affiche_bouton(14);
  type_motif_ligne(2);
  g_rectangle(Bt[14].X+7,Bt[14].Y+7,Bt[14].X+17,Bt[14].Y+17,15,0);
  type_motif_ligne(0);
  g_rectangle(Bt[14].X+4,Bt[14].Y+4,Bt[14].X+20,Bt[14].Y+20,15,0);

  // --------- Bouton sauvegarde de la scŠne
  init_bouton(16,XMax-218,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[16]);
  affiche_bouton(16);
  affiche_icone(XMax-218+3,YMenuD+23,1,Disquette);

  // --------- Bouton lancement du rendu
  init_bouton(17,XMax-243,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[5]);
  affiche_bouton(17);
  affiche_icone(XMax-243+2,YMenuD+22,1,Oeil);

  // --------- Bouton rafraŒchir l'‚cran
  init_bouton(1,XMax-182,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[140]);
  affiche_bouton(1);
  affiche_icone(XMax-182+5,YMenuD+24,1,IconeRafraichit);

  // --------- Bouton rotate
  init_bouton(20,XMax-303,YMenuD+20,24,25,"",CENTRE,ATTEND,"Set or add the rotate vector");
  affiche_bouton(20);
  affiche_icone(XMax-303+3,YMenuD+23,1,RotateObjet);

  // --------- Bouton scale
  init_bouton(19,XMax-328,YMenuD+20,24,25,"",CENTRE,ATTEND,"Set or add the scale vector");
  affiche_bouton(19);
  affiche_icone(XMax-328+3,YMenuD+23,1,ScaleObjet);

  // --------- Bouton translate
  init_bouton(21,XMax-278,YMenuD+20,24,25,"",CENTRE,ATTEND,"Set or add the translate vector");
  affiche_bouton(21);
  affiche_icone(XMax-278+4,YMenuD+24,1,TranslateObjet);

  // --------- Bouton coordonn‚es manuelles
  init_bouton(8,XMax-353,YMenuD+20,24,25,"",DROITE,ATTEND,RES_AIDE[101]);
  affiche_bouton(8);
  affiche_icone(XMax-353+3,YMenuD+24-1,1,IconeInterrogation);
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE -------------------------------------------
// -----------------------------------------------------------------------
void interface(byte ShowPal) {
  palette_noire();
  ecran_off(1);
  windows(0,0,XMax,YMax,1,FOND); // g‚n‚rale
  init_vues(0);

  border(XMenuD,5,XMenuF,YMenuD-5,FOND,1); // menu droit
  border(5,YMenuD-2,XMax-5,YMenuD+65,FOND,1); // menu bas

  affiche_bureau();
  affiche_donnees();

  affiche_boutons_principaux();
  affiche_boutons_graphique();
  
  init_police(0,0);

  
  // --------- Case de s‚lection
  init_case(0,XMenuD+5,YMenuD-22,"Selection",0,RES_AIDE[99]);
  affiche_case(0);

  g_border(NOIR);

  if (NbCouleurs==256) {
    init_bouton(40,9,YMenuD+19,123,27,"",0,ATTEND,"");
    affiche_bouton(40);
    affiche_icone(11,YMenuD+21,0,LogoChroma);
  }

  message_aide(0,YMenuD+7,JAUNE,13,"Welcome to POVLAB !",BOUTON);

  ecran_off(0);

  init_graphic_mouse(CentX,CentY);
  forme_mouse(MS_FLECHE);

  if (ShowPal) fading_palette(FADEOUT);
}

// -----------------------------------------------------------------------
// --------- AFFICHE LE LOGO DU LOGICIEL ---------------------------------
// -----------------------------------------------------------------------
void show_logo_logiciel(void) {
  /*
  char OldGraphic[5];
  char Buffer[255];
  char *Image;
  int i=0,X,Y;

  if (NbCouleurs==256) {
    strcpy(OldGraphic,Resolution);
    efface_buffer_clavier();
    bouton_resolution("640",0);
    g_border(NOIR);
    palette_noire();

    decompresse_gif(&X,&Y,"SYSTEM\\MODELLER.BIN","",1);
    if ((Image=(char *) mem_alloc((size_t) (6+(X*Y))))==NULL) {
      f_erreur("Can't allocate buffer");
      return;
    }
    i=decompresse_gif(&X,&Y,"SYSTEM\\MODELLER.BIN",Image,0);
    if (i) {
      put_palette();
      affiche_gif(0,0,X,Y,Image,1,0,16);
      select_vue(5,CLIP_ON);
      text_xy(320-largeur_text(Buffer)/2,430,Buffer,BLANC);
      for (i=0;i<=2500;i++) {
        delay(3);
        if (kbhit()) { getch(); break; }
      }
      fading_palette(FADEIN);
      strcpy(Resolution,OldGraphic);
      bouton_resolution(Resolution,0);
    }
    mem_free((char *) Image,6+(X*Y));
  }
  */
}

// -----------------------------------------------------------------------
// --------- AFFICHE A PROPOS DE -----------------------------------------
// -----------------------------------------------------------------------
void a_propos_de (void) {
  int i;
  int X1=CentX-(NbCouleurs==16 ? 120:225);
  int X2=CentX+(NbCouleurs==16 ? 125:225);
  int Y1=CentY-127;
  int Y2=CentY+125;

  sprintf(StrBoite[0],"%s version %s build %s.",NomLogiciel,VerLogiciel,VERSION_MODELEUR);
  sprintf(StrBoite[1],"Compiled %s.",__DATE__);
  strcpy(StrBoite[2]," ");
  sprintf(StrBoite[3],"%s %s %s.",RES_COPY[1],RES_COPY[2],RES_COPY[3]);
  sprintf(StrBoite[4],"Production (P) 1994-%s, by Denis Olivier.",RES_COPY[4]);
  sprintf(StrBoite[5],"Copyright (C) 1994-%s, ChromaGraphics.",RES_COPY[4]);
  strcpy(StrBoite[6]," ");
  strcpy(StrBoite[7],"Persistence Of Vision raytracer");
  sprintf(StrBoite[8],"Production 1993-%s, by POV-Team, USA.",RES_COPY[4]);
  sprintf(StrBoite[9],"Copyright 1993-%s, POV-Team, USA.",RES_COPY[4]);
  strcpy(StrBoite[10]," ");
  strcpy(StrBoite[11],"Lens Flares based on NK-Flares version 4.0");
  sprintf(StrBoite[12],"Copyright 1998-%s, Nathan Kopp.",RES_COPY[4]);

  forme_mouse(MS_FLECHE);
  g_fenetre(X1,Y1,X2,Y2,RES_COPY[0],AFFICHE);

  if (NbCouleurs==256) {
    relief(X1+9,Y1+28,X1+10+Logo[0],Y1+29+Logo[2],1);
    affiche_icone(X1+10,Y1+29,0,Logo);
  }

  i=NbCouleurs==256 ? 220:10;
  text_xy(X1+i,Y1+ 30,StrBoite[ 0],NOIR);
  text_xy(X1+i,Y1+ 45,StrBoite[ 1],NOIR);
  text_xy(X1+i,Y1+ 60,StrBoite[ 2],NOIR);
  text_xy(X1+i,Y1+ 75,StrBoite[ 3],NOIR);
  text_xy(X1+i,Y1+ 90,StrBoite[ 4],NOIR);
  text_xy(X1+i,Y1+105,StrBoite[ 5],NOIR);
  text_xy(X1+i,Y1+120,StrBoite[ 6],NOIR);
  text_xy(X1+i,Y1+135,StrBoite[ 7],NOIR);
  text_xy(X1+i,Y1+150,StrBoite[ 8],NOIR);
  text_xy(X1+i,Y1+165,StrBoite[ 9],NOIR);
  text_xy(X1+i,Y1+170,StrBoite[10],NOIR);
  text_xy(X1+i,Y1+185,StrBoite[11],NOIR);
  text_xy(X1+i,Y1+200,StrBoite[12],NOIR);
  
  bouton_dialog(X1,X2,Y2,1,0);

  for (i=0;i<=2500;i++) {
    delay(3);
    if (bouton_dialog(X1,X2,Y2,0,0)!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
}

// -----------------------------------------------------------------------
// --------- CHARGE LE MOTIF EN COURS ------------------------------------
// -----------------------------------------------------------------------
void charge_motif(byte Travail) {
  if (Travail==SAUVE && MotifOk) {
    if (NbCouleurs==16) { MotifOk=0; return; }
    if (!decompresse_gif(&XMotif,&YMotif,CheminMotif,"",1)) {
      MotifOk=0;
      return;
    }
    if ((ImageMotif=(char *) mem_alloc((size_t) (6+(XMotif*YMotif))))==NULL) {
      f_erreur("Can't allocate buffer for the|interface pattern.");
      MotifOk=0;
      return;
    }
    if (!decompresse_gif(&XMotif,&YMotif,CheminMotif,ImageMotif,0)) return;
    affiche_gif(0,0,XMotif,YMotif,ImageMotif,2,1,0);
    return;
  }
  if (Travail==EFFACE && MotifOk==1) mem_free((char *) ImageMotif,6+(XMotif*YMotif));
}

// -------------------------------------------------------------------------
// -- LECTURE DES MOTIFS ---------------------------------------------------
// -------------------------------------------------------------------------
byte choix_motif(void) {
  struct find_t find_t;
  int X1=CentX-146;
  int X2=CentX+146;
  int Y1=CentY-125;
  int Y2=CentY+120;
  int XA=150,YA=150;
  char NomGif[MAXPATH];
  register int i,j,Nb=-1;
  int Vu,N,X,Y;
  char Chemin[MAXPATH];
  char *Image;
  struct retour_selecteur Val;
  char *NomMotif[50];
  byte NB_MOTIF_MAX=50;

  if (NbCouleurs!=256) {
    f_erreur("16 colors interface, unavailable function.|Switch to 256 colors interface.");
    return 0;
  }
  sprintf(Chemin,"%s%s\\PATTERN\\*.GIF",NewLecteur,NewChemin);

  if (!_dos_findfirst(Chemin,0x10,&find_t)) {
    do {
      Nb++;
      NomMotif[Nb]=(char *) mem_alloc(strlen(find_t.name)+1);
      strcpy(NomMotif[Nb],find_t.name);
    } while(!_dos_findnext(&find_t) && Nb<NB_MOTIF_MAX);
  } else {
    beep_erreur();
    strcpy(StrBoite[0],"Invalid path");
    strcpy(StrBoite[1],"Pattern files missing");
    strcpy(StrBoite[2],"in the directory");
    sprintf(StrBoite[3],"%s.",Chemin);
    g_boite_ONA(CentX,CentY,3,CENTRE,0);
    return 0;
  }

  message("%d patterns loaded",Nb+1);

  g_fenetre(X1,Y1,X2,Y2,"PATTERN SELECTOR",AFFICHE);
  windows(X1+10,Y2-19,X1+150,Y2-5,0,ZFOND);  // texte
  windows(X2-10-XA,Y1+35,X2-10,Y1+35+YA,0,FOND); // cadre GIF

  init_case(11,X1+200,Y2-19,"No pattern",MotifOk ? 0:1,"");
  affiche_case(11);

  init_bouton(40,X2-70,Y2-50,60,20,"Ok",CENTRE,ATTEND,"Quit");
  affiche_bouton(40);

  init_selecteur(0,X1,Y1,12,Nb+1,NomMotif,10);
  affiche_selecteur(0);

  while (1) {
    Val=test_selecteur(0);
    N=Val.Num;
    if (Val.Ok==13) { i=0; break; }
    if (Val.Ok==27) { i=1; break; }
    test_case(11,11);
    if ((i=test_bouton(40,40))!=-1) { i-=40; break; }
    if (NbCouleurs==256 && Vu!=N) {
      sprintf(NomGif,"%s%s\\PATTERN\\%s",NewLecteur,NewChemin,NomMotif[N]);
      i=decompresse_gif(&X,&Y,NomGif,"",1);
      gprintf(X1+12,Y2-18,0,ZFOND,1,12,"Format X=%d Y=%d",X,Y);
      if ((Image=(char *) mem_alloc((size_t) (6+(X*Y))))!=NULL) {
        if ((i=decompresse_gif(&X,&Y,NomGif,Image,0))) {
          set_port(X2-10-XA,Y1+35,X2-10-1,Y1+35+YA-1);
          affiche_gif(0,0,X,Y,Image,1,1,0);
          set_port(0,0,XMax,YMax);
          mem_free((char *) Image,6+(X*Y));
        }
      }
    }
    Vu=N;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (Cc[11].Croix) {
    if (MotifOk) {
      charge_motif(EFFACE);
      MotifOk=0;
      CheminMotif[0]=NULLC;
      bouton_resolution(Resolution,1);
      redessine_fenetre(5,1);
      i=1;
    }
  } else {
    if (MotifOk==0) i=0;
  }

  if (i==0 && !Cc[11].Croix) {
    sprintf(Chemin,"%s%s\\PATTERN\\%s",NewLecteur,NewChemin,NomMotif[N]);
    if (strinstr(0,CheminMotif,Chemin)!=0) {
      strcpy(CheminMotif,Chemin);
      MotifOk=1;
      charge_motif(EFFACE);
      charge_motif(SAUVE);
      bouton_resolution(Resolution,1);
      redessine_fenetre(5,1);
    } else {
      i=1;
    }
  }

  if (Nb>-1) {
    for (j=0;j<=Nb;j++) {
      mem_free((char *) NomMotif[j],strlen(NomMotif[j])+1);
    }
  }

  return (byte) (i==0 ? 1:0);
}

