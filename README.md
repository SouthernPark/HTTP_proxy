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




