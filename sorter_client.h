#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef SORTER_CLIENT_H
#define SORTER_CLIENT_H
typedef struct params{
    char objectname[500];
}params;

//Prints error and exits program
void error(char * msg);

void* threadFile(void* parameters);

void* threadDir(void* parameters);

int iscolumn(char* col);

void recursiveSearch(char* dir);

int sendCSV(char * filename);
#endif
