#ifndef STRUCTS_H
#define STRUCTS_H

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

extern struct fat{

} fat;

extern struct rootSector{
  char* fatPtr;
  char* rootDir;
} rootSector;

extern struct directoryPage{

  
} directoryPage;

#endif
