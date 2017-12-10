#include "sorter_thread.h"
#include "MergeSort.c"
#include <unistd.h>
#include <sys/mman.h>

pthread_mutex_t csv_mutex;
pthread_mutex_t counter_mutex;
int* csvCounter;
int* counter;
DataRow*** masterList;
char* header;

char getDataType(char* data){
	if(strcmp(data, "color") == 0)
		return 's';
 	else if(strcmp(data, "director_name") == 0)
		return 's'; 
	else if(strcmp(data, "actor_2_name") == 0)
		return 's';
	else if(strcmp(data, "genres") == 0)
		return 's';
	else if(strcmp(data, "actor_1_name") == 0)
		return 's';
	else if(strcmp(data, "movie_title") == 0)
		return 's';
	else if(strcmp(data, "actor_3_name") == 0)
		return 's';
	else if(strcmp(data, "plot_keywords") == 0)
		return 's';
	else if(strcmp(data, "movie_imdb_link") == 0)
		return 's';
	else if(strcmp(data, "language") == 0)
		return 's';
	else if(strcmp(data, "country") == 0)
		return 's';
	else if(strcmp(data, "content_rating") == 0)
		return 's';

	if(strcmp(data, "num_critic_for_reviews") == 0)
		return 'n';
	else if(strcmp(data, "duration") == 0)
		return 'n'; 
	else if(strcmp(data, "director_facebook_likes") == 0)
		return 'n'; 
	else if(strcmp(data, "actor_3_facebook_likes") == 0)
		return 'n';
	else if(strcmp(data, "actor_1_facebook_likes") == 0)
		return 'n';
	else if(strcmp(data, "gross") == 0)
		return 'n';
	else if(strcmp(data, "num_voted_users") == 0)
		return 'n';
	else if(strcmp(data, "cast_total_facebook_likes") == 0)
		return 'n';
	else if(strcmp(data, "facenumber_in_poster") == 0)
		return 'n';
	else if(strcmp(data, "num_user_for_reviews") == 0)
		return 'n';
	else if(strcmp(data, "budget") == 0)
		return 'n';
	else if(strcmp(data, "title_year") == 0)
		return 'n';
	else if(strcmp(data, "actor_2_facebook_likes") == 0)
		return 'n';
	else if(strcmp(data, "imdb_score") == 0)
		return 'n';
	else if(strcmp(data, "aspect_ratio") == 0)
		return 'n';
	else if(strcmp(data, "movie_facebook_likes") == 0)
		return 'n';

	return 'e';	
}

char* trimSpace(char* str){
	int end = strlen(str) - 1;
	while(str[end] == ' ' || str[end] == '\n' || str[end] == '\r')
	{
		str[end] = '\0';
		end--;
	}
	while(*str == ' ')
	{
		str++;
	}
	return str;
}


int getColNum(char* fileDir, char* colToSort){
	FILE* file = fopen(fileDir, "r");

	if(!file){
		return -1;
	}

	const char delim[2] = ",";
	char* row1p = (char*) calloc(1024, sizeof(char));
	char* row1 = row1p;
	
	if(fgets(row1, sizeof(char) * 1024, file) == NULL){
		free(row1p);
		fclose(file);
		return -1;
	}
	
	int colNumToSort = 0;
	char* curHead = strsep(&row1, delim);
	while( curHead != NULL && strcmp(colToSort, trimSpace(curHead)) != 0){
		curHead = strsep(&row1, delim);
		colNumToSort++;
	}
	free(row1p);
	if(curHead == NULL){

		fclose(file);
		return -1;
	}
	fclose(file);
	return colNumToSort;
} 

void *getcsvFilesHelp(void* arg){
	struct csvFile_arg_struct* args = (struct csvFile_arg_struct*) arg;
	ThreadNode* firstNode = NULL;
	ThreadNode* lastNode = NULL;
	int threadCounter = 0;

	char* dirName = args->dirName;
	DIR* dir = args->dir; 
	char* colToSort = args->colToSort;
	int* counter = args->counter;

	//If 1500 csv files/directories were found
	if((*csvCounter) > 1500){
		return NULL;
	}

	//Gets first file in directory dir
	struct dirent* newDirent = readdir(dir);

	//For every file in director dir
	while(newDirent != NULL && (*csvCounter) < 1500){
		//Gets file into correct format for 
		char* base = strdup(dirName);
		base = (char*) realloc(base, strlen(base) + strlen(newDirent->d_name) + 1);
		strcat(base, newDirent->d_name);

		//If file is a directory and not . or ..
		if(newDirent->d_type == DT_DIR && !(strcmp(newDirent->d_name, ".") == 0 || strcmp(newDirent->d_name, "..") == 0))
		{
			DIR* newDir = opendir(base);
			//If new directory is found and not .git (too many directories inside)
			if(newDir != NULL && strcmp(base, "./.git") != 0){
				base = (char*) realloc(base, strlen(base) + strlen("/") + 1);
				strcat(base, "/");

				struct csvFile_arg_struct args;
				args.dirName = base;
				args.dir = newDir;
				args.colToSort = colToSort;
				args.counter = counter;

				pthread_t newThread;
				pthread_create(&newThread, 0, &getcsvFilesHelp, (void*)(&args));
				printf("%lu,", (unsigned long)newThread);

				ThreadNode* next = (ThreadNode*) malloc(sizeof(ThreadNode) * 1);
				next->thread = &newThread;
				next->nextNode = NULL;

				if(firstNode == NULL){
					firstNode = next;
				}
				else{
					lastNode->nextNode = next;
				}
				lastNode = next;
				threadCounter += 1;

			}			
		}
		//If file is not a directory
		else{
			char* point = strchr(newDirent->d_name, '.');

			//If file is a csv file with correct column name
			if(point != NULL && strcmp(point, ".csv") == 0)
			{
				int colNum = getColNum(base, colToSort);
				if(colNum != -1){
					//Only allows one thread to change counter at a time

					struct sort_arg_struct args;
					args.fileDir = base;
					args.colToSort = colToSort;
					args.colNumToSort = colNum;

					pthread_t newThread;
					pthread_create(&newThread, 0, &sortIndiv, (void*)(&args));
					printf("%lu,", (unsigned long)newThread);

					ThreadNode* next = (ThreadNode*) malloc(sizeof(ThreadNode) * 1);
					next->thread = &newThread;
					next->nextNode = NULL;

					if(firstNode == NULL){
						firstNode = next;
					}
					else{
						lastNode->nextNode = next;
					}
					lastNode = next;
					threadCounter += 1;
				}		
			}

		}
		newDirent = readdir(dir);
		//free(base);
	} 

	//Join all of this thread's children and free the nodes
	while(firstNode != NULL){
		pthread_join(*(firstNode->thread), 0);
		ThreadNode* prev = firstNode;
		firstNode = firstNode->nextNode;
		free(prev);
	}

	pthread_mutex_lock(&counter_mutex);
	*counter += threadCounter;
	pthread_mutex_unlock(&counter_mutex);

	return NULL;	
}


void sortcsvFiles(char* dirName, char* outputDir, char* colToSort, int * counter){
	DIR* dir = opendir(dirName);
	if(!dir){
		return;
	}

	pthread_mutex_init(&counter_mutex, 0);
	pthread_mutex_init(&csv_mutex, 0);

	masterList = (DataRow***) malloc(sizeof(DataRow**) * 1500);

	csvCounter = (int*) malloc(sizeof(int) * 1);

	struct csvFile_arg_struct args;
	args.dirName = dirName;
	args.dir = dir;
	args.colToSort = colToSort;
	args.counter = counter;

	char* fileName = strdup(outputDir);

	header = NULL;

	getcsvFilesHelp((void*)(&args));

	printf("\n\n\ncsvFiles: %d\n\n\n", *csvCounter);

	if(masterList[0] != NULL){
		//Create file for sorted csv
		fileName = realloc(fileName, sizeof("AllFiles-sorted-.csv") + sizeof(fileName) + sizeof(colToSort) + 1);
		strcat(fileName, "AllFiles-sorted-");
		strcat(fileName, colToSort);
		strcat(fileName, ".csv");
	
		FILE* outfp;
		outfp = fopen(fileName, "w");

		fprintf(outfp, header);

		//Place all data into new list then sort
		DataRow** sortedList = (DataRow**) malloc(sizeof(DataRow*) * 1000000);

		int cur = 0;
		int i = 0;
		while(masterList[i] != NULL){
			DataRow** curList = masterList[i];
			int j = 0;
			while(curList[j] != NULL){
				sortedList[cur] = curList[j];
				j++;
				cur++;
			}
			free(curList);
			i++;
		}
		mergeSort(sortedList, 0, (cur - 1));

		//Prints sorted list to output file then frees data
		i = 0;
		while(sortedList[i] != NULL){
			fprintf(outfp, sortedList[i]->data);

			free(sortedList[i]->dataCompare);
			free(sortedList[i]->data);
			free(sortedList[i]);
			i++;
		}
		free(sortedList);
	}
	else{
		printf("\nNo valid csv file was found\n");
	}
	free(header);
	free(masterList);
	free(fileName);
	free(csvCounter);
	return;
}

void *sortIndiv(void* arg){
	struct sort_arg_struct* args = (struct sort_arg_struct*) arg;
	
	char* fileDir = args->fileDir;
	char* colToSort = args->colToSort;
	int colNumToSort = args->colNumToSort;

	if(colNumToSort == -1)
		return NULL;

	FILE* file = fopen(fileDir, "r");

	//Get column heading and create copy of it
	char* curHeader = (char*) calloc(1024, sizeof(char));
	if(fgets(curHeader, sizeof(char) * 1024, file) == NULL){
		free(curHeader);
		return NULL;
	}
	if(header == NULL){
		header = strdup(curHeader);
	}
	free(curHeader);

	const char delim[2] = ",";
	const char otherDelim[3] = "\"";
	const char slashDelim[2] = "/";
	char* nextSep;
	char* fileName;
	char* origFile = strdup(fileDir);
	char* origFilept = origFile;
	while((nextSep = strsep(&origFile, slashDelim)) != NULL)
	{
		fileName = nextSep;
	}
	free(origFilept);
	if(fileName == NULL)
		fileName = fileDir;

	fileName[strlen(fileName) - 4] = '\0';

	//Creating list of DataRow
	DataRow** list = (DataRow**) malloc(sizeof(DataRow*) * 20000);
	char dataType = getDataType(colToSort);
	int curRowNum = 0;
	
	char* origRow = (char*) calloc(1024, sizeof(char));
	
	//Goes through each row in csv file
	while(fgets(origRow, sizeof(char) * 1024, file) != NULL){
		//New dataRow for array
		DataRow* newDataRow = (DataRow*) malloc(sizeof(DataRow));
		newDataRow->dataType = dataType;

		char* origRowDelim = strdup(origRow);
		char* rowDelim = origRowDelim;

		//newRow is data for new row with trimmed spaces
		char* newRow = (char*) calloc(1024, sizeof(char));

		//Goes through each word, trimming spaces and finding data to sort by and add data to array
		char* curWord = strsep(&rowDelim, delim);
		int firstWord = 1;
		int i = 0;

		while(curWord != NULL){
			if(firstWord)
				firstWord = 0;
			else
				strcat(newRow, ",");

			char* wordToAdd = strdup(curWord);
			if(strlen(wordToAdd) > 1 && wordToAdd[0] == '"' && wordToAdd[strlen(wordToAdd) - 1] != '"')
			{
				char* otherHalf = strsep(&rowDelim, otherDelim);
				wordToAdd = (char*)realloc(wordToAdd, strlen(wordToAdd) + strlen(",") + strlen(otherHalf) + strlen("\"") + 1);
				strcat(wordToAdd, ",");
				strcat(wordToAdd, otherHalf);	
				strcat(wordToAdd, "\"\0");	

				curWord = strsep(&rowDelim, delim);
			}
			char* origPointer = wordToAdd;
			char* trimWord = trimSpace(origPointer);

			if(i == colNumToSort){
				DataCompare* newDataCompare = (DataCompare*) malloc(sizeof(DataCompare));
				if(dataType == 'n'){
					newDataCompare->numData = atof(trimWord);
				}
				else if(dataType == 's'){
					newDataCompare->stringData = trimWord;
				}
				newDataRow->dataCompare = newDataCompare;
			}

			strcat(newRow, trimWord);
			curWord = strsep(&rowDelim, delim);
			free(wordToAdd);

			i++;
		}
		free(origRowDelim);

		strcat(newRow, "\n");
		newDataRow->data = newRow;
		list[curRowNum] = newDataRow;
		curRowNum++;
	}

	//Sort list
	mergeSort(list, 0, (curRowNum -1));

	//Ensure no other thread accesses the masterList or csvCounter
	pthread_mutex_lock(&csv_mutex);
	if(*csvCounter < 1500){
		masterList[*csvCounter] = list;
		*csvCounter += 1;
	}

	pthread_mutex_unlock(&csv_mutex);

	free(origRow);

	fclose(file);

	return NULL;
}



/*
Read input and sort them into a standard format
inputData[0] = "Error" or "-c"
inputData[1] = error message or col-to-sort
inputData[2] = "-d"
inputData[3] = empty or file path
inputData[4] = "-o"
inputData[5] = empty or file path

*/
char ** readInput(int argc, char* argv[]){
	char ** inputData = (char **)malloc(6 * sizeof(char *));
	
	//Initial data for error checking, must have malloc ready for subsequent free
	inputData[0] = (char *) malloc(10 * sizeof(char));
	inputData[1] = (char *) malloc(50 * sizeof(char));
	inputData[2] = (char *) malloc(10 * sizeof(char));
	inputData[3] = (char *) malloc(50 * sizeof(char));
	inputData[4] = (char *) malloc(10 * sizeof(char));
	inputData[5] = (char *) malloc(50 * sizeof(char));
	
	//check to see if there are correct number of arguments
	if(argc < 2){
		strcpy(inputData[0], "Error");
		strcpy(inputData[1], "Error: no arguments");
		return inputData;
	}
	
	//checkt to see if there are columns to sort by
	if(argc < 3){
		strcpy(inputData[0], "Error");
		strcpy(inputData[1], "Error: column to sort by not given");
		return inputData;
	}
	
	//3 arguments. Ex: ./Sorter -c "test"
	if(argc == 3){
		//check if there is a -c in any of the command input, return error if not
		if(strcmp(argv[1], "-c") != 0){
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: not sorting by columns");
			return inputData;
		}

		//return array with correct data format
		strcpy(inputData[0], "-c");		
		strcpy(inputData[1], argv[2]);
		strcpy(inputData[2], "-d");
		strcpy(inputData[4], "-o");
		return inputData;
	}
	
	//5 arguments
	//Ex1: ./Sorter -c "colToSort" -d "some path here"
	//Ex2: ./Sorter -c "colToSort" -o "some path here"
	if(argc == 5){
		strcpy(inputData[2], "-d");
		strcpy(inputData[4], "-o");
		//check if there is a -c in any of the command input, return error if not
		if(strcmp(argv[1], "-c") != 0  && strcmp(argv[3], "-c") != 0){
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: not sorting by columns");
			return inputData;
		}	

		//check for the first flag
		if(strcmp(argv[1], "-c") == 0){
			strcpy(inputData[0], "-c");		
			strcpy(inputData[1], argv[2]);
		}
		else if(strcmp(argv[1], "-d") == 0){
			strcpy(inputData[2], "-d");
			strcpy(inputData[3], argv[2]);
		}
		else if(strcmp(argv[1], "-o") == 0){
			strcpy(inputData[4], "-o");
			strcpy(inputData[5], argv[2]);
		}
		else{
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: Invalid flag ");
			strcat(inputData[1], argv[1]);
			return inputData;
		}

		//check for third flag
		if(strcmp(argv[3], "-c") == 0){
			strcpy(inputData[0], "-c");		
			strcpy(inputData[1], argv[4]);
		}
		else if(strcmp(argv[3], "-d") == 0){
			strcpy(inputData[2], "-d");
			strcpy(inputData[3], argv[4]);
		}
		else if(strcmp(argv[3], "-o") == 0){
			strcpy(inputData[4], "-o");
			strcpy(inputData[5], argv[4]);
		}
		else{
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: Invalid flag ");
			strcat(inputData[1], argv[3]);
			return inputData;
		}
		
		//Good inputData
		return inputData;
	}

	//7 arguments
	//Ex1: ./Sorter -c "colToSort" -d "some path here" -o "some path here"
	//Ex2: ./Sorter -o "some path ehre" -c "colToSort" -d "some path here"
	if(argc == 7){
		strcpy(inputData[2], "-d");
		strcpy(inputData[4], "-o");
		//check if there is a -c in any of the command input, return error if not
		if(strcmp(argv[1], "-c") != 0  && strcmp(argv[3], "-c") != 0 && strcmp(argv[5], "-c") != 0){
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: not sorting by columns");
			return inputData;
		}	

		//check for the first flag
		if(strcmp(argv[1], "-c") == 0){
			strcpy(inputData[0], "-c");		
			strcpy(inputData[1], argv[2]);
		}
		else if(strcmp(argv[1], "-d") == 0){
			strcpy(inputData[2], "-d");
			strcpy(inputData[3], argv[2]);
		}
		else if(strcmp(argv[1], "-o") == 0){
			strcpy(inputData[4], "-o");
			strcpy(inputData[5], argv[2]);
		}
		else{
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: Invalid flag ");
			strcat(inputData[1], argv[1]);
			return inputData;
		}

		//check for third flag
		if(strcmp(argv[3], "-c") == 0){
			strcpy(inputData[0], "-c");		
			strcpy(inputData[1], argv[4]);
		}
		else if(strcmp(argv[3], "-d") == 0){
			strcpy(inputData[2], "-d");
			strcpy(inputData[3], argv[4]);
		}
		else if(strcmp(argv[3], "-o") == 0){
			strcpy(inputData[4], "-o");
			strcpy(inputData[5], argv[4]);
		}
		else{
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: Invalid flag ");
			strcat(inputData[1], argv[3]);
			return inputData;
		}

		//check for the first flag
		if(strcmp(argv[5], "-c") == 0){
			strcpy(inputData[0], "-c");		
			strcpy(inputData[1], argv[6]);
		}
		else if(strcmp(argv[5], "-d") == 0){
			strcpy(inputData[2], "-d");
			strcpy(inputData[3], argv[6]);
		}
		else if(strcmp(argv[5], "-o") == 0){
			strcpy(inputData[4], "-o");
			strcpy(inputData[5], argv[6]);
		}
		else{
			strcpy(inputData[0], "Error");
			strcpy(inputData[1], "Error: Invalid flag ");
			strcat(inputData[1], argv[5]);
			return inputData;
		}

		//good input data
		return inputData;
	}
	

	strcpy(inputData[0], "Error");
	strcpy(inputData[1], "Error: Insufficient arguments");
	
	return inputData;
}

int main(int argc, char* argv[])
{	
	char ** inputData = readInput(argc, argv);
	
	char* colToSort = NULL;
	char* inDir = NULL;
	char* outDir = NULL;

	//UNCOMMENT THIS IF YOU WANT TO SEE THE DATA INSIDE inputData
	/*
	int i;
	for(i = 0; i < 6; i++){
		printf("%s\n", inputData[i]);
	}
	*/
	
	if(strcmp(inputData[0], "Error") == 0){
		printf("%s\n", inputData[1]);
		
	}
	else{
		colToSort = inputData[1];

		//Checks if given -d folder. If none then gets current directory
		if(strcmp(inputData[2], "-d") == 0){
			//Given is relative path
			if(strlen(inputData[3]) > 1 && inputData[3][0] == '.' && inputData[3][1] == '/'){
				inDir = strdup(inputData[3]);
			}
			//Given is absolute path
			else if(strlen(inputData[3]) > 0 && inputData[3][0] == '/'){
				inDir = strdup(inputData[3]);
			}
			//Given is assumed to be relative path
			else{
				inDir = (char*) malloc(sizeof(char) * 3);
				inDir[0] = '.';
				inDir[1] = '/';
				inDir[2] = '\0'; 

				inDir = (char*) realloc(inDir, strlen(inDir) + strlen(inputData[3]) + 1);
				strcat(inDir, inputData[3]);
				
			}
			if(inDir[strlen(inDir) - 1] != '/'){
				inDir = (char*) realloc(inDir, strlen(inDir) + strlen("/") + 1);
				strcat(inDir, "/");
			}

		}
		else{
			inDir = (char*) malloc(sizeof(char) * 3);
			inDir[0] = '.';


			inDir[1] = '/';
			inDir[2] = '\0'; 
		}

		if(strcmp(inputData[4], "-o") == 0){
			char * output = NULL;
			if(inputData[5] != NULL)
				output = strdup(inputData[5]);
			else{
				printf("\n\n\n\n");
				outDir = (char*) malloc(sizeof(char) * 3);
				outDir[0] = '.';
				outDir[1] = '/';
				outDir[2] = '\0'; 
			}
			//Given is relative path
			if(strlen(output) > 1 && output[0] == '.' && output[1] == '/'){
				outDir = strdup(output);
			}
			//Given is absolute path
			else if(strlen(output) > 0 && output[0] == '/'){
				outDir = strdup(output);
			}
			//Given is assumed to be relative path
			else{
				outDir = (char*) malloc(sizeof(char) * 3);
				outDir[0] = '.';
				outDir[1] = '/';
				outDir[2] = '\0'; 

				outDir = (char*) realloc(outDir, strlen(outDir) + strlen(output) + 1);
				strcat(outDir, output);
				
			}
			if(outDir[strlen(outDir) - 1] != '/'){
				outDir = (char*) realloc(outDir, strlen(outDir) + strlen("/") + 1);
				strcat(outDir, "/");
				
			}
		}

		
		printf("Column to sort: %s\n", colToSort);
		printf("Input directory: %s\n", inDir);
		printf("Output directory: %s\n", outDir); 
		

		printf("Initial PID: %d\n", getpid());
		counter = (int*) malloc(sizeof(int) * 1);
		printf("TIDS of all child threads: ");

		//current process counts as 1
		*counter = 1;
		sortcsvFiles(inDir, outDir, colToSort, counter);

		printf("\nTotal number of threads: %d\n", *counter); 
		free(counter);
		free(inDir);
		free(outDir);
	}

	//Checks if -o directory. If none then returns null



	free(inputData[0]);
	free(inputData[1]);
	free(inputData[2]);
	free(inputData[3]);
	free(inputData[4]);
	free(inputData[5]);
	free(inputData);

	return 0;
}

















