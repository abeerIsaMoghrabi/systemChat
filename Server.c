#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#define CLIENT_NUM 30
struct clint_str {
int id;
char name[256];
};


struct sockaddr_in createScocketAddr(int portNum){
	
	struct sockaddr_in serverSockAddr;
	bzero((char *) &serverSockAddr,sizeof(serverSockAddr));//???
	serverSockAddr.sin_port=htons(portNum);	
	serverSockAddr.sin_addr.s_addr= INADDR_ANY;// address of server machine
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

	int serverSocket,socketToReadWrite;
	
	struct sockaddr_in serverSockAddr , clientSockAddr;
	char buff[256];

///////////////////////////////////////////////
	fd_set readfds;
	int sd;
	int i;
	int new_socket;
	int max_sd;
	struct clint_str client_socket[CLIENT_NUM];
	for(i=0;i<CLIENT_NUM;i++){
		client_socket[i].id=0;
		client_socket[i].name[0]='\0';
	}
	int activity;
	int count=0;
//////////////////////////////////////////////////

	CheckError(argc , 2 , "please enter port number\n");

 	serverSocket = socket(AF_INET,SOCK_STREAM,0);//1- create socket

	
	CheckError(serverSocket , 0 , "can't creat socket\n");

	serverSockAddr=createScocketAddr(atoi(argv[1]));//2- prepare address

	CheckError(bind(serverSocket,(struct sockaddr *)&serverSockAddr,sizeof(serverSockAddr)),0,"Error on binding\n");//3- bind server address to that socket

	listen(serverSocket,5);//4- listen for connection

///////////////////////////////////////////////////////
	while(1) 
	{ 
		//every time we clear the set and fill it again to keep uptodate with closed and opened socket 
		
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(serverSocket, &readfds); 
		max_sd = serverSocket; 
			
			
	//add child sockets to set 
		for(i=0;i <CLIENT_NUM;i++) //this for loop to fill the set 
		{ 
			//socket descriptor 
			sd = client_socket[i].id; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 

			 if(sd > max_sd)   
                		max_sd = sd; 	
		
		
		} 
	
	
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) ) 
		{ 
			printf("select error"); 
		} 
		
		//there are two cases 
		//1- action happend to master socket (serversocket) 
		//2- action happend to client socket 


		
		//1- If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(serverSocket, &readfds)) 
		{ socklen_t addr_size = sizeof(clientSockAddr);//????
			 	if ((new_socket = accept(serverSocket, 
					(struct sockaddr *)&serverSockAddr, &addr_size))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
				
			//add new socket to array of sockets 
			for (i = 0; i < CLIENT_NUM; i++) 
			{ 
				//if position is empty 
				if( client_socket[i].id == 0 ) 
				{ 
					client_socket[i].id = new_socket;
					recv( new_socket , buff, 256,0);
printf("buffer=%s\n",buff);
 					strcpy(client_socket[i].name,buff );
					//count++;
					printf("socket id =%d , name=%s\n",client_socket[i].id,client_socket[i].name);
						
					break; 
				} 
			}
		} 
			
			
	
			
			
		//2- else its some IO operation on some other socket (clientsocket+)
		for (i = 0; i < CLIENT_NUM; i++) 
		{ 
			sd = client_socket[i].id; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
				//when read return 0 that mean close connction
				//this statment read and if the result is 0 then close the connection but 
				//else it means the client connected to this socket send message to server
				
				
				if (( recv( sd , buff, 256,0)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
					
					close( sd ); 
					client_socket[i].id = 0; 
				} 
					
				//Echo back the message that came in 
				else
				{ 
					
					// server read message from one of the client so now server want to broadcast the message 
					int j;
					for( j=0;j<CLIENT_NUM;j++){
						if(j!=i && client_socket[j].id>0){
						
						char message[512];
						sprintf(message,"%s: %s",client_socket[i].name,buff);
						message[strlen(message)]='\0';
						//int n = send(client_socket[j],buff,strlen(buff),0);

						 if (send(client_socket[j].id,message,strlen(message),0) < 0){ 
						 error("ERROR writing to socket");
						 }	
							//printf("message:%s\n",buff);
							bzero(buff,255);
							bzero(message,511);
						}
						
					}//for
				} //else
			} //if
		} 
	} 
////////////////////////////////////////////////////////

	/*socklen_t cli_addr_size = sizeof(clientSockAddr);//????
	printf("Runing....\n");
	
	socketToReadWrite=accept(serverSocket,(struct sockaddr *)&clientSockAddr,&cli_addr_size);//5- accept connection (block)

	CheckError(socketToReadWrite,0,"Server :cant accept\n");

	bzero(buff,255);
        int result=read(socketToReadWrite,buff,255);//6- read fron socket (block)

	CheckError(result,0,"cant read\n");

	printf("message:%s\n",buff);*/
		
	



}

