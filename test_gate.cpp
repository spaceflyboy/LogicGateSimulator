#include "gate.cpp"
#include <iostream>

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

bool testGatePulse(int test_index, int totalOutputs, std::vector<bool> inputFlags, std::vector<indirect_input_info> attachedInputInfo, std::vector<Gate> attachedOutputGates, FunctionPointer operation, std::vector<bool> directInputs, std::vector<bool> expectedOutputs) {
    try {
        Gate g = Gate(totalOutputs, inputFlags, attachedInputInfo, attachedOutputGates, &AND_2_1);
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

std::vector<bool> testGatePulses(int starting_test_index, int totalOutputs, std::vector<bool> inputFlags, std::vector<indirect_input_info> attachedInputInfo, std::vector<Gate> attachedOutputGates, FunctionPointer operation, std::vector<std::vector<bool>> directInputsList, std::vector<std::vector<bool>> expectedOutputsList) {
    std::vector<bool> results;
    try {
        Gate g = Gate(totalOutputs, inputFlags, attachedInputInfo, attachedOutputGates, operation);
        return testGatePulses_GATEPARAM(starting_test_index, g, directInputsList, expectedOutputsList);
    } catch (std::runtime_error e) {
        std::cout << e.what();
        return results;
    }
}

void test() {
    std::vector<bool> test_successes;

    // Test 0: AND_2_1 function
    std::vector<std::vector<bool>> inputs_for_testing {
        std::vector<bool> {true, true},
        std::vector<bool> {true, false},
        std::vector<bool> {false, true},
        std::vector<bool> {false, false}
    };

    int test_index = 0;
    int assertion_index = 0;
    try {
        for (std::vector<bool> input_set : inputs_for_testing) {
            std::vector<bool> outputs = AND_2_1(input_set);
            if (outputs[0] != (input_set[0] && input_set[1])) {
                char *s = (char *)"";
                sprintf(s, "Test assertion %d:%d failed", test_index, assertion_index);
                throw std::runtime_error(s);
            }
            assertion_index++;
        }
        test_successes.push_back(true);
    } catch(std::runtime_error e) {
        test_successes.push_back(false);
        std::cout << e.what();
    }
    test_index++;

    //Test 1: Gate Constructor
    try {
        Gate g = Gate(1, std::vector<bool> { true, true }, std::vector<indirect_input_info>(), std::vector<Gate>(), &AND_2_1);
        test_successes.push_back(true);
    } catch (std::invalid_argument e) {
        test_successes.push_back(false);
        std::cout << e.what();
    }
    test_index++;

    // Tests 2-5: AND_2_1 GATE PULSE FULLY DIRECT INPUT TEST
    std::vector<std::vector<bool>> expectedOutputsList = 
    { 
        {true},
        {false},
        {false},
        {false}
    };

    auto res = testGatePulses(test_index, 1, std::vector<bool> {true, true}, std::vector<indirect_input_info>(), std::vector<Gate>(), &AND_2_1, inputs_for_testing, expectedOutputsList);
    for (bool result: res) {
        test_successes.push_back(result);
    }
    
    int i = 0;
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