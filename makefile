SHELL=/bin/sh
CC=gcc
CFLAGS=-std=c99.
DEPS=main.h
PREFIX=/usr/local

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

BasilC: main.c
	$(CC) -o basilc main.c -std=c99

.PHONY: install
install: basilc
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/basilc
	@echo Finished Installing!
	@echo
	@echo If you wish to run basilc programs with the command \"basilc\", set the environment variable \"basilc\" to \"/usr/local/bin/basilc\"!
	@echo

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/basilc
	@echo Finished Uninstalling!
	@echo
	@echo If you have set the \"basilc\" environment variable during installation, you may wish to remove that!
	@echo
