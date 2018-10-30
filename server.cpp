//
// Created by 李肖赫 on 2018/10/30.
//
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/types.h>
#include "server.h"

using namespace std;

struct mt
{
    int num;
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
};

struct mt* mm;

void startServer()
{
    mm = (mt*)mmap(NULL, sizeof(*mm),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    memset(mm,0x00, sizeof(*mm));

    pthread_mutexattr_init(&mm->mutexattr);
    pthread_mutexattr_setpshared(&mm->mutexattr,PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mm->mutex,&mm->mutexattr);

    int serverSock = socket(AF_INET,SOCK_STREAM,0);
    if(serverSock == -1)
    {
        printf("socket failed\n");
        exit(0);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int on = 1;
    int ret = setsockopt( serverSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if(::bind(serverSock, (struct sockaddr*)&server_addr , sizeof(server_addr)) == -1){
        printf("bind failed\n");
        exit(0);
    }

    if(listen(serverSock,20) == -1)
    {
        printf("listen failed\n");
        exit(0);
    }

    int result;

    for (int i = 1; i < 3; ++i) {
        result = fork();
        if(result == 0){
            startWorker(i,serverSock);
            printf("start worker %d\n",i);
            break;
        }
    }

    if(result > 0){
        while (1){
            ;
        }
    }
}

void startWorker(int workerId , int serverSock)
{
    int kqueuefd = kqueue();
    struct kevent change_list[1];
    struct kevent event_list[1];

    EV_SET(&change_list[0] , serverSock, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);

    while(true){
        pthread_mutex_lock(&mm->mutex);
        printf("Worker %d get the lock\n",workerId);

        int nevents = kevent(kqueuefd,change_list,1,event_list,1,NULL);

        pthread_mutex_unlock(&mm->mutex);

        for(int i = 0 ; i < nevents ; i ++){
            struct kevent event = event_list[i];
            if(event.ident ==serverSock){
                handleNewConnection(kqueuefd,serverSock);
            }
            else if(event.filter == EVFILT_READ){
                char * msg = handleReadFromClient(workerId,event);
                handleWriteToClient(workerId,event,msg);
            }
        }
    }
}

void handleNewConnection(int kqueuefd , int serverSock){
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clientSock = accept(serverSock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);

    struct kevent change_list[1];

    EV_SET(&change_list[0],clientSock,EVFILT_READ,EV_ADD | EV_ENABLE , 0 , 0 , 0);
    kevent(kqueuefd,change_list,1,NULL,0,NULL);
}


char* handleReadFromClient(int workerId,struct kevent event)
{
    //event.ident 为 连接的 socket 描述符
    int bufferSize = 60;
    char *buffer = new char[bufferSize];
    memset(buffer, 0,  bufferSize);

    if(read(event.ident, buffer, bufferSize - 1)>0){
        printf("Worker %d receive : %s\n",workerId,buffer);
    }
    return buffer;
}


void handleWriteToClient(int workerId,struct kevent event,char* msg)
{
    //event.ident 为 连接的 socket 描述符
    string s = "I am Worker ";
    s.append(to_string(workerId));
    s.append(" & I reveive :");
    s.append(msg);
    char str[60];
    strcpy(str,s.c_str());
    write(event.ident, str, strlen(str));
}












