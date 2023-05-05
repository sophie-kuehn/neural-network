#include <iostream>
#include <fstream>
#include <sstream>
#include "../header/sclt.hpp"

namespace SCLT
{
    bool FileExists(const std::string path)
    {
        std::ifstream file(path);
        if (file.is_open()) {
            file.close();
            return true;
        }
        return false;
    };

    void WriteToFile(std::string path, std::string contents)
    {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::invalid_argument("could not open file \"" + path + "\"");
        }
        file << contents;
        file.close();
    };

    std::string ReadFromFile(std::string path)
    {
        std::string input, line;
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::invalid_argument("could not open file \"" + path + "\"");
        }
        while (getline(file, line)) input = input + line;
        file.close();
        return input;
    };

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

    void PBag::insert(std::string value)
    {
        this->children.push_back(PBag {value});
    };

    void PBag::insert(PBag children)
    {
        this->children.push_back(children);
    };

    void PBag::insert(StringVector values)
    {
        PBag n;
        for (const auto& v : values) n.insert(v);
        this->children.push_back(n);
    };

    StringVector PBag::toStringVector()
    {
        StringVector s;
        for (const auto& c : this->children) s.push_back(c.value);
        return s;
    };

    DoubleVector PBag::toDoubleVector()
    {
        DoubleVector d;
        for (const auto& c : this->children) {
            double sd = 0;
            try { sd = std::stod(c.value); } catch (...) {}
            d.push_back(sd);
        }
        return d;
    };

    std::string PBag::toString(CharVector delimiterList)
    {
        char delimiter = delimiterList[0];
        delimiterList.erase(delimiterList.begin());
        if (delimiterList.size() == 0) {
            delimiterList.push_back('\0');
        }

        std::string str = this->value;

        bool first = true;
        for (auto& children : this->children) {
            if (!first) str += delimiter;
            first = false;
            str += children.toString(delimiterList);
        }
        return str;
    };

    PBag PBag::fromString(std::string input, CharVector delimiterList)
    {
        PBag result;

        if (delimiterList.size() == 0) {
            result.value = input;
            return result;
        }

        char delimiter = delimiterList[0];
        delimiterList.erase(delimiterList.begin());

        StringVector v = SplitString(input, delimiter);
        for (const auto& s : v) {
            result.insert(fromString(s, delimiterList));
        }

        return result;
    };

    int PBag::size()
    {
        return this->children.size();
    };

    PBag PBag::operator[](int key)
    {
        return this->children[key];
    }

    std::vector<PBag>::iterator PBag::begin() { return this->children.begin(); }
    std::vector<PBag>::iterator PBag::end() { return this->children.end(); }
    std::vector<PBag>::const_iterator PBag::cbegin() const { return this->children.begin(); }
    std::vector<PBag>::const_iterator PBag::cend() const { return this->children.end(); }
    std::vector<PBag>::const_iterator PBag::begin() const { return this->children.begin(); }
    std::vector<PBag>::const_iterator PBag::end() const { return this->children.end(); }

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
