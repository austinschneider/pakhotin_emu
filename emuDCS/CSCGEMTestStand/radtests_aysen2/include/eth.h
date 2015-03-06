#ifndef RADTEST_ETH_H
#define RADTEST_ETH_H


#define SCHAR_MAJOR		42

#define SCHAR_INC		1024
#define SCHAR_TIMER_DELAY	5*HZ
#define SCHAR_POOL_MIN		10*1024

#define SCHAR_MAX_SYSCTL	512
#define DEV_SCHAR		10
#define DEV_SCHAR_ENTRY		1

/* ioctl's for schar. */
#define SCHAR_IOCTL_BASE	0xbb
#define SCHAR_RESET     	_IO(SCHAR_IOCTL_BASE, 0)
#define SCHAR_END		_IOR(SCHAR_IOCTL_BASE, 1, int)

//#define DEBUG
//#ifdef DEBUG
#define MSG(string, args...) if (schar_debug) printk(KERN_DEBUG "schar: " string, ##args)
//#else
//#define MSG(string, args...)
//#endif

//#define MAX_DAT_SIZE 9000 /* above 9000 and it balks */
#define MAX_DAT_SIZE 8192 // 8 KB

#define RAMPAGE_SIZE 4*1024 // size of RAM pages from FPGA

// One byte header to identify a pattern file, must not be odd or looptests will suppress it
#define PAT_FILE_HDR 0xf2

#endif //RADTEST_ETH_H
