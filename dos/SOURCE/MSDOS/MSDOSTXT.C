/****************************************************************************
*                   msdostxt.c
*
*  This module implements the text output routines for a 32-bit protected
*  mode msdos verion of POV-Ray on various compilers.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's Graphics Developer's
*  Forum.  The latest version of POV-Ray may be found there as well.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#include <process.h>
#include <dos.h>
#include "frame.h"
#include "povproto.h"
#include "povray.h"
#include "optin.h"
#include "msdosall.h"

#define SCROLL_BACK_SIZE 10000

typedef struct W_Data_Struct WDATA;

struct W_Data_Struct
  {
   int CurX, CurY, Wy1, Wy2, Fcol, Bcol, Ovr, TabX, Init; 
   char Name[15];
   WDATA *Prev ,*Next;
   char *Buf;
   int Wrap, Cur, Top;
  };

static WDATA Bann_Data, Warn_Data, RInf_Data, SInf_Data, Fatl_Data, Stat_Data;
static WDATA Debug_Data;
static WDATA *OnTop, *Root;
static char *Saved_Text;

static void Do_Message(WDATA *Data,char *s);
static void Update_Tabs(WDATA *Data);
static void Do_Tab(WDATA *Data);
static void Slowit(void); 
static int Scan_Back(char *Buf, int *Strt);
static int Scan_Front(char *Buf, int *Strt, int End);
static int Viewit(void);

static int Next_TabX, LastY;

#define Init_WData(d,y1,y2,f,b,cx,cy,ov,n) (d).CurX=(cx);(d).CurY=(cy);\
 (d).Init=FALSE;(d).Wy1=(y1);(d).Wy2=(y2); (d).Fcol=(f);(d).Bcol=(b);\
 (d).Ovr=(ov); (d).TabX=0; strcpy((d).Name,n); (d).Buf=NULL;

void MSDOS_Text_Init_Vars()
{
 CON_init();
 
 Init_WData(Bann_Data,  1,22,15,1,0, 0, TRUE,"About"); 
 Init_WData(SInf_Data, 24,24,15,5,0, 0,FALSE,""); 
 Init_WData(RInf_Data,  2,22,15,2,0,20, TRUE,"Options"); 
 Init_WData(Stat_Data,  2,22,15,3,0,20, TRUE,"Statistics");
 Init_WData(Warn_Data,  2,22,14,6,0,20, TRUE,"Warning"); 
 Init_WData(Fatl_Data,  2,22,14,4,0,20, TRUE,"Error"); 
 Init_WData(Debug_Data, 2,22, 0,7,0,20, TRUE,"Debug"); 

 if((Fatl_Data.Buf=(char *)malloc(SCROLL_BACK_SIZE))==NULL)
 {
    fprintf(stderr,"Cannot allocate error message buffer.\n");
    exit(1);
 }
 
 Root=OnTop=NULL;
 Next_TabX=2;
 LastY=0;
 
 CON_setForeColor(15);
 CON_setBackColor(1);
 CON_setBackground(CON_getAttr(),32);
 CON_clrscr();
 CON_writec( 0, 0,0x001f,201);
 CON_writec(79, 0,0x001f,187);
 CON_fillText( 1, 0,78, 0,0x001f,205);
 CON_fillText( 1,23,78,23,0x001f,205);
 CON_fillText( 0, 1, 0,24,0x001f,186);
 CON_fillText(79, 1,79,24,0x001f,186);
 CON_writec( 0,23,0x001f,204);
 CON_writec(79,23,0x001f,185);
 CON_setWindow(1,1,78,22);
 CON_clrscr();
}

void MSDOS_Finish(int n)
 {
  WDATA *Data;

  if (opts.Options & PROMPTEXIT)
  {
    Viewit();
  }
  
/* Note:Using free rather than POV_FREE because this is freed after the
 * generic part of POV-Ray has already cleaned up all other memory.
 */
  for (Data=Root; Data!=NULL; Data=Data->Next)
  {
     if(Data->Buf!=NULL)
     {
       free(Data->Buf);
       Data->Buf=NULL;
     }
  }
  CON_setForeColor(7);
  CON_setBackColor(0);
  CON_setBackground(CON_getAttr(),32);
  CON_setWindow(0,0,79,24);
  CON_gotoxy(79,24);
  CON_printf("\n\r");
  exit(n);
 }


static void Do_Message(WDATA *Data,char *s)
  {
   char *ptr = s;
   register unsigned char c;

   if (!(Data->Init)) 
   {
     Data->Next=Data->Prev=NULL; 
     Data->Wrap=Data->Cur=0;

     if (Data->Ovr)
     {
       /* Note:Using malloc rather than POV_MALLOC because this
        * is freed after the generic part of POV-Ray has already cleaned up
        * all other memory.  
        */

       if(Data->Buf==NULL)
       {
         if((Data->Buf=(char *)malloc(SCROLL_BACK_SIZE))==NULL) 
         {
           Error("Cannot allocate text message buffer.\n");
         }
       }
 
       Data->Buf[0]=0;
       Data->Cur=1;
       Data->TabX=Next_TabX;
       Next_TabX+=strlen(Data->Name)+5;
       if (LastY<20)
       {
          Data->CurY=LastY;
       }
       /* Only Ovr gets linked */
       Data->Init=TRUE;
       Data->Next=Root;
       if (Root!=NULL)
       {
          Root->Prev=Data;
       }
       Root=Data;
     }
   }

   Update_Tabs(Data);
   CON_setWindow(1,Data->Wy1,78,Data->Wy2);
   CON_gotoxy(Data->CurX,Data->CurY);
   CON_setForeColor(Data->Fcol);
   CON_setBackColor(Data->Bcol);
   CON_setBackground(CON_getAttr(),32);

   if (Data->Ovr)
   {
     while ((c=*ptr++) != '\0')
     {
       if (c=='\n')
       {
         CON_putc('\r');
         CON_putc(c);
         c=0;
       }
       else
       {
         CON_putc(c);
       }
       /* This wraps 1 byte early but it'll make unwrapping easier */
       if (Data->Cur==SCROLL_BACK_SIZE)
       {
          Data->Wrap=TRUE;
          Data->Cur=0;
       }
       Data->Buf[Data->Cur++]=c;
     }
   }
   else
   {
     while ((c=*ptr++) != '\0')
     {
       if (c=='\n')
       {
         CON_putc('\r');
       }
       CON_putc(c);
     }
   }
   Data->CurX=CON_wherex();
   Data->CurY=CON_wherey();
   if (Data->Ovr)
   {
      LastY=Data->CurY;
   }   
  }


void MSDOS_Banner(char *s)
  {
   Do_Message(&Bann_Data,s);
  }
  
void MSDOS_Warning(char *s)
  {
   Do_Message(&Warn_Data,s);
  }
  
void MSDOS_Render_Info(char *s)
  {
   if (In_Graph_Mode)
     return;
   Do_Message(&RInf_Data,s);
  }
  
void MSDOS_Status_Info(char *s)
  {
   if (In_Graph_Mode)
     return;
   Do_Message(&SInf_Data,s);
  }
  
void MSDOS_Fatal(char *s)
  {
   Do_Message(&Fatl_Data,s);
  }
  
void MSDOS_Statistics(char *s)
  {
   Do_Message(&Stat_Data,s);
  }
  
void MSDOS_Debug_Info(char *s)
  {
   if (In_Graph_Mode)
     return;
   Do_Message(&Debug_Data,s);
  }
  
void MSDOS_Wait_Key(void)
{
  MSDOS_getch();
}

int MSDOS_getch(void)
{
  int Key;
  
  if((Key=getch())==0)
    Key=256+getch();
    
  return(Key);
}

void MSDOS_Test_Abort (void)
{
  if (opts.Options & EXITENABLE) 
  {
    if (kbhit()) 
    { 
      if (In_Graph_Mode)
      {
         Stop_Flag = TRUE; MSDOS_getch();
      }
      else
      {
        Status_Info("\n **** Paused ****  Press 'C' to continue.");
        MSDOS_getch();
        if (!(Stop_Flag = Viewit()))
        {
           Status_Info("\n Program continued.");
        }
      }
    }
  }
}

static void Update_Tabs(WDATA *Data)
  {
  WDATA *Temp;
  
  if (!(Data->Ovr))
    return;
    
  if (Data==OnTop)
    return;
    
  OnTop=Data;
  Data->CurY=LastY;

  if (LastY<20)
  {
    CON_setWindow(1,Data->CurY+Data->Wy1,78,Data->Wy2);
    CON_setForeColor(Data->Fcol);
    CON_setBackColor(Data->Bcol);
    CON_setBackground(CON_getAttr(),32);
    CON_clrscr();
  }
  else
  {
    if (LastY==20)
    {
       CON_setWindow(1,2,78,22);
       CON_gotoxy(78,20);
       CON_setForeColor(Data->Fcol);
       CON_setBackColor(Data->Bcol);
       CON_setBackground(CON_getAttr(),32);
       CON_putc('\n');
    }
  }
  CON_setWindow(0,0,79,24);
  
  for(Temp=Root; Temp!=NULL; Temp=Temp->Next)
  {
    Do_Tab(Temp);
  }
}

static void Do_Tab(WDATA *Data)
{
   if (!(Data->Ovr))
     return;
     
   CON_gotoxy(Data->TabX,23);
   if(Data==OnTop)
   {
     CON_setForeColor(Data->Fcol);
     CON_setBackColor(Data->Bcol);
   }
   else
   {
     CON_setForeColor(7);
     CON_setBackColor(1);
   }
   CON_setBackground(CON_getAttr(),32);
   CON_printf("\\%s/",Data->Name);
}

static void Slowit(void)
{
   time_t l, lt;
   lt = l = time(&l); 
   while (time(&l) < lt + 1);
}

static int Scan_Back(char *Buf,int *Strt)
{
   int i=(*Strt)-1;
   
   if((*Strt)==0)
     return(0);
   
   while (Buf[i]!=0)
     i--;
   *Strt=i;
   return (1);
}

static int Scan_Front(char *Buf,int *Strt,int End)
{
   int i=(*Strt)+1;
   
   if ((*Strt)==End)
     return(0);
   
   while (Buf[i]!=0)
     i++;
   *Strt=i;
   return (1);
}

static int Viewit(void)
{
  WDATA *Data;
  int i, j, k, Key, Flag;
  unsigned char *tb;

  for(Data=Root; Data!=NULL; Data=Data->Next)
  {
     if (Data->Wrap)
     {
       tb=(unsigned char *)malloc(SCROLL_BACK_SIZE);
       i=SCROLL_BACK_SIZE - Data->Cur;
       memcpy(tb,&(Data->Buf[Data->Cur]),i);
       memcpy(&(tb[i]),Data->Buf,Data->Cur);
       memcpy(Data->Buf,&(tb[1]),SCROLL_BACK_SIZE-1);
       free(tb);
       Data->Cur=SCROLL_BACK_SIZE-1;
       Data->Buf[0]=0;
       Data->Wrap=0;
     }
     Data->Buf[Data->Cur]=0;
     Data->Top=Data->Cur;
     for (i=0;i<21;i++)
     {
        Scan_Back(Data->Buf,&(Data->Top));
     }
  }

  Key=99999;  /* code to initalize */
  Data=OnTop;
  Flag=TRUE;

  if (Data == NULL)
  {
    /* There's nothing to display. Just exit. */

    return(Flag);
  }

  while (Key>0)
  {
     if (Key==99999)
     {
       Key=335;  /* fake "End" key to initialize */
     }
     else
     {
       Key=MSDOS_getch();
     }

     switch(Key)
     {
        case 327:  /* Home    */
          Data->Top=0;
          break;

        case 328:  /* Up      */
          Scan_Back(Data->Buf,&(Data->Top));
          break;

        case 331:  /* Left     */
        case   2:  /* Shft Tab */
          if (Data->Next==NULL)
          {
             while(Data->Prev!=NULL)
             {
               Data=Data->Prev;
             }
          }
          else
          {
            Data=Data->Next;
          }
          break;

        case 333:  /* Right   */
        case   9:  /* Tab     */
          if (Data->Prev==NULL)
          {
            while(Data->Next!=NULL)
            {
               Data=Data->Next;
            }
          }
          else
          {
            Data=Data->Prev;
          }
          break;

        case 335:  /* End     */
          Data->Top=Data->Cur;
          Scan_Back(Data->Buf,&(Data->Top));
        case 329:  /* Pg.Up.  */
          for (i=0;i<20;i++)
          {
            Scan_Back(Data->Buf,&(Data->Top));
          }
          break;

        case 336:  /* Down    */
          Scan_Front(Data->Buf,&Data->Top,Data->Cur);
          break;

        case 337:  /* Pg.Dn.  */
          for (i=0;i<20;i++)
          {
            Scan_Front(Data->Buf,&Data->Top,Data->Cur);
          }
          break;

        case 'c':
        case 'C':
          Flag=FALSE;  /* fall through and exit */

        default:
          Key=-1;
     }
     if (Data->Top==Data->Cur)
     {
        Scan_Back(Data->Buf,&(Data->Top));
     }
     OnTop=NULL; /*force redisplay*/
     Update_Tabs(Data);
     CON_setWindow(1,Data->Wy1,78,Data->Wy2);
     CON_setForeColor(Data->Fcol);
     CON_setBackColor(Data->Bcol);
     CON_setBackground(CON_getAttr(),32);
     CON_clrscr();
     j=Data->Top;
     k=j+1;
     for(i=0;i<21;i++)
     {
        CON_gotoxy(0,i);
        if (Scan_Front(Data->Buf,&j,Data->Cur))
        {
           CON_puts(&(Data->Buf[k]));
           k=j+1;
        }
     }
     CON_setWindow(70,23,78,23);
     CON_setForeColor(15);
     CON_setBackColor(1);
     CON_setBackground(CON_getAttr(),205);
     CON_putc('\n');
     CON_setForeColor(15);
     CON_setBackColor(4);
     CON_setBackground(CON_getAttr(),205);
     CON_puts("\033\032");
     if (Data->Top!=0)
     {
       CON_gotoxy(2,0);
       CON_puts("\030More");
     }
     if (j!=Data->Cur)
     {
       CON_gotoxy(3,0);
       CON_puts("More\031");
     }
  }
  CON_setWindow(70,23,78,23);
  CON_setForeColor(15);
  CON_setBackColor(1);
  CON_setBackground(CON_getAttr(),205);
  CON_putc('\n');
  return (Flag);
}

void MSDOS_Save_Text(void)
{
#ifdef FANCY_TEXT
   Saved_Text=(char *)POV_MALLOC(80*25*2,"text screen");
   
   CON_setWindow(0,0,79,24);
   CON_saveText(0,0,79,24,Saved_Text);
#endif
}

void MSDOS_Restore_Text(void)
{
#ifdef FANCY_TEXT
     CON_setWindow(0,0,79,24);
     CON_restoreText(0,0,79,24,Saved_Text);
     POV_FREE(Saved_Text);
#endif
}

int MSDOS_System(char *s)
{
  int Code;
  int i;
  char *ARGS[20];
  char *t;
  char buf[POV_MAX_CMD_LENGTH];
  
  Render_Info("\nExecuting '%s'",s);
  strcpy(buf,s);
  t = ARGS[0] = strtok(buf, " ");
  i=1;
  while ((t!=NULL)&&(i<20))
  {
    t = ARGS[i] = strtok(0, " ");
    i++;
  }
  MSDOS_Save_Text();
  CON_setForeColor(7);
  CON_setBackColor(0);
  CON_setBackground(CON_getAttr(),32);
  CON_setWindow(0,0,79,24);
  CON_gotoxy(79,24);
  CON_printf("\n\r");
  fprintf(stderr,"\nPOV-Ray shelling out to dos.\n%s\n",s);
  Code=spawnvp(P_WAIT,ARGS[0],ARGS);
  MSDOS_Restore_Text();
  return(Code);
}

void MSDOS_Process_Povray_Ini(char *s)
{
  char *buff;
  char *t;
  
  if (Option_String_Ptr != NULL) 
  {
     return;
  }
  
  buff=(char *)POV_MALLOC(strlen(s)+13,"temp string");
  
  strcpy(buff,s);
  
  t=strrchr(buff,'\\');
  if (t==NULL)
  {
    t=strrchr(buff,'/');
    if (t==NULL)
    {
      t=strrchr(buff,':');
      if (t==NULL)
      {
         t=buff;
      }
    }
  }

  strcpy(t,"\\povray.ini");

  parse_ini_file(buff);
  
  POV_FREE(buff);
  
}

void MSDOS_Process_Env(void)
{
 if ((Option_String_Ptr = getenv("POVINI")) != NULL) 
 {
   if (!parse_ini_file(Option_String_Ptr))
   {
     Warning(0.0,"Could not find '%s' as specified in POVINI environment.\n",
      Option_String_Ptr);
   }
 }
}

void MSDOS_Other_Credits(void)
{
  Banner("MS-Dos text & video: Chris Young, Chris Cason & Kendall Bennett\n");
#ifdef __CAUSEWAY__
  Banner("CauseWay DOS Extender courtesy Michael Devore");
#endif
  Banner("\n");
}

#ifdef __WATCOMC__                        /* Watcom C/C++ C32 */
void Fix_Watcom_Bug(char *s)
{
  char *p=s;
  
  while(*p != '\0')
  {
    if (*p==' ')
    {
      *p='0';
    }
    p++;
  }  
}
#endif

