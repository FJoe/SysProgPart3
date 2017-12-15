Run client: ./client hostname portnumber Run server: ./server portnumber

EXAMPLE: ./server 5000 ./client singleton.cs.rutgers.edu 5000

Linebreaks: /~ End of stream: /EOS~

=====Design===== 
The overall design for our program is the client traverses the directories given and sends each csv file to the server. The server waits until the client tells the server to sends back the sorted data, and the server responds with the all-sorted-csv file.
sorter_server.c
---------------
Method 1: 
void * connectionHandler(void * socket);

Purpose: Handles each individual connection in a thread. The method will do three recv(), the first one reads the column to sort, the second reads the filesize, the third reads the file. Once we have all three piece of data, we call sortIndiv() [from previous assignmnet] and this will sort the csv file first. At the same time, this puts the csv file into a master csv file list and will do a final mergesort at the end.

If the user sends in a string with "/EOF~", our if statement will check and if this occurs, we call pthread_join on all threads to ensure it finishes. This is when the final mergesort occurs.

Afterwards, we send this newly sorted list back to client.

Parameters: socket - the socket with the connection to the client

Method 2(Main):
main();

the main method will create a new thread everytime we accept a connection from the client. The main method calls connectionHandler in pthread_create(...);

sorter_client.c
---------------
Method 1:
recursiveSearch(char * dir);

recurses through the given directory from userinput in dir. For every dir and csv file it finds, it creates a new thread. A thread for a csv file calls sendCSV(...)

Parameters: dir - the directory to traverse

Method 2:
sendCSV( char * fileName);

the fileName is the csv file we want to send to the server. The method will connect to the server and send the file with information regarding the col-to-sort, sizeFile, and file

Method 3(Main):
main()

The main method calls upon recursiveSearch(...) and will wait for the server to send back the final sorted csv file. The main will then read that bytestream and save the file as csv into the given output directory.

=====Assumptions===== 
We assumed that the file number will not be greater than 1024, which means bigger data may have undefined behavior.

=====Difficulties===== 
It was hard to debug the programs as both program required each other to work completely. Therefore, testing had to be done over both client and server at the same time and became harder as the project grew bigger.

Sending data over socket was difficult as this was the first time any of the group members had to deal with networking. In large data set, it was seasier for an error to occur. We solved this by ensuring that we knew the bytestream prior to reading, thus we knew exactly how many bytes we had to read.

The socket conflicted with one another and we could not figure out how this happened. More specifically, the data each socket read using recv() was different everytime we sent the same data over. Therefore, the reading of the bytestream gave different result each time. As a result, we could not properly configure our metadata in the files and subsequent failure for size allocation for the file. 

=====Finding and errors=====
Actual output, the csvfile might be missing some lines as we could not properly initialize the data in a multithreaded environment. We did not know how this occured. Our hypothesis was that reading the bytestream on recv() was inaccurately calculated, which read incorrect data.

EXAMPLE:
We had a variable called sizeString that takes in metadata for size of file from the client. When we wanted to recv() the length of the file, we somehow read the headers of the actual file instead of a integer that corresponded to length of file. 


=====Testing Procedures===== 
We broke the testing into multiple pieces to ensure each part worked independently.

1)Created to socket and client program to send basic single line message to ensure a connection was established
2)Sent single file csv to server to ensure they transferred correctly
3)Multi-threaded the Server and Client to ensure that for each file they encountered, they created a new thread with new connection. For server, it was a new thread for each accept() connection

=====How to use our code=====
1)Unpack tar file: tar xvf Project3.tar
2)Run "make all" on terminal
3)Type: ./sorter_server -p <portno>
  Type: ./sorter_client -c <col-to-sort> -h <hostname> -p <portno> -d <input directory> -o <output directory>

-d and -o are optional flags


