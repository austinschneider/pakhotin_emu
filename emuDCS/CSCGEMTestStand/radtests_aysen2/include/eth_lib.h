#ifndef RAD_TEST_ETH_LIB_H
#define RAD_TEST_ETH_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/if_ether.h> 
#include <netinet/in.h> 
#include <netinet/ip.h> 
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <netpacket/packet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include "debug.h"
#include "utils.h"
#include "eth.h"

#ifndef RAD_TEST_ETH_LIB_CPP
// these are defined in eth_lib.c
//extern char *wpkt;
extern char wpkt[4+MAX_DAT_SIZE];
extern char *wdat;
extern int nwdat;
//extern char *rpkt;
extern char rpkt[18+MAX_DAT_SIZE];
extern char *rdat;
extern int nrdat;
#endif


int eth_open(char *dev_name);

int eth_register_mac();

int eth_reset(void);

int eth_read(int suppression);

int eth_write();

void eth_close(void);


#endif //RAD_TEST_ETH_LIB_H
