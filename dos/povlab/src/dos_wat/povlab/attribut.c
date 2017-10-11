/* ---------------------------------------------------------------------------
*  ATTRIBUT.C
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
#include <STDLIB.H>
#include <STDIO.H>
#include <STRING.H>
#include <DOS.H>
#include <TIME.H>
#include <GRAPH.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

// --------------------------------------------------------------------------
// -- OBJET SANS PARAMETRES SPECIFIQUES -------------------------------------
// --------------------------------------------------------------------------
void pas_parametres_specifiques(void) {
  f_erreur("This type of object has no specific parameters.");
}

// --------------------------------------------------------------------------
// ----- ATTRIBUTS GENERAUX D'UN OBJET --------------------------------------
// --------------------------------------------------------------------------
void attributs_objet(void) {
  int i,N,Y;
  int X1=CentX-240;
  int X2=CentX+240;
  int Y1=CentY-180;
  int Y2=CentY+180;
  char StrTX[13],StrTY[13],StrTZ[13];
  char StrSX[13],StrSY[13],StrSZ[13];
  char StrRX[13],StrRY[13],StrRZ[13];
  char StrMTX[13],StrMTY[13],StrMTZ[13];
  char StrMSX[13],StrMSY[13],StrMSZ[13];
  char StrMRX[13],StrMRY[13],StrMRZ[13];
  DBL TX,TY,TZ;
  char Chaine[150];
  char Type[150];
  byte Actions;

  if (pas_objet(1)) return;

  LABEL_ATTRIBUT_OBJET:
  Actions=0;

  forme_mouse(MS_SELECTEUR);
  message("Select an object to modify");

  if ((N=trouve_volume(0,3,1))==FAUX) return;

  memcpy(Objet[0],Objet[N],sizeof(VOLUME));

  message("Enter new coordinates for the object");
  g_fenetre(X1,Y1,X2,Y2,"MODIFY OBJECT PARAMETERS",AFFICHE);

  LABEL_INIT_ACTIONS:

  sprintf(StrSX,"%.6f", Objet[N]->S[0]);
  sprintf(StrSY,"%.6f", Objet[N]->S[1]);
  sprintf(StrSZ,"%.6f", Objet[N]->S[2]);
  sprintf(StrRX,"%.6f", Objet[N]->R[0]);
  sprintf(StrRY,"%.6f", Objet[N]->R[1]);
  sprintf(StrRZ,"%.6f", Objet[N]->R[2]);
  sprintf(StrTX,"%.6f", Objet[N]->T[0]);
  sprintf(StrTY,"%.6f",-Objet[N]->T[1]);
  sprintf(StrTZ,"%.6f",-Objet[N]->T[2]);

  sprintf(StrMSX,"%.6f", Objet[N]->MS[0]);
  sprintf(StrMSY,"%.6f", Objet[N]->MS[1]);
  sprintf(StrMSZ,"%.6f", Objet[N]->MS[2]);
  sprintf(StrMRX,"%.6f", Objet[N]->MR[0]);
  sprintf(StrMRY,"%.6f", Objet[N]->MR[1]);
  sprintf(StrMRZ,"%.6f", Objet[N]->MR[2]);
  sprintf(StrMTX,"%.6f", Objet[N]->MT[0]);
  sprintf(StrMTY,"%.6f",-Objet[N]->MT[1]);
  sprintf(StrMTZ,"%.6f",-Objet[N]->MT[2]);

  Y=110;
  init_texte(0,X1+65,Y+Y1+ 30,"Scale X", StrSX,10,"Modify X scale");
  init_texte(1,X1+65,Y+Y1+ 50,"Scale Y", StrSY,10,"Modify Y scale");
  init_texte(2,X1+65,Y+Y1+ 70,"Scale Z", StrSZ,10,"Modify Z scale");
  init_texte(3,X1+65,Y+Y1+ 90,"Rotate X", StrRX,10,"Modify X rotation");
  init_texte(4,X1+65,Y+Y1+110,"Rotate Y", StrRY,10,"Modify Y rotation");
  init_texte(5,X1+65,Y+Y1+130,"Rotate Z", StrRZ,10,"Modify Z rotation");
  init_texte(6,X1+65,Y+Y1+150,"Translate X",StrTX,10,"Modify X position");
  init_texte(7,X1+65,Y+Y1+170,"Translate Y",StrTY,10,"Modify Y position");
  init_texte(8,X1+65,Y+Y1+190,"Translate Z",StrTZ,10,"Modify Z position");

  init_texte( 9,X1+235,Y+Y1+ 30,"Scale MX", StrMSX,10,"Texture X scale");
  init_texte(10,X1+235,Y+Y1+ 50,"Scale MY", StrMSY,10,"Texture Y scale");
  init_texte(11,X1+235,Y+Y1+ 70,"Scale MZ", StrMSZ,10,"Texture Z scale");
  init_texte(12,X1+235,Y+Y1+ 90,"Rotate MX", StrMRX,10,"Texture X rotation");
  init_texte(13,X1+235,Y+Y1+110,"Rotate MY", StrMRY,10,"Texture Y rotation");
  init_texte(14,X1+235,Y+Y1+130,"Rotate MZ", StrMRZ,10,"Texture Z rotation");
  init_texte(15,X1+235,Y+Y1+150,"Translate MX",StrMTX,10,"Texture X position");
  init_texte(16,X1+235,Y+Y1+170,"Translate MY",StrMTY,10,"Texture Y position");
  init_texte(17,X1+235,Y+Y1+190,"Translate MZ",StrMTZ,10,"Texture Z position");

  init_bouton(50,X2-125,Y2-30,55,20,RES_BOUT[11],CENTRE,ATTEND,RES_AIDE[30]);
  init_bouton(51,X2-65,Y2-30,55,20,RES_BOUT[10],CENTRE,ATTEND,RES_AIDE[32]);
  init_bouton(52,X1+10,Y1+ 53,85,20,"Modify name",GAUCHE,ATTEND,RES_AIDE[137]);
  init_bouton(53,X1+10,Y1+ 78,85,20,"Change color",GAUCHE,ATTEND,RES_AIDE[46]);
  init_bouton(54,X1+10,Y1+103,85,20,"Parameters",GAUCHE,ATTEND,RES_AIDE[138]);
  init_bouton(55,X1+100,Y1+ 53,85,20,"Material param.",GAUCHE,ATTEND,"Change minor texture parameters");

  init_case(18,X2-100,Y+Y1-10,"Looks_like",Objet[N]->LooksLike.Nb,"Object looks_like light or not");

  init_case(17,X2-100,Y+Y1+10,"Hollow",Objet[N]->Halo,"Set hollow to object");
  init_case(11,X2-100,Y+Y1+30,"Selected",Objet[N]->Selection,"Selected object or not");
  init_case(12,X2-100,Y+Y1+50,"Ignored object",Objet[N]->Ignore,"Ignored object during re-centering");
  init_case(13,X2-100,Y+Y1+70,"Freezed object",Objet[N]->Freeze,"Visible object but can't work with");
  init_case(14,X2-100,Y+Y1+90,"Hidden object",Objet[N]->Cache,"Unused object or not visible");

  init_pastille(11,X2-100,Y+Y1+110,"Normal display",Objet[N]->Rapide==0,"Display all faces");
  init_pastille(12,X2-100,Y+Y1+130,"Quick display",Objet[N]->Rapide==1,"Display some faces");
  init_pastille(13,X2-100,Y+Y1+150,"Cubic display",Objet[N]->Rapide==2,"Display bounding boxes");

  init_case(15,X2-100,Y+Y1+170,"Cast shadows",Objet[N]->Ombre ? 1:0,"Cast or not a shadow");
  init_case(16,X2-100,Y+Y1+190,"Scale texture",Objet[N]->ScaleOM ? 1:0,"Scale texture with object");

  if (Actions) goto LABEL_ACTIONS_AFFICHEES;
  
  switch (Objet[N]->Type) {
    case CYLINDRE: strcpy(Type,"CYLINDER"); break;
    case SPHERE  : strcpy(Type,"SPHERE"); break;
    case CUBE    : strcpy(Type,"CUBE"); break;
    case CONE    : strcpy(Type,"CONE"); break;
    case CONET   : strcpy(Type,"CONE TRONQUE"); break;
    case TORE    : strcpy(Type,"TORUS"); break;
    case TUBE    : strcpy(Type,"TUBE"); break;
    case PLANX   : strcpy(Type,"PLANE-X"); break;
    case PLANY   : strcpy(Type,"PLANE-Y"); break;
    case PLANZ   : strcpy(Type,"PLANE-Z"); break;
    case TRIANGLE:
      sprintf(Type,"TRIANGLE Faces: %d %s",
              Poly[Objet[N]->Poly]->Nombre,
              Objet[N]->Smooth ? "smooth":"flat");
      break;
    case ANNEAU  : strcpy(Type,"RING"); break;
    case DISQUE  : strcpy(Type,"DISK"); break;
    case DSPHERE : strcpy(Type,"1/2 SPHERE"); break;
    case QTORE   : strcpy(Type,"1/4 TORUS"); break;
    case QTUBE   : strcpy(Type,"1/4 RING"); break;
    case BLOB    :
    case BLOBC   :
      i=(int) Objet[N]->P[2];
      sprintf(Type,"BLOB Group: %d threshold: %.2f Strength: %.2f",
              i,Objet[N]->P[1],Objet[N]->P[0]);
      break;
    case PRISME  : strcpy(Type,"PRISM"); break;
    case PYRAMIDE: strcpy(Type,"PYRAMID"); break;
    case HFIELD  : strcpy(Type,"HEIGHT-FIELD"); break;
  }

  sprintf(Chaine,"%s [%04d] Name: \"%s\" Tex.: \"%s\"",
                  Type,N,Objet[N]->Nom,Objet[N]->Matiere);
  parse_ascii_watcom(Chaine);
  windows(X1+11,Y1+29,X2-10,Y1+29+hauteur_text("G")+2,0,ZFOND);
  text_xy(X1+12,Y1+29,Chaine,JAUNE);

  place_zone_texte(0);
  place_zone_texte(1);
  place_zone_texte(2);
  place_zone_texte(3);
  place_zone_texte(4);
  place_zone_texte(5);
  place_zone_texte(6);
  place_zone_texte(7);
  place_zone_texte(8);
  place_zone_texte(9);
  place_zone_texte(10);
  place_zone_texte(11);
  place_zone_texte(12);
  place_zone_texte(13);
  place_zone_texte(14);
  place_zone_texte(15);
  place_zone_texte(16);
  place_zone_texte(17);
  affiche_bouton(50);
  affiche_bouton(51);
  affiche_bouton(52);
  affiche_bouton(53);
  affiche_bouton(54);
  affiche_bouton(55);
  affiche_case(11);
  affiche_case(12);
  affiche_case(13);
  affiche_case(14);
  affiche_pastille(11);
  affiche_pastille(12);
  affiche_pastille(13);
  affiche_case(15);
  affiche_case(16);
  affiche_case(17);
  affiche_case(18);

  LABEL_ACTIONS_AFFICHEES:
  Actions=1;

  vignette_couleur(X1+10,Y2-30,X1+100,Y2-10,Objet[N]->Couleur,BLANC);
  i=-1;

  while (1) {
    test_texte(0,17);
    if (kbhit()) {
      i=getch();
      if (i==13) { i=0; break; }
      if (i==27) { i=1; break; }
    }
    if ((i=test_bouton(52,55))!=-1) {
      switch (i) {
        case 52: renomme_objet(0,N); goto LABEL_INIT_ACTIONS; break;
        case 53: couleur_objet(N); goto LABEL_INIT_ACTIONS; break;
        case 54:
          switch(Objet[N]->Type) {
            case BLOB:
            case BLOBC:
              force_seuil_groupe_blob(0,N);
              goto LABEL_INIT_ACTIONS;
              break;
            case HFIELD:
              water_level(N);
              goto LABEL_INIT_ACTIONS;
              break;
            case CONET:
            case TUBE:
            case QTUBE:
              edit_cone(N);
              goto LABEL_INIT_ACTIONS;
              break;
            case TORE:
            case QTORE:
              edit_tore(N);
              goto LABEL_INIT_ACTIONS;
              break;
            default:
              pas_parametres_specifiques();
              goto LABEL_INIT_ACTIONS;
              break;
          }
        case 55:
          modif_parametre_texture(N);
          goto LABEL_INIT_ACTIONS;
          break;
      }
    }
    if ((i=test_bouton(50,51))!=-1) { i-=50; break; }
    if (sequence_sortie()) { i=1; break; }
    test_groupe_pastille(11,13);
    test_case(11,18);
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  TX=atof(ZTexte[0].Variable);
  TY=atof(ZTexte[1].Variable);
  TZ=atof(ZTexte[2].Variable);

  if ((Objet[N]->Type==BLOB || Objet[N]->Type==BLOB) && i==0 && TX!=TY && TX!=TZ) {
    erreur_2D_blob();
    i=1;
  }

  if (i==0) {
    Objet[N]->MS[0]= atof(ZTexte[ 9].Variable);
    Objet[N]->MS[1]= atof(ZTexte[10].Variable);
    Objet[N]->MS[2]= atof(ZTexte[11].Variable);
    Objet[N]->MR[0]= atof(ZTexte[12].Variable);
    Objet[N]->MR[1]= atof(ZTexte[13].Variable);
    Objet[N]->MR[2]= atof(ZTexte[14].Variable);
    Objet[N]->MT[0]= atof(ZTexte[15].Variable);
    Objet[N]->MT[1]=-atof(ZTexte[16].Variable);
    Objet[N]->MT[2]=-atof(ZTexte[17].Variable);
    Objet[N]->S[0]= atof(ZTexte[0].Variable);
    Objet[N]->S[1]= atof(ZTexte[1].Variable);
    Objet[N]->S[2]= atof(ZTexte[2].Variable);
    Objet[N]->R[0]= atof(ZTexte[3].Variable);
    Objet[N]->R[1]= atof(ZTexte[4].Variable);
    Objet[N]->R[2]= atof(ZTexte[5].Variable);
    Objet[N]->T[0]= atof(ZTexte[6].Variable);
    Objet[N]->T[1]=-atof(ZTexte[7].Variable);
    Objet[N]->T[2]=-atof(ZTexte[8].Variable);
    Objet[N]->Selection=Cc[11].Croix;
    Objet[N]->Ignore=Cc[12].Croix;
    Objet[N]->Freeze=Cc[13].Croix;
    Objet[N]->Cache=Cc[14].Croix;
    Objet[N]->Ombre=Cc[15].Croix;
    Objet[N]->ScaleOM=Cc[16].Croix;
    Objet[N]->Halo=Cc[17].Croix;
    if (Objet[N]->Freeze && Objet[N]->Selection) Objet[N]->Selection=0;
    if (Pastille[11].Croix) Objet[N]->Rapide=0;
    if (Pastille[12].Croix) Objet[N]->Rapide=1;
    if (Pastille[13].Croix) Objet[N]->Rapide=2;
    if (memcmp(Objet[N],Objet[0],sizeof(VOLUME))!=0) {
      Objet[0]->Selection=0;
      Objet[0]->Couleur=FFOND;
      trace_volume_all(0,0);
      trace_volume_all(N,N);
    }
    goto LABEL_ATTRIBUT_OBJET;
  }

  forme_mouse(MS_FLECHE);
}

// --------------------------------------------------------------------------
// -- MODIFIE SCALE ROTATE AND TRANSLATE ------------------------------------
// --------------------------------------------------------------------------
int panel_srt(byte Type) {
  int X1=CentX-130;
  int X2=CentX+130;
  int Y1=CentY-50;
  int Y2=CentY+50;
  int N,i,j,C,S;
  TRANSFORM *Mat;
  static Vecteur SV={1.0,1.0,1.0};
  static Vecteur RV={0.0,0.0,0.0};
  static Vecteur TV={0.0,0.0,0.0};
  static byte Axis[3]={1,1,1};
  Vecteur V;
  Vecteur SC,SH,RO,TR;
  char CX[13],CY[13],CZ[13];

  if (pas_objet(1)) return 0;

  forme_mouse(MS_SELECTEUR);
  message("Select an object to modify");

  if ((N=trouve_volume(0,3,1))==FAUX) return 0;

  message("Enter new values for adding or setting to the object");
  init_bouton(50,X2-65,Y1+30,55,20,"SET",CENTRE,ATTEND,"Set the value to the object");
  init_bouton(51,X2-65,Y1+50,55,20,"ADD",CENTRE,ATTEND,"Add the value to the object");
  init_bouton(52,X2-65,Y1+70,55,20,"CANCEL",CENTRE,ATTEND,"Exit the window");
  init_case(11,X2-125,Y1+33,"Use X",Axis[_X],"Use entry on X axis");
  init_case(12,X2-125,Y1+53,"Use Y",Axis[_Y],"Use entry on Y axis");
  init_case(13,X2-125,Y1+73,"Use Z",Axis[_Z],"Use entry on Z axis");

  switch (Type) {
    case SCALE:
      g_fenetre(X1,Y1,X2,Y2,"MODIFY OBJECT : SCALE",AFFICHE);
      sprintf(CX,"%.4f",SV[0]);
      sprintf(CY,"%.4f",SV[1]);
      sprintf(CZ,"%.4f",SV[2]);
      init_texte(0,X1+55,Y1+ 32,"Scale x",CX,6,"Modify X scale");
      init_texte(1,X1+55,Y1+ 52,"Scale y",CY,6,"Modify Y scale");
      init_texte(2,X1+55,Y1+ 72,"Scale z",CZ,6,"Modify Z scale");
      affiche_bouton(50);
      affiche_bouton(51);
      affiche_bouton(52);
      place_zone_texte(0);
      place_zone_texte(1);
      place_zone_texte(2);
      affiche_case(11);
      affiche_case(12);
      affiche_case(13);
      break;
    case ROTATE:
      g_fenetre(X1,Y1,X2,Y2,"MODIFY OBJECT : ROTATE",AFFICHE);
      sprintf(CX,"%.4f",RV[0]);
      sprintf(CY,"%.4f",RV[1]);
      sprintf(CZ,"%.4f",RV[2]);
      init_texte(0,X1+55,Y1+ 32,"Rotate x",CX,6,"Modify X rotate");
      init_texte(1,X1+55,Y1+ 52,"Rotate y",CY,6,"Modify Y rotate");
      init_texte(2,X1+55,Y1+ 72,"Rotate z",CZ,6,"Modify Z rotate");
      affiche_bouton(50);
      affiche_bouton(51);
      affiche_bouton(52);
      place_zone_texte(0);
      place_zone_texte(1);
      place_zone_texte(2);
      affiche_case(11);
      affiche_case(12);
      affiche_case(13);
      break;
    case TRANSLATE:
      g_fenetre(X1,Y1,X2,Y2,"MODIFY OBJECT : TRANSLATE",AFFICHE);
      sprintf(CX,"%.4f",TV[0]);
      sprintf(CY,"%.4f",TV[1]);
      sprintf(CZ,"%.4f",TV[2]);
      init_texte(0,X1+65,Y1+ 32,"Translate x",CX,6,"Modify X translate");
      init_texte(1,X1+65,Y1+ 52,"Translate y",CY,6,"Modify Y translate");
      init_texte(2,X1+65,Y1+ 72,"Translate z",CZ,6,"Modify Z translate");
      affiche_bouton(50);
      affiche_bouton(51);
      affiche_bouton(52);
      place_zone_texte(0);
      place_zone_texte(1);
      place_zone_texte(2);
      affiche_case(11);
      affiche_case(12);
      affiche_case(13);
      break;
  }

  while (1) {
    test_texte(0,2);
    test_case(11,13);
    if ((i=test_bouton(50,52))!=-1) break;
    if (sequence_sortie()) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==50 || i==51) {
    S=Objet[N]->Selection;
    C=Objet[N]->Couleur;
    Objet[N]->Selection=0;
    Objet[N]->Couleur=FFOND;
    trace_volume_all(N,N);

    vect_init(SV,1.0,1.0,1.0);
    vect_init(RV,0.0,0.0,0.0);
    vect_init(TV,0.0,0.0,0.0);

    if (Type==SCALE) vect_init(V,1.0,1.0,1.0); else vect_init(V,0.0,0.0,0.0);

    for (j=_X;j<=_Z;j++) {
      if (Type==SCALE && Cc[11+j].Croix) SV[j]=atof(ZTexte[j].Variable);
      if (Type==ROTATE && Cc[11+j].Croix) RV[j]=atof(ZTexte[j].Variable);
      if (Type==TRANSLATE && Cc[11+j].Croix) TV[j]=atof(ZTexte[j].Variable);
      if (Cc[11+j].Croix) V[j]=atof(ZTexte[j].Variable);
      Axis[j]=Cc[11+j].Croix;
    }

    switch (i) {
      case 50: // ------------- SET
        for (j=_X;j<=_Z;j++) {
          if (Cc[11+j].Croix && Type==SCALE) Objet[N]->S[j]=atof(ZTexte[j].Variable);
          if (Cc[11+j].Croix && Type==ROTATE) Objet[N]->R[j]=atof(ZTexte[j].Variable);
          if (Cc[11+j].Croix && Type==TRANSLATE) Objet[N]->T[j]=atof(ZTexte[j].Variable);
        }
        break;
      case 51: // ------------- ADD
        Mat=Create_Transform();
        ajustement_objet(Mat,Objet[N]->S,'+');
        rotation_objet(Mat,Objet[N]->R,'+');
        if (Type==SCALE) ajustement_objet(Mat,V,'+');
        V[2]*=-1;
        if (Type==ROTATE) rotation_objet(Mat,V,'+');
        translation_objet(Mat,Objet[N]->T,'+');
        V[1]*=-1;
        V[2]*=-1;
        if (Type==TRANSLATE) translation_objet(Mat,V,'+');
        mat_decode(Mat->matrix,SC,SH,RO,TR);
        vect_init(Objet[N]->T,TR[0],TR[1],TR[2]);
        vect_copy(Objet[N]->R,RO);
        vect_copy(Objet[N]->S,SC);
        Efface_Transform(Mat);
        break;
    }
    Objet[N]->Selection=S;
    Objet[N]->Couleur=C;
    trace_volume_all(N,N);
  }

  return 1;
}

// --------------------------------------------------------------------------
// -- MODIFIE LES ATTRIBUTS DES OBJETS SELECTIONNES -------------------------
// --------------------------------------------------------------------------
void change_attributs_selection(void) {
  int X1=CentX-90;
  int X2=CentX+90;
  int Y1=CentY-90;
  int Y2=CentY+93,i;
  byte P1,P2,P3,P4,P5;

  if (!Selection) { f_erreur("There's no selected objects..."); return; }

  g_fenetre(X1,Y1,X2,Y2,"MODIFY SELECTED OBJECTS",AFFICHE);

  text_xy(X1+10,Y1+30,"Off",NOIR);
  text_xy(X1+40,Y1+30,"Let",NOIR);
  text_xy(X1+70,Y1+30,"On",NOIR);

  init_pastille(11,X1+10,Y1+50,"",0,"Don't cast shadows");
  init_pastille(12,X1+40,Y1+50,"",1,"Let parameter like it is");
  init_pastille(13,X1+70,Y1+50,"Cast shadows",0,"Cast shadows");

  init_pastille(14,X1+10,Y1+70,"",0,"Don't scale texture");
  init_pastille(15,X1+40,Y1+70,"",1,"Let parameter like it is");
  init_pastille(16,X1+70,Y1+70,"Scale texture",0,"Scale texture with object");

  init_pastille(17,X1+10,Y1+90,"",0,"Don't make hollow to object");
  init_pastille(18,X1+40,Y1+90,"",1,"Let parameter like it is");
  init_pastille(19,X1+70,Y1+90,"Hollow",0,"Set object to hollow");

  init_pastille(20,X1+10,Y1+110,"",0,"Don't freeze objects");
  init_pastille(21,X1+40,Y1+110,"",1,"Let parameter like it is");
  init_pastille(22,X1+70,Y1+110,"Freeze",0,"Freeze objects");

  init_pastille(23,X1+10,Y1+130,"",0,"Object don't looks_like light");
  init_pastille(24,X1+40,Y1+130,"",1,"Let parameter like it is");
  init_pastille(25,X1+70,Y1+130,"Looks_like",0,"Object looks_like light");

  for (i=11;i<=25;i++) affiche_pastille(i);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(11,13);
    test_groupe_pastille(14,16);
    test_groupe_pastille(17,19);
    test_groupe_pastille(20,22);
    test_groupe_pastille(23,25);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==1) return;

  P1=quelle_pastille_dans_groupe(11,13)-11;
  P2=quelle_pastille_dans_groupe(14,16)-14;
  P3=quelle_pastille_dans_groupe(17,19)-17;
  P4=quelle_pastille_dans_groupe(20,22)-20;
  P5=quelle_pastille_dans_groupe(23,25)-23;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Selection && Objet[i]->Cache==0) {
      Objet[i]->Ombre=P1==0 ? 0:P1==2 ? 1:Objet[i]->Ombre;
      Objet[i]->ScaleOM=P2==0 ? 0:P2==2 ? 1:Objet[i]->ScaleOM;
      Objet[i]->Halo=P3==0 ? 0:P3==2 ? 1:Objet[i]->Halo;
      Objet[i]->Freeze=P4==0 ? 0:P4==2 ? 1:Objet[i]->Freeze;
      Objet[i]->LooksLike.Nb=P5==0 ? 0:P5==2 ? 1:Objet[i]->LooksLike.Nb;
      Objet[i]->LooksLike.Light=P5==0 ? 0:P5==2 ? 1:Objet[i]->LooksLike.Light;
    }
  }
}
