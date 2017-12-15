#include<dirent.h>
#include<netdb.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>

#ifndef SORTER_SERVER_H
#define SORTER_SERVER_H

//data to compare each row by. Either comparing data of string or numeric values
union data_compare{
	double numData;
	char* stringData;
};
typedef union data_compare DataCompare;


//single row of csv file. Has content of the csv file, data to compare each row with, and the datatype of the data to compare with
struct data_row{

/**
	dataType:
		n = numData
		s = stringData
**/
	char dataType;
	char* data;
	DataCompare* dataCompare;	
};typedef struct data_row DataRow;

//linked list for child threads of directory thread
struct thread_node{
	pthread_t* thread;
	struct thread_node* nextNode;
};typedef struct thread_node ThreadNode;

//arguments for sorting csv file for thread
struct sort_arg_struct{
	char* file;
	char* colToSort;
};

//Print error and exit
void error(char *msg);

//Gets dataType of column name
char getDataType(char* data);

//Trims leading and trailing spaces from a string
char* trimSpace(char* str);

//Gets col number of string in file header (first row of file)
int getColNum(char* fileDir, char* colToSort);

//sort csv file
void *sortIndiv(void* arg);

#endif
