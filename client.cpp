//
// Created by 李肖赫 on 2018/10/30.
//
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

void startClientID(int clientId){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(9999);
    connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while (true){
        string s = "I am client ";
        s.append(to_string(clientId));

        char str[60];
        strcpy(str,s.c_str());
        write(sock, str, strlen(str));

        char buffer[60];
        if(read(sock, buffer, sizeof(buffer)-1) > 0){
            printf("client %d receive : %s\n",clientId,buffer);
        }

        sleep(9);
    }

}

void startClient()
{
    for (int i = 0; i < 6; ++i) {
        thread thread(startClientID , i);
        thread.detach();
        sleep(1);
    }
    while (1){
        ;
    }
}




