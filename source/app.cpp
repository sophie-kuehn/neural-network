#include <iostream>
#include <stdexcept>
#include "../header/app.hpp"
#include "../header/snn.hpp"
#include "../header/sclt.hpp"
#include "../header/mnist.hpp"
#include "../header/sts.hpp"

namespace SNN
{
    class TcpListener : public STS::TcpListener
    {
    public:
        void processRequest(
            STS::TcpRequest* request,
            STS::TcpResponse* response
        ) override
        {
            response->body = {"Your request was: " + request->body};
        };
    };

    int CliApp::main(int argc, char **argv)
    {
        auto args = new SCLT::CliArguments(argc, argv, {
            {'f', "file", "file for storing network", true},
            {'n', "network", "network definition (e.g. \"3;10,sigmoid;1\"; not used when --file exists!)", true},
            {'i', "input", "input (e.g. \"1,1,1;2,2,2;3,3,3\")", true},
            {'e', "expected", "expected output (e.g. \"3;6;9\")", true},
            {'p', "epsilon", "epsilon value for training (default 0.01)", true},
            {'m', "mnist", "specify data directory to run MNIST test", true},
            {'s', "server", "specify port to run in server mode", true},
            {'h', "help", "blubb"}
        }, 25);

        auto network = new Network;

        try {
            if (args->has("mnist")) {
                if (!args->has("file")) {
                    throw std::invalid_argument("you have to provide --file");
                }

                auto MNIST = new MNIST_Test;
                MNIST->execute(args->get("file"), args->get("mnist") + "/");
                return 0;
            }

            if (args->has("server")) {
                auto listener = new TcpListener;
                auto server = new STS::TcpServer;
                server->addRequestEventListener(listener);
                server->listen(std::stoi(args->get("server")));
                return 0;
            }

            if (args->has("file") && FileExists(args->get("file"))) {
                network->load(args->get("file"));

            } else if (args->has("network")) {
                network->loadShort(args->get("network"));
                if (args->has("file")) network->store(args->get("file"));

            } else {
                throw std::invalid_argument("you have to provide --file or --network");
            }

            double epsilon = SNN_DEFAULT_EPSILON;
            if (args->has("epsilon")) epsilon = std::stod(args->get("epsilon"));

            Checks checks;

            if (args->has("input")) {
                auto checksTmp = SplitString(args->get("input"), SNN_INPUT_DELIMITER_L1);
                for (const auto& inputTmp : checksTmp) {
                    Check check;
                    auto inputTmp2 = SplitString(inputTmp, SNN_INPUT_DELIMITER_L2);
                    for (const auto& value : inputTmp2) check.input.push_back(std::stod(value));
                    checks.push_back(check);
                }
            }

            if (args->has("expected")) {
                auto checksTmp = SplitString(args->get("expected"), SNN_INPUT_DELIMITER_L1);
                int i = 0;
                for (const auto& expectedTmp : checksTmp) {
                    auto expectedTmp2 = SplitString(expectedTmp, SNN_INPUT_DELIMITER_L2);
                    for (const auto& value : expectedTmp2) checks[i].expected.push_back(std::stod(value));
                    i++;
                }
            }

            for (const auto& check : checks) {
                DoubleVector output = network->process(check.input, check.expected, epsilon);
                SCLT::COutDoubleVector(check.input, false);
                std::cout << ",";
                SCLT::COutDoubleVector(check.expected, false);
                std::cout << ",";
                SCLT::COutDoubleVector(output);
            }


            if (checks.size() > 0 && args->has("file")) {
                network->store(args->get("file"));
            }

        } catch (std::exception& e) {
            std::cout << e.what() << "\n" << args->getHelp(25) << std::endl;
            return EXIT_FAILURE;
        }

        return 0;
    };
};