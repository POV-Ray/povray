/* ---------------------------------------------------------------------------
*  EXTRUDE.C
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
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <STRING.H>
#include <DOS.H>
#include <TIME.H>
#include <GRAPH.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

int NbVertex=0;
struct Chemin Pt;

// -----------------------------------------------------------------------
// --------- AFFICHE LA LIGNE DE COMMANDE : NB POINTS --------------------
// -----------------------------------------------------------------------
void affiche_etat_point(void) {
  select_vue(5,CLIP_OFF);
  gprintf(12,YMax-21,BLANC,ZFOND,1,3,"%03d",NbVertex);
}

// -----------------------------------------------------------------------
// --------- RECENTRE TOUS LES POINTS PAR RAPPORT A <0,0,0> --------------
// -----------------------------------------------------------------------
void recentre_points(void) {
  register int i;
  DBL XMA=DBL_MIN_;
  DBL YMA=DBL_MIN_;
  DBL XMI=DBL_MAX_;
  DBL YMI=DBL_MAX_;
  DBL X,Y;

  for (i=0;i<=NbVertex;i++) {
    XMA=(Pt.X[i]>XMA ? Pt.X[i]:XMA);
    YMA=(Pt.Y[i]>YMA ? Pt.Y[i]:YMA);
    XMI=(Pt.X[i]<XMI ? Pt.X[i]:XMI);
    YMI=(Pt.Y[i]<YMI ? Pt.Y[i]:YMI);
  }

  X=(XMA+XMI)/2;
  Y=(YMA+YMI)/2;

  for (i=0;i<=NbVertex;i++) {
    Pt.X[i]-=X;
    Pt.Y[i]-=Y;
  }

  redessine_forme();
}

// -----------------------------------------------------------------------
// --------- AFFICHE LES BOUTONS PRINCIPAUX POUR L'EXTRUSION -------------
// -----------------------------------------------------------------------
void affiche_boutons_extrusion(void) {
  init_bouton(41,XMenuD+5, 10,66,20,"New",DROITE,ATTEND,RES_AIDE[1]);
  affiche_bouton(41);

  init_bouton(42,XMenuD+5, 30,66,20,"Create",DROITE,ATTEND,RES_AIDE[0]);
  affiche_bouton(42);

  init_bouton(43,XMenuD+5, 50,66,20,"Modify",DROITE,ATTEND,RES_AIDE[1]);
  affiche_bouton(43);

  init_bouton(44,XMenuD+5, 70,66,20,"Add",DROITE,ATTEND,RES_AIDE[7]);
  affiche_bouton(44);

  init_bouton(45,XMenuD+5, 90,66,20,"Delete",DROITE,ATTEND,RES_AIDE[7]);
  affiche_bouton(45);

  init_bouton(46,XMenuD+5,110,66,20,"Redraw",DROITE,ATTEND,RES_AIDE[7]);
  affiche_bouton(46);

  init_bouton(47,XMenuD+5,130,66,20,"Generate",DROITE,ATTEND,RES_AIDE[7]);
  affiche_bouton(47);

  init_bouton(48,XMenuD+5,160,66,20,"Quit",DROITE,ATTEND,RES_AIDE[7]);
  affiche_bouton(48);

  
  efface_barre_bouton_bas( 6);
  efface_barre_bouton_bas( 8);
  efface_barre_bouton_bas(12);
  efface_barre_bouton_bas(14);
  efface_barre_bouton_bas(16);
  efface_barre_bouton_bas(17);
  efface_barre_bouton_bas(19);
  efface_barre_bouton_bas(20);
  efface_barre_bouton_bas(21);
}

// -------------------------------------------------------------------------
// -- PREPARATION FENETRE POUR EXTRUSION -----------------------------------
// -------------------------------------------------------------------------
void extrusion(void) {
  byte LastFx4=Fx4;
  byte LastNF=NF;
  int Bt=0;
  struct Video TmpVid;
  int X1=CentX-85;
  int X2=CentX+90;
  int Y1=CentY-50;
  int Y2=CentY+53;
  int i,N;
  byte New=1;

  g_fenetre(X1,Y1,X2,Y2,"Extruder",AFFICHE);
  init_pastille(11,X1+15,Y1+ 28,"Create new extruded object",New==1,"Create new extruded object");
  init_pastille(12,X1+15,Y1+ 48,"Edit existant extruded object",New==0,"Edit extruded object");
  affiche_pastille(11);
  affiche_pastille(12);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
   test_groupe_pastille(11,NbObjet ? 12:11);
   if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) New=Pastille[11].Croix;

  if (New) {
    NbVertex=0;
    N=0;
  } else {
    VECTOR V;

    message("Pick an object");
    forme_mouse(MS_SELECTEUR);
    if ((N=trouve_volume(0,3,1))==FAUX) return;
    forme_mouse(MS_FLECHE);

    if (Objet[N]->Type!=EXTRUDE) {
      f_erreur("This is not an extruded object !");
      return;
    }

    NbVertex=Objet[N]->Special.Nombre-1;
    for (i=0;i<=NbVertex;i++) {
      V=point_chaine(N,i);
      Pt.X[i]=V.x*Objet[N]->S[_X];
      Pt.Y[i]=V.y*Objet[N]->S[_Z];
    }
  }

  memcpy(&TmpVid,&Vid[NF],sizeof(struct Video));

  trouve_fenetre(2);
  NF=0;
  Fx4=0;
  Vid[NF].Depla.X=0.0;
  Vid[NF].Depla.Y=0.0;
  Vid[NF].Enable=1;
  Vid[NF].Echelle=20;
  init_vues(0);

  GMouseOff();
  g_rectangle(XMenuD+3,7,XMenuF-3,YMenuD-25+2,FOND,MOTIF);
  GMouseOn();
  affiche_boutons_extrusion();
  affiche_bureau();
  affiche_etat_point();
  redessine_forme();

  forme_mouse(MS_FLECHE);
  GMouseOn();

  while (1) {
    Bt=test_bouton(41,48);
    if (Bt==-1) Bt=test_bouton(4,5);
    if (Bt==-1) Bt=test_bouton(1,1);
    if (Bt==-1) Bt=test_bouton(7,7);
    if (Bt== 1) redessine_forme();
    if (Bt== 4) { plus_moins('-',0); redessine_forme(); }
    if (Bt== 5) { plus_moins('+',0); redessine_forme(); }
    if (Bt== 7) { bouton_zone(0); redessine_forme(); }
    if (Bt==41) {
      strcpy(StrBoite[0],"ERASE");
      strcpy(StrBoite[1],"Delete the current shape ?");
      if (!g_boite_ONA(CentX,CentY,1,CENTRE,1)) {
        NbVertex=0;
        affiche_etat_point();
        redessine_forme();
      }
    }
    if (Bt==42) {
      if (NbVertex) {
        strcpy(StrBoite[0],"EXTRUDE");
        strcpy(StrBoite[1],"Delete the current shape ?");
        if (!g_boite_ONA(CentX,CentY,1,CENTRE,1)) {
          NbVertex=0;
          affiche_etat_point();
          redessine_forme();
        }
      }
      creer_point(0);
    }
    if (Bt==43) modifier_point(0);
    if (Bt==44) ajouter_point(0);
    if (Bt==45) supprimer_point(0);
    if (Bt==46) { redessine_forme(); affiche_etat_point(); }
    if ((Bt==47) && generer_volume(N)) break;
    if (Bt==48) break;
  }

  forme_mouse(MS_FLECHE);

  Fx4=LastFx4;
  NF=LastNF;
  memcpy(&Vid[NF],&TmpVid,sizeof(struct Video));
  init_vues(0);
  GMouseOff();
  g_rectangle(XMenuD+3,7,XMenuF-3,DSM,FOND,MOTIF);
  redessine_fenetre(5,1);
  affiche_boutons_principaux();
  affiche_boutons_graphique();
  affiche_donnees();
}

// ----------------------------------------------------------------------------
// -- TRACE UNE LIGNE ---------------------------------------------------------
// ----------------------------------------------------------------------------
void trace_segment(DBL XA,DBL YA,DBL XB,DBL YB,byte Mode,byte Crx) {
  //static int Cpt;
  DBL X1,Y1;

  X1=Vid[NF].WXs2+(Vid[NF].Depla.X+XB)*Vid[NF].Echelle;
  Y1=Vid[NF].WYs2+(Vid[NF].Depla.Y+YB)*Vid[NF].Echelle;

  XA=Vid[NF].WXs2+(Vid[NF].Depla.X+XA)*Vid[NF].Echelle;
  YA=Vid[NF].WYs2+(Vid[NF].Depla.Y+YA)*Vid[NF].Echelle;
  XB=Vid[NF].WXs2+(Vid[NF].Depla.X+XB)*Vid[NF].Echelle;
  YB=Vid[NF].WYs2+(Vid[NF].Depla.Y+YB)*Vid[NF].Echelle;

  type_ecriture(Mode);

  select_vue(NF,CLIP_ON);

  g_ligne(XA,YA,XB,YB,(Mode==XOR_PUT ? FFOND | JAUNE:COBJET));
  
  type_motif_ligne(0);
  type_ecriture(COPY_PUT);
  if (Mode==XOR_PUT) {
    if (Crx==1) trace_croix(X1,Y1,0);
  } else {
    g_ligne(XB-1,YB,XB+1,YB,BLANC);
    g_ligne(XB,YB-1,XB,YB+1,BLANC);
  }
}

// ----------------------------------------------------------------------------
// -- CREER UNE NOUVELLE FORME DE POINTS --------------------------------------
// ----------------------------------------------------------------------------
int creer_point(byte BSpline) {
  register DBL X,Y,XA,YA,XB,YB,PasX,PasY;
  int A,B;

  forme_mouse(MS_SELECTEUR);
  
  message("Move mouse for position, left click to create");

  LABEL_CREER_POINT:

  GMouseOn();
  while (MouseB()!=1) {
    A=gmx_v(); B=gmy_v();
    if (sequence_sortie()) {
      forme_mouse(MS_FLECHE);
      return 0;
    }
  }

  GMouseOff();
  
  if (NbVertex>0) {
    X=PasX=Pt.X[NbVertex];
    Y=PasY=Pt.Y[NbVertex];
    while (MouseB());
  } else {
    Pt.X[0]=X=PasX=-Vid[NF].Depla.X+(A-Vid[NF].XF-Vid[NF].WXs2)/Vid[NF].Echelle;
    Pt.Y[0]=Y=PasY=-Vid[NF].Depla.Y+(B-Vid[NF].YF-Vid[NF].WYs2)/Vid[NF].Echelle;
  }
  
  trace_segment(X,Y,PasX,PasY,XOR_PUT,1);
  
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  delay(100);
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) {
      trace_segment(X,Y,PasX,PasY,XOR_PUT,1);
      while(MouseB());
      if (NbVertex>1) {
        if (!BSpline) joindre_point();
      } else {
        NbVertex=0;
      }
      if (!BSpline) recentre_points();
      forme_mouse(MS_FLECHE);
      return 1;
    }
    if (XA!=XB || YA!=YB) {
      trace_segment(X,Y,PasX,PasY,XOR_PUT,1);
      PasX+=(DBL) (XA-XB)/Vid[NF].Echelle;
      PasY+=(DBL) (YA-YB)/Vid[NF].Echelle;
      trace_segment(X,Y,PasX,PasY,XOR_PUT,1);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("Vertex: %03d X:%.2lf Y:%.2lf ",NbVertex,PasX,-PasY);
    }
  }

  trace_segment(X,Y,PasX,PasY,XOR_PUT,1);
  
  if (XB!=X || YB!=Y) {
    NbVertex++;
    Pt.X[NbVertex]=PasX;
    Pt.Y[NbVertex]=PasY;
    trace_segment(Pt.X[NbVertex-1],Pt.Y[NbVertex-1],
                  Pt.X[NbVertex],Pt.Y[NbVertex],COPY_PUT,0);
    affiche_etat_point();
  }

  goto LABEL_CREER_POINT;
}

// ----------------------------------------------------------------------------
// -- REDESSINE LA FORME ------------------------------------------------------
// ----------------------------------------------------------------------------
void redessine_forme(void) {
  register int i,Nb=NbVertex;
  GMouseOff();
  
  select_vue(5,CLIP_OFF);
  g_rectangle(Vid[NF].XF,Vid[NF].YF,Vid[NF].XF+Vid[NF].WX-1,Vid[NF].YF+Vid[NF].WY-1,FFOND,1);
  select_vue(NF,CLIP_ON);
  
  trace_grille(NF);
  trace_axes(NF);

  if (!Nb) return;

  trace_segment(Pt.X[0],Pt.Y[0],Pt.X[0],Pt.Y[0],COPY_PUT,0);

  for (i=0;i<Nb;i++) {
    trace_segment(Pt.X[i],Pt.Y[i],Pt.X[i+1],Pt.Y[i+1],COPY_PUT,0);
  }

  GMouseOn();
}

// ----------------------------------------------------------------------------
// -- JOINT 2 POINTS DE LA FORME ----------------------------------------------
// ----------------------------------------------------------------------------
void joindre_point(void) {
  if (Pt.X[0]==Pt.X[NbVertex] &&
      Pt.Y[0]==Pt.Y[NbVertex]) {
    return;
  } else {
    NbVertex++;
    Pt.X[NbVertex]=Pt.X[0];
    Pt.Y[NbVertex]=Pt.Y[0];
    redessine_forme();
  }
}

// ----------------------------------------------------------------------------
// -- TESTE UN SEGMENT --------------------------------------------------------
// ----------------------------------------------------------------------------
int test_segment(int BSpline) {
  register int i;
  register DBL XA,YA,XB,YB;

  for (i=0;i<NbVertex+(BSpline ? 1:0);i++) {
    XA=Vid[NF].WXs2+(Vid[NF].Depla.X+Pt.X[i  ])*Vid[NF].Echelle;
    YA=Vid[NF].WYs2+(Vid[NF].Depla.Y+Pt.Y[i  ])*Vid[NF].Echelle;
    XB=Vid[NF].WXs2+(Vid[NF].Depla.X+Pt.X[i+1])*Vid[NF].Echelle;
    YB=Vid[NF].WYs2+(Vid[NF].Depla.Y+Pt.Y[i+1])*Vid[NF].Echelle;
    if (test_ligne(XA-3,YA-3,XB+3,YB+3,gmx_v(),gmy_v())) return i;
    if (test_ligne(XA-3,YA-3,XB+3,YB+3,gmx_v(),gmy_v())) return i;
  }
  return -1;
}

// ----------------------------------------------------------------------------
// -- TESTE UN POINT ----------------------------------------------------------
// ----------------------------------------------------------------------------
int test_point(void) {
  register int i;
  register DBL XA,YA;

  for (i=0;i<=NbVertex;i++) {
    XA=Vid[NF].WXs2+(Vid[NF].Depla.X+Pt.X[i])*Vid[NF].Echelle;
    YA=Vid[NF].WYs2+(Vid[NF].Depla.Y+Pt.Y[i])*Vid[NF].Echelle;
    if (test_ligne(XA-3,YA-3,XA+3,YA+3,gmx_v(),gmy_v())) return i;
    if (test_ligne(XA-3,YA-3,XA+3,YA+3,gmx_v(),gmy_v())) return i;
  }

  return -1;
}

// ----------------------------------------------------------------------------
// -- VERIFIE SI IL EXISTE DEJA DES POINTS ------------------------------------
// ----------------------------------------------------------------------------
byte pas_point(byte Son) {
  if (NbVertex==0) {
    if (Son) beep_erreur();
    message("Sorry, there's no vertices...");
    return VRAI;
  } else {
    return FAUX;
  }
}

// ----------------------------------------------------------------------------
// -- INSERE UN POINT DANS LA FORME -------------------------------------------
// ----------------------------------------------------------------------------
void inserer_point(int i,DBL X,DBL Y,int BSpline) {
  register int j;
  BSpline=BSpline;

  for (j=NbVertex;j>i;j--) {
    Pt.X[j+1]=Pt.X[j];
    Pt.Y[j+1]=Pt.Y[j];
  }

  Pt.X[i+1]=X;
  Pt.Y[i+1]=Y;

  NbVertex++;
}

// ----------------------------------------------------------------------------
// -- TESTE LE NOMBRE DE POINT PAR RAPPORT AU MAXIMUM AUTORISE ----------------
// ----------------------------------------------------------------------------
byte test_nb_point(void) {
  if ((NbVertex+1)<NB_POINT_MAX) return 0;
  return 1;
}

// ----------------------------------------------------------------------------
// -- AJOUTE DES POINTS AU VOLUME EN COURS ------------------------------------
// ----------------------------------------------------------------------------
void ajouter_point(byte BSpline) {
  DBL X,Y;
  register int i,A,B;

  if (pas_point(1)) return;
 
  message("Move the mouse on the segment you want to break");

  LABEL_AJOUTER_POINT:

  if (test_nb_point()) return;

  forme_mouse(MS_SELECTEUR);
  GMouseOn();

  while (!MouseB()) {
    A=gmx_v(); B=gmy_v();
    if (sequence_sortie()) {
      forme_mouse(MS_FLECHE);
      return;
    }
  }

  while (MouseB());

  GMouseOff();
  
  X=-Vid[NF].Depla.X+(A-Vid[NF].XF-Vid[NF].WXs2)/Vid[NF].Echelle;
  Y=-Vid[NF].Depla.Y+(B-Vid[NF].YF-Vid[NF].WYs2)/Vid[NF].Echelle;

  if ((i=test_segment(BSpline))>-1) {
    inserer_point(i,X,Y,BSpline);
    affiche_etat_point();
    redessine_forme();
  }

  goto LABEL_AJOUTER_POINT;
}

// ----------------------------------------------------------------------------
// -- SUPPRIME UN POINT DANS LA FORME -----------------------------------------
// ----------------------------------------------------------------------------
void supprimer_point(byte BSpline) {
  register int N,i;

  if (pas_point(1)) return;
 
  message("Move the cursor on the vertex you want to delete");

  LABEL_SUPPRIMER_POINT:

  forme_mouse(MS_SELECTEUR);
  GMouseOn();

  while (MouseB()!=1) {
    gmx_v(); gmy_v();
    if (sequence_sortie()) {
      forme_mouse(MS_FLECHE);
      return;
    }
  }

  while (MouseB());

  GMouseOff();
  
  if ((N=test_point())>-1) {
    for (i=N;i<NbVertex+(BSpline ? 1:0);i++) {
      Pt.X[i]=Pt.X[i+1];
      Pt.Y[i]=Pt.Y[i+1];
      if (BSpline) {
        Spline[BSpline]->dots[i][_X]=Spline[BSpline]->dots[i+1][_X];
        Spline[BSpline]->dots[i][_Y]=Spline[BSpline]->dots[i+1][_Y];
        Spline[BSpline]->dots[i][_Z]=Spline[BSpline]->dots[i+1][_Z];
        Spline[BSpline]->dots[i][ 3]=Spline[BSpline]->dots[i+1][ 3];
      }
    }
    NbVertex--;
    redessine_forme();
    affiche_etat_point();
    message("Vertex nø%d deleted",N);
  }

  goto LABEL_SUPPRIMER_POINT;
}

// ----------------------------------------------------------------------------
// -- MODIFIER LES POINTS AU VOLUME EN COURS ----------------------------------
// ----------------------------------------------------------------------------
void modifier_point(byte BSpline) {
  DBL X0,Y0,X1,Y1,X2,Y2,PasX,PasY;
  int N0,N1,N2,XA,YA,XB,YB;
  byte Ferme=0;

  BSpline=BSpline;

  if (pas_point(1)) return;
 
  message("Move the mouse on the vertex you want to modify");

  LABEL_MODIFIER_POINT:

  forme_mouse(MS_SELECTEUR);
  GMouseOn();

  while (MouseB()!=1) {
    gmx_v(); gmy_v();
    if (sequence_sortie()) {
      forme_mouse(MS_FLECHE);
      return;
    }
  }

  while (MouseB());
  GMouseOff();

  if ((N1=test_point())>-1) { ; } else goto LABEL_MODIFIER_POINT;

  if (Pt.X[0]==Pt.X[NbVertex] &&
      Pt.Y[0]==Pt.Y[NbVertex]) Ferme=1;

  if (N1==NbVertex || N1==0) {
    if (Ferme) {
      N0=NbVertex-1;
      N2=1;
    } else {
      if (N1==0) { N0=1; N2=1; }
      if (N1==NbVertex) { N0=NbVertex-1; N2=NbVertex-1; }
    }
  } else {
    N0=N1-1;
    N2=N1+1;
  }

  X0=Pt.X[N0];
  Y0=Pt.Y[N0];
  X1=PasX=Pt.X[N1];
  Y1=PasY=Pt.Y[N1];
  X2=Pt.X[N2];
  Y2=Pt.Y[N2];

  message("Vertex: %03d X:%.2lf Y:%.2lf ",N1,PasX,-PasY);

  trace_segment(X0,Y0,X1,Y1,XOR_PUT,1);
  trace_segment(X1,Y1,X2,Y2,XOR_PUT,0);
  
  place_mouse(CentX,CentY);

  XB=gmx_r();
  YB=gmy_r();

  delay(100);
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) {
      trace_segment(X0,Y0,PasX,PasY,XOR_PUT,1);
      trace_segment(PasX,PasY,X2,Y2,XOR_PUT,0);
      while(MouseB());
      forme_mouse(MS_FLECHE);
      return;
    }
    if (XA!=XB || YA!=YB) {
      trace_segment(X0,Y0,PasX,PasY,XOR_PUT,1);
      trace_segment(PasX,PasY,X2,Y2,XOR_PUT,0);
      PasX+=(DBL) (XA-XB)/Vid[NF].Echelle;
      PasY+=(DBL) (YA-YB)/Vid[NF].Echelle;
      trace_segment(X0,Y0,PasX,PasY,XOR_PUT,1);
      trace_segment(PasX,PasY,X2,Y2,XOR_PUT,0);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      message("Vertex: %03d X:%.2lf Y:%.2lf ",N1,PasX,-PasY);
    }
  }

  trace_segment(X0,Y0,PasX,PasY,XOR_PUT,1);
  trace_segment(PasX,PasY,X2,Y2,XOR_PUT,0);
  
  if (XB!=X1 || YB!=Y1) {
    while (MouseB());
    if ((N1==0 || N1==NbVertex) && Ferme) {
      Pt.X[0]=Pt.X[NbVertex]=PasX;
      Pt.Y[0]=Pt.Y[NbVertex]=PasY;
    } else {
      Pt.X[N1]=PasX;
      Pt.Y[N1]=PasY;
    }
    redessine_forme();
    affiche_etat_point();
  }

  goto LABEL_MODIFIER_POINT;
}

// ----------------------------------------------------------------------------
// -- GENERER VOLUME ----------------------------------------------------------
// ----------------------------------------------------------------------------
int generer_volume(int N) {
  int i;
  int X1=CentX-80;
  int X2=CentX+80;
  int Y1=CentY-70;
  int Y2=CentY+67;
  int Ope=(int) (N==0 ? 0:Objet[N]->P[_X]);
  int Spl=(N==0 ? 0:Objet[N]->Special.Type);
  FILE *F;

  if (pas_point(1)) return 0;

  efface_buffer_clavier();

  strcpy(StrBoite[0],"EXTRUDING SHAPE");
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  init_pastille(11,X1+10,Y1+ 30,"linear spline",Spl==0,"");
  init_pastille(12,X1+10,Y1+ 45,"quadratic spline",Spl==1,"");
  init_pastille(13,X1+10,Y1+ 60,"cubic spline",Spl==2,"");
  affiche_pastille(11);
  affiche_pastille(12);
  affiche_pastille(13);
  init_case(11,X1+10,Y1+80,"Shape open",Ope,"Remove or not end caps");
  affiche_case(11);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_case(11,11);
    test_groupe_pastille(11,13);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    Spl=quelle_pastille_dans_groupe(11,13)-11;
    Ope=(DBL) Cc[11].Croix;

    F=fopen("EXTRUDE.$$$","wt");
    fprintf(F,"Object %05d: 0 0\n",0);
    fprintf(F,"Object %05d: %d\n",0,NbVertex+1);

    for (i=0;i<=NbVertex;i++) {
      fprintf(F,"Object %05d: %.4g %.4g\n",0,Pt.X[i],Pt.Y[i]);
    }
    fclose(F);

    if (N==0) {
      new_objet(EXTRUDE,2);
      N=NbObjet;
    } else {
      free_mem_special(N);
    }

    F=fopen("EXTRUDE.$$$","rt");
    lecture_special_scn(F,"00000:",N);
    fclose(F);

    Objet[N]->Special.Type=Spl;
    Objet[N]->P[_X]=(DBL) Ope;

    trace_volume_all(N,N);
    remove("EXTRUDE.$$$");
    return 1;
  }

  return 0;
}

// -------------------------------------------------------------------------
// -- ENREGISTRE PRISM AU FORMAT POV ---------------------------------------
// -------------------------------------------------------------------------
void enregistre_prism_pov(FILE *Fichier,int N,int L) {
  register int i;
  VECTOR V;
  int Nb,Total;

  Nb=Total=Objet[N]->Special.Nombre;

  /*
  switch (Objet[N]->P[_X]) {
    case 0.1: outl(Fichier,1,"conic_sweep\n"); break;
    default : outl(Fichier,1,"linear_sweep\n"); break;
  } */

  switch (Objet[N]->Special.Type) {
    case 0:
      outl(Fichier,1,"linear_spline\n"); break;
    case 1:
      outl(Fichier,1,"quadratic_spline\n");
      Total+=1;
      break;
    case 2:
      outl(Fichier,1,"cubic_spline\n");
      Total+=2;
      break;
  }

  outl(Fichier,1,"1,-1,%d,\n",Total);

  if (Objet[N]->Special.Type) {
    V=point_chaine(N,0);
    outl(Fichier,1,"<%.4g,%.4g>,\n",V.x,-V.y);
  }

  for (i=0;i<Nb;i++) {
    V=point_chaine(N,i);
    outl(Fichier,1,"<%.4g,%.4g>%c\n",V.x,-V.y,(i==Nb-1 ? ' ':','));
  }

  if (Objet[N]->Special.Type==2) {
    V=point_chaine(N,1);
    outl(Fichier,1,",<%.4g,%.4g>\n",V.x,-V.y);
  }

  outl(Fichier,1,"sturm\n");
  if (Objet[N]->P[_X]) outl(Fichier,1,"open\n");
}
