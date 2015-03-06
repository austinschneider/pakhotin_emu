#ifndef RADTEST_COMMANDS_H
#define RADTEST_COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include "debug.h"
#include "eth_lib.h"

int write_command(int num, int adr=0, char* block=0);
// send command code for given command and address

int read_command(char **pkt);
// read and set pkt pointer to next packet
// return >0 is number corresponding to packet type, <0 is error/unrecognized packet

int retrieve_Snap12ErrorCounts(unsigned int* counters);
// *** the counters array *must* have places for at least 8 elements
// fill the counters array
// return value!=0 indicates failure

int retrieve_TranslatorErrorCounts(unsigned short* counters); 
// *** the counters array *must* have places for at least 24 elements
// fill counters array
// return value!=0 indicates failure

int send_RAMPage(int pageid, char* block);
// pageid should be a page number of BRAM
// block should be buffer with 4KB page
// return value!=0 indicates failure

int retrieve_RAMPage(int pageid, char* block, char* hdr);
// *** block must be at least a 4KB buffer
// pageid should be a page number of RAM
// fill block buffer with 4KB block
// return value!=0 indicates failure

int retrieve_PacketID(int* id);
// set id to the packet id (page number)
// return value!=0 indicates failure

int get_EthErrorCounts(unsigned int* counters);
// *** the counters array *must* have places for at least 8 elements
// fill counters array
// return value!=0 indicates failure

int retrieve_VotingErrorCounts(unsigned int* counters);
// *** the counters array *must* have places for at least 12 elements
// fill the voting counters array
// return value!=0 indicates failure

#endif //RADTEST_ETH_LIB_H
