######################################################################
#
# name: Makefile
# date: 02 Oct 14
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
#
# desc: This GNUmakefile controls the building of the ADAQ C++ and
#       Python shared object libraries. Macros prefix "CXX" refers to
#       those used to build the ADAQ C++ shared object library
#       ("libADAQ.so") and "PY" refers to those used to build the ADAQ
#       Python shared object library ("libPyADAQ.so"), with the
#       exception that "CXX" (full macro) specifies clang as the
#       compiler that will be used to build both. The target libraries
#       are built locally in "lib/<arch> during "make" and then copied
#       to the "../lib/<arch> during "make install", where <arch> is
#       either i686 (32-bit) or x86_64 (64-bit). The makefile should
#       automatically determine the architecture using the 'uname -m'
#       command.
#
# dpnd: The ADAQ libraries have three main dependencies:
#       -- The CAEN Libraries (most recent version):
#          -> CAENDigitizer (v.2.6.0)
#          -> CAENVMELib (v2.41.0)
#          -> CAENComm (v1.2.0)
#       -- Python (headers and libraries)
#       -- Boost.Python (compiled library)
#
# 2run: To build both C++ and Python libraries:
#       $ make
# 
#       To install ADAQ libraries in $ADAQREPO/lib:
#       $ make install
#
#       To clean up build files and delete local libraries:
#       $ make clean
#    
######################################################################

#***************************#
#**** MACRO DEFINITIONS ****#
#***************************#

# Specify compilers and basic flags
CXX = g++

# WARNING: At present (02 Oct 14) building the ADAQ libraries with
# LLVM/clang compilers (kernel 3.14.19-100.fc19.x86_64) succeeds but
# ADAQAcquisition will seg. fault. upon attempt to connect to VME
# system. I do not understand this at the moment (nor do I have time
# to pursue the solution), but I suspect it has something to do with
# incorrect inclusion of the CAEN libraries within libADAQ.so.
# CXX = clang++ 
# CXXFLAGS = -ferror-limit=5 -w

# Obtain CPU architecture: 32-bit ("i686") or 64-bit ("x86_64")
ARCH=$(shell uname -m)

# Specify the "-fPIC" flag for 64-bit architecture
ifeq ($(ARCH),x86_64)
  CXXFLAGS += -fPIC
endif

# Specify the directories
BUILDDIR = build
INCLDIR = include
LIBDIR = lib/$(ARCH)
SRCDIR = src

# Add ADAQ headers to compiler include path
CXXFLAGS += -I$(INCLDIR)/

# Specify all the ADAQ header files
INCLS = $(wildcard $(INCLDIR)*.hh)

# Specify all object files (to be built in the build/ directory)
SRCS = $(wildcard $(SRCDIR)/*.cc)
TMP = $(patsubst %.cc,%.o,$(SRCS))
OBJS = $(subst src/,build/,$(TMP))

# Specify the CAEN libraries (included with ADAQ source)
CAENLIBDIR = -L../caen/lib/$(ARCH)
CAENLIBS = -lCAENVME -lCAENComm -lCAENDigitizer -lncurses -lc -lm
LDFLAGS = $(CAENLIBDIR) $(CAENLIBS)

# Specify the CAEN header files (included with ADAQ source) and Linux flag
CAENINCLDIR = -I../caen/include
CAENFLAGS = -DLINUX
CXXFLAGS += $(CAENINCLDIR) $(CAENFLAGS)

# Determine the major and minor revision number of Python and use them
# to dynamically set the correct include directory for Python headers
# on the present machine
PY_VERSION_FULL := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
PY_VERSION_MAJOR := $(word 1,${PY_VERSION_FULL})
PY_VERSION_MINOR := $(word 2,${PY_VERSION_FULL})

# Set the Python include dir in compiler flag
PYINCL = -I/usr/include/python$(PY_VERSION_MAJOR).$(PY_VERSION_MINOR)

# Set the Phyton library dir and required libs in compiler flag
PYLIB = -L/usr/lib64/python$(PY_VERSION_MAJOR).$(PY_VERSION_MINOR)/config \
	-lpython$(PY_VERSION_MAJOR).$(PY_VERSION_MINOR)

# Set the Python compiler flags
PYFLAGS = -fPIC -shared -Wl,--export-dynamic

# Set the location of the C++ Boost libraries. If Boost has been
# installed from source then the Boost.Python libraries must be built
# separately by the user. After this has been accomplished, BOOST_ROOT
# should defined in the user's environment (from .bashrc ideally) to
# point to the top level of the Boost installation directory. If Boost
# has been installed via the Linux OS package manager (e.g. "apt-get"
# on Ubuntu or "yum" on Fedora) then all required Boost functionality
# will be automatically included in the user's path and linker. In
# either case, the final ADAQ Python library must be linked against
# libboost_python
ifdef BOOST_ROOT
  BOOSTINCL = -I$(BOOST_ROOT)
  BOOSTLIB = -L$(BOOST_ROOT)/stage/lib -lboost_python
else
  BOOSTLIB= -lboost_python
endif

# Targets are C++ and Python shared object libraries
CXXTARGET=$(BUILDDIR)/libADAQ.so
PYTARGET=$(BUILDDIR)/libPyADAQ.so

all: $(CXXTARGET) $(PYTARGET)


#***************#
#**** RULES ****#
#***************#


#******************************#
# Build C++ shared object libary 

$(CXXTARGET) : $(OBJS) $(INCLS)
	@echo -e "\nBuilding $@ ..."
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -g -shared $^ -o $@
	@echo -e "\n$@ build is complete!\n"

$(BUILDDIR)/%.o : $(SRCDIR)/%.cc $(INCLS)
	@echo -e "\nBuilding object file '$@' ..."
	$(CXX) $(CXXFLAGS) -g -c -o $@ $<


#*********************************#
# Build Python shared object libary

$(PYTARGET) : $(OBJS)
	@echo -e "\nBuilding $@ ..."
	$(CXX) $(PYFLAGS) $(BOOSTLIB) $(PYLIB) $(CAENLIBDIR) $(CAENLIB) -o $@ $<
	@echo -e "\n---> Finished building $@!\n"

$(BUILDDIR)/ADAQPythonWrapper.o : $(SRC)/ADAQPythonWrapper.pcc $(INCLS)
	@echo -e "\nBuilding object file '$@' ..."
	$(CXX) $(BOOSTINCL) $(ADAQINCL) $(PYINCL) $(PYFLAGS) $(CAENINCL) -o $@ -c $<


# General cleanup
.PHONY:
clean:
	@echo -e "\nCleaning up the build files and libraries ..."
	@rm -f $(BUILDDIR)/*
	@echo -e ""

# Install libs and headers to general directory
install:
	@echo -e "\nInstalling ADAQ libraries in ../lib/$(ARCH) ..."
	@cp -v $(CXXTARGET) $(PYTARGET) ../lib/$(ARCH)
	@echo -e "\nInstalling ADAQ headers in ../include ..."
	@cp -v include/* ../include
	@echo -e ""
