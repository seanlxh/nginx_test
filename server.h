//
// Created by 李肖赫 on 2018/10/30.
//

#ifndef NGINXTRY_SERVER_H
#define NGINXTRY_SERVER_H
void startServer();
void startWorker(int workerId,int serverSock);
void handleNewConnection(int kqueuefd,int serverSock);
char* handleReadFromClient(int workerId,struct kevent event);
void handleWriteToClient(int workerId,struct kevent event, char* msg);
#endif //NGINXTRY_SERVER_H
