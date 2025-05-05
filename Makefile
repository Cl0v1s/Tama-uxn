
DATA := ./data

DATA_FILES := $(wildcard $(DATA)/*/*) $(wildcard $(DATA)/*)

FILES := $(DATA_FILES:.chr=.chr.h) 

.PHONY: tama.tal tmp.c clean run nasu

ALL: tools/chibicc tools/uxnasm tools/uxnemu tama.rom 

define submodules
	git submodule init
	git submodule update
endef

.git/modules/chibicc:
	@$(call submodules)
.git/modules/uxn: 
	@$(call submodules)

uxn/bin: .git/modules/uxn
	cd uxn && ./build.sh --no-run

tools/uxnasm: uxn/bin
	cp uxn/bin/uxnasm tools/
tools/uxnemu: uxn/bin
	cp uxn/bin/uxnemu tools/

tools/chibicc: .git/modules/chibicc
	cd chibicc && make
	mv chibicc/chibicc tools/chibicc

clean:
	rm tools/chibicc
	rm -rf ./data/*.h
	rm -f ./*.c
	rm -f tama.rom
	rm -f tama.rom.sym
	rm -f tama.tal

%.chr.h: %.chr
	xxd -i $< $@

tmp.c: $(FILES) 
	gcc -I. -P -E src/main.c -o $@

tama.tal: tmp.c
	./tools/chibicc $< > $@

tama.rom: tama.tal
	./tools/uxnasm $< $@

run:
	./tools/uxnemu tama.rom

nasu:
	./tools/uxnemu ./tools/nasu.rom
