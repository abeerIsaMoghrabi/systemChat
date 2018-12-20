#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <fcntl.h>
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

int main(int argc , char *argv[]){

	int clientSocket;
	int portNum;
	struct sockaddr_in serverSockAddr ;
	char buff[256];
	int result;
	CheckError(argc,3,"please enter server address and port number\n");
	
 	clientSocket = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
	
	CheckError(clientSocket , 0 , "can't creat socket\n");

	serverSockAddr=createScocketAddr(argv[1],atoi(argv[2]));//2- prepare address

	CheckError(connect(clientSocket,(struct sockaddr *)&serverSockAddr,sizeof(serverSockAddr)),0,"Error on connection\n");
	
	bzero(buff,255);// 255 = number of byte in buff and char size =255
	while(1){
	        printf("message:");

		fgets(buff,255,stdin);//??
		///////////////////////////////
		int status = fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
			if (status == -1){
			  perror("calling fcntl");
			  // handle the error.  By the way, I've never seen fcntl fail in this way
		}
	/////////////////////
		if( result=write(clientSocket,buff,255)<0){
			
		}
		if( result=read(clientSocket,buff,255)>0){
			printf("message:%s\n",buff);
		}
		//block

		CheckError(result,0,"cant write\n");
	}

		

	return 0;


}

