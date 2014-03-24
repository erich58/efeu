# Nicht editieren, Datei wurde mit
# /efeu/bin/shmkmf 
# generiert.

CONFIG= ./Config.make
MAKEFILE= Makefile

# CC configuration
CC= efeucc
CONFIG_CFLAGS= -g 
CONFIG_LDFLAGS= 
# End of CC configuration

all:: t_vmatch

clean::
	rm -f t_vmatch

t_vmatch:  ./t_vmatch.c
	$(CC) $(CONFIG_CFLAGS) $(CFLAGS) -o $@  ./t_vmatch.c $(CONFIG_LDFLAGS) -lefm

depend::  ./t_vmatch.c
	$(CC) $(CONFIG_CFLAGS) $(CFLAGS) ./t_vmatch.c -MM -MT t_vmatch >> Makefile

# Von shmkmf ergänzte Regeln:

all::

Makefile: ./Config.make \
  /efeu/bin/shmkmf \
  /efeu/lib/shmkmf/base.smh \
  /efeu/lib/shmkmf/cc.smh \
  /efeu/lib/shmkmf/comment.smh \
  /efeu/lib/shmkmf/dir.smh \
  /efeu/lib/shmkmf/file.smh \
  /efeu/lib/shmkmf/foreach.smh \
  /efeu/lib/shmkmf/rule.smh \
  /efeu/lib/shmkmf/srclist.smh \
  /efeu/lib/shmkmf/update.smh \
  /efeu/lib/shmkmf/var.smh \
  SourceList
	/efeu/bin/shmkmf 

update::
	/efeu/bin/shmkmf 

install:: all

depend::

purge:: 
	rm -f core

clean:: purge

uninstall::

SourceList:: 
	@find . -type f -name "[!.]*[!~]" -print | sort > $@~
	@if cmp -s $@~ $@; then rm -f $@~; else mv -f $@~ $@; fi

# Von make depend ergänzte Regeln:
t_vmatch: t_vmatch.c /home/erich/efeu/include/EFEU/Resource.h \
 /home/erich/efeu/include/EFEU/config.h \
 /home/erich/efeu/include/EFEU/vecbuf.h
