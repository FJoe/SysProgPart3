#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int COUNT = 1;

//generate file in sub-directories
void createCsvFile(char * pathName, int numFile){
	FILE* fp = NULL;
	char strFile[20];
	int i;

	for(i = 0; i < numFile; i++){
		//open the template csv file in base directory
		char* partialCsv = "partial.csv";
		FILE* partialPtr = fopen(partialCsv, "r");

		//generate a name for the new file
		sprintf(strFile, "%d", COUNT);
		char* fileName = (char*) malloc(500 * sizeof(char));
		strcpy(fileName, pathName);
		strcat(fileName, "/partial");
		strcat(fileName, strFile);
		strcat(fileName, ".csv");

		//open new file
		fp = fopen(fileName, "w");

		//place individual character into generated file
		char c; 
		c = fgetc(partialPtr);

		while(c != EOF){
			fputc(c, fp);
			c = fgetc(partialPtr);
		}		

		COUNT++;
		fclose(fp);
		free(fileName);
		fclose(partialPtr);
	}
	
}

int main(int argc, char* argv[])
{	
	//the base directory will have x amount of new directories
	//each new directory will reach a span of y level
	//each new directory will have z amount of csv files
	int numDir;
	printf("Enter the number of directories you want in base directory: \n");
	scanf("%d", &numDir);
	
	
	int numDirLevel;
	printf("Enter the number of levels you want in each directory: \n");
	scanf("%d", &numDirLevel);
	
	int numFile;
	printf("Enter the number of csv files you want to create in each directory: \n");
	scanf("%d", &numFile);
	

	int i = 0;
	char strDir[20];
	
	//make directories inside the base directory
	for(i = 0; i < numDir; i++){
		sprintf(strDir, "%d", i + 1);
		char * folderName = (char*) malloc(500 * sizeof(char));
		strcpy(folderName, "./");
		strcat(folderName, "folder");
		strcat(folderName, strDir);
		mkdir(folderName, 0777);
	
		createCsvFile(folderName, numFile);
		
		//make directories inside the newly created directories
		int j;
		char strSubDir[20];

		char * subFolderName = (char*) malloc(500 * sizeof(char));
		strcpy(subFolderName, folderName);

		for(j = 0; j < numDirLevel; j++){
			sprintf(strSubDir, "%d", j + 1);
			strcat(subFolderName, "/folder");
			strcat(subFolderName, strDir);
			strcat(subFolderName, ".");
			strcat(subFolderName, strSubDir);

			mkdir(subFolderName, 0777);

			createCsvFile(subFolderName, numFile);
		}
		free(subFolderName);
		free(folderName);
	}

	//create file in base directory
	FILE* fp = NULL;
	char strFile[20];

	for(i = 0; i < numFile; i++){
		//open the template csv file in base directory
		char* partialCsv = "partial.csv";
		FILE* partialPtr = fopen(partialCsv, "r");	

		//generate a name for the new file
		sprintf(strFile, "%d", COUNT);
		char* fileName = (char*) malloc(500 * sizeof(char));
		strcpy(fileName, "partial");
		strcat(fileName, strFile);
		strcat(fileName, ".csv");

		//open new file
		fp = fopen(fileName, "w");
	
		//place individual character into generated file
		char c; 
		c = fgetc(partialPtr);

		while(c != EOF){
			fputc(c, fp);
			c = fgetc(partialPtr);
		}	
		
		COUNT++;
		fclose(fp);
		free(fileName);
		fclose(partialPtr);
	}

	
	
	
}
