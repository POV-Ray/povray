/* ---------------------------------------------------------------------------
*  GIF.C
*
*  GIF file format is (tm) by CompuServe.
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
#include <DOS.H>
#include <STRING.H>
#include <STDLIB.H>
#include <CONIO.H>
#include <STDIO.H>
#include <MEM.H>
#include "LIB.H"
#include "GLIB.H"
#include "GLOBAL.H"
#if !WINDOWS
#include <GRAPH.H>
#endif

static int XGIF,YGIF;
byte PaletteGIF[256][3];

typedef struct GifHeader {
  char sig[6];
  byte XL,XH,YL,YH;
  byte flags,background,aspect;
};

typedef struct ImageBlock {
  byte Lleft,Hleft,Ltop,Htop,XL,XH,YL,YH;
  byte flags;
};

#define STEP            32      /* taille d'un pas de d‚placement */
#define NO_CODE         -1

// ----------------------------------------------------------------------- */
// ------------- SI LE DECODEUR TROUVE UNE EXTENSION --------------------- */
// ----------------------------------------------------------------------- */
void doextension(FILE *fp) {
  register int n,i;

  do {
    if ((n=fgetc(fp))!=EOF) { for (i=0;i<n;++i) fgetc(fp); }
  } while(n>0 && n!=EOF);
}

// ----------------------------------------------------------------------- */
// ------------- DECOMPRESSE UNE IMAGE GIF AVEC L'ALGO LZW --------------- */
// ----------------------------------------------------------------------- */
byte unpackimage(FILE *fp,char *Image,int bits,int flags) {
  register int bits2;          /* bits plus 1 */
  register int codesize;       /* taille du code courant en bits */
  register int codesize2;      /* prochaine taille de code */
  register int nextcode;       /* prochaine entr‚e de table disponible */
  register int thiscode;       /* code … expandre */
  register int oldtoken;       /* dernier symbole d‚cod‚ */
  register int currentcode;    /* code lu */
  register int oldcode;        /* code lu juste avant */
  register int bitsleft;       /* nombre de bits dans *p  */
  register int blocksize;      /* octets du prochain bloc */
  register int line=0;         /* prochaine ligne … ‚crire */
  register int octet=0;        /* prochain octet … ‚crire */
  register int pass=0;         /* num‚ro de passe pour images entrelac‚es */

  char *p;          /* pointeur sur octet courant dans buffer lecture */
  char *q;          /* pointeur sur l'octet lu dans buffer lecture */
  char b[255];          /* buffer de lecture */
  char *u;              /* Pointeur de pile dans firstcodestack */
  char *linebuffer; /* buffer de la ligne courante */

  static char firstcodestack[4096];  /* Pile des 1ers codes */
  static char lastcodestack[4096];   /* Pile des codes pr‚c‚dents */
  static int codestack[4096];        /* Pile pour les liens */

  static int wordmasktable[]={ 0x0000,0x0001,0x0003,0x0007,
                               0x000f,0x001f,0x003f,0x007f,
                               0x00ff,0x01ff,0x03ff,0x07ff,
                               0x0fff,0x1fff,0x3fff,0x7fff };

  static int inctable[]={ 8,8,4,2,0 }; /* incr‚ments pour entrelacement */
  static int startable[]={ 0,4,2,1,0 };  /* d‚buts d'entrelacement */

  p=q=b;
  bitsleft = 8;

  if (bits < 2 || bits > 8) { f_erreur("Bad symbol length in gif file."); return 0; }
  bits2 = 1 << bits;
  nextcode = bits2 + 2;
  codesize2 = 1 << (codesize = bits + 1);
  oldcode=oldtoken=NO_CODE;

  if ((linebuffer=mem_alloc(XGIF)) == NULL) {
    f_erreur("Can't allocate line buffer");
    return 0;
  }

  // boucler jusqu'… une sortie

  for (;;) {
    if (bitsleft==8) {
      if(++p >= q &&
      (((blocksize = fgetc(fp)) < 1) ||
      (q=(p=b)+fread(b,1,blocksize,fp))< (b+blocksize))) {
        mem_free(linebuffer,XGIF);
        f_erreur("Unexpected end of file");
        return 0;
      }
      bitsleft = 0;
    }
    thiscode = *p;
    if ((currentcode=(codesize+bitsleft))<=8) {
      *p>>=codesize;
      bitsleft=currentcode;
    }
    else {
      if(++p >= q &&
        (((blocksize = fgetc(fp)) < 1) ||
        (q=(p=b)+fread(b,1,blocksize,fp)) < (b+blocksize))) {
          mem_free(linebuffer,XGIF);
          f_erreur("Unexpected end of file");
          return 0;
      }
      thiscode |= *p << (8 - bitsleft);
      if(currentcode <= 16) {
        *p >>= (bitsleft=currentcode-8);
      } else {
        if(++p >= q &&
          (((blocksize = fgetc(fp)) < 1) ||
          (q=(p=b) + fread(b,1,blocksize,fp)) < (b+blocksize))) {
            mem_free(linebuffer,XGIF);
            f_erreur("Unexpected end of file");
            return 0;
        }
        thiscode |= *p << (16 - bitsleft);
        *p >>= (bitsleft = currentcode - 16);
      }
    }
    thiscode &= wordmasktable[codesize];
    currentcode = thiscode;

    if (thiscode == (bits2+1)) break;        /* fin d'image trouv‚e */
    if (thiscode > nextcode) {
       mem_free(linebuffer,XGIF);
       f_erreur("Bad code");
       return 0;
    }

    if (thiscode == bits2) {
      nextcode = bits2 + 2;
      codesize2 = 1 << (codesize = (bits + 1));
      oldtoken = oldcode = NO_CODE;
      continue;
    }

    u = firstcodestack;

    if (thiscode==nextcode) {
      if (oldcode==NO_CODE) {
        mem_free(linebuffer,XGIF);
        f_erreur("Bad first code");
        return 0;
      }
      *u++ = oldtoken;
      thiscode = oldcode;
    }

    while (thiscode >= bits2) {
      *u++ = lastcodestack[thiscode];
      thiscode = codestack[thiscode];
    }

    oldtoken=thiscode;

    do {
      linebuffer[octet++]=thiscode;
      if (octet>=XGIF) {
        memcpy(Image+(long)line*(long)XGIF,linebuffer,(long)XGIF);
        octet=0;

        /* si c'est une image entrelac‚e */

        if (flags & 0x40) {
          line+=inctable[pass];
          if (line>=YGIF) line=startable[++pass];
        } else {
          ++line;
        }
      }

      if (u<=firstcodestack) break;
      thiscode=*--u;
    } while(1);

    if(nextcode < 4096 && oldcode != NO_CODE) {
        codestack[nextcode] = oldcode;
        lastcodestack[nextcode] = oldtoken;
        if (++nextcode >= codesize2 && codesize < 12)
            codesize2 = 1 << ++codesize;
    }
    oldcode = currentcode;
  }
  mem_free(linebuffer,XGIF);
  return(1);
}

// ----------------------------------------------------------------------- */
// ------------- DECOMPRESSE UN FICHIER GIF ------------------------------ */
// ----------------------------------------------------------------------- */
byte unpackgif(FILE *fp,char *p,byte JusteXY) {
  struct GifHeader gh;
  struct ImageBlock iblk;
  register int background,flags;
  register int bits,c,b;

  // ----------------- v‚rifie la signature

  if (fread((char *)&gh,1,sizeof(gh),fp)!=sizeof(gh) || memcmp(gh.sig,"GIF87a",6)) {
    f_erreur("Error while reading header");
    return 0;
  }

  // ---------------- lit les dimensions de l'image

  bits=(gh.flags & 0x07)+1;
  background=gh.background;

  // --------------- lit la palette si il y en a une

  if (gh.flags & 0x80) {
    c=3 * (1 << ((gh.flags & 7)+1));
    if (fread(PaletteGIF,1,c,fp)!=c) {
      f_erreur("Error while reading palette");
      return 0;
    }
  }

  // --------------- boucler sur les blocs

  while((c=fgetc(fp))==',' || c=='!' || c==0) {
    // -------------  si c'est un bloc d'image

    if (c==',') {
      //puts("Wait... decompressing");

      // ---------- lit le d‚but de l'IMAGEBLOCK

      if (fread(&iblk,1,sizeof(iblk),fp)!=sizeof(iblk)) {
        f_erreur("Error while reading image block");
        return 0;
      }

      // -------------- r‚cupŠre les dimensions

      XGIF=iblk.XL+iblk.XH*256;
      YGIF=iblk.YL+iblk.YH*256;

      if (JusteXY) return 1; // juste pour lire X et Y

      // --------------- lit la palette locale si il en y a une

      if (iblk.flags & 0x80) {
          b=3*(1<<((iblk.flags & 0x07)+1));
          if (fread(PaletteGIF,1,b,fp)!=c) {
            f_erreur("Error reading local palette");
            return 0;
          }
          bits=(iblk.flags & 0x07)+1;
      }

      // ---------------------- lit la taille du code initial

      if ((c=fgetc(fp))==EOF) {
        f_erreur("Bad size in initial code");
        return 0;
      }

      flags=iblk.flags;

      // ------------ d‚compresse l'image

      unpackimage(fp,p,c,flags);

      // ------------ affiche l'image

      /*
      for (i=0;i<YGIF;i++) {
        for (j=0;j<XGIF;j++) {
          put_pixel(j,i,*p++);
        }
      }
      */

      // ------------- termin‚ !

      return(1);
    }
    // ------------ sinon c'est une extension
    else if (c=='!') doextension(fp);
  }
  f_erreur("There's no image in this file");
  return (0);
}

// ----------------------------------------------------------------------- */
// ------------- FONCTION PRINCIPALE ------------------------------------- */
// ----------------------------------------------------------------------- */
byte decompresse_gif(int *X,int *Y,char *Fichier,char *Image,byte JusteXY) {
  int Ok;
  FILE *fp;

  if ((fp=fopen(Fichier,"rb"))==NULL) {
    fclose(fp);
    return 0;
  }

  Ok=unpackgif(fp,Image,JusteXY);

  *X=XGIF;
  *Y=YGIF;

  fclose(fp);
  return Ok;
}

// -----------------------------------------------------------------------
// ------------- AFFICHE UN FICHIER GIF ----------------------------------
// -----------------------------------------------------------------------
byte affiche_gif(int PosX,int PosY,int X,int Y,char *Image,byte Prepare,byte Display,byte Pas) {
  register int i,j,y,k;
  char *Ligne;

  if (Prepare) {
    memmove(Image+6,Image,(long) X*(long) Y);
    Image[0]=X & 0xFF;
    Image[1]=X/0xFF;
    Image[2]=Y & 0xFF;
    Image[3]=Y/0xFF;
    if (Prepare==2) return 0;
  }

  if (Display) {
    #if !WINDOWS
    _putimage(PosX,PosY,Image,_GPSET);
    #endif
  } else {
    //set_port(PosX,PosY,XMax,YMax);
    Ligne=(char *) mem_alloc(X+7);
    y=0;
    k=0;
    for (j=0;j<Y;j++) {
      memcpy(Ligne,Image,6);
      Ligne[2]=1;
      Ligne[3]=0;
      memcpy(Ligne+6,Image+(6+y*X),X);
      for (i=0;i<=(Pas-1)-k;i++) {
        #if !WINDOWS
        _putimage(PosX,PosY+y+i,Ligne,_GPSET);
        #endif
      }
      y+=Pas;
      if (y>=Y) { k++; y=k; }
      delay(1);
    }
    mem_free(Ligne,X+7);
  }

  return 0;
}
