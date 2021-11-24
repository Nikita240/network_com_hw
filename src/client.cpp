#include "client.h"
#include <zmq.hpp>
#include <iostream>

using namespace std;

void client(const string endpoint) {
  zmq::context_t context;

  zmq::socket_t socket(context, zmq::socket_type::req);

  // open the connection
  printf("Connecting to %s...\n", endpoint.c_str());
  socket.connect(endpoint);
  int request_nbr;
  for (request_nbr = 0; request_nbr != 10; request_nbr++) {
    // send a message
    cout << "Sending Hello " << request_nbr <<"…" << endl;
    zmq::message_t message("Hello");
    auto result = socket.send(message, zmq::send_flags::none);

    zmq::message_t returnMessage;
    auto res = socket.recv(returnMessage, zmq::recv_flags::none);

    cout << "Received World " << request_nbr << endl;
  }
}

// int main(int argc, char *argv[]) {
//   return client("tcp://server:5555");
// }