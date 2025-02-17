#pragma once

#if defined(_WIN32)
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment( lib, "ws2_32.lib" )
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
#endif

#if !defined(_WIN32)
    #define SOCKET int
#endif

#include <utility>

class Socket
{
public:
    using AddrInfo = struct addrinfo;

    struct ConnectionData
    {
        bool isValid = false;
        struct sockaddr_storage connectionAddr;
        socklen_t addrLength;
        SOCKET socketId;
    };

    static std::pair<char const*, char const*> GetConnectionInfo(Socket::ConnectionData const& connectionData, bool numeric = true);
    static void Close(SOCKET sockId);
    static bool IsValidSocket(SOCKET socketId);

public:
    Socket(Socket::AddrInfo&& addrHints);
    ~Socket();

    Socket(Socket const& ) = default;
    Socket& operator=(Socket const& ) = default;
    Socket(Socket&& ) = default;
    Socket& operator=(Socket&& ) = default;

    bool                    IsSucessfullInit() const;
    bool                    Bind() const;
    bool                    Listen() const;
    Socket::ConnectionData  Accept() const;
    int                     GetErrorNum() const;
    SOCKET                  GetSocketId() const;
    
private:
    AddrInfo*   m_bindAddr;
    SOCKET      m_sockId;
    bool        m_isInitialized;

    static constexpr int m_queueSize = 10;

private:
    bool Initialize() const;
    void CleanUp();
};
