from chainspacecontract import ChainspaceContract

contract = ChainspaceContract('auction')

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

@contract.method('releaseItem')
def releaseItem(inputs, reference_inputs, parameters):
    item = parameters[0]
    filepath = parameters[1]
    try:
        fp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        fp.write("{0}\n".format(item))
        fp.write("NULL\n")
        fp.write("0\n")
        fp.close()
    return {}

@contract.method('bidItem')
def bidItem(inputs, reference_inputs, parameters):
    user = parameters[0]
    money = int(parameters[1])
    filepath = parameters[2]
    fp = open(filepath, "a+")
    item = fp.readline().strip()
    pre_user = fp.readline().strip()
    pre_money = int(fp.readline().strip())
    fp.close()

    if cmp(user, pre_user) == 0:
        return {}

    if pre_money > money:
        print "Not Enough.\n"
        return {}
    try:
        fp = open(filepath, "w")
    except IOError:
        raise IOError("Cannot open write file")
    else:
        fp.write("{0}\n{1}\n{2}\n".format(item, user, money))
        fp.close()
    return {}

if __name__ == '__main__':
    contract.run()
