/* ---------------------------------------------------------------------------
*  SELECT.C
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

int Selection=0;                      // Si s‚lection(s) en cours
byte OkSelect=0;                      // Utilise ou non la s‚lection

// -------------------------------------------------------------------------
// -- PAS DE SELECTION FONCTION INUTILISABLE -------------------------------
// -------------------------------------------------------------------------
byte test_si_selection_et_coche(void) {
  if (OkSelect==1) {
    if (!Selection) {
      f_erreur("No object are selected|and the selection case is checked !");
      return 0;
    }
  }
  return 1;
}

// -------------------------------------------------------------------------
// -- SELECTIONNE UN OBJET -------------------------------------------------
// -------------------------------------------------------------------------
// Type=0 Aucune
// Type=1 Tout
// Type=2 objet
// Type=3 Inverse
// Type=4 Dernier
// Type=5 Par nom
// -------------------------------------------------------------------------
void selection_objet(byte Type) {
  register int i;

  if (pas_objet(1)) return;

  switch (Type) {
    case 0:
      for (i=1;i<=NbObjet;i++) {
        if (Objet[i]->Selection && !Objet[i]->Cache) {
          Objet[i]->Selection=0;
          trace_volume_all(i,i);
        }
        Objet[i]->Selection=0;
      }
      Selection=0;
      affiche_donnees();
      break;

    case 1:
      Selection=0;
      for (i=1;i<=NbObjet;i++) {
        if (!Objet[i]->Cache && !Objet[i]->Freeze) {
          if (sequence_sortie()) break;
          Objet[i]->Selection=1;
          Selection++;
        }
      }
      redessine_fenetre(5,0);
      affiche_donnees();
      break;

    case 3:
      Selection=0;
      for (i=1;i<=NbObjet;i++) {
        if (!Objet[i]->Cache) {
          Objet[i]->Selection=(Objet[i]->Selection==1 ? 0:1);
          Selection+=(Objet[i]->Selection==0 ? 0:1);
        }
      }
      redessine_fenetre(5,0);
      affiche_donnees();
      break;

    case 4:
      Selection+=(Objet[NbObjet]->Selection==0 ? 1:-1);
      Objet[NbObjet]->Selection=(Objet[NbObjet]->Selection==1 ? 0:1);
      affiche_donnees();
      trace_volume_all(NbObjet,NbObjet);
      break;

    case 5:
      Selection=0;

      for (i=1;i<=NbObjet;i++) {
        Objet[i]->Buffer=0;
        if (Objet[i]->Selection==0 && Objet[i]->Cache==0) Objet[i]->Buffer=1;
      }

      if (choix_nom_objet(150,60)) {
        for (i=1;i<=NbObjet;i++) {
          if (!Objet[i]->Cache) {
            if (Objet[i]->Buffer) {
              Objet[i]->Selection=1; Selection++;
              trace_volume_all(i,i);
            } else {
              Objet[i]->Selection=0;
            }
          }
        }
        affiche_donnees();
      }
      break;

    case 2:
      LABEL_SELECTION:
      forme_mouse(MS_SELECTEUR);
      message("Choose the object to select");

      if ((i=trouve_volume(0,3,1))==FAUX) break;

      if (Objet[i]->Selection) {
        Objet[i]->Selection=0;
        Selection--;
      } else {
        Objet[i]->Selection=1;
        Selection++;
      }

      utilise_selection(1);
      affiche_donnees();
      message_termine(0,"");
      trace_volume_all(i,i);
      while (MouseB());
      forme_mouse(MS_FLECHE);
      GMouseOn();
      goto LABEL_SELECTION;
  }
  utilise_selection(1);
}

// -------------------------------------------------------------------------
// -- CONSTRUIT LE CUBE DE SELECTION LORS MODIF ----------------------------
// -------------------------------------------------------------------------
int cube_selection(void) {
  register int i;

  NumObjet=0;
  reset_min_max();

  Objet[0]->Type=CUBE_C;
  modif_objet(0,1,1,1,SCALE);
  modif_objet(0,0.0,0.0,0.0,ROTATE);
  modif_objet(0,0.0,0.0,0.0,TRANSLATE);
  modif_objet(0,0.0,0.0,0.0,DIVERS);
  strcpy(Objet[0]->Nom,"S‚lection");

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Selection && !Objet[i]->Cache) {
      cherche_volume_0(i,1);
      cherche_volume_1(i,1);
    }
  }

  Objet[0]->S[0]=(Vid[0].Max.X-Vid[0].Min.X)/2.0;
  Objet[0]->S[1]=(Vid[0].Max.Y-Vid[0].Min.Y)/2.0;
  Objet[0]->S[2]=(Vid[1].Max.X-Vid[1].Min.X)/2.0;

  if (Objet[0]->S[0]==0.0) Objet[0]->S[0]=1.0;
  if (Objet[0]->S[1]==0.0) Objet[0]->S[1]=1.0;
  if (Objet[0]->S[2]==0.0) Objet[0]->S[2]=1.0;

  Objet[0]->T[0]=(Vid[0].Max.X+Vid[0].Min.X)/2.0;
  Objet[0]->T[1]=(Vid[0].Max.Y+Vid[0].Min.Y)/2.0;
  Objet[0]->T[2]=(Vid[1].Max.X+Vid[1].Min.X)/2.0;

  return 0;
}

// -------------------------------------------------------------------------
// -- MODIFICATION DES OBJETS SELECTIONNES ---------------------------------
// -------------------------------------------------------------------------
void selection_modif_objet(byte Modif) {
  register int i;
  DBL SX,SY,SZ,TX,TY,TZ;
  DBL X,Y,Z,RX,RY,RZ;
  Vecteur SC,SH,RO,TR;
  TRANSFORM *Mat;

  RX=Objet[0]->R[_X];
  RY=Objet[0]->R[_Y];
  RZ=Objet[0]->R[_Z];

  SX=fabs(Objet[0]->S[0]/(Vid[0].Max.X-Vid[0].Min.X))*2;
  SY=fabs(Objet[0]->S[1]/(Vid[0].Max.Y-Vid[0].Min.Y))*2;
  SZ=fabs(Objet[0]->S[2]/(Vid[1].Max.X-Vid[1].Min.X))*2;

  if (SX==0.0) SX=1.0;
  if (SY==0.0) SY=1.0;
  if (SZ==0.0) SZ=1.0;

  TX=Objet[0]->T[0]-((Vid[0].Max.X+Vid[0].Min.X)*0.5);
  TY=Objet[0]->T[1]-((Vid[0].Max.Y+Vid[0].Min.Y)*0.5);
  TZ=Objet[0]->T[2]-((Vid[1].Max.X+Vid[1].Min.X)*0.5);

  X=Objet[0]->T[_X];
  Y=Objet[0]->T[_Y];
  Z=Objet[0]->T[_Z];

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Selection && !Objet[i]->Cache) {
      if (Modif==SCALE) {
        Mat=Create_Transform();
        ajustement_objet(Mat,Objet[i]->S,'+');
        rotation_objet(Mat,Objet[i]->R,'+');

        vect_init(TR,Objet[i]->T[_X],-Objet[i]->T[_Y],Objet[i]->T[_Z]);
        translation_objet(Mat,TR,'+');

        vect_init(TR,X,-Y,Z);
        translation_objet(Mat,TR,'-');

        vect_init(SC,SX,SY,SZ);
        ajustement_objet(Mat,SC,'+');

        vect_init(TR,X,-Y,Z);
        translation_objet(Mat,TR,'+');

        mat_decode(Mat->matrix,SC,SH,RO,TR);
        vect_init(Objet[i]->T,TR[_X],-TR[_Y],TR[_Z]);
        vect_copy(Objet[i]->S,SC);
        vect_copy(Objet[i]->R,RO);
        Efface_Transform(Mat);
      }
      if (Modif==ROTATE) {
        Mat=Create_Transform();

        rotation_objet(Mat,Objet[i]->R,'+');

        vect_init(TR,Objet[i]->T[0],-Objet[i]->T[1],Objet[i]->T[2]);
        translation_objet(Mat,TR,'+');

        vect_init(TR,X,-Y,Z);
        translation_objet(Mat,TR,'-');

        rotation_objet(Mat,Objet[0]->R,'+');

        vect_init(TR,X,-Y,Z);
        translation_objet(Mat,TR,'+');

        mat_decode(Mat->matrix,SC,SH,RO,TR);
        vect_init(Objet[i]->T,TR[0],-TR[1],TR[2]);
        vect_copy(Objet[i]->R,RO);
        Efface_Transform(Mat);
      }
      if (Modif==TRANSLATE) {
        Objet[i]->T[0]+=TX;
        Objet[i]->T[1]+=TY;
        Objet[i]->T[2]+=TZ;
      }
      if (Modif==MIROIR) {
        Objet[i]->T[0]+=TX;
        Objet[i]->T[1]+=TY;
        Objet[i]->T[2]+=TZ;
        Mat=Create_Transform();

        rotation_objet(Mat,Objet[i]->R,'+');

        vect_init(TR,Objet[i]->T[0],-Objet[i]->T[1],Objet[i]->T[2]);
        translation_objet(Mat,TR,'+');

        vect_init(TR,X,-Y,Z);
        translation_objet(Mat,TR,'-');

        vect_init(RO,0,0,0);
        if (Sens==MS_X) {
          if (NF==0) RO[1]=180;
          if (NF==1) RO[1]=180;
          if (NF==2) RO[2]=180;
        }
        if (Sens==MS_Y) {
          if (NF==0) RO[0]=180;
          if (NF==1) RO[2]=180;
          if (NF==2) RO[0]=180;
        }
        if (Sens==MS_XY) {
          if (NF==0) RO[2]=180;
          if (NF==1) RO[0]=180;
          if (NF==2) RO[1]=180;
        }

        rotation_objet(Mat,RO,'+');

        vect_init(TR,X,-Y,Z);
        translation_objet(Mat,TR,'+');

        mat_decode(Mat->matrix,SC,SH,RO,TR);
        vect_init(Objet[i]->T,TR[0],-TR[1],TR[2]);
        vect_copy(Objet[i]->R,RO);
        Efface_Transform(Mat);
      }
    }
  }
}

// -------------------------------------------------------------------------
// -- CHANGE LA COULEUR DE LA SELECTION ------------------------------------
// -------------------------------------------------------------------------
void couleur_selection(void) {
  int Temp;

  Temp=affiche_palette();
  if (Temp!=-1) {
    CSELECTION=Temp;
    for (Temp=1;Temp<=NbObjet;Temp++) {
      if (Objet[Temp]->Selection && !Objet[Temp]->Cache) {
        trace_volume_all(Temp,Temp);
      }
    }
  }
}

// ----------------------------------------------------------------------------
// -- DEFINI UNE ZONE POUR SELECTION OBJET DS FENETRE -------------------------
// ----------------------------------------------------------------------------
int selection_zone (void) {
  DBL X,Y,XA,YA,XB,YB,PasX,PasY;
  DBL E;
  register int i;

  if (pas_objet(1)) return -1;

  LABEL_SELECTION_ZONE:

  forme_mouse(MS_FLECHE);
  if ((cherche_fenetre()==FAUX) || NF==3) return -1;

  while (MouseB());

  message("Select a selection area");

  select_vue(NF,CLIP_ON);
  E=Vid[NF].Echelle;
  GMouseOff();
  while (MouseB());

  X=PasX=-Vid[NF].Depla.X+(gmx_v()-Vid[NF].XF-Vid[NF].WX/2)/Vid[NF].Echelle;
  Y=PasY=-Vid[NF].Depla.Y+(gmy_v()-Vid[NF].YF-Vid[NF].WY/2)/Vid[NF].Echelle;
  
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
    XA=X;
    YA=Y;
    XB=PasX;
    YB=PasY;

    if (XB<XA) swap_dbl(&XA,&XB);
    if (YB<YA) swap_dbl(&YA,&YB);
    
    for (i=1;i<=NbObjet;i++) {
      if (NF==0 && Objet[i]->Freeze==0) {
        reset_min_max();
        cherche_volume_0(i,1);
        if (XA<=Vid[0].Min.X && XB>=Vid[0].Max.X && YA<=Vid[0].Min.Y && YB>=Vid[0].Max.Y) {
          Objet[i]->Selection=(Objet[i]->Selection==1 ? 0:1);
          Selection+=(Objet[i]->Selection==1 ? 1:-1);
          trace_volume_all(i,i);
        }
      }
      if (NF==1 && Objet[i]->Freeze==0) {
        reset_min_max();
        cherche_volume_1(i,1);
        if (Vid[1].Min.X>=XA && Vid[1].Max.X<=XB && Vid[1].Min.Y>=YA && Vid[1].Max.Y<=YB) {
          Objet[i]->Selection=(Objet[i]->Selection==1 ? 0:1);
          Selection+=(Objet[i]->Selection==1 ? 1:-1);
          trace_volume_all(i,i);
        }
      }
      if (NF==2 && Objet[i]->Freeze==0) {
        reset_min_max();
        cherche_volume_2(i,1);
        if (Vid[2].Min.X>=XA && Vid[2].Max.X<=XB && Vid[2].Min.Y>=YA && Vid[2].Max.Y<=YB) {
          Objet[i]->Selection=(Objet[i]->Selection==1 ? 0:1);
          Selection+=(Objet[i]->Selection==1 ? 1:-1);
          trace_volume_all(i,i);
        }
      }
    }
  }
  utilise_selection(1);
  affiche_donnees();
  while (MouseB());
  GMouseOn();
  if (MouseB()!=2) goto LABEL_SELECTION_ZONE;
  return -1;
}

// --------------------------------------------------------------------------
// ----- UTILISE LA SELECTION OUI OU NON ------------------------------------
// --------------------------------------------------------------------------
void utilise_selection(byte travail) {
  travail=travail;
  OkSelect=Cc[0].Croix;
}

// -------------------------------------------------------------------------
// -- SELECTION D'OBJET(S) PAR LEUR MATIERE --------------------------------
// -------------------------------------------------------------------------
byte selection_objet_par_matiere(byte Travail) {
  int LenChar=MAXPATH;
  int NbM=0;
  int X1=CentX-161;
  int Y1=CentY-125;
  int X2=CentX+161;
  int Y2=CentY+125;
  int i,j,Ok;
  struct retour_selecteur Val;

  Travail=Travail;
  if (pas_objet(1)) return 0;

  g_fenetre(X1,Y1,X2,Y2,"SELECTION BY TEXTURE",AFFICHE);
  bouton_dialog(X1,X2,Y2,1,1);

  for (i=1;i<=NbObjet;i++) {
    if (!Objet[i]->Cache && !Objet[i]->Freeze) {
      Ok=1;
      for (j=0;j<=NbM;j++) {
        if (!strcmp(NomMat[j],Objet[i]->Matiere)) { Ok=0; break; }
      }
      if (Ok) {
        NomMat[NbM]=(char *) mem_alloc(LenChar);
        strcpy(NomMat[NbM],Objet[i]->Matiere);
        NbM++;
      }
    }
  }

  NbM--;
  tri_tableau(NomMat,NbM+1,sizeof((char *) NomMat[0]));
  message("%d texture(s) found in the scene",NbM+1);

  init_selecteur(0,X1-5,Y1-7,12,NbM+1,NomMat,32);
  affiche_selecteur(0);

  while (1) {
    Val=test_selecteur(0);
    if (Val.Ok==27) { i=1; break; }
    if (Val.Ok==13) { i=0; break; }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) {
    for (i=1;i<=NbObjet;i++) {
      if (!Objet[i]->Cache && !Objet[i]->Freeze) {
        if (!strcmp(NomMat[Val.Num],Objet[i]->Matiere)) {
          Objet[i]->Selection=1;
          trace_volume_all(i,i);
          Selection++;
        }
      }
    }
  }

  for (i=0;i<NbM;i++) {
    mem_free(NomMat[i],LenChar);
  }

  affiche_donnees();

  return (!i);
}
