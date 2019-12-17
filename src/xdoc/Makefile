# :*: documentation depending on installed modules
# :de: Von installierten Modulen abhängige Dokumentationen

TOP=../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules= edb man mkmf

all doc clean::
	@for m in $(Modules); \
	do \
		printf '********************************************\n'; \
		printf '* %-40s *\n' $$m; \
		printf '********************************************\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf '********************************************\n'
