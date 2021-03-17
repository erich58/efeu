# :*: simple tools
# :de: Einfache Hilfsprogramme

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules=  conv dump tape xml

all doc clean::
	@for m in $(Modules); \
	do \
		printf '+------------------------------------------+\n'; \
		printf '| %-40s |\n' $$m; \
		printf '+------------------------------------------+\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf -- '--------------------------------------------\n';
