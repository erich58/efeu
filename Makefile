#	Makefile for installing efeu

usage::
	@echo "usage: make config | all | purge | clean | stat"
	@echo
	@echo "config	create Makefile in build"
	@echo "build	build all (running make all in build)"
	@echo "note	display notes of building process (if any)"
	@echo "all	runs make config build note"
	@echo "purge	remove build directory"
	@echo "clean	remove all target directories"

#	The following macro sets the environment needed for some
#	commands to correctly implement efeu.

SETENV=	EFEUTOP=`pwd`; PATH=$$EFEUTOP/bin:$$PATH; export EFEUTOP PATH

config::
	if [ -f src/Makefile ]; then (cd src; make); fi
	if [ -x src/config/setup ]; then (cd src/config; ./setup ../..); fi
	($(SETENV); cd build; shmkmf)

build::
	($(SETENV); cd build; make all)

note::
	@(test -d note && find note -type f -print | xargs cat) || true

all:: config build note

purge::
	rm -rf build

clean::
	rm -rf include ppinclude build lib bin cgi-bin share doc man note
