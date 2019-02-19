//******************************************************************************
///
/// @file windows/pvguiext.h
///
/// This file contains POV-Ray for Windows GUI Extension specific defines.
///
/// @author Christopher J. Cason.
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
//*******************************************************************************

#ifndef PVGUIEXT_H_INCLUDED
#define PVGUIEXT_H_INCLUDED

#include "frontend/display.h"

#define MAX_GUI_EXT               32
#define GUI_INTERFACE_VERSION     102

#define CONTACT "Do not contact the POV-Team about this error. Contact the author of this extension - \n\n\t" AUTHOR " at " EMAIL

namespace povwin
{

using namespace pov;

typedef pov_frontend::Display::RGBA8 GuiRGBA8;
typedef COLC COLOUR[5];

#ifndef POVWIN_FILE
// WARNING: also declared in pvengine.h (for a reason)
// KEEP THESE IN SYNC
typedef enum
{
  mUnknown = 0,
  mAll = 1,
  All = 1,
  mIDE,
  mBanner,
  mWarning,
  mRender,
  mStatus,
  mDebug,
  mFatal,
  mStatistics,
  mDivider,
  mHorzLine,
} msgtype ;
#endif

#define MAX_QUEUE       512
#define OLD_MAX_QUEUE   128

typedef struct
{
  int                   RecSize ;
  char                  command_line [_MAX_PATH * 3] ;
  char                  source_file_name [_MAX_PATH] ;
  char                  lastRenderName [_MAX_PATH] ;
  char                  lastRenderPath [_MAX_PATH] ;
  char                  lastQueuePath [_MAX_PATH] ;
  char                  lastSecondaryIniFilePath [_MAX_PATH] ;
  char                  DefaultRenderIniFileName [_MAX_PATH] ;
  char                  SecondaryRenderIniFileName [_MAX_PATH] ;
  char                  SecondaryRenderIniFileSection [64] ;
  char                  ourPath [_MAX_PATH] ;
  char                  engineHelpPath [_MAX_PATH] ;
  char                  rendererHelpPath [_MAX_PATH] ;
  char                  BinariesPath [_MAX_PATH] ;
  char                  EngineIniFileName [_MAX_PATH] ;
  char                  ToolIniFileName [_MAX_PATH] ;
  unsigned              loadRerun ;
  unsigned              continueRerun ;
  unsigned              povray_return_code ;
  BOOL                  rendering ;
  BOOL                  IsWin32 ;
  BOOL                  IsW95UserInterface ;
  BOOL                  running_demo ;
  BOOL                  debugging ;
  BOOL                  isMaxiMinimized ;
  BOOL                  newVersion ;
  BOOL                  use_threads ;
  BOOL                  use_toolbar ;
  BOOL                  use_tooltips ;
  BOOL                  use_editors ;
  BOOL                  drop_to_editor ;
  BOOL                  rendersleep ;
  BOOL                  ExtensionsEnabled ;
  char                  queued_files [OLD_MAX_QUEUE] [_MAX_PATH] ;
  unsigned              queued_file_count ;
  unsigned              auto_render ;
  DWORD                 Reserved [32] ;
} ExternalVarStruct ;

typedef enum
{
  EventFirst,
  EventStartRendering,
  EventStopRendering,
  EventDisplayInit,
  EventDisplayFinished,
  EventDisplayClose,
  EventWinStartup,
  EventWinFinish,
  EventWinCooperate,
  EventLoadToolMenu,
  EventTimer,
  EventSize,
  EventMove,
  EventClose,
  EventDestroy,
  EventLast
} ExternalEvents ;

typedef enum
{
  RequestFirst,
  RequestGetVars,
  RequestSetVars,
  RequestStartRendering,
  RequestStopRendering,
  RequestExit,
  RequestSetCooperateLevel,
  RequestLast
} ExternalRequests ;

typedef enum
{
  ExRequestDisabled,
  ExRequestOK,
  ExRequestFailed,
  ExRequestFailedRendering,
  ExRequestFailedNotRendering,
  ExRequestBadRecSize,
  ExRequestUnknown
} ExternalRequestResult ;

typedef enum
{
  dfRealDrop,
  dfRenderEditor,
  dfRenderMessage,
  dfRenderCommandLine,
  dfRenderSourceFile,
  dfRenderFileQueue
} ExternalDropType ;

typedef struct
{
  // data POV passes to the DLL
  LPSTR                 PovVersion ;
  LPSTR                 GuiVersion ;
  DWORD                 GuiInterfaceVersion ;
  WPARAM                FirstMenuItem ;
  HINSTANCE             hInst ;
  HWND                  MainWindow ;
  ExternalRequestResult (WINAPI *ExternalRequest) (ExternalRequests Request, void *RequestBlock) ;

  // data the DLL passes to POV
  LPSTR                 Name ;
  LPSTR                 Author ;
  LPSTR                 AuthorEmail ;
  HMENU                 hMenu ;
  DWORD                 DLLInterfaceVersion ;
  char                  Agreement [1024] ;
} GuiExtInitStruct ;

typedef struct
{
  DWORD                 InstanceID ;
  HMENU                 hMenu ;
  WPARAM                FirstMenuItem ;
} IDataStruct ;

typedef struct
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
  DWORD                 Reserved [128] ;
} GuiPointerBlock_Version_100 ;

typedef struct
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
  DWORD                 Reserved [127] ;
} GuiPointerBlock_Version_101 ;

typedef struct
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
  DWORD                 Reserved [126] ;
} GuiPointerBlock ;

DWORD ExternalEvent (ExternalEvents Event, DWORD EventVal) ;
void ExternalDisplayPlot (int x, int y, int Red, int Green, int Blue, int Alpha) ;
void ExternalDisplayPlotBlock (int x1, int y1, int x2, int y2, const GuiRGBA8* colour) ;
void ExternalDisplayPlotRect (int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
void ExternalWinPrePixel (int x, int y, COLOUR colour) ;
void ExternalWinPostPixel (int x, int y, COLOUR colour) ;
void ExternalAssignPixel (int x, int y, COLOUR colour) ;
BOOL ExternalWinSystem (LPSTR command, int *returnval) ;
void ExternalCleanupAll (void) ;
void ExternalBufferMessage (msgtype message_type, LPSTR message) ;
void ExternalParseToolCommand (char command [512]) ;
BOOL ExternalDragFunction (LPSTR szFile, ExternalDropType DropType) ;
void LoadGUIExtensions (void) ;
DWORD ExternalMenuSelect (WPARAM Code) ;
char *ExternalMenuTip (WPARAM wParam) ;
BOOL CheckGUIExtLoaded (const char *Name) ;

}
// end of namespace povwin

#endif // PVGUIEXT_H_INCLUDED
