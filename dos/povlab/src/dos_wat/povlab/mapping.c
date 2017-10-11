/* ---------------------------------------------------------------------------
*  MAPPING.C
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
#include <STRING.H>
#include <IO.H>
#include <DOS.H>
#include <DIRECT.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

char Library[10][MAXPATH]={NULLC,NULLC,NULLC,NULLC,NULLC};

// --------------------------------------------------------------------------
// -- EXTRACTION DES COORDONNEES X ET Y D'UN MAPPING ------------------------
// --------------------------------------------------------------------------
void extract_xy_mapping(char *NomFichier,int *X,int *Y) {
  FILE *Image;
  int P1,P2,P3,P4;
  char *Buffer=(char *) malloc(256);
  int PX,PY;

  Image=fopen(NomFichier,"rb");
  fread(Buffer,256,1,Image);
  if (strinstr(0,NomFichier,".TGA")>0) { P1=12; P2=13; P3=14; P4=15; }
  if (strinstr(0,NomFichier,".GIF")>0) { P1= 6; P2= 7; P3= 8; P4= 9; }
  if (strinstr(0,NomFichier,".IFF")>0) { P1=21; P2=20; P3=23; P4=22; }
  if (strinstr(0,NomFichier,".PNG")>0) { P1=19; P2=18; P3=23; P4=22; }
  fclose(Image);

  PX=Buffer[P1]+Buffer[P2]*256;
  PY=Buffer[P3]+Buffer[P4]*256;

  *X=PX;
  *Y=PY;

  free((char *) Buffer);
}

// --------------------------------------------------------------------------
// -- PREPARE UN BOITE DE DIALOGUE POUR LES MAPPING/BUMPPING ----------------
// --------------------------------------------------------------------------
int set_mapping(int N) {
  int X1=CentX-180;
  int X2=CentX+180;
  int Y1=CentY-165;
  int Y2=CentY+165;
  int XA=X1,YA=Y1+20;
  int XB=XA+170;
  int i;
  struct Bouton Bt_Sv[2];
  char Buffer[MAXPATH];
  char *Spec[4]={"3","*.GIF","*.TGA","*.PNG"};

  if (pas_objet(1)) return 0;

  LABEL_SETUP_MAPPING:

  message("Pick an object");
  forme_mouse(MS_SELECTEUR);

  if ((N=trouve_volume(0,3,1))==FAUX) return 0;

  forme_mouse(MS_FLECHE);

  sprintf(Buffer,"Mapping/bumping: Object #%d",N);
  g_fenetre(X1,Y1,X2,Y2,Buffer,AFFICHE);

  for (i=30;i<30+2;i++) memcpy(&Bt_Sv[i-30],&Bt[i],sizeof(struct Bouton));

  // --------------- Image map

  relief(XA+10,YA+10,XB,Y2-38,0);

  init_case(11,XA+20,YA+ 20,"Image map on",Objet[N]->Map[0].On,"");
  init_case(12,XA+20,YA+ 35,"Image not tiled (once)",Objet[N]->Map[0].Once,"");

  init_pastille(11,XA+20,YA+ 55,"Planar mapping type",Objet[N]->Map[0].Type==0,"");
  init_pastille(12,XA+20,YA+ 70,"Spherical mapping type",Objet[N]->Map[0].Type==1,"");
  init_pastille(13,XA+20,YA+ 85,"Cylindrical mapping type",Objet[N]->Map[0].Type==2,"Use focal blur or not");
  init_pastille(14,XA+20,YA+100,"Torus mapping type",Objet[N]->Map[0].Type==3,"");

  init_pastille(15,XA+20,YA+120,"No interpolation",Objet[N]->Map[0].Interpolate==0,"");
  init_pastille(16,XA+20,YA+135,"Bi-linear interpolation",Objet[N]->Map[0].Interpolate==1,"");
  init_pastille(17,XA+20,YA+150,"Normalized distance",Objet[N]->Map[0].Interpolate==2,"");

  init_pastille(25,XA+20,YA+170,"No filter",Objet[N]->Map[0].Alpha==0,"");
  init_pastille(26,XA+20,YA+185,"Filter all",Objet[N]->Map[0].Alpha==1,"");
  init_pastille(27,XA+20,YA+200,"Filter color",Objet[N]->Map[0].Alpha==2,"");
  init_potar(1,XA+41,YA+231,42,0,1.0,(DBL) Objet[N]->Map[0].Filter,0.01,"Filter amount");
  sprintf(ZTexte[0].Variable,"%d",Objet[N]->Map[0].Color);
  init_texte(0,XA+90,YA+200,"",ZTexte[0].Variable,4,"# color entry in palette");
  init_bouton(30,XA+20,YA+246,120,20,Objet[N]->Map[0].Name,CENTRE,ATTEND,"Mapping file");
  if (!Objet[N]->Map[0].Name[0]) strcpy(Bt[30].Txt,"[no file]");

  // --------------- bump map

  relief(XB+10,YA+10,X2-10,Y2-38,0);

  init_case(13,XB+20,YA+ 20,"Bump map on",Objet[N]->Map[1].On,"");
  init_case(14,XB+20,YA+ 35,"Bump not tiled (once)",Objet[N]->Map[1].Once,"");

  init_pastille(18,XB+20,YA+ 55,"Planar mapping type",Objet[N]->Map[1].Type==0,"");
  init_pastille(19,XB+20,YA+ 70,"Spherical mapping type",Objet[N]->Map[1].Type==1,"");
  init_pastille(20,XB+20,YA+ 85,"Cylindrical mapping type",Objet[N]->Map[1].Type==2,"Use focal blur or not");
  init_pastille(21,XB+20,YA+100,"Torus mapping type",Objet[N]->Map[1].Type==3,"");

  init_pastille(22,XB+20,YA+120,"No interpolation",Objet[N]->Map[1].Interpolate==0,"");
  init_pastille(23,XB+20,YA+135,"Bi-linear interpolation",Objet[N]->Map[1].Interpolate==1,"");
  init_pastille(24,XB+20,YA+150,"Normalized distance",Objet[N]->Map[1].Interpolate==2,"");

  init_bouton(31,XB+20,YA+170,120,20,Objet[N]->Map[1].Name,CENTRE,ATTEND,"Bumping file");
  if (!Objet[N]->Map[1].Name[0]) strcpy(Bt[31].Txt,"[no file]");
  init_potar(0,XB+41,YA+205,52,-5,5,(DBL) Objet[N]->Map[1].Amount,0.01,"Bump size");

  for (i=11;i<=27;i++) affiche_pastille(i);
  for (i=11;i<=14;i++) affiche_case(i);
  for (i=30;i<=31;i++) affiche_bouton(i);
  for (i=0;i<=1;i++)   affiche_potar(i);
  place_zone_texte(0);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(11,14);
    test_groupe_pastille(15,17);
    test_groupe_pastille(18,21);
    test_groupe_pastille(22,24);
    test_groupe_pastille(25,27);
    test_case(11,14);
    test_potar(0,1);
    if (Pastille[27].Croix) test_texte(0,0);
    if (test_bouton(30,30)==30) {
      set_disk(Library[0][0]-65);
      chdir(Library[0]);
      strcpy(Buffer,selection_fichier(X2+5,YA,"Map file",Spec));
      if (Buffer[0]!=27) {
        split_chemin(Objet[N]->Map[0].Name,Buffer,5);
        strcpy(Bt[30].Txt,Objet[N]->Map[0].Name);
        affiche_bouton(30);
      }
      bouton_dialog(X1,X2,Y2,1,1);
    }
    if (test_bouton(31,31)==31) {
      set_disk(Library[0][0]-65);
      chdir(Library[0]);
      strcpy(Buffer,selection_fichier(X2+5,YA,"Bump file",Spec));
      if (Buffer[0]!=27) {
        split_chemin(Objet[N]->Map[1].Name,Buffer,5);
        strcpy(Bt[31].Txt,Objet[N]->Map[1].Name);
        affiche_bouton(31);
      }
      bouton_dialog(X1,X2,Y2,1,1);
    }

    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  if (i==0) {
    Objet[N]->Map[0].On=Cc[11].Croix;
    Objet[N]->Map[0].Once=Cc[12].Croix;
    Objet[N]->Map[0].Type=quelle_pastille_dans_groupe(11,14)-11;
    Objet[N]->Map[0].Interpolate=quelle_pastille_dans_groupe(15,17)-15;
    Objet[N]->Map[0].Alpha=quelle_pastille_dans_groupe(25,27)-25;
    Objet[N]->Map[0].Filter=Potar[1].Val;
    Objet[N]->Map[0].Color=atoi(ZTexte[0].Variable);

    Objet[N]->Map[1].On=Cc[13].Croix;
    Objet[N]->Map[1].Once=Cc[14].Croix;
    Objet[N]->Map[1].Type=quelle_pastille_dans_groupe(18,21)-18;
    Objet[N]->Map[1].Interpolate=quelle_pastille_dans_groupe(22,24)-22;
    Objet[N]->Map[1].Amount=Potar[0].Val;
  }

  for (i=30;i<30+2;i++) memcpy(&Bt[i-30],&Bt_Sv[i],sizeof(struct Bouton));
  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) goto LABEL_SETUP_MAPPING;
  forme_mouse(MS_FLECHE);
  return(!i);
}

// --------------------------------------------------------------------------
// -- ECRIT LE FICHIER DE MAPPING -------------------------------------------
// --------------------------------------------------------------------------
void ecrit_mapping(FILE *Fichier,int L,int N) {
  if (!Objet[N]->Map[0].On) return;
  strupr(Objet[N]->Map[0].Name);
  outl(Fichier,L,"pigment {\n");
  outl(Fichier,L+1,"image_map {\n");
  if (strinstr(0,Objet[N]->Map[0].Name,".TGA")>0) outl(Fichier,L+2,"tga\n");
  if (strinstr(0,Objet[N]->Map[0].Name,".GIF")>0) outl(Fichier,L+2,"gif\n");
  if (strinstr(0,Objet[N]->Map[0].Name,".PNG")>0) outl(Fichier,L+2,"png\n");
  outl(Fichier,L+2,"\"%s\"\n",Objet[N]->Map[0].Name);
  if (Objet[N]->Map[0].Type==0) outl(Fichier,L+2,"map_type 0\n");
  if (Objet[N]->Map[0].Type==1) outl(Fichier,L+2,"map_type 1\n");
  if (Objet[N]->Map[0].Type==2) outl(Fichier,L+2,"map_type 2\n");
  if (Objet[N]->Map[0].Type==3) outl(Fichier,L+2,"map_type 5\n");
  if (Objet[N]->Map[0].Once==1) outl(Fichier,L+2,"once\n");
  if (Objet[N]->Map[0].Interpolate==1) outl(Fichier,L+2,"interpolate 2\n");
  if (Objet[N]->Map[0].Interpolate==2) outl(Fichier,L+2,"interpolate 4\n");
  if (Objet[N]->Map[0].Alpha==1) {
    outl(Fichier,L+2,"filter all %.4g\n",Objet[N]->Map[0].Filter);
  }
  if (Objet[N]->Map[0].Alpha==2) {
    outl(Fichier,L+2,"filter %d %.4g\n",Objet[N]->Map[0].Color,
                                        Objet[N]->Map[0].Filter);
  }
  outl(Fichier,L+1,"}\n");
  if (Objet[N]->Map[0].Type!=1 && Objet[N]->Map[0].Type!=5) {
    outl(Fichier,L+1,"translate <%s,-.5,0>\n",Objet[N]->Map[0].Type==2 ? "0":"-0.5");
    outl(Fichier,L+1,"scale <%s,2,1>\n",Objet[N]->Map[0].Type==2 ? "1":"2");
  }
  outl(Fichier,L,"}\n");
}

// --------------------------------------------------------------------------
// -- ECRIT LE FICHIER BUMPING ----------------------------------------------
// --------------------------------------------------------------------------
void ecrit_bumping(FILE *Fichier,int L,int N) {
  if (!Objet[N]->Map[1].On) return;
  strupr(Objet[N]->Map[1].Name);
  outl(Fichier,L,"normal {\n");
  outl(Fichier,L+1,"bump_map {\n");
  if (strinstr(0,Objet[N]->Map[1].Name,".TGA")>0) outl(Fichier,L+2,"tga\n");
  if (strinstr(0,Objet[N]->Map[1].Name,".GIF")>0) outl(Fichier,L+2,"gif\n");
  if (strinstr(0,Objet[N]->Map[1].Name,".PNG")>0) outl(Fichier,L+2,"png\n");
  outl(Fichier,L+2,"\"%s\"\n",Objet[N]->Map[1].Name);
  if (Objet[N]->Map[1].Type==0) outl(Fichier,L+2,"map_type 0\n");
  if (Objet[N]->Map[1].Type==1) outl(Fichier,L+2,"map_type 1\n");
  if (Objet[N]->Map[1].Type==2) outl(Fichier,L+2,"map_type 2\n");
  if (Objet[N]->Map[1].Type==3) outl(Fichier,L+2,"map_type 5\n");
  if (Objet[N]->Map[1].Once==1) outl(Fichier,L+2,"once\n");
  if (Objet[N]->Map[1].Interpolate==1) outl(Fichier,L+2,"interpolate 2\n");
  if (Objet[N]->Map[1].Interpolate==2) outl(Fichier,L+2,"interpolate 4\n");
  outl(Fichier,L+2,"bump_size %.4g\n",(DBL) Objet[N]->Map[1].Amount);
  outl(Fichier,L+1,"}\n");
  if (Objet[N]->Map[0].Type!=1 && Objet[N]->Map[0].Type!=5) {
    outl(Fichier,L+1,"translate <%s,-.5,0>\n",Objet[N]->Map[0].Type==2 ? "0":"-0.5");
    outl(Fichier,L+1,"scale <%s,2,1>\n",Objet[N]->Map[0].Type==2 ? "1":"2");
  }
  outl(Fichier,L,"}\n");
}

// -------------------------------------------------------------------------
// -------- LIBRAIRIES DE MAPPING ------------------------------------------
// -------------------------------------------------------------------------
void library_mapping(void) {
  register int i;
  int X1=CentX-150;
  int X2=CentX+150;
  int Y1=CentY-130;
  int Y2=CentY+135;

  efface_buffer_clavier();
  message("Enter up to libraries where mapping can be founded");

  g_fenetre(X1,Y1,X2,Y2,"LIBRARIES",AFFICHE);

  bouton_dialog(X1,X2,Y2,1,1);

  init_texte(0,X1+65,Y1+ 30,"Library #01",Library[0],25,"Path to library #1");
  place_zone_texte(0);
  init_texte(1,X1+65,Y1+ 50,"Library #02",Library[1],25,"Path to library #2");
  place_zone_texte(1);
  init_texte(2,X1+65,Y1+ 70,"Library #03",Library[2],25,"Path to library #3");
  place_zone_texte(2);
  init_texte(3,X1+65,Y1+ 90,"Library #04",Library[3],25,"Path to library #4");
  place_zone_texte(3);
  init_texte(4,X1+65,Y1+110,"Library #05",Library[4],25,"Path to library #5");
  place_zone_texte(4);
  init_texte(5,X1+65,Y1+130,"Library #06",Library[5],25,"Path to library #6");
  place_zone_texte(5);
  init_texte(6,X1+65,Y1+150,"Library #07",Library[6],25,"Path to library #7");
  place_zone_texte(6);
  init_texte(7,X1+65,Y1+170,"Library #08",Library[7],25,"Path to library #8");
  place_zone_texte(7);
  init_texte(8,X1+65,Y1+190,"Library #09",Library[8],25,"Path to library #9");
  place_zone_texte(8);
  init_texte(9,X1+65,Y1+210,"Library #10",Library[9],25,"Path to library #10");
  place_zone_texte(9);


  while (1) {
    i=test_texte(0,9);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,NULL,EFFACE);

  if (i==0) {
    for (i=0;i<10;i++) strcpy(Library[i],ZTexte[i].Variable);
  }
}

// --------------------------------------------------------------------------
// -- VERIFIE SI TOUS LES FICHIERS MAPPING EXISTENT -------------------------
// --------------------------------------------------------------------------
byte verifie_mapping(void) {
  register int i,l;
  char *Buffer=(char *) malloc(256);
  byte Ok;

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Map[0].On) {
      Ok=0;
      for (l=0;l<10;l++) {
        sprintf(Buffer,"%s\\%s",Library[l],Objet[i]->Map[0].Name);
        if (Ok=test_fichier(Buffer)) break;
      }
      if (Ok==0) {
        f_erreur("The mapping path for object \"%s\"|"\
                 "isn't valid. Verify the libraries !",Objet[i]->Nom);
        free(Buffer);
        return 0;
      }
    }
  }

  for (i=1;i<=NbObjet;i++) {
    if (Objet[i]->Map[1].On) {
      Ok=0;
      for (l=0;l<10;l++) {
        sprintf(Buffer,"%s\\%s",Library[l],Objet[i]->Map[1].Name);
        if (Ok=test_fichier(Buffer)) break;
      }
      if (Ok==0) {
        f_erreur("The bump mapping path for object \"%s\"|"\
                 "isn't valid. Verify the libraries !",Objet[i]->Nom);
        free(Buffer);
        return 0;
      }
    }
  }

  free(Buffer);
  return 1;
}
