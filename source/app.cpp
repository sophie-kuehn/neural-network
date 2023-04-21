#include <iostream>
#include <stdexcept>
#include "../header/app.hpp"
#include "../header/snn.hpp"
#include "../header/sclt.hpp"
#include "../header/mnist.hpp"
#include "../header/sts.hpp"

namespace SNN
{
    void TcpListener::processRequest(
        STS::TcpRequest* request,
        STS::TcpResponse* response
    )
    {
        auto requestArgs = SCLT::SplitString(
            request->body,
            '_'
        );

        this->app->arguments->set("input", requestArgs[0]);

        if (requestArgs.size() > 1) {
            this->app->arguments->set("expected", requestArgs[1]);
        } else {
            this->app->arguments->unset("expected");
        }

        if (requestArgs.size() > 2) {
            this->app->arguments->set("epsilon", requestArgs[2]);
        } else {
            this->app->arguments->unset("epsilon");
        }

        auto checks = this->app->process();

        for (const auto& check : checks) {
            response->body += CheckToString(check) + "\n";
        }
    };

    std::string CheckToString(Check check)
    {
        SCLT::ParamBag outputBag;
        outputBag.push_back(SCLT::dvtosv(check.input));
        outputBag.push_back(SCLT::dvtosv(check.expected));
        outputBag.push_back({std::to_string(check.epsilon)});
        outputBag.push_back(SCLT::dvtosv(check.output));
        return SCLT::EncodeParamBag(outputBag);
    };

    int CliApp::main(int argc, char **argv)
    {
        this->network = new Network;
        this->arguments = new SCLT::CliArguments(argc, argv, {
            {'f', "file", "file for storing network", true},
            {'n', "network", "network definition (e.g. \"3;10,sigmoid;1\"; not used when --file exists!)", true},
            {'i', "input", "input (e.g. \"1,1,1;2,2,2;3,3,3\")", true},
            {'e', "expected", "expected output (e.g. \"3;6;9\")", true},
            {'p', "epsilon", "epsilon value for training (default 0.01)", true},
            {'m', "mnist", "specify data directory to run MNIST test", true},
            {'s', "server", "specify port to run in server mode", true},
            {'h', "help", "blubb"}
        }, 25);

        try {
            if (this->arguments->has("mnist")) {
                if (!this->arguments->has("file")) {
                    throw std::invalid_argument("you have to provide --file");
                }

                auto MNIST = new MNIST_Test;
                MNIST->execute(this->arguments->get("file"), this->arguments->get("mnist") + "/");
                return 0;
            }

            if (this->arguments->has("file")
                && FileExists(this->arguments->get("file"))
            ) {
                this->network->load(this->arguments->get("file"));

            } else if (this->arguments->has("network")) {
                this->network->loadShort(this->arguments->get("network"));
                if (this->arguments->has("file")) {
                    this->network->store(this->arguments->get("file"));
                }

            } else {
                throw std::invalid_argument("you have to provide --file or --network");
            }

            if (this->arguments->has("server")) {
                auto listener = new TcpListener;
                listener->app = this;
                auto server = new STS::TcpServer;
                server->addRequestEventListener(listener);
                server->listen(std::stoi(this->arguments->get("server")));
                return 0;
            }

            auto checks = this->process();

            for (const auto& check : checks) {
                std::cout << CheckToString(check) << std::endl;
            }

        } catch (std::exception& e) {
            std::cout << e.what() << "\n" << this->arguments->getHelp(25) << std::endl;
            return EXIT_FAILURE;
        }

        return 0;
    };

    Checks CliApp::process()
    {
        Checks checks;

        double epsilon = SNN_DEFAULT_EPSILON;
        if (this->arguments->has("epsilon")) {
            epsilon = std::stod(this->arguments->get("epsilon"));
        }

        if (this->arguments->has("input")) {
            auto inputBag = SCLT::DecodeParamBag(this->arguments->get("input"));

            for (const auto& inputTmp : inputBag) {
                Check check;
                check.epsilon = epsilon;
                check.input = SCLT::svtodv(inputTmp);
                checks.push_back(check);
            }
        }

        if (this->arguments->has("expected")) {
            auto expectedBag = SCLT::DecodeParamBag(this->arguments->get("expected"));

            int i = 0;
            for (const auto& expectedTmp : expectedBag) {
                checks[i].expected = SCLT::svtodv(expectedTmp);
                i++;
            }
        }

        for (auto& check : checks) {
            check.output = this->network->process(
                check.input,
                check.expected,
                check.epsilon
            );
        }

        if (checks.size() > 0 && this->arguments->has("file")) {
            this->network->store(this->arguments->get("file"));
        }

        return checks;
    };
};
