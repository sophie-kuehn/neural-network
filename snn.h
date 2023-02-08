#ifndef SNN_H
#define SNN_H

#include <vector>
#include <string>

namespace SNN
{
    class Neuron;

    typedef std::vector<Neuron*> NeuronLayer;
    typedef std::vector<double> DoubleVector;

    void COutString(std::string str);
    void COutDoubleVector(DoubleVector vector);

    class ActivationFunction
    {
    public:
        virtual double activate(double input) = 0;
        virtual double derivative(double input) = 0;
    };

    class Identity : public ActivationFunction
    {
    public:
        double activate(double input) override;
        double derivative(double input) override;
    };

    class Boolean : public ActivationFunction
    {
    public:
        double activate(double input) override;
        double derivative(double input) override;
    };

    class Sigmoid : public ActivationFunction
    {
    public:
        double activate(double input) override;
        double derivative(double input) override;
    };

    class HyperbolicTangent : public ActivationFunction
    {
    public:
        double activate(double input) override;
        double derivative(double input) override;
    };

    class Synapse
    {
    public:
        Neuron* inputNeuron;
        Neuron* outputNeuron;
        double previousWeight = 0;
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
        void reset();
        bool isInput();
        bool isOutput();
        double getValue();
        void learn(double expectedValue, double epsilon = 0.01);
    };

    class Network
    {
    public:
        std::vector<NeuronLayer> neurons;
        Neuron* addNeuron(ActivationFunction* activationFunction = nullptr);
        void openNewLayer();
        void addLayer(int numberOfNeurons = 1, ActivationFunction* activationFunction = nullptr);
        void createSynapses();
        DoubleVector process(DoubleVector input, DoubleVector expectedOutput = {}, double epsilon = 0.01);
    };
};

#endif
