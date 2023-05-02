#ifndef SNN_APP_HPP
#define SNN_APP_HPP

#include <vector>
#include <string>
#include "sclt.hpp"
#include "snn.hpp"
#include "sts.hpp"

namespace SNN
{
    class Check
    {
    public:
        SCLT::DoubleVector input;
        SCLT::DoubleVector expected;
        SCLT::DoubleVector output;
        double epsilon = SNN_DEFAULT_EPSILON;
        std::string toString();
    };

    typedef std::vector<Check> Checks;

    class CliApp
    {
    public:
        Network* network;
        SCLT::CliArguments* arguments;
        int main(int argc, char **argv);
        Checks process();
    };

    class TcpListener : public STS::TcpListener
    {
    public:
        CliApp* app;
        void processRequest(STS::TcpRequest* request, STS::TcpResponse* response) override;
    };
};

#endif
