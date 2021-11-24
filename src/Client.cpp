#include "Client.h"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
// #include "zhelpers.hpp"

void Client::upload(const std::string endpoint, const std::string filename, size_t fileSize, const std::array<unsigned char, MD5_DIGEST_LENGTH>& hash) {
    poison = false;
    progress = 0;
    clientThread = new std::thread(&Client::run, this, endpoint, filename, fileSize, hash);
}

void Client::cancel() {
    poison = true;
    if(clientThread->joinable())
        clientThread->join();
    delete clientThread;
}

void Client::run(const std::string endpoint, const std::string filename, size_t fileSize, const std::array<unsigned char, MD5_DIGEST_LENGTH>& hash) {

    zmq::socket_t socket(context, zmq::socket_type::dealer);

    socket.connect(endpoint);

    // Open file.
    std::ifstream file(filename, std::ios::binary);

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

    progress = 100;
}
