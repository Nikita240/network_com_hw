#include "client.h"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <algorithm>
// #include "zhelpers.hpp"

#define CHUNK_SIZE 25000

std::array<unsigned char, MD5_DIGEST_LENGTH> fileHash(const std::string filename, size_t fileSize) {
    std::array<unsigned char, MD5_DIGEST_LENGTH> result;

    void* fileBuffer = mmap(0, fileSize, PROT_READ, MAP_SHARED, open(filename.c_str(), O_RDONLY), 0);
    MD5((unsigned char*) fileBuffer, fileSize, result.data());
    munmap(fileBuffer, fileSize);

    return result;
}

void Client::upload(const std::string endpoint, const std::string filename) {
    poison = false;
    progress = 0;
    clientThread = new std::thread(&Client::run, this, endpoint, filename);
}

void Client::cancel() {
    poison = true;
    if(clientThread->joinable())
        clientThread->join();
    delete clientThread;
}

void Client::run(const std::string endpoint, const std::string filename) {

    zmq::socket_t socket(context, zmq::socket_type::dealer);

    socket.connect(endpoint);

    // Open file.
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    // Hash the file for testing data integrity.
    size_t fileSize = file.tellg();
    std::array<unsigned char, MD5_DIGEST_LENGTH> hash = fileHash(filename, fileSize);

    // Send file hash to start transfer.
    zmq::const_buffer hashFrame(hash.data(), MD5_DIGEST_LENGTH);
    socket.send(hashFrame, zmq::send_flags::none);

    while(!poison) {
        // We wait to receive a request for a chunk.
        zmq::message_t chunkOffsetFrame;
        socket.recv(&chunkOffsetFrame);
        size_t* chunkOffset = chunkOffsetFrame.data<size_t>();

        // Create the chunk frame.
        char data[CHUNK_SIZE];
        file.seekg(*chunkOffset);
        size_t sizeRead = file.readsome(data, CHUNK_SIZE);
        zmq::const_buffer chunkFrame(data, sizeRead);

        // Send the chunk offset back first. (In case packets arrive out of order)
        socket.send(chunkOffsetFrame, zmq::send_flags::sndmore);
        // Send the chunk itself.
        socket.send(chunkFrame, zmq::send_flags::none);

        progress = std::min(*chunkOffset, fileSize) * 100 / fileSize;

        if(sizeRead == 0) {
            break;
        }
    }
}
