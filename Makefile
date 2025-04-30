POSTFIX ?= usr/local
DESTDIR ?= /

RM ?= rm -rf

.PHONY: all
all: tiny rsad rsae rsak

rsad: rsad.c rsa.c
rsae: rsae.c rsa.c
rsak: rsak.c rsa.c

.PHONY: clean
clean:
	$(RM) tiny
	$(RM) rsae
	$(RM) rsad
	$(RM) rsak

.PHONY: install
install: tiny tiny.1 rsae rsad rsak
	install -d $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 tiny $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 rsae $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 rsad $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 rsak $(DESTDIR)$(POSTFIX)/bin/
	install -d $(DESTDIR)$(POSTFIX)/share/man/man1/
	install -m 644 tiny.1 $(DESTDIR)$(POSTFIX)/share/man/man1/

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(POSTFIX)/bin/tiny

README: tiny.1
	mandoc -man -T ascii tiny.1 | col -b > README
