from contracts import auction

def invoke(inputs=None, reference_inputs=None, parameters=None):
    return auction.bidItem(inputs,reference_inputs,["gudale","1000","/chainspace/for_testing/file/auction_state.txt"])
