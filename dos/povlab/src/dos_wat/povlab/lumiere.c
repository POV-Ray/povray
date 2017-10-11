/* ---------------------------------------------------------------------------
*  LUMIERE.C
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
#include <DOS.H>
#include <STRING.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"

// -------------------------------------------------------------------------
// -- TEST SI CLICK SUR UN SPOT, UNE OMNI OU UNE AREA ----------------------
// -------------------------------------------------------------------------
CibleOeil trouve_lumiere(void) {
  CibleOeil Valeur;

  Valeur.Num=0;
  Valeur.Type=0;

  message("Please, select a spotlight, an omnilight or an arealight");

  while (!sequence_sortie()) {
    if (cherche_fenetre()==FAUX) break;
  
    if (NbArea) Valeur=selection_area();
    if (Valeur.Num) break;
    if (NbSpot) Valeur=selection_spot();
    if (Valeur.Num) break;
    if (NbOmni) Valeur=selection_omni();
    if (Valeur.Num) break;
    if (NbCylLight) Valeur=selection_cyllight();
    if (Valeur.Num) break;
  }

  forme_mouse(MS_FLECHE);
  return Valeur;
}

// -------------------------------------------------------------------------
// -- EXECUTE MODIF SUR SPOT/OMNI/AREA -------------------------------------
// -- Travail==0 DEPLACE                                                  --
// -- Travail==1 TAILLE                                                   --
// -- Travail==2 COULEUR                                                  --
// -- Travail==3 SUPPRIME                                                 --
// -- Travail==4 ANGLE EXTERNE SPOT                                       --
// -- Travail==5 ANGLE INTERNE SPOT                                       --
// -- Travail==6 RAYON AREA                                               --
// -- Travail==7 DOUCEUR AREA                                             --
// -- Travail==8 ON-OFF LUMIERE                                           --
// -- Travail==9 OMBRE LUMIERE                                            --
// -------------------------------------------------------------------------
int travail_lumiere(byte Travail) {
  int Num,Type,Ok=1;
  CibleOeil Valeur;

  if (pas_lumiere(0)) return 0;

  LABEL_LUMIERE:
  while (MouseB());

  if (Travail==0) forme_mouse(Sens); else forme_mouse(MS_SELECTEUR);

  Valeur=trouve_lumiere();

  Num=Valeur.Num;
  Type=Valeur.Type;

  if (!Num || !Type) { Ok=0; goto LABEL_FIN_LUMIERE; }

  if (Type==OMNI) {
    switch(Travail) {
      case 0: deplace_omni(Num);  break;
      case 1: taille_omni(Num);   break;
      case 2: couleur_omni(Num);  break;
      case 3: supprime_omni(Num); break;
      case 8: on_off_omni(Num);   break;
      case 9: ombre_omni(Num);    break;
    }
  }

  if (Type==SOEIL || Type==SCIBLE) {
    switch(Travail) {
      case 0: deplace_spot(Type,Num); break;
      case 1: taille_spot(Num);    break;
      case 2: couleur_spot(Num);   break;
      case 3: supprime_spot(Num);  break;
      case 4: angle_spot(Num,1);   break;
      case 5: angle_spot(Num,2);   break;
      case 8: on_off_spot(Num);    break;
      case 9: ombre_spot(Num);     break;
    }
  }

  if (Type==COEIL || Type==CCIBLE) {
    switch(Travail) {
      case 0: deplace_cyllight(Type,Num); break;
      case 1: taille_cyllight(Num);    break;
      case 2: couleur_cyllight(Num);   break;
      case 3: supprime_cyllight(Num);  break;
      case 4: angle_cyllight(Num,1);   break;
      case 5: angle_cyllight(Num,2);   break;
      case 8: on_off_cyllight(Num);    break;
      case 9: ombre_cyllight(Num);     break;
    }
  }

  if (Type==AREA) {
    switch(Travail) {
      case 0: deplace_area(Num);  break;
      case 1: taille_area(Num);   break;
      case 2: couleur_area(Num);  break;
      case 3: supprime_area(Num); break;
      case 6: rayon_area(Num);    break;
      case 8: on_off_area(Num);   break;
      case 9: ombre_area(Num);    break;
    }
  }

  goto LABEL_LUMIERE;

  LABEL_FIN_LUMIERE:

  forme_mouse(MS_FLECHE);
  GMouseOn();
  return Ok;
}

// -------------------------------------------------------------------------
// -- RENVOI EIN BETIT MEZZAGE D'ERR“R ZI PAS DES LOUMIERES ! --------------
// -------------------------------------------------------------------------
byte pas_lumiere(byte Son) {
  if (NbArea==0 && NbOmni==0 && NbSpot==0 && NbCylLight==0) {
    if (Son) beep_erreur();
    message("There's no light source in the scene ! Unavailable function...");
    return VRAI;
  } else {
    return FAUX;
  }
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP D'UNE LUMIERE -------------------------------------
// -------------------------------------------------------------------------
void setup_lumiere(void) {
  int Num,Type;
  CibleOeil Valeur;

  LABEL_SETUP_LUMIERE:

  message("Pick a light source");
  forme_mouse(MS_SELECTEUR);

  if (cherche_fenetre()==FAUX) return;
  Valeur=trouve_lumiere();
  if (Valeur.Num==0) goto LABEL_SETUP_LUMIERE;
  forme_mouse(MS_FLECHE);

  Num=Valeur.Num;
  Type=Valeur.Type;

  if (Type==AREA) { setup_area(Num); return; }
  if (Type==SOEIL || Type==SCIBLE) { setup_spot(Num); return; }
  if (Type==COEIL || Type==CCIBLE) { setup_cyllight(Num); return; }
  if (Type==OMNI) { setup_omni(Num); return; }
}


