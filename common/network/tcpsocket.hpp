#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include <string>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h>

using namespace std;

class TCPSocket {

private:
    int fd_;
    /* maximum size of a read */
    const static size_t BUFFER_SIZE = 1024 * 1024;

    string::const_iterator write_helper( const string::const_iterator & begin,
                                       const string::const_iterator & end )
    {
        ssize_t bytes_written = write( fd_, &*begin, end - begin );
        return begin + bytes_written;
    }
    
public:

    TCPSocket() {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
    }

    TCPSocket(int fd) {
        fd_ = fd;
    }

    // Destructor
    // Close the file descriptor
    ~TCPSocket() {
        close(fd_);
    }

    // Bind socket to a specified local ip and port
    void socket_bind(string ip, int port) {
        sockaddr_in local_addr; 
        local_addr.sin_family = AF_INET; 
        local_addr.sin_port = htons(port); 
        local_addr.sin_addr.s_addr = inet_addr(ip.c_str()); 
        bind(fd_, (struct sockaddr*)&local_addr, sizeof(local_addr)); 
    }

    // Listen for a connection. It is typically called from the server socket.
    void socket_listen(const int backlog = 16) {
        listen(fd_, backlog);
    }

    // Connect the socket to a specified peer addr
    void socket_connect(string ip, int port) {
        sockaddr_in peer_addr; 
        peer_addr.sin_family = AF_INET; 
        peer_addr.sin_port = htons(port); 
        peer_addr.sin_addr.s_addr = inet_addr(ip.c_str()); 
        connect(fd_, (struct sockaddr*)&peer_addr, sizeof(peer_addr));
    }

    // Accept connect from the client. It is typically called from the server socket.
    int socket_accept() {
        int fd = accept(fd_, nullptr, nullptr);
        return fd;
    }

    // Allow the reuse of local addresses
    void socket_set_reuseaddr() {
        const int enable = 1;
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    }

    // Read data from the socket
    string read_data(const size_t limit = BUFFER_SIZE) {
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read( fd_, buffer, min( BUFFER_SIZE, limit ) );
        return string( buffer, bytes_read );
    }
    
    // Write data to the socket
    void write_data(const string & buffer) {
        auto it = buffer.begin();
        do {
            it = write_helper( it, buffer.end() );
        } while ( it != buffer.end() );
    }

    /* accessors */
    string local_address( void ) const {
        struct sockaddr_in local_address;
        socklen_t size = sizeof( local_address );
        getsockname(fd_, (struct sockaddr*)&local_address, &size);
        char* local_ip = inet_ntoa(local_address.sin_addr);
        int local_port = ntohs(local_address.sin_port);
        return string(local_ip) + ":" + to_string(local_port);
    }

    string peer_address( void ) const {
        struct sockaddr_in peer_address;
        socklen_t size = sizeof( peer_address );
        getpeername(fd_, (struct sockaddr*)&peer_address, &size);
        char* peer_ip = inet_ntoa(peer_address.sin_addr);
        int peer_port = ntohs(peer_address.sin_port);
        return string(peer_ip) + ":" + to_string(peer_port);
    }

};

#endif /* TCPSOCKET_HPP */