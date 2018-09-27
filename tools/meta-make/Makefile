include         = ../../distribution/include
scenes          = ../../distribution/scenes
backend         = ../../source/backend
base            = ../../source/base
reswrap_header  = reswrap-header.cpp
reswrap_sh      = ./reswrap.sh

FONTS           = crystal cyrvetic povlogo timrom
BENCHMARK       = benchmark_pov benchmark_ini

all: $(FONTS) $(BENCHMARK)

.PHONY: clean
clean: 
	-for name in $(FONTS); do \
		rm "$(base)/font/$${name}.cpp" "$(base)/font/$${name}.h"; \
	done
	-for name in $(BENCHMARK); do \
		rm "$(backend)/control/$${name}.cpp" "$(backend)/control/$${name}.h"; \
	done

$(FONTS): %: $(base)/font/%.cpp $(base)/font/%.h

$(base)/font/%.cpp: $(include)/%.ttf $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov_base" "font_$(*F)" "-z"

$(base)/font/%.h: $(include)/%.ttf $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov_base" "font_$(*F)" "-z"

$(BENCHMARK): benchmark_%: ../../source/backend/control/benchmark_%.cpp ../../source/backend/control/benchmark_%.h

$(backend)/control/benchmark_ini.%: $(scenes)/advanced/benchmark/benchmark.ini $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov" "Benchmark_Options" "-m -ta -c 120"

$(backend)/control/benchmark_pov.%: $(scenes)/advanced/benchmark/benchmark.pov $(reswrap_header)
	$(reswrap_sh) "$@" "$<" "pov" "Benchmark_File" "-m -ta -c 120"
