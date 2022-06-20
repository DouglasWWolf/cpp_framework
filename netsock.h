//==========================================================================================================
// netsock.h - Defines a network socket
//==========================================================================================================
#pragma once
#include <netinet/in.h>
#include <string>

class NetSock
{
public:

    // The are the codes that can be returned by get_error()
    enum
    {
        GETADDRINFO_FAILED,
        SOCKET_FAILED,
        BIND_FAILED,
        LISTEN_FAILED,
        ACCEPT_FAILED,
        NO_SUCH_SERVER,
        CANT_CONNECT
    };


    // Constructor and Destructor
    NetSock();
    ~NetSock() {close();}

    // Call this to create a server socket
    bool    create_server(int port, std::string bind_to = "", int family = AF_UNSPEC);

    // Call this to listen for connections and wait for someone to connect
    bool    listen_and_accept(NetSock* new_sock = nullptr);

    // Call this to connect to a server
    bool    connect(std::string server_name, int port);

    // Call this to turn Nagle's algorithm on or off
    void    set_nagling(bool flag);

    // After an "accept()", call this to find the IP address of the client
    std::string get_peer_address(int family = AF_INET);

    // Waits for data to arrive.  Returns 'true' if data became available before the timeout expires
    bool    wait_for_data(int milliseconds);

    // Returns the number of bytes available for reading 
    int     bytes_available();

    // Call this to receive data from the socket
    int     receive(void* buffer, int length, bool peek = false);

    // Call this to fetch a line of text from the socket
    bool    get_line(void* buffer, size_t buff_size);

    // Call these to send a string or buffer full of data
    int     send(std::string s);
    int     send(const void* buffer, int length);

    // Call this to send data using print-style formatting
    int     sendf(const char* fmt, ...);

    // Call this to close this socket.  Safe to call if socket isn't open
    void    close();

    // When connect(), create() (etc) fail, this will give information about the error
    int     get_error(std::string* p_str = nullptr);

protected:

    std::string m_error_str;
    int         m_error;

    // This will be true on a socket for which create_server() or connect() has been called
    bool    m_is_created;

    // The socket descriptor of our socket
    int     m_sd;
};