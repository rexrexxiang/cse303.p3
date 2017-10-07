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

extern struct filesystem{
  struct rootSector;
  FILE* file;
} filesystem;

extern struct rootSector{
  int FATroot;
  int rootDirectory;
} rootSector;

extern struct directoryEntry{
  int index;
  char *name;
  time_t timeCreated;
  int size;
} directoryEntry;

extern struct fatEntry{
  int nextEntry; //index of next entry, -1 means EOF
  int inUse; // 1 means yes, 0 means no
} fatEntry;

extern struct directoryPage{  
  int thisDirectory;
  int previousDirectory;
  struct directoryEntry **directories;
} directoryPage;

#endif
