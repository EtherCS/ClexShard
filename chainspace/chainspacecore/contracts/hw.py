from chainspacecontract import ChainspaceContract

contract = ChainspaceContract('hw')

@contract.method('init')
def init():
    return {
        'outputs': ('o',)
    }

@contract.method('create')
def create(inputs, reference_inputs, parameters):
    count = int(parameters[0])
    return {
        'outputs': ('o',)*count
    }

@contract.method('hw')
def hw(inputs, reference_inputs, parameters):
    filepath = parameters[0]
    try:
        fp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        fp.write("Hello World\n")
        fp.close()
    return {}

if __name__ == '__main__':
    contract.run()
