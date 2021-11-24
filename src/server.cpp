#include "server.h"
#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include "zhelpers.hpp"

#define CHUNK_SIZE 25000
#define BANDWIDTH 10

void Server::start(const std::string endpoint) {
    poison = false;
    serverThread = new std::thread(&Server::run, this, endpoint);
}

void Server::stop() {
    poison = true;
    serverThread->join();
    delete serverThread;
}

void Server::run(const std::string endpoint) {

    zmq::socket_t socket(context, zmq::socket_type::router);
    socket.set(zmq::sockopt::rcvhwm, BANDWIDTH*2);
    socket.set(zmq::sockopt::sndhwm, BANDWIDTH*2);

    socket.bind(endpoint);

    // This is the maximum number of chunks we can request and wait for at the same time.
    size_t bandwidthCredits = BANDWIDTH;

    while(!poison) {

        // First frame is client ID.
        void* ptr = malloc(5); // ID's are 5 bytes by default.
        zmq::mutable_buffer id(ptr, 5);
        socket.recv(id);

        auto itr = activeTransfers.find(id);

        // If we do not have the client ID saved, then we assume the client is
        // trying to start a new transfer.
        if(itr == activeTransfers.end()) {
            // Filename frame should come first.
            zmq::message_t nameFrame;
            socket.recv(&nameFrame);

            activeTransfers[id] = FileTransfer();
            activeTransfers[id].name = nameFrame.to_string();

            // We will prefix the filename by the id to handle identical filenames from multiple clients.
            std::string filename(std::string("files/") + "_" + activeTransfers[id].name);
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
            auto receivedBytes = socket.recv(chunkFrame);

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
            else
            {
                // Release all pending bandwidth credits since the client should stop sending after end of file.
                bandwidthCredits += itr->second.bandwidthUsage;

                // Close file.
                itr->second.file.close();

                // Remove client ID.
                activeTransfers.erase(id);
            }
        }

        // Spend our bandwidth credits by requesting chunks.
        while(bandwidthCredits) {
            if (activeTransfers.size()) {
                // @todo this could be changed to round robin between different clients.
                auto itr = activeTransfers.begin();

                // Send the client ID first so the message routes to the right client.
                // zmq::const_buffer idOutFrame(&itr->first, sizeof(uint32_t));
                // zmq::message_t idOutFrame(&itr->first, 5);
                // std::cout << "server: " << idOutFrame.str() << " idOutFrame" << std::endl;
                socket.send(itr->first, zmq::send_flags::sndmore);

                // zmq::const_buffer chunkRequestFrame(&itr->second.chunkRequested, sizeof(size_t));
                zmq::const_buffer chunkRequestFrame(&itr->second.chunkRequested, sizeof(size_t));
                socket.send(chunkRequestFrame, zmq::send_flags::none);

                // Increment the data pointer we want by our PREDEFINED chunk size.
                itr->second.chunkRequested += CHUNK_SIZE;

                --bandwidthCredits;
                ++itr->second.bandwidthUsage;
            }
        }
    }
}
