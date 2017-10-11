/* ---------------------------------------------------------------------------
*  MODELEUR.C
*
*  POVLAB project started AUGUST, 1994.
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
#include <STDIO.H>
#include <CONIO.H>
#include <DOS.H>
#include <FLOAT.H>
#include <MATH.H>
#include <STDLIB.H>
#include <SIGNAL.H>
#include "LIB.H"
#include "GLOBAL.H"
#include "GLIB.H"


#if WINDOWS
#include "WINDOWS.C"
#else
// ---------------------------------------------------------------------------
// ---------------------- PROGRAMME PRINCIPAL --------------------------------
// ---------------------------------------------------------------------------

unsigned short __8087cw = IC_AFFINE | RC_NEAR | PC_64 | 0x007F;

int main(int argc, char *argv[]) {               /* -- programme principal -- */
  int i;
  
  init_soft(LOGICIEL,VERSION);

  _fpreset();
  _control87(_PC_64,MCW_PC);
  init_arg(argc,argv,1);
  init_disk(Arg[0]);
  lecture_config_interface();
  analyse_arguments();
  if (EXEC1) printf("  - Initializing...\n");
  palette_noire();
  init_gmode(0);
  init_police(1,0);
  MemoireLibre=init_taille_memoire();

  if (alloc_mem_objet(0)==1) {
    supprime_ctrl_del_break();
    if (EXEC1) show_logo_logiciel();
    palette_noire();
    sauve_config_interface(0);
    init_fog();
    init_video_structure();
    lecture_fichier(NULL,0,0,1);
    charge_motif(SAUVE);
    interface(1);
    _harderr(critical_error_handler);
      while (!CheminRAYTRACER[0] ||
          !NomUtilisateur[0] ||
          !CheminMTEX[0]) if (!fenetre_preferences()) goto LABEL_FIN_POVLAB;
    lecture_fichier(NULL,0,0,0);
    place_mouse(Vid[0].WX/2,Vid[0].WY/2);
    trouve_fenetre(1);
    place_mouse(XMax/2,YMax/2);
    affiche_donnees();
    if (EXEC1) a_propos_de();
    place_mouse(CentX/2,0);
    ComputeBernstein();
    do {
      choix_principal();
    } while (sauve_scene_en_quittant()==0);

    LABEL_FIN_POVLAB:

    GMouseOff();
    if (NbObjet) log_out(0,"%d objects allocated -> %lu",NbObjet,(long) NbObjet*sizeof(VOLUME));
    if (NbPoly>-1) log_out(1,"%d polygonals allocated -> %lu",NbPoly,(long) NbPoly*sizeof(POLYGONE));
    if (NbSpline>-1) log_out(2,"%d b-splines allocated -> %lu",NbSpline,(long) NbSpline*sizeof(B_SPLINE));

    for (i=0;i<=NbObjet;i++) free_mem_objet(i);
    for (i=0;i<=NbPoly;i++) free_mem_poly(i);
    for (i=0;i<=NbSpline;i++) free_mem_spline(i);

    sauve_config_interface(1);
    fading_palette(FADEIN);
    charge_motif(EFFACE);
  }

  // sauve_temporaire();
  fin_mode_graphique();
  ecran_off(1);
  text_mode();
  ecran_off(0);
  return (150);
}
#endif

