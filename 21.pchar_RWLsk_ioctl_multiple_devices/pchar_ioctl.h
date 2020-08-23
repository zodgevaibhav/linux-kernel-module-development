#ifndef __PCHAR_IOCTL_H
#define __PCHAR_IOCTL_H

#include <linux/ioctl.h>

struct pchar_info {
	int size;
	int len;
	int avail;
};

#define FIFO_CLEAR		_IO('x', 1)
#define FIFO_GET_INFO	_IOR('x', 2, struct pchar_info*)

#endif


