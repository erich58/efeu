#	Makefile for creating and installing efeu

STARTER=~/bin/efeu

usage::
	@echo "usage: make config | all | purge | clean | stat"
	@echo
	@echo "config	create Makefile in build"
	@echo "build	build all (running make all in build)"
	@echo "doc	build doc (running make doc in build)"
	@echo "note	display notes of building process (if any)"
	@echo "all	runs make config build note"
	@echo "purge	remove build directory"
	@echo "clean	remove all target directories"
	@echo
	@echo "starter	Create starter script $(STARTER)"
	@echo "install	Install efeu binaries"
	@echo "arch	create archive for binaries"
	@echo
	@echo "NOTE: Instead of installing efeu, you can add"
	@echo "`pwd`/bin to PATH or use the starter script."

#	The following macro sets the environment needed for some
#	commands to correctly implement efeu.

SETENV=	EFEUTOP=`pwd`; PATH=$$EFEUTOP/bin:$$PATH; export EFEUTOP PATH

config::
	sh sanity-check.sh
	-if [ -f src/Makefile ]; then (cd src; make); fi
	if [ -x src/config/setup ]; then (cd src/config; ./setup ../..); fi
	($(SETENV); cd build; shmkmf)

build::
	($(SETENV); cd build; make all)

doc::
	($(SETENV); cd build; make doc)

note::
	@(test -d note && find note -type f -print | xargs cat) || true

all:: config build note

purge::
	rm -rf build

clean::
	rm -rf include ppinclude build lib bin cgi-bin share doc man note

starter::
	test -d ~/bin || mkdir ~/bin
	sed -e "/^%/d" -e "s|%T|`pwd`|" efeu.tpl > $(STARTER)
	chmod u+x $(STARTER)

install::
	sh efeu-install.sh

arch::
	sh efeu-install.sh --arch
