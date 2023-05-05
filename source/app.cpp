#include <iostream>
#include <stdexcept>
#include "../header/app.hpp"
#include "../header/snn.hpp"
#include "../header/sclt.hpp"
#include "../header/sts.hpp"

namespace SNN
{
    void TcpListener::processRequest(
        STS::TcpRequest* request,
        STS::TcpResponse* response
    )
    {
        this->app->arguments->set("checks", request->body);
        auto checks = this->app->process();
        for (auto& check : checks) {
            response->body += check.toString() + "\n";
        }
    };

    std::string Check::toString()
    {
        SCLT::PBag outputBag;
        outputBag.insert(SCLT::dvtosv(this->input));
        outputBag.insert(SCLT::dvtosv(this->expected));
        outputBag.insert({std::to_string(this->epsilon)});
        outputBag.insert(SCLT::dvtosv(this->output));
        return outputBag.toString(SCLT_PBAG_2_DELIMITER);
    };

    int CliApp::main(int argc, char **argv)
    {
        this->network = new Network;
        this->arguments = new SCLT::CliArguments(argc, argv, {
            {'f', "file", "file for storing network", true},
            {'n', "network", "network definition (e.g. \"3;10,sigmoid;1\"; not used when --file exists!)", true},
            {'c', "checks", "checks to run (e.g. \"1,1,1;3;0.01_1,2,3;6:0.01\")", true},
            {'s', "server", "specify port to run in server mode", true},
            {'h', "help", "blubb"}
        }, 25);

        try {
            if (this->arguments->has("file")
                && SCLT::FileExists(this->arguments->get("file"))
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

            for (auto& check : checks) {
                std::cout << check.toString() << std::endl;
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

        if (this->arguments->has("checks")) {
            auto checksInput = SCLT::PBag::fromString(
                this->arguments->get("checks"),
                SCLT_PBAG_3_DELIMITER
            );

            for (auto& checkInput : checksInput) {
                Check check;
                check.epsilon = SNN_DEFAULT_EPSILON;

                if (checkInput.size() < 1) continue;
                check.input = checkInput[0].toDoubleVector();

                if (checkInput.size() > 1) {
                    check.expected = checkInput[1].toDoubleVector();
                }

                if (checkInput.size() > 2) {
                    check.epsilon = std::stod(checkInput[2][0].value);
                }

                checks.push_back(check);
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
