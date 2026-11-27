#*************************************************************************************
# Location of Stuff

# Location of everything
rootdir         = ../..

# Location of generated source files
fontsrc         = $(rootdir)/source/base/font
scenesrc        = $(rootdir)/source/backend/control
bluenoisesrc    = $(rootdir)/source/base/data
builtinsrc      = $(rootdir)/source/base

# Location of input files
include         = $(rootdir)/distribution/include
scenes          = $(rootdir)/distribution/scenes

# Location of tools
metamake        = $(rootdir)/tools/meta-make
reswrap_header  = $(metamake)/reswrap-header.cpp
reswrap_sh      = $(metamake)/reswrap.sh
metagen_header  = $(metamake)/metagen-header.cpp
metagen_sh      = $(metamake)/metagen.sh

#*************************************************************************************
# What to Generate

FONTS           = crystal cyrvetic povlogo timrom
BENCHMARK       = benchmark_pov benchmark_ini
BLUENOISE       = bluenoise64a
BUILTIN         = builtinfiles_data

#-------------------------------------------------------------------------------------
# Generation Parameters

# File              Identifier     Seed Size [Slices]
bluenoise64a_opts = kBlueNoise64a  4711 64

#-------------------------------------------------------------------------------------
# Files to Generate

FONTFILES      = $(foreach font,$(FONTS),$(addprefix $(fontsrc)/$(font),.cpp .h))
BENCHMARKFILES = $(foreach scene,$(BENCHMARK),$(addprefix $(scenesrc)/$(scene),.cpp .h))
BLUENOISEFILES = $(foreach noise,$(BLUENOISE),$(addprefix $(bluenoisesrc)/$(noise),.cpp .h))
BUILTINFILES   = $(foreach table,$(BUILTIN),$(addprefix $(builtinsrc)/$(table),.cpp))

ALLFILES       = $(FONTFILES) $(BENCHMARKFILES) $(BLUENOISEFILES) $(BUILTINFILES)

#*************************************************************************************
# Rules

.PHONY: all
all: $(ALLFILES)

.PHONY: clean
clean: 
	-rm $(ALLFILES)

#-------------------------------------------------------------------------------------
# Fonts

vpath %.ttf $(include)

$(fontsrc)/%.cpp: %.ttf $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov_base" "font_$(*F)" "-z"

$(fontsrc)/%.h: %.ttf $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov_base" "font_$(*F)" "-z"

#-------------------------------------------------------------------------------------
# Scenes

vpath %.ini $(scenes)
vpath %.pov $(scenes)

$(scenesrc)/benchmark_ini.%: advanced/benchmark/benchmark.ini $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov" "Benchmark_Options" "-m -ta -c 120"

$(scenesrc)/benchmark_pov.%: advanced/benchmark/benchmark.pov $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov" "Benchmark_File" "-m -ta -c 120"

#-------------------------------------------------------------------------------------
# Blue Noise Patterns

$(BLUENOISEFILES): GENOPTS = $($(basename $(@F))_opts)
$(BLUENOISEFILES): NAME    = $(word 1, $(GENOPTS))
$(BLUENOISEFILES): SEED    = $(word 2, $(GENOPTS))
$(BLUENOISEFILES): SIZE    = $(word 3, $(GENOPTS))
$(BLUENOISEFILES): SLICES  = $(word 4, $(GENOPTS))

define metagen_bluenoise =
$(metagen_sh) "$@" "Blue noise pattern data" "python $(metamake)/bluenoise/metagen-bluenoise.py"
endef

$(BLUENOISEFILES): %: $(metagen_header)
	$(metagen_bluenoise) "-l -npov_base -r$(SEED) $(NAME) $(SIZE) $(SIZE) $(SLICES)" "-h"

#-------------------------------------------------------------------------------------
# Built-in Standard Include Files
# (declared in the hand-written `source/base/builtinfiles.h`, so there is no
# generated header; the list of files to leave out is in the generator itself)

builtin_py      = $(metamake)/builtinfiles/metagen-builtinfiles.py

$(BUILTINFILES): $(metagen_header) $(builtin_py) $(wildcard $(include)/*)
	$(metagen_sh) "$@" "Built-in copies of the standard include files, deflated." "python3 $(builtin_py)" "-npov_base $(include)"

#*************************************************************************************
# End of File
