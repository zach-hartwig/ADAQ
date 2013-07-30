######################################################################
# name: Makefile
# date: 17 Apr 13
# auth: Zach Hartwig
#
# desc: This GNUmakefile controls the building of the ADAQ C++ and
#       Python shared object libraries. Macros prefix "CXX" refers to
#       those used to build the ADAQ C++ shared object library
#       ("libADAQ.so") and "PY" refers to those used to build the ADAQ
#       Python shared object library ("libPyADAQ.so"), with the
#       exception that "CXX" (full macro) specifies gcc as the
#       compiler that will be used to build both. The target libraries
#       will be placed in the "./lib/<architecture>/" directory, where
#       <architecture> is either i686 (32-bit) or x86_64 (64-bit). The
#       ADAQ Setup.sh bash script should automatically set the
#       HOSTTYPE environmental variable via the 'uname -m' command.
#
#       There are essentially three dependencies required to build the
#       ADAQ libraries:
#       
#       0. The CAEN libraries (included with the ADAQ libraries):
#          --> CAENDigitizer, CAENComm, CAENVME
#          --> see http://www.caen.it/csite/ProductSupport.jsp
#
#       1. Boost.Python (a Boost library that must be compiled)
#          --> see http://www.boost.org/doc/libs/1_53_0/libs/python/doc/
#
#       2. Python (requires development headers/libraries)
#    
######################################################################


######################################
# Define all necessary build options #
######################################

# Sequential or parellel architecture
ARCH=$(HOSTTYPE)

# The directory that this Makefile lives in (convenient macro for
# specifying full path names to various items)
DIR=$(PWD)

# Specify gcc as the compiler with debug flags
CXX=g++ -g

# Add the directory with ADAQ and CAEN header files to compiler flag
INCL=$(DIR)/include
HDRS=$(wildcard $(INCL)/*)
ADAQINCL=-I$(INCL)

# Identify the C++ source and object files
SRC=$(DIR)/src
SRCS=$(wildcard $(SRC)/*.cc)
CXXOBJS=$(SRC)/ADAQDigitizer.o $(SRC)/ADAQHighVoltage.o 
PYOBJS=$(SRC)/ADAQPythonWrapper.o

# Set the CAEN library and include directories/flags
CAENLIBDIR=-L$(ADAQHOME)/lib/$(HOSTTYPE)
CAENLIB=-lCAENVME -lCAENComm -lCAENDigitizer -lncurses -lc -lm
CAENINCL=-I../include

# Specify the "-fPIC" (Position Independent Code) flag for 64-bit
# architecture
ifeq ($(ARCH),x86_64)
  CXXFLAGS+=-fPIC
endif

# Determine the major and minor revision number of Python and use them
# to dynamically set the correct include directory for Python headers
# on the present machine
PY_VERSION_FULL := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
PY_VERSION_MAJOR := $(word 1,${PY_VERSION_FULL})
PY_VERSION_MINOR := $(word 2,${PY_VERSION_FULL})

# Set the Python include dir in compiler flag
PYINCL = -I/usr/include/python$(PY_VERSION_MAJOR).$(PY_VERSION_MINOR)

# Set the Pyton library dir and required libs in compiler flag
PYLIB = -L/usr/lib64/python$(PY_VERSION_MAJOR).$(PY_VERSION_MINOR)/config \
	-lpython$(PY_VERSION_MAJOR).$(PY_VERSION_MINOR)

# Set the compiler flags for building the ADAQ Python libraries
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

###################
# Begin the build #
###################

# Targets are C++ and Python shared object libraries
CXXTARGET=$(DIR)/lib/$(ARCH)/libADAQ.so
PYTARGET=$(DIR)/lib/$(ARCH)/libPyADAQ.so

all: $(CXXTARGET) $(PYTARGET)


##################################
# Build C++ shared object libary #
##################################

# Build libADAQ.so shared library
$(CXXTARGET) : $(CXXOBJS)
	@echo -e "\n---> Building the dynamic library $@ ..."
	@g++ -shared $^ -o $@
	@echo -e "\nFinished building $@!\n"

# Build the ADAQDigitizer object from source
$(SRC)/ADAQDigitizer.o : $(SRC)/ADAQDigitizer.cc $(INCL)/ADAQDigitizer.hh
	@echo -e "\n---> Building $@ ..."
	$(CXX) $(CXXFLAGS) $(ADAQINCL) $(CAENLIBDIR) $(CAENLIB) $(CAENINCL) -c -o $@ $<

# Build the ADAQHighVoltage object from source
$(SRC)/ADAQHighVoltage.o : $(SRC)/ADAQHighVoltage.cc $(INCL)/ADAQHighVoltage.hh
	@echo -e "\n---> Building $@ ..."
	$(CXX) $(CXXFLAGS) $(ADAQINCL) $(CAENLIBDIR) $(CAENLIB) $(CAENINCL) -c -o $@ $<


#####################################
# Build Python shared object libary #
#####################################

# Build the libPyADAQ.so shared library
$(PYTARGET) : $(PYOBJS)
	@echo -e "\n---> Building $@ ..."
	$(CXX) $(PYFLAGS) $(BOOSTLIB) $(PYLIB) $(CAENLIBDIR) $(CAENLIB) -o $@ $<
	@echo -e "\n---> Finished building $@!\n"

# Build the ADAQPythonWrapper object from source
$(SRC)/ADAQPythonWrapper.o : $(SRC)/ADAQPythonWrapper.cc $(HDRS) $(SRCS)
	@echo -e "\n--->Building $@ ..."
	$(CXX) $(BOOSTINCL) $(ADAQINCL) $(PYINCL) $(PYFLAGS) $(CAENINCL) -o $@ -c $<


#########
# Phony #
#########

# General cleanup
.PHONY:
clean:
	@echo -e "\nRemoving ADAQ library object files ..."
	@rm -f $(CXXTARGET) $(PYTARGET) $(CXXOBJS) $(PYOBJS)
	@echo -e ""

# Install libs and headers to general directory
.PHONY:
install:
	@echo -e "\nInstalling ADAQ libraries in \$$ADAQHOME/lib/$(ARCH) ..."
	@cp -v lib/$(ARCH)/* ../$(ARCH)
	@echo -e "\nInstalling ADAQ headers in \$$ADAQHOME/include/ ..."
	@cp -v include/* ../include
	@echo -e ""

