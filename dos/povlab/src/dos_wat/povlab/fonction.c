/* ---------------------------------------------------------------------------
*  FONCTION.C
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
#include <FLOAT.H>
#include <STDLIB.H>
#include <STDARG.H>
#include <STDIO.H>
#include <IO.H>
#include <DOS.H>
#include <PROCESS.H>
#include <STRING.H>
#include <IO.H>
#include <DIRECT.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"
#if !WINDOWS
#include <GRAPH.H>
#endif

byte Fx4=1;

// ----------------------------------------------------------------------------
// -- TRACE UN CERCLE POUR CREATION OBJET -------------------------------------
// ----------------------------------------------------------------------------
void trace_cercle(DBL X,DBL Y,DBL S,byte Mode) {
  register int i,NbPoint;
  type_ecriture(Mode);

  NbPoint=data_objet(CERCLEZ24,NumObjet,NF);

  for (i=0;i<=NbPoint;i++) {
    Point[i].X*=Vid[NF].Echelle*S;
    Point[i].Y*=Vid[NF].Echelle*S;
    Point[i].X+=Vid[NF].Echelle*(X+Vid[NF].Depla.X);
    Point[i].Y+=Vid[NF].Echelle*(Y+Vid[NF].Depla.Y);
  }

  select_vue(NF,CLIP_ON);

  for (i=0;i<=NbPoint;i++) {
    if (Point[i].V==1) {
      move_to(Vid[NF].WXs2+Point[i].X,Vid[NF].WYs2+Point[i].Y);
    } else {
      g_ligne_to(Vid[NF].WXs2+Point[i].X,Vid[NF].WYs2+Point[i].Y,FFOND==NOIR ? 7:FFOND | BLANC);
    }
  }

  type_ecriture(COPY_PUT);
}

// ----------------------------------------------------------------------------
// -- TRACE UN CERCLE POUR PREPARER UN VOLUME ---------------------------------
// ----------------------------------------------------------------------------
DBL selecteur_cercle (DBL X,DBL Y,DBL Vmax,DBL Vini,byte Efface) {
  register DBL XA,YA,XB,YB,Facteur=Vini;

  Vmax=fabs(Vmax);

  while(MouseB());

  trace_cercle(X,Y,Facteur,XOR_PUT);
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) {
      if (Efface==1) trace_cercle(X,Y,Facteur,XOR_PUT);
      return NFAUX;
    }
    if (XA!=XB || YA!=YB) {
      trace_cercle(X,Y,Facteur,XOR_PUT);
      Facteur+=(((XA-XB)+(YA-YB))*0.5)/Vid[NF].Echelle;
      Facteur=(fabs(Facteur)>=Vmax ? Vmax:Facteur);
      trace_cercle(X,Y,Facteur,XOR_PUT);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("Radius=%.2lf",Facteur);
    }
  }

  if (Efface==1) trace_cercle(X,Y,Facteur,XOR_PUT);
  return Facteur;
}

// ----------------------------------------------------------------------------
// -- TRACE UNE CROIX DE SELECTION (2 LIGNES VERTICALES) ----------------------
// ----------------------------------------------------------------------------
void trace_croix (DBL XB,DBL YB,byte Valeur) {
  type_ecriture(XOR_PUT);
  select_vue(NF,CLIP_ON);

  if (Valeur) {
    XB=Vid[NF].WXs2+(XB+Vid[NF].Depla.X)*Vid[NF].Echelle;
    YB=Vid[NF].WYs2+(YB+Vid[NF].Depla.Y)*Vid[NF].Echelle;
  }

  move_to(XB,0);
  g_ligne_to(XB,Vid[NF].WY,FFOND | BLANC);
  move_to(0,YB);
  g_ligne_to(Vid[NF].WX,YB,FFOND | JAUNE);
  type_ecriture(COPY_PUT);
}

// ----------------------------------------------------------------------------
// -- MOTEUR POUR UN SELECTEUR DE POSITION EN CROIX (2 LIGNES VERTICALES) -----
// ----------------------------------------------------------------------------
void selecteur_croix (void) {
  DBL PasX,PasY,SX,SY;
  register DBL XA,YA,XB,YB;

  SX=PasX=-Vid[NF].Depla.X+(gmx_v()-Vid[NF].XF-Vid[NF].WXs2)/Vid[NF].Echelle;
  SY=PasY=-Vid[NF].Depla.Y+(gmy_v()-Vid[NF].YF-Vid[NF].WYs2)/Vid[NF].Echelle;
  trace_croix(PasX,PasY,1);

  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  delay(100);
  select_vue(NF,CLIP_ON);
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { Val.X=NFAUX; break; }
    if (XA!=XB || YA!=YB) {
      trace_croix(PasX,PasY,1);
      if (Vid[NF].OptSnap) {
        valeur_snap(NF,XA,XB,&PasX,&SX,Vid[NF].SnapX);
        valeur_snap(NF,YA,YB,&PasY,&SY,Vid[NF].SnapY);
      } else {
        PasX+=(XA-XB)/Vid[NF].Echelle;
        PasY+=(YA-YB)/Vid[NF].Echelle;
      }
      trace_croix(PasX,PasY,1);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("X:%.2lf Y:%.2lf ",PasX,-PasY);
    }
  }

  trace_croix(PasX,PasY,1);
  Val.X=PasX;
  Val.Y=PasY;
}

// ----------------------------------------------------------------------------
// -- TRACE UN RECTANGLE DE SELECTION -----------------------------------------
// ----------------------------------------------------------------------------
void trace_rectangle(DBL XA,DBL YA,DBL XB,DBL YB,byte Croix,byte Ligne,byte C3D) {
  if (!C3D) {
    XA=Vid[NF].WXs2+(XA+Vid[NF].Depla.X)*Vid[NF].Echelle;
    YA=Vid[NF].WYs2+(YA+Vid[NF].Depla.Y)*Vid[NF].Echelle;
    XB=Vid[NF].WXs2+(XB+Vid[NF].Depla.X)*Vid[NF].Echelle;
    YB=Vid[NF].WYs2+(YB+Vid[NF].Depla.Y)*Vid[NF].Echelle;
  }

  type_motif_ligne(Ligne);
  type_ecriture(XOR_PUT);

  select_vue(NF,CLIP_ON);

  move_to(XA,YA);
  g_ligne_to(XB,YA,FFOND | BLANC);
  g_ligne_to(XB,YB,FFOND | BLANC);
  g_ligne_to(XA,YB,FFOND | BLANC);
  g_ligne_to(XA,YA,FFOND | BLANC);
  
  type_motif_ligne(0);
  type_ecriture(COPY_PUT);
  if (Croix) trace_croix(XB,YB,0);
}

// ----------------------------------------------------------------------------
// -- DEFINI UNE NOUVELLE ZONE DE FENETRE (SELECTION) -------------------------
// ----------------------------------------------------------------------------
int bouton_zone (byte Redraw) {
  DBL X,Y,XA,YA,XB,YB,PasX,PasY;
  DBL E;

  if (pas_objet(1)) return -1;

  forme_mouse(MS_FLECHE);
  if ((cherche_fenetre()==FAUX) || NF==3) return -1;

  while (MouseB());

  message("Move the mouse to select a zone");

  E=Vid[NF].Echelle;
  GMouseOff();
  while (MouseB());

  X=PasX=-Vid[NF].Depla.X+(gmx_v()-Vid[NF].XF-Vid[NF].WX*0.5)/Vid[NF].Echelle;
  Y=PasY=-Vid[NF].Depla.Y+(gmy_v()-Vid[NF].YF-Vid[NF].WY*0.5)/Vid[NF].Echelle;
  
  trace_rectangle(X,Y,PasX,PasY,1,1,0);
  
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  delay(100);
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) { XB=X; YB=Y; break; }
    if (XA!=XB || YA!=YB) {
      trace_rectangle(X,Y,PasX,PasY,1,1,0);
      PasX+=(DBL) (XA-XB)/E;
      PasY+=(DBL) (YA-YB)/E;
      trace_rectangle(X,Y,PasX,PasY,1,1,0);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("X:%.2lf Y:%.2lf ",PasX,-PasY);
    }
  }

  trace_rectangle(X,Y,PasX,PasY,1,1,0);
  
  if (XB!=X || YB!=Y) {
    if (X>PasX) swap_dbl(&X,&PasX);
    if (Y>PasY) swap_dbl(&Y,&PasY);
    Vid[NF].Depla.X+=-Vid[NF].Depla.X-((X+PasX)*0.5);
    Vid[NF].Depla.Y+=-Vid[NF].Depla.Y-((Y+PasY)*0.5);
    XA=(PasX-X);
    YA=(PasY-Y);
    if (XA!=0 && YA!=0) {
      if (XA<=YA) {
        Vid[NF].Echelle=fabs(Vid[NF].WY/YA);
      } else {
        Vid[NF].Echelle=fabs(Vid[NF].WX/XA);
      }
    }
    if (Redraw) redessine_fenetre(NF,1);
  }

  while (MouseB());
  GMouseOn();
  return -1;
}

// ----------------------------------------------------------------------------
// -- REMISE A ZERO DES MIN ET MAX DE LA SCENE --------------------------------
// ----------------------------------------------------------------------------
void reset_min_max(void) {
  register int i;

  for (i=0;i<=3;i++) {
    Vid[i].Max.X=DBL_MIN_; // -1000000.0;
    Vid[i].Max.Y=DBL_MIN_; // -1000000.0;
    Vid[i].Min.X=DBL_MAX_; // +1000000.0;
    Vid[i].Min.Y=DBL_MAX_; // +1000000.0;
  }
}

// ----------------------------------------------------------------------------
// -- RE-CENTRE FENETRES POUR LOGER TOUT LE DESSIN ----------------------------
// ----------------------------------------------------------------------------
int bouton_recentre (void) {
  register int i,j,D,F;
  DBL LongMaxX,LongMaxY,AncEchelle;

  if (pas_objet(0) && pas_lumiere(0) && pas_camera(0)) {
    beep_erreur();
    return -1;
  }

  if (NF==3) {
    D=0; F=2;
  } else {
    if (status_clavier() & 8) {
      D=0; F=2;
    } else {
      D=F=NF;
    }
  }

  if (Fx4==0) D=F=NF;

  reset_min_max();

  for (i=1;i<=NbObjet;i++) {
    if (!Objet[i]->Ignore) {
      cherche_volume_0(i,1);
      cherche_volume_1(i,1);
      cherche_volume_2(i,1);
    }
  }

  for (j=D;j<=F;j++) {
    if (NbCamera>0) max_min_camera(j);
    if (NbSpot>0) max_min_spot(j);
    if (NbCylLight>0) max_min_cyllight(j);
    if (NbOmni>0) max_min_omni(j);
    if (NbArea>0) max_min_area(j);

    AncEchelle=Vid[j].Echelle;

    Vid[j].Depla.X=-(Vid[j].Max.X+Vid[j].Min.X)*0.5;
    Vid[j].Depla.Y=-(Vid[j].Max.Y+Vid[j].Min.Y)*0.5;

    LongMaxX=fabs(Vid[j].Max.X)+fabs(Vid[j].Min.X);
    LongMaxY=fabs(Vid[j].Max.Y)+fabs(Vid[j].Min.Y);

    if (LongMaxX/Vid[j].WX>LongMaxY/Vid[j].WY) {
      Vid[j].Echelle=(Vid[j].WX-10)/LongMaxX;
    } else {
      Vid[j].Echelle=(Vid[j].WY-10)/LongMaxY;
    }

    if (AncEchelle!=Vid[j].Echelle) modif_echelle(j,Vid[j].Echelle,'=',1);
  }

  return -1;
}

// ----------------------------------------------------------------------------
// -- NOUVELLE SCENE ----------------------------------------------------------
// ----------------------------------------------------------------------------
int bouton_nouveau(byte Question) {
  register int i;

  if (pas_objet(0) && pas_lumiere(0) && pas_camera(0)) return -1;

  if (Question) {
    strcpy(StrBoite[0],"NEW SCENE");
    strcpy(StrBoite[1],"Do you really want to delete all");
    strcpy(StrBoite[2],"objects in the current scene ?");
    if (g_boite_ONA(CentX,CentY,2,CENTRE,1)) return -1;
  }

  for (i=0;i<=NbPoly;i++) free_mem_poly(i);
  for (i=0;i<=NbSpline;i++) free_mem_spline(i);
  for (i=1;i<=NbObjet;i++) free_mem_objet(i);

  NbSpline=NbPoly=-1;
  NbObjet=NbOmni=NbSpot=NbCylLight=NbArea=NbCamera=NumCam=0;
  Selection=0;
  Atmos_OnOff=0;
  for (i=0;i<=4;i++) Fog[i].Ok=0;
  NbRendu=0;

  for (i=0;i<=2;i++) {
    Vid[i].Depla.X=0;
    Vid[i].Depla.Y=0;
    Vid[i].Echelle=45;
  }

  if (Question) {
    strcpy(CheminSCN,NewChemin);
    strcat(CheminSCN,"SCENE\\");
    strcpy(FichierSCN,"NONAME.SCN");
    redessine_fenetre(5,1);
    affiche_donnees();
  }

  log_out(0,"--> New scene");

  return -1;
}

// ----------------------------------------------------------------------------
// -- CHERCHE SI CURSEUR SOURIS SUR LES MENUS ---------------------------------
// ----------------------------------------------------------------------------
byte chercher_menu(byte Forme) {
  register int MSX=gmx_v();
  register int MSY=gmy_v();

  if (MSY-YHotSpot+4>YMenuD || (MSX-XHotSpot+4>XMenuD && MSX-XHotSpot+4<XMenuF)) {
    if (CURSEUR!=MS_FLECHE) forme_mouse(MS_FLECHE);
    return VRAI;
  }

  if (CURSEUR!=Forme) forme_mouse(Forme);
  return FAUX;
}

// --------------------------------------------------------------------------
// ----- BOITE DE DIALOGUE POUR QUITTER OU NON ------------------------------
// --------------------------------------------------------------------------
int test_quitter(void) {
  if (pas_objet(0) && pas_lumiere(0) && pas_camera(0)) {
    strcpy(StrBoite[0],"Message");
    strcpy(StrBoite[1],"Do you really want to quit");
    sprintf(StrBoite[2],"%s %s now and exit to dos ?",NomLogiciel,VerLogiciel);
    if (g_boite_ONA (CentX,CentY,2,CENTRE,1)) return 0;
  }
  return -2;
}

// ----------------------------------------------------------------------------
// -- SCANNING DES OBJETS POUR TRANSLATION DE LA SCENE ------------------------
// ----------------------------------------------------------------------------
byte translation_objets (void) {
   register int NumeroObjet,i;
   int X=gmx_r(),Y=gmy_r();

   NumeroObjet=NumObjet;
   select_vue(NF,CLIP_ON);
   _clearscreen(_GVIEWPORT);

   trace_grille(NF);
   trace_axes(NF);
   trace_coord(NF);

   for (i=1;i<=NbObjet;i++) {
     NumObjet=i;
     if (!Objet[i]->Cache) trace_boite(NF,MODIF);
     if (X!=gmx_r() || Y!=gmy_r() || MouseB()) return 1;
   }

   NumObjet=NumeroObjet;
   return 0;
}

// ----------------------------------------------------------------------------
// -- TRANSLATION DE LA SCENE -------------------------------------------------
// ----------------------------------------------------------------------------
int bouton_deplacement(void) {
  register int X1,Y1,X2,Y2;
  register DBL PasX,PasY,PasDebutX,PasDebutY;

  if (pas_objet(1)) return -1;

  LABEL_DEPLACEMENT:
  forme_mouse(Sens);
  message("Select a viewport (TAB=choose direction)");
  if (cherche_fenetre()==FAUX) return -1;

  if (NF==3) { pan_rotate_camera(ROTATE); return -1; }

  PasDebutX=PasX=Vid[NF].Depla.X;
  PasDebutY=PasY=Vid[NF].Depla.Y;

  select_vue(NF,CLIP_ON);
  
  while (MouseB());
  GMouseOff();
  forme_mouse(MS_FLECHE);

  translation_objets();
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();
  
  while (MouseB()!=1) {
    X1=gmx_r();
    Y1=gmy_r();
    if (sequence_sortie()) { PasX=PasDebutX; PasY=PasDebutY; break; }
    if (X1!=X2 || Y1!=Y2) {
      switch (Sens) {
        case MS_X:
          PasX+=(X1-X2)/Vid[NF].Echelle;
          break;
        case MS_Y:
          PasY+=(Y1-Y2)/Vid[NF].Echelle;
          break;
        default:
          PasX+=(X1-X2)/Vid[NF].Echelle;
          PasY+=(Y1-Y2)/Vid[NF].Echelle;
          break;
      }
      if ((kbhit()) && getch()==9) {
        PasX=PasDebutX;
        PasY=PasDebutY;
        Sens_Souris();
      }
      Vid[NF].Depla.X=PasX;
      Vid[NF].Depla.Y=PasY;
      place_mouse(CentX,CentY);
      X2=gmx_r();
      Y2=gmy_r();
      message("X=%+.2lf Y=%+.2lf",PasX,-PasY);
      translation_objets();
    }
  }

  Vid[NF].Depla.X=PasX;
  Vid[NF].Depla.Y=PasY;
      
  if (PasX!=PasDebutX || PasY!=PasDebutY) {
    redessine_fenetre(5,1);
  } else {
    redessine_fenetre(NF,1);
    return -1;
  }

  
  GMouseOn();
  goto LABEL_DEPLACEMENT;
}

// ----------------------------------------------------------------------------
// -- AFFICHE LA PALETTE DE COULEUR -------------------------------------------
// ----------------------------------------------------------------------------
void bouton_couleur(void) {
  register int i,j,n;

  for (i=n=1;i<=640;i+=40) {
    for (j=1;j<=480;j+=30) {
      g_rectangle(i,j,i+40,j+30,n,1);
      //gprintf(j+12,i+10,15,n,"%d",n);
      n++;
    }
  }
  getch();
  interface(1);
}

// ----------------------------------------------------------------------------
// -- PROCESSUS DE CREATION D'UN TORE -----------------------------------------
// ----------------------------------------------------------------------------
byte creation_tore (void) {
  DBL PasX,PasY,Scale;
  DBL Facteur[2];
  int N=NumObjet;

  message("Select a viewport for the torus");

  forme_mouse(MS_FLECHE);
  if ((cherche_fenetre()==FAUX) || NF==3) return 0;

  message("Clic for the torus center");

  select_vue(NF,CLIP_ON);
  while (MouseB());
  GMouseOff();

  // --------------- recherche position

  selecteur_croix();
  if (Val.X==NFAUX) return 0;

  PasX=Val.X;
  PasY=Val.Y;

  if (NF==0) Objet[N]->T[0]=PasX;
  if (NF==0) Objet[N]->T[1]=PasY;
  if (NF==1) Objet[N]->T[2]=PasX;
  if (NF==1) Objet[N]->T[1]=PasY;
  if (NF==2) Objet[N]->T[0]=PasX;
  if (NF==2) Objet[N]->T[2]=PasY;

  // ----------------------- Recherche des rayons majeur/mineur
  // ----------------------- Boucle en 2 temps

  message("Move the mouse for the outer radius");
  if ((Facteur[0]=selecteur_cercle(PasX,PasY,10000,0.000001,1))==NFAUX) {
    return 0;
  }

  Facteur[0]=fabs(Facteur[0]);
  trace_cercle(PasX,PasY,Facteur[0],XOR_PUT);
  message("Move the mouse for the inner radius");

  if ((Facteur[1]=selecteur_cercle(PasX,PasY,Facteur[0],0.000001,1))==NFAUX) {
    trace_cercle(PasX,PasY,Facteur[0],XOR_PUT);
    return 0;
  }

  Facteur[1]=fabs(Facteur[1]);

  trace_cercle(PasX,PasY,Facteur[0],XOR_PUT);

  Scale=Facteur[0];
  Facteur[0]=1;        // Rescale pour tenir sur 1
  Facteur[1]/=Scale;   // Idem pour l'autre facteur

  // ----------- Assignation du scale (X/Z)

  Objet[N]->S[_X]=Scale;
  Objet[N]->S[_Z]=Scale;

  // ----------- Calcul majeur puis mineur (P[_X]=majeur / P[_Y]=mineur)

  Objet[N]->P[_X]=(Facteur[0]+Facteur[1])/2;
  Objet[N]->P[_Y]=(Facteur[0]-Facteur[1])/2;
  Objet[N]->P[_Z]=Scale;
  Objet[N]->S[_Y]=(Objet[N]->S[_X]/(1/Objet[N]->P[_Y]));

  // Le reste du calcul est automatique … l'affichage
  // Voir pour cela le module DONNEES2.C

  if (NF==0) Objet[N]->R[0]=90;
  if (NF==1) Objet[N]->R[2]=90;

  if (Objet[N]->Type==QTORE) {
    Objet[N]->S[_X]=Objet[N]->S[_Z]=Scale/2;
    Objet[N]->T[_X]-=Objet[N]->S[_X];
  }

  while (MouseB());
  GMouseOn();
  return 1;
}

// -------------------------------------------------------------------------
// -- CHANGE LES ATTRIBUTS D'UN CONE TRONQUE -------------------------------
// -------------------------------------------------------------------------
void edit_cone(int N) {
  int X1=CentX-85;
  int X2=CentX+85;
  int Y1=CentY-50;
  int Y2=CentY+55;
  int i;

  g_fenetre(X1,Y1,X2,Y2,"Properties",AFFICHE);
  
  sprintf(ZTexte[0].Variable,"%.4f",Objet[N]->P[0]);
  init_texte(0,X1+83,Y1+30,"Radius top",ZTexte[0].Variable,7,"Set the cone top radius");
  sprintf(ZTexte[1].Variable,"%.4f",1.00);
  init_texte(1,X1+83,Y1+50,"Radius base",ZTexte[1].Variable,7,"Set the cone base radius");

  place_zone_texte(0);
  place_zone_texte(1);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,1);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    Objet[N]->P[0]=atof(ZTexte[0].Variable)*(1/atof(ZTexte[1].Variable));
    Objet[N]->S[0]*=atof(ZTexte[1].Variable);
    Objet[N]->S[1]*=atof(ZTexte[1].Variable);
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- CHANGE LES ATTRIBUTS D'UN TORE ---------------------------------------
// -------------------------------------------------------------------------
void edit_tore(int N) {
  int X1=CentX-85;
  int X2=CentX+85;
  int Y1=CentY-50;
  int Y2=CentY+55;
  int i;

  g_fenetre(X1,Y1,X2,Y2,"Properties",AFFICHE);
  
  sprintf(ZTexte[0].Variable,"%.4f",Objet[N]->P[0]);
  init_texte(0,X1+83,Y1+30,"Major radius",ZTexte[0].Variable,7,"Set torus major radius");
  sprintf(ZTexte[1].Variable,"%.4f",Objet[N]->P[1]);
  init_texte(1,X1+83,Y1+50,"Minor radius",ZTexte[1].Variable,7,"Set torus minor radius");

  place_zone_texte(0);
  place_zone_texte(1);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,1);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    Objet[N]->P[0]=atof(ZTexte[0].Variable);
    Objet[N]->P[1]=atof(ZTexte[1].Variable);
  }

  forme_mouse(MS_FLECHE);
}


// ----------------------------------------------------------------------------
// -- PROCESSUS DE CREATION D'UN TUBE/ANNEAU/DISQUE/CONET/QTUBE ---------------
// ----------------------------------------------------------------------------
byte creation_anneau_tube_disque (byte Lequel) {
  DBL PasX,PasY,Scale;
  DBL Facteur[2];

  message("Select a viewport for the shape");

  forme_mouse(MS_FLECHE);
  if ((cherche_fenetre()==FAUX) || NF==3) return 0;

  message("Clic for the center of the shape");

  select_vue(NF,CLIP_ON);
  while (MouseB());
  GMouseOff();

  // --------------- recherche position

  selecteur_croix();
  if (Val.X==NFAUX) { return 0; }

  PasX=Val.X;
  PasY=Val.Y;

  if (NF==0) Objet[NumObjet]->T[0]=PasX;
  if (NF==0) Objet[NumObjet]->T[1]=PasY;
  if (NF==1) Objet[NumObjet]->T[2]=PasX;
  if (NF==1) Objet[NumObjet]->T[1]=PasY;
  if (NF==2) Objet[NumObjet]->T[0]=PasX;
  if (NF==2) Objet[NumObjet]->T[2]=PasY;

  // ----------------------- Recherche des rayons majeur/mineur
  // ----------------------- Boucle en 2 temps

  message("Move the mouse for the outer radius");
  if ((Facteur[0]=selecteur_cercle(PasX,PasY,10000,0.000001,1))==NFAUX) {
    return 0;
  }

  Scale=Facteur[0]=fabs(Facteur[0]);
  trace_cercle(PasX,PasY,Facteur[0],XOR_PUT);

  if (Lequel==TUBE || Lequel==ANNEAU || Lequel==QTUBE || Lequel==CONET) {
    message("Move the mouse for the inner radius");
    if ((Facteur[1]=selecteur_cercle(PasX,PasY,Facteur[0],0.000001,1))==NFAUX) {
      trace_cercle(PasX,PasY,Facteur[0],XOR_PUT);
      return 0;
    }
  
    trace_cercle(PasX,PasY,Facteur[1],XOR_PUT);
    Facteur[1]=fabs(Facteur[1]);

    // ----------- Assignation du scale (X/Y)

    Objet[NumObjet]->S[0]=Facteur[0];
    Objet[NumObjet]->S[1]=Facteur[0];

    // ----------- R‚cup‚ration diamŠtre du trou

    Objet[NumObjet]->P[0]=Facteur[1]/Facteur[0];
    Objet[NumObjet]->P[1]=Facteur[0];
  }

  trace_cercle(PasX,PasY,Facteur[0],XOR_PUT);

  if (Lequel==TUBE || Lequel==ANNEAU || Lequel==QTUBE || Lequel==CONET) {
    trace_cercle(PasX,PasY,Facteur[1],XOR_PUT);
  }

  if (NF==1) Objet[NumObjet]->R[1]=90;
  if (NF==2) Objet[NumObjet]->R[0]=90;

  if (Objet[NumObjet]->Type==QTUBE) {
    Objet[NumObjet]->S[_X]=Objet[NumObjet]->S[_Y]=Scale/2;
    Objet[NumObjet]->T[_X]-=Scale/2;
    Objet[NumObjet]->T[_Y]+=Scale/2;
  }

  // ----------- Pas d'‚paisseur si disque ou anneau

  if (Lequel==DISQUE || Lequel==ANNEAU) {
    if (Lequel==DISQUE) {
      Objet[NumObjet]->S[_X]=Objet[NumObjet]->S[_Y]=Facteur[0];
    }
    Objet[NumObjet]->S[_Z]=1;
  }

  while (MouseB());
  GMouseOn();
  return 1;
}

// ----------------------------------------------------------------------------
// -- RENVOI EIN BETIT MEZZAGE D'ERR“R ZI PAS DES OBJETTES ! ------------------
// ----------------------------------------------------------------------------
byte pas_objet (byte Son) {
  if (NbObjet==0) {
    if (Son) beep_erreur();
    message("There's no object in the scene ! Unavailable function...");
    return VRAI;
  } else {
    return FAUX;
  }
}

// ----------------------------------------------------------------------------
// -- AGRANDI OU DIMINUE LA FENETRE (ECHELLE) ---------------------------------
// ----------------------------------------------------------------------------
int plus_moins (byte Operation,byte Redraw) {
  register int i,D,F;
  DBL Valeur;

  if (pas_objet(1)) return -1;
  if (NF==3) return -1;
  if (status_clavier() & 8) { D=0; F=2; } else D=F=NF;
  if (Fx4==0) D=F=NF;

  for (i=D;i<=F;i++) {
    Valeur=(DBL) (Vid[i].Echelle*15/100);
    zoom_rect(i,Operation);
    modif_echelle(i,Valeur,Operation,Redraw);
  }

  return -1;
}

// ----------------------------------------------------------------------------
// -- REFRAICHIT LA FENETRE OU x3 ---------------------------------------------
// ----------------------------------------------------------------------------
int bouton_rafraichit(void) {
  register byte i,D,F;

  if (status_clavier() & 8) { D=0; F=2; } else D=F=NF;
  if (Fx4==0) D=F=NF;

  for (i=D;i<=F;i++) {
    redessine_fenetre(i,1);
  }

  return -1;
}

// --------------------------------------------------------------------------
// ----- CHANGE DE RESOLUTION DANS LE LOGICIEL ------------------------------
// --------------------------------------------------------------------------
void bouton_resolution(char *NewGraphic,byte Fenetre) {
  byte Erreur;
  char OldGraphic[5];

  strcpy(OldGraphic,Resolution);
  strcpy(Resolution,NewGraphic);
  Erreur=init_gmode(1);
  if (Erreur==1) { strcpy(Resolution,OldGraphic); init_gmode(1); }
  if (Fenetre) interface(1);
  if (Erreur==1) {
    strcpy(Resolution,OldGraphic);
    strcpy(StrBoite[0],"Graphic error");
    sprintf(StrBoite[1],"%s can't switch to this graphic",NomLogiciel);
    strcpy(StrBoite[2],"mode. Reinit the old one.");
    g_boite_ONA(CentX,CentY,2,CENTRE,0);
  }
}

// --------------------------------------------------------------------------
// ----- AGGRANDIR OU RETRECIR UNE FENETRE ----------------------------------
// --------------------------------------------------------------------------
int bouton_fenetre(byte F) {
  DBL OldWX;

  if (Fx4==1) {
    trouve_fenetre(2);
    Fx4=0;
    OldWX=Vid[F].WX;
    init_vues(0);
    Vid[0].Echelle*=Vid[0].WX/OldWX;
    Vid[1].Echelle*=Vid[1].WX/OldWX;
    Vid[2].Echelle*=Vid[2].WX/OldWX;
  } else {
    if (F!=0) {
      NF=F-'1';
      redessine_fenetre(NF,1);
      return -1;
    } else {
      Fx4=1;
      OldWX=Vid[F].WX;
      init_vues(0);
      Vid[0].Echelle*=Vid[0].WX/OldWX;
      Vid[1].Echelle*=Vid[1].WX/OldWX;
      Vid[2].Echelle*=Vid[2].WX/OldWX;
    }
  }
  redessine_fenetre(5,1);
  return -1;
}

// --------------------------------------------------------------------------
// ----- RENOMME LA SCENE EN COURS ------------------------------------------
// --------------------------------------------------------------------------
byte renomme_scene(char *Titre) {
  int X1=CentX-95;
  int X2=CentX+95;
  int Y1=CentY-50;
  int Y2=CentY+50;
  int i=-1,j=-1;

  LABEL_ENTER_NAME:

  g_fenetre(X1,Y1,X2,Y2,Titre,AFFICHE);

  init_texte(0,X1+70,Y1+40,"File name",FichierSCN,10,"Name of the scene file");

  place_zone_texte(0);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,0);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    CheminSCN[0]=NULLC;
    if (ZTexte[0].Variable==NULLC || strlen(ZTexte[0].Variable)<=0) {
      goto LABEL_ENTER_NAME;
    }
    strcpy(FichierSCN,strupr(ZTexte[0].Variable));
    if ((j=strinstr(0,FichierSCN,"."))!=-1) FichierSCN[j]=NULLC;
    FichierSCN[9]=NULLC;
    strcat(FichierSCN,".SCN");
  }

  forme_mouse(MS_FLECHE);
  affiche_donnees();
  return (!i);
}

// --------------------------------------------------------------------------
// ----- COULEUR DE L'INTERFACE ---------------------------------------------
// --------------------------------------------------------------------------
byte couleur_interface (void) {
  register unsigned int X1=CentX-70;
  register unsigned int X2=CentX+70;
  register unsigned int Y1=CentY-90;
  register unsigned int Y2=CentY+90;
  int i=-1,j=-2;

  g_fenetre(X1,Y1,X2,Y2,"COLORS CHOICE",AFFICHE);

  init_bouton(50,X1+25,Y1+ 35,90,20,"Interface",CENTRE,ATTEND,RES_AIDE[23]);
  init_bouton(51,X1+25,Y1+ 55,90,20,"Viewports",CENTRE,ATTEND,RES_AIDE[33]);
  init_bouton(52,X1+25,Y1+ 75,90,20,"Texte zone",CENTRE,ATTEND,RES_AIDE[34]);
  init_bouton(53,X1+25,Y1+ 95,90,20,"Axis",CENTRE,ATTEND,RES_AIDE[35]);
  init_bouton(54,X1+25,Y1+115,90,20,"Grid",CENTRE,ATTEND,RES_AIDE[36]);
  init_bouton(55,X1+45,Y1+145,50,20,RES_BOUT[11],CENTRE,ATTEND,RES_AIDE[32]);

  affiche_bouton(50);
  affiche_bouton(51);
  affiche_bouton(52);
  affiche_bouton(53);
  affiche_bouton(54);
  affiche_bouton(55);

  while (1) {
    if (sequence_sortie()) { i=0; break; }
    if (kbhit()) { i=getch(); if (i==13 || i==27) break; }
    i=test_bouton(50,55);
    if (i==50) { j=affiche_palette(); if (j!=-1) FOND=j; else j=-2; }
    if (i==51) { j=affiche_palette(); if (j!=-1) FFOND=j; else j=-2; }
    if (i==52) { j=affiche_palette(); if (j!=-1) ZFOND=j; else j=-2; }
    if (i==53) { j=affiche_palette(); if (j!=-1) CAXE=j; else j=-2; }
    if (i==54) { j=affiche_palette(); if (j!=-1) CGRILLE=j; else j=-2; }
    if (i==55) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (j!=-2) {
    interface(1);
    redessine_fenetre(5,1);
    return 1;
  }
  
  forme_mouse(MS_FLECHE);
  return 0;
}

// --------------------------------------------------------------------------
// ----- AFFICHAGE D'UN MESSAGE DE FIN DE TRAITEMENT ------------------------
// --------------------------------------------------------------------------
void message_termine(int Sec,char *String,...) {
  char Sortie[256];
  va_list parametre;

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);
  parse_ascii_watcom(Sortie);

  message("** Function ended ** %s",Sortie);
  if (OptBeep) beep();
  sleep(Sec);
}

// -------------------------------------------------------------------------
// -- MODIFIE LA COULEUR DU FOND DE L'IMAGE --------------------------------
// -------------------------------------------------------------------------
void couleur_fond_image(void) {
  creation_couleur(&Fond_RVB[_R],&Fond_RVB[_V],&Fond_RVB[_B],"Background color for image");
}

// -------------------------------------------------------------------------
// -- APPELLE UNE BOITE DE CREATION DE COULEUR -----------------------------
// -------------------------------------------------------------------------
void creation_couleur(byte *R,byte *V,byte *B,char *Titre) {
  int X1=CentX-140;
  int X2=CentX+140;
  int Y1=CentY-105;
  int Y2=CentY+160;
  int i,j;
  byte RVBT1[3];
  byte RVBT2[3];
  byte RVB[3];
  DBL HSL[3];
  DBL RGB[3];
  byte C=(NbCouleurs==256 ? 255:13);

  RVB[_R]=*R;
  RVB[_V]=*V;
  RVB[_B]=*B;

  for (i=_R;i<=_B;i++) {
    RVBT1[i]=RVB[i];
    RVBT2[i]=Palette[C][i];
  }

  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,Titre,AFFICHE);

  modif_entree_palette(C,RVBT1[_R],RVBT1[_V],RVBT1[_B]);
  relief(X1+9,Y2-96,X2-9,Y2-44,0);
  g_rectangle(X1+10,Y2-95,X2-10,Y2-45,C,FOND);

  init_potar(0,X1+65,Y1+ 40,120,0,1,(DBL) RVBT1[_R]/255,0.01,"Red (R)");
  init_potar(1,X1+65,Y1+ 60,120,0,1,(DBL) RVBT1[_V]/255,0.01,"Green (G)");
  init_potar(2,X1+65,Y1+ 80,120,0,1,(DBL) RVBT1[_B]/255,0.01,"Blue (B)");

  RGB[_R]=(DBL) RVBT1[_R]/255;
  RGB[_V]=(DBL) RVBT1[_V]/255;
  RGB[_B]=(DBL) RVBT1[_B]/255;

  RGB[_R]=(RGB[_R]>1.0 ? 1.0:RGB[_R]);
  RGB[_V]=(RGB[_V]>1.0 ? 1.0:RGB[_V]);
  RGB[_B]=(RGB[_B]>1.0 ? 1.0:RGB[_B]);

  HSL[_R]=(HSL[_R]>1.0 ? 1.0:HSL[_R]);
  HSL[_V]=(HSL[_V]>1.0 ? 1.0:HSL[_V]);
  HSL[_B]=(HSL[_B]>1.0 ? 1.0:HSL[_B]);

  RGB_to_HSL(RGB[_R],RGB[_V],RGB[_B],&HSL[_R],&HSL[_V],&HSL[_B]);

  init_potar(3,X1+65,Y1+110,120,0,1,(DBL) HSL[_R],0.01,"Hue");
  init_potar(4,X1+65,Y1+130,120,0,1,(DBL) HSL[_V],0.01,"Saturation");
  init_potar(5,X1+65,Y1+150,120,0,1,(DBL) HSL[_B],0.01,"Luminosity");

  text_xy(X1+15,Y1+ 40-7,"Red",BLANC);
  text_xy(X1+15,Y1+ 60-7,"Grn",BLANC);
  text_xy(X1+15,Y1+ 80-7,"Blu",BLANC);

  text_xy(X1+15,Y1+110-7,"Hue",BLANC);
  text_xy(X1+15,Y1+130-7,"Sat",BLANC);
  text_xy(X1+15,Y1+150-7,"Lum",BLANC);

  affiche_potar(0);
  affiche_potar(1);
  affiche_potar(2);
  affiche_potar(3);
  affiche_potar(4);
  affiche_potar(5);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    j=test_potar(0,5);
    if (j>=0 && j<=2) {
      RGB[_R]=Potar[0].Val;
      RGB[_V]=Potar[1].Val;
      RGB[_B]=Potar[2].Val;
      RGB_to_HSL(RGB[_R],RGB[_V],RGB[_B],&HSL[_R],&HSL[_V],&HSL[_B]);
      modif_curseur(3,AFFICHE);
      modif_curseur(4,AFFICHE);
      modif_curseur(5,AFFICHE);
      Potar[3].Val=HSL[_R];
      Potar[4].Val=HSL[_V];
      Potar[5].Val=HSL[_B];
      modif_curseur(3,SAUVE);
      modif_curseur(4,SAUVE);
      modif_curseur(5,SAUVE);
      modif_entree_palette(C,Potar[0].Val*255,Potar[1].Val*255,Potar[2].Val*255);
      g_rectangle(X1+10,Y2-95,X2-10,Y2-45,C,FOND);
    }
    if (j>=3 && j<=5) {
      HSL[_R]=Potar[3].Val;
      HSL[_V]=Potar[4].Val;
      HSL[_B]=Potar[5].Val;
      HSL_to_RGB(HSL[_R],HSL[_V],HSL[_B],&RGB[_R],&RGB[_V],&RGB[_B]);
      modif_curseur(0,AFFICHE);
      modif_curseur(1,AFFICHE);
      modif_curseur(2,AFFICHE);
      Potar[0].Val=RGB[_R];
      Potar[1].Val=RGB[_V];
      Potar[2].Val=RGB[_B];
      modif_curseur(0,SAUVE);
      modif_curseur(1,SAUVE);
      modif_curseur(2,SAUVE);
      modif_entree_palette(C,Potar[0].Val*255,Potar[1].Val*255,Potar[2].Val*255);
      g_rectangle(X1+10,Y2-95,X2-10,Y2-45,C,FOND);
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  if (i==0) {
    *R=Palette[C][_R];
    *V=Palette[C][_V];
    *B=Palette[C][_B];
    for (i=_R;i<=_B;i++) {
      RVB[i]=Palette[C][i];
      Palette[C][i]=RVBT2[i];
    }
  } else {
    for (i=_R;i<=_B;i++) {
      RVB[i]=RVBT1[i];
      Palette[C][i]=RVBT2[i];
    }
    *R=RVBT1[_R];
    *V=RVBT1[_V];
    *B=RVBT1[_B];
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE FENETRE DE STATISTIQUE DE LA SCENE -----------------------
// -------------------------------------------------------------------------
void statistique_scene(void) {
  register int i;
  int Nb[23];
  long NbP=0,NbF=0;
  char Ligne[1000];

  for (i=0;i<=22;i++) Nb[i]=0;

  for (i=1;i<=NbObjet;i++) {
    switch (Objet[i]->Type) {
      case CYLINDRE :  Nb[ 0]++; break;
      case SPHERE   :  Nb[ 1]++; break;
      case CUBE     :  Nb[ 2]++; break;
      case CONE     :  Nb[ 3]++; break;
      case TORE     :  Nb[ 4]++; break;
      case TUBE     :  Nb[ 5]++; break;
      case PLANX    :  Nb[ 6]++; break;
      case PLANY    :  Nb[ 7]++; break;
      case PLANZ    :  Nb[ 8]++; break;
      case TRIANGLE :  Nb[ 9]++; break;
      case ANNEAU   :  Nb[10]++; break;
      case DISQUE   :  Nb[11]++; break;
      case DSPHERE  :  Nb[12]++; break;
      case QTORE    :  Nb[13]++; break;
      case QTUBE    :  Nb[14]++; break;
      case BLOB     :  Nb[15]++; break;
      case PRISME   :  Nb[16]++; break;
      case CONET    :  Nb[17]++; break;
      case PYRAMIDE :  Nb[18]++; break;
      case HFIELD   :  Nb[19]++; break;
      case BLOBC    :  Nb[20]++; break;
      case SUPEREL  :  Nb[21]++; break;
      case SPLINE   :  Nb[22]++; break;
    }
  }

  for (i=1;i<=NbObjet;i++) {
    switch (Objet[i]->Type) {
      case CYLINDRE :  NbP+=26;  NbF+=48;  break;
      case SPHERE   :  NbP+=114; NbF+=224; break;
      case CUBE     :  NbP+=8;   NbF+=12;  break;
      case CONE     :  NbP+=26;  NbF+=48;  break;
      case CONET    :  NbP+=14;  NbF+=24;  break;
      case TORE     :  NbP+=216; NbF+=232; break;
      case TUBE     :  NbP+=48;  NbF+=96;  break;
      case PLANX    :  NbP+=441; NbF+=800; break;
      case PLANY    :  NbP+=441; NbF+=800; break;
      case PLANZ    :  NbP+=441; NbF+=800; break;
      case TRIANGLE :
        NbP+=Poly[Objet[i]->Poly]->Nombre*2;
        NbF+=Poly[Objet[i]->Poly]->Nombre;
        break;
      case SPLINE :
        NbP+=Spline[Objet[i]->Poly]->Nombre*2;
        NbF+=Spline[Objet[i]->Poly]->Nombre;
        break;
      case ANNEAU   :  NbP+=24;  NbF+=24;  break;
      case DISQUE   :  NbP+=13;  NbF+=12;  break;
      case DSPHERE  :  NbP+=65;  NbF+=126; break;
      case QTORE    :  NbP+=74;  NbF+=144; break;
      case QTUBE    :  NbP+=16;  NbF+=28;  break;
      case BLOB     :  NbP+=114; NbF+=224; break;
      case BLOBC    :  NbP+=26;  NbF+=48;  break;
      case PRISME   :  NbP+=8;   NbF+=12;  break;
      case PYRAMIDE :  NbP+=5;   NbF+=6;   break;
      case HFIELD   :
        NbP+=Poly[Objet[i]->Poly]->Nombre*2;
        NbF+=Poly[Objet[i]->Poly]->Nombre;
        break;
    }
  }

  sprintf(Ligne,
    "     %04d : CYLINDER      |"\
    "     %04d : SPHERE        |"\
    "     %04d : CUBE          |"\
    "     %04d : CONE          |"\
    "     %04d : TORUS         |"\
    "     %04d : TUBE          |"\
    "     %04d : PLANE-X       |"\
    "     %04d : PLANE-Y       |"\
    "     %04d : PLANE-Z       |"\
    "     %04d : TRIANGLE      |"\
    "     %04d : RING          |"\
    "     %04d : DISK          |"\
    "     %04d : 1/2 SPHERE    |"\
    "     %04d : 1/4 TORUS     |"\
    "     %04d : 1/4 TUBE      |"\
    "     %04d : SPHERE BLOB   |"\
    "     %04d : PRISM         |"\
    "     %04d : TRUNCATED CONE|"\
    "     %04d : PYRAMID       |"\
    "     %04d : HEIGHT-FIELD  |"\
    "     %04d : CYLINDER BLOB |"\
    "     %04d : SUPERELLIPSOID|"\
    "     %04d : SPLINE        ",
    Nb[ 0],Nb[ 1],Nb[ 2],Nb[ 3],Nb[ 4],Nb[ 5],Nb[ 6],Nb[ 7],Nb[ 8],
    Nb[ 9],Nb[10],Nb[11],Nb[12],Nb[13],Nb[14],Nb[15],Nb[16],Nb[17],
    Nb[18],Nb[19],Nb[20],Nb[21],Nb[22]);

  message("Egual to %lu vertices and %lu faces...",NbP,NbF);

  x_fenetre(CentX,CentY,GAUCHE,0,"Scene's statistics|%s",Ligne);
}

// -------------------------------------------------------------------------
// -- MODIFIE LE NIVEAU DE MAILLAGE DE CERTAINS OBJETS ---------------------
// -------------------------------------------------------------------------
void modifie_maillage(void) {
  int X1=CentX-160;
  int X2=CentX+160;
  int Y1=CentY-50;
  int Y2=CentY+58;
  int M0=MailleTore;
  int M1=MailleHF;
  int i,j;
  byte Ok[2]={0,0};
  byte S,C;

  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"Mesh precision",AFFICHE);

  init_potar(0,X1+110,Y1+40,120,3,50,(DBL) M0,1.0,"Segments for torus objects");
  init_potar(1,X1+110,Y1+60,120,4,100,(DBL) M1,1.0,"Scanning grid for HF objects");

  text_xy(X1+10,Y1+32,"Torus segments",BLANC);
  text_xy(X1+10,Y1+52,"Height-field grid",BLANC);

  affiche_potar(0);
  affiche_potar(1);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    j=test_potar(0,1);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    if (Potar[0].Val!=M0) Ok[0]=1;
    if (Potar[1].Val!=M1) Ok[1]=1;

    for (i=1;i<=NbObjet;i++) {
      if ((Objet[i]->Type==TORE || Objet[i]->Type==QTORE) && Ok[0]) {
        S=Objet[i]->Selection;
        C=Objet[i]->Couleur;
        Objet[i]->Selection=0;
        Objet[i]->Couleur=FFOND;
        trace_volume_all(i,i);
      }
    }

    if (Potar[0].Val!=M0 && Ok[0]) MailleTore=(int) Potar[0].Val;
    if (Potar[1].Val!=M1 && Ok[1]) MailleHF=(int) Potar[1].Val;

    for (i=1;i<=NbObjet;i++) {
      if ((Objet[i]->Type==TORE || Objet[i]->Type==QTORE) && Ok[0]) {
        Objet[i]->Selection=S;
        Objet[i]->Couleur=C;
        trace_volume_all(i,i);
      }
    }
  }
  
  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- RUN AN EXTERNAL PROGRAM ----------------------------------------------
// -------------------------------------------------------------------------
void appel_programme_externe(byte N) {
  int i;

  GMouseOff();

  if (N==1 && !test_fichier(CheminVIEWERI)) {
    f_erreur("Can't find the file %s",CheminVIEWERI);
    return;
  }

  if (N==2 && !test_fichier(CheminVIEWERS)) {
    f_erreur("Can't find the file %s",CheminVIEWERS);
    return;
  }

  if ((N==2) && x_fenetre(CentX,CentY,GAUCHE,1,"Generate .POV source|"\
              "Would you like to regenerate the .POV source from|"\
              "the scene before view it ?")==0) genere_script_raytracer(3);

  if (N==3 && !test_fichier(CheminEDITEUR)) {
    f_erreur("Can't find the file %s",CheminEDITEUR);
    return;
  }

  if (N==4 && !test_fichier(CheminEDITEUR)) {
    f_erreur("Can't find the file %s",CheminEDITEUR);
    return;
  }

  if (!sauve_scene_en_quittant()) return;
  sauve_config_interface(1);

  for (i=0;i<=NbPoly;i++) free_mem_poly(i);
  for (i=0;i<=NbSpline;i++) free_mem_spline(i);
  for (i=1;i<=NbObjet;i++) free_mem_objet(i);

  fading_palette(FADEIN);
  charge_motif(EFFACE);
  fin_mode_graphique();
  text_mode();
  exit(100+N);
}

// -------------------------------------------------------------------------
// -- AFFICHE UNE FENETRE MESSAGE SHAREWARE --------------------------------
// -------------------------------------------------------------------------
byte message_shareware(void) {
  return 0;
}
