/* ---------------------------------------------------------------------------
*  MENU.C
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
#include "VERSIONC.H"
#include <STDLIB.H>
#include <STRING.H>
#include <I86.H>
#include <DOS.H>
#include <STDIO.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

int DSM=180; // ligne de D‚but Sous-Menu
extern DBL Ratio;

// -----------------------------------------------------------------------
// -- COCHE LES PASTILLES DES BOUTONS PRINCIPAUX MENU --------------------
// -----------------------------------------------------------------------
void coche_bouton_menu(byte N) {
  init_pastille(0,Bt[ 2].X+4,Bt[ 2].Y+4,"",N== 2 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[ 3].X+4,Bt[ 3].Y+4,"",N== 3 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[10].X+4,Bt[10].Y+4,"",N==10 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[ 9].X+4,Bt[ 9].Y+4,"",N== 9 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[11].X+4,Bt[11].Y+4,"",N==11 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[13].X+4,Bt[13].Y+4,"",N==13 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[15].X+4,Bt[15].Y+4,"",N==15 ? 1:0,""); affiche_pastille(0);
  init_pastille(0,Bt[18].X+4,Bt[18].Y+4,"",N==18 ? 1:0,""); affiche_pastille(0);
}

// -----------------------------------------------------------------------
// -- EFFACE BOUTONS SOUS-MENUS PRINCIPAL --------------------------------
// -----------------------------------------------------------------------
void efface_boutons (int Y) {
  g_rectangle(XMenuD+5,Y,XMenuD+5+66,YMenuD-25+2,FOND,-1);
}

// ----------------------------------------------------------------------- */
// --------- MODIFIE L'ECHELLE AVEC BOUTONS BAS INTERFACE ---------------- */
// ----------------------------------------------------------------------- */
void modif_echelle (byte N,DBL Valeur,byte Operation,byte Redraw) {
  if (Vid[N].Echelle==0) Vid[N].Echelle=EPSILON;
  if (Vid[N].Echelle>10000) Vid[N].Echelle=10000;
  if (Operation=='+') Vid[N].Echelle+=Valeur;
  if (Operation=='-') Vid[N].Echelle-=Valeur;
  if (Operation=='=') Vid[N].Echelle=Valeur;
  Vid[N].Echelle=fabs(Vid[N].Echelle);
  if (Redraw) redessine_fenetre(N,1);
}

// ----------------------------------------------------------------------- */
// --------- GERE CHOIX MENU BOUTONS PRINCIPAUX -------------------------- */
// ----------------------------------------------------------------------- */
void choix_principal (void) {
  int i=-1;

  while (1) {
    if (test_entree_clavier()==-2) i=-2;
    if (i==-2) if ((i=test_quitter())==-2) break;
    if (test_barre_menu(CURSEUR,0)) { i=execute_barre(); i=(i>=0 ? -1:i); }
    if (i==-1) i=test_bouton(0,21);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) { menu_souris(0); i=-1; }
    if (i== 1) i=bouton_rafraichit();
    if (i== 2) i=bouton_camera();
    if (i== 3) i=bouton_lumiere();
    if (i== 4) { plus_moins('-',1); i=-1; }
    if (i== 5) { plus_moins('+',1); i=-1; }
    if (i== 6) i=bouton_recentre();
    if (i== 7) i=bouton_zone(1);
    if (i== 8) { attributs_objet(); i=-1; }
    if (i== 9) i=bouton_selection();
    if (i==10) i=bouton_matiere();
    if (i==11) i=bouton_render();
    if (i==12) i=bouton_deplacement();
    if (i==13) i=bouton_modifie();
    if (i==14) i=bouton_fenetre(0);
    if (i==15) i=bouton_affiche();
    if (i==16) { sauve_fichier(0,0); i=-1; }
    if (i==17) i=lance_rendu_rapide(0);
    if (i==18) i=menu_csg();
    if (i==19) { panel_srt(SCALE); i=-1; }
    if (i==20) { panel_srt(ROTATE);  i=-1; }
    if (i==21) { panel_srt(TRANSLATE); i=-1; }
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE CAMERA ------------------------------------
// -----------------------------------------------------------------------
int bouton_camera(void) {
  int i;

  init_bouton(30,XMenuD+5,DSM+  0,66,15,"Create",DROITE,ATTEND,RES_AIDE[153]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"Delete",DROITE,ATTEND,RES_AIDE[154]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"Translate",DROITE,ATTEND,RES_AIDE[48]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Place",DROITE,ATTEND,"Auto place camera");
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+ 60,66,15,"Color",DROITE,ATTEND,RES_AIDE[51]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 75,66,15,"Hide",DROITE,ATTEND,RES_AIDE[52]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+ 90,66,15,"Show",DROITE,ATTEND,RES_AIDE[53]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+105,66,15,"Fov",DROITE,ATTEND,RES_AIDE[53]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+120,66,15,"Roll",DROITE,ATTEND,"Side roll the camera");
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+135,66,15,"Focal blur",DROITE,ATTEND,RES_AIDE[49]);
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+150,66,15,"Setup",DROITE,ATTEND,"Other parameters for camera");
  affiche_bouton(40);

  coche_bouton_menu(2);
  efface_boutons(DSM+15*11);
  cache_camera(0,1,NbCamera);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,40);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) beep_erreur();
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) new_camera();
    if (i==31) supprime_camera(0);
    if (i==32) deplace_camera();
    if (i==33) place_camera();
    if (i==34) couleur_camera(NumCam);
    if (i==35) cache_camera(1,1,NbCamera);
    if (i==36) cache_camera(0,1,NbCamera);
    if (i==37) modif_ouverture(NumCam);
    if (i==38) roll_camera(NumCam);
    if (i==39) profondeur_champ(NumCam);
    if (i==40) setup_camera();
    /*
    { int Key=0;
      if (Key=='+') { Focale+=5.001; redessine_fenetre(3,1); }
      if (Key=='-') { Focale-=5.001; redessine_fenetre(3,1); }
      if (Key=='9') { Ratio+=.001; redessine_fenetre(3,1); }
      if (Key=='3') { Ratio-=.001; redessine_fenetre(3,1); }
      if (Key=='7') { Ratio+=1.001; redessine_fenetre(3,1); }
      if (Key=='1') { Ratio-=1.001; redessine_fenetre(3,1); }
      if (Key=='8') { Ratio+=10.001; redessine_fenetre(3,1); }
      if (Key=='2') { Ratio-=10.001; redessine_fenetre(3,1); }
      message("Ratio=%.6f",Ratio);
      // if (Key=='4') { for (i=1;i<=NbObjet;i++) Objet[i].RY+=10; redessine_fenetre(3,1); }
      // if (Key=='6') { for (i=1;i<=NbObjet;i++) Objet[i].RY-=10; redessine_fenetre(3,1); }
      // if (Key=='8') { for (i=1;i<=NbObjet;i++) Objet[i].RX+=10; redessine_fenetre(3,1); }
      // if (Key=='2') { for (i=1;i<=NbObjet;i++) Objet[i].RX-=10; redessine_fenetre(3,1); }
    } */
  }
}

// ----------------------------------------------------------------------- */
// --------- DESSINE INTERFACE SELECTION --------------------------------- */
// ----------------------------------------------------------------------- */
int bouton_selection (void) {
  int i;

  init_bouton(30,XMenuD+5,DSM+  0,66,15,"Object",DROITE,ATTEND,RES_AIDE[61]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"All",DROITE,ATTEND,RES_AIDE[62]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"None",DROITE,ATTEND,RES_AIDE[63]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Invert",DROITE,ATTEND,RES_AIDE[64]);
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+ 60,66,15,"Zone",DROITE,ATTEND,RES_AIDE[65]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 75,66,15,"Last",DROITE,ATTEND,RES_AIDE[66]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+ 90,66,15,"Color",DROITE,ATTEND,RES_AIDE[67]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+105,66,15,"By name",DROITE,ATTEND,RES_AIDE[68]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+120,66,15,"By texture",DROITE,ATTEND,RES_AIDE[125]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+135,66,15,"Parameters",DROITE,ATTEND,"Multii change object parameters");
  affiche_bouton(39);

  coche_bouton_menu(9);
  efface_boutons(DSM+15*10);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,39);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) beep_erreur();
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) selection_objet(2);
    if (i==31) selection_objet(1);
    if (i==32) selection_objet(0);
    if (i==33) selection_objet(3);
    if (i==34) selection_zone();
    if (i==35) selection_objet(4);
    if (i==36) couleur_selection();
    if (i==37) selection_objet(5);
    if (i==38) selection_objet_par_matiere(0);
    if (i==39) change_attributs_selection();
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE RENDERER ----------------------------------
// -----------------------------------------------------------------------
int bouton_render (void) {
  int i;

  LABEL_INIT_RENDER:

  init_bouton(30,XMenuD+5,DSM+  0,66,15,"Preview",DROITE,ATTEND,RES_AIDE[69]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"Quick",DROITE,ATTEND,RES_AIDE[70]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"Optimum",DROITE,ATTEND,RES_AIDE[71]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Final",DROITE,ATTEND,RES_AIDE[72]);
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+ 60,66,15,"Region",DROITE,ATTEND,RES_AIDE[121]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 75,66,15,"Fog",DROITE,ATTEND,RES_AIDE[123]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+ 90,66,15,"Atmosphere",DROITE,ATTEND,"Manage atmosphere parameters");
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+105,66,15,"Background",DROITE,ATTEND,RES_AIDE[108]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+120,66,15,"Palette",DROITE,ATTEND,RES_AIDE[158]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+135,66,15,"Video",DROITE,ATTEND,"POV-Ray's inline options");
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+150,66,15,"View last",DROITE,ATTEND,"View last rendered TGA image");
  affiche_bouton(40);
  init_bouton(41,XMenuD+5,DSM+165,66,15,"Setup",DROITE,ATTEND,"Setup for rendering options");
  affiche_bouton(41);

  coche_bouton_menu(11);
  efface_boutons(DSM+15*12);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,41);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) beep_erreur();
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) if (lance_calcul((DernierRendu=0),1)<0) return -1;
    if (i==31) if (lance_calcul((DernierRendu=1),1)<0) return -1;
    if (i==32) if (lance_calcul((DernierRendu=2),1)<0) return -1;
    if (i==33) if (lance_calcul((DernierRendu=3),1)<0) return -1;
    if (i==34) if (lance_calcul((DernierRendu=4),0)<0) return -1;
    if (i==35) { parametre_brouillard(); goto LABEL_INIT_RENDER; }
    if (i==36) { parametre_atmosphere(); goto LABEL_INIT_RENDER; }
    if (i==37) couleur_fond_image();
    if (i==38) palette_rendu_rapide();
    if (i==39) options_video();
    if (i==40) voir_image(0);
    if (i==41) setup_render();
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE BOUTONS TEXTURE ---------------------------
// -----------------------------------------------------------------------
int bouton_matiere (void) {
  int i;
  init_bouton(30,XMenuD+5,DSM    ,66,15,"Texture",DROITE,ATTEND,RES_AIDE[73]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"Acquire",DROITE,ATTEND,RES_AIDE[74]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"Assign",DROITE,ATTEND,RES_AIDE[75]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Rotate",DROITE,ATTEND,RES_AIDE[112]);
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+ 60,66,15,"Scale 2D",DROITE,ATTEND,RES_AIDE[113]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 75,66,15,"Scale 3D",DROITE,ATTEND,RES_AIDE[114]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+ 90,66,15,"Translate",DROITE,ATTEND,RES_AIDE[115]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+105,66,15,"Reinit.",DROITE,ATTEND,RES_AIDE[128]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+120,66,15,"Finish",DROITE,ATTEND,"Texture finish properties");
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+135,66,15,"Setup",DROITE,ATTEND,"Setup for general textures");
  affiche_bouton(39);

  coche_bouton_menu(10);
  efface_boutons(DSM+15*10);

  init_bouton(40,XMenuD+5,DSM+160,66,15,"Mapping",DROITE,ATTEND,"Add a map file to and object");
  affiche_bouton(40);
  init_bouton(41,XMenuD+5,DSM+175,66,15,"Library",DROITE,ATTEND,"Mapping paths");
  affiche_bouton(41);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,41);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) beep_erreur();
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) lecture_matiere();
    if (i==31) give_me_your_matiere();
    if (i==32) assigne_matiere();
    if (i==33) modif_matiere(ROTATE);
    if (i==34) modif_matiere(SCALE);
    if (i==35) modif_matiere(DIVERS);
    if (i==36) modif_matiere(TRANSLATE);
    if (i==37) modif_matiere(4);
    if (i==38) modif_parametre_texture(0);
    if (i==39) setup_texture();
    if (i==40) set_mapping(0);
    if (i==41) library_mapping();
  }
}

// ----------------------------------------------------------------------- */
// --------- DESSINE INTERFACE BOUTON MODIFICATION OBJET ----------------- */
// ----------------------------------------------------------------------- */
int bouton_modifie (void) {
  int i;

  init_bouton(30,XMenuD+5,DSM+  0,66,15,"Rotate",DROITE,ATTEND,RES_AIDE[76]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"Scale 2D",DROITE,ATTEND,RES_AIDE[77]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"Scale 3D",DROITE,ATTEND,RES_AIDE[78]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Translate",DROITE,ATTEND,RES_AIDE[79]);
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+ 60,66,15,"Copy",DROITE,ATTEND,RES_AIDE[80]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 75,66,15,"Delete",DROITE,ATTEND,RES_AIDE[81]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+ 90,66,15,"Center XYZ",DROITE,ATTEND,RES_AIDE[83]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+105,66,15,"Reinit.",DROITE,ATTEND,RES_AIDE[84]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+120,66,15,"Manual",DROITE,ATTEND,RES_AIDE[85]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+135,66,15,"Color",DROITE,ATTEND,RES_AIDE[46]);
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+150,66,15,"Smooth",DROITE,ATTEND,RES_AIDE[122]);
  affiche_bouton(40);
  init_bouton(41,XMenuD+5,DSM+165,66,15,"Mirror",DROITE,ATTEND,RES_AIDE[144]);
  affiche_bouton(41);

  coche_bouton_menu(13);
  efface_boutons(DSM+15*12);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,21);
    if (i<0) i=test_bouton(30,41);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) beep_erreur();
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) rotation(0,0);
    if (i==31) deformation2D(0,0);
    if (i==32) deformation3D(0,0);
    if (i==33) translation(0,0);
    if (i==34) copie_objet();
    if (i==35) supprime_objet();
    if (i==36) recentre_objet();
    if (i==37) reinitialisation_objet();
    if (i==38) attributs_objet();
    if (i==39) couleur_objet(0);
    if (i==40) lisse_objet();
    if (i==41) miroir_objet(0,0);
  }
}

// ----------------------------------------------------------------------- */
// --------- DESSINE INTERFACE AFFICHE/CACHE ----------------------------- */
// ----------------------------------------------------------------------- */
int bouton_affiche (void) {
  int i;

  init_bouton(30,XMenuD+5,DSM+ 0,66,15,"Hide",DROITE,ATTEND,RES_AIDE[87]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+15,66,15,"None",DROITE,ATTEND,RES_AIDE[88]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+30,66,15,"All",DROITE,ATTEND,RES_AIDE[89]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+45,66,15,"Invert",DROITE,ATTEND,RES_AIDE[90]);
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+60,66,15,"Last",DROITE,ATTEND,RES_AIDE[91]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+75,66,15,"Selection",DROITE,ATTEND,RES_AIDE[92]);
  affiche_bouton(35);

  coche_bouton_menu(15);
  efface_boutons(DSM+15*6);

  init_bouton(36,XMenuD+5,DSM+105,66,15,"Normal",DROITE,ATTEND,RES_AIDE[102]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+120,66,15,"Fast",DROITE,ATTEND,RES_AIDE[22]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+135,66,15,"Cubic",DROITE,ATTEND,RES_AIDE[103]);
  affiche_bouton(38);

  init_bouton(39,XMenuD+5,DSM+165,66,15,"Freeze",DROITE,ATTEND,RES_AIDE[131]);
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+180,66,15,"Ignore",DROITE,ATTEND,RES_AIDE[132]);
  affiche_bouton(40);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,40);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) beep_erreur();
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) cache_objet(2);
    if (i==31) cache_objet(1);
    if (i==32) cache_objet(0);
    if (i==33) cache_objet(3);
    if (i==34) cache_objet(4);
    if (i==35) cache_objet(5);
    if (i==36) affiche_objet(0,0,0);
    if (i==37) affiche_objet(1,0,0);
    if (i==38) affiche_objet(2,0,0);
    if (i==39) cache_objet(6);
    if (i==40) cache_objet(7);
  }
}

// -----------------------------------------------------------------------
// --------- MENU POUR LES COMBINAISONS DE CSG ---------------------------
// -----------------------------------------------------------------------
int menu_csg(void) {
  int i;
  
  init_bouton(30,XMenuD+5,DSM+  0,66,15,"Difference",DROITE,ATTEND,RES_AIDE[146]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"Union",DROITE,ATTEND,RES_AIDE[147]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"Intersection",DROITE,ATTEND,RES_AIDE[148]);
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Merge",DROITE,ATTEND,RES_AIDE[160]);
  affiche_bouton(33);
  init_bouton(34,XMenuD+5,DSM+ 60,66,15,"Inverse",DROITE,ATTEND,"CSG inverse operation");
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 75,66,15,"Remove CSG",DROITE,ATTEND,RES_AIDE[150]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+ 90,66,15,"Show",DROITE,ATTEND,RES_AIDE[151]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+105,66,15,"Hide",DROITE,ATTEND,RES_AIDE[152]);
  affiche_bouton(37);

  coche_bouton_menu(18);
  efface_boutons(DSM+15*8);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,21);
    if (i<0) i=test_bouton(30,37);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) beep_erreur();
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) csg_objet(DIFFERE);
    if (i==31) csg_objet(UNION);
    if (i==32) csg_objet(INTERSE);
    if (i==33) csg_objet(MERGE);
    if (i==34) inverse_csg();
    if (i==35) csg_objet(PAS_CSG);
    if (i==36) affiche_csg(1);
    if (i==37) affiche_csg(0);
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE LUMIERE -----------------------------------
// -----------------------------------------------------------------------
int bouton_lumiere(void) {
  int i;

  init_bouton(30,XMenuD+5,DSM+  0,66,15,"Omni...",DROITE,ATTEND,RES_AIDE[54]);
  affiche_bouton(30);
  init_bouton(31,XMenuD+5,DSM+ 15,66,15,"Spot...",DROITE,ATTEND,RES_AIDE[109]);
  affiche_bouton(31);
  init_bouton(32,XMenuD+5,DSM+ 30,66,15,"Area...",DROITE,ATTEND,"Edit aeralights");
  affiche_bouton(32);
  init_bouton(33,XMenuD+5,DSM+ 45,66,15,"Cylindrical...",DROITE,ATTEND,"Edit cylindrical lights");
  affiche_bouton(33);

  efface_boutons(DSM+15*4);
  coche_bouton_menu(3);

  cache_area(0,1,NbArea);
  cache_spot(0,1,NbSpot);
  cache_omni(0,1,NbOmni);
  cache_cyllight(0,1,NbCylLight);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,33);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    LABEL_DEBUT_LUMIERE:
    if (i<-1) return i;
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) beep_erreur();
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) {
      i=menu_omni();
      efface_boutons(DSM+15*4);
      goto LABEL_DEBUT_LUMIERE;
    }
    if (i==31) {
      i=menu_spot();
      efface_boutons(DSM+15*4);
      goto LABEL_DEBUT_LUMIERE;
    }
    if (i==32) {
      i=menu_area();
      efface_boutons(DSM+15*4);
      goto LABEL_DEBUT_LUMIERE;
    }
    if (i==33) {
      i=menu_cyllight();
      efface_boutons(DSM+15*4);
      goto LABEL_DEBUT_LUMIERE;
    }
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE LUMIERE OMNI ------------------------------
// -----------------------------------------------------------------------
int menu_omni(void) {
  int i;

  init_bouton(34,XMenuD+5,DSM+ 70,66,15,"Create",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 85,66,15,"Delete",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+100,66,15,"Translate",DROITE,ATTEND,RES_AIDE[56]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+115,66,15,"Color",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+130,66,15,"Size",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+145,66,15,"Looks_Like",DROITE,ATTEND,"Attach a light to an object");
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+160,66,15,"Setup",DROITE,ATTEND,"Setup for omnilights");
  affiche_bouton(40);

  efface_boutons(DSM+15*(7+5));

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,40);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) beep_erreur();
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) beep_erreur();
    if (i==31) return 31;
    if (i==32) return 32;
    if (i==33) return 32;
    if (i==34) new_omni();
    if (i==35) travail_lumiere(3);
    if (i==36) travail_lumiere(0);
    if (i==37) travail_lumiere(2);
    if (i==38) travail_lumiere(1);
    if (i==39) looks_like_omni();
    if (i==40) setup_lumiere();
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE LUMIERE SPOT ------------------------------
// -----------------------------------------------------------------------
int menu_spot(void) {
  int i;

  init_bouton(34,XMenuD+5,DSM+ 70 ,66,15,"Create",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 85,66,15,"Delete",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+100,66,15,"Translate",DROITE,ATTEND,RES_AIDE[56]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+115,66,15,"Color",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+130,66,15,"Size",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+145,66,15,"Fall off",DROITE,ATTEND,RES_AIDE[110]);
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+160,66,15,"Hot spot",DROITE,ATTEND,RES_AIDE[111]);
  affiche_bouton(40);
  init_bouton(41,XMenuD+5,DSM+175,66,15,"Setup",DROITE,ATTEND,"Setup for spotlights");
  affiche_bouton(41);

  efface_boutons(DSM+15*(8+5)-4);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,41);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) beep_erreur();
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) return 30;
    if (i==31) beep_erreur();
    if (i==32) return 32;
    if (i==33) return 33;
    if (i==34) new_spot();
    if (i==35) travail_lumiere(3);
    if (i==36) travail_lumiere(0);
    if (i==37) travail_lumiere(2);
    if (i==38) travail_lumiere(1);
    if (i==39) travail_lumiere(4);
    if (i==40) travail_lumiere(5);
    if (i==41) setup_lumiere();
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE LUMIERE AREA ------------------------------
// -----------------------------------------------------------------------
int menu_area(void) {
  int i;

  init_bouton(34,XMenuD+5,DSM+ 70,66,15,"Create",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 85,66,15,"Delete",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+100,66,15,"Translate",DROITE,ATTEND,RES_AIDE[56]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+115,66,15,"Color",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+130,66,15,"Size",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+145,66,15,"Looks_Like",DROITE,ATTEND,"Attach a light to an object");
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+160,66,15,"Setup",DROITE,ATTEND,"Setup for arealights");
  affiche_bouton(40);

  efface_boutons(DSM+15*(7+5)-4);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,40);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) return 3;
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) return 30;
    if (i==31) return 31;
    if (i==32) beep_erreur();
    if (i==33) return 33;
    if (i==34) new_area();
    if (i==35) travail_lumiere(3);
    if (i==36) travail_lumiere(0);
    if (i==37) travail_lumiere(2);
    if (i==38) travail_lumiere(1);
    if (i==39) looks_like_area();
    if (i==40) setup_lumiere();
  }
}

// -----------------------------------------------------------------------
// --------- DESSINE INTERFACE LUMIERE CYLINDRICAL LIGHT -----------------
// -----------------------------------------------------------------------
int menu_cyllight(void) {
  int i;

  init_bouton(34,XMenuD+5,DSM+ 70,66,15,"Create",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(34);
  init_bouton(35,XMenuD+5,DSM+ 85,66,15,"Delete",DROITE,ATTEND,RES_AIDE[55]);
  affiche_bouton(35);
  init_bouton(36,XMenuD+5,DSM+100,66,15,"Translate",DROITE,ATTEND,RES_AIDE[56]);
  affiche_bouton(36);
  init_bouton(37,XMenuD+5,DSM+115,66,15,"Color",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(37);
  init_bouton(38,XMenuD+5,DSM+130,66,15,"Size",DROITE,ATTEND,RES_AIDE[57]);
  affiche_bouton(38);
  init_bouton(39,XMenuD+5,DSM+145,66,15,"Fall off",DROITE,ATTEND,RES_AIDE[110]);
  affiche_bouton(39);
  init_bouton(40,XMenuD+5,DSM+160,66,15,"Hot spot",DROITE,ATTEND,RES_AIDE[111]);
  affiche_bouton(40);
  init_bouton(41,XMenuD+5,DSM+175,66,15,"Setup",DROITE,ATTEND,"Setup for cylindrical lights");
  affiche_bouton(41);

  efface_boutons(DSM+15*(8+5)-4);

  GMouseOn();
  while (1) {
    if ((i=test_entree_clavier())<0) return i;
    if (test_barre_menu(CURSEUR,0)) if ((i=execute_barre())<0) return i;
    i=test_bouton(0,41);
    if (test_case(0,0)==0) utilise_selection(0);
    if (MouseB()==2) menu_souris(0);
    if (i== 0) return 0;
    if (i== 1) bouton_rafraichit();
    if (i== 2) return 2;
    if (i== 3) beep_erreur();
    if (i== 4) plus_moins('-',1);
    if (i== 5) plus_moins('+',1);
    if (i== 6) bouton_recentre();
    if (i== 7) bouton_zone(1);
    if (i== 8) attributs_objet();
    if (i== 9) return 9;
    if (i==10) return 10;
    if (i==11) return 11;
    if (i==12) bouton_deplacement();
    if (i==13) return 13;
    if (i==14) bouton_fenetre(0);
    if (i==15) return 15;
    if (i==16) sauve_fichier(0,0);
    if (i==17) lance_rendu_rapide(0);
    if (i==18) return 18;
    if (i==19) panel_srt(SCALE);
    if (i==20) panel_srt(ROTATE);
    if (i==21) panel_srt(TRANSLATE);
    if (i==30) return 30;
    if (i==31) return 31;
    if (i==32) return 32;
    if (i==33) beep_erreur();
    if (i==34) new_cyllight();
    if (i==35) travail_lumiere(3);
    if (i==36) travail_lumiere(0);
    if (i==37) travail_lumiere(2);
    if (i==38) travail_lumiere(1);
    if (i==39) travail_lumiere(4);
    if (i==40) travail_lumiere(5);
    if (i==41) setup_lumiere();
  }
}


