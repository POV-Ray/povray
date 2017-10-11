/* ---------------------------------------------------------------------------
*  WINDOWS.C
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
#include <WINDOWS.H>

long FAR PASCAL WndProc(HWND,UINT,UINT,LONG);
HWND Global_Wnd;
HDC hdc;

int PASCAL WinMain(HANDLE hInstance,
                   HANDLE hPrevInstance,
                   LPSTR lpzsCmdLine,
                   int nCmdShow) {
  static char szAppName[]="POVLAB";
  HWND hwnd;
  MSG msg;
  WNDCLASS wndclass;

  if (!hPrevInstance) {
    wndclass.style=CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc= (LPVOID) WndProc;
    wndclass.cbClsExtra=0;
    wndclass.cbWndExtra=0;
    wndclass.hInstance=hInstance;
    wndclass.hIcon=LoadIcon(hInstance,"ICONE\POVLAB.ICO");
    wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
    wndclass.hbrBackground=GetStockObject(GRAY_BRUSH);
    wndclass.lpszMenuName=szAppName;
    wndclass.lpszClassName=szAppName;

    RegisterClass(&wndclass);
  }

  Global_Wnd=hwnd=CreateWindow(szAppName,"POVLAB for Windows 4.0 beta 1",
                   WS_OVERLAPPEDWINDOW,
                   0,
                   0,
                   640,
                   480, NULL, NULL, hInstance, NULL);

  ShowWindow(hwnd,nCmdShow);
  UpdateWindow(hwnd);

  while (GetMessage(&msg,NULL,0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return msg.wParam;
}

// ---------------------------------------------------------------------------
// ---------------------- PROGRAMME PRINCIPAL --------------------------------
// ---------------------------------------------------------------------------
long far PASCAL _export WndProc(HWND hwnd,
                                UINT message,
                                UINT wParam,
                                LONG lParam) {
  PAINTSTRUCT ps;
  int i;
  RECT Rect;
  
  switch (message) {
    case WM_CREATE:
      init_soft(LOGICIEL,VERSION);

      _fpreset();
      _control87(_PC_64,MCW_PC);

      init_disk(Arg[0]);
      lecture_config_interface();
      analyse_arguments();
      init_gmode(0);
      init_police(1,0);
      MemoireLibre=init_taille_memoire();

      sauve_config_interface(0);
      // lecture_fichier(NULL,0,0,1);
      // charge_motif(SAUVE);

      
      // lecture_fichier(NULL,0,0,0);
      return 0;
    case WM_PAINT:
      hdc=BeginPaint(hwnd,&ps);
      GetClientRect(hwnd,&Rect);
      /*
      place_mouse(WX/2,WY/2);
      trouve_fenetre(1);
      place_mouse(XMax/2,YMax/2);

      affiche_donnees();
      if (EXEC1) a_propos_de();
      place_mouse(CentX/2,0);
      */
      interface(1);

      DrawText(hdc,"Hello, POVLAB !",-1,&Rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      EndPaint(hwnd,&ps);
      return 0;
    case WM_COMMAND:
      choix_principal();
      return 0;
    case WM_DESTROY:
      for (i=0;i<=NbObjet;i++) free_mem_objet(i);
      for (i=0;i<=NbPoly;i++) free_mem_poly(i);
      for (i=0;i<=NbSpline;i++) free_mem_spline(i);
      GMouseOff();
      sauve_config_interface(1);
      fading_palette(FADEIN);
      charge_motif(EFFACE);
      fin_mode_graphique();
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProc(hwnd,message,wParam,lParam);
}

