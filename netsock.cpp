//==========================================================================================================
// netsock.cpp - Implements a network socket
//==========================================================================================================
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "netsock.h"
using namespace std;

//==========================================================================================================
// Constructor
//==========================================================================================================
NetSock::NetSock()
{
    // Ensure that we start out with an invalid socket descriptor
    m_sd = -1;

    // This socket has not yet been created
    m_is_created = false;
}
//==========================================================================================================


//==========================================================================================================
// close() - Closes the socket
//==========================================================================================================
void NetSock::close()
{
    if (m_sd >= 0) ::close(m_sd);
    m_sd = -1;
}
//==========================================================================================================


//==========================================================================================================
// connect() - Creates the socket and connects it to a server
//==========================================================================================================
bool NetSock::connect(std::string server, int port)
{
    char ascii_port[20];
    struct addrinfo hints, *p_res;

    // The socket is not yet created
    m_is_created = false;

    // Close this socket if it happens to be open
    close();

    // Get an ASCII version of the port number
    sprintf(ascii_port, "%i", port);

    // We're going to build an IPv4/IPv6 TCP socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    // Get information about this server
    if (getaddrinfo(server.c_str(), ascii_port, &hints, &p_res) != 0)
    {
        m_error_str = "no such server: "+server;
        m_error     = NO_SUCH_SERVER;
        return false;
    }

    // If we didn't get a result from getaddrinfo, something's wrong
    if (p_res == nullptr)
    {
        m_error_str = "failure on getaddrinfo()";
        m_error     = GETADDRINFO_FAILED;
        return false;
    }

    // Save a copy of the results
    struct addrinfo res = *p_res;

    // Free the memory that was allocated by getaddrinfo
    freeaddrinfo(p_res);

    // Create the socket
    m_sd = socket(res.ai_family, res.ai_socktype, res.ai_protocol);

    // If the socket() call fails, complain
    if (m_sd < 0)
    {
        m_error_str = "failure on socket()";
        m_error     = SOCKET_FAILED;
        return false;
    }

    // Attempt to connect to the server
    if (::connect(m_sd, res.ai_addr, res.ai_addrlen) < 0)
    {
        m_error_str = "can't connect to "+server;
        m_error     = GETADDRINFO_FAILED;
        close();
        return false;
    }

    // If we get here, we have a connected socket
    return true;
}
//==========================================================================================================




//==========================================================================================================
// create_server() - Creates a server socket
//
// Passed:  port    = The TCP port number to create the socket on
//          bind_to = The IP address of the network card to bind to (optional)
//          family  = AF_UNSPEC, AF_INET, or AF_INET6
//
// Returns: 'true' if the server socket was created succesfully, otherwise 'false' 
//==========================================================================================================
bool NetSock::create_server(int port, string bind_to, int family)
{
    char ascii_port[20];
    struct addrinfo hints, *p_res;

    // The socket is not yet created
    m_is_created = false;

    // Close this socket if it happens to be open
    close();

    // Get a pointer to the IP address we want to bind to
    const char* bind_addr = bind_to.empty() ? nullptr : bind_to.c_str();

    // Get an ASCII version of the port number
    sprintf(ascii_port, "%i", port);

    // We're going to build an IPv4/IPv6 TCP socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = family;  
    hints.ai_socktype = SOCK_STREAM;
    
    // Handle the case where we're not binding to a specific IP address
    if (bind_addr == nullptr) hints.ai_flags = AI_PASSIVE;  

    // Fetch important information about the socket we're going to create
    if (getaddrinfo(bind_addr, ascii_port, &hints, &p_res) != 0)
    {
        m_error_str = "failure on getaddrinfo()";
        m_error     = GETADDRINFO_FAILED;
        return false;
    }

    // If we didn't get a result from getaddrinfo, something's wrong
    if (p_res == nullptr)
    {
        m_error_str = "failure on getaddrinfo()";
        m_error     = GETADDRINFO_FAILED;
        return false;
    }

    // Save a copy of the results
    struct addrinfo res = *p_res;

    // Free the memory that was allocated by getaddrinfo
    freeaddrinfo(p_res);

    // Create the socket
    m_sd = socket(res.ai_family, res.ai_socktype, res.ai_protocol);

    // If the socket() call fails, complain
    if (m_sd < 0)
    {
        m_error_str = "failure on socket()";
        m_error     = SOCKET_FAILED;
        return false;
    }

    // Allow us to re-use this port if it's still in TIME_WAIT
    int optval = 1;
    setsockopt(m_sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    // Bind it to the port we passed in to getaddrinfo():
    if (bind(m_sd, res.ai_addr, res.ai_addrlen) < 0)
    {
        m_error_str = "failure on bind()";
        m_error     = BIND_FAILED;
        close();
        return false;
    }

    // This socket has been created
    m_is_created = true;

    // Tell the caller that all is well
    return true;
}
//==========================================================================================================



//==========================================================================================================
// listen_and_accept() - Starts listening for connections and waits for a client to connect to our socket
//==========================================================================================================
bool NetSock::listen_and_accept(NetSock* new_sock)
{
    // If the socket isn't created yet, don't even try 
    if (!m_is_created) return false;

    // Start listening for an incoming connection
    int status = listen(m_sd, 0);

    // If listen() barfed on us, tell the caller
    if (status < 0)
    {
        m_error_str = "failure on listen()";
        m_error     = LISTEN_FAILED;
        return false;
    }
    
    // Accept the connection
    int new_sd = accept(m_sd, nullptr, 0);

    // If accept() failed, tell the caller
    if (new_sd < 0)
    {
        m_error_str = "failure on accept()";
        m_error     = ACCEPT_FAILED;
        return false;
    }

    // If the caller passed us a socket object to clone ourselves into...
    if (new_sock)
    {
        *new_sock = *this;
        new_sock->m_sd = new_sd;
    }

    // Otherwise, the new socket-descriptor is the one we'll read and write on
    else
    {
        ::close(m_sd);
        m_sd = new_sd;        
    }

    // Tell the caller that all is well
    return true;
}
//==========================================================================================================


//==========================================================================================================
// get_peer_address() - Returns the IP address of the other side of the connection
//==========================================================================================================
string NetSock::get_peer_address(int family)
{
    sockaddr_in peer_addr;
    char ip[INET6_ADDRSTRLEN];

    // getpeername() will need to know the size of peer_addr
    socklen_t addr_size = sizeof(peer_addr);

    // Fetch the IP address of the machine on the other side of the socket
    if (getpeername(m_sd, (sockaddr*)&peer_addr, &addr_size) < 0) return "unknown";

    // Convert that address to an IP address
    inet_ntop(family, &(peer_addr.sin_addr), ip, sizeof(ip));

    // Hand the IP address of the connected client to the caller
    return ip;
}
//==========================================================================================================


//==========================================================================================================
// set_nagling() - Turn Nagle's algorithm on or off for this socket.
//
// When Nagle's algorithm is on, the socket will attempt to coalesce data before sending it to the network.
// When Nagle's algorithm is off, all writes will be sent to the network immediately
//==========================================================================================================
void NetSock::set_nagling(bool flag)
{
    setsockopt(m_sd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof flag);
}
//==========================================================================================================


//==========================================================================================================
// wait_for_data() - Waits for the specified amount of time for data to be available for reading
//
// Passed: timeout_ms = timeout in milliseconds.  -1 = Wait forever
//
// Returns: true if data is available for reading, else false
//==========================================================================================================
bool NetSock::wait_for_data(int timeout_ms)
{
    fd_set  rfds;
    timeval timeout;

    // Assume for the moment that we are going to wait forever
    timeval* pTimeout = NULL;

    // If the caller wants us to wait for a finite amount of time...
    if (timeout_ms != -1)
    {
        // Convert milliseconds to microseconds
        int usecs = timeout_ms * 1000;

        // Determine the timeout in seconds and microseconds
        timeout.tv_sec  = usecs / 1000000;
        timeout.tv_usec = usecs % 1000000;

        // Point to the timeout structure we just initialized
        pTimeout = &timeout;
    }

    // We'll wait on input from the file descriptor
    FD_ZERO(&rfds);
    FD_SET(m_sd, &rfds);

    // Wait for a character to be available for reading
    int status = select(m_sd+1, &rfds, NULL, NULL, pTimeout);

    // If status > 0, there is a character ready to be read
    return (status > 0);
}
//==========================================================================================================


//==========================================================================================================
// bytes_available() - Returns the number of bytes available for reading
//==========================================================================================================
int NetSock::bytes_available()
{
    int count = 0;
    ioctl(m_sd, FIONREAD, &count);
    return count;
}
//==========================================================================================================



//==========================================================================================================
// receive() - Receives data from the socket
//
// Passed:  buffer = Pointer to the place to store the received data
//          length = The number of bytes to read in
//          peek   = If true, the bytes will be returned but not removed from the buffer
//
// Returns: The number of bytes that were read
//             -- or -- -1 = An error occured
//             -- or --  0 = The socket was closed (possibly by the other side)
//==========================================================================================================
int NetSock::receive(void* buffer, int length, bool peek)
{
    // Don't attempt to recv zero byutes
    if (length == 0) return 0;

    // This is the set of flags that we're going to pass to recv
    int flags = peek ?  MSG_PEEK : 0;

    // Get a byte-pointer to the caller's buffer
    unsigned char* ptr = (unsigned char*)buffer;

    // Keep track of how many bytes we have left to read
    int bytes_remaining = length;

    // Loop until there are no more bytes to read...
    while (bytes_remaining)
    {
        // Fetch some bytes from the socket
        int bytes_rcvd = recv(m_sd, ptr, bytes_remaining, flags);

        // If the read failed, tell the caller
        if (bytes_rcvd < 0) return -1;

        // If the socket is closed, tell the caller
        if (bytes_rcvd == 0) return 0;

        // Adjust our pointer and the number of bytes remaining to be read
        ptr             += bytes_rcvd;
        bytes_remaining -= bytes_rcvd;
    }

    // Tell the caller that we received all of the data they wanted
    return length;
}
//==========================================================================================================


//==========================================================================================================
// get_line() - Fetches a line of text from the socket
//==========================================================================================================
bool NetSock::get_line(void* buffer, size_t buff_size)
{
    char c;

    // Don't let the caller pass us a buffer size of zero
    if (buff_size == 0) return false;

    // Reduce the buffer size by 1 to allow for appending the nul-byte to the end of it
    --buff_size;

    // We'll be keeping track of how many characters are in the buffer
    int line_length = 0;

    // Get a byte pointer to the caller's buffer
    unsigned char* ptr = (unsigned char*) buffer;

    // Loop until either an error or until we see a linefeed
    while (true)
    {
        // Fetch a single byte from the socket
        if (recv(m_sd, &c, 1, 0) < 1) return false;

        // If it's a carriage-return, throw it away
        if (c == '\r') continue;

        // If it's a backspace, adjust our pointer
        if (c == 8)
        {
            if (line_length)
            {
                --line_length;
                --ptr;
            }
            continue;            
        }

        // If it's a line-feed, it's the end of the line
        if (c == '\n') break;

        // If this character will fit into the caller's buffer, append it there
        if (line_length < buff_size)
        {
            *ptr++ = c;
            line_length ++;
        }
    }

    // We've encountered the end of the line.  Terminate the output string
    *ptr = 0;

    // And tell the caller that he has a line of data waiting in his buffer
    return true;
}
//==========================================================================================================


//==========================================================================================================
// send() - Sends a string to the other side of a connected socket
//
// Returns either : -1 = An error occured
//                  Anything else = the number of bytes actually sent.  The entire string will always be
//                  sent unless the socket was closed by the other side
//==========================================================================================================
int NetSock::send(string s)
{
    const char* p = s.c_str();
    int length    = s.size();
    return send(p, length);
}
//==========================================================================================================


//==========================================================================================================
// send() - Sends a buffer to the other side of a connected socket
//
// Returns either : -1 = An error occured
//                  Anything else = the number of bytes actually sent.  The entire string will always be
//                  sent unless the socket was closed by the other side
//==========================================================================================================
int NetSock::send(const void* buffer, int length)
{
    // Don't attempt to send zero bytes
    if (length == 0) return 0;

    // Get a byte pointer to the caller's buffer
    unsigned char* ptr = (unsigned char*)buffer;

    // Keep track of how many bytes remain to be sent
    int bytes_remaining = length;

    // Loop until there are no more bytes to send...
    while (bytes_remaining)
    {
        // Attempt to send all of the bytes
        int sent = ::send(m_sd, ptr, bytes_remaining, 0);

        // If an error occured, tell the caller
        if (sent < 0) return -1;

        // If the socket is closed, we're done
        if (sent == 0) break;

        // Adjust the pointer and the count of bytes remaining to be sent
        ptr             += sent;
        bytes_remaining -= sent;
    }

    // Tell the caller how many bytes we sent
    return (length - bytes_remaining);
}
//==========================================================================================================



//==========================================================================================================
// sendf() - Sends a printf-style formatt data to the the other side of a connected socket
//
// Returns either : -1 = An error occured
//                  Anything else = the number of bytes actually sent.  The entire string will always be
//                  sent unless the socket was closed by the other side
//==========================================================================================================
int NetSock::sendf(const char* fmt, ...)
{
    char buffer[2000];

    // This is a pointer to the variable argument list
    va_list ap;

    // Point to the first argument after the "fmt" parameter
    va_start(ap, fmt);

    // Perform a printf of our arguments into the buffer area;
    vsprintf(buffer, fmt, ap);

    // Tell the system that we're done with the "ap"
    va_end(ap);

    // And send the buffer
    return send(buffer, strlen(buffer));
}
//==========================================================================================================



//==========================================================================================================
// get_error() - Returns information about the most recent failure
//==========================================================================================================
int NetSock::get_error(string* p_str)
{
    if (p_str) *p_str = m_error_str;
    return m_error;
}
//==========================================================================================================



