#include "socket/socket.hpp"

#include <iostream>

bool Socket::Initialize() const
{
#if defined(_WIN32)
    WSADATA d;
    if( WSAStartup(MAKEWORD(2, 2), &d) )
    {
        std::cerr << "Failed to intialize WinSock.\n";
        return false;
    }
#endif
    return true;
}

Socket::Socket(Socket::AddrInfo&& addrHints)
{
    if( !this->Initialize() )
    {
        m_isInitialized = false;
        return;
    }

    getaddrinfo(0, "8080", &addrHints, &m_bindAddr);

    m_sockId = socket(m_bindAddr->ai_family,
                      m_bindAddr->ai_socktype,
                      m_bindAddr->ai_protocol);
    if( !IsValidSocket(m_sockId) )
    {
        std::cerr << "socket() creation failed. "
            << GetErrorNum() << "\n";
        m_isInitialized = false;
        return;
    }

    m_isInitialized = true;
}

bool Socket::IsSucessfullInit() const
{
    return m_isInitialized;
}

Socket::~Socket()
{
    this->CleanUp();
}

bool Socket::IsValidSocket(SOCKET socketId)
{
#if defined(_WIN32)
    return socketId != INVALID_SOCKET;
#else
    return socketId >= 0;
#endif
}

bool Socket::Bind() const
{
    int retval = bind(m_sockId,
                      m_bindAddr->ai_addr,
                      m_bindAddr->ai_addrlen);
    if( retval )
    {
        std::cerr << "bind() failed. "
            << GetErrorNum() << "\n";
        return false;
    }

    return true;
}

bool Socket::Listen() const
{
    std::cout << "Listening ...\n";
    int retval = listen(m_sockId, Socket::m_queueSize);
    if( retval < 0 )
    {
        std::cerr << "listen() failed."
            << GetErrorNum() << "\n";
        return false;
    }

    return true;
}

Socket::ConnectionData Socket::Accept() const
{
    Socket::ConnectionData retdata;
    struct sockaddr_storage& clientAddr = retdata.connectionAddr;
    retdata.addrLength = sizeof(clientAddr);

    SOCKET& clientSocketId = retdata.socketId;
    clientSocketId = accept(m_sockId, (struct sockaddr*) &clientAddr, &(retdata.addrLength));
    if( !IsValidSocket(clientSocketId) )
    {
        std::cerr << "accept() failed to make new connection. "
            << GetErrorNum() << "\n";
        return retdata;
    }

    retdata.isValid = true;
    return retdata;
}

int Socket::GetErrorNum() const
{
#if defined(_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

SOCKET Socket::GetSocketId() const
{
    return m_sockId;
}

void Socket::Close(SOCKET sockId)
{
#if defined(_WIN32)
    closesocket(sockId);
#else
    close(sockId);
#endif
}

void Socket::CleanUp()
{
    freeaddrinfo(m_bindAddr);
    Socket::Close(m_sockId);
#if defined(_WIN32)
    WSACleanup();
#endif
}

std::pair<char const*, char const*> Socket::GetConnectionInfo(Socket::ConnectionData const& connectionData, bool numeric)
{
    char addrBuffer[100];
    char serviceBuffer[100];
    auto* clientAddr = reinterpret_cast<struct sockaddr const*>( &connectionData.connectionAddr );
    getnameinfo(clientAddr, connectionData.addrLength, 
                addrBuffer, sizeof(addrBuffer), 
                serviceBuffer, sizeof(serviceBuffer), 
                numeric ? NI_NUMERICHOST : NI_NUMERICHOST);
    return { addrBuffer, serviceBuffer };
}
