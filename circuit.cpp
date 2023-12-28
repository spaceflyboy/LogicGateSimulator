#include "circuit.h"
#include <stdexcept>
#include <iostream>

Circuit::Circuit(std::vector<Gate> inputGates, std::vector<Gate> outputGates) {
    this->inputGates = inputGates;
    this->outputGates = outputGates;
    this->totalInputs = this->inputGates.size();
    this->
}

std::vector<bool> collectPulseInputs() {
    
}

std::vector<bool> Circuit::pulse(std::vector<bool> inputs) {
    int input_index;
    for (Gate inputGate : this->inputGates) {
        inputGate.pulse(inputs, input_index);
        input_index++;
    }

        std::vector<bool> outputs();
}