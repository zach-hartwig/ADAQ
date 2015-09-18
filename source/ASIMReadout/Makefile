######################################################################
#
# name: Makefile
# date: 23 Dec 14
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
#
# desc: This GNUmakefile controls the building of the ROOT-based
#       extension library for the readout and analysis of simulated
#       data within the ADAQ framework (libASIMReadout.so). The
#       purpose is provide a standardized and straightforward way to
#       save simulated detector data persistently then easily access
#       it later for analysis. For example, the user may choose to
#       readout Geant4- simulated detector data into persistent
#       storage and then seemlessly analyze it with the ADAQAnalysis
#       program.
#
#       The libASIMReadout.so is built locally within the build/
#       directory when 'make' is called and then copied - along with
#       the header files -into the top-level $ADAQ/lib/$ARCH and
#       $ADAQ/include when a 'make install' for general purpose access
#       by other codes. Note that $ARCH specifies the system
#       architecture (32- or 64-bit)
#
# dpnd: The library has only a single dependency:
#       -- The ROOT toolkit
#
# 2run: To build libASIMReadout.so locally
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

# Obtain CPU architecture: 32-bit ("i686") or 64-bit ("x86_64")
ARCH=$(shell uname -m)

# Include the Makefile for ROOT-based projects
RC:=root-config
ROOTSYS:=$(shell $(RC) --prefix)
ROOTMAKE:=$(ROOTSYS)/etc/Makefile.arch
include $(ROOTMAKE)

# Note that at this point in the makefile CXXFLAGS now includes
# ROOT-based flags so we must ensure to use "+=" to add more flags

# Include the Geant4 header files and libraries
GC:=geant4-config
G4SYS:=$(shell $(GC) --prefix)
G4INCLDIR=$(G4SYS)/include/Geant4
G4LIBS=$(shell $(GC) --libs)
CXXFLAGS += -I$(G4INCLDIR)

# Specify header files directory. Note that this must be an absolute
# path to ensure the ROOT dictionary files can find the headers
INCLDIR = $(PWD)/include
CXXFLAGS += -I$(INCLDIR)/

# Specify the build and source directories
BUILDDIR = build
SRCDIR = src

# List all of the ADAQ header files
INCLS = $(wildcard $(INCLDIR)/*.hh)

# List of headers necessary for ROOT dictionary generation
ROOTDICTINCLS = $(INCLDIR)/ASIMEvent.hh $(INCLDIR)/ASIMRun.hh $(INCLDIR)/ASIMStorageManager.hh

# Specify all object files (to be built in the build/ directory)
SRCS = $(wildcard $(SRCDIR)/*.cc)
TMP = $(patsubst %.cc,%.o,$(SRCS))
OBJS = $(subst src/,build/,$(TMP))

# Add the mandatory ROOT dictionary object file
OBJS += $(BUILDDIR)/ASIMReadoutDict.o

# Specify main library as the target
TARGET = $(BUILDDIR)/libASIMReadout.so

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

$(BUILDDIR)/ASIMReadoutDict.o : $(BUILDDIR)/ASIMReadoutDict.cc
	@echo -e "\nBuilding '$@' ..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/ASIMReadoutDict.cc : $(ROOTDICTINCLS) $(INCLDIR)/RootLinkDef.h
	@echo -e "\nGenerating ROOT dictionary '$@' ..."
	rootcint -f $@ -c -I$(ADAQHOME)/source/extensions/root/ASIMReadout/$(INCLDIR) $^ 

# PHONY rules

.PHONY:

# Clean up build files and libraries
clean:
	@echo -e "\nCleaning up the build files and libraries ..."
	@rm -f $(BUILDDIR)/*
	@echo -e ""

# Install libs and headers into top-level ADAQ directory
install:
	@echo -e "\nInstalling the ASIM library into $ADAQ/lib/$(ARCH) ..."
	@cp -v $(TARGET) $(DESTLIB)
	@echo -e "\nInstalling the ASIM headers into $ADAQ/include ..."
	@cp -v include/*.hh $(DESTINCL)
	@echo -e ""
