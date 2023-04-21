#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include "../header/sts.hpp"

namespace STS
{
    void TcpServer::addRequestEventListener(TcpListener* listener)
    {
        this->requestEventListener.push_back(listener);
    };

    TcpResponse* TcpServer::processRequest(TcpRequest* request)
    {
        auto response = new TcpResponse;
        for (const auto& listener : this->requestEventListener) {
            listener->processRequest(request, response);
        }
        return response;
    };

    void TcpServer::listen(int port)
    {
        // Create a socket (IPv4, TCP)
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            throw std::invalid_argument("Failed to create socket. errno: "
                + std::to_string(errno));
        }

        sockaddr_in sockaddr;
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        sockaddr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
            throw std::invalid_argument("Failed to bind to port "
                + std::to_string(port) + ". errno: "
                + std::to_string(errno));
        }

        if (::listen(sockfd, STC_HOLD_CONNECTIONS) < 0) {
            throw std::invalid_argument("Failed to listen on socket. errno "
                + std::to_string(errno));
        }

        auto addrlen = sizeof(sockaddr);

        while(true) {
            int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
            if (connection < 0) {
                throw std::invalid_argument("Failed to grab connection. errno "
                    + std::to_string(errno));
            }

            char buffer[STC_REQUEST_BUFFER_SIZE];
            auto bytesRead = read(connection, buffer, sizeof(buffer));
            if (bytesRead > 0) buffer[bytesRead] = '\0';
            auto request = new TcpRequest;
            request->body = buffer;
            auto response = this->processRequest(request);
            send(connection, response->body.c_str(), response->body.size(), 0);
            close(connection);
        }

        close(sockfd);
    };
};
