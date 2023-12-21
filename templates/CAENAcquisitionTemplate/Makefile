# name: Makefile 
# date: 20 Dec 23
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
# desc: GNU makefile for building the CAENAcquisitionTemplate

#***************************#
#**** MACRO DEFINITIONS ****#
#**************************#*

# Specify the the binary, build, and source directories
BUILDDIR = build
BINDIR = bin
SRCDIR = src

# Specify absolute path to the header files directory. 
INCLDIR = $(PWD)/include

# Specify all header files
INCLS = $(INCLDIR)/*.hh

# Specify all object files (to be built in the build/ directory)
SRCS = $(wildcard $(SRCDIR)/*.cc)
TMP = $(patsubst %.cc,%.o,$(SRCS))
OBJS = $(subst src/,build/,$(TMP))

# Specify the other necessary header file locations
CXXFLAGS += -I$(ADAQHOME)/include -I$(INCLDIR)

# Specify the location of the ADAQ control library
LDFLAGS+=-L$(ADAQHOME)/lib/$(HOSTTYPE) -lADAQControl

# Specify the location of the CAEN libraries (architecture-dependent)
LDFLAGS+=-L$(ADAQHOME)/lib/$(HOSTTYPE) -lCAENVME -lCAENComm -lCAENDigitizer -lncurses -lc -lm

# Specify Boost libraries to link against
LDFLAGS+=-lboost_thread

# Specify the target
TARGET = $(BINDIR)/CAENAcquisitionTemplate

#***************#
#**** RULES ****#
#***************#

# Build the main binary

$(TARGET) : $(OBJS) 
	@echo -e "\nBuilding the binary $@ ..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(ROOTGLIBS) $(ROOTLIB)
	@echo -e "\n$@ build is complete!\n"

$(BUILDDIR)/%.o : $(SRCDIR)/%.cc $(INCLS)
	@echo -e "\nBuilding object file '$@' ..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean the directory of all build files and binaries
.PHONY: 
clean:
	@echo -e "\nCleaning up the build and binary directories ..."
	rm -f $(BUILDDIR)/* $(BINDIR)/*
	@echo -e ""

# Useful notes for the uninitiated:
#
# target : dependency list
#  --TAB-- rule
#
# "$@" == subst. the word to the left of the ":"
# "$^" == subst. the words to the right of ":"
# "$<" == subst. first item in dependency list
