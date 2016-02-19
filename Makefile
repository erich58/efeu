#	Makefile for creating and installing efeu

STARTER=~/bin/efeu

usage::
	@echo "usage: make base | all | doc | purge | clean | stat"
	@echo
	@echo "config	create efeu configuration tools"
	@echo "basic	create basic functionality"
	@echo "all	create all tools"
	@echo "doc	create all with documentation"
	@echo
	@echo "install	Install efeu (what was build)"
	@echo "purge	remove build directory"
	@echo "clean	remove all target directories"
	@echo
	@echo "starter	Create starter script $(STARTER)"
	@echo "arch	create archive for binaries"
	@echo
	@echo "NOTE: Instead of installing efeu, you can add"
	@echo "`pwd`/bin to PATH or use the starter script."

#	The following macro sets the environment needed for some
#	commands to correctly implement efeu.

SETENV=	EFEUTOP=`pwd`; PATH=$$EFEUTOP/bin:$$PATH; export EFEUTOP PATH

config:: build/Makefile

build/Makefile:
	sh sanity-check.sh
	-if [ -f src/Makefile ]; then (cd src; make); fi
	if [ -x src/config/setup ]; then (cd src/config; sh ./setup ../..); fi
	($(SETENV); cd build; shmkmf)

basic:: config
	($(SETENV); cd build; $(MAKE) base.all tk.all)

all:: config
	($(SETENV); cd build; $(MAKE) all)

doc:: config
	($(SETENV); cd build; $(MAKE) all doc)

basic all doc:: note

note::
	@(test -d note && find note -type f -print | xargs cat) || true

purge::
	rm -rf build

clean::
	rm -rf include ppinclude build lib bin etc cgi-bin share doc man note

starter::
	test -d ~/bin || mkdir ~/bin
	sed -e "/^%/d" -e "s|%T|`pwd`|" efeu.tpl > $(STARTER)
	chmod u+x $(STARTER)

install::
	sh efeu-install.sh

arch::
	sh efeu-install.sh --arch
