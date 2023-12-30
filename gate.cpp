#include "gate.h"
#include <stdexcept>
#include <iostream>
#include <string>

std::string gate_to_string(const Gate& g) {
    std::string s = "";
    std::vector<int> gateFields = g.debugGet();
    s += "G(" + std::to_string(gateFields[0]) + " totalInputs, "\
    + std::to_string(gateFields[1]) + " directInputs, "\
    + std::to_string(gateFields[2]) + " totalOutputs, "\
    + std::to_string(gateFields[3]) + " inputFlags size, "\
    + std::to_string(gateFields[4]) + " attachedInputInfo objects, "\
    + std::to_string(gateFields[5]) + " attachedOutputGate objects)";
    return s;
}

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
    std::cout << "*** checkPulseCalled: Gate = \n";

    std::cout << "\t" + gate_to_string(*this) + "\n";

    operation_output result;
    if (this->validPulse) {
        std::cout << "*** checkPulse success\n";
        result.success = true;
        result.outputs = this->pulseOutputs;
    } else {
        std::cout << "*** checkPulse failure\n";
        result.success = false;
        result.outputs = std::vector<bool>();
    }
    return result;
}

std::vector<bool> Gate::collectPulseInputs(std::vector<bool> inputs) {

    std::cout << "*** collecting pulse inputs\n";

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

    std::cout << "*** finished collecting pulse inputs\n";
    return args;
}

pulse_status Gate::pulse(std::vector<bool> inputs) {
    std::cout << "*** pulse called: Gate = \n";

    using std::to_string;
    std::cout << "\t" + gate_to_string(*this) + "\n";

    std::vector<bool> args = this->collectPulseInputs(inputs);

    if (args.size() == this->totalInputs) {
        operation_output operationResult = this->operate(args);
        if (operationResult.success) {
            this->pulseOutputs = operationResult.outputs;
            this->validPulse = true;
            for (Gate outputGate : this->attachedOutputGates) {
                std::vector<bool> emptyInputs;
                auto pSC = outputGate.pulse(emptyInputs); // recurse (kind of)
                if (pSC == operationFailure) {
                    throw std::runtime_error("*** pulse received an operationFailure when forward chaining. Something is wrong.");
                } else if(pSC == inputGateFailure) {
                    std::cout << "*** pulse received an inputGateFailure in forward chaining\n";
                }
            }

            std::cout << "*** pulse: success\n";
            return success;
        } else {
            std::cout << "*** pulse: operation failure\n";
            this->validPulse = false;
            return operationFailure;
        }
    } else {
        std::cout << "*** pulse: input gate failure\n";
        this->validPulse = false;
        return inputGateFailure;
    }
}

Gate construct_and_link(int totalOutputs, std::vector<bool> inputFlags, std::vector<indirect_input_info> attachedInputInfo, std::vector<Gate> attachedOutputGates, FunctionPointer operation) { 
    Gate returnGate = Gate(totalOutputs, inputFlags, attachedInputInfo, attachedOutputGates, operation);
    returnGate.backwardLink(attachedInputInfo, true);

    for (indirect_input_info info : attachedInputInfo) {
        info.attachedInputGate->forwardLink(std::vector<Gate> {returnGate}, true);
    }

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
    this->attachedInputInfo = attachedInputInfo;
}


void Gate::backwardLink(std::vector<indirect_input_info> info, bool replace_flag) {
    
    if (replace_flag) {
        this->attachedInputInfo = info;
    } else {    
        for (indirect_input_info gate_info : info) {
            this->attachedInputInfo.push_back(gate_info);
        }
    }
    
}

void Gate::forwardLink(std::vector<Gate> gatesToLink, bool replace_flag) {

    std::cout << "*** forwardLink called with " + std::to_string(gatesToLink.size()) + " gates to link for Gate = \n";
    
    std::cout << "\t" + gate_to_string(*this) + "\n";

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
