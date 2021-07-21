from contracts import hw

def invoke(inputs=None, reference_inputs=None, parameters=None):
    return hw.hw(inputs, reference_inputs, ['/chainspace/for_testing/file/hw_output.txt'])
