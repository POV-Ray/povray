!include <win32.mak>

CFLAGS        = $(cvarsdll) /Ox /G5 /D__MSC__ /DFX /D__WIN32__ \
                /DWIN32 /DMESA_MINWARN /I..\include

OBJS          = glut_8x13.obj glut_9x15.obj glut_bitmap.obj glut_bwidth.obj glut_cindex.obj \
                glut_cursor.obj glut_dials.obj glut_dstr.obj glut_event.obj glut_ext.obj \
                glut_fullscrn.obj glut_gamemode.obj glut_get.obj glut_hel10.obj glut_hel12.obj glut_hel18.obj \
                glut_init.obj glut_input.obj glut_joy.obj glut_key.obj glut_keyctrl.obj glut_keyup.obj \
				glut_mesa.obj glut_modifier.obj glut_mroman.obj \
                glut_overlay.obj glut_roman.obj glut_shapes.obj glut_space.obj glut_stroke.obj \
                glut_swidth.obj glut_tablet.obj glut_teapot.obj glut_tr10.obj glut_tr24.obj \
                glut_util.obj glut_vidresize.obj glut_warp.obj glut_win.obj glut_winmisc.obj \
				glut_swap.obj glut_cmap.obj \
                win32_glx.obj win32_menu.obj win32_util.obj win32_winproc.obj win32_x11.obj

PROGRAM       = ..\lib\GLUT32.dll

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
                $(link) $(dlllflags) /out:$(PROGRAM) \
                        /def:fxglut.def $(OBJS) $(guilibsdll) ..\lib\GLU32.lib ..\lib\OpenGL32.lib winmm.lib > link.log