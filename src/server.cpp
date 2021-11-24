#include "server.h"
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

void server() {
  const string endpoint = "tcp://*:5555";

  zmq::context_t context;

  zmq::socket_t socket(context, zmq::socket_type::rep);

  zmq::message_t replyMessage("World");

  // bind to the socket
  socket.bind(endpoint);
  while (1) {
    // receive the message
    zmq::message_t message;
    // decompose the message
    socket.recv(message, zmq::recv_flags::none);
    string text = message.data<char>();

    //Do some 'work'
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "Received Hello" << endl;
    socket.send(replyMessage, zmq::send_flags::none);
  }
}

// int main(int argc, char *argv[]) {
//     return server();
// }