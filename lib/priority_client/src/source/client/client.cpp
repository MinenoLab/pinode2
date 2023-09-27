#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#include <config.h>


#include "PriorityPacket.h"
#include "Agent_CL.h"

#define TIMEOUT 30

#define ORDER_PROC 2
#define FILE_SEND_START 3
#define FILE_SEND_CONTINUE 4

#define DEVID_SIZE 2

using namespace std;

int sock;

typedef struct Buffer
{
	size_t size;
	unsigned char data[SENDBUFSIZE];
} ucbuffer;

void tcp_close()
{
  printf("do close \n");
  close(sock);
}

int tcp_connect(const char *hostname, int port)
{
  int sock0;
  int err;
  struct sockaddr_in addr;
  struct hostent *host;

  sock0 = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  //host = gethostbyname(hostname);
  addr.sin_addr.s_addr = inet_addr(hostname);

  struct timeval tv;
  tv.tv_sec = TIMEOUT;
  tv.tv_usec = 0;
  setsockopt(sock0, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));

  int flag = 1;
  setsockopt(sock0, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );

  if (addr.sin_addr.s_addr == 0xffffffff) {
    //struct hostent *host;

    host = gethostbyname(hostname);
    if (host == NULL) {
      return 1;
    }
    addr.sin_addr.s_addr = *(unsigned int *)host->h_addr_list[0];
  }
  

  err = connect(sock0, (struct sockaddr *)&addr, sizeof addr);
  if(err < 0){
    return -1;
  }else{
    return sock0;
  }
}


int main()
{
	string filepath = "nodata";
	unsigned char recvbuf[3];
	ucbuffer sendbuf;
	size_t capacity = RECVBUFSIZE;

	bool loop_flag;
	bool rm_flag=0;
	unsigned int state = 2;
	unsigned int datatype;
	unsigned int datasize;
	int res;
    int retsize;
    int packetsize;
    
    FILE * fp;
    OrderInfo order_info;
    FileInfo file_info;


	PriorityPacket * pripacket = new PriorityPacket;
	Agent * agent = new Agent;
	agent->initAgent("./conf/client_prioritydata.csv");
	agent->setMode(FILO_RR);
	
	cout << "BUFSIZE = " << RECVBUFSIZE << endl;

    sock = tcp_connect(SERVER_IP, PORT);

	if(sock == -1){
	    printf("ERROR cannnot connect\n");
	    close(sock);
		delete pripacket;
		delete agent;
	    return -1;
	}

	//start communication
	
	//send deviceid
	memset(sendbuf.data, 0, capacity);
	sendbuf.data[0] = (unsigned char)MSG_DEVID;
	sendbuf.data[1] = (unsigned char)((uint32_t)DEVICE_ID % 256);
	sendbuf.size = DEVID_SIZE;

    
	res = send(sock, sendbuf.data, DEVID_SIZE, 0);
	if (res <= 0) {
        cout << "send() failed." << endl;
        tcp_close();
		delete pripacket;
		delete agent;
        return -1;
    }
    

    while(1){
	    memset(recvbuf, 0, sizeof(recvbuf));
	    packetsize = recv(sock, recvbuf, sizeof(recvbuf), 0);
		    
		if (packetsize < 0) {
			cout << "recv() failed." << endl;
			tcp_close();
			delete pripacket;
			delete agent;
			return -1;
		} else if(packetsize == 0){
		    cout << "connection closed by foreign host." << endl;
		    tcp_close();
			delete pripacket;
			delete agent;
		    return -1;
		}

		datatype = pripacket->getDataType(recvbuf);

    	loop_flag = 1;
    	while(loop_flag){
    		switch(state){
    			case ORDER_PROC:
    				cout << "ORDER_PROC" << endl; 
					if(datatype == MSG_ORDER){//order message receve
						datatype = pripacket->orderInterpretation(recvbuf,&order_info);

					    cout << "show order info in main" << endl;
					  	pripacket->showOrderInfo(order_info);

					  	agent->updateNext(order_info);
					    file_info = agent->getNextInfo(&filepath);

					  	cout << "close flag is " << order_info.close << endl;
					    if(order_info.close){
					      tcp_close();
						  delete pripacket;
						  delete agent;
					      return 0;
					    }
						
						pripacket->showFileInfo(file_info);
						cout << "filepath = " << filepath << endl;

						memset(sendbuf.data, 0, sendbuf.size);
						sendbuf.size = pripacket->getMETAdata(file_info,sendbuf.data);

						res = send(sock, sendbuf.data, sendbuf.size, 0);
						if (res <= 0) {
				        	cout << "send() failed." << endl;
				        	tcp_close();

						  	delete pripacket;
						  	delete agent;
				        	return -1;
				    	}

				    	if(file_info.timestamp != FILEINFO_INITTIMESTAMP){
				    		state = FILE_SEND_START;
				    		loop_flag = 0;
				    	}else{
				    		state = ORDER_PROC;
				    		agent->updateExist();
				    		loop_flag = 0;
				    	}


					}else{//other message receve or receve illegal messace
						cout << "receve illegal messace" << endl;
						tcp_close();
						delete pripacket;
						delete agent;
						return -1;
					}


	    			break;
	    		case FILE_SEND_START:
	    			cout << "FILE_SEND_START" << endl; 
					if(datatype == MSG_ACK){//ACK receve
						if ((fp = fopen(filepath.c_str(), "rb")) == NULL) {
							cout << "cannot open " << filepath.c_str() << endl;
							tcp_close();
						  	delete pripacket;
						  	delete agent;
							return -1;
						}else{
							cout << filepath << " opend" << endl;
						}

						memset(sendbuf.data, 0, sendbuf.size);
						sendbuf.size = fread(sendbuf.data+3,1,capacity-3,fp);
						if(sendbuf.size <= 0){
							cout << "fread() faild." << endl;
							tcp_close();
							fclose(fp);
						  	delete pripacket;
						  	delete agent;
							return -1;
						}
						datasize = sendbuf.size;

						sendbuf.size += pripacket->getFILEdataHeader(sendbuf.size,sendbuf.data);
						res = send(sock, sendbuf.data, sendbuf.size, 0);
						if (res <= 0) {
							cout << "send() failed." << endl;
							tcp_close();
							fclose(fp);
						  	delete pripacket;
						  	delete agent;
							return -1;
				    	}

						
						if(file_info.filesize <= datasize){//send all file
							fclose(fp);
							rm_flag=1;
							cout << "send all file " << endl;
						  	
							state = ORDER_PROC;
							agent->updateExist();
							loop_flag = 0;
						}else{
							state = FILE_SEND_CONTINUE;
							loop_flag = 0;

							memset(sendbuf.data, 0, sendbuf.size);
							sendbuf.size = fread(sendbuf.data+3,1,capacity-3,fp);
							if(sendbuf.size <= 0){
								cout << "fread() faild." << endl;
								tcp_close();
								fclose(fp);
						  		delete pripacket;
						  		delete agent;
								return -1;
							}
							datasize += sendbuf.size;

							sendbuf.size += pripacket->getFILEdataHeader(sendbuf.size,sendbuf.data);
						}


					}else if(datatype == MSG_ORDER){//order message receve
						state = ORDER_PROC;
						fclose(fp);
						agent->updateExist();
						loop_flag = 1;
					}else{//other message receve or receve illegal messace
						cout << "receve illegal messace" << endl;
						tcp_close();
						delete pripacket;
						delete agent;
						return -1;
					}


    				break;
	    		case FILE_SEND_CONTINUE:
	    			//cout << "FILE_SEND_CONTINUE" << endl; 
					if(datatype == MSG_ACK){//ACK receve
						
						res = send(sock, sendbuf.data, sendbuf.size, 0);
						if (res <= 0) {
							cout << "send() failed." << endl;
							tcp_close();
							fclose(fp);
						  	delete pripacket;
						  	delete agent;
							return -1;
				    	}
						
						if(file_info.filesize <= datasize){//send all file
							fclose(fp);
							rm_flag=1;
						  	
							state = ORDER_PROC;
							agent->updateExist();
							loop_flag = 0;
						}else{
							state = FILE_SEND_CONTINUE;
							loop_flag = 0;

							memset(sendbuf.data, 0, sendbuf.size);
							sendbuf.size = fread(sendbuf.data+3,1,capacity-3,fp);
							if(sendbuf.size <= 0){
								cout << "fread() faild." << endl;
								tcp_close();
								fclose(fp);
						  		delete pripacket;
						  		delete agent;
								return -1;
							}
							datasize += sendbuf.size;

							sendbuf.size += pripacket->getFILEdataHeader(sendbuf.size,sendbuf.data);
						}


					}else if(datatype == MSG_ORDER){//order message receve
						state = ORDER_PROC;
						fclose(fp);
						agent->updateExist();
						loop_flag = 1;
					}else{//other message receve or receve illegal messace
						cout << "receve illegal messace" << endl;
						fclose(fp);
						tcp_close();
						delete pripacket;
						delete agent;
						return -1;
					}

	    			break;
    			default:
	    			cout << "illegal state : " << state << endl;
	    			tcp_close();
	    			delete pripacket;
	    			delete agent;
	    			return -1;
    		}

			

    	}

    }

    

	tcp_close();

	return 0;
}
