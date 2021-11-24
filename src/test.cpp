#include "server.h"
#include "client.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

zmq::context_t context;

int main(int argc, char **argv) {

    Server server(context);
    server.start("inproc://example");

    Client client(context);
    client.upload("inproc://example", "files/helloworld.txt");

    std::cout << "\r" << "Progress: " << std::to_string(client.getProgress().load()) << "%" << std::flush;

    while(client.getProgress() < 100)
    {
        std::this_thread::sleep_for(100ms);

        std::cout <<  "\r" <<  "Progress: " << std::to_string(client.getProgress().load()) << "%" << std::flush;
    }

    std::cout << std::endl;
    std::cout << "Done" << std::endl;

    server.stop();

    return 0;
}