# http-proxy

An HTTP proxy will be implemented with C++. 

# Some mistakes encountered during this project.

1. getaddrinfo(param1, param2, param3, struct addrinfo ** servinfo);

The last parameter is a double pointer.
It's job is to write the servinfo to **servinfo.
And change the pointer of *servinfo to pointer at **servinfo.

The error I encounter is :
When I write a function say func(..., struct addrinfo * p1):
    this function will call getaddrinfo(.... , &p1) and pass &p1 to getaddrinfo
    this is actuall not correct, cause getaddinfo actually changes p1 to point at the result (**servinfo)
    not the pointer that calls func()

    For example: we may write something like:
    int main(){
        struct addrinfo * servinfo; //point to nowhere
        func(..., servinfo);
        //after calling the func, sercinfo is still points to no where
        //what changes is the p1 pointer inside the func stack frame
    }


    To solve this problem we need to pass struct addrinfo ** p1 (double pointer) to func


2. recv 

ssize_t recv(int socket, void *buffer, size_t length, int flags);

This function will be blocked if you set flags MSG_WAITALL :
    If MSG_WAITALL is set,
        this function will wait until length byte has been filled into the buffer.
    Another case is that:
        If the remote socket is disconnected, then recv will stop and return 0.
    
But if you do not use MSG_WAITALL, there is cases:
    the mess is not fully received with you specified length.

The correct way to use this function is firstly knows the size of received message or have
other protocols reqgarding stop.

EX:
HTTP response will firstly response a header end with \r\n,
the header will possible specify the followings:
    1. Content-Length(uppercase, lower case is ok)

Other thing about HTTP response header:
    1. Connection: close
        The server close the connection
    2. Connection: keep-alive
        The server keep the connection with you, but you can choose to close the sock
        if you do not want to connect with the server

3. length send and recv
    ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    ssize_t recv(int socket, void *buffer, size_t length, int flags);

    Ex: when you send
    send(.. "12345", 5, 0);
    send(.. "67890", 5, 0);

    when you call recv(..., 10, 0);
    Recv will only recv the first send which is "12345"

    But if you send:
    send(.. "1234567890", 10, 0);

    when you call recv(..., 10, 0);
    Recv will recv all which is "1234567890"

# Connect

CONNECT server.example.com:433 HTTP/1.1
Host: server.example.com:433
Proxy-Authorization: basic aGVsbG86d29ybGQ=

When connect request, you have to parse the port number
to build sock

# HTTP is stateless
Refer https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview for more info

## GET and POST
HTTP is stateless: there is no link between two requests being successively carried out on the same connection.

The default behavior of HTTP/1.0 is to open a separate TCP connection for each HTTP request/response pair. This is less efficient than sharing a single TCP connection when multiple requests are sent in close succession.

For HTTP/1.0, GET and POST usually need two seperate sock connection even if the GET and POST are connsecutive.
For example, if we have GET request to a web and then a POST request to the same web.
These two requests will create different sock.

In order to mitigate this flaw, HTTP/1.1 introduced pipelining (which proved difficult to implement) and persistent connections: the underlying TCP connection can be partially controlled using the Connection header. 

https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
The Connection general header controls whether the network connection stays open after the current transaction finishes. If the value sent is keep-alive, the connection is persistent and not closed, allowing for subsequent requests to the same server to be done.

## CONNECT
The HTTP CONNECT method starts two-way communications with the requested resource. It can be used to open a tunnel.

For example, the CONNECT method can be used to access websites that use SSL (HTTPS). The client asks an HTTP Proxy server to tunnel the TCP connection to the desired destination. The server then proceeds to make the connection on behalf of the client. Once the connection has been established by the server, the Proxy server continues to proxy the TCP stream to and from the client.

Connect example:
CONNECT server.example.com:80 HTTP/1.1
Host: server.example.com:80
Proxy-Authorization: basic aGVsbG86d29ybGQ=


## C++ use self defined object as the key of unordered map
You have to specify another hash function class which will overload the () operator.
Then you can put the hash function class into ordered map template <>.

Also, you have to specify == operator in you self defined class in case of hash collision.

In this project, I used unordered_map in the LRUCahe class, the key of the map is the Request
and the val of the req is the response.

For the same request, we may want to response the resp cached in our map.

But I do not take time into consieration.


## CONNECT
CONNECT header is in the following format:

CONNECT www.youtube.com:443 HTTP/1.1
User-Agent: PostmanRuntime/7.29.0
Accept: */*
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
host: www.youtube.com:443

When you build server sock, you need to split www.youtube.com:443

Actually, GET and POST may also have format.

If there is not : in the hostname, that means the defaul 80.

## https

CONNECT before sending GET, REQUEST and POST

CONNECT request will send to the proxy which will ask the proxy set up a tunel with
the server.

Then GET, REQUEST and POST request will be followed.

Once one end quit and close the connection, maybe wait a few seconds.
The tunel will be closed.

CONNECT will need multi process, the reason is that:
once a tunel is established, it will block there waiting for client and server
message, which means they will occupy one process.

We can either design CONNECT as multi process or multi thread

## issues in my muti-process implementation of request
1. Cache was copied across each process not efficient.
    But for CONNECT, there is no need to cache.

2. Normally the framework to handle request with multi process is

    Listener accept a connection from client

    fork --> leads to parent process and child process

        child process will firstly close listener and then do request staff

        parent will close the accepted connection with client 

    The problem here is:
        If I close the listener in child process before do request staff,
        CONNECT will not work, but pure GET and Request (http) will work fine.
        If I do request staff then do close listener, everything works well.

        
# thread 
C++ 11 and later version use <thread> lib, but basically the lib uses C pthread

For program that uses thread lib, we need to add -lpthread at the end of compile command.


# revalidation

Because the server may respond differently. For example, you have an expired cached response and it has "must-revalidate" field in its header. This response is quite large (you can imagine 1GB). If you send the request to server and the server gives back a 304 response (which is quite small), it saves a lot. Now you have another expired cached response and it does not require revalidation, you can simply print  "ID: in cache, but expired at EXPIREDTIME" and then treat it as a new request.

For re-validation, the proxy SHOULD send send both Last-Modified and ETag if it has both. It MUST send the ETag if it has that. If there's no ETag, then just send the Last-Modified field.

If the response has neither (which may be rare), just send the whole HTTP request to the server and ignore the step of revalidation. 


If there is no Cache-Control or at least the request does not ask you not to use cached data, you can always use cached data if any. You may care about "max-age", "max-stale", "min-fresh", "no-cache", "no-store" and "must-revalidate".




# cache lock
When using cache in multi-thread, we have to make sure the cache data sturecure is thread safe.
Because only get method will use cache, I add lock to the cache when there is a GET request.
But the problem is that this approach is not efficient. I add lock to the whole cache and this means 
the get requests will actually runs in parallel.
A good way to improve this is to improve the granduality of the lock.
We can add lock to each key-value pairs inside the cache.


# dup and dup2

## int dup(int oldfd);
dup will make a newfd that is the same of oldfd and assign the newly created fd to the lowest fd that is  
not used by the process.

In other words: create a duplicated fd.

## int dup2(int oldfd, int newfd);
dup2 will make a newfd that us the same as oldfd and assign the newly created fd to newfd.


# docker command

## ADD <src> <dest>
add the files in the local computer src from the build path to docker image dest.

## WOEKDIR <path>
set the working dir, things will run here.
Where the entry will go.

## docker exec -it <container_id> bash
enter into the running container using bash

## docker run --rm -it <image_id> bash
enter into the stale image when running up




