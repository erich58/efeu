# Determine Top-Directory

ifndef TOP
TOP=$(shell pwd)
endif

_TOP=$(shell cd $(TOP) && pwd)

# Determine Version

ifeq ($(wildcard $(TOP)/CHANGELOG),)
$(error "$$(TOP)/CHANGELOG not found")
endif

VERSION=$(shell sed -e 1q $(TOP)/CHANGELOG)

# Determine Modul-Name

ifndef NAME
NAME=$(notdir $(shell pwd))
endif

# Configuration directory 

CFG=$(_TOP)/config

# Build directory

ifndef BUILD
BUILD=.build
endif

# Target directories

SRC=$(_TOP)/src
BIN=$(_TOP)/bin
LIB=$(_TOP)/lib
INC=$(_TOP)/include
MAN=$(_TOP)/man
DOC=$(_TOP)/doc
SHR=$(_TOP)/share
ETC=$(_TOP)/etc

# Default targets

.PHONY: autonom prereq all files doc clean

autonom:: prereq

prereq::

autonom:: all

all:: files

test::

files::

doc::

clean::
