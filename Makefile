POSTFIX ?= usr/local
DESTDIR ?= /

RM ?= rm -rf

all: tiny

install: tiny
	install -d $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 tiny $(DESTDIR)$(POSTFIX)/bin/
	install -d $(DESTDIR)$(POSTFIX)/share/man/man1/
	install -m 644 tiny.1 $(DESTDIR)$(POSTFIX)/share/man/man1/

uninstall:
	RM $(DESTDIR)$(POSTFIX)/bin/tiny
