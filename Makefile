TARGETS=Main Daemon

all: $(TARGETS)
clean:
	rm *.o
	rm Main
	rm Daemon



Main: Utility.o Request.o Response.o ClientSock.o ServerSock.o ListenerSock.o Proxy.o
	g++ -g -o Main *.o Main.cpp -lpthread

Daemon:	Daemon.cpp Main
	g++ -g -o Daemon Daemon.cpp

Utility.o: Utility.cpp
	g++ -c Utility.cpp

Request.o: Request.cpp
	g++ -c Request.cpp

Response.o: Response.cpp
	g++ -c Response.cpp

ClientSock.o: ClientSock.cpp
	g++ -c ClientSock.cpp

ServerSock.o: ServerSock.cpp
	g++ -c ServerSock.cpp

ListenerSock.o: ListenerSock.cpp
	g++ -c ListenerSock.cpp

Proxy.o: Proxy.cpp
	g++ -c Proxy.cpp

test_utility: Utility.cpp UtilityTest.cpp
	g++ -g -o test_utility Utility.cpp UtilityTest.cpp

test_listener: Utility.cpp ClientSock.cpp ListenerSock.cpp test_listener.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp
	g++ -g Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp LRUCache.h test_listener.cpp -o test_listener -lpthread

test_cache: 
	g++ -g -o test_cache Utility.cpp ClientSock.cpp ListenerSock.cpp Request.cpp Response.cpp ServerSock.cpp Proxy.cpp LRUCache.h test_cache.cpp