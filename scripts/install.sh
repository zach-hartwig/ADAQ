#!/bin/bash
#
# name: install.sh
# date: 03 Feb 14
# auth: Zach Hartwig
#
# desc: Bash script for installing the ADAQ libraries and headers into
#       the publicly accessible directory /usr/local/adaq. This is
#       done in order to make ADAQ accessible system-wide for
#       non-developers
#
# 2run: sudo -E ./install.sh 

if [ ! "$#" == "0" ]; then
    echo -e "\nInstall script error! No cmd line args are presently allowed!"
    echo -e   "   arg1 == ADAQAnalysis version number"
    echo -e   "   example: ./install 1.3.2"
    exit
fi

INSTALLDIR=/usr/local/adaq

if [ ! -d $INSTALLDIR ]; then 
    mkdir $INSTALLDIR
    mkdir $INSTALLDIR/include
    mkdir $INSTALLDIR/lib
fi

cp -r $ADAQHOME/include/* $INSTALLDIR/include/
cp -r $ADAQHOME/lib/$HOSTTYPE $INSTALLDIR/lib/
cp $ADAQHOME/scripts/setup.sh $INSTALLDIR

echo -e ""
echo -e "****************************************************"
echo -e "**   ADAQ headers and libraries are installed in:"
echo -e "**        $INSTALLDIR"
echo -e "****************************************************\n"
