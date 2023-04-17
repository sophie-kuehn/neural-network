#include <iostream>
#include "snn.cpp"
#include "mnist.cpp"

int main(int argc, char **argv)
{
    // run MNIST test
    auto MNIST = new SNN::MNIST_Test;
    MNIST->execute("../mnist-data/");
    return 0;

    // run own test
    auto network = new SNN::Network;
    auto actFn = new SNN::Identity;
    //auto actFn = new SNN::Sigmoid;
    //auto actFn = new SNN::HyperbolicTangent;
    network->addLayer(3);
    network->addLayer(10, actFn);
    //network->addLayer(10, actFn);
    network->addLayer(1);
    network->createSynapses();

    for (int i=0; i<200000; i++) {
        SNN::DoubleVector input = {
            (rand() % 10) * 1.0,
            (rand() % 10) * 1.0,
            (rand() % 10) * 1.0
        };

        //SNN::DoubleVector expected = {input[0] * input[1] * input[2]};
        //SNN::DoubleVector expected = {input[0] - input[1] - input[2]};
        SNN::DoubleVector expected = {input[0] + input[1] + input[2]};
        SNN::DoubleVector output = network->process(input, expected, 0.0001);
        std::cout << std::to_string(expected[0]) << " - op: " << std::to_string(output[0]) << " - diff: " << std::to_string(fabs(output[0] - expected[0])) << std::endl;
    }

    return 0;
};
