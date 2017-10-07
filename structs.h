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
typedef struct{
  int FATroot;
  int rootDirectory;
}rootSector;

typedef struct{
  short prev;
  short current;
  short index;
  char *name;
  short size;
}directoryEntry;

typedef struct{
  int use;
  int next;
}fatEntry;

typedef struct{  
  int thisDirectory;
  int previousDirectory;
  struct directoryEntry **directories;
}directoryPage;

#endif