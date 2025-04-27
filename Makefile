POSTFIX ?= usr/local
DESTDIR ?= /

RM ?= rm -rf

LDLIBS += -lm

all: tiny

clean:
	$(RM) tiny

install: tiny tiny.1
	install -d $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 tiny $(DESTDIR)$(POSTFIX)/bin/
	install -d $(DESTDIR)$(POSTFIX)/share/man/man1/
	install -m 644 tiny.1 $(DESTDIR)$(POSTFIX)/share/man/man1/

uninstall:
	$(RM) $(DESTDIR)$(POSTFIX)/bin/tiny

README: tiny.1
	mandoc -man -T ascii tiny.1 | col -b > README
