# :*: utilities
# :de: Hilfsprogramme

TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules=  conv dump tape

all doc clean::
	@for m in $(Modules); \
	do \
		printf '+------------------------------------------+\n'; \
		printf '| %-40s |\n' $$m; \
		printf '+------------------------------------------+\n'; \
		make -C $$m $@ || exit 1; \
	done
	@printf -- '--------------------------------------------\n';
