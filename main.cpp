#include <iostream>
#include <vector>
#include <map>
#include <string>

namespace SNN
{
    class Neuron;

    class Synapse
    {
    public:
        double weight = 0;
        Neuron* inputNeuron;
        Neuron* outputNeuron;

        void fire();
    };

    class Neuron
    {
    public:
        double value = 0;
        std::vector<Synapse*> inputSynapses;
        std::vector<Synapse*> outputSynapses;
    };

    void Synapse::fire()
    {
        //std::cout << "." << std::endl;
        this->outputNeuron->value = this->outputNeuron->value + (this->inputNeuron->value * this->weight);
    };

    typedef std::vector<Neuron*> NeuronLayer;

    class Network
    {
    public:
        std::vector<NeuronLayer> neurons;

        Neuron* addNeuron()
        {
            auto neuron = new Neuron;
            this->neurons.back().push_back(neuron);
            return neuron;
        };

        void openNewLayer()
        {
            NeuronLayer layer;
            this->neurons.push_back(layer);
        };

        void createSynapses()
        {
            int leftLayer = 0;
            for (const auto& leftNeurons : this->neurons) {
                this->createSynapsesBetween(leftLayer, leftLayer+1);
                leftLayer++;
            }
        };

        void createSynapsesBetween(int leftLayer, int rightLayer)
        {
            if (this->neurons.size()-1 < rightLayer) return;

            //int leftIndex = 0;
            for (const auto& leftNeuron : this->neurons[leftLayer]) {

                //int rightIndex = 0;
                for (const auto& rightNeuron : this->neurons[rightLayer]) {
                    auto synapse = new Synapse;
                    leftNeuron->outputSynapses.push_back(synapse);
                    rightNeuron->inputSynapses.push_back(synapse);
                    synapse->inputNeuron = leftNeuron;
                    synapse->outputNeuron = rightNeuron;


                    //std::cout << "creating neurons for left " << std::to_string(leftIndex) << " right " << std::to_string(rightIndex) << std::endl;
                    //rightIndex++;
                }

                //leftIndex++;
            }

        };

        void randomizeWeights()
        {
            srand(time(0));

            for (const auto& layer : this->neurons) {
                for (const auto& neuron : layer) {
                    for (const auto& synapse : neuron->outputSynapses) {
                        synapse->weight = (rand() % 201) / 100.0 - 1.0;
                        //std::cout << std::to_string(synapse->weight) << std::endl;
                    }
                }
            }
        };

        void fire()
        {
            for (const auto& layer : this->neurons) {
                for (const auto& neuron : layer) {
                    for (const auto& synapse : neuron->outputSynapses) {
                        synapse->fire();
                    }
                }
            }
        };
    };
};

int main(int argc, char **argv)
{
    auto network = new SNN::Network;

    network->openNewLayer();
    for (int i = 0; i < 3; i++) network->addNeuron();

    network->neurons[0][0]->value = 1.0;
    network->neurons[0][1]->value = 2.0;
    network->neurons[0][2]->value = 3.0;

    network->openNewLayer();
    for (int i = 0; i < 10; i++) network->addNeuron();
    network->openNewLayer();
    for (int i = 0; i < 10; i++) network->addNeuron();
    network->openNewLayer();
    network->addNeuron();
    network->createSynapses();
    network->randomizeWeights();
    network->fire();

    for (const auto& outputNeuron : network->neurons.back()) {
        std::cout << std::to_string(outputNeuron->value) << std::endl;
    }


    std::cout << "Hello, world!" << std::endl;
    return 0;
}
