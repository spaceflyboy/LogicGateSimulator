// gate.h
#ifndef GATE_H
#define GATE_H

// MAJOR TODO: Tweak pulse logic to allow for cyclic connections in boolean circuits.
// This will complicate things but I think it should be supported. 

// MAJOR TODO: Tweak all gate logic to allow parallelization of circuit pulse
// This means I want to be able to call pulse() on every input gate in parallel and the circuit should still
// finish pulsing.
// The reason this is currently a problem is because pulse relies on indirect input via checkPulse on its preceeding gates. 
// But this logic would probably have a race condition.

#include <vector> // Necessary here because many of the methods and class members use/are vectors

// Useful struct for simultaneously checking success and obtaining a value from some class methods
typedef struct  {
    bool success;
    std::vector<bool> outputs;
} operation_output;

class Gate; // Forward declaration to not upset compiler :)

// Struct for specifying which outputs of an attached input gate
// the gate being created is actually attached to
typedef struct {
    std::vector<int> attachmentIndices;
    Gate *attachedInputGate;
} indirect_input_info;

enum pulse_status {
    success = 0,
    inputGateFailure = 1,
    operationFailure = 2
};

typedef struct {
    pulse_status pulseStatus;
    int nextIndex;
} circuit_pulse_status;

// Type for gate operation function pointers. 
// Gate operations should return an integer (0 for success)
// and take in a vector of booleans as arguments
typedef std::vector<bool>(*FunctionPointer)(std::vector<bool>); 

// Class representing a generic logic gate (arbitrary number of inputs)
class Gate {


    protected:
        int totalInputs; // Total number of inputs this gate takes
        int directInputs; // Number of inputs supplied directly (i.e. not by a different gate's pulse)
        int totalOutputs; // Total number outputs this gate has
        std::vector<bool> inputFlags; // totalInputs-length vector of flags indicating direct inputs
        std::vector<indirect_input_info> attachedInputInfo; // linkages to gates which supply indirect inputs
        std::vector<Gate *> attachedOutputGates; // linkages to gates which this gate supplies indirect inputs to
        bool validPulse; // flag indicating whether the value in pulseOutput is valid
        std::vector<bool> pulseOutputs; // boolean output of the logic gate
        FunctionPointer operation; // Pointer to the operation performed by the logic gate 

        // Perform the actual logic gate operation using the stored function pointer
        // Inputs: 
        // // argInputs: Vector of boolean inputs to the gate to perform the stored operation on
        // Outputs:
        // // operation_output struct containing status code and output
        operation_output operate(std::vector<bool> argInputs);

        // Helper function for pulse that orders and collects both direct and indirect inputs
        // Inputs: 
        // // inputs: Vector of direct inputs
        // Outputs:
        // // Vector of collected outputs
        std::vector<bool> collectPulseInputs(std::vector<bool> inputs);

    public:
        // Constructor
        // Inputs:
        // // inputFlags: Vector of flags indicating which inputs (in order) are direct or indirect
        // // attachedInputGates: Vector of input gates which supply indirect inputs (backward links)
        // // forwardLinks: Vector of gates which take in this gate's output(s) as input
        // // operation: Function pointer representing the logic gate's actual operation. 
        Gate(int totalOutputs, std::vector<bool> inputFlags, FunctionPointer operation);
        
        // Pulse the logic gate and perform the operation
        // Inputs:
        // // inputs: Vector of direct inputs
        // Outputs:
        // // None
        pulse_status pulse(std::vector<bool> inputs);

        // Get information about the validity and content of the stored pulse value for this gate
        // Inputs: None
        // Outputs: operation_output struct containing status code and output
        // Note that this is a repurposing of the operation_output struct- "success" indicates validity.
        operation_output checkPulse();

        // Pulse wrapper for circuits to use. 
        // Will supply only necessary inputs and return the next index in oversized_inputs to be used
        circuit_pulse_status pulse(std::vector<bool> oversized_inputs, int startdex); 

        // Connect input gates to the calling gate and forward link the calling gate as an output gate for those input gates.
        // Inputs:
        // // inputGatesToLink: vector of Gate pointers to attach as input gates.
        // // attachmentIndicesList: list of output indices for each input gate to allow granularity of connections
        // Outputs:
        // // None
        void connect(std::vector<Gate *> inputGatesToLink, std::vector<std::vector<int>> attachmentIndicesList);

        // Clear the attachedInputInfo vector
        void clearInputConnections();

        // Clear the attachedOutputGates vector
        void clearOutputConnections();

        // Debug method to fetch most of the class member fields for printing
        // Inputs:
        // // None
        // Outputs:
        // // fields (all sent as integers)
        std::vector<int> debugGateFieldFetch() const {
            return std::vector<int> { this->totalInputs, this->directInputs, this->totalOutputs, (int) this->inputFlags.size(), (int) this->attachedInputInfo.size(), (int) this->attachedOutputGates.size(), (int) this->validPulse };
        }        
};

#endif