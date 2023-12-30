#include <string>
#include "gate.h"

std::string gate_to_string(const Gate& g) {
    std::string s = "";
    std::vector<int> gateFields = g.debugGateFieldFetch();
    s += "G(" + std::to_string(gateFields[0]) + " totalInputs, "\
    + std::to_string(gateFields[1]) + " directInputs, "\
    + std::to_string(gateFields[2]) + " totalOutputs, "\
    + std::to_string(gateFields[3]) + " inputFlags size, "\
    + std::to_string(gateFields[4]) + " attachedInputInfo objects, "\
    + std::to_string(gateFields[5]) + " attachedOutputGate objects, "\
    + std::to_string(gateFields[6]) + " validPulse flag)";
    return s;
}
