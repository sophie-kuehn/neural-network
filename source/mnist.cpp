#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include "../header/mnist.hpp"

namespace SNN
{
    MNIST_DataSet MNIST_Decoder::loadDataSet(std::string dataPath, std::string labelPath)
    {
        if (!SNN::FileExists(dataPath)) {
            throw std::invalid_argument("file \"" + dataPath + "\" not found");
        }

        MNIST_ByteVector dataBytes = this->readAllBytes(dataPath);
        MNIST_ByteVector labelBytes = this->readAllBytes(labelPath);
        MNIST_DataSet dataSet;

        int readHeadData = 16;
        int readHeadLabel = 8;

        while(readHeadData < dataBytes.size()) {
            MNIST_Digit digit;

            for(int i = 0; i<28; i++){
                for(int k = 0; k<28; k++){
                    digit.data[i][k] = dataBytes[readHeadData++];
                }
            }

            digit.label = labelBytes[readHeadLabel++] & 0xFF;
            dataSet.push_back(digit);
        }

        return dataSet;

    };

    MNIST_ByteVector MNIST_Decoder::readAllBytes(std::string filename)
    {
        std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
        if (!ifs.is_open()) return {};
        std::ifstream::pos_type pos = ifs.tellg();
        MNIST_ByteVector result(pos);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], pos);
        return result;
    };

    bool MNIST_ProbabilityDigitCompare(MNIST_ProbabilityDigit a, MNIST_ProbabilityDigit b)
    {
        return a.probability > b.probability;
    };

    void MNIST_Test::test()
    {
        std::cout << "test" << std::endl;

        float correct = 0;
        float incorrect = 0;

        for (int i = 0; i < this->digitsTest.size(); i++) {
            SNN::DoubleVector input;
            input.push_back(1); // bias

            for (int x = 0; x < 28; x++) {
                for (int y = 0; y < 28; y++) {
                    input.push_back( (digitsTest[i].data[x][y] & 0xFF) / 255.0 );
                }
            }

            DoubleVector output = network->process(input);

            MNIST_ProbabilityDigit probs[10];
            for (int k = 0; k < 10; k++) {
                probs[k].digit = k;
                probs[k].probability = output[k];
            }

            std::sort(probs, probs + 10, MNIST_ProbabilityDigitCompare);

            if (this->digitsTest[i].label == probs[0].digit) {
                correct++;
            } else {
                incorrect++;
            }
        }

        float percentage = correct / (correct + incorrect);
        std::cout << "result: " << std::to_string(percentage) << std::endl;
    };

    void MNIST_Test::execute(std::string networkSaveFilePath, std::string mnistFilesRootPath)
    {
        this->digitsTrain = this->decoder->loadDataSet(
            mnistFilesRootPath + "train-images.idx3-ubyte",
            mnistFilesRootPath + "train-labels.idx1-ubyte"
        );

        this->digitsTest = this->decoder->loadDataSet(
            mnistFilesRootPath + "t10k-images.idx3-ubyte",
            mnistFilesRootPath + "t10k-labels.idx1-ubyte"
        );

        if (FileExists(networkSaveFilePath)) {
            this->network->load(networkSaveFilePath);
        } else {
            this->network->addLayer(1+(28*28));
            this->network->addLayer(10, SNN_AF_ID_SIGMOID);
            this->network->createSynapses();
        }

        double epsilon = 0.01;

        while(true) {
            std::cout << "train" << std::endl;

            for (int i = 0; i < digitsTrain.size(); i++) {
                SNN::DoubleVector input;
                input.push_back(1); // bias

                for (int x = 0; x < 28; x++) {
                    for (int y = 0; y < 28; y++) {
                        input.push_back( (digitsTrain[i].data[x][y] & 0xFF) / 255.0 );
                    }
                }

                SNN::DoubleVector expected = {0,0,0,0,0,0,0,0,0,0};
                expected[digitsTrain[i].label] = 1;

                network->process(input, expected, epsilon);
            }

            this->test();
            this->network->store(networkSaveFilePath);
            epsilon *= 0.9;
        }
    };
};
