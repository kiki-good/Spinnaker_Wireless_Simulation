ifndef SPINN_DIRS
DIRS = sark spin1_api
$(info ************  ifndef SPINN_DIRS ************)
else
DIRS = $(SPINN_DIRS)/sark $(SPINN_DIRS)/spin1_api
$(info ************ def SPINN_DIRS ************)
endif

all: $(DIRS)
	for d in $(DIRS); do (cd $$d; "$(MAKE)") || exit $$?; done

clean: $(DIRS)
	for d in $(DIRS); do (cd $$d; "$(MAKE)" clean) || exit $$?; done
