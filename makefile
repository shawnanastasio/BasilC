SHELL=/bin/sh
CC=gcc
CFLAGS=-std=c99
PREFIX=/usr/local
SRCDIR=src
INCLUDEDIR=include
OUTDIR=out
DEPS=$(SRCDIR)/stringhelpers.o

.PHONY: all
all: pre-build BasilC

pre-build:
	if [ ! -d out ]; then mkdir out; fi

BasilC: $(DEPS)
	$(CC) -o $(OUTDIR)/basilc $(DEPS) $(SRCDIR)/main.c $(CFLAGS) -I$(INCLUDEDIR)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) -I$(INCLUDEDIR)

.PHONY: clean
clean:
	rm -rf out/ $(DEPS)

.PHONY: install
install: BasilC
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/basilc
	@echo Finished Installing!

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/basilc
	@echo Finished Uninstalling!
