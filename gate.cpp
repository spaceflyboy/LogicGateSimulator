#include "gate.h"
#include <stdexcept>
#include <iostream>

operation_output Gate::operate(std::vector<bool> argInputs) {
    operation_output result;
    std::vector<bool> outputs = this->operation(argInputs);
    if (outputs.size() != this->totalOutputs) {
        result.success = false;
        result.outputs = std::vector<bool>();
    } else {
        result.success = true;
        result.outputs = outputs;
    }
    return result;
}   

operation_output Gate::checkPulse() {
    operation_output result;
    if (this->validPulse) {
        result.success = true;
        result.outputs = this->pulseOutputs;
    } else {
        result.success = false;
        result.outputs = std::vector<bool>();
    }
    return result;
}

std::vector<bool> Gate::collectPulseInputs(std::vector<bool> inputs) {
    int inputsIndex = 0;
    int attachedInputGatesIndex = 0;
    std::vector<bool> args;
    for (bool flag : this->inputFlags) {
        if (flag) {
            args.push_back(inputs[inputsIndex]);
            inputsIndex++;
        } else {
            indirect_input_info inputInfo = this->attachedInputInfo[attachedInputGatesIndex];
            Gate inputGate = *inputInfo.attachedInputGate;
            
            operation_output pulseOutput = inputGate.checkPulse(); // Re-using struct for convenience
            if (pulseOutput.success) {
                for (int outputIndex : inputInfo.attachmentIndices) {
                    args.push_back(pulseOutput.outputs[outputIndex]);
                }
                attachedInputGatesIndex++;
            } else {
                this->pulseOutputs = std::vector<bool>();
                this->validPulse = false;
            }
        }
    }

    return args;
}

pulseStatusCode Gate::pulse(std::vector<bool> inputs) {

    if (inputs.size() != this->directInputs) {
        throw std::invalid_argument("Invalid argument: Pulse does not have the required number of inputs.");
    }

    std::vector<bool> args = this->collectPulseInputs(inputs);
    if (args.size() == this->totalInputs) {
        operation_output operationResult = this->operate(args);
        if (operationResult.success) {
            this->pulseOutputs = operationResult.outputs;
            this->validPulse = true;
            for (Gate outputGate : this->attachedOutputGates) {
                std::vector<bool> emptyInputs;
                outputGate.pulse(emptyInputs); // recurse (kind of)
                // We don't care about the return code because partial pulsing is intended
                // I.e. if it has direct inputs it shouldn't be pulsed until it is supplied with real inputs
                // instead of emptyInputs
            }
            
            return success;
        } else {
            this->validPulse = false;
            return operationFailure;
        }
    } else {
        this->validPulse = false;
        return inputGateFailure;
    }
}

Gate::Gate(int totalOutputs, std::vector<bool> inputFlags, std::vector<indirect_input_info> attachedInputInfo, std::vector<Gate> attachedOutputGates, FunctionPointer operation) { 
    this->totalInputs = inputFlags.size();
    int cnt = 0;
    for (int i = 0; i < this->totalInputs; i++) {
        if(inputFlags[i]) {
            cnt++;
        }
    }
    this->directInputs = cnt;
    if (this->totalInputs <= 0 || this->directInputs > this->totalInputs || this->directInputs < 0 || inputFlags.size() != this->totalInputs || ((this->directInputs != this->totalInputs) && attachedInputInfo.size() == 0)) {
        throw std::invalid_argument("Invalid argument(s) for constructing a Gate object. "
                                    "Total inputs must be a positive integer, direct inputs "
                                    "must be an integer between 0 and total inputs, and input " 
                                    "flags are required. Also check that if you have indirect "
                                    "inputs, you are providing a pointer to the required indirect "
                                    "input providing gates.");
    }
    

    this->attachedInputInfo = attachedInputInfo;
    this->inputFlags = inputFlags;
    this->validPulse = false;
    this->pulseOutputs = std::vector<bool>();
    this->operation = operation;
    this->attachedOutputGates = attachedOutputGates;
    this->totalOutputs = totalOutputs;
}


void Gate::backwardLink(std::vector<indirect_input_info> info, bool replace_flag) {
    for (indirect_input_info gate_info : info) {
        if (!replace_flag) {
            this->attachedInputInfo.push_back(gate_info);
        }
        gate_info.attachedInputGate->forwardLink(std::vector<Gate> { *this }, replace_flag);
    }
    
    if (replace_flag) {
        this->attachedInputInfo = info;
    }
}

void Gate::forwardLink(std::vector<Gate> gatesToLink, bool replace_flag) {
    if (replace_flag || gatesToLink.size() == 0) {
        this->attachedOutputGates = gatesToLink;
    } else {
        for (Gate gate : gatesToLink) {
            this->attachedOutputGates.push_back(gate);
        }
    }
}


circuitPulseStatus Gate::pulse(std::vector<bool> oversized_inputs, int startdex) {
    
    std::vector<bool> selected_inputs;
    for (int i = 0; i < this->directInputs; i++) {
        selected_inputs.push_back(oversized_inputs[startdex + i]);
    }
    
    circuitPulseStatus cPS;
    cPS.pSC = this->pulse(selected_inputs);
    cPS.nextIndex = startdex + this->directInputs;
    return cPS;
}
