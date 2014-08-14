////////////////////////////////////////////////////////////////////////////////
//
// name: RootLinkDef.h
// date: 14 Aug 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
//
// desc: The standard Root header file required to build dictionaries
//       for custom user classes to be used with ROOT. Utilized by
//       'rootcint' dictionary generator in the GNU makefile
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class AcquisitionManager+;

#endif
