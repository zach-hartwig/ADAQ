######################################################################
#
# name: Makefile
# date: 05 Jan 14
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
#
# desc: This GNUmakefile controls the building of the ROOT-based
#       extension library for the readout and analysis of experimental
#       data acquired from CAEN data acquisition hardware entitled the
#       libADAQReadout.so library. The purpose is to provide a
#       standardized and straightforward way to save experimental data
#       persistently then easily access it later with analysis
#       programs such as ADAQAnalysis.
#       
#       The library is built locally within the build/ directory when
#       'make' is called and then copied - along with the header files
#       -into the top-level $ADAQ/lib/$ARCH and $ADAQ/include when a
#       'make install' for general purpose access by other codes. Note
#       that $ARCH specifies the system architecture (32- or 64-bit)
#
# dpnd: The library has only a single dependency:
#       -- The ROOT toolkit
#
# 2run: To build libADAQReadout.so locally:
#       $ make
# 
#       To install library and headers in $ADAQ/lib and $ADAQ/include:
#       $ make install
#
#       To clean up transient build files and delete local library:
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

# Note that at this point in the makefile CXXFLAGS now includes
# ROOT-based flags so we must ensure to use "+=" to add more flags

# Add C++11 capabilities
CXXFLAGS += -std=c++11

# Obtain CPU architecture: 32-bit ("i686") or 64-bit ("x86_64") (Must
# be set after the ROOT makefile is included)
ARCH=$(shell uname -m)

# Specify header files directory. Note that this must be an absolute
# path to ensure the ROOT dictionary files can find the headers
INCLDIR = $(PWD)/include

# Specify the build and source directories
BUILDDIR = build
SRCDIR = src

CXXFLAGS += -I$(INCLDIR)/ -I$(ADAQHOME)/include

# Specify all the ADAQ header files
INCLS = $(wildcard $(INCLDIR)/*.hh)

# Specify all object files (to be built in the build/ directory)
SRCS = $(wildcard $(SRCDIR)/*.cc)
TMP = $(patsubst %.cc,%.o,$(SRCS))
OBJS = $(subst src/,build/,$(TMP))

# Add the mandatory ROOT dictionary object file
OBJS += $(BUILDDIR)/ADAQReadoutDict.o

# Specify main library as the target
TARGET = $(BUILDDIR)/libADAQReadout.so

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

$(BUILDDIR)/ADAQReadoutDict.o : $(BUILDDIR)/ADAQReadoutDict.cc
	@echo -e "\nBuilding '$@' ..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/ADAQReadoutDict.cc : $(INCLS) $(INCLDIR)/RootLinkDef.h
	@echo -e "\nGenerating ROOT dictionary '$@' ..."
	rootcling -f $@ -c $^

# PHONY rules

.PHONY:

# Clean up build files and libraries
clean:
	@echo -e "\nCleaning up the build files and libraries ..."
	@rm -f $(BUILDDIR)/*
	@echo -e ""

# Install libs and headers into top-level ADAQ directory
install:
	@echo -e "\nInstalling the ADAQReadout library and PCM files in $(ADAQ)/lib/$(ARCH) ..."
	@cp -v $(TARGET) $(DESTLIB)
	@cp -v build/*.pcm $(DESTLIB)
	@echo -e "\nInstalling the ADAQReadout headers in $(ADAQ)/include ..."
	@cp -v include/*.hh $(DESTINCL)
	@echo -e ""

test:
	@echo $(INCLDIR)
