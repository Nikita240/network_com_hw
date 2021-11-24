#ifndef D989034F_E0EE_43F5_A05B_72BC3A9288BA
#define D989034F_E0EE_43F5_A05B_72BC3A9288BA

#include <string>
#include <zmq.hpp>
#include <thread>
#include <atomic>
#include "data.h"

class Client {

public:
    Client(zmq::context_t& a_context) : context(a_context), poison(false) {};
    ~Client() { cancel(); };

    void upload(const std::string endpoint, const std::string filename, size_t fileSize, const std::array<unsigned char, MD5_DIGEST_LENGTH>& hash);
    void cancel();

    std::atomic<unsigned int>& getProgress() { return progress; };

private:

    void run(const std::string endpoint, const std::string filename, size_t fileSize, const std::array<unsigned char, MD5_DIGEST_LENGTH>& hash);

    zmq::context_t& context;
    std::thread* clientThread;
    std::atomic<bool> poison;

    std::atomic<unsigned int> progress;
};

#endif /* D989034F_E0EE_43F5_A05B_72BC3A9288BA */
