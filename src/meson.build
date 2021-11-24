project('transfer', 'cpp')

zmqpp = dependency('libzmqpp')

# executable('client', 'client.cpp', dependencies : zmqpp)
# executable('server', 'server.cpp', dependencies : zmqpp)

test = executable('test-server-client', ['test.cpp', 'server.cpp', 'client.cpp'], dependencies : zmqpp)
test('test-server-client', test)