#ifndef SNN_APP_HPP
#define SNN_APP_HPP

#include <vector>

namespace SNN
{
    struct Check
    {
        SNN::DoubleVector input;
        SNN::DoubleVector expected;
    };

    typedef std::vector<Check> Checks;

    class CliApp
    {
    public:
        int main(int argc, char **argv);
    };
};

#endif
