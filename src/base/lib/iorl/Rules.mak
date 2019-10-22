#LD_CURSES = $(shell $(BIN)/efeu-config ldflags -lncurses || \
#	$(BIN)/efeu-config ldflags -lcurses)
#LD_READLINE = $(shell $(BIN)/efeu-config ldflags -lreadline)
#
#LDFLAGS += $(LD_READLINE) $(LD_CURSES)

LDFLAGS += -lreadline -lncurses
