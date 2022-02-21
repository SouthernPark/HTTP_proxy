TARGETS=Main Daemon

all: $(TARGETS)
clean:
	rm -f $(TARGETS)



Main: Utility.cpp ClientSock.cpp ListenerSock.cpp test_listener.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp
	g++ -g -o Main Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp Main.cpp -lpthread

Daemon:	Daemon.cpp
	g++ -g -o Daemon Daemon.cpp

test_utility: Utility.cpp UtilityTest.cpp
	g++ -g -o test_utility Utility.cpp UtilityTest.cpp

test_listener: Utility.cpp ClientSock.cpp ListenerSock.cpp test_listener.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp
	g++ -g Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp LRUCache.h test_listener.cpp -o test_listener -lpthread

test_cache: 
	g++ -g -o test_cache Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp LRUCache.h test_cache.cpp