#ifndef D989034F_E0EE_43F5_A05B_72BC3A9288BA
#define D989034F_E0EE_43F5_A05B_72BC3A9288BA

#include <string>
#include <zmq.hpp>
#include <thread>
#include <atomic>

class Client {

public:
    Client(zmq::context_t& a_context) : context(a_context), poison(false) {};
    ~Client() { cancel(); };

    void upload(const std::string endpoint, const std::string filename);
    void cancel();

private:

    void run(const std::string endpoint, const std::string filename);

    zmq::context_t& context;
    std::thread* clientThread;
    std::atomic<bool> poison;
};

#endif /* D989034F_E0EE_43F5_A05B_72BC3A9288BA */
