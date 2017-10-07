#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "support.h"
#include "structs.h"
#include "filesystem.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

/*
 * generateData() - Converts source from hex digits to
 * binary data. Returns allocated pointer to data
 * of size amt/2.
 */
char* generateData(char *source, size_t size)
{
	char *retval = (char *)malloc((size >> 1) * sizeof(char));

	for(size_t i=0; i<(size-1); i+=2)
	{
		sscanf(&source[i], "%2hhx", &retval[i>>1]);
	}
	return retval;
}

void verifyFileSystem(char *file)
{
    printf("Verify of File System:\n");
    rootSector test_sector;
    int fp = open(file, O_RDWR);
    read(fp, &test_sector, sizeof(test_sector));
    
    printf("Start of FAT: %d\nStart of root Directory: %d\n\n", test_sector.FATroot, test_sector.rootDirectory);
}

void fat_error(char *message){
    printf("%s\n", message);
    exit(-1);
}

void dump(char *file, int pagenumber)
{
    unsigned char out[512];
    int i;
    int fp = open(file, O_RDWR);
    
    if (pagenumber < 0 || pagenumber > 8192){
        fat_error("ERROR: page outside filesystem!");
    }
    
    lseek(fp, 0 + 512 * pagenumber, SEEK_SET);

    read(fp, &out, sizeof(out));
    
    for(i = 0; i < 512; i++){
        if (i > 0 && i % 16 == 0){
           printf("    "); 
        }
        
        if (i > 0 && i % 32 == 0){
           printf("\n"); 
        }
        printf("%02x ", out[i]);
    } 
    printf("\n");
}

void usage (char *file)
{
    rootSector test_sector;
    char use;
    int fp = open(file, O_RDWR);
    read(fp, &test_sector, sizeof(test_sector));
    
    lseek(fp, test_sector.FATroot, SEEK_SET);
    int i;
    int total_file_size = 0;
    int total_system_size = 0;
    
    fatEntry fat_entry;
    for (i = 0; i < 8129 - 63 * 2; i++){
        read(fp, &fat_entry, sizeof(fat_entry));
        if (fat_entry.use == 1){
            total_file_size += 512;
        }else{
            total_file_size += 0;
        }
        lseek(fp, 512 + (i + 1) * sizeof(fat_entry), SEEK_SET);
    }
    
    close(fp);
    fp = open(file, O_RDWR);
    for (i = 0; i < 4194304; i++){
        read(fp, &use, sizeof(use));
        if (use != '\0'){
            total_system_size += 1;
        }
        lseek(fp, i + 1, SEEK_SET);
    }
    
    printf("Total actual files used: %d byte\n", total_file_size);
    printf("Total filesystem used: %d byte\n", total_system_size);
}

void filesystem_init(char* file)
{
    int filesize = 4194304;
    int fp, i;
    
    fp = open(file, O_RDWR | O_CREAT);
    
    if (fp < 0){
        printf("open error\n");
		exit(-1);
	}
    
    ftruncate(fp, filesize);
    rootSector root_sector;
    root_sector.FATroot = 512;
    root_sector.rootDirectory = 512 + 512 * 63;
    
    write(fp, &root_sector, sizeof(root_sector));
    lseek(fp, 513, SEEK_SET);
    
    fatEntry fat_entry;

    
    for (i = 0; i < 8129 - 63 * 2; i++){
        write(fp, &fat_entry, sizeof(fat_entry));
        lseek(fp, 512 + (i + 1) * sizeof(fat_entry), SEEK_SET);
    }
    
    directoryEntry directory_entry;
    directory_entry.index = 512 + 512 * 63 * 2;
    directory_entry.name = "root";
    
    lseek(fp, 512 + 512 * 63 * 2, SEEK_SET);
    write(fp, &directory_entry, sizeof(directory_entry));
    close(fp);
}
/*
 * filesystem() - loads in the filesystem and accepts commands
 */
void filesystem(char *file)
{
	/* pointer to the memory-mapped filesystem */
	char *map = NULL;
	int filesize = 4194304;
    int fp;
	/*
	 * open file, handle errors, create it if necessary.
	 * should end up with map referring to the filesystem.
	 */


    filesystem_init(file);
    
    fp = open(file, O_RDWR);
    map = mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fp, 0);

    if (map == MAP_FAILED){
        printf("map error\n");
        exit(-1);;
    }
    
    close(fp);
    
	/* You will probably want other variables here for tracking purposes */

    verifyFileSystem(file);
    
	/*
	 * Accept commands, calling accessory functions unless
	 * user enters "quit"
	 * Commands will be well-formatted.
	 */
	char *buffer = NULL;
	size_t size = 0;
	while(getline(&buffer, &size, stdin) != -1)
	{
		/* Basic checks and newline removal */
		size_t length = strlen(buffer);
		if(length == 0)
		{
            continue;
		}
		if(buffer[length-1] == '\n')
		{
			buffer[length-1] = '\0';
		}

		/* TODO: Complete this function */
		/* You do not have to use the functions as commented (and probably can not)
		 *	They are notes for you on what you ultimately need to do.
		 */

		if(!strcmp(buffer, "quit"))
		{
			break;
		}
		else if(!strncmp(buffer, "dump ", 5))
		{
			if(isdigit(buffer[5]))
			{
				dump(file, atoi(buffer + 5));
			}
			else
			{
				char *filename = buffer + 5;
				char *space = strstr(buffer+5, " ");
				*space = '\0';
				//open and validate filename
				//dumpBinary(file, atoi(space + 1));
			}
		}
		else if(!strncmp(buffer, "usage", 5))
		{
			usage(file);
		}
		else if(!strncmp(buffer, "pwd", 3))
		{
			//pwd();
		}
		else if(!strncmp(buffer, "cd ", 3))
		{
			//cd(buffer+3);
		}
		else if(!strncmp(buffer, "ls", 2))
		{
			//ls();
		}
		else if(!strncmp(buffer, "mkdir ", 6))
		{
			//mkdir(buffer+6);
		}
		else if(!strncmp(buffer, "cat ", 4))
		{
			//cat(buffer + 4);
		}
		else if(!strncmp(buffer, "write ", 6))
		{
			char *filename = buffer + 6;
			char *space = strstr(buffer+6, " ");
			*space = '\0';
			size_t amt = atoi(space + 1);
			space = strstr(space+1, " ");

			char *data = generateData(space+1, amt<<1);
			//write(filename, amt, data);
			free(data);
		}
		else if(!strncmp(buffer, "append ", 7))
		{
			char *filename = buffer + 7;
			char *space = strstr(buffer+7, " ");
			*space = '\0';
			size_t amt = atoi(space + 1);
			space = strstr(space+1, " ");

			char *data = generateData(space+1, amt<<1);
			//append(filename, amt, data);
			free(data);
		}
		else if(!strncmp(buffer, "getpages ", 9))
		{
			//getpages(buffer + 9);
		}
		else if(!strncmp(buffer, "get ", 4))
		{
			char *filename = buffer + 4;
			char *space = strstr(buffer+4, " ");
			*space = '\0';
			size_t start = atoi(space + 1);
			space = strstr(space+1, " ");
			size_t end = atoi(space + 1);
			//get(filename, start, end);
		}
		else if(!strncmp(buffer, "rmdir ", 6))
		{
			//rmdir(buffer + 6);
		}
		else if(!strncmp(buffer, "rm -rf ", 7))
		{
			//rmForce(buffer + 7);
		}
		else if(!strncmp(buffer, "rm ", 3))
		{
			//rm(buffer + 3);
		}
		else if(!strncmp(buffer, "scandisk", 8))
		{
			//scandisk();
		}
		else if(!strncmp(buffer, "undelete ", 9))
		{
			//undelete(buffer + 9);
		}



		free(buffer);
		buffer = NULL;
	}
	free(buffer);
	buffer = NULL;

}

/*
 * help() - Print a help message.
 */
void help(char *progname)
{
	printf("Usage: %s [FILE]...\n", progname);
	printf("Loads FILE as a filesystem. Creates FILE if it does not exist\n");
	exit(0);
}

/*
 * main() - The main routine parses arguments and dispatches to the
 * task-specific code.
 */
int main(int argc, char **argv)
{
	/* for getopt */
	long opt;

	/* run a student name check */
	check_student(argv[0]);

	/* parse the command-line options. For this program, we only support */
	/* the parameterless 'h' option, for getting help on program usage. */
	while((opt = getopt(argc, argv, "h")) != -1)
	{
		switch(opt)
		{
		case 'h':
			help(argv[0]);
			break;
		}
	}

	if(argv[1] == NULL)
	{
		fprintf(stderr, "No filename provided, try -h for help.\n");
		return 1;
	}

	filesystem(argv[1]);
	return 0;
}
