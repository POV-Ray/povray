/* ---------------------------------------------------------------------------
*  BARRE.C
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

#include <STRING.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "MALLOC.H"
#include "GLIB.H"

#if !WINDOWS
#include <GRAPH.H>
#else
#include <WINDOWS.H>
extern HDC hdc;
#endif

int HY;
struct texte_barre Menu[NbMenu];

// ---------------------------------------------------------------------------
// --------- AFFICHAGE D'UN MENU DEROULANT -----------------------------------
// ---------------------------------------------------------------------------
int deroule_menu(byte N) {
  int X1=Menu[N].Debut;
  int X2=X1+150;
  int Y1=HY;
  int YD=HY+5;
  int Y2=YD+(Menu[N].Nb)*(HY-5)+5;
  int MX,MY,Num1=-1,Num2=-1,Ok=-1;
  register int i;
  char *Bitmap;
  long Taille;
  #if WINDOWS
  HBITMAP hBitMap;
  #endif

  GMouseOff();
  #if !WINDOWS
  Taille=_imagesize(X1+1,YD+1*(HY-5),X2-25,YD+(1+1)*(HY-5)+1);
  Bitmap=(char *) mem_alloc(Taille);
  g_bitmap(X1,Y1,X2,Y2,SAUVE,2);
  #else
  hBitMap=CreateCompatibleBitmap(hdc,X2-X1,Y2-Y1);
  #endif
  windows(X1,Y1,X2,Y2,1,FOND);
  init_police(0,2);

  for (i=0;i<Menu[N].Nb;i++) {
    parse_ascii_watcom(Menu[N].Choix[i]);
    if (Menu[N].Choix[i][0]=='#') {
      g_ligne(X1+1,YD+i*(HY-5)+6,X2-2,YD+i*(HY-5)+6,OMBRE);
      g_ligne(X1+2,YD+i*(HY-5)+8,X2-2,YD+i*(HY-5)+8,ECLAIRE);
    } else {
      if (Menu[N].Enable[i]==0) {
        text_xy(Menu[N].Debut+6,YD+i*(HY-5),Menu[N].Choix[i],NbCouleurs==16 ? BLANC:20);
        text_xy(Menu[N].Debut+5,YD+i*(HY-5)-1,Menu[N].Choix[i],NbCouleurs==16 ? OMBRE:18);
      } else {
        text_xy(Menu[N].Debut+5,YD+i*(HY-5),Menu[N].Choix[i],TEXTE);
      }
    }
    if (Menu[N].Case[i][0]) {
      if (Menu[N].Case[i][0]==1) {
        init_case(1,X2-20,YD+i*(HY-5)+2,"",Menu[N].Case[i][1],"");
        affiche_case(1);
      } else {
        init_pastille(1,X2-20,YD+i*(HY-5)+2,"",Menu[N].Case[i][1],"");
        affiche_pastille(1);
      }
    }
  }

  init_police(0,0);
  GMouseOn();
  while (MouseB());

  while (!sequence_sortie()) {
    MX=gmx_v();
    MY=gmy_v();
    if (MX>X1 && MX<X2 && MY>HY && MY<Y2) {
      Num1=(MY-Y1-5)/(HY-5);
      Num1=(Num1>=Menu[N].Nb ? Menu[N].Nb-1:Num1);
      if (Num1!=Num2 && Num1>=0 && MouseB()==0) {
        GMouseOff();
        if (Num2>=0 && (Menu[N].Choix[Num2][0]!='#' && Menu[N].Enable[Num2]==1)) {
          //g_rectangle(X1+2,YD+Num2*(HY-5),X2-25,YD+(Num2+1)*(HY-5)+1,FOND,0);
          #if WINDOWS
          draw_bitmap(hdc,X1+1,YD+Num2*(HY-5));
          #else
          _putimage(X1+1,YD+Num2*(HY-5),Bitmap,_GPSET);
          #endif
        }
        if (Menu[N].Choix[Num1][0]!='#' && Menu[N].Enable[Num1]==1) {
          #if !WINDOWS
          _getimage(X1+1,YD+Num1*(HY-5),X2-25,YD+(Num1+1)*(HY-5)+1,Bitmap);
          #else
          hBitMap=CreateCompatibleBitmap(hdc,(X2-25)-(X1+1),
                                         (YD+(Num1+1)*(HY-5)+1)-(YD+Num1*(HY-5)));
          #endif
          relief(X1+1,YD+Num1*(HY-5),X2-25,YD+(Num1+1)*(HY-5)+1,0);
          message_aide(0,YMenuD+7,JAUNE,13,Menu[N].Aide[Num1],BARRE);
        }
        GMouseOn();
        Num2=Num1;
      }
      if (MouseB()==1 && Num1>=0 && Num1<Menu[N].Nb && Menu[N].Enable[Num1]==1) {
        Ok=N*256+(Num1+1);
        while (MouseB());
        goto SORTIE_DEROULE_MENU;
      }
    } else {
      if (MouseB()==1) { Ok=0; break; }
    }
  }

  SORTIE_DEROULE_MENU:
  GMouseOff();
  g_bitmap(X1,Y1,X2,Y2,AFFICHE,2);
  g_bitmap(X1,Y1,X2,Y2,EFFACE,2);
  mem_free(Bitmap,Taille);
  return Ok;
}

// ---------------------------------------------------------------------------
// --------- INTIALISE UNE LIGNE DE MENU -------------------------------------
// ---------------------------------------------------------------------------
void forme_barre(byte MN,byte BN,char *Texte,byte Case,byte OnOff,byte Enable,char *Aide) {
  Menu[MN].Case[BN][0]=Case;
  Menu[MN].Case[BN][1]=OnOff;
  Menu[MN].Enable[BN]=(Enable>0 ? 1:0);
  strcpy(Menu[MN].Choix[BN],Texte);
  if (Aide[0]!=NULL) {
    strcpy(Menu[MN].Aide[BN],Aide);
  } else {
    strcpy(Menu[MN].Aide[BN],Texte);
  }
}

// ----------------------------------------------------------------------------
// --------- AFFICHAGE DE LA BARRE DE MENUS -----------------------------------
// ----------------------------------------------------------------------------
void affiche_barre_menu(void) {
  register int HY=hauteur_text("Gq")+7,i;
  int X,Y,P=-10;

  forme_mouse(MS_FLECHE);
  windows(BarreD,0,BarreF,HY,1,FOND);
  affiche_icone(BarreF-100,2,1,POVLAB);

  if (NbCouleurs==256) {
    windows(BarreF-109+P,HY+1,BarreF+P,HY+110,1,FOND);
    relief(BarreF-105+P,HY+5,BarreF-4+P,HY+106,1); // cadre GIF
    if (MatiereCouranteGif[0]) {
      char *Image;

      if (test_fichier(MatiereCouranteGif)) {
        Image=(char *) malloc(10006);
        decompresse_gif(&X,&Y,MatiereCouranteGif,Image,0);
        affiche_gif(BarreF-104+P,HY+6,100,100,Image,1,1,0);
        free(Image);
      }
    } else {
      affiche_icone(BarreF-104+P,HY+6,0,PasDeTexture);
    }
  }

  init_police(0,2);
  i=init_barre();

  for (i=0;i<NbMenu;i++) {
    text_xy(Menu[i].Debut,2,Menu[i].Texte,TEXTE);
  }

  init_police(0,0);
}

// ----------------------------------------------------------------------------
// --------- AFFICHAGE D'UNE BARRE DE MENU VOLANTE ----------------------------
// ----------------------------------------------------------------------------
int barre_menu(void) {
  register int MX,MY,i,Ok=0;

  GMouseOff();
  init_police(0,2);
  i=init_barre();
  init_police(0,0);
  HY=hauteur_text("Gq")+7;
  g_bitmap(BarreD,0,BarreF,HY+111,SAUVE,1);
  affiche_barre_menu();

  MX=gmx_v();
  MY=gmy_v();
  
  while(MX>BarreD && MX<BarreF && MY<HY) {
    GMouseOn();
    MX=gmx_v();
    MY=gmy_v();
    for (i=0;i<NbMenu;i++) {
      if (MouseB()==1 && MX>Menu[i].Debut && MX<Menu[i].Fin) {
        Ok=deroule_menu(i);
        if (Ok) goto SORTIE_BARRE; else break;
      }
    }
  }

  SORTIE_BARRE:
  
  GMouseOff();
  g_bitmap(BarreD,0,BarreF,HY+111,AFFICHE,1);
  g_bitmap(BarreD,0,BarreF,HY+111,EFFACE,1);
  return Ok;
}

// ----------------------------------------------------------------------------
// --------- TEST ZONE DE LA BARRE DE MENU VOLANTE ----------------------------
// ----------------------------------------------------------------------------
int test_barre_menu(byte Forme,byte Retour) {
  register int MSX=gmx_v();
  register int MSY=gmy_v();
  int Ok=0;

  if (MSX>BarreD && MSX<BarreF && MSY==0) {
    if (CURSEUR!=MS_FLECHE) forme_mouse(MS_FLECHE);
    if (Retour) Ok=barre_menu(); else Ok=1;
    if (CURSEUR!=Forme) forme_mouse(Forme);
  }

  return Ok;
}

// ----------------------------------------------------------------------------
// --------- AFFICHAGE D'UNE BARRE DE MENU VOLANTE ----------------------------
// ----------------------------------------------------------------------------
byte init_barre(void) {
  strcpy(Menu[0].Texte,RES_MENU[5]);
  Menu[0].Debut=BarreD+10;
  Menu[0].Fin=Menu[0].Debut+largeur_text(Menu[0].Texte);
  forme_barre(0, 0,RES_COPY[0],0,0,1,RES_AIDE[8]);
  forme_barre(0, 1,"#",0,0,1,"");
  forme_barre(0, 2,RES_MENU[0],0,0,1,RES_AIDE[9]);
  //forme_barre(0, 3,RES_MENU[1],0,0,1,RES_AIDE[10]);
  forme_barre(0, 3,RES_MENU[32],0,0,1,RES_AIDE[118]);
  forme_barre(0, 4,RES_MENU[33],1,MSMouse,1,RES_AIDE[119]);
  forme_barre(0, 5,RES_MENU[34],0,MSMenu,1,RES_AIDE[120]);
  forme_barre(0, 6,RES_MENU[58],0,0,(NbObjet ? 1:0),RES_AIDE[142]);
  forme_barre(0, 7,"#",0,0,1,"");
  forme_barre(0, 8,"Run picture viewer",0,0,1,"Run your favorite pictures viewer");
  forme_barre(0, 9,"View .POV source",0,0,(NbObjet ? 1:0),"View the POVLAB's .POV source");
  forme_barre(0,10,"Edit manually .TEX",0,0,1,"Edit the texture lib database");
  forme_barre(0,11,"Edit manually .INC",0,0,1,"Edit the POV lib include file");
  forme_barre(0,12,RES_MENU[3],0,0,1,RES_AIDE[12]);
  forme_barre(0,13,RES_MENU[2],0,0,1,RES_AIDE[11]);
  Menu[0].Nb=14;

  strcpy(Menu[1].Texte,RES_MENU[4]); // -------------------------- Fichier
  Menu[1].Debut=Menu[0].Fin+10;
  Menu[1].Fin=Menu[1].Debut+largeur_text(Menu[1].Texte);
  forme_barre(1, 0,RES_MENU[ 6],0,0,1,RES_AIDE[13]); // nouveau
  forme_barre(1, 1,RES_MENU[ 7],0,0,1,RES_AIDE[14]); // Renomme
  forme_barre(1, 2,"#",0,0,1,"");
  forme_barre(1, 3,RES_MENU[ 8],0,0,1,RES_AIDE[ 15]);  // Charge
  forme_barre(1, 4,RES_MENU[ 9],0,0,(NbObjet ? 1:0),RES_AIDE[ 16]);  // Sauve
  forme_barre(1, 5,RES_MENU[36],0,0,Selection,RES_AIDE[127]);  // Sauve s‚lection
  forme_barre(1, 6,RES_MENU[37],0,0,(NbObjet ? 1:0),RES_AIDE[126]);  // Importe .Scn
  forme_barre(1, 7,RES_MENU[28],1,OptSauve,1,RES_AIDE[105]);  // Sauve en quittant
  forme_barre(1, 8,"Load POVLAB.SCN",1,OptLabScn,1,"Load POVLAB.SCN at startup");
  forme_barre(1, 9 ,"#",0,0,1,"");
  forme_barre(1,10,"Import Lathe",0,0,1,"Import a lathe from Lather");
  forme_barre(1,11,"Import Bezier patch",0,0,1,"Import a bezier patch");
  forme_barre(1,12,"#",0,0,1,"");
  forme_barre(1,13,"-> POV-Ray (.pov)",0,0,(NbObjet ? 1:0),RES_AIDE[17]); // Sauve .POV
  forme_barre(1,14,"Set output tab",0,0,1,"Set output format for POV-Ray");
  Menu[1].Nb=15;

  strcpy(Menu[2].Texte,RES_MENU[10]); // -------------------- Affichage
  Menu[2].Debut=Menu[1].Fin+10;
  Menu[2].Fin=Menu[2].Debut+largeur_text(Menu[2].Texte);
  forme_barre(2, 0,RES_MENU[11],0,0,(NbObjet ? 1:0),RES_AIDE[18]);
  forme_barre(2, 1,RES_MENU[12],0,0,(NbObjet ? 1:0),RES_AIDE[19]);
  forme_barre(2, 2,"Rescale viewports",0,0,Fx4,"Rescale viewports");
  forme_barre(2, 3,"#",0,0,1,"");
  forme_barre(2, 4,RES_MENU[13],1,OptAxe,1,RES_AIDE[20]);
  forme_barre(2, 5,RES_MENU[14],1,OptGrille,1,RES_AIDE[21]);
  forme_barre(2, 6,"Display dot grid",1,!GrilleType,OptGrille,"Display a grid with dots");
  forme_barre(2, 7,"Show coordinates",1,Vid[NF].Coord,1,"Show coordinates in viewport");
  forme_barre(2, 8,"Snap translation",1,Vid[NF].OptSnap,1,"Snap translation to value");
  forme_barre(2, 9,"Show control pts",1,ShowCtrlPt,1,"Show bezier/spline control points");
  forme_barre(2,10,"#",0,0,1,"");
  forme_barre(2,11,RES_MENU[25],0,0,(NbObjet ? 1:0),RES_AIDE[102]);
  forme_barre(2,12,RES_MENU[15],0,0,(NbObjet ? 1:0),RES_AIDE[22]);
  forme_barre(2,13,RES_MENU[26],0,0,(NbObjet ? 1:0),RES_AIDE[103]);
  forme_barre(2,14,RES_MENU[39],1,SuiviSelection,1,RES_AIDE[130]);
  forme_barre(2,15,"#",0,0,1,"");
  forme_barre(2,16,RES_MENU[61],1,!Omni[0].Cache,NbOmni,RES_AIDE[59]);
  forme_barre(2,17,RES_MENU[62],1,!Spot[0].Cache,NbSpot,RES_AIDE[60]);
  forme_barre(2,18,RES_MENU[60],1,!Area[0].Cache,NbArea,RES_AIDE[145]);
  forme_barre(2,19,"Display cyl lights",1,!CylLight[0].Cache,NbCylLight,RES_AIDE[145]);
  forme_barre(2,20,"#",0,0,1,"");
  forme_barre(2,21,"Mesh precision",0,0,(NbObjet ? 1:0),"Mesh precision for objects");
  forme_barre(2,22,"Image control",0,0,1,"Control the dithering type");
  forme_barre(2,23,"Drawing aids",0,0,Vid[NF].Enable,"Configure snap etc...");
  Menu[2].Nb=24;

  strcpy(Menu[3].Texte,RES_MENU[16]); // ---------------------- Interface
  Menu[3].Debut=Menu[2].Fin+10;
  Menu[3].Fin=Menu[3].Debut+largeur_text(Menu[3].Texte);
  forme_barre(3, 0,RES_MENU[17],0,0,1,RES_AIDE[23]);
  forme_barre(3, 1,RES_MENU[18],1,(NbCouleurs==256),1,RES_AIDE[24]);
  forme_barre(3, 2,RES_MENU[27],1,MotifOk,(NbCouleurs==256),RES_AIDE[104]);
  forme_barre(3, 3,"#",0,0,1,"");
  forme_barre(3, 4,RES_MENU[19],1,OptAide,1,RES_AIDE[25]);
  forme_barre(3, 5,RES_MENU[20],1,OptBeep,1,RES_AIDE[26]);
  forme_barre(3, 6,RES_MENU[31],1,OptFade,1,RES_AIDE[117]);
  forme_barre(3, 7,RES_MENU[24],1,OptBtD,1,RES_AIDE[100]);
  forme_barre(3, 8,"#",0,0,1,"");
  forme_barre(3, 9,RES_MENU[21],2,!strcmp(Resolution,"640"),1,RES_AIDE[27]);
  forme_barre(3,10,RES_MENU[22],2,!strcmp(Resolution,"800"),1,RES_AIDE[28]);
  forme_barre(3,11,RES_MENU[23],2,!strcmp(Resolution,"1024"),1,RES_AIDE[29]);
  Menu[3].Nb=12;

  strcpy(Menu[4].Texte,RES_MENU[30]); // ----------------------------- Outils
  Menu[4].Debut=Menu[3].Fin+10;
  Menu[4].Fin=Menu[4].Debut+largeur_text(Menu[4].Texte);
  forme_barre(4, 0,RES_MENU[38],0,0,(NbObjet ? 1:0),RES_AIDE[129]);
  forme_barre(4, 1,RES_MENU[63],0,0,(NbObjet ? 1:0),RES_AIDE[157]);
  forme_barre(4, 2,"#",0,0,1,"");
  forme_barre(4, 3,RES_MENU[29],0,0,1,RES_AIDE[116]);
  forme_barre(4, 4,"#",0,0,1,"");
  forme_barre(4, 5,RES_MENU[65],0,0,(NbObjet ? 1:0),RES_AIDE[162]);
  forme_barre(4, 6,"Make 3D fonts",0,0,1,"Create a 3D font");
  forme_barre(4, 7,"#",0,0,1,"");
  forme_barre(4, 8,"Load Plugins",0,0,1,"External objects generators");
  forme_barre(4, 9,"#",0,0,1,"");
  forme_barre(4,10,"Create B-Spline",0,0,1,"Prepare a B-Spline path");
  forme_barre(4,11,"Edit Spline",0,0,NbSpline>=0,"Edit parameters for spline");
  forme_barre(4,12,"Move vertex B-Sp",0,0,NbSpline>=0,"Move points on B-Spline path");
  forme_barre(4,13,"Delete vertex B-Sp",0,0,NbSpline>=0,"Erase points on B-Spline path");
  forme_barre(4,14,"Add vertex B-Sp",0,0,NbSpline>=0,"Add points on B-Spline path");
  forme_barre(4,15,"Edit vertex",0,0,NbSpline>=0,"Edit radius for sphere");
  forme_barre(4,16,"Smooth vertices",0,0,NbSpline>=0,"Smooth between 0 and end vertices");
  Menu[4].Nb=17;

  strcpy(Menu[5].Texte,RES_MENU[40]); // ---------------------- Objets
  Menu[5].Debut=Menu[4].Fin+10;
  Menu[5].Fin=Menu[5].Debut+largeur_text(Menu[5].Texte);
  forme_barre(5, 0,RES_MENU[41],2,Objet[NbObjet]->Type==CYLINDRE,1,RES_AIDE[37]);
  forme_barre(5, 1,RES_MENU[42],2,Objet[NbObjet]->Type==SPHERE  ,1,RES_AIDE[38]);
  forme_barre(5, 2,RES_MENU[43],2,Objet[NbObjet]->Type==CUBE    ,1,RES_AIDE[39]);
  forme_barre(5, 3,RES_MENU[44],2,Objet[NbObjet]->Type==CONE    ,1,RES_AIDE[40]);
  forme_barre(5, 4,RES_MENU[59],2,Objet[NbObjet]->Type==CONET   ,1,RES_AIDE[143]);
  forme_barre(5, 5,RES_MENU[45],2,Objet[NbObjet]->Type==TORE    ,1,RES_AIDE[41]);
  forme_barre(5, 6,RES_MENU[46],2,Objet[NbObjet]->Type==TUBE    ,1,RES_AIDE[42]);
  forme_barre(5, 7,RES_MENU[47],2,Objet[NbObjet]->Type==PLANX   ,1,RES_AIDE[43]);
  forme_barre(5, 8,RES_MENU[48],2,Objet[NbObjet]->Type==PLANY   ,1,RES_AIDE[44]);
  forme_barre(5, 9,RES_MENU[49],2,Objet[NbObjet]->Type==PLANZ   ,1,RES_AIDE[45]);
  forme_barre(5,10,RES_MENU[50],2,Objet[NbObjet]->Type==TRIANGLE,1,RES_AIDE[47]);
  forme_barre(5,11,RES_MENU[51],2,Objet[NbObjet]->Type==ANNEAU  ,1,RES_AIDE[106]);
  forme_barre(5,12,RES_MENU[52],2,Objet[NbObjet]->Type==DISQUE  ,1,RES_AIDE[107]);
  forme_barre(5,13,RES_MENU[53],2,Objet[NbObjet]->Type==DSPHERE ,1,RES_AIDE[134]);
  forme_barre(5,14,RES_MENU[54],2,Objet[NbObjet]->Type==QTORE   ,1,RES_AIDE[135]);
  forme_barre(5,15,RES_MENU[55],2,Objet[NbObjet]->Type==BLOB    ,1,RES_AIDE[136]);
  forme_barre(5,16,RES_MENU[66],2,Objet[NbObjet]->Type==BLOBC   ,1,RES_AIDE[163]);
  forme_barre(5,17,RES_MENU[56],2,Objet[NbObjet]->Type==PRISME  ,1,RES_AIDE[139]);
  forme_barre(5,18,RES_MENU[57],2,Objet[NbObjet]->Type==QTUBE   ,1,RES_AIDE[141]);
  forme_barre(5,19,RES_MENU[64],2,Objet[NbObjet]->Type==PYRAMIDE,1,RES_AIDE[159]);
  forme_barre(5,20,"Height-field",2,Objet[NbObjet]->Type==HFIELD,1,"HF object from TGA or GIF");
  forme_barre(5,21,"Superellipso‹de",2,Objet[NbObjet]->Type==SUPEREL,1,"Superellipsoide objet");
  forme_barre(5,22,"Automap square",2,Objet[NbObjet]->Type==AUTOMAP,1,"Automap square object");
  Menu[5].Nb=23;

  strcpy(Menu[6].Texte,"?    "); // ---------------------- Aide
  Menu[6].Debut=Menu[5].Fin+10;
  Menu[6].Fin=Menu[6].Debut+largeur_text(Menu[6].Texte);
  forme_barre(6,0,"Help on keyboard",0,0,1,"Show assignations hotkeys");
  Menu[6].Nb=1;

  return NbMenu;
}

// ----------------------------------------------------------------------------
// --------- TEST ZONE DE LA BARRE DE MENU VOLANTE ----------------------------
// ----------------------------------------------------------------------------
int execute_barre(void) {
  int i,M,N;

  if (!(i=barre_menu())) return 0;

  M=i/256;
  N=i & 255;

  log_out(0,"Go menu [%d:%d]:%s",M,N,Menu[M].Choix[N-1]);

  // --------------- G‚n‚ral

  if (M==0) {
    switch(N) {
      case  1: a_propos_de(); break;
      case  3: fenetre_preferences(); break;
      case  4: gestion_memoire(); break;
      case  5: MSMouse=!MSMouse; break;
      case  6: menu_souris(MODIF); break;
      case  7: statistique_scene(); break;
      case  9: appel_programme_externe(1); return -1;
      case 10: appel_programme_externe(2); return -1;
      case 11: appel_programme_externe(3); return -1;
      case 12: appel_programme_externe(4); return -1;
      case 13: appel_programme_externe(0); return -1;
      case 14:  return -2;
    }
  }

  // ------------------- Fichier

  if (M==1) {
    switch(N) {
      case  1: bouton_nouveau(1); break;
      case  2: renomme_scene("NEW NAME"); break;
      case  4: if (lecture_fichier(NULL,1,0,1)) lecture_fichier(NULL,0,0,0); break;
      case  5: sauve_fichier(0,0); break;
      case  6: sauve_fichier(0,1); break;
      case  7: lecture_fichier(NULL,1,1,0); break;
      case  8: OptSauve=!OptSauve; break;
      case  9: OptLabScn=!OptLabScn; break;
      case 11: import_lathe(); break;
      case 12: import_bezier(); break;
      case 14: genere_script_raytracer(3); break;
      case 15: set_indentation_output(); break;
    }
  }

  // --------------- Affichage

  if (M==2) {
    switch(N) {
      case  1: redessine_fenetre(NF,1); break;
      case  2: redessine_fenetre(5,1); break;
      case  3: rescale_fenetres(); break;
      case  5: OptAxe=!OptAxe; redessine_fenetre(5,1); break;
      case  6: OptGrille=!OptGrille; redessine_fenetre(5,1); break;
      case  7: GrilleType=!GrilleType; redessine_fenetre(5,1); break;
      case  8: Vid[NF].Coord=!Vid[NF].Coord; redessine_fenetre(5,1); break;
      case  9: Vid[NF].OptSnap=!Vid[NF].OptSnap; break;
      case 10: ShowCtrlPt=!ShowCtrlPt; redessine_fenetre(5,1); break;
      case 12: affiche_objet(0,0,NbObjet); redessine_fenetre(5,1); break;
      case 13: affiche_objet(1,0,NbObjet); redessine_fenetre(5,1); break;
      case 14: affiche_objet(2,0,NbObjet); redessine_fenetre(5,1); break;
      case 15: SuiviSelection=!SuiviSelection; break;
      case 17: cache_omni(!Omni[0].Cache,1,NbOmni); break;
      case 18: cache_spot(!Spot[0].Cache,1,NbSpot); break;
      case 19: cache_area(!Area[0].Cache,1,NbArea); break;
      case 20: cache_cyllight(!CylLight[0].Cache,1,NbCylLight); break;
      case 22: modifie_maillage(); break;
      case 23: modifie_dithering(); break;
      case 24: drawing_aids(NF); break;
    }
  }

  // --------------- Interface

  if (M==3) {
    switch(N) {
      case 1: if (couleur_interface()) return -1; else break;
      case 2:
        NbCouleurs=(NbCouleurs==16 ? 256:16);
        bouton_resolution(Resolution,1);
        redessine_fenetre(5,1);
        return -1;
      case  3: if (choix_motif()) return -1;
      case  5: OptAide=!OptAide; break;
      case  6: OptBeep=!OptBeep; break;
      case  7: OptFade=!OptFade; break;
      case 8:
           OptBtD=!OptBtD;
           bouton_resolution(Resolution,1);
           redessine_fenetre(5,1);
           return -1;
      case 10: bouton_resolution("640",1); redessine_fenetre(5,1); return -1;
      case 11: bouton_resolution("800",1); redessine_fenetre(5,1); return -1;
      case 12: bouton_resolution("1024",1); redessine_fenetre(5,1); return -1;
    }
  }

  // --------------- Outils

  if (M==4) {
    switch(N) {
      case  1: alignement_objet(); break;
      case  2: duplique_rotation(); break;
      case  4: extrusion(); break;
      case  6: alignement(); break;
      case  7: genere_police_3D(); break;
      case  9: choix_plugins(); break;
      case 11: new_objet(SPLINE,1); break;
      case 12: parametre_spline(); break;
      case 13: move_point_spline(); break;
      case 14: delete_point_spline(); break;
      case 15: add_point_spline(); break;
      case 16: edit_vertex_spline(); break;
      case 17: smooth_spline(); break;
    }
  }

  // --------------- Cr‚ation Objets

  if (M==5) {
    switch(N) {
      case  1: new_objet(CYLINDRE,1);       break;
      case  2: new_objet(SPHERE,1);         break;
      case  3: new_objet(CUBE,1);           break;
      case  4: new_objet(CONE,1);           break;
      case  5: new_objet(CONET,1);          break;
      case  6: new_objet(TORE,1);           break;
      case  7: new_objet(TUBE,1);           break;
      case  8: new_objet(PLANX,1);          break;
      case  9: new_objet(PLANY,1);          break;
      case 10: new_objet(PLANZ,1);          break;
      case 11: lecture_triangle_raw("?",1); break;
      case 12: new_objet(ANNEAU,1);         break;
      case 13: new_objet(DISQUE,1);         break;
      case 14: new_objet(DSPHERE,1);        break;
      case 15: new_objet(QTORE,1);          break;
      case 16: new_objet(BLOB,1);           break;
      case 17: new_objet(BLOBC,1);          break;
      case 18: new_objet(PRISME,1);         break;
      case 19: new_objet(QTUBE,1);          break;
      case 20: new_objet(PYRAMIDE,1);       break;
      case 21: lecture_hfield("?",1);       break;
      case 22: lecture_objet_externe(SUPEREL);    break;
      case 23: lecture_automap();                 break;
    }
  }

  // --------------- ? (Aide)

  if (M==6) {
    switch(N) {
      case  1: affiche_aide_clavier(); break;
    }
  }

  return 1;
}

