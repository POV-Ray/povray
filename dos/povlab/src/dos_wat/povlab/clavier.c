/* ---------------------------------------------------------------------------
*  CLAVIER.C
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

#define F1          0x3B       /* Function keys              */
#define F2          0x3C
#define F3          0x3D
#define F4          0x3E
#define F5          0x3F
#define F6          0x40
#define F7          0x41
#define F8          0x42
#define F9          0x43
#define F10         0x44
#define F11         0x85
#define F12         0x86
#define CF1         0x5E       /* Ctrl-Function keys         */
#define CF2         0x5F
#define CF3         0x60
#define CF4         0x61
#define CF5         0x62
#define CF6         0x63
#define CF7         0x64
#define CF8         0x65
#define CF9         0x66
#define CF10        0x67
#define CF11        0x89
#define CF12        0x8A
#define SF1         0x54       /* Shift-Function keys        */
#define SF2         0x55
#define SF3         0x56
#define SF4         0x57
#define SF5         0x58
#define SF6         0x59
#define SF7         0x5A
#define SF8         0x5B
#define SF9         0x5C
#define SF10        0x5D
#define SF11        0x87
#define SF12        0x88
#define ALTF1       0x68       /* Alt-Function keys          */
#define ALTF2       0x69
#define ALTF3       0x6A
#define ALTF4       0x6B
#define ALTF5       0x6C
#define ALTF6       0x6D
#define ALTF7       0x6E
#define ALTF8       0x6F
#define ALTF9       0x70
#define ALTF10      0x71
#define ALTF11      0x8B
#define ALTF12      0x8C
#define INS         0x52       /* Numeric pad keys           */
#define DEL         0x53
#define HOME        0x47
#define END         0x4F
#define PGUP        0x49
#define PGDN        0x51
#define UP          0x48
#define DOWN        0x50
#define LEFT        0x4B
#define RIGHT       0x4D
#define PADMIDDLE   0x4C
#define PADEQ       0x3D
#define PADPLUS     0x22B
#define PADMINUS    0x22D
#define PADASTERISK 0x22A
#define PADSLASH    0x22F
#define CTRL1       0x175       /* Ctrl-Numeric pad keys      */
#define CTRL2       0x191
#define CTRL3       0x176
#define CTRL4       0x173
#define CTRL5       0x18F
#define CTRL6       0x174
#define CTRL7       0x177
#define CTRL8       0x18D
#define CTRL9       0x184
#define PADINS      0x252       /* Cursor pad keys            */
#define PADDEL      0x253
#define PADHOME     0x247
#define PADEND      0x24F
#define PADPGUP     0x249
#define PADPGDN     0x251
#define PADUPARROW  0x248
#define PADDNARROW  0x250
#define PADLTARROW  0x24B
#define PADRTARROW  0x24D
#define ALTA        30
#define ALTB        48
#define ALTC        46
#define ALTD        32
#define ALTE        18
#define ALTF        33
#define ALTG        34
#define ALTH        35
#define ALTI        23
#define ALTJ        36
#define ALTK        37
#define ALTL        38
#define ALTM        50
#define ALTN        49
#define ALTO        24
#define ALTP        25
#define ALTQ        16
#define ALTR        19
#define ALTS        31
#define ALTT        20
#define ALTU        22
#define ALTV        47
#define ALTW        17
#define ALTX        45
#define ALTY        21
#define ALTZ        44
#define ALT1        120
#define ALT2        121
#define ALT3        122
#define ALT4        123
#define ALT5        124
#define ALT6        125
#define ALT7        126
#define ALT8        127
#define ALT9        128
#define ALT0        129

#define ALT         8
#define SHIFT       3
#define CTRL        4
#define ESPACE      32

// -----------------------------------------------------------------------
// --------- AFFICHE UNE FENETRE D'AIDE POUR LE CLAVIER ------------------
// -----------------------------------------------------------------------
void affiche_aide_clavier(void) {
  x_fenetre(CentX,CentY,GAUCHE,1,"Help for shortkeys|"\
    "A : Redraw all viewports|"\
    "C : Re-center current viewport|"\
    "F : Toggle (no) full screen viewport|"\
    "G : Selection object|"\
    "M : Pan viewport|"\
    "O : Object attribute|"\
    "P : Extra texture parameters|"\
    "R : Redraw the current viewport|"\
    "O : Show object's dialog box|"\
    "S : Snap to current snap value|"\
    "T : Show texture's dialog box|"\
    "U : Unfreeze all unfrozen objects|"\
    "1/2/3 : Change view front/left/top in full screen|"\
    " |"\
    "+ : Zoom in current viewport|"\
    "- : Zoom out current viewport|"\
    "Space : Use selection|"\
    "Up/down : Move scene to up/down|"\
    "Left/right : Move scene to left/right|"\
    " |"\
    "Alt-A : Select all objects|"\
    "Alt-B : Box display|"\
    "Alt-C : Hide/show camera(s)");
    
    

  x_fenetre(CentX,CentY,GAUCHE,1,"Help for shortkeys|"\
    "Alt-D : Rotation object|"\
    "Alt-E : 2D scale object|"\
    "Alt-F : Fast display|"\
    "Alt-G : Zone selection with mouse|"\
    "Alt-H : Show this help screen|"\
    "Alt-J : Translation object|"\
    "Alt-L : Hide/show light(s)|"\
    "Alt-N : Normal display|"\
    "Alt-R : View last image|"\
    "Alt-S : Save the scene on disk|"\
    "Alt-T : 3D scale object|"\
    "Alt-U : Deselect all objects|"\
    "Alt-X : Quit POVLAB|"\
    "Alt-F1 : Align objects|"\
    "Alt-F4 : Quit POVLAB|"\
    " |"\
    "F1 : Shell to MS-DOS|"\
    "F2 : Run the pictures viewer|"\
    "F3 : Run the .pov viewer|"\
    "F4 : Run .TEX manual editor|"\
    "F5 : Run .INC manual editor|"\
    " |"\
    "Tab : Switch between cursor direction|"\
    "Del : Delete an object / selection");
}

// -----------------------------------------------------------------------
// --------- TEST SI ACTION PAR RACCOURCI CLAVIER ------------------------
// -----------------------------------------------------------------------
int test_entree_clavier(void) {
  register int Key=0,Ctrl=0,Flag=0,i;

  if (kbhit()) {
    Flag=test_touche();
    Ctrl=status_clavier();
    if (Flag==1) {
      Key=getch();
    } else {
      Key=Flag;
      if (Key>='a' && Key<='z') Key=Key & 223;
    }

    if (Flag!=1) { // -------------------------- touche simple
      switch (Key) {
        case ('R') : redessine_fenetre(NF,1); break;
        case ('A') : redessine_fenetre(5,1); break;
        case ('+') :
          plus_moins('+',1);
          break;
        case ('-') :
          plus_moins('-',1);
          break;
        case (ESPACE) :
          coche_case(0,!Cc[0].Croix);
          utilise_selection(0);
          break;
        case ('C') : bouton_recentre(); break;
        case ('G') : selection_objet(2); break;
        case ('M') : bouton_deplacement(); break;
        case ('O') : attributs_objet(); break;
        case ('P') : modif_parametre_texture(0); break;
        case ('S') : {
          Vid[NF].OptSnap=!Vid[NF].OptSnap;
          message("%s",Vid[NF].OptSnap ? "Snap enable":"Snap disable");
          break;
        }
        case ('T') : lecture_matiere(); break;
        case ('U') :
          for (i=1;i<=NbObjet;i++) {
            if (Objet[i]->Freeze) Objet[i]->Freeze=0;
            trace_volume_all(i,i);
          }
          break;
        case ('1') : bouton_fenetre('1'); break;
        case ('2') : bouton_fenetre('2'); break;
        case ('3') : bouton_fenetre('3'); break;
        case ('F') : bouton_fenetre(0); break;
      }
      return 1;
    }


    if (Ctrl & ALT) { // -------------------------- touche alt
      switch (Key) {
        case (ALTF4): return -2; break;
        case (ALTF1): alignement(); break;
        case (ALTA) : selection_objet(1); break;
        case (ALTB) : affiche_objet(2,1,NbObjet); redessine_fenetre(5,1); break;
        case (ALTC) : cache_camera(!Camera[0].Cache,1,NbCamera); break;
        case (ALTD) : rotation(0,0); break;
        case (ALTE) : deformation2D(0,0); break;
        case (ALTF) : affiche_objet(1,1,NbObjet); redessine_fenetre(5,1); break;
        case (ALTH) : affiche_aide_clavier(); break;
        case (ALTG) : selection_zone(); break;
        case (ALTI) : voir_image(1); break;
        case (ALTJ) : translation(0,0); break;
        case (ALTL) :
          cache_omni(!Omni[0].Cache,1,NbOmni);
          cache_area(!Area[0].Cache,1,NbArea);
          cache_spot(!Spot[0].Cache,1,NbSpot);
          cache_cyllight(!CylLight[0].Cache,1,NbCylLight);
          break;
        case (ALTN) : affiche_objet(0,1,NbObjet); redessine_fenetre(5,1); break;
        case (ALTR) : voir_image(0); break;
        case (ALTU) : selection_objet(0); break;
        case (ALTS) : sauve_fichier(0,0); break;
        case (ALTT) : deformation3D(0,0); break;
        case (ALTX) : return -2; break;
      }
      return 1;
    }

    

    if (Flag==1) { // -------------------------- touche ‚tendue
      switch (Key) {
        case (UP)    : Vid[NF].Depla.Y--; redessine_fenetre(NF,1); break;
        case (DOWN)  : Vid[NF].Depla.Y++; redessine_fenetre(NF,1); break;
        case (LEFT)  : Vid[NF].Depla.X--; redessine_fenetre(NF,1); break;
        case (RIGHT) : Vid[NF].Depla.X++; redessine_fenetre(NF,1); break;
        case (F1)    : appel_programme_externe(0); break;
        case (F2)    : appel_programme_externe(1); break;
        case (F3)    : appel_programme_externe(2); break;
        case (F4)    : appel_programme_externe(3); break;
        case (F5)    : appel_programme_externe(4); break;
        case (DEL)   : supprime_objet(); break;
      }
      return 1;
    }
  }

  return 0;
}
