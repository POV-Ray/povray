/* ---------------------------------------------------------------------------
*  PREFEREN.C
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

#include <STRING.H>
#include <DOS.H>
#include <STDIO.H>
#include <IO.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"

int  NbRendu;
byte OptBeep=1;
byte OptAxe=1;
byte OptGrille=1;
byte OptAide=1;
byte OptBtD=0;
byte OptLabScn=1;
byte MSMenu=0;
byte SuiviSelection=1;
byte PalRenduRapide=2;
byte ShowCtrlPt=1;
char CheminPOVSCN[MAXPATH];
char CheminIMAGE[MAXPATH];
char CheminVIEWERI[MAXPATH];
char CheminVIEWERS[MAXPATH];
char CheminEDITEUR[MAXPATH];
char POVExeName[MAXPATH];
char CheminUserINC[MAXPATH];

byte POVForWindows=0;
byte GenerateHidden=0;
byte DEBUG=0;

// --------------------------------------------------------------------------
// ----- AFFICHAGE D'UNE FENETRE POUR LES PREFERENCES -----------------------
// --------------------------------------------------------------------------
byte fenetre_preferences(void) {
  register int i;
  register int X1,Y1,X2,Y2;
  char *Temp;

  X1=CentX-190;
  X2=CentX+190;
  Y1=CentY-140;
  Y2=CentY+140;

  efface_buffer_clavier();
  message("Enter your firstname, name, serial number and path needed by %s",NomLogiciel);

  sprintf(StrBoite[0],"ENVIRONMENT %s %s",NomLogiciel,VerLogiciel);
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  bouton_dialog(X1,X2,Y2,1,1);

  relief(X1+10,Y1+30,X2-10,Y2-37,0);
  sprintf(CheminRAYTRACER,"%s%s",CheminRAYTRACER,POVExeName);
  init_texte(0,X1+115,Y1+ 40,"Path to POV-Ray",CheminRAYTRACER,25,"Path where's POV-Ray");
  place_zone_texte(0);
  init_texte(1,X1+115,Y1+ 60,"POV output dir",CheminPOVSCN,25,"Path where to output POV's scenes");
  place_zone_texte(1);
  init_texte(2,X1+115,Y1+ 80,"Images output dir",CheminIMAGE,25,"Path where to output images");
  place_zone_texte(2);
  init_texte(3,X1+115,Y1+100,"Path for texture",CheminMTEX,25,"File for description file");
  place_zone_texte(3);
  init_texte(4,X1+115,Y1+120,"User's .INC file",CheminUserINC,25,"Entry for a specific .INC file");
  place_zone_texte(4);
  init_texte(5,X1+115,Y1+140,"User name",NomUtilisateur,25,"enter first name and name");
  place_zone_texte(5);
  init_texte(6,X1+115,Y1+160,"Image viewer",CheminVIEWERI,25,"Enter the path for your viewer");
  place_zone_texte(6);
  init_texte(7,X1+115,Y1+180,"Scene viewer",CheminVIEWERS,25,"Enter the path for  your viewer");
  place_zone_texte(7);
  init_texte(8,X1+115,Y1+200,"Manual editor",CheminEDITEUR,25,"Enter the path for your editor");
  place_zone_texte(8);
  init_case(11,X1+113,Y1+222,"Run POV-Ray for Windows",POVForWindows,"Run POV-Ray for Windows");
  affiche_case(11);

  while (1) {
    i=test_texte(0,8);
    test_case(11,11);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,NULL,EFFACE);

  Temp=(char *) malloc(strlen(ZTexte[0].Variable)+1);
  strcpy(Temp,ZTexte[0].Variable);
  split_chemin(POVExeName,ZTexte[0].Variable,5);
  split_chemin(CheminRAYTRACER,Temp,4);
  free(Temp);

  if (i==1) {
    return 0;
  } else {
    strcpy(CheminPOVSCN,   ZTexte[1].Variable);
    strcpy(CheminIMAGE,    ZTexte[2].Variable);
    strcpy(CheminMTEX,     ZTexte[3].Variable);
    strcpy(CheminUserINC,  ZTexte[4].Variable);
    strcpy(NomUtilisateur, ZTexte[5].Variable);
    strcpy(CheminVIEWERI,  ZTexte[6].Variable);
    strcpy(CheminVIEWERS,  ZTexte[7].Variable);
    strcpy(CheminEDITEUR,  ZTexte[8].Variable);
    
    POVForWindows=Cc[11].Croix;
    return 1;
  }
}

// -------------------------------------------------------------------------
// -------- ECRITURE DU FICHIER DE CONFIGURATION INTERFACE -----------------
// -------------------------------------------------------------------------
void sauve_config_interface(byte Travail) {
  FILE *Fichier;
  char Date[17],Heure[9];
  int i;

  get_date(Date);
  get_heure(Heure);

  Fichier=fopen("SYSTEM\\MODELLER.CFG","w+t");

  fprintf(Fichier,"%s version %s - Configuration file.\n",NomLogiciel,VerLogiciel);
  fprintf(Fichier,"(C) %s, Denis Olivier - ",RES_COPY[4]);
  fprintf(Fichier,"All rights reserved.\n");
  fprintf(Fichier,"Date=%s\n",Date);
  fprintf(Fichier,"Time=%s\n",Heure);
  fprintf(Fichier,"UserName=%s\n",NomUtilisateur);
  fprintf(Fichier,"RaytracerPath=%s\n",CheminRAYTRACER);
  fprintf(Fichier,"RaytracerExeName=%s\n",POVExeName);
  fprintf(Fichier,"PathToLastScene=%s%s\n",CheminSCN,FichierSCN);
  fprintf(Fichier,"PathToPovScene=%s\n",CheminPOVSCN);
  fprintf(Fichier,"SceneName=%s\n",FichierSCN);
  fprintf(Fichier,"PathToRenderImage=%s\n",CheminIMAGE);
  fprintf(Fichier,"PathToViewerImage=%s\n",CheminVIEWERI);
  fprintf(Fichier,"PathToViewerScene=%s\n",CheminVIEWERS);
  fprintf(Fichier,"PathToEditor=%s\n",CheminEDITEUR);
  fprintf(Fichier,"PathToSpecificInc=%s\n",CheminUserINC);
  fprintf(Fichier,"LastImage=%s\n",LastImage);
  fprintf(Fichier,"PatternPath=%s\n",CheminMotif);
  fprintf(Fichier,"TrueTypePath=%s\n",CheminTTF);
  fprintf(Fichier,"GraphicMode=%s\n",Resolution);
  fprintf(Fichier,"PaletteInterface=%d\n",NbCouleurs);
  fprintf(Fichier,"IncludeTexture=%s\n",CheminMTEX);
  fprintf(Fichier,"BeepInterface=%d\n",OptBeep);
  fprintf(Fichier,"DisplayPattern=%d\n",MotifOk);
  fprintf(Fichier,"MouseSpeed=%d\n",OptVSouris);
  fprintf(Fichier,"RightButtons=%d\n",OptBtD);
  fprintf(Fichier,"SaveWhenExit=%d\n",OptSauve);
  fprintf(Fichier,"FadeInterface=%d\n",OptFade);
  fprintf(Fichier,"MicrosoftMouse=%d\n",MSMouse);
  fprintf(Fichier,"MouseMenu=%d\n",MSMenu);
  fprintf(Fichier,"ModeLastTrace=%d\n",DernierRendu);
  fprintf(Fichier,"FollowSelection=%d\n",SuiviSelection);
  fprintf(Fichier,"PaletteTrace=%d\n",PalRenduRapide);
  fprintf(Fichier,"TorusSegment=%d\n",MailleTore);
  fprintf(Fichier,"HFPrecision=%d\n",MailleHF);
  fprintf(Fichier,"DitherControl=%d\n",Dither);
  fprintf(Fichier,"GraphicCard=%c\n",OptVideo);
  fprintf(Fichier,"AntiAliasValue=%.4f\n",AntiAliasValue);
  fprintf(Fichier,"JitterValue=%.4f\n",JitterValue);
  fprintf(Fichier,"AntiAliasDepth=%d\n",AntiAliasDepth);
  fprintf(Fichier,"ResolutionX=%d\n",ResolutionX);
  fprintf(Fichier,"ResolutionY=%d\n",ResolutionY);
  fprintf(Fichier,"POVForWindows=%d\n",POVForWindows);
  fprintf(Fichier,"DebugMode=%d\n",DEBUG);
  fprintf(Fichier,"GenerateHidden=%d\n",GenerateHidden);
  fprintf(Fichier,"AnalyseTexture=%d\n",AnalyseTexture);
  fprintf(Fichier,"DisplayAxis=%d\n",OptAxe);
  fprintf(Fichier,"GrilleType=%d\n",GrilleType);
  fprintf(Fichier,"DisplayGrid=%d\n",OptGrille);
  fprintf(Fichier,"ColorCamera=%d\n",CCAMERA);
  fprintf(Fichier,"ColorViewport=%d\n",FFOND);
  fprintf(Fichier,"ColorTextBackground=%d\n",ZFOND);
  fprintf(Fichier,"ColorSelection=%d\n",CSELECTION);
  fprintf(Fichier,"ColorInterface=%d\n",FOND);
  fprintf(Fichier,"ColorAxis=%d\n",CAXE);
  fprintf(Fichier,"ColorGrid=%d\n",CGRILLE);
  fprintf(Fichier,"ShowControlPoint=%d\n",ShowCtrlPt);
  fprintf(Fichier,"LoadPovlabScn=%d\n",OptLabScn);
  for (i=0;i<10;i++) fprintf(Fichier,"Library#%02d=%s\n",i+1,Library[i]);

  sauve_config_raytracer(Fichier,Travail);

  fclose(Fichier);
}

// -------------------------------------------------------------------------
// -------- LECTURE DU FICHIER DE CONFIGURATION INTERFACE ------------------
// -------------------------------------------------------------------------
void lecture_config_interface(void) {
  FILE *Fichier;
  char Buffer[MAXPATH];
  register int i;

  strcpy(Buffer,"SYSTEM\\MODELLER.CFG");
  log_out(0,"Reading config file");

  if (access(Buffer,0)==0) {
    for (i=0;i<=39;i++) ConfigRaytracer[i]=0;

    Fichier=fopen(Buffer,"r+t");
    while (!feof(Fichier)) {
      fgets(Buffer,80,Fichier);
      Buffer[strinstr(0,Buffer,"\n")]=NULLC;
      supprime_tab(Buffer);
      if (!strinstr(0,Buffer,"UserName")) {
        strcpy(NomUtilisateur,strupr(Buffer)+9);
        log_out(1,"Name : %s",NomUtilisateur);
      }
      if (!strinstr(0,Buffer,"RaytracerPath")) {
        strcpy(CheminRAYTRACER,strupr(Buffer)+14);
        log_out(1,"Raytracer : %s",CheminRAYTRACER);
      }
      if (!strinstr(0,Buffer,"RaytracerExeName")) {
        strcpy(POVExeName,strupr(Buffer)+17);
      }
      if (!strinstr(0,Buffer,"PatternPath")) {
        strcpy(CheminMotif,strupr(Buffer)+12);
        log_out(2,"Pattern : %s",CheminMotif);
      }
      if (!strinstr(0,Buffer,"IncludeTexture")) {
        strcpy(CheminMTEX,strupr(Buffer)+15);
        if (CheminMTEX[1]!=':') {
          sprintf(CheminMTEX,"%c:%s\\TEXTURE\\POVLAB.TEX",NewLecteur[0],NewChemin);
          log_out(3,"Texture : %s",CheminMTEX);
        }
      }
      if (!strinstr(0,Buffer,"PathToLastScene")) {
        strcpy(CheminLastSCN,strupr(Buffer)+16);
        log_out(4,"LastScene : %s",CheminLastSCN);
      }
      if (!strinstr(0,Buffer,"PathToPovScene")) {
        strcpy(CheminPOVSCN,strupr(Buffer)+15);
        log_out(5,"POVScene : %s",CheminPOVSCN);
      }
      if (!strinstr(0,Buffer,"PathToRenderImage")) {
        strcpy(CheminIMAGE,strupr(Buffer)+18);
        log_out(6,"POVImage : %s",CheminIMAGE);
      }
      if (!strinstr(0,Buffer,"PathToViewerImage")) {
        strcpy(CheminVIEWERI,strupr(Buffer)+18);
        log_out(7,"Viewer pic : %s",CheminVIEWERI);
      }
      if (!strinstr(0,Buffer,"PathToViewerScene")) {
        strcpy(CheminVIEWERS,strupr(Buffer)+18);
        log_out(7,"Viewer pov : %s",CheminVIEWERS);
      }
      if (!strinstr(0,Buffer,"PathToEditor")) {
        strcpy(CheminEDITEUR,strupr(Buffer)+13);
        log_out(7,"Editor : %s",CheminEDITEUR);
      }
      if (!strinstr(0,Buffer,"LastImage")) {
        strcpy(LastImage,strupr(Buffer)+10);
      }
      if (!strinstr(0,Buffer,"TrueTypePath")) {
        strcpy(CheminTTF,strupr(Buffer)+13);
        log_out(8,"TTF : %s",CheminTTF);
      }
      if (!strinstr(0,Buffer,"GraphicMode")) {
        strcpy(Resolution,strupr(Buffer)+12);
        log_out(9,"Graphic : %s",Resolution);
      }
      if (!strinstr(0,Buffer,"PathToSpecificInc")) {
        strcpy(CheminUserINC,strupr(Buffer)+18);
        log_out(6,"SpecificINC : %s",CheminUserINC);
      }
      if (!strinstr(0,Buffer,"Library#01")) strcpy(Library[0],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#02")) strcpy(Library[1],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#03")) strcpy(Library[2],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#04")) strcpy(Library[3],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#05")) strcpy(Library[4],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#06")) strcpy(Library[5],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#07")) strcpy(Library[6],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#08")) strcpy(Library[7],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#09")) strcpy(Library[8],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"Library#10")) strcpy(Library[9],strupr(Buffer)+11);
      if (!strinstr(0,Buffer,"AntiAliasValue")) AntiAliasValue=atof(Buffer+15);
      if (!strinstr(0,Buffer,"JitterValue")) JitterValue=atof(Buffer+12);
      if (!strinstr(0,Buffer,"AntiAliasDepth")) AntiAliasDepth=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"PaletteInterface")) NbCouleurs=atoi(Buffer+17);
      if (!strinstr(0,Buffer,"MouseSpeed")) OptVSouris=atoi(Buffer+11);
      if (!strinstr(0,Buffer,"BeepInterface")) OptBeep=atoi(Buffer+14);
      if (!strinstr(0,Buffer,"PaletteTrace")) PalRenduRapide=atoi(Buffer+13);
      if (!strinstr(0,Buffer,"RightButtons")) OptBtD=atoi(Buffer+13);
      if (!strinstr(0,Buffer,"DisplayPattern")) MotifOk=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"SaveWhenExit")) OptSauve=atoi(Buffer+13);
      if (!strinstr(0,Buffer,"FadeInterface")) OptFade=atoi(Buffer+14);
      if (!strinstr(0,Buffer,"MicrosoftMouse")) MSMouse=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"MouseMenu")) MSMenu=atoi(Buffer+10);
      if (!strinstr(0,Buffer,"ModeLastTrace")) DernierRendu=atoi(Buffer+14);
      if (!strinstr(0,Buffer,"FollowSelection")) SuiviSelection=atoi(Buffer+16);
      if (!strinstr(0,Buffer,"TorusSegment")) MailleTore=atoi(Buffer+13);
      if (!strinstr(0,Buffer,"HFPrecision")) MailleHF=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"DitherControl")) Dither=atoi(Buffer+14);
      if (!strinstr(0,Buffer,"GraphicCard")) OptVideo=Buffer[12];
      if (!strinstr(0,Buffer,"ResolutionX")) ResolutionX=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"ResolutionY")) ResolutionY=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"POVForWindows")) POVForWindows=atoi(Buffer+14);
      if (!strinstr(0,Buffer,"DebugMode")) DEBUG=atoi(Buffer+10);
      if (!strinstr(0,Buffer,"GrilleType")) GrilleType=atoi(Buffer+11);
      if (!strinstr(0,Buffer,"DiplayAxis")) OptAxe=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"DisplayGrid")) OptGrille=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"GenerateHidden")) GenerateHidden=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"AnalyseTexture")) AnalyseTexture=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"ColorCamera")) CCAMERA=atoi(Buffer+12);
      if (!strinstr(0,Buffer,"ColorViewport")) FFOND=NbCouleurs==256 ? atoi(Buffer+14):NOIR;
      if (!strinstr(0,Buffer,"ColorTextBackground")) ZFOND=atoi(Buffer+20);
      if (!strinstr(0,Buffer,"ColorSelection")) CSELECTION=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"ColorInterface")) FOND=atoi(Buffer+15);
      if (!strinstr(0,Buffer,"ColorAxis")) CAXE=atoi(Buffer+10);
      if (!strinstr(0,Buffer,"ColorGrid")) CGRILLE=atoi(Buffer+10);
      if (!strinstr(0,Buffer,"LoadPovlabScn")) OptLabScn=atoi(Buffer+14);
      if (!strinstr(0,Buffer,"ShowControlPoint")) ShowCtrlPt=atoi(Buffer+17);
      lecture_config_raytracer(Buffer);
    }
    fclose(Fichier);
  }
}

// --------------------------------------------------------------------------
// ----- AFFICHAGE D'UNE FENETRE POUR LES PARAMETRES SYSTEME ----------------
// --------------------------------------------------------------------------
byte fenetre_systeme(void) {
  /*
  #define N 10
  FILE *Fichier;
  int i;
  byte H=18;
  int X1=CentX-160;
  int X2=CentX+160;
  int Y1=(int) CentY-(25+(H*N)/2);
  int Y2=(int) CentY+(40+(H*N)/2);
  char Buffer[50];
  char Chaine[N][100];
  char Rubrique[N][30]={
    "Microprocessor",
    "Coprocessor",
    "Mode microprocessor",
    "Clock frequency",
    "Total memory",
    "CD-Rom present",
    "Support VESA version",
    "Video memory",
    "VESA signature",
    "Path for raytracer"
  };

  strcpy(Buffer,"SYSTEM\\SYSTEM.CFG");

  if (access(Buffer,0)==0) {
    Fichier=fopen(Buffer,"r+t");
    while (!feof(Fichier)) {
      fgets(Buffer,80,Fichier);
      Buffer[strinstr(0,Buffer,"\n")]=NULLC;
      if (!strinstr(0,Buffer,"Processor"))     strcpy(Chaine[0],Buffer+10);
      if (!strinstr(0,Buffer,"CoProcessor"))   strcpy(Chaine[1],Buffer+12);
      if (!strinstr(0,Buffer,"ModeProcessor")) strcpy(Chaine[2],Buffer+14);
      if (!strinstr(0,Buffer,"Frequency"))     strcpy(Chaine[3],Buffer+10);
      if (!strinstr(0,Buffer,"TotalMemory"))   strcpy(Chaine[4],Buffer+12);
      if (!strinstr(0,Buffer,"CdRomDrive"))    strcpy(Chaine[5],Buffer+11);
      if (!strinstr(0,Buffer,"SupportVESA"))   strcpy(Chaine[6],Buffer+12);
      if (!strinstr(0,Buffer,"VideoMemory"))   strcpy(Chaine[7],Buffer+12);
      if (!strinstr(0,Buffer,"VESASignature")) strcpy(Chaine[8],Buffer+14);
    }
    fclose(Fichier);
  } else {
    return 0;
  }

  strcpy(Chaine[9],CheminRAYTRACER);

  sprintf(StrBoite[0],"SYSTEM UNDER %s %s",NomLogiciel,VerLogiciel);
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  init_bouton(40,X1+(X2-X1)/2-25,Y2-30,50,20,"Ok",CENTRE,ATTEND,"Quit");
  affiche_bouton(40);

  for (i=0;i<N;i++) {
    parse_ascii_watcom(Rubrique[i]);
    parse_ascii_watcom(Chaine[i]);
    relief(X1+10,Y1+28+i*H,X1+130-3,Y1+43+i*H,0);
    text_xy(X1+130-largeur_text(Rubrique[i])-6,Y1+29+i*H,Rubrique[i],15);
    windows(X1+130,Y1+29+i*H,X2-10,Y1+43+i*H,0,ZFOND);
    text_xy(X1+135,Y1+29+i*H,Chaine[i],JAUNE);
  }

  while (1 && !sequence_sortie()) {
    if ((i=test_bouton(40,41))!=-1) { i-=40; break; }
  }

  g_fenetre(X1,Y1,X2,Y2,NULL,EFFACE);
  */
  return 1;
}

// --------------------------------------------------------------------------
// ----- AFFICHAGE D'UNE FENETRE POUR LA GESTION DE LA MEMOIRE --------------
// --------------------------------------------------------------------------
byte gestion_memoire(void) {
  register int i;
  register int X1,Y1,X2,Y2;
  char MinMem[12];
  char MaxMem[12];
  char VSize[12];
  byte TmpFile=0;
  char Buffer[MAXPATH];
  FILE *Fichier;
  long Tmp;
  char NomFichier[]="SYSTEM\\MODELLER.MEM";

  X1=CentX-120;
  X2=CentX+120;
  Y1=CentY-80;
  Y2=CentY+80;

  if (access(NomFichier,0)==0) {
    Fichier=fopen(NomFichier,"r+t");
    while (!feof(Fichier)) {
      fgets(Buffer,80,Fichier);
      Buffer[strinstr(0,Buffer,"\n")]=NULLC;
      supprime_tab(Buffer);
      strlwr(Buffer);
      analyse_ligne(Buffer,32);
      if (!strinstr(0,Argu[0],"minmem")) strcpy(MinMem,Argu[2]);
      if (!strinstr(0,Argu[0],"maxmem")) strcpy(MaxMem,Argu[2]);
      if (!strinstr(0,Argu[0],"virtualsize")) strcpy(VSize,Argu[2]);
      if (!strinstr(0,Argu[0],"deleteswap")) TmpFile=1;
    }
    fclose(Fichier);
  }

  efface_buffer_clavier();
  message("Sizes are in Kb. Modifications takes effect after quitting.");

  strcpy(StrBoite[0],"VIRTUAL MEMORY");
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  init_texte(1,X1+110,Y1+ 35,"Minimum memory",MinMem,12,"");
  place_zone_texte(1);
  init_texte(2,X1+110,Y1+ 55,"Maximum memory",MaxMem,12,"");
  place_zone_texte(2);
  init_texte(3,X1+110,Y1+ 75,"Virtual memory",VSize,12,"");
  place_zone_texte(3);
  init_case(10,X1+109,Y1+100,"Temp file",TmpFile,"");
  affiche_case(10);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_texte(1,3);
    test_case(10,10);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==1) {
    return 0;
  } else {
    TmpFile=Cc[10].Croix;
    strcpy(MinMem,ZTexte[1].Variable);
    strcpy(MaxMem,ZTexte[2].Variable);
    strcpy(VSize,ZTexte[3].Variable);
    Tmp=atol(MinMem);
    if (!compris_entre(512L,Tmp,32768L)) return 0;
    Tmp=atol(MaxMem);
    if (!compris_entre(512L,Tmp,32768L)) return 0;
    Tmp=atol(VSize);
    if (!compris_entre(512L,Tmp,32768L)) return 0;
    Fichier=fopen(NomFichier,"w+t");
    fprintf(Fichier,"minmem = %s\n",MinMem);
    fprintf(Fichier,"maxmem = %s\n",MaxMem);
    if (TmpFile) fprintf(Fichier,"deleteswap\n");
    fprintf(Fichier,"virtualsize = %s\n",VSize);
    fprintf(Fichier,"swapname = MODELLER.TMP\n\n");
    fclose(Fichier);
    return 1;
  }
}

// --------------------------------------------------------------------------
// --- AFFICHAGE D'UNE FENETRE/UTILISE POUR LE CHOIX DES MENUS SOURIS -------
// --------------------------------------------------------------------------
void menu_souris(byte Bool) {
  int i;
  register int X1,Y1,X2,Y2;

  if (Bool==MODIF) {
    X1=CentX-90;
    X2=CentX+90;
    Y1=CentY-120;
    Y2=CentY+120;

    message("Assign one of this functions to the right mouse button");

    strcpy(StrBoite[0],"FUNCTIONS FOR MOUSE");
    g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

    bouton_dialog(X1,X2,Y2,1,1);

    init_pastille(10,X1+30,Y1+ 30,"None",(MSMenu==0),"");
    affiche_pastille(10);
    init_pastille(11,X1+30,Y1+ 45,"Dialog box options",(MSMenu==1),"");
    affiche_pastille(11);
    init_pastille(12,X1+30,Y1+ 60,"Recentering viewport",(MSMenu==2),"");
    affiche_pastille(12);
    init_pastille(13,X1+30,Y1+ 75,"Save file",(MSMenu==3),"");
    affiche_pastille(13);
    init_pastille(14,X1+30,Y1+ 90,"No selection",(MSMenu==4),"");
    affiche_pastille(14);
    init_pastille(15,X1+30,Y1+105,"Texture dialog box",(MSMenu==5),"");
    affiche_pastille(15);
    init_pastille(16,X1+30,Y1+120,"Render parameters",(MSMenu==6),"");
    affiche_pastille(16);
    init_pastille(17,X1+30,Y1+135,"Fast preview in 320x240",(MSMenu==7),"");
    affiche_pastille(17);
    init_pastille(18,X1+30,Y1+150,"Zoom in viewport",(MSMenu==8),"");
    affiche_pastille(18);
    init_pastille(19,X1+30,Y1+165,"Zoom out viewport",(MSMenu==9),"");
    affiche_pastille(19);
    init_pastille(20,X1+30,Y1+180,"Pan in viewport",(MSMenu==10),"");
    affiche_pastille(20);

    while (1) {
      test_groupe_pastille(10,20);
      if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
    }

    g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

    if (i==0) MSMenu=quelle_pastille_dans_groupe(10,20)-10;
  } else {
    while (MouseB());
    switch (MSMenu) {
      case  0: return;
      case  1: options_fenetres(); return;
      case  2: bouton_recentre(); break;
      case  3: sauve_fichier(0,0); break;
      case  4: selection_objet(0); break;
      case  5: lecture_matiere(); break;
      case  6: lance_calcul(3,1); break;
      case  7: lance_calcul(1,1); break;
      case  8: plus_moins('+',1); break;
      case  9: plus_moins('-',1); break;
      case 10: bouton_deplacement(); break;
    }
  }
}

// --------------------------------------------------------------------------
// --- CHOIX PALETTE POUR RENDU RAPIDE --------------------------------------
// --------------------------------------------------------------------------
void palette_rendu_rapide(void) {
  int i;
  register int X1,Y1,X2,Y2;

  X1=CentX-75;
  X2=CentX+75;
  Y1=CentY-70;
  Y2=CentY+70;

  message("Choose palette for rendering");

  strcpy(StrBoite[0],"PALETTES S-VGA");
  g_fenetre(X1,Y1,X2,Y2,StrBoite[0],AFFICHE);

  init_pastille(10,X1+20,Y1+ 30,"64 niveaux de gris",(PalRenduRapide==1),"");
  affiche_pastille(10);
  init_pastille(11,X1+20,Y1+ 45,"8 bits 3-3-2 dithering",(PalRenduRapide==2),"");
  affiche_pastille(11);
  init_pastille(12,X1+20,Y1+ 60,"16 bits 64K",(PalRenduRapide==3),"");
  affiche_pastille(12);
  init_pastille(13,X1+20,Y1+ 75,"24 bits 16M",(PalRenduRapide==4),"");
  affiche_pastille(13);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(10,13);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) PalRenduRapide=(quelle_pastille_dans_groupe(10,13)-10)+1;
}

