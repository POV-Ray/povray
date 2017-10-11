// ***********************************************************************
// *  LIB.C                                                              *
// *  Bibliothäque personnelle : Denis Olivier (C) ChromaGraphics 1994.  *
// *  Avril 1994                                                         *
// ***********************************************************************

#ifndef LIB_H_INCLUDED

#include <STDIO.H>
#include <LIMITS.H>
#include <CONIO.H>
#include <FLOAT.H>
#include <STDLIB.H>
#include <MATH.H>

#if __WATCOMC__
  #define MAXPATH   144
  #define MAXDRIVE  3
  #define MAXDIR    130
  #define MAXFILE   9
  #define MAXEXT    5
#endif

#define DBL float
#define byte unsigned char
#define word unsigned short

#define ON  1
#define OFF 0

extern int PageY;
extern int NbArg;

extern byte LIGHT;
extern byte Video;
extern byte DiskError;

extern char Titre_Fenetre [80];
extern char NomLogiciel[30];
extern char VerLogiciel[30];
extern char NewChemin[MAXPATH];
extern char NewLecteur[4];
extern char OldChemin[MAXPATH];
extern char OldLecteur[4];
extern char Arg[4][64];
extern char NomUtilisateur[64];

extern long NSerieLogiciel;
extern long MemoireLibre;
extern long MemoirePrise;

#ifndef __cplusplus

extern union REGS regs,inregs,outregs;

long duplique_fichier(char *Source,char *TmpDestination);
void forme_souris (void);
void MouseOn(void);
void MouseOff (void);
int  MouseY (void);
int  MouseB (void);
int  MouseX (void);
void RestaureChemin(void);
void beep (void);
void beep_erreur (void);
long diskspace (byte disque);
void efface_buffer_clavier (void);
void get_date(char *Date);
void get_heure(char *Heure);
void init_arg (int NbArg,char *ArgTmp[],byte New);
void init_char(void);
void init_nserie (void);
void init_disk(char *Chemin);
void init_mouse (byte Y,byte X);
void init_curseur (byte Debut,byte Fin);
void init_soft (char *Nom,char *Ver);
void message_dos(void);
long init_temps (void);
void my_exec (char *Prg, char *Cmd,int Video);
int  match(char *s1, char *s2);
byte status_clavier (void);
long strinstr (long decalage,char *ch1,char *ch2);
byte test_touche (void);
unsigned int bin(char *Binaire);
void split_chemin(char *Resultat,char *Source,byte Choix);
byte peek_b (unsigned S,unsigned O);
byte get_disk(void);
void set_disk(byte Disk);
void parse_ascii_watcom(char *String);
tri_tableau(char *CharVect[],int Nb,int Taille);
void swap_dbl(DBL *a,DBL *b);
void swap_int(int *a,int *b);
unsigned init_taille_memoire(void);
byte compris_entre(long A,long B,long C);
byte sequence_sortie(void);
byte test_fichier(char *Chemin);
void supprime_ctrl_del_break(void);
void *mem_alloc(size_t Taille);
void mem_free(void *Pointeur,size_t Taille);
int __far critical_error_handler( unsigned deverr,unsigned errcode,unsigned far *devhdr);
void log_out(int Ct,char *String,...);
void supprime_tab(char *String);

#endif
#define LIB_H_INCLUDED
#endif
