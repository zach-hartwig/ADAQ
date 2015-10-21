#############################################################################
#
# name: Makefile
# date: 20 Oct 15
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
#       To build the simulation for sequential processing:
#         $ make
#
#       To build the simulation with Open MPI for parallel processing
#       (requires the ASIM libraries to be built with Open MPI)
#         $ make PROC=PAR
#
# dpnd: 1. The ROOT toolkit (mandatory)
#       2. The Boost C+ libraries (mandatory)
#       3. ADAQ libraries (mandatory)
#       4. Open MPI (optional)
#
#############################################################################

# Name for the binary
name := ASIMExample

# Name for the associated ROOT shared object library
ASIMExampleLIBS := libASIMExampleRoot.so

# Define sequential (SEQ) processing as the default build mode
PROC := SEQ


##################
#  G4 Makefile  #
##################

# Set the architecture-dependent 'name' and G4TARGET variables. Note
# that the 'name' variable is required to set the name of the build
# directory in $G4INSTALL/tmp/$G4SYSTEM directory.

G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

 .PHONY: all
all: lib bin # lib/$(ASIMExampleLIBS)

ifeq ($(PROC),PAR)
  include $(G4INSTALL)/config/binmake.gmk#.NO_VISUALIZATION
else
  include $(G4INSTALL)/config/binmake.gmk
endif

# Prevent shadow warnings needed to suppress ROOT/Geant4 shadowing
# mainly of global variable "s". This may come back to bite me...
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
LDLIBS += $(ROOTDISTLIBS)

# The following code enables the user to generate a ROOT dictionary is
# necessary in order to use custom TObject-derived classes within ROOT
# in post-processing. At present, there are no such classes so we'll
# comment this out.

# lib/libASIMExampleRoot.so : lib/ASIMExampleDict.o 
# 	@echo -e "\nBuilding the ASIMExample ROOT library ...\n"
# 	@$(CXX) -shared -o $@  $^

# lib/ASIMExampleDict.o : lib/ASIMExampleDict.cc
# 	@echo -e "Building $@ ..."
# 	@$(CXX) $(CXXFLAGS) $(ROOTINCLUDES) -I. -c -o $@ $<

# lib/ASIMExampleDict.cc : include/RootLinkDef.hh
# 	@echo -e "Generating the ASIMExample ROOT dictionary ..."
# 	@rootcint -f $@ -c $^


########
# ASIM #
########

# The ADAQ Simulation Readout (ASIM) libraries are used for readout of
# detector data into persistent ROOT simulation (*.asim.root) files
CPPFLAGS += -I$(ADAQHOME)/include
LDLIBS += -L$(ADAQHOME)/lib/$(HOSTTYPE) -lASIMReadout -lASIMStorage


#########################
#  Parallization / MPI  #
#########################

# Necessary flags to build ASIMExample with MPI parallelization. Note
# that the user should have the environment variable MPIHOME set to
# point to the top-level Open MPI installation directory
ifeq ($(PROC),PAR)

  # Open MPI C++ compiler
  CXX := mpic++

  # Necessary flags for parallel compilation
  CPPFLAGS += -I$(MPIHOME)/include/ -DMPI_ENABLED
endif


.PHONY:

libclean:
	@echo -e "\nCleaning up the ASIMExample libraries ...\n"
	@rm lib/* -f
