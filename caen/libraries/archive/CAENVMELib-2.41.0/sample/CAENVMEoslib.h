/*
        -----------------------------------------------------------------------------

                --- CAEN SpA - Computing Systems Division ---

        -----------------------------------------------------------------------------

        CAENVMEoslib.h

        -----------------------------------------------------------------------------

        Author:  Stefano Coluccini (s.coluccini@caen.it)

        Created: January 2004

        -----------------------------------------------------------------------------
*/


#ifndef __CAENVMEOSLIB_H
#define __CAENVMEOSLIB_H

#ifdef WIN32

#include <windows.h>
#include <winioctl.h>

#define CAENVME_API CVErrorCodes __stdcall

#else   // WIN32

#define CAENVME_API CVErrorCodes

#endif  // WIN32

#endif  // __CAENVMEOSLIB_H
