/* ---------------------------------------------------------------------------
*  LIB.C
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
#include <BIOS.H>
#include <CONIO.H>
#include <CTYPE.H>
#include <DIRECT.H>
#include <DOS.H>
#include <FLOAT.H>
#include <I86.H>
#include <IO.H>
#include <LIMITS.H>
#include <MALLOC.H>
#include <MATH.H>
#include <MEM.H>
#include <SIGNAL.H>
#include <STDARG.H>
#include <STDARG.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include "GLIB.H"
#include "VERSIONC.H"
#include "LIB.H"
#include "GLOBAL.H"

union REGS regs;
#define Couleur(t,f) ((f<<4)+t)

char Caractere;
char Sortie           [256];
char NewChemin        [MAXPATH];
char OldChemin        [MAXPATH];
char OldLecteur       [4];
char NewLecteur       [4];
char Titre_Fenetre    [80];
char NomLogiciel      [30];
char VerLogiciel      [30];
char NomUtilisateur   [64];
char Arg              [4][64];

byte LIGHT;
byte Video;
byte DiskError=0;

int PageY;
int NbArg;
int VidSeg=0xB800;

long NSerieLogiciel=0;
long MemoireLibre=0L;
long MemoirePrise=0L;

// --------------------------------------------------------------------------
// -- RECUPERE LES ARGUMEMTS DE LA LIGNE DE COMMANDE ------------------------
// --------------------------------------------------------------------------
void init_arg (int Nb,char *ArgTmp[],byte New) {
  register int i;

  switch (New) {
    case 0:
      break;
    case 1:
      NbArg=Nb;
      for (i=0;i<=NbArg;i++) {
        strcpy(Arg[i],ArgTmp[i]);
      }
      break;
  }
}

// -----------------------------------------------------------------------
// -- RETOURNE L'ENTIER D'UN NOMBRE BINAIRE 16 BITS ----------------------
// -----------------------------------------------------------------------
unsigned int bin(char *Binaire) {
  register unsigned int i,Entier=0;

  for (i=0;i<=15;i++) {
    Entier+=ldexp((Binaire[i]=='1' ? 2:0),14-i);
  }

  return Entier;
}

// -----------------------------------------------------------------------
// -- EXECUTION D'UN BEEP ------------------------------------------------
// -----------------------------------------------------------------------
void beep(void) {
  if (!OptBeep) return;
  sound(1850);
  delay(30);
  sound(1500);
  delay(30);
  nosound();
}

// -----------------------------------------------------------------------
// ------------ TESTE L'ETAT DES TOUCHE DE CTRL DU CLAVIER ---------------
// -----------------------------------------------------------------------
void efface_buffer_clavier (void) { while (kbhit()) getch(); }

// ----------------------------------------------------------------------- */
// ---- RENVOI LES PARAMETRES ACTUELS DES BOUTONS DE LA SOURIS ----------- */
// ----------------------------------------------------------------------- */
int MouseB (void) {
  if (BitMouse==2) return 0;
  #if WINDOWS
    regs.w.ax=0x0003;
    int86(0x33,&regs,&regs);
    return regs.w.bx;
  #else
    regs.w.ax=0x0003;
    int386(0x33,&regs,&regs);
    return regs.w.bx;
  #endif
}

/* ---------------------------------------------------------------------- */
/* ------------- D‚finition de la forme du curseur ---------------------- */
/* ---------------------------------------------------------------------- */
void init_curseur (byte Debut,byte Fin) {
  #if WINDOWS
    regs.w.ax=0x0100;
    regs.h.ch=Debut;
    regs.h.cl=Fin;
    int86(0x10,&regs,&regs);
  #else
    regs.w.ax=0x0100;
    regs.h.ch=Debut;
    regs.h.cl=Fin;
    int386(0x10,&regs,&regs);
  #endif
}

/* ----------------------------------------------------------------------- */
/* -- RESTAURE CHEMIN DE DEPART ------------------------------------------ */
/* ----------------------------------------------------------------------- */
void RestaureChemin(void) {
  _dos_setdrive(OldLecteur[0]-64,0);
  chdir(OldChemin);
}

// -----------------------------------------------------------------------
// -- RETOURNE LA NIEME POSITION DE CHAINE2 DANS CHAINE1 -----------------
// -----------------------------------------------------------------------
long strinstr(long decalage,char *ch1,char *ch2) {
  register long i=0,j,k=0;

  if (decalage<0) decalage=0;
  if (strlen(ch2)<=0) return -2;

  for (i=decalage;ch1[i]!=NULLC;i++) {
    k=0;j=i;
    while (ch1[i]==ch2[k]) {
      i++;k++;
      if (k==strlen(ch2)) return j;
    }
  }

  return -1;
}

// -------------------------------------------------------------------------
// ----------------------- RETOURNE L'ESPACE DISQUE DISPONIBLE -------------
// -------------------------------------------------------------------------
long diskspace(byte Drive) {
  long avail;
  int drive;

  struct diskfree_t free;

  drive=toupper(Drive)-65;
  _dos_getdiskfree(drive+1,&free);
  if (free.sectors_per_cluster==0xFFFF) return 0;

  avail=(long) free.avail_clusters *
        (long) free.bytes_per_sector *
        (long) free.sectors_per_cluster;

  return avail;
}

// -----------------------------------------------------------------------
// ---------------- DESACTIVATION DE LA SOURIS : OFF ---------------------
// -----------------------------------------------------------------------
void MouseOff(void) {
  if (BitMouse==2) return;
  #if !WINDOWS
  if (BitMouse==1) {
    // ------------------- D‚sactive scanning de la souris
    regs.w.ax=0x001C;
    regs.w.bx=1;
    int386(0x33,&regs,&regs);
    // ------------------- Cache souris
    regs.w.ax=0x0002;
    int386(0x33,&regs,&regs);
    BitMouse=0;
  }
  #endif
}

/* ----------------------------------------------------------------------- */
/* -- INIT_SOFT ---------------------------------------------------------- */
/* ----------------------------------------------------------------------- */
void init_soft (char *Nom,char *Ver) {
  strcpy(VerLogiciel,Ver);
  strcpy(NomLogiciel,Nom);
}

/* ----------------------------------------------------------------------- */
/* ------------------- CALCULE UN RATIO (POURCENTAGE) -------------------- */
/* ----------------------------------------------------------------------- */
/*
unsigned int ratio(unsigned long a,unsigned long b) {
  // [(1000a + [b/2]) / b]
  int i;

  for (i = 0; i < 3; i++)
      if (a <= ULONG_MAX / 10) a *= 10;  else b /= 10;
  if ((unsigned long)(a + (b >> 1)) < a) {  a >>= 1;  b >>= 1;  }
  if (b == 0) return 0;
  return (unsigned int)((a + (b >> 1)) / b);
}
*/

/* -------------------------------------------------------------------------- */
/* --- TEST SI TOUCHE ETENDUE OU NON ---------------------------------------- */
/* -------------------------------------------------------------------------- */
byte test_touche (void) {
  byte Key;
  if ((Key=getch())==0) return 1; else return Key;
}

// --------------------------------------------------------------------------
// --- INITIALISE LES ENVIRONNEMENTS DISQUE AU DEMARRAGE --------------------
// --------------------------------------------------------------------------
void init_disk(char *Chemin) {
  char Nom[14];
  char Ext[4];
  char Path[MAXPATH];
  int AX;

  // ---------------------- R‚cupŠre screen et coordonn‚es Y vid‚o -------

  PageY=where_y();

  // ----------- R‚cupŠre mode vid‚o (0=mono,1=couleur)-------------------

  #if !WINDOWS
    regs.w.ax=0x0011;
    int386(0x11,&regs,&regs);
    AX=regs.w.ax;
  #endif

  #if !WINDOWS
    if ((AX & 48)==48) {
      Video=0;
      VidSeg=0xB000;
    } else {
      Video=1;
      VidSeg=0xB800;
    }
  #endif

  /* ----------------- Supprime tous les caractŠres buffer keyboard ------ */

  efface_buffer_clavier();  

  /* --------- Analyse le chemin, r‚cupŠre ancien lecteur/chemin ------- */
  
  strupr(Chemin);
  getcwd(Path,MAXPATH);
  strcat(Path,"\\");
  _splitpath(Path,OldLecteur,OldChemin,Nom,Ext);

  /* --------- Analyse le chemin, r‚cupŠre nouveau lecteur/chemin ------- */

  if (OldChemin[strlen(OldChemin)-1]=='\\') OldChemin[strlen(OldChemin)-1]=NULLC;
  strcpy(NewChemin,OldChemin);
  strcpy(NewLecteur,OldLecteur);

  set_disk(NewLecteur[0]-65);
  chdir(NewChemin);
}

/* ----------------------------------------------------------------------- */
/* -- RETOURNE L'HEURE --------------------------------------------------- */
/* ----------------------------------------------------------------------- */
void get_heure(char *Heure) {
  regs.h.ah=0x2C;
  #if !WINDOWS
    int386(0x21,&regs,&regs);
  #else
    int86(0x21,&regs,&regs);
  #endif
  sprintf(Heure,"%02d:%02d:%02d",regs.h.ch,regs.h.cl,regs.h.dh);
}

/* ----------------------------------------------------------------------- */
/* -- RETOURNE LA DATE --------------------------------------------------- */
/* ----------------------------------------------------------------------- */
void get_date(char *Date) {
  int CX;
  char *Jour[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
  regs.h.ah=0x2A;
  #if !WINDOWS
    int386(0x21,&regs,&regs);
    CX=regs.w.cx;
  #else
    int86(0x21,&regs,&regs);
    CX=regs.w.cx;
  #endif
  sprintf(Date,"%s %02d-%02d-%02d",Jour[regs.h.al],regs.h.dh,regs.h.dl,CX);
}

/* ----------------------------------------------------------------------- */
/* --- RETOURNE SI SPECIFICATION * et ? CORRESPOND AU NOM DE FICHIER ----- */
/* ----------------------------------------------------------------------- */
int match(char *s1, char *s2) {
  for ( ; ; ) {
    while (*s2=='*' || *s2=='?') {
      if (*s2++=='*')
        while (*s1 && *s1!=*s2) s1++;
      else if (*s1==0)
        return 0;
      else s1++;
    }
    if (*s1!=*s2) return 0;
    if (*s1==0  ) return 1;
    s1++;s2++;
  }
}

/* ----------------------------------------------------------------------- */
/* -- RETOURNE L'HEURE --------------------------------------------------- */
/* ----------------------------------------------------------------------- */
long init_temps(void) {
  regs.h.ah=0x2C;
  #if !WINDOWS
    int386(0x21,&regs,&regs);
  #else
    int86(0x21,&regs,&regs);
  #endif
  return (long) (regs.h.ch*360000+regs.h.cl*6000+regs.h.dh*100+regs.h.dl);
}

/* ----------------------------------------------------------------------- */
/* -- SON ERREUR --------------------------------------------------------- */
/* ----------------------------------------------------------------------- */
void beep_erreur (void) {
  if (!OptBeep) return;
  sound(50);
  delay(150);
  nosound();
}

// -----------------------------------------------------------------------
// ------------ TESTE L'ETAT DES TOUCHE DE CTRL DU CLAVIER ---------------
// -----------------------------------------------------------------------
byte status_clavier (void) {
  #if __WATCOMC__
    char *ptr;
    ptr = (char *) 0x417;
    return *ptr;
  #else
    int Status;
    _disable();
    Status=peek_b(0x40,0x17);
    _enable();
    return Status;
  #endif
}

// -----------------------------------------------------------------------
// -- RETOURNE UNE CHAINE DE CARACTERE POUR CHEMIN -----------------------
// -----------------------------------------------------------------------
void split_chemin(char *Resultat,char *Source,byte Choix) {
  char Drive [MAXDRIVE];
  char Dir   [MAXDIR];
  char Name  [MAXFILE];
  char Ext   [MAXEXT];

  _splitpath(Source,Drive,Dir,Name,Ext);

  switch (Choix) {
    case 0:
      strcpy(Resultat,Drive);
      break;
    case 1:
      strcpy(Resultat,Dir);
      break;
    case 2:
      strcpy(Resultat,Name);
      break;
    case 3:
      strcpy(Resultat,Ext);
      break;
    case 4:
      strcpy(Resultat,Drive);
      strcat(Resultat,Dir);
      break;
    case 5:
      strcpy(Resultat,Name);
      strcat(Resultat,Ext);
      break;
  }
}

/* ----------------------------------------------------------------------- */
/* ------------ RETOURNE LA VALEUR D'UNE ADRESSE MEMOIRE ----------------- */
/* ----------------------------------------------------------------------- */
byte peek_b(unsigned S,unsigned O) {
  byte *Adresse;
  Adresse=(byte *) MK_FP(S,O);
  return Adresse[0];
}

/* ----------------------------------------------------------------------- */
/* ------------ RETOURNE LE DISQUE EN COURS ------------------------------ */
/* ----------------------------------------------------------------------- */
byte get_disk(void) {
  unsigned Disk;
  _dos_getdrive(&Disk);
  return (byte) (Disk-1);
}

/* ----------------------------------------------------------------------- */
/* ------------ RETOURNE LE DISQUE EN COURS ------------------------------ */
/* ----------------------------------------------------------------------- */
void set_disk(byte Disk) {
  _dos_setdrive(Disk+1,0);
}

// ----------------------------------------------------------------------- */
// -- MODIFIE LES CARACTERES ASCII -> WATCOM ----------------------------- */
// ----------------------------------------------------------------------- */
#if __WATCOMC__
void parse_ascii_watcom(char *String) {
  register int i=0;

  while (1) {
    switch (String[i]) {
      case 'Š'   : String[i]='è'; break;
      case '‚'   : String[i]='é'; break;
      case 'ƒ'   : String[i]='â'; break;
      case '“'   : String[i]='ô'; break;
      case '…'   : String[i]='à'; break;
      case '‡'   : String[i]='ç'; break;
      case 'Œ'   : String[i]='î'; break;
      case 'ˆ'   : String[i]='ê'; break;
      case 'ø'   : String[i]='°'; break;
      case '–'   : String[i]='û'; break;
      case '‰'   : String[i]='ë'; break;
      case '‹'   : String[i]='ï'; break;
      case NULLC : return;
    }
    i++;
  }
}
#endif

// -----------------------------------------------------------------------
// -- TRI LES CARACTERES -------------------------------------------------
// -----------------------------------------------------------------------
int compare(const void *op1,const void *op2) {
  const char **p1=(const char **) op1;
  const char **p2=(const char **) op2;
  return(strcmp(*p1,*p2));
}

tri_tableau(char *CharVect[],int Nb,int Taille) {
  qsort((void *) CharVect,Nb,Taille,compare);
}

// -----------------------------------------------------------------------
// -- ECHANGE DEUX FLOAT -------------------------------------------------
// -----------------------------------------------------------------------
void swap_dbl(DBL *a,DBL *b) {
  register DBL Swap=*b;
  *b=*a;
  *a=Swap;
}

// -----------------------------------------------------------------------
// -- ECHANGE DEUX ENTIERS -----------------------------------------------
// -----------------------------------------------------------------------
void swap_int(int *a,int *b) {
  register int Swap=*b;
  *b=*a;
  *a=Swap;
}

// -----------------------------------------------------------------------
// -- GERE LA TAILLE MEMOIRE RESTANTE ------------------------------------
// -----------------------------------------------------------------------
unsigned init_taille_memoire(void) {
  #if WINDOWS
  #else
      struct SREGS sregs;
      struct {
      unsigned LargestBlockAvail;
      unsigned MaxUnlockedPage;
      unsigned LargestLockablePage;
      unsigned LinAddrSpace;
      unsigned NumFreePagesAvail;
      unsigned NumPhysicalPagesFree;
      unsigned TotalPhysicalPages;
      unsigned FreeLinAddrSpace;
      unsigned SizeOfPageFile;
      unsigned Reserved[3];
    } MemInfo;

    segread(&sregs);

    regs.x.eax=0x00000500;
    memset(&sregs,0,sizeof(sregs) );
    sregs.es=FP_SEG(&MemInfo);
    regs.x.edi=FP_OFF(&MemInfo);
    int386x(0x31,&regs,&regs,&sregs);

    return (unsigned) MemInfo.LargestBlockAvail;
  #endif
}

// -----------------------------------------------------------------------
// -- COMPARE UNE VALEUR ENTRE < ET ENTRE > ------------------------------
// -----------------------------------------------------------------------
byte compris_entre(long A,long B,long C) {
  if (B<A || B>C) {
    f_erreur("Number %lu invalid !|Number between %lu and %lu",B,A,C);
    return 0;
  }
  return 1;
}

// -----------------------------------------------------------------------
// -- TEST SI TOUCHE ESC OU BOUTON 2 DE LA SOURIS ------------------------
// -----------------------------------------------------------------------
byte sequence_sortie(void) {
  int T;

  if (MouseB()==2) { while (MouseB()==2); return 1; }
  if (kbhit()) { if ((T=getch())==27) return 1; else ungetch(T); }
  return 0;
}

// -----------------------------------------------------------------------
// -- TEST SI UN FICHIER EXISTE OU NON -----------------------------------
// -----------------------------------------------------------------------
byte test_fichier(char *Chemin) {
  if (access(Chemin,0)!=0) return 0;
  return 1;
}

// -----------------------------------------------------------------------
// -- SUPPRIME LE CTRL-ALT-DEL, CTRL-BREAK, CTRL-C -----------------------
// -----------------------------------------------------------------------
void (__interrupt __far *OldVect09) (void);
void restaure_int_09(void) {
  #if!WINDOWS
    _dos_setvect(0x9,OldVect09);
  #endif
}

void _interrupt _far noctrl(void) {
  byte Bit;
  static int flag;

  #if !WINDOWS

  _enable();

  if ((Bit=(byte)inp(0x60))==29) flag=1; // Ctrl

  if (Bit == 157) flag = 0;

  if (flag==0) {
    (*OldVect09)();
  } else  {
    switch (Bit) {
      case 46 :   // Ctrl-C
      case 70 :   // Ctrl-Break
      // case 56 : // Alt
      // case 83 : // Del
        Bit = (byte) inp(0x61);
        outp(0x61,Bit | 0x80);
        outp(0x61,Bit);
        outp(0x20,0x20);
        break;
      default :
        (*OldVect09)();
    }
  }
  #endif
}

void supprime_ctrl_del_break(void) {
  #if !WINDOWS
  _disable();
  OldVect09=_dos_getvect(0x9);
  _dos_setvect(0x9,noctrl);
  atexit(restaure_int_09);
  #endif
}

// -----------------------------------------------------------------------
// -- FONCTION D'ALLOCATION DE MEMOIRE PERSONNELLE -----------------------
// -----------------------------------------------------------------------
void *mem_alloc(size_t Taille) {
  MemoirePrise+=Taille;
  return malloc(Taille);
}

// -----------------------------------------------------------------------
// -- FONCTION DE-ALLOCATION DE MEMOIRE PERSONNELLE ----------------------
// -----------------------------------------------------------------------
void mem_free(void *Pointeur,size_t Taille) {
  MemoirePrise-=Taille;
  free(Pointeur);
}

// -----------------------------------------------------------------------
// -- ECRIT UN FICHIER LOG AVEC PARAMETRES -------------------------------
// -----------------------------------------------------------------------
void log_out(int Ct,char *String,...) {
  char Sortie[256];
  va_list parametre;
  FILE *Fichier;
  char Heure[10];
  char *Temp;

  if (!DEBUG) return;

  va_start(parametre,String);
  vsprintf(Sortie,String,parametre);
  va_end(parametre);

  Temp=(char *) malloc(strlen(NewChemin)+20);
  strcpy(Temp,NewLecteur);
  strcat(Temp,NewChemin);
  strcat(Temp,"\\");
  strcat(Temp,"POVLAB.LOG");

  get_heure(Heure);

  Fichier=fopen(Temp,"a+t");
  fprintf(Fichier,"%s: [%04d] %s\n",Heure,Ct,Sortie);
  fclose(Fichier);

  free((char *) Temp);
}

// -----------------------------------------------------------------------
// -- GERE LES PROBLEMES D'ACCES AU DISQUE -------------------------------
// -----------------------------------------------------------------------
int __far critical_error_handler(unsigned deverr,unsigned errcode,unsigned far *devhdr) {
  DiskError=1;
  _hardresume(_HARDERR_IGNORE);
  return (_HARDERR_IGNORE);
}

// -----------------------------------------------------------------------
// -- SUPPRIME LES TABULATIONS DANS UNE CHAINE ---------------------------
// -----------------------------------------------------------------------
void supprime_tab(char *String) {
  int i=-1;
  while (String[i++]) if (String[i]==9) String[i]=9;
}
