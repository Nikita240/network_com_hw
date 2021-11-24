#include "client.h"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include "zhelpers.hpp"

#define CHUNK_SIZE 25000

void Client::upload(const std::string endpoint, const std::string filename) {
    poison = false;
    clientThread = new std::thread(&Client::run, this, endpoint, filename);
}

void Client::cancel() {
    poison = true;
    clientThread->join();
    delete clientThread;
}

void Client::run(const std::string endpoint, const std::string filename) {

    zmq::socket_t socket(context, zmq::socket_type::dealer);

    socket.connect(endpoint);

    // Open file.
    std::ifstream file(filename, std::ios::binary);

    // Send file name message to start transfer.
    socket.send(zmq::str_buffer("output.txt"), zmq::send_flags::none);

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
    }
}
