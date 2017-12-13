#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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


int iscolumn(char* col){
    int j;
    for(j = 0; j < 28; j++){
        if(strcmp(col,headers[j])==0){
            return j;
        }
    }
    return -1;
    
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
