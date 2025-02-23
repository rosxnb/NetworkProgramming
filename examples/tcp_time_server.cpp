#include <socket/socket.hpp>

#include <iostream>
#include <chrono>
#include <ctime>
#include <cstring>


int main()
{
    std::cout << "Configuring local address ...\n";
    Socket::AddrInfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::cout << "Creating socket ...\n";
    Socket socket(std::move(hints));
    std::cout << "Socket created sucessfully." << " Socket type: "
        << (socket.IsDualStack() ? "Dual Stack" : "Single Stack") << "\n";

    if( !socket.IsSucessfullInit() )
    {
        std::cerr << "Socket creation failed. exiting program ...\n";
        return -1;
    }

    std::cout << "Listening for client connection ...\n";
    if( !socket.Listen() )
        return -1;

    auto connectionData = socket.Accept();
    if( connectionData.isValid )
    {
        std::cout << "Client is connected ...\n";
        auto [clientAddr, serviceName] = Socket::GetConnectionInfo(connectionData);
        std::cout << " client address: " << clientAddr << "\n";
        std::cout << " service name: " << serviceName << "\n";
    }
    else return -1;

    SOCKET& clientSocket = connectionData.socketId;
    std::cout << "Reading request ... \n";
    char request[1024];
    int bytesReceived = recv(clientSocket, request, 1024, 0);
    std::cout << "Received " << bytesReceived << " bytes.\n";
    std::cout.write(request, bytesReceived);

    std::cout << "Sending response ...\n";
    char const* response =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
    int bytesSent = send(clientSocket, response, strlen(response), 0);
    std::cout << "Sent " << bytesSent << " of " << strlen(response) << " bytes.\n";

    auto curtime = std::chrono::system_clock::now();
    auto timeP = std::chrono::system_clock::to_time_t(curtime);
    char* timeMsg = std::ctime(&timeP);
    bytesSent = send(clientSocket, timeMsg, strlen(timeMsg), 0);
    std::cout << "Sent " << bytesSent << " of " << strlen(timeMsg) << " bytes.\n";

    std::cout << "Closing connection\n";
    Socket::Close(clientSocket);
}
