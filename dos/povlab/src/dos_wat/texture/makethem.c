
/***************************************************************/
/*                                                             */
/* MAKETHEM.C  - Copyright Denis Olivier for POVLAB.           */
/*               Source code provide as is.                    */
/*               Freeware.                                     */
/*                                                             */
/***************************************************************/
/*                                                             */
/* Version 1.2 - Updated on 15/01/97 by Juan R.Migoya          */
/*             - Updated  allow the use of                     */
/*               differents HD units and to modify the         */
/*               TEXTURE.POV file creation ("hollow"           */
/*               missed).                                      */
/*                                                             */
/* Version 1.3 - Updated on 21/05/97 by Juan R.Migoya          */
/*             - allow larger directory                        */
/*               names to be used. -                           */
/*             - Also added -l option for library              */
/*                                                             */
/* Version 1.4 - Updated on 11/07/97 by Denis Olivier.         */
/*             - Allow the output                              */
/*               of antialiasing amount defined by user.       */
/*             - Removed French code.                          */
/*             - Added multi objects support.                  */
/*             - Clearing code.                                */
/*             - Added prompt for parameters before render.    */
/*             - Removed Faster Than POV-Ray code.             */
/*                                                             */
/***************************************************************/

#if __BORLANDC__
#include <DIR.H>
#include <CTYPE.H>
#endif

#include <DOS.H>
#include <IO.H>
#include <CONIO.H>
#include <STDIO.H>
#include <DIRECT.H>
#include <PROCESS.H>
#include <STDARG.H>
#include <STRING.H>
#include <STDLIB.H>

#define NULL 0   /* Juan R. Migoya: To avoid MS compiler warnings */

#define byte unsigned char
#define ARGU_MAX 30

char NomLogiciel[]="MAKE THEM ALL";
char VerLogiciel[]="1.4";

char Argu[ARGU_MAX][256];
char PathTEX[81]={NULL};
char Path_to_TEX[81]={NULL}; /* Juan R. Migoya: Added only-path of TEX file */
char Path_for_POVLAB[81]={NULL}; /* Juan R. Migoya: Path accesible for POVLAB */
char PathINC[81]={NULL};
char PathPOV[81]={NULL};
char PathTGA[81]={NULL};
char PathRDR[81]={NULL};
char PathMakeLOG[81]={NULL};  /* Juan R. Migoya: Full path to makethem.log,
                                 now in the same place that the outputs files */
char Temp[81]={NULL};         /* Juan R. Migoya: Temporal buffer to build up the
                                 output file name                             */
char PathLIB[81]={NULL};      /* Juan R. Migoya 21/05/97. Used for -L option  */
int Object=0;                 // Denis Olivier 11/07/97. Used for -O option

// -----------------------------------------------------------------------
// -- WRITE AN ERROR AND EXIT --------------------------------------------
// -----------------------------------------------------------------------
void exit_error(char *String,...) {
  va_list parametre;
  char Sortie[80];

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  printf("%s",Sortie);
  exit(1);
}

// -----------------------------------------------------------------------
// -- WRITE TEXTURE FILE -------------------------------------------------
// -----------------------------------------------------------------------
void write_pov_file(char *Name,char *Inc) {
  FILE *F;

  if (!(F=fopen("TEXTURE.POV","w+t"))) exit_error("Can write file TEXTURE.POV");
  fprintf(F,"// TEXTURE FILE FOR POVLAB\n"\
   "// Generated for MAKETHEM version %s\n"\
   "// Create with this file a 100x100 tile.\n"\
   "\n"\
   "#include \"%s\"\n"\
   "\n"\
   "camera {\n"\
   "  location <0, 0, -6.5>\n"\
   "  direction <0, 0, 2>\n"\
   "  right <1, 0, 0>\n"\
   "  look_at <0, 0, 0>\n"\
   "}\n"\
   "\n"\
   "object { light_source { <30, 30, -30>  color red 1.0 green 1.0 blue 1.0 }}\n"\
   "\n\n",VerLogiciel,Inc);

  switch(Object) {
    case 1:
      fprintf(F,"cylinder { <0,0.8,0>,<0,-1,0>,0.9\n");
      break;
    case 2:
      fprintf(F,"cone { <0,1,0>,0,<0,-1,0>,1\n");
      break;
    case 3:
      fprintf(F,"difference {\n");
      fprintf(F,"  sphere { <0, 0, 0>, 1 }\n");
      fprintf(F,"  box { <0, 1.1, 0>,<1.1,-1.1,-1.1> }\n");
      break;
    default:
      fprintf(F,"sphere { <0, 0, 0>, 1\n");
      break;
  }
   
  fprintf(F,"  texture {\n"\
   "    //  ------- Here your texture, defined in POVLAB.INC or other\n"\
   "    %s\n"\
   "    // ----------------------------------------------------------\n"\
   "  }\n"\
   "  hollow\n"\
   "  translate <0,0,-1.5>\n"\
   "}\n"\
   "\n"\
   "box { <-2.5,-1.001,-3>, <2.5,-1,0>\n"\
   "  texture {\n"\
   "    pigment { checker color rgb <1,1,1> color rgb <0,0,0> scale <0.51 0.51 0.51> } \n"\
   "    finish { phong 1.0 ambient 0.6 diffuse 0.7 }\n"\
   "  }\n"\
   "}\n"\
   "\n"\
   "box { <-2.5,-1,0>, <2.5,3,3>\n"\
   "  texture {\n"\
   "    pigment { checker color rgb <1,1,1> color rgb <0,0,0> scale <0.51 0.51 0.51> } \n"\
   "    finish { phong 1.0 ambient 0.6 diffuse 0.7 }\n"\
   "  }\n"\
   "}\n",Name);

   
  fclose(F);

}


// -----------------------------------------------------------------------
// -- TEST SI UN FICHIER EXISTE OU NON -----------------------------------
// -----------------------------------------------------------------------
byte test_fichier(char *Chemin) {
  if (access(Chemin,0)!=0) return 0;
  return 1;
}

// -----------------------------------------------------------------------
// -- RETOURNE LA NIEME POSITION DE CHAINE2 DANS CHAINE1 -----------------
// -----------------------------------------------------------------------
long strinstr(long decalage,char *ch1,char *ch2) {
  register long i=0,j,k=0;

  if (decalage<0) decalage=0;
  if (strlen(ch2)<=0) return -2;

  for (i=decalage;ch1[i]!=NULL;i++) {
     k=0;j=i;
     while (ch1[i]==ch2[k]) {
       i++;k++;
       if (k==strlen(ch2)) return j;
     }
  }
  return -1;
}

// -------------------------------------------------------------------------
// -- ANALYSE UNE LIGNE D'UN FICHIER ---------------------------------------
// -------------------------------------------------------------------------
byte analyse_ligne (char *TempChar,byte Separateur) {
  register int k,i;
  char Marque[2];
  char *Pointeur;

  Marque[0]=Separateur;
  Marque[1]=NULL;

  for (k=0;k<ARGU_MAX;k++) Argu[k][0]=NULL;
  k=0;

  Pointeur=strtok(TempChar,Marque);

  if (Pointeur[0]=='\n') return 0;

  while (Pointeur) {
    strcpy(Argu[k],Pointeur);
    i=strinstr(0,Argu[k],"\n");
    if (i) Argu[k][i]=NULL;
    Pointeur=strtok(NULL,Marque);
    k++;
  }

  return k;
}

// -----------------------------------------------------------------------
// ------------- MESSAGE COPYRIGHT DU LOGICIEL ---------------------------
// -----------------------------------------------------------------------
void message_dos(void) {
  puts("");
  printf("%s release %s, (C) Copyright ChromaGraphics, 1994-1997.\n",NomLogiciel,VerLogiciel);
  printf("Texture library maker and manager for POVLAB.\n");
  printf("All rights reserved, (R) Denis Olivier & Juan R. Migoya - %s.\n",__DATE__);

  puts("");
}

// -----------------------------------------------------------------------
// ------------- MESSAGE COPYRIGHT DU LOGICIEL ---------------------------
// -----------------------------------------------------------------------
void usage(void) {
  puts("    Syntax : makethem [option(s)]");
  puts("");
  puts("    -Ppath : specifie path for POV-Ray (default current)");
  puts("    -Ipath : specifie file library.TEX (default POVLAB.TEX)");
  puts("    -D     : display while rendering (+d0)");
  puts("    -An.n  : use antialiasing rendering option (-A0.3 for example)");
  puts("    -T     : render all the textures int library.TEX (overwrite)");
  puts("    -Lpath : specifie library for map files (optional). If path is");
  puts("           : omitted, the path indicated with -I option is used");
  puts("    -On    : use object described as bellow (default sphere)");
  puts("             considering n : 1=cylinder");
  puts("                             2=cone");
  puts("                             3=cut sphere");
  puts("    -?     : syntax and help screen");
  puts("");
  puts("    Example : makethem -pc:\\povray -ic:\\povlab\\povlab.tex");

  exit_error("");
}

// -----------------------------------------------------------------------
// ------------- Juan R. Migoya:               ---------------------------
// ------------- RETRIEVE PATH OF TEX FILE     ---------------------------
// Args:
//   <full_path>: full_path (also filename) of TEX file as given in
//                the command line. <full_path> will remain unchanged.
//   <path>: pointer to a buffer to hold the path (without filename) of
//         the TEX file.
//   <path_for_povlab>: Pointer to a buffer to hold a valid path for POVLAB,
//         i.e: this will be the path written in MAKETHEM.LOG.
//         Note that MAKETHEM must be runnig in the TEXTURE directory
//         ( users must be adviced about this). So, the path in the command
//         line must be valid "as is" for TGAVGIF and for POVRAY, or MAKETHEM
//         will issue an error message because it can't open the TEX file. But
//         for POVLAB it's different because the TEX file directory may be
//         below the TEXTURE directory. In this case, it will be available
//         "as is" for TGAVGIF and POVRAY but not for POVLAB.
//
//         Note that <path> and <path_for_povlab> will end with  a '\' character.
//
//         This function has been severely modified on 27/02/97 to
//         allow the use of diferents HD units
//
// -----------------------------------------------------------------------
void getTexPath( char *full_path, char *path, char* path_for_povlab)
{
  int i, j , k;
  char drive[3];

  path[0]=0;
  path_for_povlab[0]=0;
  if ( !full_path[0] ) return; /* Error. Do nothing  */

  /* Get the path without filename */

  k = strlen(full_path) ;
  j = -1;

  /* Find the last '\' if any */
  for(i=0; i<k; i++)
     if (full_path[i]=='\\') j=i;

  if ( j != -1 ) {
     strcpy( path, full_path);
     path[j+1]=0;   /* Keep the '\'  */
     }
  else {
     /* --------- First block: There is not '\' character in the path.  */
     /* Test for drive letter:                                          */
     if( full_path[1]==':') {
        /* Test if it is the current drive:                             */
        if( toupper(full_path[0])==(_getdrive() + 'A' - 1)) {
           /* There is only filename and it is in the current directory */
           /* of current drive:                                         */
           /* <path> is empty and <path_for_povlab> need a "TEXTURE\"   */
           /*  string inserted.                                         */
           strcpy(path_for_povlab, "TEXTURE\\");
           return;
           }
        else {
           /* There is only filename but in another drive: get the full */
           /* path in this drive:                                       */
           drive[0]=full_path[0];
           drive[1]=':';
           drive[2]=0;
           /* Get the current path of this drive                       */
           _fullpath( path_for_povlab, drive, _MAX_PATH );
           /* If is not the root directory add a '\'                   */
           if( strlen(path_for_povlab) > 3)   /* i.e. not "D:\"        */
                    strcat(path_for_povlab, "\\");
           /* And set up <path>                                        */
           strcpy( path, drive);
           return;
           }
        }
     else {
        /* There is only filename. <path> is empty and <path_for_povlab> */
        /* is "TEXTURE\".                                                */
        strcpy(path_for_povlab, "TEXTURE\\");
        return;
        }
     }

  /* ---------- Second block: There is '\' character in the path ----- */
  /* Test for drive letter in the path                                 */
  if( path[1]==':') {
     /* If it is the current drive.                                    */
     if( toupper(full_path[0])==(_getdrive() + 'A' - 1)) {
        if( path[2]=='\\') {
           /* Full path provided.                                      */
           strcpy(path_for_povlab, &path[2]);
           return;
           }
        else {
           /* Add "TEXTURE\" to <path_for_povlab>                      */
           strcpy(path_for_povlab, "TEXTURE\\");
           strcat(path_for_povlab, &path[2]);
           return;
           }
        }
     else {
        /* It isn't the current drive:                                 */
        /* If <path> has '\' at the beginning, this is also the path   */
        /* for POVLAB:                                                 */
        if (path[2]=='\\') {
           strcpy(path_for_povlab, path);
           return;
           }
        /* If not, Check for the current path in this unit             */
        drive[0]=full_path[0];
        drive[1]=':';
        drive[2]=0;
        _fullpath( path_for_povlab, drive, _MAX_PATH );
        /* If it is not the root directory add a '\'                   */
        if( strlen(path_for_povlab) > 3)   /* i.e. not "D:\"           */
           strcat(path_for_povlab, "\\");
        /* And add the <path>                                          */
        strcat( path_for_povlab, &path[2]);
        return;
        }
     }
  else {
     /* There is not drive letter.                                    */
     if( path[0]=='\\') {
        /* Full path to povlab already provided.                      */
        strcpy(path_for_povlab, path);
        return;
        }
     else {
        /* Added on 21/05/97, Juan R. Migoya: The user provided path  */
        /* may be in the form ..\, so we must check for it.           */
        if ( strstr( path, "..\\") ){
           /* The user path is just below the POVLAB directory        */
           strcpy( path_for_povlab, &path[3]);   /* "\" also eliminated */
           return;
           }
        /* Just add "TEXTURE\"                                        */
        strcpy(path_for_povlab, "TEXTURE\\");
        strcat(path_for_povlab, path);
        return;
        }
     }
}

// -----------------------------------------------------------------------
// -- MAIN OF THE PROGRAM ------------------------------------------------
// -----------------------------------------------------------------------
void main(int argc,char *argv[]) {
  int i,j;
  int k=0;       /* Juan R. Migoya: Added for not overwrite recently created */
                 /* files                                                    */

  /* Juan R. Migoya: Size of TGA an GIF changed from 15 to 81 */
  char TGA[81]={NULL};
  char GIF[81]={NULL};

  char Buffer[512];
  FILE *F1,*F2, *F3;
  int Count=0;
  byte Display=0;
  byte Alias=0;
  char Amount[5];
  byte All=0;

  byte Lib=0; /* Added 21/05/97                                      */
  strcpy(PathTEX,"POVLAB.TEX");
  Path_to_TEX[0]= NULL;    /* Juan R. Migoya */
  strcpy(PathINC,"POVLAB.INC");
  strcpy(PathPOV,"POVRAY.EXE");
  
  message_dos();

  if (argc<2) usage();

  for (i=1;i<=argc;i++) {
    if (argv[i][0]=='/' || argv[i][0]=='-') {
      strupr(argv[i]);
      switch(argv[i][1]) {
        case 'P':
          sprintf(PathPOV,"%s\\POVRAY.EXE",argv[i]+2);
          break;

        case 'I':
              strcpy(PathTEX,argv[i]+2);
              /* Juan R. Migoya: Get path of TEX file               */
              getTexPath( PathTEX, Path_to_TEX, Path_for_POVLAB);
           break;

        case 'D': Display=1; break;
        case 'O':
           Object=argv[i][2]-48;
           break;
        case 'A':
           Alias=1;
           strcpy(Amount,argv[i]+2);
           if (!atof(Amount)) strcpy(Amount,"0.3");
           break;

        case '?': usage(); break;
        case 'T': All=1; break;
        case 'L':   /* Juan R. Migoya. Added in version 1.3 */
           Lib=1;
           if ( argv[i][2] ) strcpy( PathLIB, &argv[i][2]);
           break;

        /* Juan R. Migoya: Bad Parameters not allowed */
        default:
           if(argv[i][1]) argv[i][2]=NULL;
              exit_error("Unknown option: \"%s\"\n", argv[i]);

      }
    }
  }

  printf("Rendering thumbnails using parameters :\n\n");
  printf("- Set antialiasing %s ",Alias ? "on":"off");
  if (Alias) printf("using amount %s\n",Amount); else puts("");
  printf("- Using object ");
  switch (Object) {
    case 1:  printf("cylinder\n"); break;
    case 2:  printf("cone\n"); break;
    case 3:  printf("cut sphere\n"); break;
    default: printf("sphere\n"); break;
  }

  printf("- Path to POV-Ray : %s\n",PathPOV);
  printf("- Path to texture file : %s\n",PathTEX);
  if (Lib) printf("- Path to library file : %s\n",PathLIB);

  if (All) {
    printf("- Render all the textures (overwrite)\n");
  } else {
    printf("- Render only missing textures\n");
  }

  if (Display) {
    printf("- Displaying tracing during render\n");
  } else {
    printf("- Do not displaying tracing during render\n");
  }

  printf("\nHit ENTER to render, ESC to abort ");
  if (getch()==27) exit_error("\nEnded.");

  /* Juan R. Migoya: Changed to give the adecuate error message */

  if (!test_fichier(PathPOV)) exit_error("%s not found!\n", PathPOV);


  /* Juan R. Migoya: Added to put MAKETHEM.LOG in the same directory that
     the TEX file and the output files                                    */

  strcpy(PathMakeLOG, Path_to_TEX);
  strcat(PathMakeLOG, "MAKETHEM.LOG");


  /* Juan R. Migoya: remove MAKETHEM.LOG from the output path */
  remove(PathMakeLOG);


  /* Juan R. Migoya: In the following if, PathINC has been replaced by PathTEX */
  /* if (!(F1=fopen(PathTEX,"r+t"))) {
        exit_error("Can't open file %s",PathINC);

     }
  */

  if (!(F1=fopen(PathTEX,"r+t"))) {
    exit_error("Can't open file %s",PathTEX);
  }

     if (!(F2=fopen(PathMakeLOG,"w+t"))) exit_error("Can't open log file MAKETHEM");


  /* Treat the -L option if present                                          */
  if( Lib ) {
     if ( !PathLIB[0]) {
        /* Default                                                           */
        strcpy( PathLIB, Path_to_TEX);
        /* But throw away the "\" terminator if exist                        */
        if( PathLIB[strlen(PathLIB)-1]=='\\' ) PathLIB[strlen(PathLIB)-1]=0;
        }
     }

  Buffer[0]=PathPOV[0];
  fgets(Buffer,256,F1);

  Buffer[strinstr(0,Buffer,"\n")]=NULL;
  if (!test_fichier(Buffer)) {
     /* Juan R. Migoya: Changed to give correct error message */
     /* usage();                                               */
     exit_error("Can't find include file: %s!\n", Buffer);
     } else {

     printf("Include file : <%s>\n",Buffer);

     strcpy(PathINC,Buffer);
     fprintf(F2,"%s\n",PathINC);
     }


  puts("");

  while (1) {
     fgets(Buffer,256,F1);
     if (feof(F1)) break;
     if (kbhit()) if (getch()==27) break;

     /* Juan R. Migoya: Write also the titles in MAKETHEM.LOG:   */
     if( Buffer[0]=='#') {
        fprintf(F2, "%s", Buffer);
        continue;
        }
     /* Juan R. Migoya: condition in previous "if" has been eliminated */
     /* from the following "if":                                       */
     if (Buffer[0]!=32 && Buffer[0]!=NULL && Buffer[0]!='\n' ) {
        analyse_ligne(Buffer,32);
        /* Juan R. Migoya, 21/05/97. The following lines has been      */
        /* changed. Previous version assumed that there could be just  */
        /* one "\" in Argu[1]. Doing so, test_fichier(Argu[1]) was     */
        /* always wrong.                                               */

        /* Quote */
        /* i=strinstr(0,Argu[1],"\\");
           if (i>=0) {
           memmove(Argu[1],Argu[1]+i+1,strlen(Argu[1])-i+1);
              } End of quote. */

        /* Not neccesary to put a filename in .TEX file                */
        if( Argu[1][0]) {
           j=0;
           /* Found the last "\" if any                                */
           while( (i=strinstr(j, Argu[1], "\\")) != -1 ) j=i+1;
           /* Found?                                                   */
           if( j>0 ) strcpy( Argu[1], &(Argu[1][j]) ); /* Skip the "\" */
           /* If not found don't change Argu[1]                        */
           }
        /* If not name provided, use TEX0999.GIF                       */
        else  strcpy(Argu[1], "TEX0999.GIF");

        /* End of change */

        printf("\r[%03d] %s -- %s",Count++,Argu[0],Argu[1]);

        /* Juan R. Migoya: Now put the full new path: */
        strcpy(Temp, Path_to_TEX);
        strcat(Temp, Argu[1]);
        strcpy(Argu[1], Temp);

        /* Juan R. Migoya: the following line      */
        /* if (!test_fichier(Argu[1]) && !All) {   */
        /* has been replaced by:                   */

        if ( (!test_fichier(Argu[1])) || All) {

           printf("-- will be rendered...\n");

           write_pov_file(Argu[0],PathINC);
           /* changed for(i=0;... to for(i=k;...       */
           for (i=k;i<=999;i++) {
                 /* Juan R. Migoya: Path_to_TEX added */
                 sprintf(TGA,"%sTEX%04d.TGA",Path_to_TEX, i);
                 sprintf(GIF,"%sTEX%04d.GIF",Path_to_TEX, i);


              /* Juan R. Migoya: No change, just a comment:               */
              /* The next "if" is intended to find the first filename     */
              /* not yet used. So this is not redundant with the previous */
              /* "if": note the "for..." loop. When the user uses a file- */
              /* name that doesn't exist, the program doesn't necessarily */
              /* use it: it checks before for free filenames with smaller */
              /* number.                                                  */
              /* What it's not clear is why it checks also for TGA files. */
              /* It may be for the case that TGAVGIF might not works. In  */
              /* this way, MAKETHEM doesn't overwrite the TGA file        */
              /* created by POVRAY just before.                           */

              if ((test_fichier(TGA)==0 && test_fichier(GIF)==0) || All) {
                    printf("  Image name : %s\n",TGA);

                    /* Juan R. Migoya: No change, just a comment:               */
                    /* Note that now the output file will be in the same        */
                    /* directory that the TEX file. The same for the OUT file.  */
                    /* Version 1.3: Now we use a INI file for the commands: The */
                    /* Spawn function only alows 128 characters, limiting the   */
                    /* directories names                                        */

                    /* Current directory. We'll build the INI file here         */
                    /* and will pass this string to POVRAY                      */
                    #if __BORLANDC__
                    getcwd(Buffer, _MAX_PATH);
                    #else
                    _getcwd(Buffer, _MAX_PATH);
                    #endif
                    if (Buffer[strlen(Buffer)] != '\\' ) strcat(Buffer, "\\");
                    strcat(Buffer,"POVRAY.INI");

                    if (!(F3=fopen(Buffer,"w+t"))) exit_error("Can write file POVRAY.INI");
                    fprintf(F3,"-iTEXTURE.POV -o%s %s%s \n+b50 +ft -p \n+w100 +h100 \n+mb1 \n+x %s \n+ga%sTEX%04d.OUT %s%s %s \n+ul +uv",
                               TGA,
                               Lib ? "-L":" ",
                               Lib ? PathLIB:" ",
                               Display ? "+d0 -v":"-d +v",
                               Path_to_TEX,
                               i,
                               Alias ? "+a":" ",
                               Alias ? Amount:""); // ***  DO 11/07/97
                                                  
                    fclose(F3);
                    printf("  Access : %s\n",PathPOV);
                    spawnl(P_WAIT,PathPOV,PathPOV,Buffer,NULL);
                 k=i+1;  /* Juan R. Migoya */


                    /* Juan R. Migoya: Path_to_TEX added                */
                    /* TGAVGIF will use the same output path by default.*/
                    sprintf(Buffer,"%sTEX%04d /f /i",Path_to_TEX,i);

                 if (spawnlp(P_WAIT,"TGAVGIF.EXE","TGAVGIF.EXE",Buffer,NULL)>-1) {
                    if (test_fichier(GIF)) {
                       sprintf(Temp,"%sTEX%04d.GIF", Path_for_POVLAB, i);
                       fprintf(F2,"%-30s %s\n",Argu[0],Temp);
                    } else {
                       fprintf(F2,"%-30s ---> Error !\n",Argu[0]);
                    }
                    break;
                  } else {
                     exit_error("Can't find TGAVGIF.EXE in the path.");
                  }
                }
              }  /* End of for... */
            } else {
                 printf(" -- already rendered.\n");

              }
         }
     }

  fclose(F2);
  fclose(F1);

    /* Juan R. Migoya: Test for MAKETHEM.LOG in the right path */
    if (test_fichier(PathMakeLOG)) printf("\nMAKETHEM.LOG created.\n");


}

