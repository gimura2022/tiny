POSTFIX ?= usr/local
DESTDIR ?= /

RM ?= rm -rf

.PHONY: all
all: tiny

.PHONY: clean
clean:
	$(RM) tiny

.PHONY: install
install: tiny tiny.1
	install -d $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 tiny $(DESTDIR)$(POSTFIX)/bin/
	install -d $(DESTDIR)$(POSTFIX)/share/man/man1/
	install -m 644 tiny.1 $(DESTDIR)$(POSTFIX)/share/man/man1/

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(POSTFIX)/bin/tiny

README: tiny.1
	mandoc -man -T ascii tiny.1 | col -b > README
