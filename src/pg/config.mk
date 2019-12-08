# build configuration

TOP=../..
include $(TOP)/Setup.mak

EFEU_INCLUDE = $(INC)/DB
include $(CFG)/EFEU.mak

files:: .build/libpq.mk

.build/libpq.mk::
	mkdir -p $(@D)
	$(BIN)/efeu-config update libpq.mkcfg $@

include .build/libpq.mk
