#!/bin/bash
#
# name: BuildLibraries.sh
# date: 22 Dec 15
# auth: Zach Hartwig
# mail: hartwig@psfc.mit.edu
# desc: A bash script to build and install all ADAQ libraries. This is
#       not a great build system, but it will serve present needs.
#
# 2run: To build all three libarries (ASIM library in sequential)
#         $ ./BuildLibraries.sh seg
#
#       To build all three libraries (ASIM library in parallel)
#         $ ./BuildLibraries.sh par


if [ $# -ne 1 ]; then
    echo -e "\nError! Script only accepts a single cmd line argument!"
    echo -e   "       <arg0> : seq == sequential ASIM library"
    echo -e   "              : par == parallel ASIM library\n"
    exit
fi

# Get the number of processors available on system
NPROC=$(nproc)

# Set the current directory
DIR=$PWD

# List all of the libraries to build/install
declare -a LIBS=("ADAQControl" "ADAQReadout" "ASIM")

# Loop over the list and do the work
for lib in "${LIBS[@]}"
do

    # Set the default (sequential) build arguments
    BUILD="-e -j$NPROC"

    # Override is ASIM should be built in parallel
    if [ "$lib" = "ASIM" ]; then
	if [ "$1" = "par" ]; then
	    BUILD="par"
	fi
	
	if [ -z "$G4INSTALL" ]; then
	    echo -e "\n--------------------------------------------------------"
	    echo -e "\nWARNING: Geant4 does not appear to exist on this system!"
	    echo -e   "         The ASIMReadout library will not be built.\n"
	    echo -e   "--------------------------------------------------------\n"
	fi
    fi
    
    cd $DIR/$lib
    make clean
    make $BUILD
    make install
done
