#include "gate.h"
#include <stdexcept>
#include <iostream>
//#include "utilities.cpp"

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
            if (inputs.size() >= inputsIndex+1) {
                args.push_back(inputs[inputsIndex]);
                inputsIndex++;
            }
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

pulse_status Gate::pulse(std::vector<bool> inputs) {
    std::vector<bool> args = this->collectPulseInputs(inputs);

    if (args.size() == this->totalInputs) {
        operation_output operationResult = this->operate(args);
        if (operationResult.success) {
            this->pulseOutputs = operationResult.outputs;
            this->validPulse = true;
            for (Gate *outputGate : this->attachedOutputGates) {
                std::vector<bool> emptyInputs;
                auto pSC = outputGate->pulse(emptyInputs); // recurse (kind of)
                if (pSC == operationFailure) {
                    throw std::runtime_error("*** pulse received an operationFailure when forward chaining. Something is wrong.");
                } 
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

Gate::Gate(int totalOutputs, std::vector<bool> inputFlags, FunctionPointer operation) { 
    this->totalInputs = inputFlags.size();

    if (this->totalInputs == 0) {
        throw std::invalid_argument("Invalid argument(s) for constructing a Gate object. "
                                    "Total inputs must be a positive integer, direct inputs "
                                    "must be an integer between 0 and total inputs, and input " 
                                    "flags are required. Also check that if you have indirect "
                                    "inputs, you are providing a pointer to the required indirect "
                                    "input providing gates.");
    }

    int cnt = 0;
    for (int i = 0; i < this->totalInputs; i++) {
        if(inputFlags[i]) {
            cnt++;
        }
    }
    this->directInputs = cnt;
    this->inputFlags = inputFlags;
    this->validPulse = false;
    this->pulseOutputs = std::vector<bool>();
    this->operation = operation;
    this->attachedOutputGates = std::vector<Gate *>();
    this->totalOutputs = totalOutputs;
    this->attachedInputInfo = std::vector<indirect_input_info>();
}

void Gate::connect(std::vector<Gate *> inputGatesToLink, std::vector<std::vector<int>> attachmentIndicesList) {
    int inputIndex = 0;
    for (Gate * gate: inputGatesToLink) {
        if (gate) {
            gate->attachedOutputGates.push_back(this);
        } else {
            std::runtime_error("Null pointer in connect's inputGatesToLink parameter");
        }

        indirect_input_info info;
        info.attachedInputGate = gate;
        info.attachmentIndices = attachmentIndicesList[inputIndex];

        for (int index : info.attachmentIndices) {
            if (index + 1 > info.attachedInputGate->totalOutputs) {
                throw std::invalid_argument("attachmentIndicesList contains one or more indices which exceed the size of the attachedInputGate's totalOutputs.");
            }
        }

        this->attachedInputInfo.push_back(info);

        inputIndex++;
    }
}

void Gate::clearInputConnections() {
    this->attachedInputInfo = std::vector<indirect_input_info>();
}

void Gate::clearOutputConnections() {
    this->attachedOutputGates = std::vector<Gate *>();
}

circuit_pulse_status Gate::pulse(std::vector<bool> oversized_inputs, int startdex) {
    
    std::vector<bool> selected_inputs;
    for (int i = 0; i < this->directInputs; i++) {
        selected_inputs.push_back(oversized_inputs[startdex + i]);
    }
    
    circuit_pulse_status cPS;
    cPS.pulseStatus = this->pulse(selected_inputs);
    cPS.nextIndex = startdex + this->directInputs;
    return cPS;
}
