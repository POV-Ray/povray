## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include $(GLUT_CFLAGS)

check_PROGRAMS = accum bitmap1 bitmap2 blendeq blendxor copy cursor depth \
	eval fog font line logo nurb oglinfo olympic overlay point prim \
	quad select shape sphere star stencil stretch texture tri wave

EXTRA_PROGRAMS = rgbtoppm

EXTRA_DIST = tkmap.c loadppm.c

LDADD = $(GLUT_LIBS) $(top_builddir)/$(GLU_DIR)/libGLU.la \
	$(top_builddir)/src/libGL.la -lm
#LDFLAGS = -no-install

# default image
IMAGE = girl

# use double buffering if possible
BUFFER = -db

CLEANFILES = rgbtoppm $(IMAGE).ppm

# execute all programs
exec: $(check_PROGRAMS) $(IMAGE).ppm
	@for prog in $(check_PROGRAMS);	\
	do				\
	 echo -n "Running $$prog ..." ;	\
	 case "$$prog" in		\
	 copy) \
	    ./$$prog -f $(IMAGE).ppm ;;	\
	 quad | sphere | stretch | texture) \
	    ./$$prog $(BUFFER) -f $(IMAGE).ppm ;; \
	 bitmap* | blend* | font | select | shape | stencil) \
	    ./$$prog ;; 		\
	 *) ./$$prog $(BUFFER) ;;	\
	 esac ;				\
	 echo ;				\
	done

$(IMAGE).ppm: rgbtoppm
	./rgbtoppm $(top_srcdir)/images/$(IMAGE).rgb > $(IMAGE).ppm


include $(top_srcdir)/common_rules.make
