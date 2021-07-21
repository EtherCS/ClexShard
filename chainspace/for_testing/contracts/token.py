from chainspacecontract import ChainspaceContract

contract = ChainspaceContract('token')

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

balance = [0, 0, 0]
@contract.method('create_account')
def create_account(inputs, reference_inputs, parameters):
    user = int(parameters[0])
    ini_coins = int(parameters[1])
    filepath = parameters[2]

    try:
        fp = open(filepath, "a+")
        for index in range(3):
            line = fp.next()
            balance[index] = int(line.strip())
    except StopIteration:
        fp.close()
    else:
        balance[user] = ini_coins
        fp.close()
    try:
        wfp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        for i in range(3):
            wfp.write("{0}\n".format(balance[i]))
        wfp.close()

    return {}

@contract.method('getBalance')
def getBalance(inputs, reference_inputs, parameters):
    user = int(parameters[0])
    filepath = parameters[1]
    outputs = ()
    try:
        fp = open(filepath, "a+")
        for index in range(3):
            line = fp.next()
            balance[index] = int(line.strip())
    except StopIteration:
        fp.close()
    else:
        fp.close()
    outputs = outputs + ("The balance of user {0} is {1}.".format(user, balance[user]),)
    return {
        'outputs': outputs
    }

@contract.method('transfer')
def transfer(inputs, reference_inputs, parameters):
    user_a, user_b, value, filepath = int(parameters[0]), int(parameters[1]), int(parameters[2]), parameters[3]
    outputs = ()
    try:
        fp = open(filepath, "a+")
        for index in range(3):
            line = fp.next()
            balance[index] = int(line.strip())
    except StopIteration:
        fp.close()
    else:
        fp.close()
    try:
        wfp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        if balance[user_a] < value:
            outputs = outputs + ("user {0} has not enough coins!\n".format(user_a),)
        else:
            balance[user_a] -= value
            balance[user_b] += value
            for i in range(3):
                wfp.write("{0}\n".format(balance[i]))
            outputs = outputs + ("ok\n",)
        wfp.close()
    return {
        'outputs': outputs
    }

if __name__ == '__main__':
    contract.run()
