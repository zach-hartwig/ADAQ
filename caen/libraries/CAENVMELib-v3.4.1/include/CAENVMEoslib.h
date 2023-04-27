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

#define CAENVME_DEPRECATED_MSG(V, R)		"Deprecated since " #V ". Use " #R

#ifdef _WIN32

#define CAENVME_DLLAPI
#define CAENVME_API		__stdcall
#define CAENVME_DEPRECATED(V, R, ...)	__declspec(deprecated(CAENVME_DEPRECATED_MSG(V, R))) __VA_ARGS__

#else   // _WIN32

#define CAENVME_DLLAPI	__attribute__((visibility("default")))
#define CAENVME_API
#define CAENVME_DEPRECATED(V, R, ...)	__VA_ARGS__ __attribute__((deprecated(CAENVME_DEPRECATED_MSG(V, R))))

#endif  // _WIN32

#endif  // __CAENVMEOSLIB_H
