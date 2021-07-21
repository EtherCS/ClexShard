from contracts import auction

def invoke(inputs=None, reference_inputs=None, parameters=None):
    return auction.releaseItem(inputs,reference_inputs,["macbook","/chainspace/for_testing/file/auction_state.txt"])
