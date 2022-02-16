TARGETS=Main Daemon

all: $(TARGETS)
clean:
	rm -f $(TARGETS)



Main: Utility.cpp ClientSock.cpp ListenerSock.cpp test_listener.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp
	g++ -g -o Main Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp Main.cpp

Daemon:	Daemon.cpp
	g++ -g -o Daemon Daemon.cpp



test: Utility.cpp test.cpp test_server.cpp ServerSock.cpp Response.cpp
	g++ -g -ggdb3 -o main_test Utility.cpp ServerSock.cpp Response.cpp test.cpp
	g++ -g -ggdb3 -o main_server Utility.cpp test_server.cpp

test_response: Utility.cpp test.cpp ServerSock.cpp
	g++ -g -ggdb3 -o test_response Utility.cpp test.cpp ServerSock.cpp

test_utility: Utility.cpp UtilityTest.cpp
	g++ -g -o test_utility Utility.cpp UtilityTest.cpp

test_listener: Utility.cpp ClientSock.cpp ListenerSock.cpp test_listener.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp
	g++ -g -o test_listener Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp test_listener.cpp
