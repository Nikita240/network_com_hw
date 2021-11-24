project('transfer', 'cpp')

cppzmq = dependency('cppzmq')
openssl = dependency('openssl')

# executable('client', 'client.cpp', dependencies : zmqpp)
executable('server', ['server_main.cpp', 'Server.cpp', 'Client.cpp'], dependencies : [cppzmq, openssl])

executable('test-server-client', ['test.cpp', 'Server.cpp', 'Client.cpp'], dependencies : [cppzmq, openssl])