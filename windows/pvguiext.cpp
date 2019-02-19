//******************************************************************************
///
/// @file windows/pvguiext.cpp
///
/// This file contains POV-Ray for Windows GUI Extension support code.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#define POVWIN_FILE
#define _WIN32_IE COMMONCTRL_VERSION

#include <setjmp.h>
#include <string.h>

#include <windows.h>
#include <commctrl.h>

#include "pvengine.h"
#include "pvedit.h"
#include "resource.h"
#include "pvdialog.h"
#include "pvguiext.h"

// this must be the last file included
#include "syspovdebug.h"

namespace povwin
{

typedef struct _RGPB0X0100
{
  DWORD                 Signature ;
  DWORD                 InstanceID ;
  BOOL                  (WINAPI *Init) (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct) ;
  void                  (WINAPI *Destroy) (IDataStruct *InstanceData) ;
  DWORD                 (WINAPI *MenuSelect) (IDataStruct *InstanceData, WPARAM Code) ;
  LPSTR                 (WINAPI *MenuTip) (IDataStruct *InstanceData, WPARAM Code) ;
  DWORD                 (WINAPI *Event) (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal) ;
  void                  (WINAPI *DisplayPlot) (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *DisplayPlotRect) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *WinPrePixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *WinPostPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  BOOL                  (WINAPI *WinSystem) (IDataStruct *InstanceData, LPSTR command, int *returnval) ;
  void                  (WINAPI *CleanupAll) (IDataStruct *InstanceData) ;
  void                  (WINAPI *BufferMessage) (IDataStruct *InstanceData, msgtype message_type, LPSTR message) ;
  LPSTR                 (WINAPI *ParseToolCommand) (IDataStruct *InstanceData, char command [512]) ;
  BOOL                  (WINAPI *DragFunction) (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType) ;
  struct _RGPB0X0100    *Next ;
  int                   IniID ;
  IDataStruct           InstanceData ;
  char                  Name [256] ;
  char                  Author [256] ;
  char                  AuthorEmail [256] ;
  char                  FileName [_MAX_PATH] ;
} RealGuiPointerBlock_Version_100 ;

typedef struct _RGPB0X0101
{
  DWORD                 Signature ;
  DWORD                 InstanceID ;
  BOOL                  (WINAPI *Init) (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct) ;
  void                  (WINAPI *Destroy) (IDataStruct *InstanceData) ;
  DWORD                 (WINAPI *MenuSelect) (IDataStruct *InstanceData, WPARAM Code) ;
  LPSTR                 (WINAPI *MenuTip) (IDataStruct *InstanceData, WPARAM Code) ;
  DWORD                 (WINAPI *Event) (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal) ;
  void                  (WINAPI *DisplayPlot) (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *DisplayPlotRect) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *WinPrePixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *WinPostPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  BOOL                  (WINAPI *WinSystem) (IDataStruct *InstanceData, LPSTR command, int *returnval) ;
  void                  (WINAPI *CleanupAll) (IDataStruct *InstanceData) ;
  void                  (WINAPI *BufferMessage) (IDataStruct *InstanceData, msgtype message_type, LPSTR message) ;
  LPSTR                 (WINAPI *ParseToolCommand) (IDataStruct *InstanceData, char command [512]) ;
  BOOL                  (WINAPI *DragFunction) (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType) ;
  void                  (WINAPI *AssignPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  struct _RGPB0X0101    *Next ;
  int                   IniID ;
  IDataStruct           InstanceData ;
  char                  Name [256] ;
  char                  Author [256] ;
  char                  AuthorEmail [256] ;
  char                  FileName [_MAX_PATH] ;
} RealGuiPointerBlock_Version_101 ;

typedef struct _RGPB
{
  DWORD                 Signature ;
  DWORD                 InstanceID ;
  BOOL                  (WINAPI *Init) (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct) ;
  void                  (WINAPI *Destroy) (IDataStruct *InstanceData) ;
  DWORD                 (WINAPI *MenuSelect) (IDataStruct *InstanceData, WPARAM Code) ;
  LPSTR                 (WINAPI *MenuTip) (IDataStruct *InstanceData, WPARAM Code) ;
  DWORD                 (WINAPI *Event) (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal) ;
  void                  (WINAPI *DisplayPlot) (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *DisplayPlotRect) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *WinPrePixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *WinPostPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  BOOL                  (WINAPI *WinSystem) (IDataStruct *InstanceData, LPSTR command, int *returnval) ;
  void                  (WINAPI *CleanupAll) (IDataStruct *InstanceData) ;
  void                  (WINAPI *BufferMessage) (IDataStruct *InstanceData, msgtype message_type, LPSTR message) ;
  LPSTR                 (WINAPI *ParseToolCommand) (IDataStruct *InstanceData, char command [512]) ;
  BOOL                  (WINAPI *DragFunction) (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType) ;
  void                  (WINAPI *AssignPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *DisplayPlotBlock) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, const GuiRGBA8* colour) ;
  struct _RGPB          *Next ;
  int                   IniID ;
  IDataStruct           InstanceData ;
  char                  Name [256] ;
  char                  Author [256] ;
  char                  AuthorEmail [256] ;
  char                  FileName [_MAX_PATH] ;
} RealGuiPointerBlock ;

DWORD                             CooperateLevel = 9 ; // this should really be per-GUIEXT rather than global
GuiExtInitStruct                  InitStruct ;
RealGuiPointerBlock               *GuiPointerBlocks = NULL ;

DWORD (WINAPI *GuiExtGetPointers) (int RecSize, GuiPointerBlock *PointerBlock) ;

extern char                       source_file_name [_MAX_PATH] ;
extern bool                       ExtensionsEnabled ;
extern bool                       rendering ;
extern bool                       stop_rendering ;
extern bool                       hide_newuser_help ;
extern bool                       debugging;
extern HWND                       main_window ;
extern HMENU                      hPluginsMenu ;

void getvars (ExternalVarStruct *v) ;
void setvars (ExternalVarStruct *v) ;

DWORD ExternalEvent (ExternalEvents Event, DWORD EventVal)
{
  DWORD                 rval = 0 ;
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (0) ;
  if (Event == EventWinCooperate)
    if (EventVal >= CooperateLevel)
      return (0) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->Event != NULL)
      rval |= (*p->Event ) (&p->InstanceData, Event, EventVal) ;
    p = p->Next ;
  }
  return (rval) ;
}

void ExternalDisplayPlot (int x, int y, int Red, int Green, int Blue, int Alpha)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DisplayPlot != NULL)
      (*p->DisplayPlot) (&p->InstanceData, x, y, Red, Green, Blue, Alpha) ;
    p = p->Next ;
  }
}

void ExternalDisplayPlotBlock (int x1, int y1, int x2, int y2, const GuiRGBA8* colour)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;

  int pixelsCount = ( x2 - x1 + 1 ) * ( y2 - y1 + 1 );
  GuiRGBA8* localColourCopy = NULL;

  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DisplayPlotBlock != NULL)
    {
      if (localColourCopy == NULL)
        localColourCopy = new GuiRGBA8[ pixelsCount ];
      memcpy( localColourCopy, colour, pixelsCount * sizeof( GuiRGBA8 ) );
      (*p->DisplayPlotBlock) (&p->InstanceData, x1, y1, x2, y2, localColourCopy ) ;
    }
    p = p->Next ;
  }
  delete[] localColourCopy;
}

void ExternalDisplayPlotRect (int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DisplayPlotRect != NULL)
      (*p->DisplayPlotRect) (&p->InstanceData, x1, y1, x2, y2, Red, Green, Blue, Alpha) ;
    p = p->Next ;
  }
}

void ExternalWinPrePixel (int x, int y, COLOUR colour)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->WinPrePixel != NULL)
      (*p->WinPrePixel) (&p->InstanceData, x, y, colour) ;
    p = p->Next ;
  }
}

void ExternalWinPostPixel (int x, int y, COLOUR colour)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->WinPostPixel != NULL)
      (*p->WinPostPixel) (&p->InstanceData, x, y, colour) ;
    p = p->Next ;
  }
}

void ExternalAssignPixel (int x, int y, COLOUR colour)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->AssignPixel != NULL)
      (*p->AssignPixel) (&p->InstanceData, x, y, colour) ;
    p = p->Next ;
  }
}

BOOL ExternalWinSystem (LPSTR command, int *returnval)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (false) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->WinSystem != NULL)
      if ((*p->WinSystem) (&p->InstanceData, command, returnval))
        return (true) ;
    p = p->Next ;
  }
  return (false) ;
}

void ExternalCleanupAll (void)
{
  // always enabled
  while (GuiPointerBlocks)
  {
    if (GuiPointerBlocks->Destroy != NULL)
      (*GuiPointerBlocks->Destroy) (&GuiPointerBlocks->InstanceData) ;
    GuiPointerBlocks = GuiPointerBlocks->Next ;
  }
}

void ExternalBufferMessage (msgtype message_type, LPSTR message)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->BufferMessage != NULL)
      (*p->BufferMessage) (&p->InstanceData, message_type, message) ;
    p = p->Next ;
  }
}

void ExternalParseToolCommand (char command [512])
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->ParseToolCommand != NULL)
      (*p->ParseToolCommand) (&p->InstanceData, command) ;
    p = p->Next ;
  }
}

BOOL ExternalDragFunction (LPSTR szFile, ExternalDropType DropType)
{
  int                   n ;
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (false) ;
  n = get_file_type (szFile) ;
  if (n == filePOV || n == fileINI)
    return (false) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DragFunction != NULL)
      if ((*p->DragFunction) (&p->InstanceData, szFile, DropType))
        return (true) ;
    p = p->Next ;
  }
  return (false) ;
}

ExternalRequestResult WINAPI ExternalRequest (ExternalRequests Request, void *RequestBlock)
{
  if (!ExtensionsEnabled)
    return (ExRequestDisabled) ;
  switch (Request)
  {
    case RequestGetVars :
         if (((ExternalVarStruct *) RequestBlock)->RecSize != sizeof (ExternalVarStruct))
           return (ExRequestBadRecSize) ;
         getvars ((ExternalVarStruct *) RequestBlock) ;
         return (ExRequestOK) ;

    case RequestSetVars :
         if (((ExternalVarStruct *) RequestBlock)->RecSize != sizeof (ExternalVarStruct))
           return (ExRequestBadRecSize) ;
         setvars ((ExternalVarStruct *) RequestBlock) ;
         return (ExRequestOK) ;

    case RequestSetCooperateLevel :
         CooperateLevel = *(DWORD *) RequestBlock ;
         return (ExRequestOK) ;

    case RequestStartRendering :
         if (rendering)
           return (ExRequestFailedRendering) ;
         message_printf ("Rendering started by GUI extension\n") ;
         start_rendering (false) ;
         return (ExRequestOK) ;

    case RequestStopRendering :
         if (!rendering)
           return (ExRequestFailedNotRendering) ;
         stop_rendering = true ;
         message_printf ("Rendering stopped by GUI extension\n") ;
         SendMessage (main_window, WM_COMMAND, CM_STOPRENDER, 0) ;
         return (ExRequestOK) ;

    case RequestExit :
         if (rendering)
           return (ExRequestFailedRendering) ;
         message_printf ("Exit requested by GUI extension\n") ;
         Sleep (1000) ;
         SendMessage (main_window, WM_CLOSE, 0, 0) ;
         return (ExRequestOK) ;
  }
  return (ExRequestUnknown) ;
}

DWORD ExternalMenuSelect (WPARAM Code)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (1) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (Code >= p->InstanceData.FirstMenuItem && Code <= p->InstanceData.FirstMenuItem + 59)
      return ((*p->MenuSelect) (&p->InstanceData, Code - p->InstanceData.FirstMenuItem)) ;
    p = p->Next ;
  }
  return (1) ;
}

char *ExternalMenuTip (WPARAM wParam)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ("Extensions Disabled") ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (wParam >= p->InstanceData.FirstMenuItem && wParam <= p->InstanceData.FirstMenuItem + 59)
      return ((*p->MenuTip) (&p->InstanceData, wParam - p->InstanceData.FirstMenuItem)) ;
    p = p->Next ;
  }
  return ("") ;
}

bool ValidateMenu (HMENU hMenu, int first, int last)
{
  int         count ;
  int         i ;
  int         id ;
  HMENU       hSubMenu ;

  count = GetMenuItemCount (hMenu) ;
  for (i = 0 ; i < count ; i++)
  {
    if ((id = GetMenuItemID (hMenu, i)) == 0xffffffff)
    {
      // could be a popup menu
      if ((hSubMenu = GetSubMenu (hMenu, i)) == NULL)
        continue ;
      if (!ValidateMenu (hSubMenu, first, last))
        return (false) ;
      continue ;
    }
    if (id == 0 || id == 0xffff)
      continue ;
    if (id < first || id > last)
      return (false) ;
  }
  return (true) ;
}

void LoadGUIExtensions (void)
{
  int                   i ;
  int                   FirstMenuItem = CM_FIRSTGUIEXT ;
  char                  str [64] ;
  char                  filename [_MAX_PATH] ;
  char                  name [_MAX_PATH] ;
  bool                  first = true ;
  bool                  loaded = false ;
  HINSTANCE             hLib ;
  GuiPointerBlock       PointerBlock ;
  RealGuiPointerBlock   *RealPointerBlock ;
  RealGuiPointerBlock   *LastPointerBlock = NULL ;

  if (!ExtensionsEnabled)
    return ;
  for (i = 0 ; i < MAX_GUI_EXT ; i++)
  {
    sprintf (str, "ExtDll%02d", i) ;
    GetHKCU("GuiExtensions", str, "", filename, sizeof (filename));
    if (filename [0] != '\0')
    {
      if (first)
      {
        first = false ;
        buffer_message (mDivider, "\n") ;
        message_printf ("DISCLAIMER : The POV-Team takes no responsibility for the operation of third-party GUI Extensions.\n") ;
        message_printf ("             Please do not send us bug reports if they cause things to go haywire.\n\n") ;
      }

      splitpath (filename, NULL, name) ;
      if ((hLib = LoadLibrary (filename)) == NULL)
      {
        if (debugging)
          debug_output ("Could not load GUI Extension DLL '%s', error code is %08lx\n", filename, GetLastError ()) ;
        message_printf ("Could not load GUI Extension DLL '%s'\n", name) ;
        continue ;
      }
      GuiExtGetPointers = (unsigned long(WINAPI *)(int, GuiPointerBlock *))GetProcAddress (hLib, "PovGuiExtensionGetPointers") ;
      if (GuiExtGetPointers == NULL)
      {
        if (debugging)
          debug_output ("Could not get Init process address for GUI extension '%s', error code is %08lx\n", filename, GetLastError ()) ;
        message_printf ("Could not get Init address for GUI Extension DLL '%s'\n", name) ;
        message_printf ("  Contact author of '%s'\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      memset (&PointerBlock, 0, sizeof (PointerBlock)) ;
      if (GuiExtGetPointers (sizeof (PointerBlock), &PointerBlock) == 0)
      {
        if (debugging)
          debug_output ("Could not get pointers for GUI extension '%s'\n", filename) ;
        message_printf ("Could not get pointers for GUI extension '%s'\n", name) ;
        message_printf ("  Contact author of '%s'\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (PointerBlock.Init == NULL ||
          PointerBlock.MenuSelect == NULL ||
          PointerBlock.MenuTip == NULL ||
          (PointerBlock.Signature != 'CJC!' && PointerBlock.Signature != '!CJC'))
      {
        if (debugging)
          debug_output ("PointerBlock not correctly initialized by GUI extension '%s'\n", filename) ;
        message_printf ("PointerBlock not correctly initialized by GUI extension '%s'\n", name) ;
        message_printf ("  Contact author of '%s'\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      memset (&InitStruct, 0, sizeof (InitStruct)) ;
      InitStruct.PovVersion = "" ;
      InitStruct.GuiVersion = "" ;
      InitStruct.GuiInterfaceVersion = GUI_INTERFACE_VERSION ;
      InitStruct.hInst = hInstance ;
      InitStruct.MainWindow = main_window ;
      InitStruct.ExternalRequest = ExternalRequest ;
      InitStruct.FirstMenuItem = FirstMenuItem ;
      if ((*PointerBlock.Init) (PointerBlock.InstanceID, sizeof (InitStruct), &InitStruct) == 0)
      {
        if (debugging)
          debug_output ("Init () failed (return code == 0) for GUI extension '%s'\n", filename) ;
        message_printf ("Init () failed (return code == 0) for GUI extension '%s'\n", name) ;
        message_printf ("  Contact %s <%s>\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (InitStruct.Name == NULL || strlen (InitStruct.Name) < 8 ||
          InitStruct.Author == NULL || strlen (InitStruct.Author) < 8 ||
          InitStruct.AuthorEmail == NULL || strlen (InitStruct.AuthorEmail) < 8 ||
          strncmp (InitStruct.Author, "Place ", 6) == 0 ||
          strncmp (InitStruct.AuthorEmail, "Place ", 6) == 0)
      {
        if (debugging)
          debug_output ("GUI extension '%s' has invalid name/author/email\n", filename) ;
        message_printf ("GUI extension '%s' has invalid name/author/email. Contact author of add-in.\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (InitStruct.DLLInterfaceVersion / 100 != GUI_INTERFACE_VERSION / 100)
      {
        if (debugging)
          debug_output ("GUI extension '%s' ('%s') is wrong version (%d)\n", filename, InitStruct.Name, InitStruct.DLLInterfaceVersion) ;
        message_printf ("GUI extension '%s' ('%s') is wrong version (%d)\n", name, InitStruct.Name, InitStruct.DLLInterfaceVersion) ;
        message_printf ("  Contact %s <%s>\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (strcmp (InitStruct.Agreement, "The author of this POV-Ray(tm) GUI Extension DLL certifies that, at the time of "
                                        "its production or distribution, it complied with the POV-Ray Team's then current "
                                        "requirements for GUI Extensions to POV-Ray for Windows, and acknowledges that it "
                                        "is a violation of copyright to fail to do so. This text is copyright (c) the "
                                        "POV-Team 1996. Used by permission.") != 0)
      {
        if (debugging)
          debug_output ("GUI extension '%s' ('%s') has invalid agreement\n  '%s'", filename, InitStruct.Name, InitStruct.Agreement) ;
        message_printf ("GUI extension '%s' ('%s') has invalid agreement\n", name, InitStruct.Name) ;
        message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (InitStruct.hMenu == NULL || !IsMenu (InitStruct.hMenu))
      {
        if (debugging)
          debug_output ("GUI extension '%s' ('%s') has no menu\n", filename, InitStruct.Name) ;
        message_printf ("GUI extension '%s' ('%s') has no menu\n", name, InitStruct.Name) ;
        message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (!ValidateMenu (InitStruct.hMenu, FirstMenuItem, FirstMenuItem + 59))
      {
        if (debugging)
          debug_output ("GUI extension '%s' ('%s') has invalid menu ID's\n", filename, InitStruct.Name) ;
        message_printf ("GUI extension '%s' ('%s') has invalid menu ID's\n", name, InitStruct.Name) ;
        message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if ((RealPointerBlock = (RealGuiPointerBlock *)calloc (sizeof (RealGuiPointerBlock), 1)) == NULL)
      {
        message_printf ("Memory allocation error\n") ;
        FreeLibrary (hLib) ;
        continue ;
      }
      switch (InitStruct.DLLInterfaceVersion)
      {
        case 100 :
             PointerBlock.AssignPixel = NULL ;
             PointerBlock.DisplayPlotBlock = NULL;
             break ;

        case 101 :
             PointerBlock.DisplayPlotBlock = NULL;
             break ;

        case 102 :
             break;

        default :
             if (debugging)
               debug_output ("GUI extension '%s' ('%s') is for a later version of POV (DLLInterfaceVersion is %04x)\n",
                                   filename, InitStruct.Name, InitStruct.DLLInterfaceVersion) ;
             message_printf ("GUI extension '%s' ('%s') is for a later version of POV-Ray\n", name, InitStruct.Name) ;
             message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
             FreeLibrary (hLib) ;
             continue ;
      }
      memcpy (RealPointerBlock, &PointerBlock, sizeof (PointerBlock) - sizeof (PointerBlock.Reserved)) ;
      strncpy (RealPointerBlock->Name, InitStruct.Name, sizeof (RealPointerBlock->Name)) ;
      strncpy (RealPointerBlock->Author, InitStruct.Author, sizeof (RealPointerBlock->Author)) ;
      strncpy (RealPointerBlock->AuthorEmail, InitStruct.AuthorEmail, sizeof (RealPointerBlock->AuthorEmail)) ;
      strncpy (RealPointerBlock->FileName, filename, sizeof (RealPointerBlock->FileName)) ;
      RealPointerBlock->InstanceData.FirstMenuItem = FirstMenuItem ;
      RealPointerBlock->InstanceData.InstanceID = PointerBlock.InstanceID ;
      RealPointerBlock->InstanceData.hMenu = InitStruct.hMenu ;
      RealPointerBlock->IniID = i ;
      if (!loaded)
        DeleteMenu (hPluginsMenu, hide_newuser_help ? 1 : 3, MF_BYPOSITION) ;
      AppendMenu (hPluginsMenu, MF_POPUP, (UINT_PTR) InitStruct.hMenu, InitStruct.Name) ;
      FirstMenuItem += 64 ;
      message_printf ("GUI Extension '%s' loaded from '%s'\n", InitStruct.Name, name) ;
      message_printf ("  Report any problems to %s <%s>\n", InitStruct.Author, InitStruct.AuthorEmail) ;
      if (LastPointerBlock != NULL)
      {
        LastPointerBlock->Next = RealPointerBlock ;
        LastPointerBlock = RealPointerBlock ;
      }
      else
        LastPointerBlock = GuiPointerBlocks = RealPointerBlock ;
      loaded = true ;
    }
  }
  if (!first)
    buffer_message (mDivider, "\n") ;
  if (debugging && loaded)
  {
    RealPointerBlock = GuiPointerBlocks ;
    debug_output ("\nGUI extension summary:\n") ;
    while (RealPointerBlock)
    {
      debug_output ("\nExtension DLL '%s'\n\n", RealPointerBlock->FileName) ;
      debug_output ("  Name             : '%s'\n", RealPointerBlock->Name) ;
      debug_output ("  Author           : '%s'\n", RealPointerBlock->Author) ;
      debug_output ("  AuthorEmail      : '%s'\n", RealPointerBlock->AuthorEmail) ;
      debug_output ("  IniID            : %d\n", RealPointerBlock->IniID) ;
      debug_output ("  Init             : %p\n", RealPointerBlock->Init) ;
      debug_output ("  Destroy          : %p\n", RealPointerBlock->Destroy) ;
      debug_output ("  MenuSelect       : %p\n", RealPointerBlock->MenuSelect) ;
      debug_output ("  Event            : %p\n", RealPointerBlock->Event) ;
      debug_output ("  DisplayPlot      : %p\n", RealPointerBlock->DisplayPlot) ;
      debug_output ("  DisplayPlotRect  : %p\n", RealPointerBlock->DisplayPlotRect) ;
      debug_output ("  WinPrePixel      : %p\n", RealPointerBlock->WinPrePixel) ;
      debug_output ("  WinPostPixel     : %p\n", RealPointerBlock->WinPostPixel) ;
      debug_output ("  WinSystem        : %p\n", RealPointerBlock->WinSystem) ;
      debug_output ("  CleanupAll       : %p\n", RealPointerBlock->CleanupAll) ;
      debug_output ("  BufferMessage    : %p\n", RealPointerBlock->BufferMessage) ;
      debug_output ("  ParseToolCommand : %p\n", RealPointerBlock->ParseToolCommand) ;
      debug_output ("  DragFunction     : %p\n", RealPointerBlock->DragFunction) ;
      debug_output ("  FirstMenuItem    : %p\n", RealPointerBlock->InstanceData.FirstMenuItem) ;
#ifdef _WIN64
      debug_output ("  InstanceID       : %016lX\n", RealPointerBlock->InstanceData.InstanceID) ;
#else
      debug_output ("  InstanceID       : %08X\n", RealPointerBlock->InstanceData.InstanceID) ;
#endif
      debug_output ("  hMenu            : %p\n", RealPointerBlock->InstanceData.hMenu) ;
      debug_output ("  AssignPixel      : %p\n", RealPointerBlock->AssignPixel) ;
      RealPointerBlock = RealPointerBlock->Next ;
    }
    debug_output ("\n") ;
  }
}

BOOL CheckGUIExtLoaded (const char *Name)
{
  RealGuiPointerBlock *pb = GuiPointerBlocks ;
  while (pb)
  {
    if (_stricmp (pb->Name, Name) == 0)
      return (true) ;
    pb = pb->Next ;
  }
  return (false) ;
}

}
// end of namespace povwin
