#include "Server.h"
#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include <sys/stat.h>
// #include "zhelpers.hpp"

void Server::start(const std::string endpoint) {
    mkdir("files", 0777);

    poison = false;
    serverThread = new std::thread(&Server::run, this, endpoint);
}

void Server::stop() {
    poison = true;
    if(serverThread->joinable())
        serverThread->join(); // This segfaults sometimes?????
    delete serverThread;
}

void Server::run(const std::string endpoint) {

    zmq::socket_t socket(context, zmq::socket_type::router);
    // Chunks are sent in 3 frames, so our hvm is x3 + some margin for new transfer requests.
    socket.set(zmq::sockopt::rcvhwm, BANDWIDTH*3 + 2*10);

    socket.bind(endpoint);

    // This is the maximum number of chunks we can request and wait for at the same time.
    size_t bandwidthCredits = BANDWIDTH;

    while(!poison) {
        // First frame is client ID.
        void* ptr = malloc(5); // ID's are 5 bytes by default.
        zmq::mutable_buffer id(ptr, 5);
        auto result = socket.recv(id, zmq::recv_flags::dontwait);

        // Don't block the loop in case client tries to exit.
        if(!result)
            continue;

        auto itr = activeTransfers.find(id);

        // If we do not have the client ID saved, then we assume the client is
        // trying to start a new transfer.
        if(itr == activeTransfers.end()) {
            activeTransfers[id] = FileTransfer();

            // Hash frame should come first.
            zmq::mutable_buffer hashFrame(activeTransfers[id].hash, MD5_DIGEST_LENGTH);
            socket.recv(hashFrame, zmq::recv_flags::none);

            // We will name files by base64 encoding their hash to avoid dupes that overwrite each other.
            std::string filename("files/" + macaron::Base64::Encode(std::string(activeTransfers[id].hash, activeTransfers[id].hash+MD5_DIGEST_LENGTH)));
            activeTransfers[id].file = std::ofstream(filename, std::ios::binary);
        }
        // If we have the client ID saved, then we have an active transfer.
        else {
            // Chunk offset comes first.
            zmq::message_t chunkOffsetFrame;
            socket.recv(&chunkOffsetFrame);
            size_t* chunkOffset = chunkOffsetFrame.data<size_t>();

            // The actual data comes next.
            void* ptr = malloc(CHUNK_SIZE);
            zmq::mutable_buffer chunkFrame(ptr, CHUNK_SIZE);
            auto receivedBytes = socket.recv(chunkFrame, zmq::recv_flags::none);

            if (receivedBytes && receivedBytes->untruncated_size) {
                // Write the data to file.
                itr->second.file.seekp(*chunkOffset);
                itr->second.file.write(reinterpret_cast<char*>(chunkFrame.data()), receivedBytes->untruncated_size);
                // itr->second.file.flush(); // for debugging only

                // We proccessed a chunk. Reclaim our bandwidth credit!
                --itr->second.bandwidthUsage;
                ++bandwidthCredits;
            }
            // End transfer if we start receive zero bytes.
            else {
                // Release all pending bandwidth credits since the client should stop sending after end of file.
                bandwidthCredits += itr->second.bandwidthUsage;

                // Close file.
                itr->second.file.close();

                // Remove client ID.
                activeTransfers.erase(id);
            }
        }

        // Spend our bandwidth credits by requesting chunks.
        while(bandwidthCredits && activeTransfers.size()) {
            // @todo this could be changed to round robin between different clients.
            auto itr = activeTransfers.begin();

            // Send the client ID first so the message routes to the right client.
            socket.send(itr->first, zmq::send_flags::sndmore);

            zmq::const_buffer chunkRequestFrame(&itr->second.chunkRequested, sizeof(size_t));
            socket.send(chunkRequestFrame, zmq::send_flags::none);

            // Increment the data pointer we want by our PREDEFINED chunk size.
            itr->second.chunkRequested += CHUNK_SIZE;

            --bandwidthCredits;
            ++itr->second.bandwidthUsage;
        }
    }
}
