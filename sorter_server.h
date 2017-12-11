/*****
*
*	Define structures and function prototypes for your sorter
*
*
*
******/
#include<dirent.h>
#include<netdb.h>
#include<netinet/in.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
	char* fileDir;
	char* colToSort;
	int colNumToSort;
};

//arguments for sortcsvFilesHelp 
struct csvFile_arg_struct{
	char* dirName;
	DIR* dir;
	char* colToSort;
	int* counter;
};

//Print error and exit
void error(char *msg);

//Gets pointer to array of file pointers of type csv files. Also traverses through any directories inside dir
void sortcsvFiles(char* dir, char* outputDir, char* colToSort, int* counter);

//Helper method to getcsvFiles
void sortcsvFilesHelp(char* dirName, DIR* dir, char* outputDir, char* colToSort, int* counter);

//Gets col number of string in file header (first row of file)
int getColNum(char* fileDir, char* colToSort);

//sort csv file
void *sortIndiv(void* arg);

//Trims leading and trailing spaces from a string
char* trimSpace(char* str);

//Gets dataType of column name
char getDataType(char* data);

//Suggestion: prototype a mergesort function
void merge(DataRow ** array, int left, int mid, int right);
void mergeSort(DataRow ** array, int left, int right);

//Gets dataType of column name
char getDataType(char* data);

//Formats user input to a standard order of -c <col-to-sort> -d <input file path> -o <output file path>
char ** readInput(int argc, char* argv[]);

#endif
