// circuit.h
#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector> // Necessary because many functions / members use/are vectors. 
#include <gate.h>

class Circuit {
    protected:
        int directInputs;
        std::vector<bool> inputFlags;
        std::vector<Gate> inputGates;
        std::vector<Gate> outputGates;
    public:
        Circuit(std::vector<Gate> inputGates, std::vector<Gate> outputGates);

        std::vector<bool> pulse(std::vector<bool> inputs);
};
#endif