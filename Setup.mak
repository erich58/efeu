# Determine Top-Directory

ifndef TOP
TOP=$(shell pwd)
endif

# Determine Modul-Name

ifndef NAME
NAME=$(notdir $(shell pwd))
endif

# Configuration directory 

CFG=$(abspath $(TOP)/config)

# Build directory

BUILD=.build

# Target directories

BIN=$(abspath $(TOP)/bin)
LIB=$(abspath $(TOP)/lib)
INC=$(abspath $(TOP)/include)
MAN=$(abspath $(TOP)/man)
SHR=$(abspath $(TOP)/share)
