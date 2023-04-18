#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../header/snn.hpp"

namespace SNN
{
    bool FileExists(const std::string path)
    {
        std::ifstream file(path);
        if (file.is_open()) {
            file.close();
            return true;
        }
        return false;
    };

    StringVector SplitString(const std::string &s, char delim)
    {
        StringVector result;
        std::stringstream ss(s);
        std::string item;

        while (getline(ss, item, delim)) {
            result.push_back(item);
        }

        return result;
    };

    void COutString(std::string str)
    {
        std::cout << str << std::endl;
    };

    void COutDoubleVector(DoubleVector vector)
    {
        std::cout << "{ ";
        for (int i = 0; i < vector.size(); i++) {
            std::cout << std::to_string(vector[i]);
            if (i < vector.size()-1) std::cout << ", ";
        }
        std::cout << " }" << std::endl;
    };

    std::string Identity::getId()
    {
        return SNN_AF_ID_IDENTITY;
    };

    double Identity::activate(double input)
    {
        return input;
    };

    double Identity::derivative(double input)
    {
        return 1;
    };

    std::string Boolean::getId()
    {
        return SNN_AF_ID_BOOLEAN;
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

    std::string Sigmoid::getId()
    {
        return SNN_AF_ID_SIGMOID;
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

    std::string HyperbolicTangent::getId()
    {
        return SNN_AF_ID_HTANGENT;
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

    void ActivationFunctionRegistry::add(ActivationFunction* activationFunction)
    {
        this->registry[activationFunction->getId()] = activationFunction;
    };

    ActivationFunction* ActivationFunctionRegistry::get(std::string id)
    {
        if (this->registry[id] == nullptr) {
            throw std::invalid_argument("no activation function with id \"" + id + "\"");
        }

        return this->registry[id];
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

    Network::Network(ActivationFunctionRegistry* afRegistry)
    {
        if (afRegistry == nullptr) {
            afRegistry = new ActivationFunctionRegistry;
            afRegistry->add(new SNN::Sigmoid);
            afRegistry->add(new SNN::HyperbolicTangent);
        }

        afRegistry->add(new SNN::Identity);
        this->afRegistry = afRegistry;
    };

    Neuron* Network::addNeuron(int layerId, std::string activationFunctionId)
    {
        this->initLayerUpTo(layerId);
        auto neuron = new Neuron;
        neuron->activationFunction = this->afRegistry->get(activationFunctionId);
        neuron->id = (std::string)"N"
            + SNN_NEURON_ID_DELIMITER + std::to_string(layerId)
            + SNN_NEURON_ID_DELIMITER + std::to_string(this->neurons[layerId].size());
        this->neurons[layerId].push_back(neuron);
        return neuron;
    };

    Neuron* Network::getNeuron(std::string id)
    {
        StringVector query = SplitString(id, '.');

        for (const auto& neuronLayer : this->neurons) {
            for (const auto& neuron : neuronLayer) {
                if (neuron->id == id) return neuron;
            }
        }

        throw std::invalid_argument("could not find neuron \"" + id + "\"");
    };

    Synapse* Network::addSynapse(Neuron* leftNeuron, Neuron* rightNeuron, double weight)
    {
        auto synapse = new Synapse;
        leftNeuron->outputSynapses.push_back(synapse);
        rightNeuron->inputSynapses.push_back(synapse);
        synapse->inputNeuron = leftNeuron;
        synapse->outputNeuron = rightNeuron;
        synapse->weight = weight;
        return synapse;
    }

    void Network::initLayerUpTo(int layerId)
    {
        while (this->neurons.size() < layerId+1) {
            NeuronLayer layer;
            this->neurons.push_back(layer);
        }
    };

    void Network::addLayer(int numberOfNeurons, std::string activationFunctionId)
    {
        int layerId = this->neurons.size();
        for (int i = 0; i < numberOfNeurons; i++) {
            this->addNeuron(layerId, activationFunctionId);
        }
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
                    this->addSynapse(leftNeuron, rightNeuron, (rand() % 100) / 100.0);
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

    void Network::store(std::string filePath)
    {
        std::string neuronSetup, synapseSetup;

        for (const auto& neuronLayer : this->neurons) {
            for (const auto& neuron : neuronLayer) {
                std::string neuronId = neuron->id;
                std::replace(
                    neuronId.begin(),
                    neuronId.end(),
                    SNN_NEURON_ID_DELIMITER,
                    SNN_SAVE_ARGUMENT_DELIMITER
                );

                neuronSetup = neuronSetup + SNN_SAVE_COMMAND_ADD_NEURON
                    + SNN_SAVE_ARGUMENT_DELIMITER + neuronId
                    + SNN_SAVE_ARGUMENT_DELIMITER + neuron->activationFunction->getId()
                    + SNN_SAVE_COMMAND_DELIMITER + "\n";

                for (const auto& synapse : neuron->outputSynapses) {
                    synapseSetup = synapseSetup + SNN_SAVE_COMMAND_ADD_SYNAPSE
                        + SNN_SAVE_ARGUMENT_DELIMITER + neuron->id
                        + SNN_SAVE_ARGUMENT_DELIMITER + synapse->outputNeuron->id
                        + SNN_SAVE_ARGUMENT_DELIMITER + std::to_string(synapse->weight)
                        + SNN_SAVE_COMMAND_DELIMITER + "\n";
                }
            }
        }

        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::invalid_argument("could not open file \"" + filePath + "\"");
        }
        file << neuronSetup + synapseSetup;
        file.close();
    };

    void Network::load(std::string filePath)
    {
        this->neurons.clear();

        std::string input, line;
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::invalid_argument("could not open file \"" + filePath + "\"");
        }
        while (getline(file, line)) input = input + line;
        file.close();

        StringVector commands = SplitString(input, SNN_SAVE_COMMAND_DELIMITER);
        for (const auto& command : commands) {
            StringVector arguments = SplitString(command, SNN_SAVE_ARGUMENT_DELIMITER);
            if (arguments[0] == SNN_SAVE_COMMAND_ADD_NEURON) {
                this->addNeuron(
                    std::atoi(arguments[2].c_str()),
                    arguments[4]
                );
            } else if (arguments[0] == SNN_SAVE_COMMAND_ADD_SYNAPSE) {
                Neuron* leftNeuron = this->getNeuron(arguments[1]);
                Neuron* rightNeuron = this->getNeuron(arguments[2]);
                this->addSynapse(
                    leftNeuron,
                    rightNeuron,
                    std::stod(arguments[3])
                );
            }
        }
    };
};
