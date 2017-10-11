/* ---------------------------------------------------------------------------
*  CSG.C
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
// -- EFFECTUE DES OPERATIONS DE CSG SUR LES OBJETS -------------------------
// --------------------------------------------------------------------------
byte csg_objet(byte T) {
  register int i;
  int O1,O2,C,S;

  if (pas_objet(1)) return 0;
  if (!test_si_selection_et_coche()) return 0;

  if (T==PAS_CSG) goto LABEL_PAS_CSG;

  forme_mouse(MS_SELECTEUR);

  if (T==DIFFERE) {
    message("Select object to substract with");
  } else {
    message("Select first object for boolean operation");
  }

  if ((O1=trouve_volume(0,3,1))==FAUX) return 0;

  LABEL_PAS_CSG:

  while (MouseB());
  forme_mouse(MS_SELECTEUR);

  if (T==DIFFERE) {
    message("Select object to substract from");
  } else {
    message("Select second object for boolean operation");
  }

  if (Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return 0;
    O2=cube_selection();
  } else {
    if ((O2=trouve_volume(0,3,1))==FAUX) return 0;
  }

  if (T!=PAS_CSG) {
    Objet[O1]->Operator=OK_CSG;
    Objet[O1]->CSG=PAS_CSG;
  }

  if (Selection && OkSelect==1) {
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Selection && !Objet[i]->Cache) {
        Objet[i]->Operator=T;
        if (T==PAS_CSG) {
          Objet[i]->CSG=PAS_CSG;
        } else {
          Objet[i]->CSG=O1;
        }
        trace_volume_all(i,i);
      }
    }
  } else {
    if (T==PAS_CSG) {
      Objet[O2]->Operator=PAS_CSG;
      Objet[O2]->CSG=PAS_CSG;
      trace_volume_all(O2,O2);
    } else {
      Objet[O2]->Operator=T;
      Objet[O2]->CSG=O1;
      C=Objet[O2]->Couleur;
      S=Objet[O2]->Selection;
      Objet[O2]->Couleur=FFOND;
      Objet[O2]->Selection=0;
      trace_volume_all(O2,O2);
      Objet[O2]->Couleur=C;
      Objet[O2]->Selection=S;
    }
  }

  forme_mouse(MS_FLECHE);
  return 1;
}

// -------------------------------------------------------------------------
// -- AFFICHE OU CACHE UN OBJET EN CSG -------------------------------------
// -------------------------------------------------------------------------
void affiche_csg(byte T) {
  register int N,i,C;

  if (pas_objet(1)) return;

  if (T==1) {
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Cache && Objet[i]->CSG!=PAS_CSG) {
        Objet[i]->Cache=0;
        trace_volume_all(i,i);
      }
    }
    return;
  }

  if (!test_si_selection_et_coche()) return;

  forme_mouse(MS_SELECTEUR);
  message("Select the CSG object to hide");

  if (Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return;
    N=cube_selection();
  } else {
    if ((N=trouve_volume(0,3,1))==FAUX) return;
  }

  while (MouseB());

  if (Selection && OkSelect==1) {
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Selection && !Objet[i]->Cache && Objet[i]->CSG!=PAS_CSG) {
        Objet[i]->Selection=0;
        C=Objet[i]->Couleur;
        Objet[i]->Couleur=FFOND;
        trace_volume_all(i,i);
        Objet[i]->Couleur=C;
        Objet[i]->Cache=1;
      }
    }
  } else {
    if (Objet[N]->CSG==PAS_CSG) return;
    Objet[N]->Selection=0;
    C=Objet[N]->Couleur;
    Objet[N]->Couleur=FFOND;
    trace_volume_all(N,N);
    Objet[N]->Couleur=C;
    Objet[N]->Cache=1;
  }

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- ANALYSE LA CSG ET RE-ORGANISE LES ASSIGNATIONS LECTURE PARTIELLE -----
// -------------------------------------------------------------------------
void analyse_csg_partiel(int N) {
  register int i,j;

  for (i=N+1;i<=NbObjet;i++) {
    if (Objet[i]->Operator==OK_CSG) {
      for (j=N+1;j<=NbObjet;j++) {
        if (Objet[i]->Buffer==-Objet[j]->Buffer) Objet[j]->CSG=i;
      }
    }
  }
}

// -------------------------------------------------------------------------
// -- VERIFY SI UN OBJET DONNE EST UN OBJET DECOUPE PAR CSG ----------------
// -------------------------------------------------------------------------
void analyse_delete_csg_objet(int N) {
  register int i;

  if (Objet[N]->CSG==OK_CSG) {
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Operator!=PAS_CSG && Objet[i]->CSG==N) {
        Objet[i]->Operator=Objet[i]->CSG=PAS_CSG;
        Objet[i]->Inverse=0;
      }
    }
  }

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Operator!=PAS_CSG && Objet[i]->CSG>N) {
      Objet[i]->CSG-=1;
    }
  }
}

// --------------------------------------------------------------------------
// -- INVERSE OPERATION DE CSG SUR OBJET ------------------------------------
// --------------------------------------------------------------------------
void inverse_csg(void) {
  register int i;
  int N;

  if (pas_objet(1)) return;
  if (!test_si_selection_et_coche()) return;

  LABEL_INVERSE_CSG:
  forme_mouse(MS_SELECTEUR);
  message("Select the object to inverse the CSG operation");

  if (Selection && OkSelect==1) {
    if (cherche_fenetre()==FAUX) return;
    N=cube_selection();
  } else {
    if ((N=trouve_volume(0,3,1))==FAUX) return;
  }

  if (Selection && OkSelect==1) {
    for (i=1;i<=NbObjet;i++) {
      if (Objet[i]->Selection && !Objet[i]->Cache) {
        Objet[i]->Inverse=!Objet[i]->Inverse;
      }
    }
  } else {
    Objet[N]->Inverse=!Objet[N]->Inverse;
  }

  goto LABEL_INVERSE_CSG;
}
