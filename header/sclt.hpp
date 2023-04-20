#ifndef SCLT_HPP
#define SCLT_HPP

#include <getopt.h>
#include <vector>
#include <map>
#include <string>

namespace SCLT
{
    typedef std::vector<double> DoubleVector;

    void COutDoubleVector(DoubleVector vector, bool endl = true);

    struct CliOption {
        char shortOption;
        std::string longOption;
        std::string description = "";
        bool requireArgument = false;
        bool requireOption = false;
    };

    typedef std::map<std::string, std::string> StringMap;
    typedef std::vector<CliOption> CliOptionSet;

    class CliArguments {
    public:
        CliOptionSet options;
        StringMap arguments;
        CliArguments(int argc, char** argv, CliOptionSet options, int helpPadding = 30);
        std::string getShortOptions();
        std::vector<option> getLongOptions();
        std::string getHelp(int padding = 30);
        bool has(std::string option);
        std::string get(std::string option);
    };
}

#endif
