// gate.h
#ifndef GATE_H
#define GATE_H

#include <vector> // Necessary here because many of the methods and class members use/are vectors

// Type for gate operation function pointers. 
// Gate operations should return an integer (0 for success)
// and take in a vector of booleans as arguments
typedef int(*FunctionPointer)(std::vector<bool>); 

// Class representing a generic logic gate (arbitrary number of inputs)
class Gate {
    // Useful struct for simultaneously checking success and obtaining a value from some class methods
    struct operation_output {
        bool success;
        bool output;
    };

    protected:
        int totalInputs; // Total number of inputs this gate takes
        int directInputs; // Number of inputs supplied directly (i.e. not by a different gate's pulse)
        std::vector<bool> inputFlags; // totalInputs-length vector of flags indicating direct inputs

        std::vector<Gate> attachedInputGates; // linkages to gates which supply indirect inputs
        std::vector<Gate> forwardLinks; // linkages to gates which this gate supplies indirect inputs to
        bool validPulse; // flag indicating whether the value in pulseOutput is valid
        bool pulseOutput; // boolean output of the logic gate
        FunctionPointer operation; // Pointer to the operation performed by the logic gate 

        // Perform the actual logic gate operation using the stored function pointer
        // Inputs: 
        // // argInputs: List of boolean inputs to the gate to perform the stored operation on
        // Outputs:
        // // operation_output struct containing status code and output
        operation_output operate(std::vector<bool> argInputs);

        // Get information about the validity and content of the stored pulse value for this gate
        // Inputs: None
        // Outputs: operation_output struct containing status code and output
        // Note that this is a repurposing of the operation_output struct- "success" indicates validity.
        operation_output checkPulse();

    public:
        Gate(std::vector<bool> inputFlags, std::vector<Gate> attachedInputGates, std::vector<Gate> forwardLinks, FunctionPointer operation);

        void forward_link(std::vector<Gate> gatesToLink, bool replace_flag);

        void pulse(std::vector<bool> inputs);
};

#endif