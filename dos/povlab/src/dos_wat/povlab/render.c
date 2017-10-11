/* ---------------------------------------------------------------------------
*  RENDER.C
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
#include <DOS.H>
#include <FLOAT.H>
#include <GRAPH.H>
#include <IO.H>
#include <MATH.H>
#include <PROCESS.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include "GLIB.H"
#include "GLOBAL.H"
#include "LIB.H"

byte DernierRendu=0;
byte Fond_RVB[3]={50,50,60};

DBL RZ[4]; // Rendu d'une zone (fenetrage)
char LastImage[MAXPATH];

byte Global_Ambient[3]={255,255,255};
DBL  Global_ADC=(DBL) 1/255;
DBL  Global_A_Gamma=2.2;
DBL  Global_Irid[3]={0.25,0.18,0.14};
int  Global_MaxTrace=4;
int  Global_MaxInter=64;
int  Global_NbWave=10;

byte Atmos_OnOff=0;
byte Atmos_Type=1;
DBL  Atmos_Dist=10;
DBL  Atmos_Scatt=0.1;
DBL  Atmos_Eccent=0;
byte Atmos_Samples=20;
DBL  Atmos_Jitter=0.25;
DBL  Atmos_aa_t=0.1;
byte Atmos_aa_l=8;
byte Atmos_RGB[3]={255,255,255};

DBL AntiAliasValue=0.3;
DBL JitterValue=1.0;
int AntiAliasDepth=2;

int ResolutionX=320;
int ResolutionY=240;

FOG Fog[5];

byte Rad_OnOff=0;
byte Rad_Type=3;
DBL  Rad_Brightness=3.3;
int  Rad_Count=200;
DBL  Rad_Dist_Max=10;
DBL  Rad_Err_Bound=0.3;
DBL  Rad_Gray_Threshold=0.5;
DBL  Rad_Low_Err_Fact=0.75;
DBL  Rad_Min_Reuse=0.017;
int  Rad_Near_Count=7;
int  Rad_Rec_Lim=1;

// ----------------------------------------------------------------------------
// -- LANCE LE RENDU D'UNE ZONE DE FENETRE ------------------------------------
// ----------------------------------------------------------------------------
byte rendu_zone(void) {
  DBL X,Y,XA,YA,XB,YB,PasX,PasY;
  DBL A,B;

  if (pas_objet(1)) return 0;
  if (!Fx4) {
    f_erreur("This function doesn't work in full screen mode !");
    return 0;
  }

  forme_mouse(MS_FLECHE);

  cherche_fenetre();

  if (NF==FAUX) return 0;
  if (NF!=3) {
    f_erreur("Please select the camera viewport !");
    return 0;
  }

  while (MouseB());
  message("Move the mouse to select a zone");
  GMouseOff();
  while (MouseB());

  if (ConfigRaytracer[15]) {
    A=((DBL) ResolutionX/Vid[NF].WX);
    B=((DBL) ResolutionY/Vid[NF].WY);
  }
  if (ConfigRaytracer[16]) { A=((DBL) 128/Vid[NF].WX); B=((DBL) 96/Vid[NF].WY); }
  if (ConfigRaytracer[17]) { A=((DBL) 320/Vid[NF].WX); B=((DBL)200/Vid[NF].WY); }
  if (ConfigRaytracer[18]) { A=((DBL) 320/Vid[NF].WX); B=((DBL)240/Vid[NF].WY); }
  if (ConfigRaytracer[19]) { A=((DBL) 640/Vid[NF].WX); B=((DBL)480/Vid[NF].WY); }
  if (ConfigRaytracer[20]) { A=((DBL) 800/Vid[NF].WX); B=((DBL)600/Vid[NF].WY); }
  if (ConfigRaytracer[21]) { A=((DBL)1024/Vid[NF].WX); B=((DBL)768/Vid[NF].WY); }

  X=PasX=(gmx_v()-Vid[NF].XF)-Vid[NF].WXs2;
  Y=PasY=(gmy_v()-Vid[NF].YF)-Vid[NF].WYs2;
  Vid[NF].Echelle=1.0;
  
  trace_rectangle(X,Y,PasX,PasY,0,2,0);
  place_mouse(CentX,CentY);
  XB=gmx_r();
  YB=gmy_r();

  delay(100);
  
  while (MouseB()!=1) {
    XA=gmx_r();
    YA=gmy_r();
    if (sequence_sortie()) return 0;
    if (XA!=XB || YA!=YB) {
      trace_rectangle(X,Y,PasX,PasY,0,2,0);
      PasX+=(DBL) (XA-XB);
      PasY+=(DBL) (YA-YB);
      trace_rectangle(X,Y,PasX,PasY,0,2,0);
      place_mouse(CentX,CentY);
      XB=gmx_r();
      YB=gmy_r();
      XA=(PasX+Vid[NF].WXs2)*A;
      YA=(PasY+Vid[NF].WYs2)*B;
      message("X:%d Y:%d",(int)XA,(int)YA);
    }
  }

  trace_rectangle(X,Y,PasX,PasY,0,2,0);

  XA=(X+Vid[NF].WXs2)*A;
  YA=(Y+Vid[NF].WYs2)*B;
  XB=(PasX+Vid[NF].WXs2)*A;
  YB=(PasY+Vid[NF].WYs2)*B;
  
  if (XA>XB) swap_dbl(&XA,&XB);
  if (YA>YB) swap_dbl(&YA,&YB);

  if (ConfigRaytracer[15]) {
    XA/= ResolutionX;
    XB/= ResolutionX;
    YA/= ResolutionY;
    YB/= ResolutionY;
  }
  if (ConfigRaytracer[16]) { XA/= 128; XB/= 128; YA/= 96; YB/= 96; }
  if (ConfigRaytracer[17]) { XA/= 320; XB/= 320; YA/=200; YB/=200; }
  if (ConfigRaytracer[18]) { XA/= 320; XB/= 320; YA/=240; YB/=240; }
  if (ConfigRaytracer[19]) { XA/= 640; XB/= 640; YA/=480; YB/=480; }
  if (ConfigRaytracer[20]) { XA/= 800; XB/= 800; YA/=600; YB/=600; }
  if (ConfigRaytracer[21]) { XA/=1024; XB/=1024; YA/=768; YB/=768; }

  RZ[0]=XA<0 ? 0:XA;
  RZ[1]=YA<0 ? 0:YA;
  RZ[2]=XB<0 ? 0:XB;
  RZ[3]=YB<0 ? 0:YB;

  RZ[0]=XA>=1 ? 0.999:XA;
  RZ[1]=YA>=1 ? 0.999:YA;
  RZ[2]=XB>=1 ? 0.999:XB;
  RZ[3]=YB>=1 ? 0.999:YB;

  while (MouseB());
  GMouseOn();
  return 1;
}

// --------------------------------------------------------------------------
// ----- AFFICHE UNE BOITE DE DIALOGUE POUR PARAMETRES RAYTRACER ------------
// --------------------------------------------------------------------------
byte dialog_povray(void) {
  register int i,j,RadType,ModeMouse,Y=7;
  register int X1,Y1,X2,Y2;
  int XA,YA,XB,YB;

  ConfigRaytracer[15]=0; // autres formats
  ConfigRaytracer[20]=0; // 800x600
  ConfigRaytracer[21]=0; // 1024x768
  ConfigRaytracer[ 3]=0; // Continue

  X1=CentX-240;
  X2=CentX+240;
  Y1=CentY-155;
  Y2=CentY+160;

  ModeMouse=BitMouse;

  efface_buffer_clavier();

  strcpy(StrBoite[0],"RENDERING PARAMETERS FOR POV-RAY");
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);
  i=bouton_dialog(X1,X2,Y2,1,1);

  init_case(10,X1+ 10,Y1+Y+ 20,"Buffer enabled",ConfigRaytracer[0],"Image buffer fixed to 128 Kb");
  affiche_case(10);
  init_case(11,X1+ 10,Y1+Y+ 35,"Continue image",ConfigRaytracer[3],"Continue abordted trace");
  affiche_case(11);
  init_case(12,X1+ 10,Y1+Y+ 50,"No graphic mode",ConfigRaytracer[4],"Just display number lines");
  affiche_case(12);
  init_case(13,X1+ 10,Y1+Y+ 65,"64 levels of gray",ConfigRaytracer[5],"Graphic in gray levels");
  affiche_case(13);
  init_case(14,X1+ 10,Y1+Y+ 80,"Ascii Report",ConfigRaytracer[6],"Generate a ascii report");
  affiche_case(14);

  init_pastille(15,X1+170,Y1+Y+ 20,"Resolution 128x96",ConfigRaytracer[16],"Render in 128x96 pixels");
  affiche_pastille(15);
  init_pastille(16,X1+170,Y1+Y+ 35,"Resolution 320x200",ConfigRaytracer[17],"Render in 320x200 pixels");
  affiche_pastille(16);
  init_pastille(17,X1+170,Y1+Y+ 50,"Resolution 320x240",ConfigRaytracer[18],"Render in 320x240 pixels");
  affiche_pastille(17);
  init_pastille(18,X1+170,Y1+Y+ 65,"Resolution 640x480",ConfigRaytracer[19],"Render in 640x480 pixels");
  affiche_pastille(18);
  init_pastille(19,X1+170,Y1+Y+ 80,"Resolution 800x600",ConfigRaytracer[20],"Render in 800x600 pixels");
  affiche_pastille(19);
  init_pastille(20,X1+170,Y1+Y+ 95,"Resolution 1024x768",ConfigRaytracer[21],"Render in 1024x768 pixels");
  affiche_pastille(20);
  init_pastille(21,X1+170,Y1+Y+110,"Other resolution",ConfigRaytracer[15],"Render in a specific resolution");
  affiche_pastille(21);

  sprintf(ZTexte[13].Variable,"%d",ResolutionX);
  init_texte(13,X1+250,Y+Y1+130,"Width X",ZTexte[13].Variable,4,"Set X width");
  place_zone_texte(13);
  sprintf(ZTexte[14].Variable,"%d",ResolutionY);
  init_texte(14,X1+250,Y+Y1+150,"Height Y",ZTexte[14].Variable,4,"Set Y height");
  place_zone_texte(14);

  init_case(27,X1+170,Y1+Y+173,"Pause after trace",ConfigRaytracer[1],"Hit a key to continue...");
  affiche_case(27);
  init_case(28,X1+170,Y1+Y+188,"Bounding boxes",ConfigRaytracer[2],"Optimize rendering speed");
  affiche_case(28);
  init_case(22,X1+170,Y1+Y+203,"Light buffer",ConfigRaytracer[7],"Use light buffer");
  affiche_case(22);
  init_case(23,X1+170,Y1+Y+218,"Vista buffer",ConfigRaytracer[8],"Use vista buffer");
  affiche_case(23);

  XA=0; YA=60;
  border(XA+X1+5,Y2-65,XA+X1+290,Y2-39,FOND,1);
  init_pastille(23,XA+X1+ 10,Y2-58,"Quick",ConfigRaytracer[22],"Just colors");
  affiche_pastille(23);
  init_pastille(24,XA+X1+ 80,Y2-58,"ambiante",ConfigRaytracer[23],"With shadows");
  affiche_pastille(24);
  init_pastille(25,XA+X1+160,Y2-58,"textures",ConfigRaytracer[24],"With textures");
  affiche_pastille(25);
  init_pastille(26,XA+X1+236,Y2-58,"Best",ConfigRaytracer[25],"Best, reflexion & refraction");
  affiche_pastille(26);

  init_case(21,X1+10,Y1+Y+120,"Antialiasing",ConfigRaytracer[13],"Enabled antialiasing");
  affiche_case(21);
  XB=60; YB=145;
  border(X1+5,Y1+YB-27,X1+XB+70,Y1+YB+70,FOND,1);
  sprintf(ZTexte[0].Variable,"%.1f",AntiAliasValue);
  init_texte(0,XB+X1+10,YB+Y+Y1+  0,"Threshold",ZTexte[0].Variable,4,"Set antialiasing");
  place_zone_texte(0);
  sprintf(ZTexte[1].Variable,"%.1f",JitterValue);
  init_texte(1,XB+X1+10,YB+Y+Y1+ 20,"Jitter",ZTexte[1].Variable,4,"Set jitter value");
  place_zone_texte(1);
  sprintf(ZTexte[2].Variable,"%d",AntiAliasDepth);
  init_texte(2,XB+X1+10,YB+Y+Y1+ 40,"Depth",ZTexte[2].Variable,4,"Set antialiasing depth");
  place_zone_texte(2);

  XB=X1+304; YB=Y1+25;
  border(XB-5,YB,XB+171,Y2-39,FOND,1);
  init_case(29,XB+5,YB+10,"Radiosity",Rad_OnOff,"Enabled radiosity rendering");
  affiche_case(29);
  sprintf(ZTexte[3].Variable,"%.3f",Rad_Brightness);
    init_texte(3,XB+120,YB+ 30,"Brightness level",ZTexte[3].Variable,4,"Radiosity brightness");
    place_zone_texte(3);
  sprintf(ZTexte[4].Variable,"%d",Rad_Count);
    init_texte(4,XB+120,YB+ 50,"Count",ZTexte[4].Variable,4,"Set radiosity count");
    place_zone_texte(4);
  sprintf(ZTexte[5].Variable,"%.3f",Rad_Dist_Max);
    init_texte(5,XB+120,YB+ 70,"Depth",ZTexte[5].Variable,4,"Set radiosity distance max");
    place_zone_texte(5);
  sprintf(ZTexte[6].Variable,"%.3f",Rad_Err_Bound);
    init_texte(6,XB+120,YB+ 90,"Error Bound",ZTexte[6].Variable,4,"Set radiosity error bound");
    place_zone_texte(6);
  sprintf(ZTexte[7].Variable,"%.3f",Rad_Gray_Threshold);
    init_texte(7,XB+120,YB+110,"Gray threshold",ZTexte[7].Variable,4,"Set radiosity gray threshold");
    place_zone_texte(7);
  sprintf(ZTexte[8].Variable,"%.3f",Rad_Low_Err_Fact);
    init_texte(8,XB+120,YB+130,"Low error factor",ZTexte[8].Variable,4,"Set Radiosity low err factor");
    place_zone_texte(8);
  sprintf(ZTexte[9].Variable,"%.3f",Rad_Min_Reuse);
    init_texte(9,XB+120,YB+150,"Minimum reuse",ZTexte[9].Variable,4,"Set radiosity min reuse");
    place_zone_texte(9);
  sprintf(ZTexte[10].Variable,"%d",Rad_Near_Count);
    init_texte(10,XB+120,YB+170,"Nearest count",ZTexte[10].Variable,4,"Set radiosity nearest count");
    place_zone_texte(10);
  sprintf(ZTexte[11].Variable,"%d",Rad_Rec_Lim);
    init_texte(11,XB+120,YB+190,"Recursion limit",ZTexte[11].Variable,4,"Set radiosity recursion limit");
    place_zone_texte(11);

  init_pastille(27,XB+  5,YB+213,"Manual",Rad_Type==0,"Set radiosity manually");
  affiche_pastille(27);
  init_pastille(28,XB+ 65,YB+213,"Debug",Rad_Type==1,"Debug radiosity mode");
  affiche_pastille(28);
  init_pastille(29,XB+127,YB+213,"Fast",Rad_Type==2,"Fast radiosity mode");
  affiche_pastille(29);
  init_pastille(30,XB+  5,YB+230,"Normal",Rad_Type==3,"Normal radiosity mode");
  affiche_pastille(30);
  init_pastille(31,XB+ 65,YB+230,"2Bounce",Rad_Type==4,"Quite good quality");
  affiche_pastille(31);
  init_pastille(32,XB+127,YB+230,"Final",Rad_Type==5,"Final quality mode");
  affiche_pastille(32);
  RadType=quelle_pastille_dans_groupe(27,32)-27;

  while (i==-1) {
    i=bouton_dialog(X1,X2,Y2,0,1);
    
    test_groupe_pastille(15,21);
    test_case(10,14);

    test_groupe_pastille(23,26);
    test_groupe_pastille(27,32);
    if (RadType!=quelle_pastille_dans_groupe(27,32)-27) {
      switch ((RadType=quelle_pastille_dans_groupe(27,32)-27)) {
        case 1: // ---- Debug
          sprintf(ZTexte[ 4].Variable,"%d"  ,10);
          sprintf(ZTexte[ 6].Variable,"%.3f",0.3);
          sprintf(ZTexte[ 7].Variable,"%.3f",0);
          sprintf(ZTexte[ 5].Variable,"%.3f",10);
          sprintf(ZTexte[ 8].Variable,"%.3f",0.8);
          sprintf(ZTexte[10].Variable,"%d"  ,1);
          sprintf(ZTexte[ 9].Variable,"%.3f",0.015);
          sprintf(ZTexte[ 3].Variable,"%.3f",3.3);
          sprintf(ZTexte[11].Variable,"%d"  ,1);
          for (j=3;j<=11;j++) place_zone_texte(j);
          break;
        case 2: // ---- Fast
          sprintf(ZTexte[ 4].Variable,"%d"  ,80);
          sprintf(ZTexte[ 6].Variable,"%.3f",0.4);
          sprintf(ZTexte[ 7].Variable,"%.3f",0.6);
          sprintf(ZTexte[ 5].Variable,"%.3f",10);
          sprintf(ZTexte[ 8].Variable,"%.3f",0.9);
          sprintf(ZTexte[10].Variable,"%d"  ,5);
          sprintf(ZTexte[ 9].Variable,"%.3f",0.025);
          sprintf(ZTexte[ 3].Variable,"%.3f",3.3);
          sprintf(ZTexte[11].Variable,"%d"  ,1);
          for (j=3;j<=11;j++) place_zone_texte(j);
          break;
        case 3: // ---- Normal
          sprintf(ZTexte[ 4].Variable,"%d"  ,200);
          sprintf(ZTexte[ 6].Variable,"%.3f",0.3);
          sprintf(ZTexte[ 7].Variable,"%.3f",0.5);
          sprintf(ZTexte[ 5].Variable,"%.3f",10);
          sprintf(ZTexte[ 8].Variable,"%.3f",0.75);
          sprintf(ZTexte[10].Variable,"%d"  ,7);
          sprintf(ZTexte[ 9].Variable,"%.3f",0.017);
          sprintf(ZTexte[ 3].Variable,"%.3f",3.3);
          sprintf(ZTexte[11].Variable,"%d"  ,1);
          for (j=3;j<=11;j++) place_zone_texte(j);
          break;
        case 4: // ---- 2Bounce
          sprintf(ZTexte[ 4].Variable,"%d"  ,200);
          sprintf(ZTexte[ 6].Variable,"%.3f",0.3);
          sprintf(ZTexte[ 7].Variable,"%.3f",0.5);
          sprintf(ZTexte[ 5].Variable,"%.3f",10);
          sprintf(ZTexte[ 8].Variable,"%.3f",0.75);
          sprintf(ZTexte[10].Variable,"%d"  ,7);
          sprintf(ZTexte[ 9].Variable,"%.3f",0.017);
          sprintf(ZTexte[ 3].Variable,"%.3f",3.3);
          sprintf(ZTexte[11].Variable,"%d"  ,2);
          for (j=3;j<=11;j++) place_zone_texte(j);
          break;
        case 5: // ---- Final
          sprintf(ZTexte[ 4].Variable,"%d"  ,800);
          sprintf(ZTexte[ 6].Variable,"%.3f",0.2);
          sprintf(ZTexte[ 7].Variable,"%.3f",0.5);
          sprintf(ZTexte[ 5].Variable,"%.3f",4);
          sprintf(ZTexte[ 8].Variable,"%.3f",0.7);
          sprintf(ZTexte[10].Variable,"%d"  ,9);
          sprintf(ZTexte[ 9].Variable,"%.3f",0.01);
          sprintf(ZTexte[ 3].Variable,"%.3f",3.3);
          sprintf(ZTexte[11].Variable,"%d"  ,1);
          for (j=3;j<=11;j++) place_zone_texte(j);
          break;
      }
    }

    test_case(21,23);
    test_case(27,29);
    test_texte(0,2);
    if (RadType==0) test_texte(3,11);
    if (Pastille[21].Croix) test_texte(13,14);
  }

  if (!i) {
    modif_cfg_raytracer( 0,Cc[10].Croix);
    modif_cfg_raytracer( 1,Cc[27].Croix);
    modif_cfg_raytracer( 2,Cc[28].Croix);
    modif_cfg_raytracer( 3,Cc[11].Croix);
    modif_cfg_raytracer( 4,Cc[12].Croix);
    modif_cfg_raytracer( 5,Cc[13].Croix);
    modif_cfg_raytracer( 6,Cc[14].Croix);
    modif_cfg_raytracer( 7,Cc[22].Croix);
    modif_cfg_raytracer( 8,Cc[23].Croix);
    modif_cfg_raytracer(13,Cc[21].Croix);

    modif_cfg_raytracer(15,Pastille[21].Croix);
    modif_cfg_raytracer(16,Pastille[15].Croix);
    modif_cfg_raytracer(17,Pastille[16].Croix);
    modif_cfg_raytracer(18,Pastille[17].Croix);
    modif_cfg_raytracer(19,Pastille[18].Croix);
    modif_cfg_raytracer(20,Pastille[19].Croix);
    modif_cfg_raytracer(21,Pastille[20].Croix);

    modif_cfg_raytracer(22,Pastille[23].Croix);
    modif_cfg_raytracer(23,Pastille[24].Croix);
    modif_cfg_raytracer(24,Pastille[25].Croix);
    modif_cfg_raytracer(25,Pastille[26].Croix);

    AntiAliasValue=atof(ZTexte[0].Variable);
    JitterValue   =atof(ZTexte[1].Variable);
    AntiAliasDepth=atoi(ZTexte[2].Variable);
    ResolutionX   =atoi(ZTexte[13].Variable);
    ResolutionY   =atoi(ZTexte[14].Variable);

    Rad_OnOff=Cc[29].Croix;
    Rad_Type=quelle_pastille_dans_groupe(27,32)-27;
    Rad_Brightness    =atof(ZTexte[ 3].Variable);
    Rad_Count         =atoi(ZTexte[ 4].Variable);
    Rad_Dist_Max      =atof(ZTexte[ 5].Variable);
    Rad_Err_Bound     =atof(ZTexte[ 6].Variable);
    Rad_Gray_Threshold=atof(ZTexte[ 7].Variable);
    Rad_Low_Err_Fact  =atof(ZTexte[ 8].Variable);
    Rad_Min_Reuse     =atof(ZTexte[ 9].Variable);
    Rad_Near_Count    =atoi(ZTexte[10].Variable);
    Rad_Rec_Lim       =atoi(ZTexte[11].Variable);

  init_pastille(27,XB+  5,YB+213,"Manual",Rad_Type==0,"Set radiosity manually");
  init_pastille(28,XB+ 65,YB+213,"Debug",Rad_Type==1,"Debug radiosity mode");
  init_pastille(29,XB+127,YB+213,"Fast",Rad_Type==2,"Fast radiosity mode");
  init_pastille(30,XB+  5,YB+230,"Normal",Rad_Type==3,"Normal radiosity mode");
  init_pastille(31,XB+ 65,YB+230,"2Bounce",Rad_Type==4,"Quite good quality");
  init_pastille(32,XB+127,YB+230,"Final",Rad_Type==5,"Final quality mode");
  }

  while(MouseB());
  GMouseOff();
  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
  if (ModeMouse) GMouseOn();
  return i;
}

// ----------------------------------------------------------------------------
// --------  EXECUTE POVRAY POUR UN RENDU -------------------------------------
// ----------------------------------------------------------------------------
int render_povray(byte Niveau,byte BDialog) {
  char Parametre[2000]={NULLC};
  int i;
  FILE *Fichier;
  char Buffer[30];

  sprintf(Parametre,"%s%s",CheminRAYTRACER,POVExeName);

  if (access(Parametre,0)!=0) {
    beep_erreur();
    strcpy(StrBoite[0],"Invalid path");
    strcpy(StrBoite[1],"The directory for POV-Ray isn't valid !");
    strcpy(StrBoite[2],"Please, check your config file");
    strcpy(StrBoite[3],"in the sub-path SYSTEM :");
    sprintf(StrBoite[4],"%s",Parametre);
    g_boite_ONA(CentX,CentY,4,CENTRE,0);
    return 1;
  }

  switch (Niveau) {
    case 0:
      strcpy(Parametre,"+w128 +h96 -c +p +uv +ul +ft +x +b50 -v +mb10 +sp8\n");
      Rad_OnOff=0;
      break;
    case 1:
      strcpy(Parametre,"+w320 +h240 -c +p +uv +ul +ft +x +b50 -v +mb10 +sp8\n");
      Rad_OnOff=0;
      break;
    case 2:
      strcpy(Parametre,"+w640 +h480 -c +p +uv +ul +ft +x +b50 -v +mb10 +sp8\n");
      Rad_OnOff=0;
      break;
  }

  if (Niveau<3) {
    if (PalRenduRapide==1) sprintf(Buffer," +d%cG\n",OptVideo);
    if (PalRenduRapide==2) sprintf(Buffer," +d%c3\n",OptVideo);
    if (PalRenduRapide==3) sprintf(Buffer," +d%cH\n",OptVideo);
    if (PalRenduRapide==4) sprintf(Buffer," +d%cT\n",OptVideo);
    strcat(Parametre,Buffer);
  }

  if (Niveau==4) {
    if (!rendu_zone()) return 0;
    if (!analyse_matiere_scene(NULL)) return 1;
    if (!genere_include_matiere()) return 0;
  }

  if (Niveau>2) {
    if (BDialog) if (dialog_povray()) return 1;
    Parametre[0]=NULLC;
    for (i=0;i<=30;i++) {
      if (ConfigRaytracer[i]) {
        switch (i) {
          case  0: strcat(Parametre,"+b128\n"); break;
          case  1: strcat(Parametre,"+p\n"); break;
          case  2: strcat(Parametre,"+mb10\n"); break;
          case  3: strcat(Parametre,"+c\n"); break;
          case  4: strcat(Parametre,"-d +v\n"); break;
          case  7: strcat(Parametre,"+ul\n"); break;
          case  8: strcat(Parametre,"+uv\n"); break;
          case 13: sprintf(Buffer,"+a%.1f +j%.1f +r%d\n",
                             AntiAliasValue,
                             JitterValue,
                             AntiAliasDepth);
                     strcat(Parametre,Buffer);
                     break;
          case 22: strcat(Parametre,"+q0\n"); break;
          case 23: strcat(Parametre,"+q3\n"); break;
          case 24: strcat(Parametre,"+q6\n"); break;
          case 25: strcat(Parametre,"+q9\n"); break;
        }
      }
    }
  }

  if (Niveau>2) {
    if (ConfigRaytracer[5]) {
      if (ConfigRaytracer[15]) {
        sprintf(Buffer,"-w%d -h%d\n",ResolutionX,ResolutionY);
        strcat(Parametre,Buffer);
      }
      if (ConfigRaytracer[16]) strcat(Parametre,"-w128 -h96\n");
      if (ConfigRaytracer[17]) strcat(Parametre,"-w320 -h200\n");
      if (ConfigRaytracer[18]) strcat(Parametre,"-w320 -h240\n");
      if (ConfigRaytracer[19]) strcat(Parametre,"-w640 -h480\n");
      if (ConfigRaytracer[20]) strcat(Parametre,"-w800 -h600\n");
      if (ConfigRaytracer[21]) strcat(Parametre,"-w1024 -h768\n");
      if (!ConfigRaytracer[4]) {
        strcat(Parametre,"+dGG\n");
      } else {
        strcat(Parametre,"+v\n");
      }
    } else {
      if (ConfigRaytracer[15]) {
        sprintf(Buffer,"-w%d -h%d\n",ResolutionX,ResolutionY);
        strcat(Parametre,Buffer);
      }
      if (ConfigRaytracer[16]) strcat(Parametre,"-w128 -h96\n");
      if (ConfigRaytracer[17]) strcat(Parametre,"-w320 -h200\n");
      if (ConfigRaytracer[18]) strcat(Parametre,"-w320 -h240\n");
      if (ConfigRaytracer[19]) strcat(Parametre,"-w640 -h480\n");
      if (ConfigRaytracer[20]) strcat(Parametre,"-w800 -h600\n");
      if (ConfigRaytracer[21]) strcat(Parametre,"-w1024 -h768\n");
      if (!ConfigRaytracer[4]) {
        if (PalRenduRapide==1) strcat(Parametre," +dGG\n");
        if (PalRenduRapide==2) strcat(Parametre," +dG3\n");
        if (PalRenduRapide==3) strcat(Parametre," +dGH\n");
        if (PalRenduRapide==4) strcat(Parametre," +dGT\n");
      } else {
        strcat(Parametre,"+v\n");
      }
    }
  }

  if (Niveau==4) {
    sprintf(Buffer," +sr%.3f +er%.3f +sc%.3f +ec%.3f",RZ[1],RZ[3],RZ[0],RZ[2]);
    strcat(Parametre,Buffer);
  }

  strcat(Parametre," -i");
  strcat(Parametre,CheminPOVSCN);
  strcat(Parametre,"\\");
  strcat(Parametre,FichierSCN);
  i=strinstr(0,Parametre,".SCN");
  Parametre[i]=NULLC;
  strcat(Parametre,".POV\n");

  strcat(Parametre,"-o");
  strcat(Parametre,CheminIMAGE);
  strcat(Parametre,"\\");
  strcat(Parametre,FichierSCN);
  i=strinstr(0,Parametre,".SCN");
  Parametre[i]=NULLC;
  strcat(Parametre,".TGA\n");

  strcat(Parametre,"+ga");
  strcat(Parametre,CheminPOVSCN);
  strcat(Parametre,"\\");
  strcat(Parametre,FichierSCN);
  i=strinstr(0,Parametre,".SCN");
  Parametre[i]=NULLC;
  strcat(Parametre,".LOG\n");

  strcat(Parametre,"+x\n");

  sprintf(LastImage,"%s\\%s",CheminIMAGE,FichierSCN);
  i=strinstr(0,LastImage,".SCN");
  LastImage[i]=NULLC;
  strcat(LastImage,".TGA\n");

  for (i=0;i<5;i++) {
    if (Library[i][0]) {
      strcat(Parametre,"-l");
      strcat(Parametre,Library[i]);
      strcat(Parametre,"\n");
    }
  }

  if (Rad_OnOff) strcat(Parametre,"+QR\n");

  if (!genere_povray(Niveau)) return 0;

  // --------- dirige le r‚pertoire vers povray
  
  if (!sauve_scene_en_quittant()) return 1;
  sauve_config_interface(1);
  RestaureChemin();
  GMouseOff();
  reset_mouse();
  fin_mode_graphique();
  set_disk(CheminRAYTRACER[0]-65);
  CheminRAYTRACER[strlen(CheminRAYTRACER)-1]=NULLC;
  chdir(CheminRAYTRACER);

  // --------- ‚criture du fichier de paramŠtre

  Fichier=fopen("POVLAB.INI","w+t");
  fprintf(Fichier,"%s",Parametre);
  fclose(Fichier);

  // --------- Pr‚pare la sortie en mode texte

  _enable();
  GMouseOff();
  sauve_config_interface(0);
  charge_motif(EFFACE);
  palette_noire();
  reset_mouse();

  fin_mode_graphique();
  text_mode();

  // --------- Affiche les paramŠtres

  #if !WINDOWS
  _settextcolor(15);
  _setbkcolor(1);

  goto_xy(1,1);
  _outtext(RES_MESS[3]);

  for (i=strlen(RES_MESS[3]);i<80;i++) { goto_xy(i,1); _outtext(" "); }

  _settextcolor(7);
  _setbkcolor(0);
  #endif

  goto_xy(1,2);
  printf("\nþ Rendering scene named             : %s",FichierSCN);
  printf("\nþ Rendering in radiosity            : %s",Rad_OnOff ? "On":"Off");
  for (i=0;i<=4;i++) {
    printf("\nþ Rendering fog layer #%d        : %s",i,Fog[i].Ok ? "On":"Off");
  }
  printf("\nþ Rendering atmosphere              : %s",Atmos_OnOff ? "On":"Off");
  printf("\nþ Number of objects in scene        : %04d",NbObjet);
  printf("\nþ Number of omnilights sources      : %03d",NbOmni);
  printf("\nþ Number of spotlights sources      : %03d",NbSpot);
  printf("\nþ Number of cylindrical lights      : %03d",NbCylLight);
  printf("\nþ Number of arealights sources      : %03d",NbArea);
  printf("\nþ Number of polygones in scene      : %03d",NbPoly+1);
  printf("\n\n");
  init_curseur(6,7);

  // --------- D‚marrage de povray

  for (i=0;i<=NbObjet;i++) free_mem_objet(i);
  for (i=0;i<=NbPoly;i++) free_mem_poly(i);
  for (i=0;i<=NbSpline;i++) free_mem_spline(i);

  exit(200);
  return 1;
}

// -------------------------------------------------------------------------
// ------------ LECTURE/ECRITURE DU FICHIER DE CONFIGURATION ---------------
// -------------------------------------------------------------------------
// -- de gauche … droite
// -- bit nø 0 : Buffer activ‚
// -- bit nø 1 : Pause aprŠs calculs
// -- bit nø 2 : Boites englobantes
// -- bit nø 3 : Continue rendu
// -- bit nø 4 : Mode vid‚o activ‚
// -- bit nø 5 : Palette 64 Niveaux de gris
// -- bit nø 6 : Rapport ascii
// -- bit nø 7 : Use light buffer
// -- bit nø 8 : Use vista buffer
// -- bit nø13 : antialiasing
// -- bit nø15 : Autres formats
// -- bit nø16 : format 128x96
// -- bit nø17 : format 320x200
// -- bit nø18 : format 320x240
// -- bit nø19 : format 640x480
// -- bit nø20 : format 800x600
// -- bit nø21 : format 1024x768
// -- bit nø22 : qualit‚ rapide
// -- bit nø23 : qualit‚ ambiance
// -- bit nø24 : qualit‚ matiŠres
// -- bit nø25 : qualit‚ ombres
// -------------------------------------------------------------------------
// ---------- ECRITURE DU FICHIER DE CONFIGURATION POUR POV-RAY ------------
// -------------------------------------------------------------------------
void sauve_config_raytracer(FILE *Fichier,byte Travail) {
  register int i;

  if (Travail) {
    message("Save raytracer configuration");
  }

  fprintf(Fichier,"QuickQuality=%c\n",ConfigRaytracer[22]+'0');
  fprintf(Fichier,"AmbianteQuality=%c\n",ConfigRaytracer[23]+'0');
  fprintf(Fichier,"TextureQuality=%c\n",ConfigRaytracer[24]+'0');
  fprintf(Fichier,"OptimumQuality=%c\n",ConfigRaytracer[25]+'0');
  fprintf(Fichier,"BufferImage=%c\n",ConfigRaytracer[0]+'0');
  fprintf(Fichier,"PauseAfterTrace=%c\n",ConfigRaytracer[1]+'0');
  fprintf(Fichier,"BoundedBoxes=%c\n",ConfigRaytracer[2]+'0');
  fprintf(Fichier,"ContinueTrace=%c\n",ConfigRaytracer[3]+'0');
  fprintf(Fichier,"VideoDisable=%c\n",ConfigRaytracer[4]+'0');
  fprintf(Fichier,"LevelOfGray=%c\n",ConfigRaytracer[5]+'0');
  fprintf(Fichier,"AsciiReport=%c\n",ConfigRaytracer[6]+'0');
  fprintf(Fichier,"Antialiasing=%c\n",ConfigRaytracer[13]+'0');
  fprintf(Fichier,"UseLightBuffer=%c\n",ConfigRaytracer[7]+'0');
  fprintf(Fichier,"UseVistaBuffer=%c\n",ConfigRaytracer[8]+'0');

  for (i=0;i<=6;i++) {
    if (ConfigRaytracer[15+i]) {
      switch (i) {
        case 0: fprintf(Fichier,"Resolution=0\n"); break;
        case 1: fprintf(Fichier,"Resolution=1\n"); break;
        case 2: fprintf(Fichier,"Resolution=2\n"); break;
        case 3: fprintf(Fichier,"Resolution=3\n"); break;
        case 4: fprintf(Fichier,"Resolution=4\n"); break;
        case 5: fprintf(Fichier,"Resolution=5\n"); break;
        case 6: fprintf(Fichier,"Resolution=6\n"); break;
      }
    }
  }
}

// -------------------------------------------------------------------------
// ------- ECRITURE DU FICHIER DE CONFIGURATION POUR POV-RAY ---------------
// -------------------------------------------------------------------------
void lecture_config_raytracer(char *Buffer) {
  if (!strinstr(0,Buffer,"Resolution")) modif_cfg_raytracer(15+Buffer[11]-'0',1);
  if (!strinstr(0,Buffer,"QuickQuality")) modif_cfg_raytracer(22,Buffer[13]-'0');
  if (!strinstr(0,Buffer,"AmbianteQuality")) modif_cfg_raytracer(23,Buffer[16]-'0');
  if (!strinstr(0,Buffer,"TextureQuality")) modif_cfg_raytracer(24,Buffer[15]-'0');
  if (!strinstr(0,Buffer,"OptimumQuality")) modif_cfg_raytracer(25,Buffer[15]-'0');
  if (!strinstr(0,Buffer,"BufferImage")) modif_cfg_raytracer(0,Buffer[12]-'0');
  if (!strinstr(0,Buffer,"PauseAfterTrace")) modif_cfg_raytracer(1,Buffer[16]-'0');
  if (!strinstr(0,Buffer,"BoundedBoxes")) modif_cfg_raytracer(2,Buffer[13]-'0');
  if (!strinstr(0,Buffer,"ContinueTrace")) modif_cfg_raytracer(3,Buffer[14]-'0');
  if (!strinstr(0,Buffer,"VideoDisable")) modif_cfg_raytracer(4,Buffer[13]-'0');
  if (!strinstr(0,Buffer,"LevelOfGray")) modif_cfg_raytracer(5,Buffer[12]-'0');
  if (!strinstr(0,Buffer,"AscciReport")) modif_cfg_raytracer(6,Buffer[12]-'0');
  if (!strinstr(0,Buffer,"Antialiasing")) modif_cfg_raytracer(13,Buffer[13]-'0');
  if (!strinstr(0,Buffer,"UseLightBuffer")) modif_cfg_raytracer(7,Buffer[15]-'0');
  if (!strinstr(0,Buffer,"UseVistaBuffer")) modif_cfg_raytracer(8,Buffer[15]-'0');
}

// --------------------------------------------------------------------------
// ----- EXECUTE UN RENDU AVEC LE LOGICIEL SPECIFIE -------------------------
// --------------------------------------------------------------------------
int lance_calcul(byte Niveau,byte BDialog) {
  if (pas_lumiere(0)) {
    if (x_fenetre(CentX,CentY,GAUCHE,1,"Error|There no light(s). Continue anyway ?")) return 1;
  }
  if (pas_camera(0)) {
    if (x_fenetre(CentX,CentY,GAUCHE,1,"Error|There no camera. Continue anyway ?")) return 1;
  }
  if (pas_objet(1)) return 1;
  if (!verifie_mapping()) return 0;
  if (!test_si_nouveau_scn()) return 0;
  if (Niveau<3 && !OptSauve) {
    strcpy(StrBoite[0],"Attention");
    strcpy(StrBoite[1],"Do you really want to render the scene ?");
    strcpy(StrBoite[2],"Have you saved your modifications in the");
    sprintf(StrBoite[3],"scene file %s ?",FichierSCN);
    if (g_boite_ONA (CentX,CentY,3,CENTRE,1)) return 0;
  }
  if (Niveau!=4) if (!analyse_matiere_scene(NULL)) return 0;
  if (!test_couleur_omni(1,NbOmni)) return 0;
  if (!test_couleur_area(1,NbArea)) return 0;
  if (!test_couleur_spot(1,NbSpot)) return 0;
  if (!test_couleur_cyllight(1,NbCylLight)) return 0;
  if (Niveau!=4) if (!genere_include_matiere()) return 0;
  
  return render_povray(Niveau,BDialog);
}

// --------------------------------------------------------------------------
// ----- EXECUTE UN RENDU AVEC LE LOGICIEL SPECIFIE -------------------------
// --------------------------------------------------------------------------
byte dialog_raytracer(void) {
  return dialog_povray();
}

// --------------------------------------------------------------------------
// ----- LANCE LE RENDU DU DERNIER MODE DEMANDE -----------------------------
// --------------------------------------------------------------------------
byte lance_rendu_rapide(byte Bool) {
  Bool=Bool;
  lance_calcul(DernierRendu,0);
  return Bool;
}

// -------------------------------------------------------------------------
// -- INITIALISE LES PARARETRES DU BROUILLARD ------------------------------
// -------------------------------------------------------------------------
void init_fog(void) {
  int i;

  for (i=0;i<=4;i++) {
    Fog[i].Ok=0;
    Fog[i].Type=1;
    Fog[i].Offset=0.0;
    Fog[i].Alt=1.0;
    Fog[i].TurbD=0.0;
    Fog[i].Distance=10;
    Fog[i].Turbulence=0;
    Fog[i].RGB[0]=255;
    Fog[i].RGB[1]=255;
    Fog[i].RGB[2]=255;
  }
}

// -------------------------------------------------------------------------
// -- MODIFIE LES PARARETRES DU BROUILLARD ---------------------------------
// -------------------------------------------------------------------------
void parametre_brouillard(void) {
  int X1=CentX-130;
  int X2=CentX+130;
  int Y1=CentY-100;
  int Y2=CentY+114,i;
  char String[30];
  static Nb=1;
  int Y=35;
  int X=25;
  FOG *FogTmp[5];
  int Ok=0;
  int NbTmp;

  for (i=0;i<=4;i++) {
    FogTmp[i]=(FOG *) mem_alloc(sizeof(FOG));
    memcpy(FogTmp[i],&Fog[i],sizeof(FOG));
  }

  LABEL_REDRAW:

  g_fenetre(X1,Y1,X2,Y2,"Fog parameters",AFFICHE);
  bouton_dialog(X1,X2,Y2,1,1);

  LABEL_REDRAW_2:

  border(X1+5,Y1+Y-5,X2-5,Y1+Y+30,FOND,1);
  init_pastille(11,X1+X    ,Y1+Y+7,"#1",Nb==1,"Select layer #1");
  affiche_pastille(11);
  init_pastille(12,X1+X+ 45,Y1+Y+7,"#2",Nb==2,"Select layer #2");
  affiche_pastille(12);
  init_pastille(13,X1+X+ 90,Y1+Y+7,"#3",Nb==3,"Select layer #3");
  affiche_pastille(13);
  init_pastille(14,X1+X+135,Y1+Y+7,"#4",Nb==4,"Select layer #4");
  affiche_pastille(14);
  init_pastille(15,X1+X+180,Y1+Y+7,"#5",Nb==5,"Select layer #5");
  affiche_pastille(15);

  init_bouton(30,X1+190,Y1+121,55,55,"",GAUCHE,ATTEND,RES_AIDE[46]);
  affiche_bouton(30);
  modif_entree_palette(255,Fog[Nb].RGB[_R],Fog[Nb].RGB[_V],Fog[Nb].RGB[_B]);
  vignette_couleur(Bt[30].X+5,
                   Bt[30].Y+5,
                   Bt[30].X+Bt[30].L-5,
                   Bt[30].Y+Bt[30].H-6,255,BLANC);

  init_case(11,X1+190,Y1+ 80,"Use fog",Fog[Nb].Ok,"enable fog in the scene");
  affiche_case(11);
  init_case(12,X1+190,Y1+100,"Layered",Fog[Nb].Type-1,"Enable layered fog in the scene");
  affiche_case(12);

  sprintf(String,"%.4g",Fog[Nb].Distance);
  init_texte(0,X1+85,Y1+ 79,"Fog distance",String,10,"Modify fog's distance");
  place_zone_texte(0);

  sprintf(ZTexte[1].Variable,"%.4g",Fog[Nb].Alt);
  init_texte(1,X1+85,Y1+ 99,"Fog altitude",ZTexte[1].Variable,10,"Modify fog's altitude");
  place_zone_texte(1);

  sprintf(ZTexte[2].Variable,"%.4g",Fog[Nb].Offset);
  init_texte(2,X1+85,Y1+119,"Fog offset",ZTexte[2].Variable,10,"Modify fog's offset");
  place_zone_texte(2);

  sprintf(ZTexte[3].Variable,"%.4g",Fog[Nb].Turbulence);
  init_texte(3,X1+85,Y1+139,"Fog turbulence",ZTexte[3].Variable,10,"Modify fog's turbulence");
  place_zone_texte(3);

  sprintf(ZTexte[4].Variable,"%.4g",Fog[Nb].TurbD);
  init_texte(4,X1+85,Y1+159,"Fog turb depth",ZTexte[4].Variable,10,"Modify fog's turbulence depth");
  place_zone_texte(4);

  init_bouton(31,X1+10,Y2-30,70,21,"Use mouse",CENTRE,ATTEND,"Visual distance with mouse");
  affiche_bouton(31);

  while (1) {
    test_case(11,12);
    test_texte(0,(Cc[12].Croix ? 4:0));
    if ((i=test_groupe_pastille(11,15))!=-1) {
      i-=10;
      NbTmp=Nb;
      if (Nb!=i) {
        Nb=i;
        Ok=1;
        break;
      }
    }
    if ((i=test_bouton(30,31))!=-1) {
      switch(i) {
        case 30:
          creation_couleur(&Fog[Nb].RGB[_R],&Fog[Nb].RGB[_V],&Fog[Nb].RGB[_B],"Fog's color");
          bouton_dialog(X1,X2,Y2,1,1);
          break;
        case 31:
          g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
          Fog[Nb].Distance=distance_fog(Fog[Nb].Distance);
          forme_mouse(MS_FLECHE);
          goto LABEL_REDRAW;
      }
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) { Ok=0; break; }
  }

  if (i==0 || Ok==1) {
    if (Ok==1) swap_int(&Nb,&NbTmp);
    Fog[Nb].Distance=atof(ZTexte[0].Variable);
    Fog[Nb].Ok=Cc[11].Croix;
    Fog[Nb].Type=Cc[12].Croix+1;
    if (Fog[Nb].Type==2) {
      Fog[Nb].Alt=atof(ZTexte[1].Variable);
      Fog[Nb].Offset=atof(ZTexte[2].Variable);
      Fog[Nb].Turbulence=atof(ZTexte[3].Variable);
      Fog[Nb].TurbD=atof(ZTexte[4].Variable);
    }
    if (Ok==1) {
      swap_int(&Nb,&NbTmp);
      Ok=0;
      goto LABEL_REDRAW_2;
    }
  }

  if (i>1) {
    for (i=0;i<=4;i++) {
      memcpy(&Fog[i],FogTmp[i],sizeof(FOG));
      free(FogTmp[i]);
    }
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
}

// -------------------------------------------------------------------------
// -- ATTRIBUTS ET SETUP DU RENDU ------------------------------------------
// -------------------------------------------------------------------------
void setup_render(void) {
  int X1=CentX-95;
  int X2=CentX+95;
  int Y1=CentY-120;
  int Y2=CentY+125;
  int XA=X1+10,YA=Y1+23;
  int i;

  if (pas_camera(1)) return;
  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"Render setup",AFFICHE);

  message("Please read the POV-Ray documentation for more information");
  // border(XA,YA,XA+170,YA+136,0,1);

  sprintf(ZTexte[0].Variable,"%.4g",Global_ADC);
  init_texte(0,XA+100,YA+ 10,"Adaptive depth ctrl",ZTexte[0].Variable,7,"");

  sprintf(ZTexte[1].Variable,"%.1f",Global_A_Gamma);
  init_texte(1,XA+100,YA+ 30,"Assumed gamma",ZTexte[1].Variable,7,"");

  sprintf(ZTexte[2].Variable,"%d",Global_MaxTrace);
  init_texte(2,XA+100,YA+ 50,"Max trace level",ZTexte[2].Variable,7,"");

  sprintf(ZTexte[3].Variable,"%d",Global_MaxInter);
  init_texte(3,XA+100,YA+ 70,"Max intersection",ZTexte[3].Variable,7,"");

  sprintf(ZTexte[4].Variable,"%d",Global_NbWave);
  init_texte(4,XA+100,YA+ 90,"Number of waves",ZTexte[4].Variable,7,"");

  sprintf(ZTexte[5].Variable,"%.3f",Global_Irid[_R]);
  init_texte(5,XA+100,YA+110,"Irid wavelenght R",ZTexte[5].Variable,7,"");

  sprintf(ZTexte[6].Variable,"%.3f",Global_Irid[_V]);
  init_texte(6,XA+100,YA+130,"Irid wavelenght G",ZTexte[6].Variable,7,"");

  sprintf(ZTexte[7].Variable,"%.3f",Global_Irid[_B]);
  init_texte(7,XA+100,YA+150,"Irid wavelenght B",ZTexte[7].Variable,7,"");

  init_case(11,XA+10,YA+170,"Generate hidden objects",GenerateHidden,"Generate or not hidden objects");

  place_zone_texte(0);
  place_zone_texte(1);
  place_zone_texte(2);
  place_zone_texte(3);
  place_zone_texte(4);
  place_zone_texte(5);
  place_zone_texte(6);
  place_zone_texte(7);
  affiche_case(11);
  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(0,7);
    test_case(11,11);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  if (i==0) {
    Global_ADC=      atof(ZTexte[0].Variable);
    Global_A_Gamma=  atof(ZTexte[1].Variable);
    Global_MaxTrace= atoi(ZTexte[2].Variable);
    Global_MaxInter= atoi(ZTexte[3].Variable);
    Global_NbWave=   atoi(ZTexte[4].Variable);
    Global_Irid[_R]= atof(ZTexte[5].Variable);
    Global_Irid[_V]= atof(ZTexte[6].Variable);
    Global_Irid[_B]= atof(ZTexte[7].Variable);
    GenerateHidden=Cc[11].Croix;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  forme_mouse(MS_FLECHE);
}

// -------------------------------------------------------------------------
// -- MODIFIE LES PARAMETRES DE L'ATMOSPHERE -------------------------------
// -------------------------------------------------------------------------
void parametre_atmosphere(void) {
  int X1=CentX-110;
  int X2=CentX+110;
  int Y1=CentY-180;
  int Y2=CentY+180,i;
  char String[30];

  if (Atmos_Dist==0.0) Atmos_Dist=10;

  LABEL_REDRAW:

  g_fenetre(X1,Y1,X2,Y2,"Atmosphere parameters",AFFICHE);

  init_case(11,X1+10,Y1+30,"Use atmosphere",Atmos_OnOff,"enable atmosphere in the scene");
  affiche_case(11);

  init_bouton(30,X1+10,Y1+50,X2-X1-20,40,"",GAUCHE,ATTEND,RES_AIDE[46]);
  affiche_bouton(30);
  modif_entree_palette(255,Atmos_RGB[_R],Atmos_RGB[_V],Atmos_RGB[_B]);
  vignette_couleur(Bt[30].X+5,
                   Bt[30].Y+5,
                   Bt[30].X+Bt[30].L-5,
                   Bt[30].Y+Bt[30].H-6,255,BLANC);

  sprintf(String,"%.4g",Atmos_Dist);
  init_texte(0,X1+115,Y1+105,"Atmosphere distance",String,10,"Modify atmosphere's distance");
  place_zone_texte(0);

  sprintf(String,"%.4g",Atmos_Scatt);
  init_texte(1,X1+115,Y1+125,"Scattering",String,10,"");
  place_zone_texte(1);

  sprintf(String,"%.4g",Atmos_Eccent);
  init_texte(2,X1+115,Y1+145,"Eccentricity",String,10,"");
  place_zone_texte(2);
  
  sprintf(String,"%d",Atmos_Samples);
  init_texte(3,X1+115,Y1+165,"Samples",String,10,"");
  place_zone_texte(3);

  sprintf(String,"%.4g",Atmos_aa_t);
  init_texte(4,X1+115,Y1+185,"Aa threshold",String,10,"");
  place_zone_texte(4);

  sprintf(String,"%d",Atmos_aa_l);
  init_texte(5,X1+115,Y1+205,"Aa level",String,10,"");
  place_zone_texte(5);

  sprintf(String,"%.4f",Atmos_Jitter);
  init_texte(6,X1+115,Y1+225,"Jitter",String,10,"");
  place_zone_texte(6);

  init_pastille(11,X1+10,Y1+245,"Isotropic",(Atmos_Type==1),"Use isotropic scheme");
  affiche_pastille(11);
  init_pastille(12,X1+10,Y1+260,"Mie hazy",(Atmos_Type==2),"Use Mie hazy scheme");
  affiche_pastille(12);
  init_pastille(13,X1+10,Y1+275,"Mie murky",(Atmos_Type==3),"Use Mie murky scheme");
  affiche_pastille(13);
  init_pastille(14,X1+10,Y1+290,"Rayleigh",(Atmos_Type==4),"Use Rayleigh scheme");
  affiche_pastille(14);
  init_pastille(15,X1+10,Y1+305,"Henyey greenstein",(Atmos_Type==5),"Use Henyey greenstein scheme");
  affiche_pastille(15);

  init_bouton(31,X1+10,Y2-30,70,20,"Use mouse",CENTRE,ATTEND,"Visual distance with mouse");
  affiche_bouton(31);
  
  bouton_dialog(X1,X2,Y2,1,0);

  while (1) {
    test_case(11,11);
    test_texte(0,6);
    test_groupe_pastille(11,15);
    if ((i=test_bouton(30,31))!=-1) {
      switch(i) {
        case 30:
          creation_couleur(&Atmos_RGB[_R],&Atmos_RGB[_V],&Atmos_RGB[_B],"Atmosphere's color");
          bouton_dialog(X1,X2,Y2,1,0);
          break;
        case 31:
          g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
          Atmos_Dist=distance_fog(Atmos_Dist);
          forme_mouse(MS_FLECHE);
          goto LABEL_REDRAW;
      }
    }
    if ((i=bouton_dialog(X1,X2,Y2,0,0))!=-1) break;
  }

  if (i==0) {
    Atmos_Dist=atof(ZTexte[0].Variable);
    Atmos_Scatt=atof(ZTexte[1].Variable);
    Atmos_Eccent=atof(ZTexte[2].Variable);
    Atmos_Samples=atoi(ZTexte[3].Variable);
    Atmos_aa_t=atof(ZTexte[4].Variable);
    Atmos_aa_l=atoi(ZTexte[5].Variable);
    Atmos_Jitter=atof(ZTexte[6].Variable);
    Atmos_OnOff=Cc[11].Croix;
    Atmos_Type=quelle_pastille_dans_groupe(11,15)-10;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
}
