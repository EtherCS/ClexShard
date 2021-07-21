from contracts import vote

def invoke(inputs=None, reference_inputs=None, parameters=None):
    return vote.addCandidate(inputs,reference_inputs,["1","/chainspace/for_testing/file/vote_state.txt"])
