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
        bool isBias = false;
        bool cacheValue = false;
        double value = 0;
        double smallDelta = 0;

        bool isInput()
        {
            return (this->inputSynapses.size() == 0);
        };

        bool isOutput()
        {
            return (this->outputSynapses.size() == 0);
        };

        void reset()
        {
            this->value = 0;
            this->smallDelta = 0;
            this->cacheValue = false;
        };

        double getValue()
        {
            if (this->isBias) return 1;
            if (this->isInput() || this->cacheValue) return this->value;

            double value = 0;

            for (const auto& synapse : this->inputSynapses) {
                value += synapse->getValue();
            }

            if (this->activationFunction != nullptr) {
                value = activationFunction->activate(value);
            }

            this->cacheValue = true;
            return this->value = value;
        };

        void learn(double expectedValue, double epsilon = 0.01)
        {
            if (this->isInput()) return;

            if (this->isOutput()) {
                this->smallDelta = expectedValue - this->getValue();

            } else {
                for (const auto& synapse : this->outputSynapses) {
                    this->smallDelta += synapse->outputNeuron->smallDelta * synapse->weight;
                }
            }

            for (const auto& synapse : this->inputSynapses) {
                double bigDelta = epsilon * this->smallDelta * synapse->inputNeuron->getValue();
                synapse->weight += bigDelta;
            }
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

        Neuron* addNeuron(ActivationFunction* activationFunction = nullptr)
        {
            auto neuron = new Neuron;
            neuron->activationFunction = activationFunction;
            this->neurons.back().push_back(neuron);
            return neuron;
        };

        void openNewLayer()
        {
            // add bias
            if (this->neurons.size() > 0) {
                auto neuron = new Neuron;
                neuron->isBias = true;
                this->neurons.back().push_back(neuron);
            }

            NeuronLayer layer;
            this->neurons.push_back(layer);
        };

        void addLayer(int numberOfNeurons = 1, ActivationFunction* activationFunction = nullptr)
        {
            this->openNewLayer();
            for (int i = 0; i < 10; i++) this->addNeuron(activationFunction);
        };

        void createSynapses()
        {
            srand(time(0));
            int leftLayer = 0;
            for (const auto& leftNeurons : this->neurons) {
                int rightLayer = leftLayer+1;
                if (this->neurons.size()-1 < rightLayer) return;
                for (const auto& leftNeuron : leftNeurons) {
                    for (const auto& rightNeuron : this->neurons[rightLayer]) {
                        if (rightNeuron->isBias) continue;
                        auto synapse = new Synapse;
                        leftNeuron->outputSynapses.push_back(synapse);
                        rightNeuron->inputSynapses.push_back(synapse);
                        synapse->inputNeuron = leftNeuron;
                        synapse->outputNeuron = rightNeuron;
                        synapse->weight = (rand() % 100) / 100.0;
                    }
                }
                leftLayer++;
            }
        };

        DoubleVector process(DoubleVector input, DoubleVector expectedOutput = {}, double epsilon = 0.01)
        {
            for (const auto& neurons : this->neurons) {
                for (const auto& neuron : neurons) {
                    neuron->reset();
                }
            }

            int index = 0;
            for (const auto& neuron : this->neurons[0]) {
                double value = 0;
                if (input.size() > index) value = input[index];
                neuron->value = value;
                index++;
            }

            DoubleVector output;
            for (const auto& neuron : this->neurons.back()) {
                output.push_back(neuron->getValue());
            }

            if (expectedOutput.size() == 0) return output;

            for(int i = this->neurons.size()-1; i >= 0; i--) {
                index = 0;
                for (const auto& neuron : this->neurons[i]) {
                    double expectedOutputValue = 0;
                    if (expectedOutput.size() > index) expectedOutputValue = expectedOutput[index];
                    neuron->learn(expectedOutputValue, epsilon);
                    index++;
                }
            }

            return output;
        };
    };
};


int main(int argc, char **argv)
{
    // nn --network="3;10,i;10,i;1" --input="10;10;10" --epsilon=0.00005 --iterations=50000


    // SETUP

    auto network = new SNN::Network;
    auto actFn = new SNN::IdentityFunction;
    network->addLayer(3);
    network->addLayer(10, actFn);
    network->addLayer(10, actFn);
    network->addLayer(1);
    network->createSynapses();

    // TESTRUN

    for (int i=0; i<50000; i++) {
        SNN::DoubleVector input = {
            (rand() % 10),
            (rand() % 10),
            (rand() % 10)
        };

        SNN::DoubleVector expected = {input[0] + input[1] + input[2]};
        SNN::DoubleVector output = network->process(input, expected, 0.00005);

        std::cout << std::to_string(expected[0]) << " - op: " << std::to_string(output[0]) << " - diff: " << std::to_string(fabs(output[0] - expected[0])) << std::endl;
    }

    return 0;
}
