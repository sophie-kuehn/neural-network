#include <iostream>
#include "../header/sclt.hpp"

#define SCLT_PARAM_BAG_L1_DELIMITER ';'
#define SCLT_PARAM_BAG_L2_DELIMITER ','

namespace SCLT
{
    StringVector dvtosv(DoubleVector in)
    {
        StringVector s;
        for (const auto& d : in) s.push_back(std::to_string(d));
        return s;
    };

    DoubleVector svtodv(StringVector in)
    {
        DoubleVector d;
        for (const auto& s : in) {
            double sd = 0;
            try { sd = std::stod(s); } catch (...) {}
            d.push_back(sd);
        }
        return d;
    };

    StringVector SplitString(const std::string &s, char delim)
    {
        StringVector result;
        std::stringstream ss(s);
        std::string item;

        while (getline(ss, item, delim)) {
            result.push_back(item);
        }

        return result;
    };

    ParamBag DecodeParamBag(std::string in, char l1delim, char l2delim)
    {
        ParamBag bag;
        StringVector l1 = SplitString(in, l1delim);
        for (const auto& l1in : l1) {
            bag.push_back({});
            StringVector l2 = SplitString(l1in, l2delim);
            for (const auto& l2in : l2) {
                if (l2in.size() > 0) bag.back().push_back(l2in);
            }
        }
        return bag;
    }

    std::string EncodeParamBag(ParamBag in, char l1delim, char l2delim)
    {
        std::string out;
        for (const auto& l1 : in) {
            std::string l2out;
            for (const auto& l2 : l1) {
                if (l2out.size() != 0) l2out += l2delim;
                l2out += l2;
            }
            if (out.size() != 0) out += l1delim;
            out += l2out;
        }
        return out;
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

    void CliArguments::set(std::string option, std::string value)
    {
        this->arguments[option] = value;
    };

    void CliArguments::unset(std::string option)
    {
        auto iterator = this->arguments.find(option);
        if (iterator != this->arguments.end()) {
            this->arguments.erase(iterator);
        }
    };
}
