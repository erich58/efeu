# :*: TK interface
# :de: TK Schnittstelle

TOP=../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

prereq::
	make -C ../base

Modules= config lib pgm examples manual

all doc clean::
	@for m in $(Modules); \
	do \
		printf '********************************************\n'; \
		printf '* %-40s *\n' $$m; \
		printf '********************************************\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf '********************************************\n'

clean::
	rm -rf $(BUILD)
