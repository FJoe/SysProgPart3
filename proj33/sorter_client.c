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


void* threadDir(void* parameters);
void* threadFile(void* parameters);
void recursiveSearch(char* dir);
int ti = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t * tarr;

typedef struct params{
    char objectname[500];
}params;

void error(char * msg) {
    perror(msg);
    exit(1);
}

char headers[28][50] = {
    "color",
    "director_name",
    "num_critic_for_reviews",
    "duration",
    "director_facebook_likes",
    "actor_3_facebook_likes",
    "actor_2_name",
    "actor_1_facebook_likes",
    "gross",
    "genres",
    "actor_1_name",
    "movie_title",
    "num_voted_users",
    "cast_total_facebook_likes",
    "actor_3_name",
    "facenumber_in_poster",
    "plot_keywords",
    "movie_imdb_link",
    "num_user_for_reviews",
    "language","country",
    "content_rating",
    "budget",
    "title_year",
    "actor_2_facebook_likes",
    "imdb_score",
    "aspect_ratio",
    "movie_facebook_likes"
};

void* threadFile(void* parameters){
    params *input = (params*)parameters;
    char *fill = (char*)malloc(sizeof(char*)*500);
    strcpy(fill, input->objectname);
    //printf("SORTING CSV:%s\n",input->filename);
    //printf("SORTING CSV COLNAME:%s\n",input->colname);
    free(fill);
    //free(coll);
    //free(outt);
    //printf("Row#:%d\n",rownum);
    pthread_exit(NULL);
}

void* threadDir(void* parameters){
    params *input = (params*)parameters;
    char *dirn = (char*)malloc(sizeof(char*)*500);
    printf("dirname: %s\n", input->objectname);
    strcpy(dirn, input->objectname);
    // printf("RECURSIVE DIR:%s\n",dirn);
    recursiveSearch(dirn);
    
    free(dirn);
    pthread_exit(NULL);
}

int iscolumn(char* col){
    int j;
    for(j = 0; j < 28; j++){
        if(strcmp(col,headers[j])==0){
            return j;
        }
    }
    return -1;
    
}

void recursiveSearch(char* dir){
    //printf("%s is dir name\n", dir);
    DIR * dp = opendir(dir);
    
    if(!dp){
        printf("\nNot a directory!\n");
        printf("\n%s\n",dir);
        exit(0);
    }
    struct dirent * ptr;
    while((ptr = readdir(dp))){
        if(ptr->d_type == DT_REG){
            /////// CHECK IF ITS NOT SORTED -- this causes infinite recursion
            if(strcmp(&(ptr->d_name[strlen(ptr->d_name)-4]), ".csv") == 0 && !strstr(ptr->d_name, "sorted")){
                char * new_name;
                new_name = (char *)malloc(strlen(dir)+strlen(ptr->d_name)+2);
                strcpy(new_name, dir);
                strcat(new_name, "/");
                strcat(new_name, ptr->d_name);
                strcat(new_name, "\0");
                
                params *temp = malloc(sizeof(params));
                strcpy(temp->objectname, new_name);
                printf("New CSV Thread! dir = %s\n", new_name);
                pthread_create(&tarr[ti], NULL, threadFile, temp);
                ti+=1;
            }
        }else if (ptr->d_type == DT_DIR) {
            if(strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, "..")){
                //printf("%s is a directory\n", ptr->d_name);
                char * new_dir;
                int status;
                printf("\nCurrent dir: %s\n", dir);
                new_dir = (char *)malloc(strlen(dir)+strlen(ptr->d_name)+2);
                strcpy(new_dir, dir);
                strcat(new_dir, "/");
                strcat(new_dir, ptr->d_name);
                strcat(new_dir, "\0");
                params *temp = malloc(sizeof(params));
                strcpy(temp->objectname, new_dir);
                //-------------------------------------------call to thread-----
                //printf("RECURSING CSV BEFORE:%s\n",temp2->dirname);
                printf("New DIR Thread! dir = %s\n", new_dir);
                pthread_create(&tarr[ti], NULL, threadDir, temp);
                free(new_dir);
                ti+=1;
            }
        }
    }
    return;
}

int main(int argc, char * argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;
    char input[250];
    char output[250];
    char col[250];
    int port;
    char * hostname;
    
    char buffer[256];
    
    //Check to see args numbers are correct
    if(argc > 2 && argc < 10){
        // look at the column name
        if(strcmp(argv[1],"-c") == 0){
            if(iscolumn(argv[2]) >= 0){
                strcpy(col, argv[2]);
            }
            else{
                printf("Not a column in table!");
                return 0;
            }
            //work on the other args
            if(strcmp("-h", argv[3]) != 0 || strcmp("-p", argv[5]) != 0 ){
                printf("wrong order of flags");
                return 0;
            }
            //store args as local vars
            hostname = (char *)malloc(sizeof(argv[4]));
            strcpy(hostname, argv[4]);
            port = atoi(argv[6]);
            if(argc >7 && argc < 11){
                if(strcmp(argv[7],"-d") == 0){
                    strcpy(input, argv[8]);
                }
                if(strcmp(argv[9],"-o") == 0){
                    strcpy(output, argv[10]);
                }
            }
            else if(argc >7 && argc < 9){
                if(strcmp(argv[7],"-d") == 0){
                    strcpy(input, argv[8]);
                    strcpy(output, ".");
                }
                if(strcmp(argv[7],"-o") == 0){
                    strcpy(output, argv[8]);
                    strcpy(input, ".");
                }
            }else{
                strcpy(input, ".");
                strcpy(output, ".");
            }
            
            
            
        }
        else{
            printf("No column flag");
            return 0;
        }
        
    }else{
        printf("Improper number of arguments!");
        return 0;
    }
    
    
    //set portno. to specified between 2000 and 65655(?)
    portno = atoi(argv[2]);
    
    //tests if portno is valid
    if(portno < 2000 || portno > 65655){
        error("Port number must be between 2000 and 65655!");
    }
    //create a socket(int domain, int type, int protocol)
    //domain is IPv4
    //type is TCP
    //protocol is 0 for default
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //check to see socket was correctly opened
    if (sockfd < 0) {
        error("ERROR opening socekt");
    }
    
    //host name: "visitor.cs.rutgers.edu" = "visitor"
    server = gethostbyname(argv[1]);
    
    //check if hostname exists
    if (server == NULL) {
        fprintf(stderr, "Error, no such host");
        return (0);
    }
    
    //clears the memory and initializes to 0s
    bzero((char * ) & serv_addr, sizeof(serv_addr));
    
    //set domain
    serv_addr.sin_family = AF_INET;
    
    //copy the bytesequency from src to destination
    bcopy((char * ) server -> h_addr, (char * ) & serv_addr.sin_addr.s_addr, server -> h_length);
    
    //assign portno
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd, & serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    
    printf("Please enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    
    //send message to server, not using send(...)
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing to socket");
    }
    
    //clean the string
    bzero(buffer, 256);
    
    //read response of the server
    n = read(sockfd, buffer, 255);
    if (n < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n", buffer);
    
    return 0;
}
