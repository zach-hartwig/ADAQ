# name: Makefile 
# date: 26 Jan 15
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
# desc: GNU makefile for building the CAENAcquisitionTemplate

#***************************#
#**** MACRO DEFINITIONS ****#
#**************************#*

# Include the Makefile for ROOT-based projects
RC:=root-config
ROOTSYS:=$(shell $(RC) --prefix)
ROOTMAKE:=$(ROOTSYS)/etc/Makefile.arch
include $(ROOTMAKE)

# Specify the the binary, build, and source directories
BUILDDIR = build
BINDIR = bin
SRCDIR = src

# Specify header files directory. Note that this must be an absolute
# path to ensure the ROOT dictionary files can find the headers
INCLDIR = $(PWD)/include

# Specify all header files
INCLS = $(INCLDIR)/*.hh

# Specify all object files (to be built in the build/ directory)
SRCS = $(wildcard $(SRCDIR)/*.cc)
TMP = $(patsubst %.cc,%.o,$(SRCS))
OBJS = $(subst src/,build/,$(TMP))

# Add the mandatory ROOT dictionary object file
OBJS += $(BUILDDIR)/CAENAcquisitionTemplateDict.o

# Specify the compiler
CXX = clang++

# Specify the other necessary header file locations
CXXFLAGS += -I$(ADAQHOME)/include -I$(INCLDIR)

# Specify the location of the ADAQ control library
LDFLAGS+=-L$(ADAQHOME)/lib/$(HOSTTYPE) -lADAQControl

# Specify the location of the CAEN libraries (architecture-dependent)
LDFLAGS+=-L$(ADAQHOME)/lib/$(HOSTTYPE) -lCAENVME -lCAENComm -lCAENDigitizer -lncurses -lc -lm

# Specify Boost libraries to link against
# C-Mod workstations)
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

# Build the ROOT dictionary for user's custom classes

$(BUILDDIR)/CAENAcquisitionTemplateDict.o : $(BUILDDIR)/CAENAcquisitionTemplateDict.cc
	@echo -e "\n---> Building $@ ..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/CAENAcquisitionTemplateDict.cc : $(INCLS) $(INCLDIR)/RootLinkDef.h
	@echo -e "\nGenerating ROOT dictionary $@ ..."
	@rootcint -f $@ -c -I$(ADAQHOME)/include $^

# Clean the directory of all build files and binaries
.PHONY: 
clean:
	@echo -e "\n---> Cleaning up the build and binary ..."
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
