TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules= esh types edb efeudoc vimrc

all test doc clean::
	@for m in $(Modules); \
	do \
		printf '+------------------------------------------+\n'; \
		printf '| %-40s |\n' $$m; \
		printf '+------------------------------------------+\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf -- '--------------------------------------------\n';
