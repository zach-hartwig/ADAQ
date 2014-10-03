/*
        ----------------------------------------------------------------------

        --- CAEN SpA - Computing Systems Division ---

        CAENUSBdrvB.h

        Header file for the CAEN USb driver B

        June  2004 :   Created.

        ----------------------------------------------------------------------
*/
#ifndef _CAENUSBdrvB_H
#define _CAENUSBdrvB_H

#ifndef VERSION
	#define VERSION(ver,rel,seq) (((ver)<<16) | ((rel)<<8) | (seq))
#endif	


/*
        Defines for the v1718
*/

#define V1718_MAGIC                     'U'

#define V1718_IOCTL_REV                 _IOWR(V1718_MAGIC, 1, v1718_rev_t)
#define V1718_IOCTL_REBOOTB             _IOWR(V1718_MAGIC, 2, v1718_rev_t)
#define V1718_IOCTL_REBOOTF             _IOWR(V1718_MAGIC, 3, v1718_rev_t)


/*
        ----------------------------------------------------------------------

        Types

        ----------------------------------------------------------------------
*/

// Rev 0.2
/*
	Struct for revision argument in ioctl calls
*/
#define V1718_DRIVER_VERSION_LEN	20
typedef struct v1718_rev_t {
        char 		rev_buf[V1718_DRIVER_VERSION_LEN];
} v1718_rev_t;


#endif
