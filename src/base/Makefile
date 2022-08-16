# :*: basic libraries and commands	
# :de: EFEU-Basisbibliotheken/Standardkommandos

TOP=../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules= scripts lib pgm

all test doc clean::
	@for m in $(Modules); \
	do \
		printf '********************************************\n'; \
		printf '* %-40s *\n' $$m; \
		printf '********************************************\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf '********************************************\n'
