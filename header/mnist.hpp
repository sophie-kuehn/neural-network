#ifndef MNIST_HPP
#define MNIST_HPP

#include <vector>
#include <string>
#include "../header/snn.hpp"

namespace SNN
{
    class MNIST_Digit
    {
    public:
        int label;
        char data[28][28];
    };

    typedef std::vector<MNIST_Digit> MNIST_DataSet;
    typedef std::vector<char> MNIST_ByteVector;

    class MNIST_Decoder
    {
    public:
        MNIST_DataSet loadDataSet(std::string dataPath, std::string labelPath);
        MNIST_ByteVector readAllBytes(std::string filename);
    };

    class MNIST_ProbabilityDigit
    {
    public:
        int digit;
        float probability;
    };

    bool MNIST_ProbabilityDigitCompare(MNIST_ProbabilityDigit a, MNIST_ProbabilityDigit b);

    class MNIST_Test
    {
    public:
        MNIST_DataSet digitsTrain;
        MNIST_DataSet digitsTest;
        MNIST_Decoder* decoder = new MNIST_Decoder;
        Network* network = new Network;

        void test();
        void execute(std::string networkSaveFilePath, std::string mnistFilesRootPath);
    };
};

#endif
