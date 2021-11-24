#include "client.h"
#include <zmqpp/zmqpp.hpp>
#include <iostream>

using namespace std;

void client(const string endpoint) {
  // initialize the 0MQ context
  zmqpp::context context;

  // generate a push socket
  zmqpp::socket_type type = zmqpp::socket_type::req;
  zmqpp::socket socket (context, type);

  // open the connection
  printf("Connecting to %s...\n", endpoint.c_str());
  socket.connect(endpoint);
  int request_nbr;
  for (request_nbr = 0; request_nbr != 10; request_nbr++) {
    // send a message
    cout << "Sending Hello " << request_nbr <<"…" << endl;
    zmqpp::message message;
    // compose a message from a string and a number
    message << "Hello";
    socket.send(message);
    string buffer;
    socket.receive(buffer);

    cout << "Received World " << request_nbr << endl;
  }
}

// int main(int argc, char *argv[]) {
//   return client("tcp://server:5555");
// }