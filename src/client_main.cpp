#include "Client.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string>
#include <fstream>

using namespace std::chrono_literals;

volatile sig_atomic_t stop;

zmq::context_t context;

void inthand(int signum) {
    stop = 1;
}

std::array<unsigned char, MD5_DIGEST_LENGTH> fileHash(const std::string filename, size_t fileSize) {
    std::array<unsigned char, MD5_DIGEST_LENGTH> result;

    void* fileBuffer = mmap(0, fileSize, PROT_READ, MAP_SHARED, open(filename.c_str(), O_RDONLY), 0);
    MD5((unsigned char*) fileBuffer, fileSize, result.data());
    munmap(fileBuffer, fileSize);

    return result;
}

int main(int argc, char **argv) {

    if(argc < 3) {
        std::cout << "Please enter a connection endpoint and filename and try again." << std::endl;
        std::cout << "Example: client \"tcp://localhost:5557\" files/cad_mesh.stl"  << std::endl;

        return 1;
    }

    const std::string endpoint = argv[1];
    const std::string filename = argv[2];

    // Check the file.
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(!file.good()) {
        std::cout << "File does not exist." << std::endl;

        return 1;
    }
    size_t fileSize = file.tellg();

    // Hash the file for testing data integrity.
    std::array<unsigned char, MD5_DIGEST_LENGTH> hash = fileHash(filename, fileSize);

    // Upload file.
    Client client(context);
    client.upload(endpoint, filename, fileSize, hash);

    signal(SIGINT, inthand);

    while(client.getProgress() < 100) {
        std::this_thread::sleep_for(100ms);

        std::cout <<  "\r" <<  "Progress: " << std::to_string(client.getProgress().load()) << "%" << std::flush;

        if(stop) {
            std::cout << std::endl << "Exiting Safely" << std::endl;

            client.cancel();

            return 0;
        }
    }

    std::cout << std::endl;
    std::cout << "Done" << std::endl;

    return 0;
}