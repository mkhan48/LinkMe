/* 
	Mohammad Saad Khan
	TCP-Server for LinkMe Android Application

*/


#include "server.h"
#include <sys/stat.h>


int main(int argc, char** argv){

	/* Variable Declaration */

	int port, serverSocket, clientSocket;					/* variables representing port, server, and client */
	int recvSize, sendSize;									/* variables to upadte with expected size for messages */
	char *user, *pass;										/* username and password */
	char *request; 											/* request to "Authorize" from Client */
	char *reply; 											/* Authenication Process result for Client */
	char *received; 										/* concatenated username and password from Client */
	char *verify;											/* concatenated username and password to be compared */
	char *filename;											/* filename to be sent to Client */
	char* auth="Authorize";									/* expected authorization request from Client */
	FILE *fp;												/* variable to create file */
	char* file_read_buffer;									/* variable to hold reading buffer */
	struct stat temp;										/* varible to get information of the received file */

	unsigned int clientLength;
	struct sockaddr_in server_struct; /* Local address */
	struct sockaddr_in client_struct; /* Client address */

	/* Check for correct number of arguments */
	if(argc!=5)
	{
		printf("\n\
			Error:		Incorrect number of arguments.\n\
			Usage:		./letmein-server <port> <username> <password> <keep-alive(0 or 1)>\n \
			Example:	./letmein-server 5000 user pass 0\n\
		\n");
		exit(1);
	}


	/* Assign arguments to variables */

	port=atoi(argv[1]);									
	user=argv[2];
	pass=argv[3];
	if(strlen(user)>15 || strlen(pass)>15)
	{	
		printf("\n			Error:		Username or password too long.\n\n");
		exit(1);
	}
	printf("\nStep 1: \nServer started on Port:%d with Username:%s and Password:%s\n\n",port,user,pass);
	


	/* Start server: Create socket */

	memset(&server_struct, 0, sizeof(server_struct));							/* sets bytes of memory for Server socket */
	server_struct.sin_family = AF_INET;
	//server_struct.sin_addr.s_addr = htonl(INADDR_ANY);						/* set the IP Address of Socket for local emulator
	server_struct.sin_addr.s_addr = inet_addr("192.168.0.105");					/* set the IP Address to current WiFi IP Address. */
																				/* check Network Settings for it. */
	server_struct.sin_port = htons(port);

	if(((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) ||						/* creates socket */
	  (bind(serverSocket, (struct sockaddr *) &server_struct, sizeof(server_struct)) < 0) || 	/* binds socket to the allocated memory */
	  (listen(serverSocket, 1) < 0))															/* start listening to socket */
	{	
		printf("\nError:		Failed to create socket. Please try again.\n\n");
		exit(1);
	}

	/* ********************************************** */
	/* 		Initialization Complete		  */
	/* ********************************************** */


	/* ********************************************** */
	/* 		Handle Client Authorization	  */
	/* ********************************************** */


	/* Keep server alive */

	do{

		/* Step 1: Wait for Client to Connect */

		printf("Waiting for client to connect...\n\n");
		clientLength = sizeof(client_struct);
		if ((clientSocket = accept(serverSocket, (struct sockaddr *) &client_struct, &clientLength)) < 0)		/* accept connection to Client */
		{
			printf("\nError:		Error in receiving\n\n");
			close(serverSocket);
			exit(1);
		}

		request = (char*) malloc(strlen(auth));										/* allocate space for request from Client */
		recvSize=recv(clientSocket, request, 255, 0);								/* receive request from Client */
		printf("\nStep 2: \nRequest from Client: %s\n", request);


		/* Step 2: Authenticate */

		received = (char*)malloc(255);												/* username and password from Client */
		if((recvSize=recv(clientSocket, received, 255, 0)) > 0 && recvSize<70) printf("\nStep 3: \nUsername+PW Received: %s\n", received);
		else
		{
			if(recvSize==0) printf("\nError:		Connection closed\n\n");
			else if(recvSize>70) printf("\nError:		Excess data received %s\n\n", received);
			else printf("\nError:		Error in receiving\n\n");
			exit(1);
		}

		verify = (char*)malloc(strlen((char*)user)+strlen((char*)pass));			/* username+password for strcmp */
		strcpy(verify, user);														/* username+password for strcmp */
		strcat(verify, (char*)pass);												/* username+password for strcmp */
		strcat(verify, "\n");

		// printf("%s\n", verify);
		// printf("received:%d\nverify:%d", strlen(received), strlen(verify));
		if(!strcmp(verify, received))												/* checks if username and password match */
		{
			printf("\nStep 4: \nUser passed authentication\n\n");
			reply="Pass";
		}
		else
		{
			printf("\nStep 4: \nUser failed authentication\n\n");
			reply="Fail";
		}


		sendSize=strlen(reply);
		if (send(clientSocket, reply, sendSize, 0) != sendSize)						/* send authentication results to Client */
		{
			printf("\nError:		Error in sending\n\n");
			close(clientSocket);
			close(serverSocket);
			exit(1);
		}

		/* Step 3: Recieving the File */

		filename = (char*)malloc(255);												/* filename to be sent */

		if((recvSize=recv(clientSocket, filename, 255, 0)) > 0 && recvSize<70) printf("\nStep 5: \nFilename Received: %s", filename);
		else
		{
			if(recvSize==0) printf("\nError:		Connection closed\n\n");
			else if(recvSize>70) printf("\nError:		Excess data received %s\n\n", filename);
			else printf("\nError:		Error in receiving\n\n");
			exit(1);
		}
		
		int filename_length = strlen(filename);
		filename[filename_length-1] = '\0';

		fp = fopen(filename, "rb");													/* opening the file; "rb" = readBuffer */
		
		if(fp == NULL)																/* check if file exists */
		{
			
			printf("\n File Not Found \n");
			
		}
		else
		{
			stat(filename, &temp);													/* determine information about file and store in "temp" */
			printf("The size of the file is %d \n\n\n", (int)temp.st_size);
			
			file_read_buffer = malloc(temp.st_size+20);								/* variable for contents of file */
			int bytesRead = fread(file_read_buffer, 1, temp.st_size, fp);			/* store contents of file in "file_read_buffer" */
			
			/* Sending the contents of the file to client */
			if (send(clientSocket, file_read_buffer, bytesRead, 0) != bytesRead)	/* send contents of file to Client */
			{
				printf("\nError:		Error in sending\n\n");
				close(clientSocket);
				close(serverSocket);
				exit(1);
			}
			printf("Step 6: \nThe file \"%s\" was sent. \n\n", filename);
		}
		fclose(fp);

		/* Step 4: Close client socket to accept new connections and free the buffers that have been used */
		close(clientSocket);
		free(request);
		free(verify);
		free(received);
		free(filename);
		free(file_read_buffer);

	} while(argv[4][0]-48);
	printf("END OF CONNECTION\n");


	/* ********************************************** */
	/* 		Clients Handled			  */
	/* ********************************************** */
	return 0;



}