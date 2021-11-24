project('transfer', 'cpp')

zmqpp = dependency('libzmqpp')

executable('client', 'client.cpp', dependencies : zmqpp)
executable('server', 'server.cpp', dependencies : zmqpp)