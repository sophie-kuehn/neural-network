#ifndef SCLT_HPP
#define SCLT_HPP

#include <getopt.h>
#include <vector>
#include <map>
#include <string>

#define SCLT_PARAM_BAG_L1_DELIMITER ';'
#define SCLT_PARAM_BAG_L2_DELIMITER ','

namespace SCLT
{
    typedef std::vector<double> DoubleVector;
    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string, std::string> StringMap;
    typedef std::vector<StringVector> ParamBag;

    StringVector SplitString(const std::string &s, char delim);

    StringVector dvtosv(DoubleVector in);
    DoubleVector svtodv(StringVector in);

    ParamBag DecodeParamBag(
        std::string in,
        char l1delim = SCLT_PARAM_BAG_L1_DELIMITER,
        char l2delim = SCLT_PARAM_BAG_L2_DELIMITER
    );

    std::string EncodeParamBag(
        ParamBag in,
        char l1delim = SCLT_PARAM_BAG_L1_DELIMITER,
        char l2delim = SCLT_PARAM_BAG_L2_DELIMITER
    );

    struct CliOption {
        char shortOption;
        std::string longOption;
        std::string description = "";
        bool requireArgument = false;
        bool requireOption = false;
    };

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
        void set(std::string option, std::string value);
        void unset(std::string option);
    };
}

#endif
