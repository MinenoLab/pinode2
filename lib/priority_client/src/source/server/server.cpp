#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>

#include "config.h"

#include "Agent_SV.h"
#include "PriorityPacket.h"
#include "server_state.h"
#include "measure_module.h"
#include "upload_module.h"

#define SOCK_TIMEOUT 30
#define SOCK_MAX 255
#define MAX_EVENTS 1

#define ACKSIZE 1
#define ORDER_SIZE 3

#define LISTEN_TIMEOUT 60


using namespace std;

int sock_listen;
int fds[SOCK_MAX];

typedef struct Buffer
{
  size_t size;
  unsigned char data[RECVBUFSIZE];
} ucbuffer;

void tcp_close(){
  printf("do close \n");
  close(sock_listen);
  for(int i=0;i<SOCK_MAX;i++){
    if(fds[i] == 0){
      close(fds[i]);
      fds[i]=0;  
    }
  }
}

void sock_close(int sock, int fdsnum){
  printf("close %d\n",sock);
  close(sock);
  fds[fdsnum]=0;
}

int tcp_listen(int port){
  int sock;
  struct sockaddr_in addr;
  int yes = 1;
  int err;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) {
    perror("ERROR socket");
    return 1;
  }

  struct timeval tv;
  tv.tv_sec = SOCK_TIMEOUT;
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));

  bzero((char *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  err =  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if(err < 0) {
    perror("ERROR bind");
    printf( "bind error\n");
    printf("print error string by strerror: %s\n", strerror(errno));
    close(sock);
    exit(1);
  }

  err = listen(sock, 5);
  if(err < 0) {
    perror("ERROR listen");
    printf( "bind error\n");
    printf("print error string by strerror: %s\n", strerror(errno));
    close(sock);
    exit(1);
  }

  return sock;
}


int pri_server(int sock,Agent * agent,MeasureModule * measure, int fdsnum){
  bool loop_flag;
  unsigned int state = RECV_DEVID;
  unsigned int device_id=0;
  unsigned int datatype;

  ucbuffer recvbuf;
  unsigned char orderpacket[3];
  unsigned char ackpacket = (unsigned char)MSG_ACK;
  size_t capacity = RECVBUFSIZE;

  int retsize;
  unsigned int packetsize = 0;
  unsigned int writesize = 0;
  unsigned int datasize;
  int res;
  string tmp_header;
  string savefile;
  string sql_str;
  FILE * fp;
  OrderInfo order_info;
  FileInfo file_info;
  PriorityPacket * pripacket = new PriorityPacket;
  UploadModule * upload = new UploadModule;


  tmp_header = "fds[" +to_string(fdsnum) + "]:x:";
  cout << tmp_header << "use socket " << sock << endl;


  int epfd;
  if ((epfd = epoll_create(1)) < 0) {
    perror("epoll_create err");
    sock_close(sock,fdsnum);
    close(epfd);
    delete pripacket;
    delete upload;
    return -1;
  }

  struct epoll_event ev;
  memset(&ev, 0, sizeof ev);
  ev.events = EPOLLIN;
  ev.data.fd = sock;
  if ((epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev)) < 0) {
    perror("epoll_ctl error");
    sock_close(sock,fdsnum);
    close(epfd);
    delete pripacket;
    delete upload;
    return -1;
  }

  int fd_count = 0;
  struct epoll_event events[MAX_EVENTS];
  while(1){
    loop_flag = 1;
    while(loop_flag) {
      fd_count = epoll_wait(epfd, events, MAX_EVENTS, LISTEN_TIMEOUT*1000);
      if(fd_count <= 0){
        cout << tmp_header << "sock wait timeout and close " << sock << endl;
        sock_close(sock,fdsnum);
        close(epfd);
        delete pripacket;
    	  delete upload;
        return -1;
      }
      for (int i = 0; i < fd_count; i++) {
        if (events[i].data.fd == sock ){
          if(events[i].events == EPOLLIN){
            break;
          }

        }else{
          cout << tmp_header << "recev packet from illegal descriptor" << endl;
        }
      }

      //check packet size
      memset(recvbuf.data, 0, capacity);
      recvbuf.size = recv(sock, recvbuf.data, capacity, MSG_PEEK);
      if (recvbuf.size < 0) {
      	cout << "recv() failed." << endl;
      	sock_close(sock,fdsnum);
      	close(epfd);
      	delete pripacket;
    	  delete upload;
      	return -1;
      } else if(recvbuf.size == 0){
      	cout << "connection closed by foreign host." << endl;
      	sock_close(sock,fdsnum);
      	close(epfd);
      	delete pripacket;
    	  delete upload;
      	return -1;
      }
      datatype = pripacket->getDataType(recvbuf.data);
      switch(datatype){
      	case MSG_DEVID:
      		if(recvbuf.size == 2){
      			loop_flag = 0;
            //cout << "recv device id!" << endl;
      		}
      		break;
      	case MSG_META:
      		if(recvbuf.size == ((unsigned int)recvbuf.data[16]+17) ){
      			loop_flag = 0;
            //cout << "recv metadata!" << endl;
      		}
          break;
      	case MSG_FILE:
      		if( 3 <= recvbuf.size){
      			loop_flag = 0;
      		}
      		break;
      	default:
      		loop_flag = 0;
      		break;

      }


    }


    memset(recvbuf.data, 0, recvbuf.size);
    recvbuf.size = recv(sock, recvbuf.data, capacity, 0);
          
    if (recvbuf.size < 0) {
      cout << "recv() failed." << endl;
      sock_close(sock,fdsnum);
      close(epfd);
      delete pripacket;
      delete upload;
      return -1;
    } else if(recvbuf.size == 0){
      cout << "connection closed by foreign host." << endl;
      sock_close(sock,fdsnum);
      close(epfd);
      delete pripacket;
      delete upload;
      return -1;
    }


    
    loop_flag = 1;
    while(loop_flag){
      switch(state){
        case RECV_DEVID:
          cout << tmp_header << "RECV_DEVID" << endl;
          if(datatype == MSG_DEVID){

            device_id = (unsigned int)recvbuf.data[1];

            tmp_header = "fds[" + to_string(fdsnum) + "]:" + to_string(device_id) + ":";

            pripacket->initMETAdata(&file_info);

            agent->updateNext(device_id,file_info);

            state = SEND_ORDER;
            loop_flag = 1;
          }else{
            state = SEND_CLOSE;
            loop_flag = 1;
            break;
          }

        case SEND_ORDER:
          cout << tmp_header << "SEND_ORDER" << endl;
          pripacket->clearOrder(&order_info);

          order_info = agent->getNextInfo(device_id);
          pripacket->showOrderInfo(order_info,tmp_header);
          if(pripacket->getOrder(order_info,orderpacket) == 3){
            res = send(sock, orderpacket, ORDER_SIZE, 0);
          }else{
            res = 1;
            state = SEND_CLOSE;
            loop_flag = 1;
            cout << tmp_header << "make orderpacket failed." << endl;
          }
          if (res <= 0) {
            cout << tmp_header << "send() failed." << endl;
            sock_close(sock,fdsnum);
            close(epfd);
            delete pripacket;
    		    delete upload;
            return -1;
          }
          if(order_info.close){
            state = SEND_CLOSE;
            loop_flag = 1;
          }else{
            if(MEASURE_THROUGHPUT){
              measure->measureThroughput(device_id,SEND_ORDER,file_info);
            }
            state = RECV_METADATA;
            loop_flag = 0;
            break;
          }


        case SEND_CLOSE:
          cout << tmp_header << "SEND_CLOSE" << endl;
          pripacket->clearOrder(&order_info);

          order_info.close = 1;
          if(pripacket->getOrder(order_info,orderpacket) == 3){
            res = send(sock, orderpacket, ORDER_SIZE, 0);
          }else{
            res = -1;
            cout << tmp_header << "make closepacket failed." << endl;
          }
          if (res <= 0) {
            cout << tmp_header << "send() failed." << endl;
            sock_close(sock,fdsnum);
            close(epfd);
            delete pripacket;
    		    delete upload;
            return -1;
          }
          sleep(3);
          sock_close(sock,fdsnum);
          close(epfd);
          delete pripacket;
    	    delete upload;
          return 0;

        case RECV_METADATA:
          cout << tmp_header << "RECV_METADATA" << endl;
          if(datatype == MSG_META){
            pripacket->initMETAdata(&file_info);
            datatype = pripacket->METAdataInterpretation(recvbuf.data,&file_info);
            pripacket->showFileInfo(file_info,tmp_header);

            if(file_info.filesize <= 0){
              /*if(MEASURE_THROUGHPUT){
                measure->measureThroughput(device_id,RECV_METADATA,file_info);
              }*/
              agent->updateNext(device_id,file_info);
              state = SEND_ORDER;
              loop_flag = 1;
            }else{
              res = send(sock, &ackpacket, ACKSIZE, 0);
              if (res <= 0) {
                cout << tmp_header << "send() failed." << endl;
                sock_close(sock,fdsnum);
                close(epfd);
                delete pripacket;
    			      delete upload;
                return -1;
              }
              state = FILE_RECV_START;
              loop_flag = 0;
            }


          }else{
            state = SEND_CLOSE;
            loop_flag = 1;
          }

          break;

        case FILE_RECV_START:
          cout << tmp_header << "FILE_RECV_START" << endl;
          if(datatype == MSG_FILE){

            if(!agent->getSavefile(device_id,file_info,&savefile)){
              cout << tmp_header <<"recev packet from illegal device" << endl;
              state = SEND_CLOSE;
              loop_flag = 1;
              break;
            }
            cout << tmp_header << "save to " << savefile << endl;
            if ((fp = fopen(savefile.c_str(), "wb")) == NULL) {
                cout << tmp_header << "cannot open " << savefile.c_str() << endl;
                sock_close(sock,fdsnum);
                close(epfd);
                delete pripacket;
    			      delete upload;
                return -1;
            }

            packetsize = (unsigned int)recvbuf.data[1]*256+((unsigned int)recvbuf.data[2])%256;
            retsize = fwrite(recvbuf.data+3,1,recvbuf.size-3,fp);
            

            if(retsize == -1){
              cout << tmp_header <<"fwrite() error." << endl;
              sock_close(sock,fdsnum);
              fclose(fp);
              close(epfd);
              delete pripacket;
    		      delete upload;
              return -1;
            }


            datasize = packetsize;
            writesize = retsize;


            if(datasize < file_info.filesize){

              res = send(sock, &ackpacket, ACKSIZE, 0);
              if (res <= 0) {
                cout << tmp_header << "send() failed." << endl;
                sock_close(sock,fdsnum);
                fclose(fp);
                close(epfd);
                delete pripacket;
    			      delete upload;
                return -1;
              }
              //read file packet
              while(writesize < packetsize) {
                memset(recvbuf.data, 0, recvbuf.size);
                if(sizeof(recvbuf) < (packetsize - writesize)){
                  recvbuf.size = recv(sock, recvbuf.data, capacity, MSG_WAITALL);
                }else{
                  recvbuf.size = recv(sock, recvbuf.data, (packetsize - writesize), 0);
                }
      			    if (recvbuf.size < 0) {
      				    cout << "recv() failed." << endl;
      				    sock_close(sock,fdsnum);
      				    fclose(fp);
      				    close(epfd);
      				    delete pripacket;
      			   		delete upload;
      				    return -1;
      			    } else if(recvbuf.size == 0){
      				    cout << "connection closed by foreign host." << endl;
      				    sock_close(sock,fdsnum);
      				    fclose(fp);
      				    close(epfd);
      				    delete pripacket;
      			   		delete upload;
      				    return -1;
      			    }
      			    retsize = fwrite(recvbuf.data,1,recvbuf.size,fp);
	             
  	            if(retsize == -1){
    			        cout << tmp_header <<"fwrite() error." << endl;
    			        sock_close(sock,fdsnum);
    			        fclose(fp);
    			        close(epfd);
    			        delete pripacket;
    			    	  delete upload;
    			        return -1;
  	            }

  	            writesize += retsize;
              }
              state = FILE_RECV_CONTINUE;
              loop_flag = 0;
            }else{
              //read file packet
              while(writesize < packetsize) {
                memset(recvbuf.data, 0, recvbuf.size);
                if(sizeof(recvbuf) < (packetsize - writesize)){
                  recvbuf.size = recv(sock, recvbuf.data, capacity, MSG_WAITALL);
                }else{
                  recvbuf.size = recv(sock, recvbuf.data, (packetsize - writesize), 0);
                }

                if (recvbuf.size < 0) {
                  cout << "recv() failed." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                } else if(recvbuf.size == 0){
                  cout << "connection closed by foreign host." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                }
                retsize = fwrite(recvbuf.data,1,recvbuf.size,fp);
               
                if(retsize == -1){
                  cout << tmp_header <<"fwrite() error." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                }

                writesize += retsize;
              }
              fclose(fp);

              if(MEASURE_THROUGHPUT){
                measure->measureThroughput(device_id,FILE_RECV_START,file_info);
              }
              agent->updateNext(device_id,file_info);
              if(UPLOAD_DB){
                upload->uploadpsql(device_id,savefile,file_info);
              }
              state = SEND_ORDER;
              loop_flag = 1;
            }


          }else{
            state = SEND_CLOSE;
            loop_flag = 1;
          }

          break;
        case FILE_RECV_CONTINUE:
          //cout << tmp_header << "FILE_RECV_CONTINUE" << endl;
          if(datatype == MSG_FILE){

            packetsize = (unsigned int)recvbuf.data[1]*256+((unsigned int)recvbuf.data[2])%256;
            retsize = fwrite(recvbuf.data+3,1,recvbuf.size-3,fp);
            if(retsize == -1){
              cout << tmp_header <<"fwrite() error." << endl;
              sock_close(sock,fdsnum);
              fclose(fp);
              close(epfd);
              delete pripacket;
    		      delete upload;
              return -1;
            }
            

            datasize += packetsize;
            writesize = retsize;
            
            if(datasize < file_info.filesize){
              res = send(sock, &ackpacket, ACKSIZE, 0);
              if (res <= 0) {
                cout << tmp_header << "send() failed." << endl;
                sock_close(sock,fdsnum);
                fclose(fp);
                close(epfd);
                delete pripacket;
                delete upload;
                return -1;
              }
              //read file packet
              while(writesize < packetsize) {
                memset(recvbuf.data, 0, recvbuf.size);
                if(sizeof(recvbuf) < (packetsize - writesize)){
                  recvbuf.size = recv(sock, recvbuf.data, capacity, MSG_WAITALL);
                }else{
                  recvbuf.size = recv(sock, recvbuf.data, (packetsize - writesize), 0);
                }

                if (recvbuf.size < 0) {
                  cout << "recv() failed." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                } else if(recvbuf.size == 0){
                  cout << "connection closed by foreign host." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                }
                retsize = fwrite(recvbuf.data,1,recvbuf.size,fp);
               
                if(retsize == -1){
                  cout << tmp_header <<"fwrite() error." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                }

                writesize += retsize;
              }

              state = FILE_RECV_CONTINUE;
              loop_flag = 0;
            }else{
              //read file packet
              while(writesize < packetsize) {
                memset(recvbuf.data, 0, recvbuf.size);
                if(sizeof(recvbuf) < (packetsize - writesize)){
                  recvbuf.size = recv(sock, recvbuf.data, capacity, MSG_WAITALL);
                }else{
                  recvbuf.size = recv(sock, recvbuf.data, (packetsize - writesize), 0);
                }

                if (recvbuf.size < 0) {
                  cout << "recv() failed." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                } else if(recvbuf.size == 0){
                  cout << "connection closed by foreign host." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                }
                retsize = fwrite(recvbuf.data,1,recvbuf.size,fp);
               
                if(retsize == -1){
                  cout << tmp_header <<"fwrite() error." << endl;
                  sock_close(sock,fdsnum);
                  fclose(fp);
                  close(epfd);
                  delete pripacket;
                  delete upload;
                  return -1;
                }

                writesize += retsize;
              }
              
              fclose(fp);
              if(MEASURE_THROUGHPUT){
                measure->measureThroughput(device_id,FILE_RECV_CONTINUE,file_info);
              }
              agent->updateNext(device_id,file_info);
              if(UPLOAD_DB){
                upload->uploadpsql(device_id,savefile,file_info);
              }
              state = SEND_ORDER;
              loop_flag = 1;
            }

          }else{
          	fclose(fp);
            state = SEND_CLOSE;
            loop_flag = 1;
          }
          break;
        default:
          cout << "illegal state : " << state << endl;
          sock_close(sock,fdsnum);
          close(epfd);
          delete pripacket;
    	    delete upload;
          return -1;
      }
    }
  }
   
}

int main(){
  struct sockaddr addr;
  int len = 0;
  int i,j,ret;
  int yes = 1;


  Agent * agent = new Agent;

  agent->setAgentOpt(FUNC_CONNECTION,1);

  agent->initAgent("./conf/server_prioritydata.csv");
  MeasureModule * measure = new MeasureModule;
  measure->setOutFile("./log/throughputlog.csv");

  unsigned int client_num = agent->getDeviceNum();
  if(client_num < 1){
  	perror("no client");
  	return -1;
  }
  cout << "bufsize = " << RECVBUFSIZE << endl;


  for(i=0;i<SOCK_MAX;i++){
    fds[i] = 0;
  }

  sock_listen = tcp_listen(PORT);


  int epfd;
  if ((epfd = epoll_create(1)) < 0) {
    perror("epoll_create err");
  }

  struct epoll_event ev;
  memset(&ev, 0, sizeof ev);
  ev.events = EPOLLIN;
  ev.data.fd = sock_listen;
  if ((epoll_ctl(epfd, EPOLL_CTL_ADD, sock_listen, &ev)) < 0) {
    perror("epoll_ctl error");
  }

  struct epoll_event events[MAX_EVENTS];

  while(1) {

    int fd_count = epoll_wait(epfd, events, MAX_EVENTS, -1);


    for (i = 0; i < fd_count; i++) {
      if (events[i].data.fd == sock_listen ){

        int sock_client = accept(sock_listen, &addr, (socklen_t*) &len);
        if (sock_client == -1) {
          perror("accept err");
          continue;
        }


        if(setsockopt(sock_client, IPPROTO_TCP, TCP_NODELAY, (char *)&yes, sizeof(yes))){
        	perror("sockopt error");
        }

        for(j=0;j<SOCK_MAX;j++){
          if(fds[j] == 0){
            break;
          }
        }
        if(j !=SOCK_MAX ){
          fds[j]=sock_client;

          thread th(pri_server,sock_client,agent,measure,j);
          th.detach();
        }else{
          close(sock_client);
          cout << "reach thread max!" << endl;
        }

      }else{
        cout << "recev packet from illegal descriptor" << endl;
      }
    }
  }


  tcp_close();

	
  close(epfd);
  return 0;
}
