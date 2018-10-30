
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "client.h"
#include "server.h"

using namespace std;

int main(int argc, const char * argv[]) {
    signal(SIGPIPE, SIG_IGN);
    int result = fork();
    if(result == 0){
        startClient();
    }else{
        startServer();
    }
    return 0;
}