project('transfer', 'cpp')

cppzmq = dependency('cppzmq')

# executable('client', 'client.cpp', dependencies : zmqpp)
# executable('server', 'server.cpp', dependencies : zmqpp)

test = executable('test-server-client', ['test.cpp', 'server.cpp', 'client.cpp'], dependencies : [cppzmq])
test('test-server-client', test)