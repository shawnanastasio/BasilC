SHELL=/bin/sh
CC=gcc
CFLAGS=-std=c99
PREFIX=/usr/local
SRCDIR=src
INCLUDEDIR=include
OUTDIR=out
MANDIR=doc
DEPS=$(SRCDIR)/stringhelpers.o $(SRCDIR)/cmd.o

include $(SRCDIR)/libbasilc/make.config

.PHONY: all
all: pre-build BasilC

pre-build:
	if [ ! -d out ]; then mkdir out; fi

BasilC: $(DEPS)
	$(CC) -o $(OUTDIR)/basilc $(DEPS) $(SRCDIR)/main.c $(CFLAGS) -I$(INCLUDEDIR)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) -I$(INCLUDEDIR)

.PHONY: clean
clean:
	rm -rf out/ $(DEPS)

.PHONY: install
install: BasilC
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/man/man1
	cp $(OUTDIR)/basilc $(DESTDIR)$(PREFIX)/bin/basilc
	cp $(MANDIR)/basilc.1 $(DESTDIR)$(PREFIX)/man/man1/basilc.1
	@echo Finished Installing!

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/basilc
	rm -f $(DESTDIR)$(PREFIX)/man/man1/basilc.1
	@echo Finished Uninstalling!
