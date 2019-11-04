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

BUILD=.build

# Target directories

BIN=$(_TOP)/bin
LIB=$(_TOP)/lib
INC=$(_TOP)/include
MAN=$(_TOP)/man
SHR=$(_TOP)/share

# Default targets

.PHONY: autonom prereq all files clean

autonom:: prereq

prereq::

autonom:: all

all:: files

files::

clean::
