// circuit.h
#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector> // Necessary because many functions / members use/are vectors. 
#include "gate.cpp"

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


/* TODO !!!!

when fetching the outputs of an attached input circuit, provide
the number of forward links. This will be useful to determine if
certain inputs need to be "duplicated" in the pulse method.

As part of this ^^^ Need to rethink Gate struct maybe. (And pulse wrapper for gate)

*/