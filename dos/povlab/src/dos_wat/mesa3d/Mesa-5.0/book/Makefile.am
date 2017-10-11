## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include $(GLUT_CFLAGS)

check_PROGRAMS = aaindex aapoly aargb accanti accpersp alpha alpha3D anti \
	bezcurve bezmesh checker clip colormat cube depthcue dof \
	double drawf feedback fog fogindex font hello image light \
	lines list material mipmap model movelight nurbs pickdepth \
	picksquare plane planet polyoff polys quadric robot sccolorlight \
	scene scenebamb sceneflat select smooth stencil stroke surface \
	teaambient teapots tess tesswind texbind texgen texprox texsub \
	texturesurf torus trim unproject varray wrap 

EXTRA_DIST = jitter.h

LDADD = $(GLUT_LIBS) $(top_builddir)/$(GLU_DIR)/libGLU.la \
	$(top_builddir)/src/libGL.la -lm
#LDFLAGS = -no-install

# execute all programs
exec: $(check_PROGRAMS)
	@for prog in $(check_PROGRAMS);	\
	do				\
	 echo -n "Running $$prog ..." ;	\
	 ./$$prog ;			\
	 echo ;				\
	done


include $(top_srcdir)/common_rules.make
