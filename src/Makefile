TOP=..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

Modules= config base tk misc pg scope manuals # pdf xdoc

all:: prereq
doc:: all

all doc clean::
	@for m in $(Modules); \
	do \
		printf '############################################\n'; \
		printf '# %-40s #\n' $$m; \
		printf '############################################\n'; \
		$(MAKE) -C $$m $@ || exit 1; \
	done
	@printf '############################################\n'
