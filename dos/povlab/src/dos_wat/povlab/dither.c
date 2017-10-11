/* ---------------------------------------------------------------------------
*  DITHER.C
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
#include <CTYPE.H>
#include <GRAPH.H>
#include <STDLIB.H>
#include <MALLOC.H>
#include <STRING.H>
#include "GLIB.H"
#include "GLOBAL.H"
#include "LIB.H"

#define MIN(a,b)		((a)<(b)?(a):(b))
#define MAX(a,b)		((a)>(b)?(a):(b))
#define CLIP(x) ((x)>255?255:((x)<0?0:(x)))

#define PRINT_STEP  16
#define RES         64
#define DEF_NOISE   8
#define CHUNK       32000
#define MIN_SIZE    1024
#define PAL_256 	4
#define COUNT_LIMIT (0xFFFFL)

typedef struct node {
  unsigned short  color;
  unsigned short  count;  // doubles as pixel count & palette index
  struct node	  *next;
} Node;

typedef struct {
  int X,Y;
} XY;

int ord_dith[4][4]={
  {-7,  1, -5,  3},
  { 5, -3,  7, -1},
  {-4,  4, -6,  2},
  { 8,  0,  6, -2}
};

Node *grid[RES][RES];        // linked list headers for each red/green pair

int red[256], grn[256], blu[256];           // 6 bit palette
int red8[256], grn8[256], blu8[256];        // 8 bit for gif

int width,height;
int noise;           /* default to no noise */
int ordered;         /* do ordered dither */
int last_pass;       /* to help with out of RAM conditions */
int line;
unsigned int bytes_left;
long count_limit;   // limit pixel count to this

char *BitMap8;
char *BitMap24;
char *ptr_calloc;
int *buf[2][3];             /* our buffer */
Node *ptr;
Node *prev;
char *ret;

byte Dither=0;

// ---------------------------------------------------------------------------
// -- RETURN L'ADRESSE D'UN PIXEL DANS BITMAP LINEAIRE -----------------------
// ---------------------------------------------------------------------------
long bitmap_pixel(DBL x,DBL y,int n,int bbp) {
  return (long) (n*bbp*y+x*bbp);
}

// ---------------------------------------------------------------------------
// -- AJUSTE LE BITMAP D'UNE MANIERE HORIZONTALE (SUR X) ---------------------
// ---------------------------------------------------------------------------
void ajuste_bitmap(int X1,int X2,int Y1,int Y2,int YS,int YD,int XS,int XD) {
  register int dx,dy,e,d,DX2;
  register int sx,sy,CR,CG,CB;

  dx=abs((int)(X2-X1));
  dy=abs((int)(Y2-Y1));
  sx=signe(X2-X1);
  sy=signe(Y2-Y1);
  e=(dy<<1)-dx;
  DX2=dx<<1;
  dy<<=1;

  for (d=0;d<=dx;d++) {
    CR=BitMap24[bitmap_pixel(Y1,YS,XS,3)];
    CG=BitMap24[bitmap_pixel(Y1,YS,XS,3)+1];
    CB=BitMap24[bitmap_pixel(Y1,YS,XS,3)+2];
    BitMap24[bitmap_pixel(X1,YD,XD,3)]=CR;
    BitMap24[bitmap_pixel(X1,YD,XD,3)+1]=CG;
    BitMap24[bitmap_pixel(X1,YD,XD,3)+2]=CB;

    while(e>=0) { Y1+=sy; e-=DX2; }

    X1+=sx;
    e+=dy;
  }
}

// ---------------------------------------------------------------------------
// -- MAP -- FIND THE "BEST" MAPPING OF AN RGB VALUE IN THE PALETTE ----------
// ---------------------------------------------------------------------------
int map (int r,int g,int b) {
  int     i, j, k;
  register int     dr, dg, db, c;
  int     best;
  long    min_dist, dist;

  best=0;
  i=red[best];
  j=grn[best];
  k=blu[best];
  dr=i - r;
  dg=j - g;
  db=k - b;
  min_dist=(long)dr*(long)dr + (long)dg*(long)dg + (long)db*(long)db;

  for (c=1;c<256;c++) {
    i=red[c];
    j=grn[c];
    k=blu[c];
    dr=i - r;
    dg=j - g;
    db=k - b;
    dist=(long)dr*(long)dr + (long)dg*(long)dg + (long)db*(long)db;
    if (dist<min_dist) {
      min_dist=dist;
      best=c;
    }
  }
  return best;
}

// ----------------------------------------------------------------------------
// -- MEM_CALLOC --------------------------------------------------------------
// ----------------------------------------------------------------------------
void *mem_calloc(size_t n) { // --- how many bytes
  /*
  if (n>bytes_left) {		 // --- we need more RAM
    ptr_calloc=calloc(CHUNK,1);
    if (ptr_calloc==NULL) {         // --- squeeze every last bit
	  ret=calloc(n,1);
      if (ret) return (void *) ret;
      if (last_pass) return (void *) NULL;
      f_erreur("Out of memory for dithering.");
	  exit(1);
	}
	bytes_left=CHUNK;
  }
  ret=ptr_calloc;
  ptr_calloc +=n;
  bytes_left -=n;

  return (void *) ret;
  */
  return (void *) malloc(n);
}

// ----------------------------------------------------------------------------
// -- ADD_COLOR -- ADD A COLOR TO THE TREE (OR AT LEAST UP ITS COUNT) ---------
// ----------------------------------------------------------------------------
void add_color(int r,int g,int b,int c) { /* color to add to tree, mber of pixels */
  register long ltmp;

  c=MIN(c, count_limit);

  ptr=grid[r][g];
  prev=ptr;
  if (!ptr) {              /* new list */
    ptr=(Node *) mem_calloc(sizeof(Node));
    grid[r][g]=ptr;
    ptr->color=b;
    ptr->count=(unsigned int)c;
    ptr->next=NULL;
    return;
  }
  if (ptr->color > b) {    /* need a new node at the head */
    prev=(Node *) mem_calloc(sizeof(Node));
    prev->color=b;
    prev->count=(unsigned int)c;
    prev->next=ptr;
    grid[r][g]=prev;
    return;
  }

  for (;;) {       /* walk down the list looking for the right place */
    if (ptr->color==b) {
      ltmp=ptr->count;
      ltmp += (unsigned int)c;
      ltmp=MIN(ltmp, count_limit);
      ptr->count=(unsigned int)ltmp;

      return;
    }

    if (ptr->next==(Node *) NULL) {     /* end of the rope */
      ptr->next=(Node *) mem_calloc(sizeof(Node));
      ptr=ptr->next;
      ptr->color=b;
      ptr->count=(unsigned int)c;
      ptr->next=NULL;
      return;
    }

    prev=ptr;             /* step down to next node */
    ptr=ptr->next;
  }
}

// ----------------------------------------------------------------------------
// -- GET_INDEX -- GET THE PALETTE INDEX OF AN RGB COLOR ----------------------
// ----------------------------------------------------------------------------
unsigned short get_index(int r,int g,int b) { /* color to find*/
  register unsigned short color;

  ptr=grid[r][g];
  prev=ptr;
  if (!ptr) {      /* no color so we must create one */
    ptr=(Node *) mem_calloc(sizeof(Node));
    if (ptr) {
      grid[r][g]=ptr;
      color=map(r, g, b);
      ptr->count=color;
      ptr->color=b;
      ptr->next=NULL;
    } else {
      color=map(r, g, b);
    }
    return color;
  }
  if (ptr->color > b) {    /* need a new node at the head */
    prev=(Node *) mem_calloc(sizeof(Node));
    if (prev) {
      color=map(r, g, b);
      prev->count=color;
      prev->color=b;
      prev->next=ptr;
      grid[r][g]=prev;
    } else {
      color=map(r, g, b);
    }
    return color;
  }

  for (;;) {       /* walk down the list looking for the right color */
    if (ptr->color==b) {
      return ptr->count;
    }
    if (ptr->next==(Node *)NULL) {     /* end of the rope */
      ptr->next=(Node *) mem_calloc(sizeof(Node));
      if (ptr->next) {
        ptr=ptr->next;
        ptr->color=b;
        color=map(r, g, b);
        ptr->count=color;
        ptr->next=NULL;
      } else {
        color=map(r, g, b);
      }
      return color;
    }

    prev=ptr;             /* step down to next node */
    ptr=ptr->next;
  }
}

// --------------------------------------------------------------------------
// -- GET_PIX(X,Y) -- RETURN THE PALETTE INDEX OF X,Y -----------------------
// --------------------------------------------------------------------------
int get_pix(int x,int y) {
  register int pal;
  int r, g, b;
  int r2, g2, b2;
  register long k=bitmap_pixel(x,y,width,3);

  if (line!=y) line=y;

  b=BitMap24[k+2]; // C[2]; //getc(infp);
  g=BitMap24[k+1]; // C[1]; //getc(infp);
  r=BitMap24[k+0]; // C[0]; //getc(infp);
  pal=get_index(r>>2,g>>2,b>>2);

  if (noise) {
	r2=r + rand()%noise - rand()%noise; r2=CLIP(r2);
	g2=g + rand()%noise - rand()%noise; g2=CLIP(g2);
	b2=b + rand()%noise - rand()%noise; b2=CLIP(b2);
	pal=get_index(r2>>2,g2>>2,b2>>2);
  }

  if (ordered) {
	if (noise==0) {
	  r2=r;
	  g2=g;
	  b2=b;
	}
    r2 += ord_dith[x%4][y%4]; r2=CLIP(r2);
    g2 += ord_dith[x%4][y%4]; g2=CLIP(g2);
    b2 += ord_dith[x%4][y%4]; b2=CLIP(b2);
	pal=get_index(r2>>2, g2>>2, b2>>2);
  }

  return pal;
}

// ----------------------------------------------------------------------------
// -- GET_PIX_FS -- GET_PIX THAT DOES FS DITHERING ----------------------------
// ----------------------------------------------------------------------------
int get_pix_fs(int x,int y) {
  register int i, j, r, g, b, c;
  int rerr, gerr, berr;
  register long k;

  if (line==-1) {
    for (i=0; i<2; i++) { // ------- allocate space for the buffer
      for (j=0; j<3; j++) {
        buf[i][j]=(int *) mem_calloc((width+1)*sizeof(int));
      }
    }
    i=0;                  // ------- read in scan line 0
    while (i<width) {
      k=bitmap_pixel(i,y,width,3);
      b=BitMap24[k+2];
      g=BitMap24[k+1];
      r=BitMap24[k+0];
      rerr=r;
      gerr=g;
      berr=b;
      if (noise) {
        rerr=r + rand()%noise - rand()%noise; rerr=CLIP(rerr);
        gerr=g + rand()%noise - rand()%noise; gerr=CLIP(gerr);
        berr=b + rand()%noise - rand()%noise; berr=CLIP(berr);
      }
      buf[line&0x01][0][i]=rerr;
      buf[line&0x01][1][i]=gerr;
      buf[line&0x01][2][i]=berr;
      i++;
    }       /* we've got line 0 */
  }       /* end of initial setup */

  if (line!=y) {         // --- we're starting a new line
    line=y;
    if (y < height-1) {     // --- if not at last scan line
      i=0;                  // --- read in scan line y
      while (i<width) {
        k=bitmap_pixel(i,y,width,3);
        b=BitMap24[k+2];
        g=BitMap24[k+1];
        r=BitMap24[k+0];
        rerr=r;
        gerr=g;
        berr=b;
        if(noise) {
          rerr=r + rand()%noise - rand()%noise; rerr=CLIP(rerr);
          gerr=g + rand()%noise - rand()%noise; gerr=CLIP(gerr);
          berr=b + rand()%noise - rand()%noise; berr=CLIP(berr);
        }
        buf[line&0x01][0][i]=rerr;
        buf[line&0x01][1][i]=gerr;
        buf[line&0x01][2][i]=berr;
        i++;
      }       // --- we've got line y in buf y%2

      j=(line+1)%2;

      // --- dither buf (line+1)%2  ie j

      if (j) {     // --- alternate for serpentine
        for(i=1; i<width-1; i++) {
            r=CLIP(buf[j][0][i]);
            g=CLIP(buf[j][1][i]);
            b=CLIP(buf[j][2][i]);
            c=get_index(r>>2, g>>2, b>>2);
            buf[j][0][i]=c;   // --- put palette index back for output
            rerr=r - red8[c];
            gerr=g - grn8[c];
            berr=b - blu8[c];
            if (rerr) {
              buf[j][0][i+1] += rerr*7/16;
              buf[y%2][0][i] += rerr*5/16;
              buf[y%2][0][i+1] += rerr/16;
              buf[y%2][0][i-1] += rerr*3/16;
            }
            if (gerr) {
              buf[j][1][i+1] += gerr*7/16;
              buf[y%2][1][i] += gerr*5/16;
              buf[y%2][1][i+1] += gerr/16;
              buf[y%2][1][i-1] += gerr*3/16;
            }
            if (berr) {
              buf[j][2][i+1] += berr*7/16;
              buf[y%2][2][i] += berr*5/16;
              buf[y%2][2][i+1] += berr/16;
              buf[y%2][2][i-1] += berr*3/16;
            }
        }   // --- end of i loop
        // --- fix first and last pixels in scan line
        i=0;
        r=CLIP(buf[j][0][i]);
        g=CLIP(buf[j][1][i]);
        b=CLIP(buf[j][2][i]);
        c=get_index(r>>2, g>>2, b>>2);
        buf[j][0][i]=c;
        i=width-1;
        r=CLIP(buf[j][0][i]);
        g=CLIP(buf[j][1][i]);
        b=CLIP(buf[j][2][i]);
        c=get_index(r>>2, g>>2, b>>2);
        buf[j][0][i]=c;
      } else {    // --- else scan left
        for (i=width-2; i>0; i--) {
          r=CLIP(buf[j][0][i]);
          g=CLIP(buf[j][1][i]);
          b=CLIP(buf[j][2][i]);
          c=get_index(r>>2, g>>2, b>>2);
          buf[j][0][i]=c;   // --- put palette index back for output
          rerr=r - red8[c];
          gerr=g - grn8[c];
          berr=b - blu8[c];
          if (rerr) {
            buf[j][0][i-1] += rerr*7/16;
            buf[y%2][0][i] += rerr*5/16;
            buf[y%2][0][i-1] += rerr/16;
            buf[y%2][0][i+1] += rerr*3/16;
          }
          if (gerr) {
            buf[j][1][i-1] += gerr*7/16;
            buf[y%2][1][i] += gerr*5/16;
            buf[y%2][1][i-1] += gerr/16;
            buf[y%2][1][i+1] += gerr*3/16;
          }
          if (berr) {
            buf[j][2][i-1] += berr*7/16;
            buf[y%2][2][i] += berr*5/16;
            buf[y%2][2][i-1] += berr/16;
            buf[y%2][2][i+1] += berr*3/16;
          }
        }   // --- end of i loop
        // --- fix first and last pixels in scan line
        i=0;
        r=CLIP(buf[j][0][i]);
        g=CLIP(buf[j][1][i]);
        b=CLIP(buf[j][2][i]);
        c=get_index(r>>2, g>>2, b>>2);
        buf[j][0][i]=c;
        i=width-1;
        r=CLIP(buf[j][0][i]);
        g=CLIP(buf[j][1][i]);
        b=CLIP(buf[j][2][i]);
        c=get_index(r>>2, g>>2, b>>2);
        buf[j][0][i]=c;
      }   // --- end of right to left scan
    } else {    // --- we're at the last scan line
      j=(line+1) & 0x01;
      // --- just convert last line, no dithering
      for (i=0; i<width; i++) {
        r=CLIP(buf[j][0][i]);
        g=CLIP(buf[j][1][i]);
        b=CLIP(buf[j][2][i]);
        c=get_index(r>>2, g>>2, b>>2);
        buf[j][0][i]=c;
      }
    }
  }       // --- end of if at beginning of new line

  // -- output appropriate pixel value from buf

  c=buf[(line+1)&0x01][0][x];
  if (c>255 || c<0) c=CLIP(c);

  return c;
}

// ---------------------------------------------------------------------------
// -- FONCTION DE DEMARRAGE DU DITHERING -------------------------------------
// ---------------------------------------------------------------------------
int dither(int X,int Y) {
  register int i,j;
  Node *Tmp,*r_ptr,*r_prev;

  switch (Dither) {
    case 0:
      ordered=0;
      noise=0;
      break;
    case 1:
      noise=0;
      ordered=1;
      break;
    case 2:
      srand(1962);
      noise=DEF_NOISE;
      break;
  }

  for (i=0;i<256;i++) {
    red8[i]=(int)Palette[i][0];
    grn8[i]=(int)Palette[i][1];
    blu8[i]=(int)Palette[i][2];
    red[i]=red8[i]>>2;
    grn[i]=grn8[i]>>2;
    blu[i]=blu8[i]>>2;
  }

  last_pass=1;
  count_limit=COUNT_LIMIT;
  width=X;
  height=Y;
  line=-1;
  bytes_left=0;
  r_ptr=ptr;
  r_prev=prev;

  f_jauge(1,AFFICHE,0,0,"Dithering image");

  if (Dither==2) {
    for (j=0;j<Y;j++) {
      for (i=0;i<X;i++) {
        BitMap8[X*j+i]=(byte) get_pix_fs(i,j);
      }
      if (j%16==0) f_jauge(1,MODIF,j,Y,NULL);
    }
  } else {
    for (j=0;j<Y;j++) {
      for (i=0;i<X;i++) {
        BitMap8[X*j+i]=(byte) get_pix(i,j);
      }
      if (j%16==0) f_jauge(1,MODIF,j,Y,NULL);
    }
  }

  f_jauge(1,EFFACE,0,0,NULL);

   for (ptr = r_ptr; ptr != (Node *)NULL; ptr = Tmp)
  {
        Tmp = ptr->next;
        free((Node *)ptr);
  }
  for (prev = r_prev; prev != (Node *)NULL; prev = Tmp)
  {
        Tmp = prev->next;
        free((Node *)prev);
  }

  if (Dither==2) {
    for (i=0;i<2;i++) {
      for (j=0;j<3;j++) {
        free((int *) buf[i][j]);
      }
    }
  }

  return 1;
}

// ---------------------------------------------------------------------------
// -- MISE A L'ECHELLE D'UNE IMAGE -------------------------------------------
// ---------------------------------------------------------------------------
XY scale_image(int Xi,int Yi) {
  DBL rx,ry;
  int MX=400;
  int MY=300;
  register long DX,DY,e,i,DX2;
  int SX,SY;
  int XS1,YS1;
  int XS2,YS2;
  int XD1,YD1;
  int XD2,YD2;
  XY N;

  N.X=Xi;
  N.Y=Yi;

  if (Xi<=MX && Yi<=MY) return N;

  while (N.X>MX && N.Y>MY) {
    rx=(DBL) N.X/1.1;
    ry=(DBL) N.Y/1.1;
    N.X=(int) rx;
    N.Y=(int) ry;
    if (kbhit()) { getch(); N.X=N.Y=0; return N; }
  }

  message("New size %dx%d pixels",N.X,N.Y);

  XS1=YS1=0;
  XS2=Xi; YS2=Yi;
  XD1=YD1=0;
  XD2=N.X; YD2=N.Y;

  DX=abs((int)(YD2-YD1));
  DY=abs((int)(YS2-YS1));
  SX=signe(YD2-YD1);
  SY=signe(YS2-YS1);
  e=(DY<<1)-DX;
  DX2=DX<<1;
  DY<<=1;

  f_jauge(1,AFFICHE,0,0,"Rescaling image");

  for (i=0;i<=DX;i++) {
    ajuste_bitmap(XD1,XD2,XS1,XS2,YS1,YD1,Xi,N.X);
    while (e>=0) {
      YS1+=SY;
      e-=DX2;
    }
    YD1+=SX;
    e+=DY;
    if (!(i%16)) f_jauge(1,MODIF,i,DX,NULL);
  }

  f_jauge(1,EFFACE,0,0,NULL);
  return N;
}

// ---------------------------------------------------------------------------
// -- AFFICHE UNE IMAGE AVEC DITHERING PALETTE FORCEE ------------------------
// ---------------------------------------------------------------------------
byte voir_image(byte Laquelle) {
  char NomFichier[MAXPATH];
  int Ok=0;
  IMAGE *ImgTGA=NULL;
  char *ImgGIF=NULL;
  int Xi,Yi,X1,Y1,X2,Y2;
  byte Image=0;
  char *Spec[3]={"2","*.GIF","*.TGA"};
  long k=0;
  register int i,j;
  XY New;

  if (NbCouleurs!=256) { f_erreur("Needed 256 color modes"); return 0; }

  if (Laquelle) {
    strcpy(NomFichier,selection_fichier(100,100,"VIEW GIF-TGA FILE",Spec));
    if (NomFichier[0]==27) return 0;
  } else {
    if (!test_fichier(LastImage)) {
      f_erreur("No previous render !");
      return 0;
    }
    strcpy(NomFichier,LastImage);
  }

  if (strinstr(0,NomFichier,".GIF")>=0) { // ------------ Lecture GIF
    Image=GIF;
    i=decompresse_gif(&Xi,&Yi,NomFichier,"",1);
    if (i==0) {
      f_erreur("Can't read GIF file %s.",NomFichier);
      return 0;
    }
    if ((ImgGIF=(char *) mem_alloc(Xi*Yi))==NULL) {
      f_erreur("No memory for image GIF");
      return 0;
    }
    message("Please wait, reading %dx%d GIF file %s...",Xi,Yi,NomFichier);
    i=decompresse_gif(&Xi,&Yi,NomFichier,ImgGIF,0);
    if (i==0) {
      f_erreur("Error reading GIF file %s.",NomFichier);
      mem_free(ImgGIF,Xi*Yi);
      return 0;
    }
    Ok=1;
  }

  if (strinstr(0,NomFichier,".TGA")>=0) { // ------------ Lecture TGA
	if ((ImgTGA=(IMAGE *) mem_alloc(sizeof(IMAGE)))==NULL) {
      f_erreur("No memory for dithering [TGA]");
	  return 0;
	}
	Image=TGA;
    message("Please wait, reading TGA file %s...",NomFichier);
	i=lecture_targa(ImgTGA,NomFichier);
    if (i==0 || ImgTGA->Colour_Map) {
      if (ImgTGA->Colour_Map) f_erreur("Can't read 8 bits TGA file");
      mem_free(ImgTGA,sizeof(IMAGE));
	  return 0;
	}
	Ok=1;
	Xi=(int) ImgTGA->width;
	Yi=(int) ImgTGA->height;
  }

  if (!Image) f_erreur("Need GIF ou TGA file format !");

  BitMap24=(char *) mem_alloc(Xi*Yi*3);
  BitMap8=(char *) mem_alloc((Xi*Yi)+6);
  if (BitMap24==NULL) { f_erreur("Can't allocate memory [24 bits]"); return 0; }
  if (BitMap8==NULL) { f_erreur("Can't allocate memory [8 bits]"); return 0; }

  f_jauge(1,AFFICHE,0,0,"Scanning image");

  for (j=0;j<Yi;j++) {
	for (i=0;i<Xi;i++) {
      k=bitmap_pixel(i,j,Xi,3);
      if (Image==GIF) {
        BitMap24[k  ]=PaletteGIF[ImgGIF[Xi*j+i]][0];
        BitMap24[k+1]=PaletteGIF[ImgGIF[Xi*j+i]][1];
        BitMap24[k+2]=PaletteGIF[ImgGIF[Xi*j+i]][2];
      } else {
        BitMap24[k  ]=ImgTGA->data.rgb_lines[j].red[i];
        BitMap24[k+1]=ImgTGA->data.rgb_lines[j].green[i];
        BitMap24[k+2]=ImgTGA->data.rgb_lines[j].blue[i];
      }
	}
    if (j%16==0) f_jauge(1,MODIF,j,Yi,NULL);
  }

  f_jauge(1,EFFACE,0,0,NULL);

  if (Image==TGA) {
    for (j=0;j<Yi;j++) {
      mem_free(ImgTGA->data.rgb_lines[j].red,0);
      mem_free(ImgTGA->data.rgb_lines[j].green,0);
      mem_free(ImgTGA->data.rgb_lines[j].blue,0);
    }
    mem_free(ImgTGA->data.rgb_lines,0);
    mem_free(ImgTGA,sizeof(IMAGE));
  }
  if (Image==GIF) mem_free(ImgGIF,Xi*Yi);

  New=scale_image(Xi,Yi);

  if (New.X && New.Y) {
    if (dither(New.X,New.Y)) {
      memmove((char *) BitMap8+6,(char *) BitMap8,(long) Xi*Yi);
      BitMap8[0]=((int) New.X) & 0xFF;
      BitMap8[1]=(int) New.X/0xFF;
      BitMap8[2]=((int) New.Y) & 0xFF;
      BitMap8[3]=(int) New.Y/0xFF;

      X1=CentX-New.X/2-10;
      X2=CentX+New.X/2+10;
      Y1=CentY-New.Y/2-35;
      Y2=CentY+New.Y/2+35;
      g_fenetre(X1,Y1,X2,Y2,NomFichier,AFFICHE);
      windows(X1+10,Y1+30,X1+10+New.X,Y1+30+New.Y,0,FOND);
      #if !WINDOWS
      _putimage(X1+10,Y1+30,(char *) BitMap8,_GPSET);
      #endif
      bouton_dialog(X1,X2,Y2,1,0);

      while (bouton_dialog(X1,X2,Y2,0,0)==-1);

      g_fenetre(X1,Y1,X2,Y2,"",EFFACE);
    }
  }

  mem_free(BitMap24,Xi*Yi*3);
  mem_free(BitMap8,(Xi*Yi)+6);
  return 1;
}

// -------------------------------------------------------------------------
// -- MODIFIE LE NIVEAU DE DITHERING DE L'IMAGE ----------------------------
// -------------------------------------------------------------------------
void modifie_dithering(void) {
  int X1=CentX-100;
  int X2=CentX+100;
  int Y1=CentY-50;
  int Y2=CentY+58;
  int i;

  forme_mouse(MS_FLECHE);

  g_fenetre(X1,Y1,X2,Y2,"Dithering control",AFFICHE);

  init_pastille(10,X1+20,Y1+ 30,"None",(Dither==0),"Fast but bad");
  affiche_pastille(10);
  init_pastille(11,X1+20,Y1+ 45,"Ordered",(Dither==1),"Good for preview");
  affiche_pastille(11);
  init_pastille(12,X1+20,Y1+ 60,"Random noise",(Dither==2),"Very good but long");
  affiche_pastille(12);

  bouton_dialog(X1,X2,Y2,1,1);

  while (1) {
    test_groupe_pastille(10,12);
    if ((i=bouton_dialog(X1,X2,Y2,0,1))!=-1) break;
  }

  g_fenetre(X1,Y1,X2,Y2,"",EFFACE);

  if (i==0) Dither=quelle_pastille_dans_groupe(10,12)-10;
  
  forme_mouse(MS_FLECHE);
}
