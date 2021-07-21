def parse_client_simplelog(filename):
    data = open(filename).readlines()[2:]
    txes = {}
    for line in data:
        record = line.split()
        txes[record[1]] = int(record[0])

    return txes
        
def parse_tcpdump(filename):
    try:
        lines = open(filename).readlines()

        txes = {}

        for i, line in enumerate(lines):
            if 'Flags' in line:
                time_line = line
            elif 'accepted_t_commit;' in line:
                hash_index = line.index('accepted_t_commit;')+len('accepted_t_commit;')+65
                try:
                    if(line[hash_index] != '<'):
                        first_input = line[hash_index:hash_index+64]
                        timestamp = float(time_line[:17])
                        timestamp = timestamp*1000
                        txes[first_input] = timestamp
                except IndexError:
                    pass

        return txes
    except Exception:
        return []
