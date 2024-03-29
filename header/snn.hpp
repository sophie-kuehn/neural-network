#ifndef SNN_HPP
#define SNN_HPP

#include <vector>
#include <map>
#include <string>
#include "sclt.hpp"

#define SNN_AF_ID_IDENTITY "Identity"
#define SNN_AF_ID_BOOLEAN "Boolean"
#define SNN_AF_ID_SIGMOID "Sigmoid"
#define SNN_AF_ID_HTANGENT "HTangent"

#define SNN_NEURON_ID_DELIMITER '-'

#define SNN_SAVE_COMMAND_ADD_NEURON "AN"
#define SNN_SAVE_COMMAND_ADD_SYNAPSE "AS"

#define SNN_DEFAULT_EPSILON 0.01

namespace SNN
{
    class Neuron;

    typedef std::vector<Neuron*> NeuronLayer;

    class ActivationFunction
    {
    public:
        virtual std::string getId() = 0;
        virtual double activate(double input) = 0;
        virtual double derivative(double input) = 0;
    };

    class Identity : public ActivationFunction
    {
    public:
        std::string getId() override;
        double activate(double input) override;
        double derivative(double input) override;
    };

    class Boolean : public ActivationFunction
    {
    public:
        std::string getId() override;
        double activate(double input) override;
        double derivative(double input) override;
    };

    class Sigmoid : public ActivationFunction
    {
    public:
        std::string getId() override;
        double activate(double input) override;
        double derivative(double input) override;
    };

    class HyperbolicTangent : public ActivationFunction
    {
    public:
        std::string getId() override;
        double activate(double input) override;
        double derivative(double input) override;
    };

    class ActivationFunctionRegistry
    {
    public:
        std::map<std::string, ActivationFunction*> registry;
        void add(ActivationFunction* activationFunction);
        ActivationFunction* get(std::string id);
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
        std::string id;
        std::vector<Synapse*> inputSynapses;
        std::vector<Synapse*> outputSynapses;
        ActivationFunction* activationFunction = nullptr;
        bool cacheValue = false;
        double value = 0;
        double smallDelta = 0;
        void reset();
        bool isInput();
        bool isOutput();
        double getValue();
        void learn(double expectedValue, double epsilon = SNN_DEFAULT_EPSILON);
    };

    class Network
    {
    public:
        Network(ActivationFunctionRegistry* afRegistry = nullptr);
        ActivationFunctionRegistry* afRegistry;
        std::vector<NeuronLayer> neurons;
        Neuron* addNeuron(int layer, std::string activationFunctionId = SNN_AF_ID_IDENTITY);
        Neuron* getNeuron(std::string id);
        Synapse* addSynapse(Neuron* leftNeuron, Neuron* rightNeuron, double weight = 0.0);
        void initLayerUpTo(int layer);
        void addLayer(int numberOfNeurons = 1, std::string activationFunctionId = SNN_AF_ID_IDENTITY);
        void store(std::string filePath);
        void load(std::string filePath);
        void loadShort(std::string definition);
        void createSynapses();
        SCLT::DoubleVector process(
            SCLT::DoubleVector input,
            SCLT::DoubleVector expectedOutput = {},
            double epsilon = SNN_DEFAULT_EPSILON
        );
    };
};

#endif
