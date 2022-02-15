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





    

