
/*
 * CAEN USB driver - 1.5.4
 *
 * Written by CAEN SpA (support.computing@caen.it) - CAEN SpA
 *
 * based on rio500.c by Cesar Miquel (miquel@df.uba.ar)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#undef DEBUG  

#ifndef VERSION
        #define VERSION(ver,rel,seq) (((ver)<<16) | ((rel)<<8) | (seq))
#endif
 
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE >= VERSION(4,11,0)
#include <linux/sched/signal.h>
#else
#include <linux/signal.h>
#endif
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/random.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/usb.h>
#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
#endif

/*
 * Version Information
 */
#define DRIVER_VERSION "1.5.4"
#define DRIVER_AUTHOR "CAEN Computing Division <support.computing@caen.it>"
#define DRIVER_DESC "CAEN USB driver"

#define V1718_MINOR_BASE   178

/* stall/wait timeout for V1718 */
#define NAK_TIMEOUT (HZ)

#define IBUF_SIZE 0x10000	/* 64 kbytes */

/* Size of the V1718 buffer */
#define OBUF_SIZE 0x10000	/* 64 kbytes */

/* we can have up to this number of devices plugged in at once */
#define MAX_DEVICES	16
//

#if LINUX_VERSION_CODE >= VERSION(2,5,0)
	#define USB_ST_NOERROR 0
	#define USB_ST_CRC (-EILSEQ)
	#define USB_ST_BITSTUFF (-EPROTO)
	#define USB_ST_DATAOVERRUN (-EOVERFLOW)
	#define USB_ST_DATAUNDERRUN		(-EREMOTEIO)
	#define USB_ST_BUFFEROVERRUN (-ECOMM)
	#define USB_ST_BUFFERUNDERRUN (-ENOSR)
	#define USB_ST_INTERNALERROR (-EPROTO) /* unknown error */
	#define USB_ST_SHORT_PACKET (-EREMOTEIO)
	#define USB_ST_PARTIAL_ERROR (-EXDEV) /* ISO transfer only parti ally completed */
	#define USB_ST_URB_KILLED (-ENOENT) /* URB canceled by user */
	#define USB_ST_URB_PENDING (-EINPROGRESS)
	#define USB_ST_REMOVED (-ENODEV) /* device not existing or removed */
	#define USB_ST_TIMEOUT (-ETIMEDOUT) /* communication timed out, also in urb->status**/
	#define USB_ST_NOTSUPPORTED (-ENOSYS)
	#define USB_ST_BANDWIDTH_ERROR (-ENOSPC) /* too much bandwidth used */
	#define USB_ST_URB_INVALID_ERROR (-EINVAL) /* invalid value/transfer type */
	#define USB_ST_URB_REQUEST_ERROR (-ENXIO) /* invalid endpoint */
	#define USB_ST_STALL (-EPIPE) /* pipe stalled, also in urb->status*/
#else
    #error ######################################################
    #error ######################################################
    #error THIS DRIVER NOT IS COMPATIBLE WITH YOUR KERNEL VERSION
    #error ######################################################
    #error ######################################################
#endif

// Rev. 0.4
#if LINUX_VERSION_CODE >= VERSION(2,6,11)
        #include <linux/mutex.h>		// for unlocked ioctl interface
#endif

#if defined(CONFIG_KERNEL_LOCK) || LINUX_VERSION_CODE < VERSION(2,6,38)
#include <linux/smp_lock.h>
#endif

#include "CAENUSBdrvB.h"
//
// Rev 0.2: added end

struct v1718_usb_data {
    struct usb_device *v1718_dev;   /* init: probe_v1718 */	
    unsigned int ifnum;             /* Interface number of the USB device */
    int open_count;                 /* number of users */
    int present;                    /* Device is present on the bus */
    int minor;                      /* minor number for the device */
    char *obuf, *ibuf;              /* transfer buffers */
    char bulk_in_ep, bulk_out_ep;   /* Endpoint assignments */
    wait_queue_head_t wait_q;       /* for timeouts */
    struct semaphore lock;          /* general race avoidance */
// Rev 0.4
#if LINUX_VERSION_CODE >= VERSION(2,6,11)
    struct mutex           ioctl_lock;
#endif
};

/* array of pointers to our devices that are currently connected */
static struct v1718_usb_data *minor_table[MAX_DEVICES];

/* lock to protect the minor_table structure */
#if LINUX_VERSION_CODE <= VERSION(2,6,35)
static DECLARE_MUTEX (minor_table_mutex);
#else
static DEFINE_SEMAPHORE (minor_table_mutex);
#endif

static void v1718_delete(struct v1718_usb_data *v1718)
{
#ifndef CONFIG_USB_DYNAMIC_MINORS
	minor_table[v1718->minor] = NULL;
#else
	int i = 0;
	for (i = 0; i < MAX_DEVICES; i++) { 
		if (minor_table[i] != NULL) {
			if (v1718->minor == minor_table[i]->minor) break;
		}
	}
	if (i < MAX_DEVICES) minor_table[i] = NULL;
#endif
	if( v1718->ibuf != NULL )
		kfree(v1718->ibuf);
	if( v1718->obuf != NULL )
		kfree(v1718->obuf);
	kfree(v1718);
}


#if LINUX_VERSION_CODE >= VERSION(2,6,11)
static long unlocked_ioctl_v1718(struct file *, unsigned int, unsigned long);
#endif

static int open_v1718(struct inode *inode, struct file *file)
{
	struct v1718_usb_data *v1718 = NULL;
	int subminor;

#ifndef CONFIG_USB_DYNAMIC_MINORS
	subminor = MINOR(inode->i_rdev) - V1718_MINOR_BASE;
#else
	for (subminor=0;  subminor<MAX_DEVICES; subminor++) if (MINOR(inode->i_rdev) == minor_table[subminor]->minor) break;
#endif

	if( (subminor < 0) || (subminor > MAX_DEVICES) )
		return -ENODEV;

	down(&minor_table_mutex);

	v1718 = minor_table[subminor];
	if( v1718 == NULL ) {
		up(&minor_table_mutex);
		return -ENODEV;
	}

	down(&v1718->lock);

	up(&minor_table_mutex);

	v1718->open_count++;

	init_waitqueue_head(&v1718->wait_q);


#if LINUX_VERSION_CODE >= VERSION(2,6,11)
        mutex_init(&v1718->ioctl_lock);
#endif
	file->private_data = v1718;
	up(&v1718->lock);

	return 0;
}

static int close_v1718(struct inode *inode, struct file *file)
{
	struct v1718_usb_data *v1718 = (struct v1718_usb_data *)file->private_data;
	int retval = 0;
	if( v1718 == NULL ) {
		printk("Close: object is null\n");
		return -ENODEV;
	}
	down(&minor_table_mutex);
	down(&v1718->lock);

	if( v1718->open_count <= 0 ) {
		printk("Device not opened\n");
		retval = -ENODEV;
		goto exit_not_opened;
	}

	if( v1718->v1718_dev == NULL ) {
		/* the device was unplugged before the file was released */
		up(&v1718->lock);
		v1718_delete(v1718);
		up(&minor_table_mutex);
		return 0;
	}

	v1718->open_count--;

exit_not_opened:
	up(&(v1718->lock));
	up(&minor_table_mutex);

	return retval;
}

static int
ioctl_v1718(struct inode *inode, struct file *file, unsigned int cmd,
            unsigned long arg)
{


	int ret= 0;
	struct v1718_usb_data *v1718;
	
	v1718 = (struct v1718_usb_data *)file->private_data;
	down(&(v1718->lock));
    /* Sanity check to make sure v1718 is connected, powered, etc */
    if ( v1718 == NULL ||
         v1718->present == 0 ||
         v1718->v1718_dev == NULL )
	{
		up(&(v1718->lock));
		return -ENOTTY;
	}

    switch (cmd) {
        case V1718_IOCTL_REV:
			{
				v1718_rev_t rev;
                if( copy_from_user(&rev, (v1718_rev_t *)arg, sizeof(rev)) > 0 ) {
                        ret = -EFAULT;
                        break;
                }
				strcpy( rev.rev_buf, DRIVER_VERSION);
                if( copy_to_user((v1718_rev_t *)arg, &rev, sizeof(rev)) > 0) {
                        ret = -EFAULT;
                        break;
                }
			}
	        break;
		case V1718_IOCTL_REBOOTB:
			ret =	usb_control_msg(v1718->v1718_dev, usb_sndctrlpipe(v1718->v1718_dev,0),0xB0, 0x20,1, 0, NULL, 0, 1 * HZ);
			ret |= 	usb_control_msg(v1718->v1718_dev, usb_sndctrlpipe(v1718->v1718_dev,0),0xB0, 0x20,0, 0, NULL, 0, 1 * HZ);
	        break;
		case V1718_IOCTL_REBOOTF:
			ret =	usb_control_msg(v1718->v1718_dev, usb_sndctrlpipe(v1718->v1718_dev,0),0xB0, 0x20,3, 0, NULL, 0, 1 * HZ);
			ret |=	usb_control_msg(v1718->v1718_dev, usb_sndctrlpipe(v1718->v1718_dev,0),0xB0, 0x20,2, 0, NULL, 0, 1 * HZ);
	        break;
		default:
			ret= -ENOTTY;
			break;
    }
	up(&(v1718->lock));
    return ret;
}


/*
        ----------------------------------------------------------------------

        unlocked_ioctl_v1718 (Called in preference to ioctl_v1718 on newer kernels)

        ----------------------------------------------------------------------
*/

#if LINUX_VERSION_CODE >= VERSION(2,6,11)
static long unlocked_ioctl_v1718(struct file *file, unsigned int cmd, unsigned long arg)
{
#if LINUX_VERSION_CODE >= VERSION(2,6,18)	
	struct inode *inode = file->f_path.dentry->d_inode;
#else	
    struct inode *inode = file->f_dentry->d_inode;    
#endif	
    struct v1718_usb_data *s = (struct v1718_usb_data *)file->private_data;
	long ret;

	/* ioctl() calls can cause the Big Kernel Lock (BKL) to be taken, which
	 * can have significant performance penalties system-wide.  By providing
	 * an unlocked ioctl() method the BKL will not be taken, but the driver
	 * becomes responsible for its own locking.  Furthermore, the lock can be
	 * broken down per A2818 so that multiple threads accessing different CONET
	 * chains do not contend with one another during ioctl() calls.
	 */
	mutex_lock(&s->ioctl_lock);
	ret = (long) ioctl_v1718(inode, file, cmd, arg);
	mutex_unlock(&s->ioctl_lock);

	return ret;
}
#endif

static ssize_t
write_v1718(struct file *file, const char *buffer,
            size_t count, loff_t * ppos)
{
	struct v1718_usb_data *v1718;

	unsigned long copy_size;
	unsigned long bytes_written = 0;
	unsigned int partial;

	int result = 0;
	int maxretry;
	int errn = 0;

	v1718 = (struct v1718_usb_data *)file->private_data;

	down(&(v1718->lock));
        /* Sanity check to make sure v1718 is connected, powered, etc */
        if ( v1718 == NULL ||
             v1718->present == 0 ||
             v1718->v1718_dev == NULL )
	{
		up(&(v1718->lock));
		return -ENODEV;
	}

	do {
		unsigned long thistime;
		char *obuf = v1718->obuf;

		thistime = copy_size =
		    (count >= OBUF_SIZE) ? OBUF_SIZE : count;
		if (copy_from_user(v1718->obuf, buffer, copy_size)) {
			errn = -EFAULT;
			goto error;
		}
		maxretry = 1;   
		while (thistime) {
			if (!v1718->v1718_dev) {
				errn = -ENODEV;
				goto error;
			}
			if (signal_pending(current)) {
				up(&(v1718->lock));
				return bytes_written ? bytes_written : -EINTR;
			}

			result = usb_bulk_msg(v1718->v1718_dev,
					 usb_sndbulkpipe(v1718->v1718_dev, 2),
					 obuf, thistime, &partial, 1 * HZ);     // TEMP - 1 sec basta?


			if (result == USB_ST_TIMEOUT) {	
				if (!maxretry--) {
					errn = -ETIME;
					goto error;
				}

				wait_event_interruptible_timeout(v1718-> wait_q,(result == USB_ST_TIMEOUT),NAK_TIMEOUT);
				continue;
			} else if (!result && partial) {
				obuf += partial;
				thistime -= partial;
			} else
				break;
		};
		if (result) {
			printk("Write Whoops - %x\n", result);
			errn = -EIO;
			goto error;
		}
		bytes_written += copy_size;
		count -= copy_size;
		buffer += copy_size;
	} while (count > 0);

	up(&(v1718->lock));

	return bytes_written ? bytes_written : -EIO;

error:
	up(&(v1718->lock));
	return errn;
}

static ssize_t
read_v1718(struct file *file, char *buffer, size_t count, loff_t * ppos)
{
	struct v1718_usb_data *v1718;
	ssize_t read_count;
	unsigned int partial;
	int this_read;
	int result;
	int maxretry = 1;	
        char *ibuf;

	v1718 = (struct v1718_usb_data *)file->private_data;

	down(&(v1718->lock));
	/* Sanity check to make sure v1718 is connected, powered, etc */
        if ( v1718 == NULL ||
             v1718->present == 0 ||
             v1718->v1718_dev == NULL )
	{
		up(&(v1718->lock));
		return -ENODEV;
	}

	ibuf = v1718->ibuf;

	read_count = 0;

	while (count > 0) {

		if (!v1718->v1718_dev) {
			up(&(v1718->lock));
			return -ENODEV;
		}
		this_read = (count >= IBUF_SIZE) ? IBUF_SIZE : count;

		result = usb_bulk_msg(v1718->v1718_dev,
				      usb_rcvbulkpipe(v1718->v1718_dev, 6),
				      ibuf, this_read, &partial,
				      (int) (HZ * 1)); 

		if (partial) {
			count = this_read = partial;
		} else {
			switch( result){
			case USB_ST_BUFFERUNDERRUN:
				printk( "DBG read_v1718 USB_ST_BUFFERUNDERRUN ...\n" );
				usb_clear_halt( v1718->v1718_dev, 6);
				up(&(v1718->lock));
				return (0);
			case USB_ST_TIMEOUT:
			case 15: 
				if (!maxretry--) {
					up(&(v1718->lock));
					printk("read: maxretry timeout\n");
					return -ETIME;
				}

				wait_event_interruptible_timeout(v1718-> wait_q,1,NAK_TIMEOUT);
				this_read= 0;
				break;
			case USB_ST_DATAOVERRUN:
				{
					int count_retry= 100;
					do {
						//printk( "DATAOVERRUN\n");
						usb_clear_halt( v1718->v1718_dev, 6);
						if( usb_bulk_msg(v1718->v1718_dev,
						      usb_rcvbulkpipe(v1718->v1718_dev, 6),
						      ibuf, IBUF_SIZE, &partial,
						      (int) (HZ * 1))!= USB_ST_DATAOVERRUN){
							break;
						}
					} while( count_retry--);
				}
				up(&(v1718->lock));
				return -EIO;
			case USB_ST_SHORT_PACKET:
			case USB_ST_PARTIAL_ERROR:
			case USB_ST_URB_KILLED:
			case USB_ST_URB_PENDING:
			case USB_ST_REMOVED:
			case USB_ST_CRC:
			case USB_ST_BITSTUFF:
			case USB_ST_BUFFEROVERRUN:
			case USB_ST_NOTSUPPORTED:
			case USB_ST_BANDWIDTH_ERROR:
			case USB_ST_URB_INVALID_ERROR:
			case USB_ST_URB_REQUEST_ERROR:
			case USB_ST_STALL:
			default:
				usb_clear_halt( v1718->v1718_dev, 6);
				up(&(v1718->lock));
				printk("Read Whoops - result:0x%08x partial:%u this_read:%u\n",
				     result, partial, this_read);
				return -EIO;
			}
		}
		if (this_read) {
			if (copy_to_user(buffer, ibuf, this_read)) {
				up(&(v1718->lock));
				return -EFAULT;
			}
			count -= this_read;
			read_count += this_read;
			buffer += this_read;
		}
	}
	up(&(v1718->lock));
	return read_count;
}

static struct
file_operations usb_v1718_fops = {
	owner:		THIS_MODULE,            
	read:		read_v1718,
	write:		write_v1718,
#if LINUX_VERSION_CODE < VERSION(2,6,38)		
	ioctl:		ioctl_v1718,
#endif
// Rev. 0.4
#if LINUX_VERSION_CODE >= VERSION(2,6,11)
        unlocked_ioctl: unlocked_ioctl_v1718,
#endif
	open:		open_v1718,
	release:	close_v1718,
};

static struct usb_class_driver usb_v1718_class = {
  .name = "usb/v1718_%d",
  .fops = &usb_v1718_fops ,
#if LINUX_VERSION_CODE <= VERSION(2,6,13)
  .mode = S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP ,
#endif
  .minor_base = V1718_MINOR_BASE ,
};

static int probe_v1718(struct usb_interface *intf,
                         const struct usb_device_id *id)						 
{
	struct usb_device *dev = interface_to_usbdev(intf);
	int retval= 0;
	struct v1718_usb_data *v1718 = NULL;
// Rev 0.2	
//	char name[12];
	int minor;
	switch(id->idProduct) {
		case 0x0000:
			printk(KERN_INFO "CAEN Desktop Waveform Digitizers Carrier found at address %d\n", dev->devnum);	
			break;
		case 0x0001:
			printk(KERN_INFO "CAEN NIM Waveform Digitizers Carrier found at address %d\n", dev->devnum);
			break;
		case 0x0002:
			printk(KERN_INFO "CAEN V1718 found at address %d\n", dev->devnum);
			break;
		case 0x0005:
			printk(KERN_INFO "CAEN DT55xx HV Desktop Power Supply Carrier found at address %d\n", dev->devnum);
			break;
		case 0x0011:
			printk(KERN_INFO "CAEN HEXAGON MCA found at address %d\n", dev->devnum);
			break;
		case 0x0015:
			printk(KERN_INFO "CAEN NI6741 Peak Sensing ADC found at address %d\n", dev->devnum);
			break;
		case 0x0019:
			printk(KERN_INFO "CAEN V3718 found at address %d\n", dev->devnum);
			break;
		default:
			printk(KERN_INFO "CAEN V1718/(N957) found at address %d\n", dev->devnum);
			break;
	}
	down(&minor_table_mutex);
	for( minor = 0; minor < MAX_DEVICES; ++minor ) {
		if( minor_table[minor] == NULL )
			break;
	}
	if( minor >= MAX_DEVICES ) {
		printk(KERN_INFO "Too many devices\n");
		goto exit;
	}

	v1718 = kmalloc(sizeof(struct v1718_usb_data), GFP_KERNEL);
	if (v1718 == NULL) {
		printk("Out of memory\n");
		goto exit;
	}
	memset(v1718, 0x00, sizeof(*v1718));
	minor_table[minor] = v1718;

	if( usb_register_dev(intf, &usb_v1718_class))
  	{
    	printk("probe: Not able to get a minor for this device.\n");
		goto error;
	}
#ifdef CONFIG_USB_DYNAMIC_MINORS
	minor = intf->minor;
#endif
	v1718->present = 1;
	v1718->v1718_dev = dev;
	v1718->minor = minor;

	if (!(v1718->obuf = (char *) kmalloc(OBUF_SIZE, GFP_KERNEL))) {
		printk("probe: Not enough memory for the output buffer\n");
		goto error;
	}
	printk("probe: obuf address:%p", v1718->obuf);

	if (!(v1718->ibuf = (char *) kmalloc(IBUF_SIZE, GFP_KERNEL))) {
		printk("probe: Not enough memory for the input buffer\n");
		goto error;
	}
	printk("probe: ibuf address:%p", v1718->ibuf);

	usb_set_intfdata (intf, v1718);
#if LINUX_VERSION_CODE <= VERSION(2,6,32)
	init_MUTEX(&(v1718->lock));
#else
	sema_init(&(v1718->lock),1);
#endif
	goto exit;

error:
	usb_deregister_dev(intf, &usb_v1718_class);
	retval= -ENOMEM;
	v1718_delete(v1718);
	v1718 = NULL;

exit:
	up(&minor_table_mutex);
	return retval;

}

static void disconnect_v1718(struct usb_interface *intf)
{
	struct v1718_usb_data *v1718 = usb_get_intfdata (intf);

	int minor;

	down(&minor_table_mutex);
	down(&(v1718->lock));

	minor = v1718->minor;

	usb_set_intfdata (intf, NULL);
	if (v1718) {
		usb_deregister_dev(intf, &usb_v1718_class);
		if (v1718->open_count != 0) {
			/* better let it finish - the release will do whats needed */
			v1718->v1718_dev = NULL;
			up(&(v1718->lock));
		} else {
			up(&(v1718->lock));
			v1718_delete(v1718);
		}
	}


	printk(KERN_INFO "CAEN #%d device disconnected\n", minor);

	up(&minor_table_mutex);
}

static struct usb_device_id v1718_table [] = {
	{ USB_DEVICE(0x0547, 0x1002) }, 		/* CAEN V1718 */
	{ USB_DEVICE(0x21e1, 0x0000) }, 		
	{ USB_DEVICE(0x21e1, 0x0001) }, 
	{ USB_DEVICE(0x21e1, 0x0011) }, 
	{ USB_DEVICE(0x21e1, 0x0019) }, 
	{ USB_DEVICE(0x21e1, 0x0015) }, 
	{ }						/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, v1718_table);

// Rev 0.4
static struct usb_driver v1718_driver = {
#if LINUX_VERSION_CODE < VERSION(2,6,15)
	owner:		THIS_MODULE,
#endif	
	name:		"v1718",
	probe:		probe_v1718,
	disconnect:	disconnect_v1718,	
	id_table:	v1718_table,
};

static int __init usb_v1718_init(void)

{
	if (usb_register(&v1718_driver) < 0)
		return -1;
	printk(DRIVER_DESC " version " DRIVER_VERSION "\n");

	return 0;
}

static void __exit usb_v1718_cleanup(void)

{
	usb_deregister(&v1718_driver);
}

module_init(usb_v1718_init);
module_exit(usb_v1718_cleanup);

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

