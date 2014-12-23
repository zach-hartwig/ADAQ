######################################################################
#
# name: Makefile
# date: 23 Dec 14
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
#
# desc: This GNUmakefile controls the building of the ROOT-based
#       extension library for the readout and analysis of simulated
#       data within the ADAQ framework. The purpose is provide a
#       standardized and straightforward way to save simulated
#       detector data persistently then easily access it later for
#       analysis. For example, the user may choose to readout Geant4-
#       simulated detector data into persistent storage and then
#       seemlessly analyze it with the ADAQAnalysis program.
#
#       The ADAQSimulationReadout.so library is built locally within
#       the build/ directory when 'make' is called and then copied -
#       along with the header files -into the top-level
#       $ADAQ/lib/$ARCH and $ADAQ/include when a 'make install' for
#       general purpose access by other codes. Note that $ARCH
#       specifies the system architecture (32- or 64-bit)
#
# dpnd: The library has only a single dependency:
#       -- The ROOT toolkit
#
# 2run: To build the ADAQSimulationReadout.so library locally
#       $ make
# 
#       To install library and headers in $ADAQ/lib and $ADAQ/include
#       $ make install
#
#       To clean up transient build files and delete local library
#       $ make clean
#    
######################################################################

#***************************#
#**** MACRO DEFINITIONS ****#
#***************************#

# Include the Makefile for ROOT-based projects
RC:=root-config
ROOTSYS:=$(shell $(RC) --prefix)
ROOTMAKE:=$(ROOTSYS)/etc/Makefile.arch
include $(ROOTMAKE)

# Note that CXXFLAGS now includes ROOT-based flags to we must ensure
# to use "+=" to add more flags to CXXFLAGS after this point

# Specify compilers and basic flags
CXX = clang++
XXFLAGS += -ferror-limit=5 -w

# Obtain CPU architecture: 32-bit ("i686") or 64-bit ("x86_64")
ARCH=$(shell uname -m)

# Specify header files directory. Note that this must be an absolute
# path to ensure the ROOT dictionary files can find the headers
INCLDIR = $(PWD)/include

# Specify the build and source directories
BUILDDIR = build
SRCDIR = src

CXXFLAGS += -I$(INCLDIR)/

# Specify all the ADAQ header files
INCLS = $(wildcard $(INCLDIR)/*.hh)

# Specify all object files (to be built in the build/ directory)
SRCS = $(wildcard $(SRCDIR)/*.cc)
TMP = $(patsubst %.cc,%.o,$(SRCS))
OBJS = $(subst src/,build/,$(TMP))

# Add the mandatory ROOT dictionary object file
OBJS += $(BUILDDIR)/ADAQSimulationReadoutDict.o

# Specify main library as the target
TARGET = $(BUILDDIR)/libADAQSimulationReadout.so

# Specify the top-level destination directory
DESTINCL = $(ADAQHOME)/include
DESTLIB = $(ADAQHOME)/lib/$(ARCH)


#***************#
#**** RULES ****#
#***************#


# Build shared object libary 

$(TARGET) : $(OBJS) $(INCLS)
	@echo -e "\nBuilding $@ ..."
	$(CXX) $(CXXFLAGS) -g $(OBJS) $(ROOTLIBS) -shared -o $@ 
	@echo -e "\n$@ build is complete!\n"

$(BUILDDIR)/%.o : $(SRCDIR)/%.cc $(INCLS)
	@echo -e "\nBuilding object file '$@' ..."
	$(CXX) $(CXXFLAGS) -g -c -o $@ $<

# Build the ROOT dictionary

$(BUILDDIR)/ADAQSimulationReadoutDict.o : $(BUILDDIR)/ADAQSimulationReadoutDict.cc
	@echo -e "\nBuilding '$@' ..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/ADAQSimulationReadoutDict.cc : $(INCLS) $(INCLDIR)/RootLinkDef.h
	@echo -e "\nGenerating ROOT dictionary '$@' ..."
	rootcint -f $@ -c -I$(ADAQHOME)/source/extensions/root/simulationReadout/$(INCLDIR) $^ 

# PHONY rules

.PHONY:

# Clean up build files and libraries
clean:
	@echo -e "\nCleaning up the build files and libraries ..."
	@rm -f $(BUILDDIR)/*
	@echo -e ""

# Install libs and headers into top-level ADAQ directory
install:
	@echo -e "\nInstalling ADAQ libraries in $ADAQ/lib/$(ARCH) ..."
	@cp -v $(TARGET) $(DESTLIB)
	@echo -e "\nInstalling ADAQ headers in $ADAQ/include ..."
	@cp -v include/* $(DESTINCL)
	@echo -e ""
