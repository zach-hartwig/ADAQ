#!/bin/bash
#
# name: BuildLibraries.sh
# date: 29 Apr 15
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
# desc: A bash script to build and install all ADAQ libraries. This is
#       not a great build system, but it will serve present needs

# Get the number of processors available on system
NPROC=$(nproc)

# Set the current directory
DIR=$PWD

# List all of the libraries to build/install
declare -a LIBS=("ADAQControl" "ADAQReadout" "ASIM")

# Loop over the list and do the work
for lib in "${LIBS[@]}"
do
    if [ "$lib" = "ASIM" ]; then
	if [ -z "$G4INSTALL" ]; then
	    echo -e "\n--------------------------------------------------------"
	    echo -e "\nWARNING: Geant4 does not appear to exist on this system!"
	    echo -e   "         The ASIM libraries will not be built.\n"
	    echo -e   "--------------------------------------------------------\n"
	    continue
	fi
    fi

    cd $DIR/$lib
    make clean
    make -e -j$NPROC
    make install
done
