#include "socket/socket.hpp"

bool 
Socket::Initialize() const
{
#if defined(_WIN32)
    WSADATA d;
    if( WSAStartup(MAKEWORD(2, 2), &d) )
    {
        // Failed to initialize WinSocket
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

    if( getaddrinfo(0, "8080", &addrHints, &m_bindAddr) )
    {
        m_isInitialized = false;
        return;
    }

    m_sockId = socket(m_bindAddr->ai_family,
                      m_bindAddr->ai_socktype,
                      m_bindAddr->ai_protocol);
    if( !IsValidSocket(m_sockId) )
    {
        m_isInitialized = false;
        return;
    }

    // enable dual-stack socket to support IPV6 and IPV4
    m_isDualStack = false;
    if( addrHints.ai_family == AF_INET6 
        && TryDualStackConfig() )
    {
        m_isDualStack = true;
    }

    if( !this->Bind() )
    {
        m_isInitialized = false;
        return;
    }

    m_isInitialized = true;
}

bool
Socket::TryDualStackConfig()
{
#if defined(_WIN32)
    char const option = 0;
#else
    int _temp = 0;
    auto* option = reinterpret_cast<void*>(&_temp);
#endif

    int retval = setsockopt(m_sockId, IPPROTO_IPV6, IPV6_V6ONLY, &option, sizeof(option));
    return !retval;
}

bool 
Socket::IsSucessfullInit() const
{
    return m_isInitialized;
}

Socket::~Socket()
{
    this->CleanUp();
}

bool 
Socket::IsValidSocket(SOCKET socketId)
{
#if defined(_WIN32)
    return socketId != INVALID_SOCKET;
#else
    return socketId >= 0;
#endif
}

bool
Socket::IsDualStack() const
{
    return m_isDualStack;
}

bool 
Socket::Bind() const
{
    int retval = bind(m_sockId,
                      m_bindAddr->ai_addr,
                      m_bindAddr->ai_addrlen);
    if( retval )
        return false;

    return true;
}

bool 
Socket::Listen() const
{
    int retval = listen(m_sockId, Socket::m_queueSize);
    if( retval < 0 )
        return false;

    return true;
}

Socket::ConnectionData 
Socket::Accept() const
{
    Socket::ConnectionData retdata;
    struct sockaddr_storage& clientAddr = retdata.connectionAddr;
    retdata.addrLength = sizeof(clientAddr);

    SOCKET& clientSocketId = retdata.socketId;
    clientSocketId = accept(m_sockId, (struct sockaddr*) &clientAddr, &(retdata.addrLength));
    if( !IsValidSocket(clientSocketId) )
        return retdata;

    retdata.isValid = true;
    return retdata;
}

int 
Socket::GetErrorNum() const
{
#if defined(_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

SOCKET 
Socket::GetSocketId() const
{
    return m_sockId;
}

void 
Socket::Close(SOCKET sockId)
{
#if defined(_WIN32)
    closesocket(sockId);
#else
    close(sockId);
#endif
}

void 
Socket::CleanUp()
{
    freeaddrinfo(m_bindAddr);
    Socket::Close(m_sockId);
#if defined(_WIN32)
    WSACleanup();
#endif
}

Socket::StringPair
Socket::GetConnectionInfo(Socket::ConnectionData const& connectionData, bool numeric)
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
