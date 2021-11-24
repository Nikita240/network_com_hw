#include "Server.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <signal.h>

using namespace std::chrono_literals;

volatile sig_atomic_t stop;

zmq::context_t context;

void inthand(int signum) {
    stop = 1;
}

int main(int argc, char **argv) {

    Server server(context);
    server.start(argv[1]);

    signal(SIGINT, inthand);

    while(!stop) {
        std::this_thread::sleep_for(100ms);
    }

    std::cout << std::endl << "Exiting Safely" << std::endl;

    server.stop();

    return 0;
}