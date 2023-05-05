#ifndef SCLT_HPP
#define SCLT_HPP

#include <getopt.h>
#include <vector>
#include <map>
#include <string>

#define SCLT_PARAM_BAG_L1_DELIMITER ';'
#define SCLT_PARAM_BAG_L2_DELIMITER ','

#define SCLT_PBAG_3_DELIMITER {'_',';',','}
#define SCLT_PBAG_2_DELIMITER {';',','}
#define SCLT_PBAG_1_DELIMITER {','}

namespace SCLT
{
    typedef std::vector<char> CharVector;
    typedef std::vector<double> DoubleVector;
    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string, std::string> StringMap;

    StringVector SplitString(const std::string &s, char delim);

    StringVector dvtosv(DoubleVector in);
    DoubleVector svtodv(StringVector in);

    bool FileExists(const std::string path);
    void WriteToFile(std::string path, std::string contents);
    std::string ReadFromFile(std::string path);

    class PBag
    {
    public:
        std::string value;

        std::vector<PBag> children;
        void insert(std::string value);
        void insert(PBag children);
        void insert(StringVector values);

        StringVector toStringVector();
        DoubleVector toDoubleVector();
        std::string toString(CharVector delimiterList);
        static PBag fromString(std::string input, CharVector delimiterList);

        int size();
        PBag operator[](int key);
        std::vector<PBag>::iterator begin();
        std::vector<PBag>::iterator end();
        std::vector<PBag>::const_iterator cbegin() const;
        std::vector<PBag>::const_iterator cend() const;
        std::vector<PBag>::const_iterator begin() const;
        std::vector<PBag>::const_iterator end() const;
    };

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
