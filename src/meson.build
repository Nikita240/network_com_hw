project('transfer', 'cpp')

cppzmq = dependency('cppzmq')
openssl = dependency('openssl')
libcurl = dependency('libcurl')

executable('server', ['server_main.cpp', 'Server.cpp', 'Client.cpp'], dependencies : [cppzmq, openssl])
executable('client', ['client_main.cpp', 'Server.cpp', 'Client.cpp'], dependencies : [cppzmq, openssl, libcurl])

executable('test-server-client', ['test.cpp', 'Server.cpp', 'Client.cpp'], dependencies : [cppzmq, openssl])