
SOURCES := src/*
DATA := data/*

ALL: tama.rom

tmp.c: $(SOURCES)
	gcc -I. -P -E $? -o $@

tama.tal: tmp.c
	./tools/chibicc $< > $@

tama.rom: tama.tal
	./tools/uxnasm $< $@

run:
	./tools/uxnemu tama.rom

nasu:
	./tools/uxnemu ./tools/nasu.rom
