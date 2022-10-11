#	Makefile for creating and installing efeu

STARTER=~/bin/efeu

usage::
	@echo "usage: make base | all | doc | clean | stat"
	@echo
	@echo "basic	create basic functionality"
	@echo "all	create all tools"
	@echo "test	create and run unit tests"
	@echo "doc	create all with documentation"
	@echo "clean	cleanup the build"
	@echo
	@echo "install	Install efeu (what was build)"
	@echo "starter	Create starter script $(STARTER)"
	@echo "arch	create archive for binaries"
	@echo
	@echo "NOTE:"
	@echo "make install creates the command efeu-uninstall"
	@echo "wich removes al un installed files"
	@echo "Instead of installing efeu, you can add"
	@echo "`pwd`/bin to PATH or use the starter script."
	@echo

config::
	$(MAKE) -C config all

all test doc clean:: config

all test doc clean::
	$(MAKE) -C src $@

basic::
	$(MAKE) -C src/base all

basic all doc:: note

note::
	@(test -d note && find note -type f | xargs head -v) || true

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
