#!/bin/bash
#
# name: setup.sh
# date: 02 Feb 14
# auth: Zach Hartwig
#
# desc: Setup Bash script for the ADAQ libraries. The script sets up
#       access for two different types of users: 'users' will have the
#       libraries and headers from /usr/local/adaq; 'developers'
#       require path settings relative to their top-level Git checkout
#       directory of the ADAQ source code. The script should be
#       sourced from the user's bashrc file with the single argument
#       'usr' or 'dev' as appropriate.

# The HOSTTYPE variable is used to determine whether 32 or 64 bit for
# ADAQ and CAEN libraries should be used
export HOSTTYPE=$(uname -m)

if [ ! "$#" -eq 1 ]; then
    echo -e "\nError! The ADAQ setup.sh script only accepts single cmd line arg!"
    echo -e   "       'usr' to setup environment for installed libraries and headers in /usr/local/adaq"
    echo -e   "       'dev' to setup developer environment in local Git checkout directory\n"
fi


if [ "$1" ==  'usr' ]; then
    export ADAQUSER=user
    export PATH=/usr/local/adaq:$PATH
    export LD_LIBRARY_PATH=/usr/local/adaq/lib/$HOSTTYPE:$LD_LIBRARY_PATH

    echo -e "ADAQ : Environment has been configured for ADAQ user!"

elif [ "$1" == 'dev' ]; then
    export ADAQUSER=developer

    SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    export ADAQHOME=${SCRIPTDIR///scripts/}
    export PATH=$ADAQHOME/bin:$PATH
    export LD_LIBRARY_PATH=$ADAQHOME/lib/$HOSTTYPE:$LD_LIBRARY_PATH
    export PYTHONPATH=$ADAQHOME/lib/$HOSTTYPE:$PYTHONPATH

    echo -e "ADAQ : Environment has been configured for ADAQ developer!"
fi
