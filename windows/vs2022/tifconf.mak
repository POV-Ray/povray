PATH=..\..\libraries\tiff\libtiff

all: $(PATH)\tif_config.h $(PATH)\tiffconf.h

$(PATH)\tif_config.h: $(PATH)\tif_config.h.vc
	copy $(PATH)\tif_config.h.vc $(PATH)\tif_config.h

$(PATH)\tiffconf.h: $(PATH)\tiffconf.h.vc
	copy $(PATH)\tiffconf.h.vc $(PATH)\tiffconf.h

