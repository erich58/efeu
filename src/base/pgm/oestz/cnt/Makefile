#	Nicht editieren: Makefile wurde aus Imakefile generiert
#	Konfiguriert mit "alpha.cf"

STDCFLAGS=	-g
WARN_FLAGS=	-Wall -Wmissing-prototypes

GCC_FLAGS=	-fPIC
CC=		gcc $(WARN_FLAGS) $(GCC_FLAGS)

CPP=		$(CC) -E
CPP2DEP=	cpp2dep -l

TROFF=	groff -Tlatin1
LINT=	lint -u
MV=	mv -f
RM=	rm -f
CP=	cp
LN=	ln -s

EFEULIB=	/local/lib/efeu
PPFLAGS=	-DALPHA
STDLIBDIR=	/local/lib
STDINCDIR=	/local/include
STDBINDIR=	/local/bin
MANDIR=		/local/man
CATMANDIR=	/local/catman
STDDEFINES=
SYSCFLAGS=	-I$(STDINCDIR)
SYSLFLAGS=	-L$(STDLIBDIR) -Wl,-rpath,$(STDLIBDIR)
SYSLIBS=

ARCHLIST=	arch.list
ARCHFILE=	arch.cpio
MAKEARCHLIST=	mkflist -f $(ARCHLIST)

HDR=	/Oestz
BINDIR=	/usr/local/bin

DEP=	$(STDLIBDIR)/libmd.so $(STDLIBDIR)/libefmath.so $(STDLIBDIR)/libefx.so $(STDLIBDIR)/libefm.so
LIB=	-lmd -lefmath -lefx -lm -lefm
CFLAGS=	-I $(HDR)

all:: files

OBJ1=	vz.o db.o dbdef.o konv.o

all :: vz.o

clean ::
	$(RM) vz.o

vz.o : vz.c
	$(CC) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c vz.c

depend::
	$(CPP) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c vz.c | $(CPP2DEP) vz.o >> Makefile

archlist:: vz.c
	$(MAKEARCHLIST) vz.c

all :: db.o

clean ::
	$(RM) db.o

db.o : db.c
	$(CC) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c db.c

depend::
	$(CPP) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c db.c | $(CPP2DEP) db.o >> Makefile

archlist:: db.c
	$(MAKEARCHLIST) db.c

all :: dbdef.o

clean ::
	$(RM) dbdef.o

dbdef.o : dbdef.c
	$(CC) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c dbdef.c

depend::
	$(CPP) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c dbdef.c | $(CPP2DEP) dbdef.o >> Makefile

archlist:: dbdef.c
	$(MAKEARCHLIST) dbdef.c

all :: konv.o

clean ::
	$(RM) konv.o

konv.o : konv.c
	$(CC) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c konv.c

depend::
	$(CPP) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c konv.c | $(CPP2DEP) konv.o >> Makefile

archlist:: konv.c
	$(MAKEARCHLIST) konv.c

OBJ3=	wbneu.o

all :: wbneu.o

clean ::
	$(RM) wbneu.o

wbneu.o : wbneu.c
	$(CC) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c wbneu.c

depend::
	$(CPP) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -c wbneu.c | $(CPP2DEP) wbneu.o >> Makefile

archlist:: wbneu.c
	$(MAKEARCHLIST) wbneu.c

all :: wbneu.c

clean ::
	$(RM) wbneu.c

wbneu.c : wbneu.tpl
	mksource -c wbneu.tpl

archlist:: wbneu.tpl
	$(MAKEARCHLIST) wbneu.tpl

depend:: wbneu.tpl
	mksource -d -c wbneu.tpl >> Makefile

OBJ=	$(OBJ1) $(OBJ2) $(OBJ3)

all :: $(STDBINDIR)/oestzcnt

clean ::
	$(RM) $(STDBINDIR)/oestzcnt

$(STDBINDIR)/oestzcnt : oestzcnt.c $(OBJ) $(DEP)
	$(CC) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) -o $@ oestzcnt.c $(SYSLFLAGS) $(OBJ) $(LIB) $(SYSLIBS)

depend::
	$(CPP) $(STDDEFINES) $(CFLAGS) $(STDCFLAGS) $(SYSCFLAGS) oestzcnt.c | $(CPP2DEP) $(STDBINDIR)/oestzcnt >> Makefile

archlist:: oestzcnt.c
	$(MAKEARCHLIST) oestzcnt.c

files :: $(EFEULIB)/app-defaults/oestzcnt.app

clean ::
	$(RM) $(EFEULIB)/app-defaults/oestzcnt.app

archlist:: oestzcnt.app
	$(MAKEARCHLIST) oestzcnt.app

$(EFEULIB)/app-defaults/oestzcnt.app : oestzcnt.app
	$(RM) $@
	$(CP) oestzcnt.app $@

Makefile : Imakefile /local/lib/efeu/CFG_ALPHA
	/lbin/mkmf
	$(MAKE) -f Makefile depend

/local/lib/efeu/CFG_ALPHA :
	@echo Makefile wegen Plattformwechsel erneuern
	/lbin/mkmf

update::
	/lbin/mkmf
	$(MAKE) -f Makefile depend

depend::
	@$(RM) Makefile.bak

archlist::
	$(MAKEARCHLIST) Imakefile

arch::
	$(MAKEARCHLIST) -c Imakefile
	$(MAKE) -f Makefile archlist
	cpio -oac -O $(ARCHFILE) < $(ARCHLIST)
	$(RM) $(ARCHLIST)

purge::
	$(RM) core $(ARCHFILE) $(ARCHLIST) Makefile.bak

clean::	purge

# DO NOT DELETE

Makefile: /home/efeu/config/alpha.cf /home/efeu/config/base.tpl \
 /home/efeu/config/rules.tpl /home/efeu/config/stdmake.tpl \
 /home/efeu/config/config.tpl Imakefile \
 /home/efeu/config/mksource.tpl

vz.o: vz.c /local/include/EFEU/pconfig.h /local/include/EFEU/Info.h \
 /local/include/EFEU/vecbuf.h /local/include/EFEU/types.h \
 /local/include/EFEU/config.h /local/include/EFEU/io.h \
 /local/include/EFEU/memalloc.h /local/include/EFEU/refdata.h \
 /local/include/EFEU/strbuf.h /local/include/EFEU/efmain.h \
 /local/include/EFEU/msgtab.h /local/include/EFEU/mstring.h \
 /local/include/EFEU/procenv.h /local/include/EFEU/efio.h \
 /local/include/EFEU/ioctrl.h /local/include/EFEU/ioscan.h \
 /local/include/EFEU/parsub.h /local/include/EFEU/iocpy.h \
 /local/include/EFEU/object.h /local/include/EFEU/data.h \
 /local/include/EFEU/ftools.h agf.h /local/include/EFEU/mdmat.h \
 /local/include/EFEU/konvobj.h

db.o: db.c /local/include/EFEU/ftools.h /local/include/EFEU/mstring.h \
 /local/include/EFEU/memalloc.h /local/include/EFEU/config.h \
 /local/include/EFEU/types.h /local/include/EFEU/efmain.h \
 /local/include/EFEU/msgtab.h /local/include/EFEU/strbuf.h \
 /local/include/EFEU/procenv.h agf.h /local/include/EFEU/mdmat.h \
 /local/include/EFEU/object.h /local/include/EFEU/efio.h \
 /local/include/EFEU/refdata.h /local/include/EFEU/io.h \
 /local/include/EFEU/ioctrl.h /local/include/EFEU/ioscan.h \
 /local/include/EFEU/parsub.h /local/include/EFEU/iocpy.h \
 /local/include/EFEU/vecbuf.h /local/include/EFEU/data.h \
 /local/include/EFEU/konvobj.h /local/include/EFEU/pconfig.h \
 /local/include/EFEU/Info.h

dbdef.o: dbdef.c /local/include/EFEU/efmain.h \
 /local/include/EFEU/types.h /local/include/EFEU/config.h \
 /local/include/EFEU/msgtab.h /local/include/EFEU/mstring.h \
 /local/include/EFEU/memalloc.h /local/include/EFEU/strbuf.h \
 /local/include/EFEU/procenv.h agf.h /local/include/EFEU/mdmat.h \
 /local/include/EFEU/object.h /local/include/EFEU/efio.h \
 /local/include/EFEU/refdata.h /local/include/EFEU/io.h \
 /local/include/EFEU/ioctrl.h /local/include/EFEU/ioscan.h \
 /local/include/EFEU/parsub.h /local/include/EFEU/iocpy.h \
 /local/include/EFEU/vecbuf.h /local/include/EFEU/data.h \
 /local/include/EFEU/ftools.h /local/include/EFEU/konvobj.h \
 /local/include/EFEU/pconfig.h /local/include/EFEU/Info.h

konv.o: konv.c konv.h /local/include/EFEU/efmain.h \
 /local/include/EFEU/types.h /local/include/EFEU/config.h \
 /local/include/EFEU/msgtab.h /local/include/EFEU/mstring.h \
 /local/include/EFEU/memalloc.h /local/include/EFEU/strbuf.h \
 /local/include/EFEU/procenv.h /local/include/EFEU/mdmat.h \
 /local/include/EFEU/object.h /local/include/EFEU/efio.h \
 /local/include/EFEU/refdata.h /local/include/EFEU/io.h \
 /local/include/EFEU/ioctrl.h /local/include/EFEU/ioscan.h \
 /local/include/EFEU/parsub.h /local/include/EFEU/iocpy.h \
 /local/include/EFEU/vecbuf.h /local/include/EFEU/data.h \
 /local/include/EFEU/ftools.h /local/include/EFEU/konvobj.h \
 /local/include/EFEU/pconfig.h /local/include/EFEU/Info.h

wbneu.o: wbneu.c agf.h /local/include/EFEU/efmain.h \
 /local/include/EFEU/types.h /local/include/EFEU/config.h \
 /local/include/EFEU/msgtab.h /local/include/EFEU/mstring.h \
 /local/include/EFEU/memalloc.h /local/include/EFEU/strbuf.h \
 /local/include/EFEU/procenv.h /local/include/EFEU/mdmat.h \
 /local/include/EFEU/object.h /local/include/EFEU/efio.h \
 /local/include/EFEU/refdata.h /local/include/EFEU/io.h \
 /local/include/EFEU/ioctrl.h /local/include/EFEU/ioscan.h \
 /local/include/EFEU/parsub.h /local/include/EFEU/iocpy.h \
 /local/include/EFEU/vecbuf.h /local/include/EFEU/data.h \
 /local/include/EFEU/ftools.h /local/include/EFEU/konvobj.h \
 /local/include/EFEU/pconfig.h /local/include/EFEU/Info.h

wbneu.c: ./wbneu.tpl /home/efeu/mksource/classdef.ms \
 /home/efeu/mksource/function.ms /home/efeu/mksource/macro.ms \
 /local/lib/efeu//app-defaults/efmain.app \
 /local/lib/efeu//app-defaults/mksource.app \
 /local/lib/efeu//messages/efmain.msg

/local/bin/oestzcnt: oestzcnt.c /local/include/EFEU/pconfig.h \
 /local/include/EFEU/Info.h /local/include/EFEU/vecbuf.h \
 /local/include/EFEU/types.h /local/include/EFEU/config.h \
 /local/include/EFEU/io.h /local/include/EFEU/memalloc.h \
 /local/include/EFEU/refdata.h /local/include/EFEU/strbuf.h \
 /local/include/EFEU/efmain.h /local/include/EFEU/msgtab.h \
 /local/include/EFEU/mstring.h /local/include/EFEU/procenv.h \
 /local/include/EFEU/efio.h /local/include/EFEU/ioctrl.h \
 /local/include/EFEU/ioscan.h /local/include/EFEU/parsub.h \
 /local/include/EFEU/iocpy.h /local/include/EFEU/object.h \
 /local/include/EFEU/data.h /local/include/EFEU/ftools.h \
 /local/include/EFEU/mdmat.h /local/include/EFEU/konvobj.h agf.h
