#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file or library
# 'make clean'  removes all .o and executable files or library
#

TOP = $(shell cd ../../../; pwd)

# Exporting a variable makes it into the environment of every shell invocation
# It is used in $(TOP)/Makefile
export TOP


include $(TOP)/Makefile.config

APPTarget = interrupt
APPSRC = interrupt.c

# TO DO...need to replace the app name with variable

APPDIR = $(TOP)/example/unitest/interrupt
APPBINARYDIR = $(TOP)/example/unitest/interrupt/$(platform)

# define linker and compiler flag for App

ifeq ($(platform),no-os)
APPLDFLAGS = -ldes_no_os
APPSUPPORTNOOS += -DSUPPORT_NO_OS
else ifeq ($(platform),linuxuser)
APPLDFLAGS = -ldes
else
endif

APPLDFLAGS += -L $(TOP)/bin/$(platform)

#define Run Time library for libdes.so
RPATH = -Wl,-rpath,$(TOP)/bin/$(platform) 

# define the target file 
# TO DO.....
#Library += libdes.so


# include Spinnaker Library


#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

ifeq ($(platform),linuxuser)
all:    $(APPBINARYDIR)/$(APPTarget)
#$(APPTarget): $(APPTarget).*
$(APPBINARYDIR)/$(APPTarget): $(APPTarget).c
# inclue Makefile to build Discrete Event Simulator library
# Make runs in a different shell so there is no need to change
# the directory back
	$(MAKE) -C $(TOP)	
	$(CC) $(APPSUPPORTNOOS) $(RPATH) $(INCLUDES) $(COUPUTFLAG) $(APPBINARYDIR)/$(APPTarget) $(APPSRC) $(APPLDFLAGS) 
else ifeq ($(platform),no-os)
#-------------------------------------------------------------------------------

# Build the application

# List of objects making up the application. If there are other files
# in the application, add their object file names to this variable.

OBJECTS := $(APPBINARYDIR)/$(APPTarget).o

# Primary target is an APLX file - built from the ELF

#  1) Create a binary file which is the concatenation of RO and RW sections
#  2) Make an APLX header from the ELF file with "mkaplx" and concatenate
#     that with the binary to make the APLX file
#  3) Remove temporary files and "ls" the APLX file

$(APPBINARYDIR)/$(APPTarget).aplx: $(APPBINARYDIR)/$(APPTarget).elf
ifeq ($(GNU),1)
	$(OC) -O binary -j RO_DATA -j .ARM.exidx $(APPBINARYDIR)/$(APPTarget).elf RO_DATA.bin
	$(OC) -O binary -j RW_DATA $(APPBINARYDIR)/$(APPTarget).elf RW_DATA.bin
	$(TOOLS_DIR)/mkbin RO_DATA.bin RW_DATA.bin > $(APPBINARYDIR)/$(APPTarget).bin
else
	$(OC) --bin --output $(APPBINARYDIR)/$(APPTarget).bin $(APPBINARYDIR)/$(APPTarget).elf
endif
	$(TOOLS_DIR)/mkaplx $(APPBINARYDIR)/$(APPTarget).elf | $(CAT) - $(APPBINARYDIR)/$(APPTarget).bin > $(APPBINARYDIR)/$(APPTarget).aplx
	$(RM) $(APPBINARYDIR)/$(APPTarget).bin RO_DATA.bin RW_DATA.bin
	$(LS) $(APPBINARYDIR)/$(APPTarget).aplx

# Build the ELF file

#  1) Make a "sark_build.c" file containing app. name and build time
#     with "mkbuild" and compile it
#  2) Link application object(s), build file and library to make the ELF
#  3) Tidy up temporaries and create a list file

$(APPBINARYDIR)/$(APPTarget).elf: $(OBJECTS) $(SCRIPT) $(LIBRARY) 
	$(TOOLS_DIR)/mkbuild $(APPBINARYDIR)/$(APPTarget) > sark_build.c
	$(CC) $(CFLAGSNOPIC) sark_build.c
	$(LD) $(LFLAGS) $(OBJECTS) sark_build.o $(APPLDFLAGS) $(LIBRARY) -o $(APPBINARYDIR)/$(APPTarget).elf
	$(RM) sark_build.c sark_build.o
	$(OD) $(APPBINARYDIR)/$(APPTarget).elf


# Build the main object file. If there are other files in the
# application, place their build dependencies below this one.

$(APPBINARYDIR)/$(APPTarget).o: $(APPTarget).c $(INC_DIR)/spinnaker.h $(INC_DIR)/sark.h \
	  $(INC_DIR)/spin1_api.h
	$(MAKE) -C $(TOP)
	$(CC) $(APPSUPPORTNOOS) $(CFLAGS) $(INCLUDES) $(APPTarget).c $(COUPUTFLAG) $(APPBINARYDIR)/$(APPTarget).o
endif
		
clean: clean_app

clean_app:
	$(MAKE) -C $(TOP) clean
#	$(RM) $(APPBINARYDIR)/$(APPTarget)
	$(RM) $(APPBINARYDIR)/*
	$(RM) sark_build*
	$(RM) a.out

depend: depend_app

depend_app: $(SRCS)
	makedepend $(INCLUDES) $^
