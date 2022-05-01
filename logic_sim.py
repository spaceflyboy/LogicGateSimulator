import abc

# Abstract Base Class representing a logic gate
# Abstract methods:
#     pulse(...): Take in inputs and return the logic gate's output
#     get_num_req_inputs(): Return the number of inputs this gate takes in (E.g. 1 for NOT, 2 for AND)
#     to_string(): Return a string representation of the gate. E.g. \"AND\" for an AND gate.
class LogicGate(object): 
    __metaclass__ = abc.ABCMeta
    
    @abc.abstractmethod
    def pulse(self, inputs):
        # The pulse method will take in boolean input (either a bool or a tuple of bools) and return
        # output that corresponds to the particular kind of logic gate
        return
    
    @abc.abstractmethod
    def get_num_req_inputs(self):
        # This method should return the integer number of inputs required by this gate
        return
    
    @abc.abstractmethod
    def to_string():
        # Provide a string representation of the gate. E.g. \"AND\" for an AND gate.
        return

# Abstract Class which inherits from LogicGate and distinguishes UnaryGates as requiring only one input
# Abstract methods:
#     pulse(...): Take in inputs and return the logic gate's output
#     to_string(): Return a string representation of the gate. E.g. \"AND\" for an AND gate.
# Implemented methods:
#     get_num_req_inputs(): Return the number of inputs this gate takes in (always 1)
class UnaryGate(LogicGate):
    __metaclass__ = abc.ABCMeta
    
    @abc.abstractmethod
    def pulse(self, inputs):
        # The pulse method will take in boolean input (either a bool or a tuple of bools) and return
        # output that corresponds to the particular kind of logic gate
        return
    
    @abc.abstractmethod
    def to_string():
        # Provide a string representation of the gate. E.g. \"AND\" for an AND gate.
        return
        
    def get_num_req_inputs(self):
        return 1

# Abstract Class which inherits from LogicGate and distinguishes BinaryGates as requiring two inputs
# Abstract methods:
#     pulse(...): Take in inputs and return the logic gate's output
#     to_string(): Return a string representation of the gate. E.g. \"AND\" for an AND gate.
# Implemented methods:
#     get_num_req_inputs(): Return the number of inputs this gate takes in (always 2)  
class BinaryGate(LogicGate):
    __metaclass__ = abc.ABCMeta
    
    @abc.abstractmethod
    def pulse(self, inputs):
        # The pulse method will take in boolean input (either a bool or a tuple of bools) and return
        # output that corresponds to the particular kind of logic gate
        return
    
    @abc.abstractmethod
    def to_string():
        # Provide a string representation of the gate. E.g. \"AND\" for an AND gate.
        return
        
    def get_num_req_inputs(self):
        return 2

# Concrete, derived class which represents a Buffer Gate
class BUFFER(UnaryGate):
    def __init__(self):
        super().__init__()
    
    def pulse(self, inputs):
        assert isinstance(inputs[0], bool)
        return inputs[0]
        
    def to_string():
        return "BUFFER"

# Concrete, derived class which represents a NOT Gate
class NOT(UnaryGate):
    def __init__(self):
        super().__init__()
        
    def pulse(self, inputs):
        assert isinstance(inputs[0], bool)
        return not inputs[0]
        
    def to_string():
        return "NOT"

# Concrete, derived class which represents an AND Gate
class AND(BinaryGate):
    def __init__(self):
        super().__init__()
        
    def pulse(self, inputs):
        assert isinstance(inputs, list)
        assert len(inputs) == 2
        assert isinstance(inputs[0], bool)
        assert isinstance(inputs[1], bool)
        
        in1 = inputs[0]
        in2 = inputs[1]
        return in1 and in2
        
    def to_string():
        return "AND"

# Concrete, derived class which represents an OR Gate
class OR(BinaryGate):
    def __init__(self):
        super().__init__()
        
    def pulse(self, inputs):
        assert isinstance(inputs, list)
        assert len(inputs) == 2
        assert isinstance(inputs[0], bool)
        assert isinstance(inputs[1], bool)
        
        in1 = inputs[0]
        in2 = inputs[1]
        return in1 or in2
        
    def to_string():
        return "OR"

# Concrete, derived class which represents an XOR Gate
class XOR(BinaryGate):
    def __init__(self):
        super().__init__()
        
    def pulse(self, inputs):
        assert isinstance(inputs, list)
        assert len(inputs) == 2
        assert isinstance(inputs[0], bool)
        assert isinstance(inputs[1], bool)
        
        in1 = inputs[0]
        in2 = inputs[1]
        return in1 ^ in2
        
    def to_string():
        return "XOR"

# Concrete, derived class which represents a NAND Gate
class NAND(BinaryGate):
    def __init__(self):
        super().__init__()
        
    def pulse(self, inputs):
        assert isinstance(inputs, list)
        assert len(inputs) == 2
        assert isinstance(inputs[0], bool)
        assert isinstance(inputs[1], bool)
        
        in1 = inputs[0]
        in2 = inputs[1]
        return not (in1 and in2)
        
    def to_string():
        return "NAND"
        
# Concrete, derived class which represents a NOR Gate
class NOR(BinaryGate):
    def __init__(self):
        super().__init__()
        
    def pulse(self, inputs):
        assert isinstance(inputs, list)
        assert len(inputs) == 2
        assert isinstance(inputs[0], bool)
        assert isinstance(inputs[1], bool)
        
        in1 = inputs[0]
        in2 = inputs[1]
        return not (in1 or in2)
        
    def to_string():
        return "NOR"

# Concrete, derived class which represents an XNOR Gate
class XNOR(BinaryGate):
    def __init__(self):
        super().__init__()
    
    def pulse(self, inputs):
        assert isinstance(inputs, list)
        assert len(inputs) == 2
        assert isinstance(inputs[0], bool)
        assert isinstance(inputs[1], bool)
        
        in1 = inputs[0]
        in2 = inputs[1]
        return not (in1 ^ in2)
        
    def to_string():
        return "XNOR"

class GateNode():
    def __init__(self):
        self.gate = None
        self.nexts = []
        #self.prevs = []
        self.id = -1
    
    def __init__(self, nexts, gate):
        self.nexts = nexts
        #self.prevs = prevs
        self.gate = gate
        self.id = -1
        assert isinstance(self.nexts, list)
        assert isinstance(self.prevs, list)
        assert isinstance(self.gate, LogicGate)
        assert isinstance(self.id, int)
    
    def __init__(self, nexts, gate, id):
        self.nexts = nexts
        #self.prevs = prevs
        self.gate = gate
        self.id = id
        assert isinstance(self.nexts, list)
        #assert isinstance(self.prevs, list)
        assert isinstance(self.gate, LogicGate)
        assert isinstance(self.id, int)
        assert self.id >= 0
    
    def getID(self):
        return self.id
        
    def setID(self, id):
        assert isinstance(id, int)
        assert id >= 0
        self.id = id
    
    def resetAllIDs(self):
        self.resetID()
        for child in self.nexts:
            child.resetAllIDs()
    
    def resetID(self):
        self.id = -1
    
    def addNext(self, next):
        self.nexts.append(next)
        
    """
    def addPrev(self, prev):
        self.prevs.append(prev)
    """
    
    def removeNext(self, next_index):
        assert next_index < len(self.nexts)
        assert next index >= 0
        return self.nexts.pop(next_index)
    
    """
    def removePrev(self, prev_index):
        assert prev_index < len(self.prevs)
        assert prev_index >= 0
        return self.prevs.pop(prev_index)
    """
    
    def getNexts(self):
        return self.nexts
    
    """
    def getPrevs(self):
        return self.prevs
    """
    def setGate(self, gate):
        self.gate = gate
        
    def get_num_req_inputs():
        if self.gate is None:
            raise Exception("Cannot get number of required inputs because this node doesn't contain a gate")
        return self.gate.get_num_req_inputs()
        
    def pulse(self, inputs):
        if self.gate is None: 
            raise Exception("Cannot pulse this GateNode because it doesn't contain a gate")
        try:
            return self.gate.pulse(inputs)
        except AssertionError as e:
            print(f"Received an assertion error. That means the provided inputs are improper for a {self.gate.to_string()} gate")
            raise e
    
    def to_string(self):
        gate_str = "" if self.gate is None else self.gate.to_string()
        return f"GateNode with ID {self.id} : Contains a {gate_str} gate"

class CIRCUIT(LogicGate):
    def __init__(self):
        super().__init__()
        self.input_gates = []
        self.updated_IDs = False    
    
    def getInputNodes(self):
        return self.input_gates
        
    def unassignNodeIDs(self):
        for node in self.input_gates:
            node.resetAllIDs()
        self.updated_IDs = False
    
    def assign_r(root, id):
        root.setID(id)
        id += 1
        returnVal = id
        idx = 0
        children = root.getNexts()
        
        for child in children
            if idx == len(children)-1:
                returnVal = assign_r(child, id)
            else:
                id = assign_r(child, id)
            idx += 1
        return returnVal
    
    def assignNodeIDs(self):
        if self.updated_IDs:
            return
        else:
            self.unassignNodeIDs()
        
        cur_id = 0
 
        for node in self.input_gates:
            next_id = assign_r(node, cur_id)
            cur_id = next_id             
                
        self.updated_IDs = True
    
    def addInputNode(self, node):
        assert isinstance(node, GateNode)
        self.input_gates.append(node)
        self.updated_IDs = False
        
    def removeInputNode(self, index):
        assert isinstance(index, int)
        assert index >= 0
        assert index < len(self.input_gates)
        self.input_gates.pop(index)
    
    def get_num_req_inputs(self):
        input_requirement_count = 0
        for node in self.input_gates:
            assert isinstance(node, GateNode)
            try:
                input_requirement_count += node.get_num_req_inputs()
            except Exception as e:
                print(f"CIRCUIT.get_num_req_inputs caught the following exception: {e}")
                raise e
        return input_requirement_count
    
    # Helper method for CIRCUIT.pulse(...) which will sort out relationships between multiple input gates
    # Inputs:
    #     input_gates: List of input GateNodes which contain connections to all of the subsequent gate nodes.
    # Outputs:
    def preprocess_input_hieararchy(self, input_gates):
        input_ids = [gate.getID() for gate in input_gates] 
        
        
    
    # Helper method for CIRCUIT.pulse which will find intersections.
    # Inputs:
    #     input_gates: List of input GateNodes which contain connections to all of the subsequent gate nodes.
    # Outputs:
    #     list of IDs at which input gates intersect and a tuple of input_gates which intersect at that point 
    def find_intersections(self, input_gates):
        intersections = []
        visited_nodes = {}
        
        cur_layer = None
        input_idx = 0
        for node in input_gates:
            id = node.getID()
            assert id != -1
            if id in visited_nodes:
                # Intersection found
                visited_nodes[id] += (input_idx, )           
            else:
                # If it is an intersection, we won't find out on this iteration
                visited_nodes[id] = (input_idx, )
            cur_layer = node.getNexts()
            for i in range(len(cur_layer)):
                
            input_idx += 1
                
            
        return intersections
    
    def pulse(self, inputs):
        if self.input_gates:
            assert isinstance(inputs, list)
            assert self.get_num_req_inputs() == len(inputs)
            
            self.assignNodeIDs() # ID every Node se we can detect overlaps between each input node's general tree
            # Our underlying circuit actually has gates that take inputs and we have been supplied enough of them
            assert self.updated_IDs
            
            intersection_points = self.find_intersections(self.input_gates)
            
            for node in input_gates:
                
            
                try:
                    return input_gates.pulse(inputs)
                except AssertionError as e:
                    print(f"CIRCUIT.pulse(): Passing in that input tripped this assertion: {e}")
                    raise e
                except Exception as e:
                    print(f"CIRCUIT.pulse(): Caught the following exception: {e}")
                    raise e
                    
        raise Exception("This circuit has no input gates, cannot pulse it.")
        
    def to_string():
        return "CIRCUIT"
        
"""
class CompoundCircuit:
"""        
    

def main():
    return

if __name__ == "__main__:
    main()