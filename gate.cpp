#include <gate.h>
#include <stdexcept>
#include <iostream>

Gate::operation_output Gate::operate(std::vector<bool> argInputs) {
    operation_output result;
    if (int x = this->operation(argInputs)) {
        result.success = false;
        result.output = false;
    } else {
        result.success = true;
        result.output = x;
    }
    return result;
}   

// Get information about the validity and content of the stored pulse value for this gate
// Inputs: None
// Outputs: operation_output struct containing status code and output
// Note that this is a repurposing of the operation_output struct- "success" indicates validity.
Gate::operation_output Gate::checkPulse() {
    operation_output result;
    if (this->validPulse) {
        result.success = true;
        result.output = this->pulseOutput;
    } else {
        result.success = false;
        result.output = false;
    }
    return result;
}


Gate::Gate(std::vector<bool> inputFlags, std::vector<Gate> attachedInputGates, std::vector<Gate> forwardLinks, FunctionPointer operation) { 
    if (totalInputs <= 0 || directInputs > totalInputs || directInputs < 0 || inputFlags.size() != totalInputs || ((directInputs != totalInputs) && attachedInputGates.size() == 0)) {
        throw std::invalid_argument("Invalid argument(s) for constructing a Gate object. "
                                    "Total inputs must be a positive integer, direct inputs "
                                    "must be an integer between 0 and total inputs, and input " 
                                    "flags are required. Also check that if you have indirect "
                                    "inputs, you are providing a pointer to the required indirect "
                                    "input providing gates.");
    }
    this->totalInputs = inputFlags.size();
    int cnt = 0;
    for (int i = 0; i < this->totalInputs; i++) {
        if(inputFlags[i]) {
            cnt++;
        }
    }
    this->directInputs = cnt;
    this->attachedInputGates = attachedInputGates;
    this->inputFlags = inputFlags;
    this->validPulse = false;
    this->pulseOutput = false;
    this->operation = operation;
    this->forwardLinks = forwardLinks;
}

void Gate::forward_link(std::vector<Gate> gatesToLink, bool replace_flag) {
    if (replace_flag || gatesToLink.size() == 0) {
        this->forwardLinks = gatesToLink;
    } else {
        for (Gate gate : gatesToLink) {
            this->forwardLinks.push_back(gate);
        }
        
    }
}

void Gate::pulse(std::vector<bool> inputs) {
    int inputsIndex = 0;
    int attachedInputGatesIndex = 0;
    std::vector<bool> args;
    for (bool flag : this->inputFlags) {
        if (flag) {
            args.push_back(inputs[inputsIndex]);
            inputsIndex++;
        } else {
            Gate inputGate = this->attachedInputGates[attachedInputGatesIndex];
            
            operation_output pulseOutput = inputGate.checkPulse(); // Re-using struct for convenience
            if (pulseOutput.success) {

            } else {
                args.push_back(pulseOutput.output);
                attachedInputGatesIndex++;
            }
        }
    }

    if (args.size() == this->totalInputs) {
        operation_output operationResult = this->operate(args);
        if (operationResult.success) {
            this->pulseOutput = operationResult.output;
            for (Gate forwardLink : this->forwardLinks) {
                std::vector<bool> emptyInputs;
                forwardLink.pulse(emptyInputs); // recurse (kind of)
            }
        }
    } else {
        throw std::invalid_argument("Invalid argument: Pulse does not have the required number of inputs.");
    }
}

