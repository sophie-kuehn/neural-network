#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "math.h"

namespace SNN
{
    class Neuron;

    class ActivationFunction
    {
    public:
        virtual double activate(double input) = 0;
    };

    class IdentityFunction : public ActivationFunction
    {
    public:
        double activate(double input) override
        {
            return input;
        };
    };

    class Synapse
    {
    public:
        Neuron* inputNeuron;
        Neuron* outputNeuron;

        double weight = 0;

        double getValue();
    };

    class Neuron
    {
    public:
        std::vector<Synapse*> inputSynapses;
        std::vector<Synapse*> outputSynapses;
        ActivationFunction* activationFunction = nullptr;

        double value = 0;

        double getValue()
        {
            double value = this->value;
            for (const auto& synapse : this->inputSynapses) {
                value += synapse->getValue();
            }

            if (this->activationFunction != nullptr) {
                value = activationFunction->activate(value);
            }

            return value;
        };
    };

    double Synapse::getValue()
    {
        return this->inputNeuron->getValue() * this->weight;
    };

    typedef std::vector<Neuron*> NeuronLayer;
    typedef std::vector<double> DoubleVector;

    class Network
    {
    public:
        std::vector<NeuronLayer> neurons;

        Neuron* addNeuron(ActivationFunction* activationFunction)
        {
            return this->addNeuron(0.0, activationFunction);
        };

        Neuron* addNeuron(double value = 0.0, ActivationFunction* activationFunction = nullptr)
        {
            auto neuron = new Neuron;
            neuron->value = value;
            neuron->activationFunction = activationFunction;
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

            for (const auto& leftNeuron : this->neurons[leftLayer]) {
                for (const auto& rightNeuron : this->neurons[rightLayer]) {
                    auto synapse = new Synapse;
                    leftNeuron->outputSynapses.push_back(synapse);
                    rightNeuron->inputSynapses.push_back(synapse);
                    synapse->inputNeuron = leftNeuron;
                    synapse->outputNeuron = rightNeuron;
                }
            }

        };

        void randomizeWeights()
        {
            srand(time(0));

            for (const auto& layer : this->neurons) {
                for (const auto& neuron : layer) {
                    for (const auto& synapse : neuron->outputSynapses) {
                        synapse->weight = (rand() % 201) / 100.0 - 1.0;
                    }
                }
            }
        };

        DoubleVector getOutput()
        {
            DoubleVector result;
            for (const auto& neuron : this->neurons.back()) {
                result.push_back(neuron->getValue());
            }
            return result;
        };
    };
};


int main(int argc, char **argv)
{
    auto network = new SNN::Network;

    auto actFn = new SNN::IdentityFunction;

    network->openNewLayer();
    network->addNeuron(1);
    network->addNeuron(2);
    network->addNeuron(3);
    network->addNeuron(1); // bias

    network->openNewLayer();
    for (int i = 0; i < 10; i++) network->addNeuron(actFn);
    network->openNewLayer();
    for (int i = 0; i < 10; i++) network->addNeuron(actFn);
    network->openNewLayer();
    network->addNeuron();
    network->createSynapses();
    network->randomizeWeights();

    double expectedResult = 1.0 + 2.0 + 3.0;
    SNN::DoubleVector result = network->getOutput();
    double diff = fabs(result[0] - expectedResult);

    std::cout << std::to_string(result[0]) << " - diff: " << std::to_string(diff) << std::endl;

    return 0;
}
