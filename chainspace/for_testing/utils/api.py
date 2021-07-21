from chainspaceapi import ChainspaceClient

clients= {0:{0:ChainspaceClient()}}

def map_object_id_to_shard(num_shards, object_id):
    i = int(object_id, 16)
    return (i % num_shards)

def get_client(shard,replica):
    if not clients.has_key(shard):
        clients[shard] = {}
    if not clients[shard].has_key(replica):
        # slightly affect the latency, but not seriously
        clients[shard][replica] = ChainspaceClient(port=(5000+shard*10+replica))
    return clients[shard][replica]

def dump(transaction,shard=0,replica=0):
    get_client(shard,replica).dump_transaction(transaction)

def dump_many(transactions, shard=0, replica=0):
    for transaction in transactions:
        dump(transaction,shard,replica)

def process(transaction,shard=0,replica=0):
    client.process_transaction(transaction)
    
def process_many(transactions,shard=0,replica=0):
    for transaction in transactions:
        process(transaction)

