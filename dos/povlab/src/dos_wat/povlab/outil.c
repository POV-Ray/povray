/* ---------------------------------------------------------------------------
*  OUTIL.C
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

// -------------------------------------------------------------------------
// -- BOITE DEMANDANT SI LA CREATION DU NOM OBJET EST AUTOMATIQUE ----------
// -------------------------------------------------------------------------
byte nom_objet_auto(void) {
  return !x_fenetre(CentX,CentY,GAUCHE,1,"Naming new objects|"\
    "Do you want the new names to be automatic|"\
    "(%s choose the name for you) or to specify a new|"\
    "name in the dialog box for each new object ?|"\
    "Auto naming ?",LOGICIEL
  );
}

// -------------------------------------------------------------------------
// -- CREATION D'OBJETS PAR DEPLACEMENT ET COPIE ---------------------------
// -------------------------------------------------------------------------
void alignement_objet(void) {
  int X1,Y1,X2,Y2;
  int XA=CentX-110;
  int XB=CentX+110;
  int YA=CentY-45;
  int YB=CentY+55;
  DBL PasX,PasY,PasDebutX,PasDebutY;
  DBL TX,TY,TZ; // Translation
  DBL DX,DY,DZ; // Pas de d‚placement
  int Debut,Fin,i,j,Nb;
  byte Ok,NA=0;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_ALIGNE:
  forme_mouse(Sens);
  message("Select object to duplicate (TAB=choose direction)");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    cube_selection();
    NumObjet=0;
  } else {
    if ((NumObjet=trouve_volume(0,2,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);

  if (NF==0) PasDebutX=PasX=Objet[NumObjet]->T[0];
  if (NF==0) PasDebutY=PasY=Objet[NumObjet]->T[1];
  if (NF==1) PasDebutX=PasX=Objet[NumObjet]->T[2];
  if (NF==1) PasDebutY=PasY=Objet[NumObjet]->T[1];
  if (NF==2) PasDebutX=PasX=Objet[NumObjet]->T[0];
  if (NF==2) PasDebutY=PasY=Objet[NumObjet]->T[2];

  TX=Objet[NumObjet]->T[_X];
  TY=Objet[NumObjet]->T[_Y];
  TZ=Objet[NumObjet]->T[_Z];

  select_vue(NF,CLIP_ON);

  forme_mouse(Sens);
  while (MouseB());
  GMouseOff();

  trace_boite(0,3);
  place_mouse(CentX,CentY);
  X2=gmx_r();
  Y2=gmy_r();

  while (VRAI==VRAI) {
	X1=gmx_r();
	Y1=gmy_r();
    if (sequence_sortie()) { Ok=0; PasX=PasDebutX; PasY=PasDebutY; break; }
	if (MouseB()==1) { Ok=1; break; }
	if (X1!=X2 || Y1!=Y2) {
      delay(5);
      trace_boite(0,3);
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
        trace_boite(0,3);
		PasX=PasDebutX;
		PasY=PasDebutY;
		Sens_Souris();
        trace_boite(0,3);
	  }
	  if (NF==0) Objet[NumObjet]->T[0]=PasX;
	  if (NF==0) Objet[NumObjet]->T[1]=PasY;
	  if (NF==1) Objet[NumObjet]->T[2]=PasX;
	  if (NF==1) Objet[NumObjet]->T[1]=PasY;
	  if (NF==2) Objet[NumObjet]->T[0]=PasX;
	  if (NF==2) Objet[NumObjet]->T[2]=PasY;
      trace_boite(0,3);
      place_mouse(CentX,CentY);
	  X2=gmx_r();
	  Y2=gmy_r();
      message("\"%s\" [%05d]: X=%+.2lf Y=%+.2lf",Objet[NumObjet]->Nom,NumObjet,PasX,-PasY);
	}
  }

  trace_boite(0,3);

  if (NF==0) Objet[NumObjet]->T[_X]=PasX;
  if (NF==0) Objet[NumObjet]->T[_Y]=PasY;
  if (NF==1) Objet[NumObjet]->T[_Z]=PasX;
  if (NF==1) Objet[NumObjet]->T[_Y]=PasY;
  if (NF==2) Objet[NumObjet]->T[_X]=PasX;
  if (NF==2) Objet[NumObjet]->T[_Z]=PasY;

  // ----------------------- Saisie du nombre d'objets

  forme_mouse(MS_FLECHE);

  g_fenetre(XA,YA,XB,YB,"Duplicate n objects",AFFICHE);

  init_texte(0,XA+115,YA+40,"Number to duplicate","10",10,"Number of objects to linear duplicate");

  place_zone_texte(0);

  bouton_dialog(XA,XB,YB,1,1);

  while (1) {
    j=test_texte(0,0);
    if ((i=bouton_dialog(XA,XB,YB,0,1))!=-1) break;
  }

  g_fenetre(XA,YA,XB,YB,"",EFFACE);

  if (i==0) {
    Nb=atoi(ZTexte[0].Variable);
    if (Nb==0) return;
  } else {
    return;
  }

  if (Nb>1) NA=nom_objet_auto();

  // ----------------------- Proc‚dure de calcul

  if (Ok==1) {
	if (Selection && OkSelect==1) {
	  Debut=1; Fin=NbObjet;
      DX=Objet[0]->T[0]-TX;
      DY=Objet[0]->T[1]-TY;
      DZ=Objet[0]->T[2]-TZ;
      TX=DX;
      TY=DY;
      TZ=DZ;
	} else {
	  Debut=Fin=NumObjet;
      DX=Objet[NumObjet]->T[0]-TX;
      DY=Objet[NumObjet]->T[1]-TY;
      DZ=Objet[NumObjet]->T[2]-TZ;
      TX=TY=TZ=0;
	}
    for (j=1;j<=Nb;j++) {
      for (i=Debut;i<=Fin;i++) {
        if (((Objet[i]->Selection && OkSelect) || i==NumObjet) && !Objet[i]->Cache) {
          auto_copy_objet(i,NA);
          Objet[NbObjet]->T[0]+=TX;
          Objet[NbObjet]->T[1]+=TY;
          Objet[NbObjet]->T[2]+=TZ;
          trace_volume_all(NbObjet,NbObjet);
        }
      }
      TX+=DX;
      TY+=DY;
      TZ+=DZ;
    }
  }

  if (NF==0) Objet[NumObjet]->T[_X]=PasDebutX;
  if (NF==0) Objet[NumObjet]->T[_Y]=PasDebutY;
  if (NF==1) Objet[NumObjet]->T[_Z]=PasDebutX;
  if (NF==1) Objet[NumObjet]->T[_Y]=PasDebutY;
  if (NF==2) Objet[NumObjet]->T[_X]=PasDebutX;
  if (NF==2) Objet[NumObjet]->T[_Z]=PasDebutY;

  affiche_donnees();
  GMouseOn();
  goto LABEL_ALIGNE;
}

// -------------------------------------------------------------------------
// -- CREATION D'OBJETS PAR ROTATION MULTIPLES -----------------------------
// -------------------------------------------------------------------------
void duplique_rotation(void) {
  int XA=CentX-110;
  int XB=CentX+110;
  int YA=CentY-65;
  int YB=CentY+115;
  static DBL Nb=10,Angle=360,Rayon=1;
  register int i,j;
  int NO,Debut,Fin;
  Vecteur TA,RA,T0;
  TRANSFORM *Mat;
  Vecteur SC,SH,RO,TR;
  int RI=1,AM=1,NA=0; // Rotation initiale, Sens aiguille montre, Nom auto
  char Buffer[20];

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  forme_mouse(MS_SELECTEUR);
  message("Select object to duplicate by rotation");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    cube_selection();
  } else {
    if ((NO=trouve_volume(0,2,1))==FAUX) return;
  }

  forme_mouse(MS_FLECHE);

  // ----------------------- Saisie du nombre d'objets & rayon

  forme_mouse(MS_FLECHE);

  g_fenetre(XA,YA,XB,YB,"objects rotation",AFFICHE);

  sprintf(Buffer,"%d",(int) Nb);
  init_texte(0,XA+105,YA+40,"How many objects",Buffer,10,"Nb of objects to duplicate");
  sprintf(Buffer,"%.4g",Angle);
  init_texte(1,XA+105,YA+60,"Rotation angle",Buffer,10,"Rotation degrees");
  sprintf(Buffer,"%.4g",Rayon);
  init_texte(2,XA+105,YA+80,"Radius",Buffer,10,"with a radius of");
  init_case(10,XA+25,YA+105,"Affect initial object's rotation",RI," ");
  init_case(11,XA+25,YA+125,"Copy Clockwise",AM," ");

  place_zone_texte(0);
  place_zone_texte(1);
  place_zone_texte(2);
  affiche_case(10);
  affiche_case(11);

  bouton_dialog(XA,XB,YB,1,1);

  while (1) {
    test_texte(0,2);
    test_case(10,11);
    if ((i=bouton_dialog(XA,XB,YB,0,1))!=-1) break;
  }

  g_fenetre(XA,YA,XB,YB,"",EFFACE);

  if (i==0) {
    Nb=atoi(ZTexte[0].Variable);
    Angle=atof(ZTexte[1].Variable);
    Rayon=atof(ZTexte[2].Variable);
    RI=Cc[10].Croix;
    AM=(Cc[11].Croix ? 1:-1);
    if (Nb==0 || Angle==0) return;
  } else {
    return;
  }

  // ----------------------- Proc‚dure de calcul

  if (OkSelect) {
    Debut=1;
    Fin=NbObjet;
  } else {
    Debut=Fin=NO;
  }

  if (Nb>1) NA=nom_objet_auto();

  for (j=1;j<=Nb;j++) {
    for (i=Debut;i<=Fin;i++) {
      if (((Objet[i]->Selection && OkSelect) || i==NO) && !Objet[i]->Cache) {
        auto_copy_objet(i,NA);

        vect_copy(TA,Objet[i]->T);
        vect_copy(RA,Objet[i]->R);

        Mat=Create_Transform();

        if (NF==0) {
          vect_init(TR,0,Rayon,0);
          vect_init(RO,0,0,-j*(Angle/Nb)*AM);
        }

        if (NF==1) {
          vect_init(TR,0,Rayon,0);
          vect_init(RO,j*(Angle/Nb)*AM,0,0);
        }

        if (NF==2) {
          vect_init(TR,0,0,-Rayon);
          vect_init(RO,0,-j*(Angle/Nb)*AM,0);
        }

        if (RI) rotation_objet(Mat,RA,'+');

        translation_objet(Mat,TR,'+');

        if (OkSelect) {
          T0[0]=Objet[0]->T[0]-TA[0];
          T0[1]=Objet[0]->T[1]-TA[1];
          T0[2]=Objet[0]->T[2]-TA[2];
          vect_init(TR,T0[0],-T0[1],T0[2]);
          translation_objet(Mat,TR,'-');
        }

        rotation_objet(Mat,RO,'+');

        if (OkSelect) {
          vect_init(TR,T0[0],-T0[1],T0[2]);
          translation_objet(Mat,TR,'+');
        }

        mat_decode(Mat->matrix,SC,SH,RO,TR);
        vect_init(Objet[NbObjet]->R,RO[0],RO[1],RO[2]);
        vect_init(Objet[NbObjet]->T,TR[0],-TR[1],TR[2]);
        Efface_Transform(Mat);

        Objet[NbObjet]->T[0]+=TA[0];
        Objet[NbObjet]->T[1]+=TA[1];
        Objet[NbObjet]->T[2]+=TA[2];

        if (NF==0) Objet[NbObjet]->T[1]+=Rayon;
        if (NF==1) Objet[NbObjet]->T[1]+=Rayon;
        if (NF==2) Objet[NbObjet]->T[2]+=Rayon;

        trace_volume_all(NbObjet,NbObjet);
      }
    }
  }
  affiche_donnees();
}

// -------------------------------------------------------------------------
// -- ALIGNEMENT D'OBJET FACE PAR FACE -------------------------------------
// -------------------------------------------------------------------------
void alignement(void) {
  int X1=CentX-130;
  int X2=CentX+130;
  int Y1=CentY-55;
  int Y2=CentY+55;
  int N1,N2;
  register int i,D,F;
  byte V=0xFF,H=0xFF;
  int XH,YH,XV,YV;
  register DBL MM1[2][2],MM2[2][2],X,Y;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  forme_mouse(MS_SELECTEUR);
  message("Select object to align");

  if (Selection && OkSelect==1) {
	if (cherche_fenetre()==FAUX) return;
    N1=cube_selection();
  } else {
    if ((N1=trouve_volume(0,2,1))==FAUX) return;
  }

  while(MouseB());

  forme_mouse(MS_SELECTEUR);
  message("Select object for alignment");

  if ((N2=trouve_volume(0,2,1))==FAUX) return;

  forme_mouse(MS_FLECHE);

  if (N1==N2) { f_erreur("Cannot be the same objects !"); return; }

  // ----------------------- Saisie du nombre d'objets & rayon

  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"objects alignment",AFFICHE);

  XV=-5; YV=11;
  relief(XV+X1+15,YV+Y1+15,XV+X1+80,YV+Y1+90,0);
  init_pastille(10,XV+X1+25,YV+Y1+25,"Top",V==0," ");
  init_pastille(11,XV+X1+25,YV+Y1+45,"Center",V==1," ");
  init_pastille(12,XV+X1+25,YV+Y1+65,"Bottom",V==2," ");

  XH=0; YH=11;
  relief(XH+X1+85,YH+Y1+15,XH+X1+250,YH+Y1+47,0);
  init_pastille(13,XH+X1+ 95,YH+Y1+25,"Left",H==0," ");
  init_pastille(14,XH+X1+148,YH+Y1+25,"Center",H==1," ");
  init_pastille(15,XH+X1+205,YH+Y1+25,"Right",H==2," ");

  affiche_pastille(10);
  affiche_pastille(11);
  affiche_pastille(12);
  affiche_pastille(13);
  affiche_pastille(14);
  affiche_pastille(15);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(10,12);
    test_groupe_pastille(13,15);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==1) return;

  V=quelle_pastille_dans_groupe(10,12)-10;
  H=quelle_pastille_dans_groupe(13,15)-13;

  // ----------------------- Cherche min-max objets

  reset_min_max();
  if (NF==0) cherche_volume_0(N1,1);
  if (NF==1) cherche_volume_1(N1,1);
  if (NF==2) cherche_volume_2(N1,1);

  MM1[_X][0]=Vid[NF].Min.X;
  MM1[_X][1]=Vid[NF].Max.X;

  MM1[_Y][0]=Vid[NF].Min.Y;
  MM1[_Y][1]=Vid[NF].Max.Y;

  reset_min_max();
  if (NF==0) cherche_volume_0(N2,1);
  if (NF==1) cherche_volume_1(N2,1);
  if (NF==2) cherche_volume_2(N2,1);

  MM2[_X][0]=Vid[NF].Min.X;
  MM2[_X][1]=Vid[NF].Max.X;

  MM2[_Y][0]=Vid[NF].Min.Y;
  MM2[_Y][1]=Vid[NF].Max.Y;

  // ----------------------- Proc‚dure de calcul

  Y=X=0.0;

  switch (V) {
    case 0:
      Y=-(MM1[_Y][1]-MM2[_Y][0]);
      break;
    case 1:
      Y=-((MM1[_Y][0]+MM1[_Y][1])-(MM2[_Y][0]+MM2[_Y][1]))/2;
      break;
    case 2:
      Y=-(MM1[_Y][0]-MM2[_Y][1]);
      break;
  }

  switch (H) {
    case 0:
      X=-(MM1[_X][1]-MM2[_X][0]);
      break;
    case 1:
      X=-((MM1[_X][0]+MM1[_X][1])-(MM2[_X][0]+MM2[_X][1]))/2;
      break;
    case 2:
      X=-(MM1[_X][0]-MM2[_X][1]);
      break;
  }

  if (OkSelect) {
    D=1;
    F=NbObjet;
  } else {
    D=F=N1;
  }

  switch(NF) {
    case 0:
      for (i=D;i<=F;i++) {
        if ((OkSelect && Objet[i]->Selection) || i==N1) {
          Objet[i]->T[_X]+=X;
          Objet[i]->T[_Y]+=Y;
        }
      }
      break;
    case 1:
      for (i=D;i<=F;i++) {
        if ((OkSelect && Objet[i]->Selection) || i==N1) {
          Objet[i]->T[_Z]+=X;
          Objet[i]->T[_Y]+=Y;
        }
      }
      break;
    case 2:
      for (i=D;i<=F;i++) {
        if ((OkSelect && Objet[i]->Selection) || i==N1) {
          Objet[i]->T[_X]+=X;
          Objet[i]->T[_Z]+=Y;
        }
      }
      break;
  }

  redessine_fenetre(5,1);
}
