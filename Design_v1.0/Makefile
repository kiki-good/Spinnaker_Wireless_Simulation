#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file or library
# 'make clean'  removes all .o and executable files or library
#

#TOP = $(shell cd .; pwd)
#Output_Library = $(TOP)/bin

ifndef TOP
$(error TOP is not defined)
endif


$(info TOP directory is $(TOP))
#create directory for libary's final .so file
#	mkdir -p $(TOP)/bin/$(platform)
#	mkdir -p $(TOP)/bin/$(platform)/obj

#ifeq ($(platform),linuxuser)
#	CC = gcc
#	LD = gcc
#	CFLAGS += -fPIC -g
#	COUPUTFLAG = -o
#	DEBUGFLAGS  = -O0 -D _DEBUG
#	RELEASEFLAGS = -O2 -D NDEBUG -combine -fwhole-program
#	LDFLAGS = -shared
#	LibTarget = libdes.so
#else ifeq ($(platform),no-os)
#	include $(TOP)/Makefile.config
#else
#endif


#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

include $(TOP)/Makefile.config

ODIR = obj

.PHONY: depend clean

all:    $(LibTarget)

ifeq ($(platform),linuxuser)
$(LibTarget): $(OBJFILES)
	$(CC) $(INCLUDES) $(COUPUTFLAG) $(TOP)/bin/$(platform)/$(LibTarget) $(LDFLAGS) \
	$(OBJFILENAMESPATH)
else ifeq ($(platform),no-os)
$(LibTarget): $(OBJFILES)
ifeq ($(STATICLINK),1)
	$(AR) $(TOP)/bin/$(platform)/$(LibTarget) $(OBJFILENAMESPATH)
else
	$(CC) $(INCLUDES) $(COUPUTFLAG) $(TOP)/bin/$(platform)/$(LibTarget) $(LDFLAGS) \
	$(OBJFILENAMESPATH)
endif
endif
# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
ifeq ($(platform),linuxuser)
	$(CC) $(CFLAGS) $(INCLUDES) $<  -o $(TOP)/bin/$(platform)/obj/$(notdir $@)
else ifeq ($(platform),no-os)
	$(CC) $(CFLAGS) $(INCLUDES) $<  -o $(TOP)/bin/$(platform)/obj/$(notdir $@)
endif

clean:
	$(RM) $(TOP)/bin/$(platform)/*.so
	$(RM) $(TOP)/bin/$(platform)/obj/*.o	

depend: $(SRCS)
	makedepend $(INCLUDES) $^
