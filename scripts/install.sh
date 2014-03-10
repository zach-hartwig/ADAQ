#!/bin/bash
#
# name: install.sh
# date: 10 Mar 14
# auth: Zach Hartwig
#
# desc: Bash script for installing the ADAQ libraries and headers into
#       the publicly accessible directory /usr/local/adaq. This is
#       done in order to make ADAQ accessible system-wide for
#       non-developers to use ADAQ in their own projects
#
# 2run: sudo -E ./install.sh 

if [ ! "$#" == "1" ]; then
    echo -e "\nInstall script error! Only a single cmd line arg is allowed from the user:"
    echo -e   "   arg1 == ADAQAnalysis version number"
    echo -e   "   example: ./install 1.3.2\n"
    exit
fi

VERSION=$1

# Check to ensure that the desired version exists
AVAILABLE=$(git tag)
if [[ $AVAILABLE != *$VERSION* ]]; then
    echo -e "\nInstall script error! The available versions are:"
    echo -e   "--> $AVAILABLE\n"
    exit
fi

# Set the installation location
INSTALLDIR=/usr/local/adaq

# Get the top-level ADAQ directory and move there
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TOPDIR=${SCRIPTDIR///scripts/}
cd $TOPDIR

# If the ADAQ directory destination dirs don't exist then create them
if [ ! -d $INSTALLDIR ]; then sudo mkdir $INSTALLDIR; fi
if [ ! -d $INSTALLDIR/include ]; then sudo mkdir $INSTALLDIR/include; fi
if [ ! -d $INSTALLDIR/lib ]; then sudo mkdir $INSTALLDIR/lib; fi

echo -e "\nChecking out tagged version from the Git repository ..."
git co $VERSION -b $VERSION >& /dev/null

echo -e "\nBuilding the ADAQ libraries ..."
cd $TOPDIR/adaq
make clean >& /dev/null
make -j2 >& /dev/null

echo -e "\nCopying files to $INSTALLDIR ..."
sudo cp -r $ADAQHOME/include/* $INSTALLDIR/include/
sudo cp -r $ADAQHOME/lib/$HOSTTYPE $INSTALLDIR/lib/
sudo cp $ADAQHOME/scripts/setup.sh $INSTALLDIR

echo -e "\nRemoving the temporary tag branch ..."
git stash >& /dev/null
git co master >& /dev/null
git br -d $VERSION >& /dev/null

echo -e "\n***************************************************"
echo -e   "**   The ADAQ libraries have been installed in:  **"
echo -e   "**               $INSTALLDIR                 **"
echo -e   "***************************************************\n"
