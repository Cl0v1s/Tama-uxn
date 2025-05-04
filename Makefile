
DATA := ./data

DATA_FILES := $(wildcard $(DATA)/*/*) $(wildcard $(DATA)/*)

FILES := $(DATA_FILES:.chr=.chr.h) 

.PHONY: tama.tal tmp.c

ALL: tools/chibicc tools/uxnasm tools/uxncli tools/uxnemu tama.rom

.git/modules:
	git submodule init
	git submodule update

uxn: .git/modules
	cd uxn && ./build.sh --no-run
	cp uxn/bin/* tools/

tools/uxnasm: uxn
tools/uxncli: uxn
tools/uxnemu: uxn

tools/chibicc: .git/modules
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
