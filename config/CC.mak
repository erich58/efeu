# C-Compiler

ifndef	_CC_mak_
_CC_mak_ = defined

CC = $(shell sh $(CFG)/efeu-config.sh cc)

ifeq ($(strip $(CC)),)
$(error "C-Compiler is not defined")
endif

CPPFLAGS = -I$(INC)
CFLAGS = -g -O0
LDFLAGS = -Wl,-rpath=$(LIB) -L$(LIB)

endif
