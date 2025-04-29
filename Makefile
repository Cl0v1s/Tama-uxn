
DATA := ./data

DATA_FILES := $(wildcard $(DATA)/*)

FILES := $(DATA_FILES:.chr=.chr.h) 

.PHONY: tama.tal tmp.c

ALL: tama.rom

clean:
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
	./tools/chibicc -O1 $< > $@

tama.rom: tama.tal
	./tools/uxnasm $< $@

run:
	./tools/uxnemu tama.rom

nasu:
	./tools/uxnemu ./tools/nasu.rom
