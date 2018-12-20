#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/time.h> 
#define CLIENT_NUM 30
struct clint_str {
int id;
char name[256];
};

/**
	@function: create address object 
	@param: socket port
	@return: sockaddr_in
	@note: INADDR_ANY is address of server machine
**/
struct sockaddr_in createScocketAddr(int portNum){
	
	struct sockaddr_in serverSockAddr;
	bzero((char *) &serverSockAddr,sizeof(serverSockAddr));
	serverSockAddr.sin_port=htons(portNum);	
	serverSockAddr.sin_addr.s_addr= INADDR_ANY;
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
	@function: fill buffer of type struct clint_str with zeros
	@param1: the buffer
**/

void fillItWithZero(struct clint_str clientSockets[]){
	int i;
	for( i=0;i<CLIENT_NUM;i++){
		clientSockets[i].id=0;
		strcpy(clientSockets[i].name,"");
		
	}
	
}
/**
	@function: bind socket to specific address and  make it listen for  action
	@param1: socket port
	@param2: socket file descriptor
	@return: struct sockaddr_in
	@note: maximum it can pend 30 connection to this socket
**/
struct sockaddr_in  prepareServerSocket(int port,int serverSocket){
	

	CheckError(serverSocket , 0 , "can't creat socket\n");

	struct sockaddr_in serverSockAddr=createScocketAddr(port);

	CheckError(bind(serverSocket,(struct sockaddr *)&serverSockAddr,sizeof(serverSockAddr)),0,"Error on binding\n");

	listen(serverSocket,30);

	return 	serverSockAddr;

}
/**
	@function: handle server socket action which is adding new socket to client array
	@param1: new socket
	@param2: client socket array
**/
 void addSocketToClientArray(int new_socket,struct clint_str client_socket[]){
	
	int i;
	char buff[256];
	for (i = 0; i < CLIENT_NUM; i++) 
	{ 
		 
		if( client_socket[i].id == 0 ) 
		{ 
			client_socket[i].id = new_socket;
			recv( new_socket , buff, 256,0);

			strcpy(client_socket[i].name,buff );
			
		
			break; 
		} 
	}

}
/**
	@function: remove all sockets in set and fill the set again according to client array
	@param1: readfds
	@param2: server socket
	@param3: client socket array
	@return: max_sd
	@note:every time we clear the set and fill it again to keep uptodate with every changed happend to client array
**/
int  clearAndFillSet(fd_set *readfds,int serverSocket,struct clint_str client_socket[]){
	FD_ZERO(readfds); 
	FD_SET(serverSocket, readfds); 
	int max_sd = serverSocket; 
	int i;
	int sd;

	for(i=0;i <CLIENT_NUM;i++) 
	{ 
		
		sd = client_socket[i].id;  
		if(sd > 0){
			FD_SET( sd , readfds); 
		} 
			

		 if(sd > max_sd){   
        		max_sd = sd; 	
		}

	} 
	return max_sd;
	
}
/**
	@function: broad cast message to all client
	@param1: sender socket
	@param2: server socket
	@param3: message buffer
	
**/
void breadCastMessage(int senderSocket,struct clint_str client_socket[],char buff[]){

	int j;
	for( j=0;j<CLIENT_NUM;j++){
		if(j!=senderSocket && client_socket[j].id>0){
	
		char message[512];
		sprintf(message,"%s: %s",client_socket[senderSocket].name,buff);
		message[strlen(message)]='\0';

		 if (send(client_socket[j].id,message,strlen(message),0) < 0){ 
		 	error("ERROR writing to socket");
		 }	
			
		
		bzero(message,511);
		}
	
	}


}
/**
	@function: this function handle select it check for any action
	@param1: server socket
	@param2: server socket address
	
**/
void serverActions(int serverSocket,struct sockaddr_in serverSockAddr){
	fd_set readfds;
	int sd;
	int i;
	int new_socket;
	int max_sd;
	struct clint_str client_socket[CLIENT_NUM];
	int activity;
	fillItWithZero(client_socket);
	char buff[256];
	while(1){ 
			

		max_sd=clearAndFillSet( &readfds, serverSocket,client_socket);

		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 

		CheckError(activity , 0,"select error\n" );
		 
	
		//there are two cases 
		//1- action happend to master socket (serversocket) 
		//2- action happend to client socket 


	
		
		if (FD_ISSET(serverSocket, &readfds)) 
		{ 		socklen_t addr_size = sizeof(serverSockAddr);//????
				new_socket = accept(serverSocket, (struct sockaddr *)&serverSockAddr, &addr_size);
				CheckError(new_socket,0,"accept faild\n");
			        addSocketToClientArray(new_socket,client_socket);

		} 
		
		

		
		
		
		for (i = 0; i < CLIENT_NUM; i++) 
		{ 
			sd = client_socket[i].id; 
			
			if (FD_ISSET( sd , &readfds)) 
			{ 
			
			
				
				if (( recv( sd , buff, 256,0)) == 0) 
				{ 
					
				
					close( sd ); 
					client_socket[i].id = 0; 
				} 
				
				else
				{ 
				
					breadCastMessage(i, client_socket, buff);//send socket postion no socket id
					bzero(buff,255);
				} 
			} 
		} 
	} 

}

int main(int argc , char *argv[]){

	int serverSocket,socketToReadWrite;
	
	struct sockaddr_in serverSockAddr ;
	
	CheckError(argc , 2 , "please enter port number\n");

 	serverSocket = socket(AF_INET,SOCK_STREAM,0);//1- create socket

	serverSockAddr=prepareServerSocket(atoi(argv[1]),serverSocket);
 	
        
	serverActions(serverSocket,serverSockAddr);

	
	return 0;

}

