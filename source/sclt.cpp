#include <iostream>
#include "../header/sclt.hpp"

namespace SCLT
{
    void COutDoubleVector(DoubleVector vector, bool endl)
    {
        std::cout << "{ ";
        for (int i = 0; i < vector.size(); i++) {
            std::cout << std::to_string(vector[i]);
            if (i < vector.size()-1) std::cout << ", ";
        }
        std::cout << " }" << std::flush;
        if (endl) std::cout << std::endl;
    };

    std::string CliArguments::getShortOptions()
    {
        std::string shortOptions;
        for (const auto& option : this->options) {
            shortOptions += option.shortOption;
            if (option.requireArgument) shortOptions += ":";
        }
        return shortOptions;
    };

    std::vector<option> CliArguments::getLongOptions()
    {
        std::vector<option> longOptions;
        for (const auto& option : this->options) {
            longOptions.push_back({
                option.longOption.c_str(),
                option.requireArgument ? required_argument : no_argument,
                nullptr,
                option.shortOption
            });
        }
        return longOptions;
    };

    std::string CliArguments::getHelp(int padding)
    {
        std::string help;
        for (const auto& option : this->options) {
            std::string line = "--" + option.longOption
                + ",-" + option.shortOption
                + (option.requireArgument ? " <arg>" : "");

            int linePadding = padding;
            linePadding -= line.size();
            if (linePadding < 0) linePadding = 0;
            line.insert(line.end(), linePadding, ' ');

            line += ": " + option.description
                + (option.requireOption ? " (required)" : "")
                + "\n";
            help += line;
        }
        return help;
    };

    CliArguments::CliArguments(int argc, char** argv, CliOptionSet options, int helpPadding)
    {
        this->options = options;

        while (true) {
            const auto opt = getopt_long(
                argc,
                argv,
                this->getShortOptions().c_str(),
                this->getLongOptions().data(),
                nullptr
            );

            if (-1 == opt) break;

            CliOption option = {'h'};
            for (const auto& possibleOption : this->options) {
                if (possibleOption.shortOption == opt) option = possibleOption;
            }

            if (option.shortOption == 'h') {
                std::cout << this->getHelp(helpPadding) << std::endl;
                exit(EXIT_FAILURE);
            }

            this->arguments[option.longOption] = option.requireArgument ? optarg : "1";
        }

        for (const auto& option : options) {
            if (option.requireOption && !this->has(option.longOption)) {
                std::cout << this->getHelp(helpPadding) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    };

    bool CliArguments::has(std::string option)
    {
        return this->arguments.find(option) != this->arguments.end();
    };

    std::string CliArguments::get(std::string option)
    {
        return this->arguments[option];
    };
}
