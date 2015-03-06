#ifndef RADTEST_UTILS_H
#define RADTEST_UTILS_H

#include "eth.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <time.h>

/* Macro to format char[] as hex for ostreams */
#define HEX(x)  std::setw(2)<<std::setfill('0')<<std::hex<<int(x)
#define DEC()   std::dec<<std::setfill(' ')


/* Common funtions */

/* Hard limit of pattern size */
#define MAX_PAT_SIZE 1024*1024 // 1 GB


int file_size(FILE *fp);
  /* Returns the size of a file */


int get_pattern(char* filename, char **buf, int verbose);
  /* A funtion to read a pattern file into a buffer.
   * filename = name of a file to read
   * *buf = pointer char array that will be allocated and filled (remeber to free)
   * verbose = a verbosity flag
   * return value: >=0 indicated the size of the pattern
   * return value: <0 indicated an error
   */

void dumphex(int n, char* buf, std::ostream* s=&std::cout);
// print array of hex numbers

int save_page_to_file(int pageid, char* block);
// save this block (must have size RAMPAGE_SIZE) to a file

int get_sequential_block(int pageid, char* block, int start=2);
// fill block (must have size RAMPAGE_SIZE) with contents of file for this pageid
// return 0 if successful

int get_page_from_file(int pageid, char* block);
// fill block (must have size RAMPAGE_SIZE) with contents of file for this pageid
// return 0 if successful

unsigned int bytes2int(char* c);
// little endian conversion of first sizeof(int) bytes in c to an unsigned int

unsigned short bytes2short(char* c);
// little endian conversion of first sizeof(short) bytes in c to an unsigned short

std::string time_string(bool terse=true);
// get formated time string

#endif //RADTEST_UTILS_H
