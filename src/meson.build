project('transfer', 'cpp')

thread_dep = dependency('threads')
cppzmq = dependency('cppzmq')
openssl = dependency('openssl')

# executable('client', 'client.cpp', dependencies : zmqpp)
# executable('server', 'server.cpp', dependencies : zmqpp)

test = executable('test-server-client', ['test.cpp', 'server.cpp', 'client.cpp'], dependencies : [cppzmq, openssl, thread_dep])
test('test-server-client', test)