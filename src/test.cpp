#include "server.h"
#include "client.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

zmq::context_t context;

int main(int argc, char **argv) {

    Server server(context);
    server.start("inproc://example");

    Client client(context);
    client.upload("inproc://example", "files/helloworld.txt");

    while(1)
    {
        std::this_thread::sleep_for(1000ms);
    }

    return 0;
}