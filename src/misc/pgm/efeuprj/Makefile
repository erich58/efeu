# :*: EFEU specific tools
# :de: EFEU spezifische Werkzeuge

TOP=../../../..
include $(TOP)/Setup.mak
include $(CFG)/EFEU.mak

prereq::
	make -C ../tools

#LDFLAGS += -liorl -lefx -lefm -lm

include $(CFG)/SimplePrograms.mak
include $(CFG)/ApplFiles.mak

SMH = $(wildcard *.smh)
FILES= $(patsubst %,$(LIB)/shmkmf/%, $(SMH))

all:: $(FILES)

clean::
	rm -f $(FILES)

$(LIB)/shmkmf/% : %
	@mkdir -p $(@D)
	cp $< $@
