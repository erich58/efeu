# :*: efeu basic documentation
# :de: EFEU Basishandbücher

TOP=../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules= base efeudoc interp man1 mdmat mdspec prog style user

all test doc clean::
	@for m in $(Modules); \
	do \
		printf '********************************************\n'; \
		printf '* %-40s *\n' $$m; \
		printf '********************************************\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf '********************************************\n'
