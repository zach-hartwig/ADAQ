![AIMS Logo](doc/figures/AIMSLogo_BoldPastelColors.png "Accelerator-based In-situ Materials Surveillance")  
**A**ccelerator-based **I**n-situ **M**aterials **S**urveillance

## The ADAQ Libraries ##

The ADAQ (AIMS Data AcQuisition) libraries form a self-contained
software layer that manages the interface between the CAEN data
acquisition hardware and the ADAQ acquisition and analysis codes. It
automatically provides the necessary CAEN libraries (CAENComm,
CAENDigitizer, CAENVME) and then adds an enhanced layer of
functionality on top to provide more powerful and straightforward
control of the CAEN hardware. ADAQ provides users with C, C++, and
Python interfaces for controlling the CAEN hardware. Additionally,
ADAQ provides a set of custom ROOT and Geant4 so that the user may
interface ROOT analysis projects and Geant4 particle transport
simulations to the ADAQ framework. The ADAQ framework can be
envisioned as follows:

```
                                                          ----------------
                                                          | ROOT classes | : Classes for integrating ROOT with ADAQ projects
                                                          ----------------
                                                                 |
                             ADAQ Libraries                      |        
                       -------------------------           User applications
-----------------      |    CAEN libraries     |      --------------------------
| CAEN HARDWARE |  ->  |          &            |  ->  | Acquisition / Analysis | : Applications built with the ADAQ framework
-----------------      |  Enhanced interfaces  |      --------------------------
                       -------------------------                 |
                                                                 |
                                                          ------------------	
                                                          | Geant4 classes | : Classes for readout of detector simulation data
                                                          ------------------	
```

### License and disclaimer ###

The **ADAQ** libraries are licensed under the GNU General Public
License v3.0.  You have the right to modify and/or redistribute this
source code under the terms specified in the license,

The **ADAQ** libraries are provided *without any warranty nor
guarantee of fitness for any particular purpose*. The author(s) shall
have no liability for direct, indirect, or other undesirable
consequences of any character that may result from the use of this
source code. This may include - but is not limited - to irrevocable
changes to the user's firmware, software, hardware, or data. By your
use of **ADAQAcquisition**, you implicitly agree to absolve the
author(s) of any liability whatsoever. The reader is encouraged to
consult the **ADAQAcquisition** User's Guide and is advised that the
use of this source code is at his or her own risk.

The **CAEN** libraries - which are distributed (a) for the user's
convenience and (b) to ensure version compatibility - are also
licensed under the GNU General Public License v3.0 by CAEN S.p.A.

A copy of the GNU General Public License v3.0 may be found within this
repository at $ADAQ/License.md or is available online at
http://www.gnu.org/licenses.



### Code dependencies ###

The ADAQ libraries depend on the following external codes and
libraries:

1. [GNU make](http://www.gnu.org/software/make/)

2. [Boost](http://www.boost.org/)



### Build instructions ###

The following lines should first be added to your .bashrc file to such
that the ADAQ setup script can configure your environment correctly:

```bash
export ADAQ_HOME=/full/path/to/ADAQ
source $ADAQ_HOME/scripts/setup.sh dev >& /dev/null
```

On Linux or MacOS, clone into the repository and then use the provided
GNU makefile to build and install the ADAQ libraries:

```bash
   # Clone the ADAQ source code
   git clone https://github.com/zach-hartwig/ADAQ.git

   # Move to the ADAQ library source code
   cd ADAQ/adaq

   # Build the libraries from source
   make

   # Install libraries locally within ADAQ directory
   make install

   # If desired to cleanup all build files and libraries:
   make clean
```


### Directory overview ###

The following contains an overview of the ADAQ directories:

#### ADAQ directory ####

The ADAQ libraries serve two main functions: first, to wrap all
CAENDigitizer library functions with a more intuitive and consistent
function style; second to provide a set of powerful enhanced methods
for programming, operating, and readout of CAEN data acquisition
hardware. Classes are provided the logically map onto the supported
pieces of CAEN hardware (*e.g.* the ADAQDigitizer class provides an
interface to support CAEN desktop and VME digitizers). Additionally,
the ADAQPythonWrapper class uses Boost.Python to wrap all C++ classes
such that the user has the additional option of seamless Python
interface without the need to rewrite any code.

All of the above is compiled into two shared object libraries :
"libADAQ.so" provides the C++ interface methods while "libPyDAQ.so"
provides the Python inteface methods. A GNU makefile control the
library building. 

#### CAEN directory ####

The $ADAQ/caen directory contains an assortment of useful CAEN
drivers, firmware, and software that are - in most cases - required to
work with the ADAQ libraries and - in a few cases - provided simply
for convenience. The purpose of including the CAEN software libraries
is twofold. First, by having the software libraries within the ADAQ
repository, the user does **not** need to install the CAEN libraries
and header files on his/her system since they can be built
again/linked to in the user's application! This provides convenience
(no installation necessary), consistency (since CAEN coding and naming
conventions are all over the place), rapid portability between
machines, aand flexibility to easily use different libraries/header
files. Second, this ensures that ADAQ is always associated with
specific version of the CAEN software libraries for compatibility and
consistency.

At present, the following CAEN libraries are used with ADAQ:  
 - CAENComm-1.2.0 (Dec 2013)
 - CAENVMELib-2.41.0 (Mar 2013)
 - CAENDigitizer-2.6.0 (Sept 2014)

#### Directory structure ####

 - **adaq/** : The ADAQ library source code

 - **caen/** : CAEN drivers, firmware, and software used as part of ADAQ
    - **libs/** : up-to-date libraries (CAEMComm, CAENDigitizer, CAENVMELib)
    - **include/** : up-to-date headers files for above libraries
    - **ref/** : includes all original CAEN sources, manual, notes, etc
        - **drivers/** : Linux drivers for the VME-USB connection
        - **firmware/** : firmware for the V1718, V1720 VME boards
        - **libraries/** : contains CAENComm, CAENDigitizer, and CAENVME libraries
	- **manuals/** : presently useful CAEN manuals
	- **notes/** : useful notes
	- **software/** : CAEN-distributed binary software (e.g. CAENUpgrader tool)

 - **lib/** : The main library directory for the ADAQ and CAEN libraries;
    - **i686/** : 32-bit libraries
    - **x86_64** : 64-bit libraries

 - **include/** : The main directory for ADAQ and CAEN header files

 - **root/** : ROOT classes for general data storage and graphical user interfaces.

 - **scripts/** : Bash scripts for installation and setup of ADAQ 

 - **templates/** : Minimal code packages to aid user's developement projects

### Contact ###

Zach Hartwig

[Dept. of Nuclear Science and
Engineering](http://web.mit.edu/nse/http://web.mit.edu/nse/) &  
[Plasma Science and Fusion Center](http://www.psfc.mit.edu)  
[Massachusetts Institute of Technology](http://mit.edu)  

phone: +1 617 253 5471  
email: [hartwig@psfc.mit.edu](mailto:hartwig@psfc.mit.edu)  
smail: 77 Massachusetts Ave, NW17-115, Cambridge MA 02139, USA
