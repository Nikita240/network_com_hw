#include "server.h"
#include "client.h"
#include <thread>

int main(int argc, char **argv) {
    std::thread serverThread(server);
    std::thread clientThread(client, "tcp://localhost:5555");

    serverThread.join();
    clientThread.join();

    return 0;
}