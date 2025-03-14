CFLAGS = -O -t apple2
H := \#

.PHONY: all binaries images

default: AppleTest.dsk

BASIC_FILES = src/MOUSE.MOVE.bas src/SPLIT.bas

AppleTest.dsk: data/ProDOS_1.9.dsk binaries images $(BASIC_FILES)
	./scripts/build_dsk.sh $< $@ out/* $(BASIC_FILES)

binaries: out/MODETEST$(H)066000 \
	out/BANKTEST$(H)066000

images: out/LGR.BIN$(H)060400 \
	out/DLGR.AUX$(H)060400 \
	out/DLGR.BIN$(H)060400 \
	out/HGR.BIN$(H)062000 \
	out/DHGR.BIN$(H)062000 \
	out/DHGR.AUX$(H)062000

out/MODETEST$(H)066000: src/modetest.o src/memutil.o
	@mkdir -p out
	cl65 $(CFLAGS) -Wl -D__EXEHDR__=0 --start-addr 0x6000 -o $@ $^

out/BANKTEST$(H)066000: src/banktest.o src/memutil.o src/memstore.o
	@mkdir -p out
	cl65 $(CFLAGS) -Wl -D__EXEHDR__=0 --start-addr 0x6000 -o $@ $^

%.o: %.c
	cl65 $(CFLAGS) -c -o $@ $<

%.o: %.asm
	cl65 -c -o $@ $<

# lores image
out/LGR.BIN$(H)060400: data/apple160.bmp
	@mkdir -p out
	b2d $< bin basename D9 L
	mv data/APPLE.SL2 $@

# double-lores image
out/DLGR.AUX$(H)060400 out/DLGR.BIN$(H)060400: data/apple160.bmp
	@mkdir -p out
	b2d $< bin basename D9 DL
	mv data/APPLE.DL1 out/DLGR.AUX$(H)060400
	mv data/APPLE.DL2 out/DLGR.BIN$(H)060400

# hires image
out/HGR.BIN$(H)062000: data/parrot280.bmp
	@mkdir -p out
	b2d $< bin basename hgr D9
	mv data/PARROTC.BIN $@

# double-hires image
out/DHGR.BIN$(H)062000 out/DHGR.AUX$(H)062000: data/parrot280.bmp
	@mkdir -p out
	b2d $< bin basename D9
	mv data/PARROT.BIN out/DHGR.BIN$(H)062000
	mv data/PARROT.AUX out/DHGR.AUX$(H)062000

clean:
	rm -rf *.dsk out src/*.o data/*.SL2 data/*.DL1 data/*.DL2 data/*.BIN data/*.AUX
