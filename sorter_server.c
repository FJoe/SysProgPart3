/* A simple server in the internet domain using TCP the port number is passed as an argument */

#include"sorter_server.h"
#include "MergeSort.c"

pthread_mutex_t csv_mutex;
pthread_mutex_t counter_mutex;
int* csvCounter;
int* counter;
DataRow*** masterList;
char* header;

void error(char *msg){
	perror(msg);
	exit(1);
}

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


int getColNum(char* header, char* colToSort){
	int colNumToSort = 0;
	const char delim[2] = ",";

	char* row1 = strdup(header);
	char* row1p = row1;

	char* curHead = strsep(&row1, delim);
	while( curHead != NULL && strcmp(colToSort, trimSpace(curHead)) != 0){
		curHead = strsep(&header, delim);
		colNumToSort++;
	}
	free(row1p);
	if(curHead == NULL){
		return -1;
	}
	return colNumToSort;
}

void *sortIndiv(void* arg){
	const char delim[2] = ",";
	const char delimRow[2] = "\0";
	const char otherDelim[3] = "\"";

	//Gets arguments
	struct sort_arg_struct* args = (struct sort_arg_struct*) arg;
	char* file = args->file;
	char* colToSort = args->colToSort;

	//Get column heading and create copy of it
	char* curHeader = strsep(&file, delimRow);
	if(curHeader == NULL){
		return NULL;
	}
	if(header == NULL){
		header = strdup(curHeader);
	}

	//Get col num to sort by
	int colNumToSort = getColNum(header, colToSort);
	if(colNumToSort == -1)
		return NULL;

	//Creating list of DataRow
	DataRow** list = (DataRow**) malloc(sizeof(DataRow*) * 20000);
	char dataType = getDataType(colToSort);
	int curRowNum = 0;
	
	char* origRow;
	//Goes through each row in csv file
	while((origRow = strsep(&file, delimRow)) != NULL){
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
	return NULL;
}



int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	//check for proper args
	if (argc < 3){
		fprintf(stderr,"ERROR, no -p parameter or port provided\n");
		return(1);
	}
	if(strcmp(argv[1], "-p") != 0){
		fprintf(stderr,"ERROR, no -p parameter given\n");
		return(1);
	}

	//create a socket(int domain, int type, int protocol)
	//domain is IPv4
	//type is TCP
	//protocol is 0 for default
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//check to see socket correctly opened
	if (sockfd < 0){
		error("ERROR opening socket");
	}

	//clear the buffer in memory to 0s
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//set portnumber to user given. Server and client portno must match
	portno = atoi(argv[2]);

	/*setup the host_addr structure for bind call*/
	//server byte order, IPv4
	serv_addr.sin_family = AF_INET;
	//filled with host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//convert integer value to network order	
	serv_addr.sin_port = htons(portno);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_
	// bind() passes file descriptor, the address 
	// and the length of the address 
	// This bind() call will bind  the socket to the current IP address on port, portno
     
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}

	// This listen() call tells the socket to listen to the incoming connections.
     	// The listen() function places all incoming connection into a backlog queue
     	// until accept() call accepts the connection.
     	// Here, we set the maximum size for the backlog queue to 32.
	// Backlog queue shouldn't grow to a large size.
	listen(sockfd,32);
	socklen_t clilen = sizeof(cli_addr);

	// This accept() function will write the connecting client's address info 
    	// into the the address structure and the size of that structure is clilen.
   	// The accept() returns a new socket file descriptor for the accepted connection.
     	// So, the original socket file descriptor can continue to be used 
     	// for accepting new connections while the new socker file descriptor is used for
     	// communicating with the connected client.
	// If multiple client wants to connect, the server replaces old client with new client and sends back an error
	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0){
		error("ERROR on accept");
	}

	//Get col to sort by through 
	char colToSort[30];
	n = recv(newsockfd, colToSort, 20, 0);
	if (n < 0){
		error("ERROR reading from socket");
	}

	//Initiate pthread_mutex
	pthread_mutex_init(&counter_mutex, 0);
	pthread_mutex_init(&csv_mutex, 0);

	//Create masterList of data and csvCounter
	masterList = (DataRow***) malloc(sizeof(DataRow**) * 1500);
	csvCounter = (int*) malloc(sizeof(int) * 1);

	//Create list of threads made for each csv file
	ThreadNode* firstNode = NULL;
	ThreadNode* lastNode = NULL;
	int threadCounter = 0;

	int cont = 1;
	int prevSize = 0;
	while(cont){
		char sizeString[256];

		n = recv(newsockfd, sizeString, 10, 0);
		if (n < 0){
			error("ERROR reading from socket");
		}

		//If EOF initiated
		if(strstr(sizeString, "/EOF~") != NULL)
			//WAIT FOR ALL THREADS TO FINISH
			cont = 0;
		else{
			//get size and convert to int. Make char array of this size
			int size = atoi(sizeString);	
			char words[size];
			bzero(words,prevSize * 16);
			prevSize = size;

			//read the message inside the socket sent from client
			n = read(newsockfd,words,size);

			if (n < 0){
				error("ERROR reading from socket");
			}

			//SEND WORDS TO NEW THREAD TO SORT THROUGH
			struct sort_arg_struct args;
			args.file = words;
			args.colToSort = colToSort;


			//send back the message to client
			n = write(newsockfd,"I got your message",18);

			if (n < 0){
				error("ERROR writing to socket");
			}
		}


	}

	//Join all of this thread's children and free the nodes
	while(firstNode != NULL){
		pthread_join(*(firstNode->thread), 0);
		ThreadNode* prev = firstNode;
		firstNode = firstNode->nextNode;
		free(prev);
	}


	//When EOF is initiated by client and there are some csv entries given
	if(masterList[0] != NULL){
		//Create file for sorted csv
		char* fileName = (char*)malloc(sizeof("AllFiles-sorted-.csv") + sizeof(fileName) + sizeof(colToSort) + 1);
		strcat(fileName, "AllFiles-sorted-");
		strcat(fileName, colToSort);
		strcat(fileName, ".csv");
	
		FILE* outfp;
		outfp = fopen(fileName, "w");

		//Print header for file
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
	return 0;
}
