#ifndef STRUCTS_H
#define STRUCTS_H
#include <time.h>

/*
 *
 * Define page/sector structures here as well as utility structures
 * such as directory entries.
 *
 * Sectors/Pages are 512 bytes
 * The filesystem is 4 megabytes in size.
 * You will have 8K pages total.
 *
 */
struct rootSector{
  int FATroot;
  int rootDirectory;
};

//This represents a directory listing on a directory page
//It is just a name - index pair
struct directoryEntry{
  char* name;
  int index;
};

struct fatEntry{
  int use;
  int next;
};

struct directoryPage{
  char *name;
  int thisDirectory; //index in fat
  int previousDirectory; //index in fat
  struct directoryEntry directories[50];
};

#endif