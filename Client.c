#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/time.h> 
#include <fcntl.h>
#include <pthread.h>
#define CLIENT_NUM  30

/**
	@function: create address object 
	@param: socket port
	@return: sockaddr_in
	@note: inet_addr used to change string to number with dots (ip format)
**/
struct sockaddr_in createScocketAddr(char* ipAddr,int portNum){
	
	struct sockaddr_in serverSockAddr;
	bzero((char *) &serverSockAddr,sizeof(serverSockAddr));
	serverSockAddr.sin_port=htons(portNum);	
	serverSockAddr.sin_addr.s_addr=inet_addr(ipAddr) ;
	serverSockAddr.sin_family=AF_INET;
	return serverSockAddr;
}
/**
	@function: check for error and exit if happend ti compare two value and if the first value less than the socend it exit 
	@param1: the value
	@param2: the code
**/
void CheckError(int value , int code , char *msg){
	if(value < code ){
		 perror(msg);
    		 exit(1);
	}
}
/**
	@function: thread to send massages
	@param1: thread id which is also socket number
	
**/
void *sendMessage(void *sock_id) {
    
	char buff[256];

	while (1) {
	bzero(buff,255);// 255 = number of byte in buff and char size =255

	printf("%s","> ");
	fgets(buff,255,stdin);

	strcat(buff,"\0");
	if (send(*((int *) sock_id), buff, strlen(buff) + 1, 0) < 0) {
	    puts("Send failed");
	}
	}
}
/**
	@function: thread to recive massages
	@param1: thread id which is also socket number
	
**/

void *receiveMessage(void *sock_id) {
	char buff[256];
	
	while (1) {
	bzero(buff,255);
	if (recv(*((int *) sock_id), buff,strlen(buff) + 1, 0) < 0) {
	   

	}
	else{
	printf("%s", buff);}
	}
	
        
 
}
/**
	@function: send client name to server
	@param1: client name (char pointer)
	@param2: socket to send through
**/
void sendUserName(char*name,int clientSocket){

	char message[256];
	sprintf(message,"%s\0",name);
	printf("%s\n",message);
	CheckError(send(clientSocket,message,strlen(message),0) , 0,"ERROR writing to socket\n");

}
int main(int argc , char *argv[]){

	int clientSocket;
	int portNum;
	struct sockaddr_in serverSockAddr ;
	char buff[256];
	int result;
	CheckError(argc,4,"please enter server address and port number\n");
	
 	clientSocket = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
	
	CheckError(clientSocket , 0 , "can't creat socket\n");

	serverSockAddr=createScocketAddr(argv[1],atoi(argv[2]));//2- prepare address

	
	connect(clientSocket,(struct sockaddr *)&serverSockAddr,sizeof(serverSockAddr));
	sendUserName(argv[3],clientSocket);
	

	int *new_sock;
	new_sock = malloc(1);
	*new_sock = clientSocket;
	pthread_t send_thread, receive_thread;
	
	pthread_create(&send_thread, NULL, sendMessage, (void *) new_sock);
     	pthread_create(&receive_thread, NULL, receiveMessage, (void *) new_sock);

    	pthread_join(receive_thread, NULL);
    	pthread_join(send_thread, NULL);
	
    	 

    	 close(clientSocket);
		

	return 0;


}

