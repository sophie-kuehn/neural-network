#include <iostream>
#include "snn.cpp"

#include <vector>
#include <fstream>
#include<algorithm>

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
        MNIST_DataSet loadDataSet(std::string dataPath, std::string labelPath)
        {
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

        MNIST_ByteVector readAllBytes(std::string filename)
        {
            std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
            if (!ifs.is_open()) return {};
            std::ifstream::pos_type pos = ifs.tellg();
            MNIST_ByteVector result(pos);
            ifs.seekg(0, std::ios::beg);
            ifs.read(&result[0], pos);
            return result;
        }
    };

    class MNIST_ProbabilityDigit
    {
    public:
        int digit;
        float probability;
    };

    bool MNIST_ProbabilityDigitCompare(MNIST_ProbabilityDigit a, MNIST_ProbabilityDigit b)
    {
        return a.probability > b.probability;
    };

    class MNIST_Test
    {
    public:
        MNIST_DataSet digitsTrain;
        MNIST_DataSet digitsTest;
        MNIST_Decoder* decoder = new MNIST_Decoder;
        Network* network = new Network;

        void test()
        {
            float correct = 0;
            float incorrect = 0;

            for (int i = 0; i < this->digitsTest.size(); i++) {
                SNN::DoubleVector input;

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
            std::cout << std::to_string(percentage) << std::endl;
        };

        void execute(std::string mnistFilesRootPath)
        {
            this->digitsTrain = this->decoder->loadDataSet(mnistFilesRootPath + "train-images.idx3-ubyte", mnistFilesRootPath + "train-labels.idx1-ubyte");
            this->digitsTest = this->decoder->loadDataSet(mnistFilesRootPath + "t10k-images.idx3-ubyte", mnistFilesRootPath + "t10k-labels.idx1-ubyte");

            this->network->addLayer(28*28);
            this->network->addLayer(10);
            this->network->createSynapses();
            double epsilon = 0.01;

            while(true) {
                for (int i = 0; i < digitsTrain.size(); i++) {
                    SNN::DoubleVector input;

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
                //return;
                epsilon *= 0.9;
            }
        };
    };
};

int main(int argc, char **argv)
{
    /*
    auto tnetwork = new SNN::Network;
    tnetwork->addLayer(10);
    tnetwork->addLayer(10);
    tnetwork->createSynapses();

    SNN::DoubleVector input = {1,1,1,1,1,1,1,1,1,1};
    SNN::DoubleVector output = tnetwork->process(input);

    SNN::COutDoubleVector(input);
    SNN::COutDoubleVector(output);

    SNN::DoubleVector input2 = {0,0,0,0,0,0,0,0,0,0};
    SNN::DoubleVector output2 = tnetwork->process(input2);

    SNN::COutDoubleVector(input2);
    SNN::COutDoubleVector(output2);
*/



    auto MNIST = new SNN::MNIST_Test;
    MNIST->execute("../mnist/");
    return 0;

    // nn --network="3;10,i;10,i;1" --input="10;10;10" --epsilon=0.00005 --iterations=50000


    // SETUP

    auto network = new SNN::Network;
    auto actFn = new SNN::Identity;
    //auto actFn = new SNN::Sigmoid;
    //auto actFn = new SNN::HyperbolicTangent;
    network->addLayer(3);
    network->addLayer(10, actFn);
    network->addLayer(10, actFn);
    network->addLayer(1);
    network->createSynapses();

    // TESTRUN

    for (int i=0; i<200000; i++) {
        SNN::DoubleVector input = {
            (rand() % 10) * 1.0,
            (rand() % 10) * 1.0,
            (rand() % 10) * 1.0
        };

        SNN::DoubleVector expected = {input[0] * input[1] * input[2]};
        //SNN::DoubleVector expected = {input[0] - input[1] - input[2]};
        //SNN::DoubleVector expected = {input[0] + input[1] + input[2]};

        SNN::DoubleVector output = network->process(input, expected, 0.00001);

        std::cout << std::to_string(expected[0]) << " - op: " << std::to_string(output[0]) << " - diff: " << std::to_string(fabs(output[0] - expected[0])) << std::endl;
    }

    return 0;
};
