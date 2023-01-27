#include <iostream>
#include "snn.cpp"

int main(int argc, char **argv)
{
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

    for (int i=0; i<100000; i++) {
        SNN::DoubleVector input = {
            (rand() % 10) * 1.0,
            (rand() % 10) * 1.0,
            (rand() % 10) * 1.0
        };

        //SNN::DoubleVector expected = {input[0] * input[1] * input[2]};
        //SNN::DoubleVector expected = {input[0] - input[1] - input[2]};
        SNN::DoubleVector expected = {input[0] + input[1] + input[2]};

        SNN::DoubleVector output = network->process(input, expected, 0.00001);

        std::cout << std::to_string(expected[0]) << " - op: " << std::to_string(output[0]) << " - diff: " << std::to_string(fabs(output[0] - expected[0])) << std::endl;
    }

    return 0;
};
