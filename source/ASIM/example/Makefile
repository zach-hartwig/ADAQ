#############################################################################
#
# name: Makefile
# date: 25 Jun 15
# auth: Zach Hartwig
#
# desc: This file is the GNU makefile that controls the ASIMExample build
#       system. Users beware that this is no ordinary Geant4 code! The
#       build system handles a number fancy maneuvers including: 
#
#       -- optional parallel build of ASIMExample with Open MPI 
#       -- generating  dictionaries for data readout into ROOT framework 
#       -- inclusion Boost C++ library dependencies
#       -- inclusion of ADAQ simulation readout extension library
#
# dpnd: 1. The ROOT toolkit (mandatory)
#       2. The Boost C+ libraries (mandatory)
#       3. ADAQ libraries (mandatory)
#       4. Open MPI (optional)
#
#############################################################################

# Sequential and parallel binary names
SEQ_TARGET := ASIMExample
PAR_TARGET := ASIMExample_MPI

ASIMExampleLIBS := libASIMExampleRoot.so

# Architecture: sequential (default), parallel (set by .PHONY 'parallel')
ARCH := sequential


##################
#  G4 Makefile  #
##################

# Set the architecture-dependent 'name' and G4TARGET variables. Note
# that the 'name' variable is required to set the name of the build
# directory in $G4INSTALL/tmp/$G4SYSTEM directory.

ifeq ($(ARCH),sequential)
  name := $(SEQ_TARGET)
else
  name := $(PAR_TARGET)
endif

G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

 .PHONY: all
all: lib/$(ASIMExampleLIBS) lib bin

# Exclude visualization for parallelized builds on PSFC Loki cluster,
# which is named 'head' via HOSTNAME env. variable
#
# Summary:
#   sws/cmodws (both) : enabled
#   Loki (serial)     : enabled
#   Loki (parallel)   : disabled

ifeq ($(HOSTNAME),head)
  ifeq ($(ARCH),parallel)
    include $(G4INSTALL)/config/binmake.gmk#.NO_VISUALIZATION
  else
    include $(G4INSTALL)/config/binmake.gmk
  endif	
else
  include $(G4INSTALL)/config/binmake.gmk
endif

# Now that G4 Makefile is included we can update the compilers
CC=clang
CXX=clang++

# Newest version of G4 with ROOT results in massive warnings output
# since ROOT local variable 's' shadows the G4Unit 's' for
# seconds. This doesn't effect anything so suppress warning.
CXXFLAGS := $(subst -Wshadow,,$(CXXFLAGS))


##########
#  ROOT  #
##########

# Several ROOT classes are presently used in ASIMExample for their immense
# utility; this requires compiling and linking against ROOT as a
# dependency. Use 'root-config' to obtain the header and library locations. 
ROOTINCLUDES = -I$(shell root-config --incdir)
ROOTDISTLIBS = $(shell root-config --nonew --libs --glibs)

CPPFLAGS += $(ROOTINCLUDES)
LDLIBS += $(ROOTDISTLIBS) -L./lib -lASIMExampleRoot

lib/libASIMExampleRoot.so : lib/ASIMExampleDict.o 
	@echo -e "\nBuilding the ASIMExample ROOT library ...\n"
	@$(CXX) -shared -o $@  $^

lib/ASIMExampleDict.o : lib/ASIMExampleDict.cc
	@echo -e "Building $@ ..."
	@$(CXX) $(CXXFLAGS) $(ROOTINCLUDES) -I. -c -o $@ $<

lib/ASIMExampleDict.cc : include/RootLinkDef.hh
	@echo -e "Generating the ASIMExample ROOT dictionary ..."
	@rootcint -f $@ -c $^


########
# ADAQ #
########

# The ADAQ Simulation Readout (ASIM) library is used for readout of
# detector data into persistent ROOT simulation (*.asim.root) files
CPPFLAGS += -I$(ADAQHOME)/include
LDLIBS += -L$(ADAQHOME)/lib/$(HOSTTYPE) -lASIMReadout


#########################
#  Parallization / MPI  #
#########################

# Necessary flags to build ASIMExample with MPI parallelization via the
# customized acroMPImanager/messenger interface.  This conditional is
# triggered automatically via the "parallel.sh" build script.
ifeq ($(ARCH),parallel)

  # Open MPI C++ compiler
  CXX := mpic++

  # Flag for parallel build enabled by parallel.sh 
  ASIM_MPI_ENABLED := 1

  # Necessary flags for parallel compilation
  CPPFLAGS += -I$(MPIHOME)/include/ \
              -DASIM_MPI_ENABLED
endif


.PHONY:

libclean:
	@echo -e "\nCleaning up the ASIMExample libraries ...\n"
	@rm lib/* -f

flags:
	@echo -e "\nCPPFLAGS = $(CPPFLAGS)\n"
	@echo -e "CXXFLAGS = $(CXXFLAGS)\n"
