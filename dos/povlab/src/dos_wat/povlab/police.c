/* ---------------------------------------------------------------------------
*  POLICE.C
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

#include <DIRECT.H>
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

int font_3d(int argc,char *argv[]);

char CheminTTF[MAXPATH];

// ----------------------------------------------------------------------------
// --------- GENERE UNE POLICE 3D SUR BASE TRUETYPE ---------------------------
// ----------------------------------------------------------------------------
void genere_police_3D(void) {
  int XA=CentX-110-70;
  int XB=CentX+110-70;
  int YA=CentY-120;
  int YB=CentY+120;
  FILE *Fichier;
  register int i;
  char Buffer[MAXPATH];
  char NomObjet[13]={NULLC};
  static char PoliceTTF[20]={"[FONT FILE NAME]"};
  static char Texte[30]={NULLC};
  static byte Cote=1,Face=1,Bevel=1;
  static byte MSTTF=1;
  static DBL LBevel=0.012;
  char *Spec[2]={"1","*.TTF"};

  forme_mouse(MS_FLECHE);
  g_fenetre(XA,YA,XB,YB,"3D FONTS MAKER",AFFICHE);

  // ----------------------- Saisie des paramŠtres

  LABEL_FENETRE_POLICE:
  
  strcpy(Objet[0]->Nom,"FONT");
  creer_nom_objet(0);
  strcpy(NomObjet,Objet[0]->Nom);

  init_bouton(55,XA+10,YA+25,200,20,PoliceTTF,CENTRE,ATTEND,"Choose a TTF font");
  strupr(Bt[55].Txt);
  init_texte(0,XA+110,YA+60,"Text string",Texte,10,"Enter text string");
  init_texte(1,XA+110,YA+80,"Object name",NomObjet,10,"Name for new object");

  init_case(10,XA+108,YA+105,"Depth faces",Cote,"Generate depth faces or not");
  init_case(11,XA+108,YA+125,"Front faces",Face,"Generate front faces or not");
  init_case(12,XA+108,YA+145,"Bevel edges",Bevel,"Bevel edges or not");
  sprintf(Buffer,"%f",LBevel);
  init_texte(2,XA+110,YA+165,"Bevel edge length",Buffer,10,"Bevel lenght for edges");

  init_case(13,XA+108,YA+190,"Microsoft",MSTTF,"Microsoft or macintosh typeface");

  place_zone_texte(0);
  place_zone_texte(1);
  place_zone_texte(2);
  affiche_case(10);
  affiche_case(11);
  affiche_case(12);
  affiche_case(13);
  affiche_bouton(55);

  bouton_dialog(XA,XB,YB,1,1);

  while (1) {
    test_texte(0,2);
    test_case(10,13);
    if ((i=bouton_dialog(XA,XB,YB,0,1))!=-1) break;
    if (test_bouton(55,55)==55) {
      set_disk(CheminTTF[0]-65);
      chdir(CheminTTF);
      strcpy(Buffer,selection_fichier(XB+5,YA,"Choose TTF",Spec));
      if (Buffer[0]!=27) {
        split_chemin(CheminTTF,Buffer,4);
        CheminTTF[strlen(CheminTTF)-1]=NULLC;
        split_chemin(PoliceTTF,Buffer,5);
        strcpy(Bt[55].Txt,PoliceTTF);
        affiche_bouton(55);
      }
      bouton_dialog(XA,XB,YB,1,1);
    }
  }

  if (i==0) {
    strcpy(Objet[0]->Nom,NomObjet);
    if (nom_objet_existe(NomObjet,0)) {
      f_erreur("That name already exist !");
      goto LABEL_FENETRE_POLICE;
    }

    Cote=Cc[10].Croix;
    Face=Cc[11].Croix;
    Bevel=Cc[12].Croix;
    LBevel=atof(ZTexte[2].Variable);
    if (LBevel==0.0 && Face && Cote) {
      f_erreur("Bevel edge lenght is 0.0 !|"\
               "Don't check the edges bevel case if you|"\
               "don't want it anymore");
      goto LABEL_FENETRE_POLICE;
    }

    if (PoliceTTF[0]=='[') {
      f_erreur("No font selected !|"\
               "Please select a truetype font or you|"\
               "will not be able to generate anything !");
      goto LABEL_FENETRE_POLICE;
    }

    strcpy(Texte,ZTexte[0].Variable);
    Texte[strinstr(0,Texte," ")]=NULLC;
    if (!strlen(Texte) || Texte[0]==NULLC) {
      f_erreur("No text string entered !|"\
               "Please enter a string text or you|"\
               "will not be able to generate anything !");
      goto LABEL_FENETRE_POLICE;
    }

    MSTTF=Cc[13].Croix;
  }

  g_fenetre(XA,YA,XB,YB,"",EFFACE);

  if (i==0) {
    Fichier=fopen("TRUETYPE.CMD","w+t");

    fprintf(Fichier,"Sortie=TRUETYPE.$$$\n"\
                    "TypeChanfrein=PLAT\n"\
                    "Profondeur=1.0\n"\
                    "FormatSortie=RAW\n"\
                    "Pr‚cision=4\n"\
                    "R‚solution=5\n"\
                    "TypeTriangle=FACETTE\n"\
                    "Police=%s\\%s\n"\
                    "Texte=%s\n"\
                    ,CheminTTF,PoliceTTF,Texte);

    fprintf(Fichier,"C“t‚s=%s\n",Cote ? "1":"0");
    
    if (!Face) {
      fprintf(Fichier,"Devant=0\n");
      fprintf(Fichier,"FaceAvant=0\n");
      fprintf(Fichier,"FaceArriŠre=0\n");
    } else {
      fprintf(Fichier,"Devant=1\n");
      fprintf(Fichier,"FaceAvant=1\n");
      fprintf(Fichier,"FaceArriŠre=1\n");
    }

    if (!Cote) {
      Cote=1;
      Face=1;
      Bevel=0;
    }
    
    if (Cote && Face) {
      fprintf(Fichier,"ChanfreinAvant=%s\n",Bevel ? "1":"0");
      fprintf(Fichier,"ChanfreinArriŠre=%s\n",Bevel ? "1":"0");
      fprintf(Fichier,"Chanfrein=%s\n",Bevel ? "1":"0");
      if (Bevel) {
        fprintf(Fichier,"TailleChanfrein1=%.4f\n",LBevel);
        fprintf(Fichier,"TailleChanfrein2=%.4f\n",LBevel);
        fprintf(Fichier,"TailleChanfrein3=%.4f\n",LBevel);
        fprintf(Fichier,"TailleChanfrein4=%.4f\n",LBevel);
      }
    }

    fprintf(Fichier,"FormatTT=%s\n",MSTTF ? "MS":"MC");
    
    fclose(Fichier);

    select_vue(5,CLIP_OFF);
    if (font_3d(1,NULL)) goto LABEL_FIN_POLICE_3D;

    if (!lecture_triangle_raw("TRUETYPE.$$$",1)) goto LABEL_FIN_POLICE_3D;
    strcpy(Objet[NbObjet]->CheminRaw,"@");
    strcpy(Objet[NbObjet]->Nom,NomObjet);

    LABEL_FIN_POLICE_3D:

    remove("TRUETYPE.$$$");
    remove("TRUETYPE.CMD");
  }
}
