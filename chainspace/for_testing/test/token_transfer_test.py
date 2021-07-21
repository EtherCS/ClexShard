from contracts import token

def invoke(inputs=None, reference_inputs=None, parameters=None):
    return token.transfer(inputs,reference_inputs,["1","2","0","/chainspace/for_testing/file/transfer_state.txt"])
