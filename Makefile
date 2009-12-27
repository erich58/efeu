#	Makefile for installing efeu

usage::
	@echo "usage: make config | all | clean"
	@echo
	@echo "config	create Makefile in build"
	@echo "all	build all (running make all in build)"
	@echo "clean	remove all target directories"

#	The following macro sets the environment needed for some
#	commands to correctly implement efeu.

SETENV=	EFEUTOP=`pwd`; PATH=$$EFEUTOP/bin:$$PATH; export EFEUTOP PATH

config::
	if [ -f src/Makefile ]; then (cd src; make); fi
	if [ -x src/config/setup ]; then (cd src/config; ./setup ../..); fi
	($(SETENV); efeubuild)

all:: config
	($(SETENV); cd build; make all)

clean::
	rm -rf include ppinclude build lib bin doc man
