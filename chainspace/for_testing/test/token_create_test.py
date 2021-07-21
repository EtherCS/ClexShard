from contracts import token

def invoke(inputs=None, reference_inputs=None, parameters=None):
    return token.create_account(inputs,reference_inputs,["1","10","/chainspace/for_testing/file/transfer_state.txt"])
