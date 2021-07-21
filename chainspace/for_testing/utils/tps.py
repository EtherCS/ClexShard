def tps(filename):
    lines = open(filename).readlines()
    lines = lines[2:]
    lines = [line.strip() for line in lines]

    start_time = int(lines[0].split()[0])
    end_time = int(lines[-1].split()[0])
    s = (end_time - start_time) / float(1000)
    tps = len(lines) / float(s)

    return tps

def shard_tps(shard_id):
    try:
        return tps('../chainspacecore-{0}-0/simplelog'.format(shard_id))
    except Exception:
        return 0

def all_shard_tps(shard_num):
    all_tps = []
    for i in range(shard_num):
        all_tps.append(shard_tps(i))
    return all_tps