TOP=../../..
include $(TOP)/Setup.mak
include $(CFG)/Config.mak

TKLIB = $(LIB)/efeu/make/tklib.mak

all:: $(TKLIB)

clean::
	rm -f $(TKLIB)

$(TKLIB)::
	mkdir -p $(@D)
	$(BIN)/efeu-config update tklib.mak $@
