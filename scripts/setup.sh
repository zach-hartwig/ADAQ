#!/bin/bash
#
# name: setup.sh
# date: 27 Nov 21
# auth: Zach Hartwig
# desc: Bash setup script to automatically configure the user's
#       environment to use the ADAQ libraries

# The HOSTTYPE variable is used to determine whether 32 or 64 bit for
# ADAQ and CAEN libraries should be used
export HOSTTYPE=$(uname -m)

# Check to ensure that ROOT is installed on the host system (a
# non-null return value) and get the version number
ROOTVER=$(root-config --version)

if [ ! "$#" -eq 0 ]; then
    echo -e "\nADAQ : Error! The ADAQ libraries setup script accepts zero arguments!"
    echo -e   "       Please rerun this setup script!\n"
else

    if [ -z "$ROOTVER" ]; then
	echo -e "ADAQ : The ADAQControl library does not require ROOT; however, the ADAQReadout and"
	echo -e "       ASIMReadout libaries do. You are strongly recommended to install ROOT!\n"
    else
	echo -e "ADAQ : ROOT version" $ROOTVER "has been detected on this system!"
    fi
    
    # Automatically determine the absolute path to top-level ADAQ
    # directory using this pro'n'shit method
    SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

    # Export necessary environmental variable values
    export ADAQHOME=${SCRIPTDIR///scripts/}
    export PATH=$ADAQHOME/bin:$PATH
    export LD_LIBRARY_PATH=$ADAQHOME/lib/$HOSTTYPE:$LD_LIBRARY_PATH
    export ROOT_INCLUDE_PATH=$ADAQHOME/include:$ROOT_INCLUDE_PATH
    export PYTHONPATH=$ADAQHOME/lib/$HOSTTYPE:$PYTHONPATH
    
    echo -e "\nADAQ : The environment has been successfully configured!"
    echo -e   "       --> Home directory set to ADAQHOME="$ADAQHOME
    echo -e   "       --> Binary directory appended to PATH"
    echo -e   "       --> Library directory appended to LD_LIBRARY_PATH and PYTHONPATH\n"
fi
