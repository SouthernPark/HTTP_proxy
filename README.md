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




    

