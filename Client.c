#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <fcntl.h>
#include <pthread.h>
#define CLIENT_NUM  30
struct sockaddr_in createScocketAddr(char* ipAddr,int portNum){
	
	struct sockaddr_in serverSockAddr;
	bzero((char *) &serverSockAddr,sizeof(serverSockAddr));//???
	serverSockAddr.sin_port=htons(portNum);	
	serverSockAddr.sin_addr.s_addr=inet_addr(ipAddr) ;// address of server machine
	serverSockAddr.sin_family=AF_INET;
	return serverSockAddr;
}

void CheckError(int value , int code , char *msg){
	if(value < code ){
		 perror(msg);
    		 exit(1);
	}
}
void *sendMessage(void *sock_desc) {
    //Send some data
 char buff[256];
	
    while (1) {
       bzero(buff,255);// 255 = number of byte in buff and char size =255
	
        printf("%s","> ");
       fgets(buff,255,stdin);//??
        
	strcat(buff,"\0");
        if (send(*((int *) sock_desc), buff, strlen(buff) + 1, 0) < 0) {
            puts("Send failed");
        }
    }
}

void *receiveMessage(void *sock_desc) {
char buff[256];
	bzero(buff,255);// 255 = number of byte in buff and char size =255
    while (1) {
        
        if (recv(*((int *) sock_desc), buff,strlen(buff) + 1, 0) < 0) {
            //puts("recv failed");

        }
	else{
	printf("%s", buff);}
   }
	//printf("not block\n");
        //Receive a reply from the server
        
 
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

	//CheckError(connect(clientSocket,(struct sockaddr *)&serverSockAddr,sizeof(serverSockAddr)),0,"Error on connection\n");
	connect(clientSocket,(struct sockaddr *)&serverSockAddr,sizeof(serverSockAddr));
	//bzero(buff,255);// 255 = number of byte in buff and char size =255
	char message[256];
	sprintf(message,"%s\0",argv[3]);
	
printf("%s\n",message);
	 if (send(clientSocket,message,strlen(message),0) < 0){ 
						 error("ERROR writing to socket");
						 }

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
