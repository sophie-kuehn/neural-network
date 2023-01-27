#include "snn.h"
#include <cmath>

namespace SNN
{
    double Identity::activate(double input)
    {
        return input;
    };

    double Identity::derivative(double input)
    {
        return 1;
    };

    double Boolean::activate(double input)
    {
        if (input < 0.0) return 0;
        return 1.0;
    };

    double Boolean::derivative(double input)
    {
        return 1;
    };

    double Sigmoid::activate(double input)
    {
        return 1.0 / (1.0 + pow(std::exp(1.0), -input));
    };

    double Sigmoid::derivative(double input)
    {
        float sigm = this->activate(input);
        return sigm * (1 - sigm);
    };

    double HyperbolicTangent::activate(double input)
    {
        double epx = pow(std::exp(1.0), input);
        double enx = pow(std::exp(1.0), -input);
        return (epx - enx) / (epx + enx);
    };

    double HyperbolicTangent::derivative(double input)
    {
        double tanh = this->activate(input);
        return 1 - tanh * tanh;
    };

    double Synapse::getValue()
    {
        return this->inputNeuron->getValue() * this->weight;
    };

    bool Neuron::isInput()
    {
        return (this->inputSynapses.size() == 0);
    };

    bool Neuron::isOutput()
    {
        return (this->outputSynapses.size() == 0);
    };

    void Neuron::reset()
    {
        this->value = 0;
        this->smallDelta = 0;
        this->cacheValue = false;
    };

    double Neuron::getValue()
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

    void Neuron::learn(double expectedValue, double epsilon)
    {
        if (this->isInput()) return;

        if (this->isOutput()) {
            this->smallDelta = expectedValue - this->getValue();

        } else {
            this->smallDelta = 0;
            for (const auto& synapse : this->outputSynapses) {
                this->smallDelta += synapse->outputNeuron->smallDelta * synapse->previousWeight;
            }
        }

        double bigDeltaFactor = 1;
        if (this->activationFunction != nullptr) {
            bigDeltaFactor = activationFunction->derivative(this->getValue());
        }

        for (const auto& synapse : this->inputSynapses) {
            double bigDelta = bigDeltaFactor * epsilon * this->smallDelta * synapse->inputNeuron->getValue();
            synapse->previousWeight = synapse->weight;
            synapse->weight += bigDelta;
        }
    };

    Neuron* Network::addNeuron(ActivationFunction* activationFunction)
    {
        auto neuron = new Neuron;
        neuron->activationFunction = activationFunction;
        this->neurons.back().push_back(neuron);
        return neuron;
    };

    void Network::openNewLayer()
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

    void Network::addLayer(int numberOfNeurons, ActivationFunction* activationFunction)
    {
        this->openNewLayer();
        for (int i = 0; i < 10; i++) this->addNeuron(activationFunction);
    };

    void Network::createSynapses()
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

    DoubleVector Network::process(DoubleVector input, DoubleVector expectedOutput, double epsilon)
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
