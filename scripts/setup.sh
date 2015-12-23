#!/bin/bash
#
# name: setup.sh
# date: 01 Feb 15
# auth: Zach Hartwig
# desc: Bash setup script to automatically configure the user's
#       environment to use the ADAQ libraries

# The HOSTTYPE variable is used to determine whether 32 or 64 bit for
# ADAQ and CAEN libraries should be used
export HOSTTYPE=$(uname -m)

if [ ! "$#" -eq 0 ]; then
    echo -e "\nADAQ : Error! The ADAQ libraries' setup script accepts zero arguments!"
    echo -e   "       Please rerun this setup script!\n"
else

    if [ -z "$ROOTSYS" ]; then
	echo -e "ADAQ : Note that while the ADAQControl library does not require ROOT, the ADAQReadout and"
	echo -e "       ASIMReadout libaries to require ROOT. It is strongly suggested that you install ROOT!\n"
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
    echo -e   "       --> ADAQHOME = "$ADAQHOME
    echo -e   "       --> The libraries are now included in PATH, LD_LIBRARY_PATH, PYTHONPATH\n"
fi
