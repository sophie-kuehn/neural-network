#ifndef STS_HPP
#define STS_HPP

#include <string>
#include <vector>

#define STC_DEFAULT_PORT 8000
#define STC_HOLD_CONNECTIONS 10
#define STC_REQUEST_BUFFER_SIZE 1000

namespace STS
{
    struct TcpRequest { std::string body;};
    struct TcpResponse { std::string body; };

    class TcpListener
    {
    public:
        virtual void processRequest(TcpRequest* request, TcpResponse* response) = 0;
    };

    class TcpServer
    {
    protected:
        std::vector<TcpListener*> requestEventListener;
        TcpResponse* processRequest(TcpRequest* request);

    public:
        void addRequestEventListener(TcpListener* listener);
        void listen(int port = STC_DEFAULT_PORT);
    };
};

#endif
