// gate.h
#ifndef GATE_H
#define GATE_H

#include <vector> // Necessary here because many of the methods and class members use/are vectors

// Useful struct for simultaneously checking success and obtaining a value from some class methods
struct operation_output {
    bool success;
    std::vector<bool> outputs;
};

// Type for gate operation function pointers. 
// Gate operations should return an integer (0 for success)
// and take in a vector of booleans as arguments
typedef std::vector<bool>(*FunctionPointer)(std::vector<bool>); 

// Class representing a generic logic gate (arbitrary number of inputs)
class Gate {
    protected:
        int totalInputs; // Total number of inputs this gate takes
        int directInputs; // Number of inputs supplied directly (i.e. not by a different gate's pulse)
        std::vector<bool> inputFlags; // totalInputs-length vector of flags indicating direct inputs
        std::vector<Gate> attachedInputGates; // linkages to gates which supply indirect inputs
        std::vector<Gate> forwardLinks; // linkages to gates which this gate supplies indirect inputs to
        bool validPulse; // flag indicating whether the value in pulseOutput is valid
        std::vector<bool> pulseOutputs; // boolean output of the logic gate
        FunctionPointer operation; // Pointer to the operation performed by the logic gate 

        // Perform the actual logic gate operation using the stored function pointer
        // Inputs: 
        // // argInputs: Vector of boolean inputs to the gate to perform the stored operation on
        // Outputs:
        // // operation_output struct containing status code and output
        operation_output operate(std::vector<bool> argInputs);

        // Get information about the validity and content of the stored pulse value for this gate
        // Inputs: None
        // Outputs: operation_output struct containing status code and output
        // Note that this is a repurposing of the operation_output struct- "success" indicates validity.
        operation_output checkPulse();

        // Helper function for pulse that orders and collects both direct and indirect inputs
        // Inputs: 
        // // inputs: Vector of direct inputs
        // Outputs:
        // // Vector of collected outputs
        std::vector<bool> collectPulseInputs(std::vector<bool> inputs);

        // Pulse the logic gate and perform the operation
        // Inputs:
        // // inputs: Vector of direct inputs
        // Outputs:
        // // None
        void pulse(std::vector<bool> inputs);

    public:
        // Constructor
        // Inputs:
        // // inputFlags: Vector of flags indicating which inputs (in order) are direct or indirect
        // // attachedInputGates: Vector of input gates which supply indirect inputs (backward links)
        // // forwardLinks: Vector of gates which take in this gate's output(s) as input
        // // operation: Function pointer representing the logic gate's actual operation. 
        Gate(std::vector<bool> inputFlags, std::vector<Gate> attachedInputGates, std::vector<Gate> forwardLinks, FunctionPointer operation);

        void backwardLink(std::vector<Gate> gatesToLink, bool replace_flag);

        void forwardLink(std::vector<Gate> gatesToLink, bool replace_flag);

        // Pulse wrapper for circuits to use. 
        // Will supply only necessary inputs and return the next index in oversized_inputs to be used
        int pulse(std::vector<bool> oversized_inputs, int startdex); 
};

#endif