from chainspacecontract import ChainspaceContract

contract = ChainspaceContract('vote')

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

@contract.method('addCandidate')
def addCandidate(inputs, reference_inputs, parameters):
    user, filepath = int(parameters[0]), parameters[1]
    candidates = [0, 0, 0]
    fp = open(filepath, "a+")
    for index in range(3):
        line = fp.next()
        candidates[index] = int(line.strip())
    fp.close()

    candidates[user] = 1

    try:
        wfp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        for i in range(3):
            wfp.write("{0}\n".format(candidates[i]))
        wfp.close()
    return {}

@contract.method('vote')
def vote(inputs, reference_inputs, parameters):
    candidate, filepath = int(parameters[0]), parameters[1]
    candidates = [0, 0, 0]
    fp = open(filepath, "a+")
    for index in range(3):
        line = fp.next()
        candidates[index] = int(line.strip())
    fp.close()

    candidates[candidate] += 1

    try:
        wfp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        for i in range(3):
            wfp.write("{0}\n".format(candidates[i]))
        wfp.close()
    return {}

if __name__ == '__main__':
    contract.run()
