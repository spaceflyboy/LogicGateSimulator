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

    std::cout << "checkPulse DEBUG PRINT:\n";
    std::cout << "this object has " + std::to_string(this->attachedInputInfo.size()) + " attached input gates\n";
    std::cout << "this object has " + std::to_string(this->directInputs) + " direct inputs\n";
    std::cout << "this object has " + std::to_string(this->attachedOutputGates.size()) + " attached output gates\n";

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
    std::cout << "collectPulseInputs: this->inputFlags has size " + std::to_string(this->inputFlags.size()) + "\n";
    int inputsIndex = 0;
    int attachedInputGatesIndex = 0;
    std::vector<bool> args;
    for (bool flag : this->inputFlags) {
        if (flag) {
            args.push_back(inputs[inputsIndex]);
            inputsIndex++;
        } else {
            std::cout << "collectPulseInputs: indirect input processing\n";
            indirect_input_info inputInfo = this->attachedInputInfo[attachedInputGatesIndex];
            Gate inputGate = *inputInfo.attachedInputGate;
            
            operation_output pulseOutput = inputGate.checkPulse(); // Re-using struct for convenience
            if (pulseOutput.success) {
                for (int outputIndex : inputInfo.attachmentIndices) {
                    std::cout << "collectPulseInputs: successfully processed indirect input for gate at index " + std::to_string(attachedInputGatesIndex) + "\n";
                    args.push_back(pulseOutput.outputs[outputIndex]);
                }
                attachedInputGatesIndex++;
            } else {
                std::cout << "collectPulseInputs got a failure for the input gate at index " + std::to_string(attachedInputGatesIndex) + "\n";
                this->pulseOutputs = std::vector<bool>();
                this->validPulse = false;
            }
        }
    }

    return args;
}

pulse_status Gate::pulse(std::vector<bool> inputs) {

    std::cout << "pulse: this->inputFlags size = " + std::to_string(this->inputFlags.size()) + "\n";

    std::vector<bool> args = this->collectPulseInputs(inputs);

    std::cout << "pulse: collected args is size = " + std::to_string(args.size()) + "\n";

    if (args.size() == this->totalInputs) {
        operation_output operationResult = this->operate(args);
        if (operationResult.success) {
            this->pulseOutputs = operationResult.outputs;
            this->validPulse = true;
            for (Gate outputGate : this->attachedOutputGates) {
                std::vector<bool> emptyInputs;
                auto pSC = outputGate.pulse(emptyInputs); // recurse (kind of)
                if (pSC == operationFailure) {
                    throw std::runtime_error("pulse received an operationFailure when forward chaining. Something is wrong.");
                } else if(pSC == inputGateFailure) {
                    std::cout << "Forward pulse input gate failure received!\n";
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

Gate construct_and_link(int totalOutputs, std::vector<bool> inputFlags, std::vector<indirect_input_info> attachedInputInfo, std::vector<Gate> attachedOutputGates, FunctionPointer operation) { 
    Gate returnGate = Gate(totalOutputs, inputFlags, attachedInputInfo, attachedOutputGates, operation);
    returnGate.backwardLink(attachedInputInfo, true);
    return returnGate;
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
    
    this->inputFlags = inputFlags;
    this->validPulse = false;
    this->pulseOutputs = std::vector<bool>();
    this->operation = operation;
    this->attachedOutputGates = attachedOutputGates;
    this->totalOutputs = totalOutputs;

    std::cout << "*** Gate Constructor:\n";
    std::cout << "totalInputs = " + std::to_string(this->totalInputs) + "\n";
    std::cout << "directInputs = " + std::to_string(this->directInputs) + "\n";
    std::cout << "inputFlags size = " + std::to_string(inputFlags.size()) + "\n";
    std::cout << "attachedInputInfo size = " + std::to_string(attachedInputInfo.size()) + "\n";
    std::cout << "attachedOutputGates size = " + std::to_string(attachedOutputGates.size()) + "\n";


    this->attachedInputInfo = attachedInputInfo;

    if (this->attachedInputInfo.size() != 0) {
        std::cout << "this->attachedInputInfo.size() != 0\n";
        for (auto info : this->attachedInputInfo) {
            auto backwardLinkedOutputGates = info.attachedInputGate->attachedOutputGates;
            for (auto outputGate : backwardLinkedOutputGates) {
                std::cout << "\"self\" object has " + std::to_string(outputGate.inputFlags.size()) + " input flags\n";
            }
        }
    }

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
    std::cout << "Forward Link Called * with " + std::to_string(gatesToLink.size()) + " gates and replace_flag = " + std::to_string(replace_flag) + "\n";
    if (replace_flag || gatesToLink.size() == 0) {
        this->attachedOutputGates = gatesToLink;
    } else {
        for (Gate gate : gatesToLink) {
            this->attachedOutputGates.push_back(gate);
        }
    }
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
