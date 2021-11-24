#ifndef ED2D2163_B9C9_4085_B7C0_2C787A68828B
#define ED2D2163_B9C9_4085_B7C0_2C787A68828B

#include <string>
#include <zmq.hpp>
#include <thread>
#include <atomic>
#include <map>
#include <fstream>
#include <openssl/md5.h>

// Specialization to allow us to use client ID packets as a map key.
namespace std
{
    template <>
    struct hash<zmq::mutable_buffer>
    {
        std::size_t operator()(const zmq::mutable_buffer& k) const {
            // We're basically casting a 5 byte integer (stored as an array of 5 bytes) to an 8 byte size_t
            // by zeroing out bytes 6-8.
            return *reinterpret_cast<std::size_t*>(k.data()) & 0xffffffffff;
        }
    };

    template<>
    struct less<zmq::mutable_buffer>
    {
       bool operator() (const zmq::mutable_buffer& lhs, const zmq::mutable_buffer& rhs) const
       {
           return std::hash<zmq::mutable_buffer>{}(lhs) < std::hash<zmq::mutable_buffer>{}(rhs);
       }
    };
}

class Server {

public:
    Server(zmq::context_t& a_context) : context(a_context), poison(false) {};
    ~Server() { stop(); };

    void start(const std::string endpoint);
    void stop();

private:
    struct FileTransfer {
        FileTransfer() : chunkRequested(0), bandwidthUsage(0) {};

        unsigned char hash[MD5_DIGEST_LENGTH];
        std::ofstream file;
        size_t chunkRequested;
        size_t bandwidthUsage;
    };

    void run(const std::string endpoint);

    zmq::context_t& context;
    std::thread* serverThread;
    std::atomic<bool> poison;

    std::map<zmq::mutable_buffer, FileTransfer> activeTransfers;
};

#endif /* ED2D2163_B9C9_4085_B7C0_2C787A68828B */
