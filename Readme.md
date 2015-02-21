![AIMS Logo](ref/doc/figures/AIMSLogo_BoldPastelColors.png "Accelerator-based In-situ Materials Surveillance")  
**A**ccelerator-based **I**n-situ **M**aterials **S**urveillance

## The ADAQ Libraries ##

The ADAQ (AIMS Data AcQuisition) libraries provide a powerful set of
C++ and Python tools for comprehensive data acquisition and analysis
of experimental and simulated particle detector data. The
'ADAQControl' library (C++, Python) forms a self-contained software
layer that manages the interface between CAEN data acquisition
hardware and the user's PC by automatically incorporating the
necessary CAEN libraries (CAENComm, CAENDigitizer, and CAENVME) and
then adding nn enhanced layer of functionality on top to provide more
powerful and straightforward control of the CAEN hardware. The
'ADAQReadout' library provides standardized readout of all critical
information (DAQ settings, digitized waveforms, analyzed waveform
data, etc) into highly compressed binary 'ADAQ' files. The
'ASIMReadout' library provides standardized readout of critical
simulated detector quantities into highly compressed binary 'ASIM'
files when used in C++-compatible Monte Carlo particle transport codes
like Geant4. Both libraries are built ontop of the [ROOT data analysis
framework](https://root.cern.ch/drupal/). The ADAQ and ASIM files can
be analyzed offline using the GUI-based
[ADAQAnalysis](https://github.com/zach-hartwig/ADAQAcquisition)
program. Finally, a set of classes are provided for use of ADAQ
projects that incorporate ROOT.

Thus, the ADAQ framework can be envisioned as follows:

```
                                       ----------------
                                       | ROOT classes | : Classes for integrating ROOT
                                       ----------------     into ADAQ projects
                                               |
               ADAQControl,ADAQReadout         |        
                  ---------------              |        
------------      | CAEN Libs   |     ------------------
|   CAEN   |  ->  |     &       | ->  |  Acquisition & | : User applications built 
| Hardware |  ->  |  Enhanced   | ->  |    Analysis    |     with the ADAQ framework
------------      | Interfaces  |     ------------------
                  ---------------              |
                                               |
                                               |
                                    -----------------------	
                                    | Detector simulation | : Readout management for
                                    -----------------------     simulated detector data
                                         ASIMReadout	
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
use of **ADAQ**, you implicitly agree to absolve the author(s) of any
liability whatsoever. The reader is encouraged to consult the **ADAQ**
User's Guide and is advised that the use of this source code is at his
or her own risk.

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

3. [ROOT](https://root.cern.ch/drupal/)



### Obtaining and building the code ###

First, a word on versioning of the ADAQ libraries. Git tags of the
form X.Y.Z are used to indicate stable, production versions of the
ADAQ libraries that may be deployed with confidence for general use by
the general user. A change in the X version number indicates a major
release that departs substantially from the previous series, while a
change in the Y version number indicates deployment of major new code
features. A change in the Z number is used to indicate bug fixes and
very minor changes to the codebase. Untagged commits are considered
development versions of the code with no guarantee of stability and
should only be used by developers in non-production situations.

To obtain the ADAQ libraries, you'll need to first clone the
repository from GitHub and then configure your environment via the
'setup.sh' script. Once complete, you can then switch to a tagged
production version to build the code. At present, you'll need to build
each ADAQ library individually using the GNU makefiles provided in
each library's source directory to build and install the libraries
into the $ADAQREPO/include (for headers) and $ADAQREPO/lib/<arch>
(for libraries, where '<arch>' is either i686 (32-bit) or x86_64
(64-bit)).

First clone the repository:
```bash
git clone https://github.com/zach-hartwig/ADAQ.git
```
Then add the following line to your .bashrc file:
```bash
source /full/path/to/ADAQ/scripts/setup.sh >& /dev/null
```
Remember to open a new terminal for the settings to take effect!
Finally, switch to a tagged production branch and build the
libraries. In this example, we'll use the 1.0.0-beta version:

 ```bash
cd /full/path/to/ADAQ/source
git checkout -b 1.0.0-beta

# Build/install each desired library
cd ADAQControl; make; make install
cd ../ADAQReadout; make; make install
cd ../ASIMReadout; make; make install
```

If you want to remove the build files and local library then - from
within each library's directory - type:
```bash
   make clean
```
Note that this will only remove local files and *not* the installed
libraries, which live in the $ADAQREPO/lib/<arch>/ directory


### Directory overview ###

The following contains an overview of the ADAQ directories:

#### The ADAQ source directory ####

Each ADAQ library lives in its own subdirectory of the
$ADAQREPO/source directory. At present, each library must be
built/installed separately rather by a single large GNU
makefile. While this may change in the future, this enables the user
to independently build the ADAQControl library without installing ROOT
on the system since ROOT is *not* an ADAQControl dependency; the
ADAQReadout and ASIMReadout libraries *require* a ROOT installation.

The ADAQControl library serves three main functions: first, to
automically provide the user with all necessary and up-to-date CAEN
libraries and headers. No need to go downloading and installing them
anymore! The CAEN libraries and headers are located *locally* within
the $ADAQREPO directory, and, therefore, will not interfere with
other global installations of CAEN headers and libraries.  The second
function is to wrap all CAEN library functions with a more intuitive
and consistent function style; third and finally, to build upon the
CAEN library methods and provide a set of powerful enhanced methods
for programming, operating, and readout of CAEN data acquisition
hardware. Classes are provided the logically map onto the supported
pieces of CAEN hardware (*e.g.* the ADAQDigitizer class provides an
interface to support CAEN desktop and VME digitizers). Additionally,
the ADAQPythonWrapper class uses Boost.Python to wrap all C++ classes
such that the user has the additional option of seamless Python
interface without the need to rewrite any code. 

The ADAQReadout and ASIMReadout library provides a streamlined method
to readout expeirment DAQ settings and digitizer data and simulated
detector data, respectively, into highly compressed, binary ROOT files
that have a standardized format. This provides the ability to easily
store digitized data along with the acquisition settings for offline
analysis, using the closely associated
[ADAQAnalysis](https://github.com/zach-hartwig/ADAQAnalysis] program,
on the ROOT command line, or in an analysis program of the user's own
design. One of the main purposes in providing these classes is to
facilitate the often difficult task of folding ROOT into the user's
own experimental and simulation codes.


#### CAEN directory ####

The $ADAQREPO/ref/caen directory contains an assortment of useful CAEN
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

At present, the following, most up-to-date CAEN libraries have been
tested succesfully used with the ADAQ libraries:
 - CAENComm-1.2.0 (Dec 2013)
 - CAENVMELib-2.41.0 (Mar 2013)
 - CAENDigitizer-2.6.0 (Sept 2014)

#### Directory structure ####

 - **include/** : The main directory for all ADAQ and CAEN header files

 - **lib/** : The main library directory for the ADAQ and CAEN libraries  
   - **i686/** : 32-bit libraries
   - **x86_64** : 64-bit libraries

 - **ref/** : Useful references, archival material, and documentation
    - **caen/** : CAEN drivers, firmware, and software used as part of ADAQ
        - **drivers/** : Linux drivers for the VME-USB connection
        - **firmware/** : firmware for the V1718, V1720 VME boards
        - **libraries/** : contains CAENComm, CAENDigitizer, and CAENVME libraries
        - **manuals/** : presently useful CAEN manuals
        - **notes/** : useful notes
        - **software/** : CAEN-distributed binary software (e.g. CAENUpgrader tool)

    - **doc/** : Future home of ADAQ manual

 - **source/** : ADAQ source code
   - **ADAQControl/** : C++/Python interface to CAEN DAQ hardware
   - **ADAQReadout/** : C++ readout of CAEN DAQ settings and digitized data to ADAQ-formatted ROOT 
   - **ASIMReadout/** : C++ readout of simulated detector data to ASIM-formatted ROOT files
   - **root/** : Classes for use with ADAQ projects utilizing ROOT

 - **scripts/** : Bash scripts for installation and setup of ADAQ 

 - **templates/** : Minimal code packages to aid user's development projects

### Contact ###

Zach Hartwig

[Dept. of Nuclear Science and
Engineering](http://web.mit.edu/nse/http://web.mit.edu/nse/) &  
[Plasma Science and Fusion Center](http://www.psfc.mit.edu)  
[Massachusetts Institute of Technology](http://mit.edu)  

phone: +1 617 253 5471  
email: [hartwig@psfc.mit.edu](mailto:hartwig@psfc.mit.edu)  
smail: 77 Massachusetts Ave, NW17-115, Cambridge MA 02139, USA
