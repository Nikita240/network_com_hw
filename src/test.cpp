#include "Server.h"
#include "Client.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

zmq::context_t context;

int main(int argc, char **argv) {

    Server server(context);
    server.start("tcp://*:5557");

    Client client(context);
    client.upload("tcp://localhost:5557", "files/testdata");

    std::cout << "\r" << "Progress: " << std::to_string(client.getProgress().load()) << "%" << std::flush;

    while(client.getProgress() < 100)
    {
        std::this_thread::sleep_for(100ms);

        std::cout <<  "\r" <<  "Progress: " << std::to_string(client.getProgress().load()) << "%" << std::flush;
    }

    std::cout << std::endl;
    std::cout << "Done" << std::endl;

    return 0;
}