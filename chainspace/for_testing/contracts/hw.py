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
    print("hello world!")
    return {}

if __name__ == '__main__':
    contract.run()
