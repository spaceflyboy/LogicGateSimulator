#include "gate.cpp"
#include <iostream>
#include <string>

std::vector<bool> AND_2_1(std::vector<bool> inputs) {
    if (inputs.size() != 2) {
        return std::vector<bool>();
    }

    bool check = true;
    int i = 0;
    while(check && i < inputs.size()) {
        check = check && inputs[i];
        i++;
    }

    return std::vector<bool> { check };
}

void give_test_fail_reason(std::string msg) {
    std::cout << msg;
}

bool testGatePulse_GATEPARAM(int test_index, Gate g, std::vector<bool> directInputs, std::vector<bool> expectedOutputs) {
    try {
        auto pSC = g.pulse(directInputs);
        if (pSC != success) {
            give_test_fail_reason("Test " + std::to_string(test_index) + " failed due to a pulse failure. pulseStatusCode: " + std::to_string(pSC) + "\n");
            return false;
        } else {
            operation_output output = g.checkPulse();
            if (output.success == false) {
                give_test_fail_reason("Test " + std::to_string(test_index) + " failed due to pulse failure indicated by checkPulse()\n");
                return false;
            } else {
                bool output_check = (expectedOutputs.size() == output.outputs.size());
                int outdex = 0;
                for (bool output_val : output.outputs) {
                    if (output_val != expectedOutputs[outdex]) {
                        output_check = false;
                        break;
                    }
                    outdex++;
                }

                if(output_check) {
                    return true;
                } else {
                    give_test_fail_reason("Test " + std::to_string(test_index) + " failed due to pulse generating the incorrect value\n");
                    return false;
                }
            }
        }
    } catch (std::runtime_error e) {
        std::cout << e.what();
        return false;
    }
}

bool testGatePulse(int test_index, int totalOutputs, std::vector<bool> inputFlags, FunctionPointer operation, std::vector<bool> directInputs, std::vector<bool> expectedOutputs) {
    try {
        Gate g = Gate(totalOutputs, inputFlags, &AND_2_1);
        return testGatePulse_GATEPARAM(test_index, g, directInputs, expectedOutputs);
    } catch (std::runtime_error e) {
        std::cout << e.what();
        return false;
    }
}

std::vector<bool> testGatePulses_GATEPARAM(int test_index, Gate g, std::vector<std::vector<bool>> directInputsList, std::vector<std::vector<bool>> expectedOutputsList) {
    std::vector<bool> testResults;
    int i = 0;
    for (std::vector<bool> expectedOutputs : expectedOutputsList) {
        try {
            testResults.push_back(testGatePulse_GATEPARAM(test_index, g, directInputsList[i], expectedOutputs));
        } catch(std::runtime_error e) {
            std::cout << e.what();
            return testResults;
        }
        test_index++;
        i++;
    }
    return testResults;
}

std::vector<bool> testGatePulses(int starting_test_index, int totalOutputs, std::vector<bool> inputFlags, FunctionPointer operation, std::vector<std::vector<bool>> directInputsList, std::vector<std::vector<bool>> expectedOutputsList) {
    std::vector<bool> results;
    try {
        Gate g = Gate(totalOutputs, inputFlags, operation);
        return testGatePulses_GATEPARAM(starting_test_index, g, directInputsList, expectedOutputsList);
    } catch (std::runtime_error e) {
        std::cout << e.what();
        return results;
    }
}

void test() {
    std::vector<bool> test_successes;
    // Test 1: AND_2_1 function
    std::vector<std::vector<bool>> inputs_for_testing {
        std::vector<bool> {true, true},
        std::vector<bool> {true, false},
        std::vector<bool> {false, true},
        std::vector<bool> {false, false}
    };

    int assertion_index = 0;
    try {
        for (std::vector<bool> input_set : inputs_for_testing) {
            std::vector<bool> outputs = AND_2_1(input_set);
            if (outputs[0] != (input_set[0] && input_set[1])) {
                char *s = (char *)"";
                sprintf(s, "Test assertion 1:%d failed", assertion_index);
                throw std::runtime_error(s);
            }
            assertion_index++;
        }
        test_successes.push_back(true);
    } catch(std::runtime_error e) {
        test_successes.push_back(false);
        std::cout << e.what();
    }

    //Test 2: Gate Constructor
    try {
        Gate g = Gate(1, std::vector<bool> { true, true }, &AND_2_1);
        test_successes.push_back(true);
    } catch (std::invalid_argument e) {
        test_successes.push_back(false);
        std::cout << e.what();
    }

    // Tests 3-6: AND_2_1 GATE PULSE FULLY DIRECT INPUT TEST
    std::vector<std::vector<bool>> expectedOutputsList = 
    { 
        {true},
        {false},
        {false},
        {false}
    };

    int testIndex = 3;
    auto res = testGatePulses(testIndex, 1, std::vector<bool> {true, true}, &AND_2_1, inputs_for_testing, expectedOutputsList);
    for (bool result: res) {
        test_successes.push_back(result);
    }

    // Test 7: AND_2_1 2 LAYER GATE PULSE WITH CONNECTIONS
    Gate layer1 = Gate(1, std::vector<bool> {true, true}, &AND_2_1);
    Gate layer2 = Gate(1, std::vector<bool> {false, true}, &AND_2_1);
    layer2.connect(std::vector<Gate *> {&layer1}, std::vector<std::vector<int>> {{0}});
    auto pSC = layer1.pulse(std::vector<bool> {true, true});

    if (pSC == success) {
        operation_output layer2_check = layer2.checkPulse();
        if (layer2_check.success) {
            give_test_fail_reason("Test 7 Failure: Second layer gate says it has a valid pulse but shouldn't have the requisite inputs.\n");
            test_successes.push_back(false);
        } else {
            pSC = layer2.pulse(std::vector<bool> { true });
            if (pSC == success) {
                operation_output layer2_check = layer2.checkPulse();
                if (layer2_check.success) {
                    if (layer2_check.outputs.size() != 0 && layer2_check.outputs[0] == true) {
                        test_successes.push_back(true);
                    } else {
                        give_test_fail_reason("Test 7 Failure: Second layer pulse succeeded and checkPulse() returned, but the output value is incorrect.\n");
                        test_successes.push_back(false);
                    }

                } else {
                    give_test_fail_reason("Test 7 Failure: Second layer checkPulse failed.\n");
                    test_successes.push_back(false);
                }
            } else {
                give_test_fail_reason("Test 7 Failure: Second layer pulse failed.\n");
                test_successes.push_back(false);
            }
        }
    } else {
        give_test_fail_reason("Test 7 Failure: First layer pulse failed despite having everything it needs.\n");
        test_successes.push_back(false);
    }

    // Test 8: Multi-layer, multi-input-gate pulse test. Also a connect and circuit pulse method test.
    Gate inputGate1 = Gate(1, std::vector<bool> {true, true}, &AND_2_1);
    Gate inputGate2 = Gate(1, std::vector<bool> {true, true}, &AND_2_1);
    layer2 = Gate(1, std::vector<bool> {false, false}, &AND_2_1);
    layer2.connect(std::vector<Gate *> {&inputGate1, &inputGate2}, std::vector<std::vector<int>> {{0}, {0}});

    std::vector<bool> oversizedInputs = {true, true, true, false};
    int pulsedex = 0;
    circuit_pulse_status cPS = inputGate1.pulse(oversizedInputs, pulsedex);
    operation_output inputGate1Pulse = inputGate1.checkPulse();
    if (cPS.pulseStatus == success && inputGate1Pulse.success && inputGate1Pulse.outputs.size() == 1 && inputGate1Pulse.outputs[0] == true) {
        cPS = inputGate2.pulse(oversizedInputs, cPS.nextIndex);
        operation_output inputGate2Pulse = inputGate2.checkPulse();
        if (cPS.pulseStatus == success && inputGate2Pulse.success && inputGate2Pulse.outputs.size() == 1 && inputGate2Pulse.outputs[0] == false) {
            operation_output pulseOutput = layer2.checkPulse();

            if (pulseOutput.success && pulseOutput.outputs.size() == 1 && pulseOutput.outputs[0] == false) {
                test_successes.push_back(true);
            } else {
                test_successes.push_back(false);
                if (pulseOutput.success) {
                    give_test_fail_reason("Test 8 Failure: layer2.checkPulse() succeeded but gave the wrong value.\n");
                } else {
                    give_test_fail_reason("Test 8 Failure: layer2.checkPulse() returned failure.\n");
                }
            }
        } else {
            test_successes.push_back(false);
            if (cPS.pulseStatus == success) {
                if(inputGate2Pulse.success) {
                    give_test_fail_reason("Test 8 Failure: inputGate2.checkPulse() succeeded but gave the wrong value.\n");

                } else {
                    give_test_fail_reason("Test 8 Failure: inputGate2.checkPulse did not succeed.\n");
                }
            } else {
                give_test_fail_reason("Test 8 Failure: inputGate2.pulse() failed.\n");
            }
        }
    } else {
        test_successes.push_back(false);
        if (cPS.pulseStatus == success) {
            if(inputGate1Pulse.success) {
                give_test_fail_reason("Test 8 Failure: inputGate1.checkPulse() succeeded but gave the wrong value.\n");

            } else {
                give_test_fail_reason("Test 8 Failure: inputGate1.checkPulse did not succeed.\n");

            }
        } else {
            give_test_fail_reason("Test 8 Failure: inputGate1.pulse did not succeed.\n");
        }
    }



    // RESULTS FORMATTING
    int i = 1;
    for (bool test_success : test_successes) {
        if (test_success) {
            std::cout << "Test " + std::to_string(i) + " Succeeded!\n";
        } else {
            std::cout << "Test " + std::to_string(i) + " Failed!\n";
        }
        i++;
    }
}   

int main() {
    std::cout << "Fuck C++\n";
    test();
    return 0;
}