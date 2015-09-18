/////////////////////////////////////////////////////////////////////////////////
// 
// name: RootLinkDef.h
// date: 11 Aug 14
// auth: Zach Hartwig
// mail: hartwig@psfc.mit.edu
// 
// desc: RootLinkDef is the mandatory header file required by rootcint
//       to generate dictionaries for custom C++ classes used in
//       ADAQAnalysis. Note that it's suffix is ".h" rather than ".hh"
//       such that the Makefile does not include it when it wildcards
//       for the other header files; this is done since RootLinkDef.h
//       must be the final header file included to rootcint.
//
/////////////////////////////////////////////////////////////////////////////////

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class ASIMEvent+;
#pragma link C++ class ASIMRun+;
#pragma link C++ class ASIMStorageManager+;

#endif
