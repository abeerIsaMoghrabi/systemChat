//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
 #define CLIENT_NUM 30
struct clint_str {
int id;
char* name;
};
void checkError(int result , int compared , char* msg){
	if(result<compared){
		printf("%s\n",msg);
		exit(0);
		
	}
	
}
void fillItWithZero(struct clint_str clientSockets[]){
	int i;
	for( i=0;i<CLIENT_NUM;i++){
		clientSockets[i].id=0;
		clientSockets[i].name="";
		
	}
	
}
int creatSocket(){//???????????????????
	int sock=socket(AF_INET , SOCK_STREAM , 0);
	checkError(sock,0,"socket failed");
	return sock;
	
}

struct sockaddr_in prepareSocketAddress(int PORT){
	struct sockaddr_in address; 
	address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY; //machine address 
    address.sin_port = htons( PORT ); 
	return address;
}


	
int main(int argc , char *argv[]) 
{ 
	checkError(argc,2,"plz enter port number");
	int PORT=atoi(argv[1]);
	int serverSocket;
	struct sockaddr_in serverAddress;
	//////////////////////
	int activity;
	 fd_set readfds;
	struct clint_str clientSockets[CLIENT_NUM];
	int max_sd;
	int newSocket;
	int sd;
	char*name[512];
	char*buffer[256];
	int valread;
	/////////////////////////////
	fillItWithZero(clientSockets);
	serverSocket=creatSocket();//1- create socket 
    serverAddress=prepareSocketAddress(PORT);//2-prepare socket address
	checkError(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)),0,"bind faild"); //3- bind socket to that address  
	listen(serverSocket, CLIENT_NUM);//4- listen for new connection 
	
	while(1){
	 FD_ZERO(&readfds);//each turn we remove all connection form set and fill it again according to client array 
	 //and we do that to keep up to date with all added and removed connection because we know 
	 FD_SET(serverSocket, &readfds);   
     max_sd = serverSocket; 
	//add child sockets to set
int i;	
        for ( i = 0 ; i < CLIENT_NUM ; i++)   
        {   
            //socket descriptor  
            sd = clientSockets[i].id;   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd) //  
                max_sd = sd;   
        } 
         //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);	
		checkError(activity,0,"select failed");
		//action can happend to server socket and client sockets 
		 if (FD_ISSET(serverSocket, &readfds))   
        {   
			newSocket=accept(serverSocket,  (struct sockaddr *)&serverAddress, sizeof(serverAddress));
			checkError(newSocket,0,"connection failed");
			            //add new socket to array of sockets  
            for (i = 0; i < CLIENT_NUM; i++)   
            {   
                //if position is empty  
                if( clientSocket[i] == 0 )   
                {   
                    clientSocket[i].id = new_socket; 
					int leng
					if( leng=recv(clientSocket[i].id,buffer, strlen(buffer), 0)<0 )   
							{   
								perror("send");   
							} 	
					 buffer[leng] = '\0';
                    clientSocket[i].name=buffer;
                         
                    break;   
                }   
            } 
		}
		
	    for (i = 0; i < CLIENT_NUM; i++)   
        {   
            sd = clientSocket[i].id;   
			name=clientSocket[i].name;
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                     
                    close( sd );   
                    clientSocket[i].id = 0;  
					clientSocket[i].name = ""; 					
                }   
                     
                //Echo back the message that came in  
                else 
                { 
					 buffer[valread] = '\0';
                    for(int j=0;j<CLIENT_NUM;j++){
						if(i!=j){
							 if( send(clientSocket[i].id,strcat(name,buffer) , strlen(name), 0)<0 )   
							{   
								perror("send");   
							} 
						}
						
					}  
                }   
            }   
        }
		
	 
		
	}
	
	
} 
