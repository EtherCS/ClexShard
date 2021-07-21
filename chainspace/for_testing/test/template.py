import os
import operator
import time
import sys
import random
import traceback
import json
import thread

from utils.api import *
from utils.parse import *
from utils.node import *
from utils.tps import *

from contracts import simulator

def invoked_template(inputs=None, reference_inputs=None, parameters=None):
    return simulator.consume(inputs, reference_inputs, parameters)

def simulation_batched(n, inputs_per_tx, clients=1, batch_size=100, batch_sleep=2, nonce=True, shards_per_tx=None, shards=None, invoke=invoke_template):
    init_tx = test.init()
    process(init_tx)

    parameters = [str(n*inputs_per_tx),]
    if nonce:
        parameters.append(str(random.randint(1,1000000)))
    create_tx = test.create((init_tx['transaction']['outputs'][0],), None, parameters)
    process(create_tx)
    time.sleep(5)

    outputs = create_tx['transaction']['outputs']

    if shards_per_tx is not None:
        outputs_map = {}
        for shard in range(shards):
            outputs_map[shard] = []
        for output in outputs:
            outputs_map[map_object_id_to_shard(shards, output.object_id)].append(output)

    transactions = []
    try:
        for i in range(0, len(outputs), inputs_per_tx):
            if shards_per_tx is not None:
                rand = random.randint(0, shards-shards_per_tx)
            objects = []
            for j in range(inputs_per_tx):
                if shards_per_tx is None:
                    objects.append(outputs[i+j])
                else:
                    shard_id = j%shards_per_tx
                    shard_id = shard_id + rand
                    objects.append(outputs_map[shard_id].pop())
            transactions.append(invoke(objects))
    except IndexError:
        pass

    for i in range(0, len(transactions), batch_size*clients):
        for j in range(0,clients):
            thread.start_new_thread(dump_many, (transactions[i+batch_size*j:i+batch_size*(j+1)],j,0))
        # dump_many(transactions[i:i+batch_size],0,0)
        time.sleep(batch_sleep)

def run(shards,nodes,clients,min_batch, max_batch, batch_step, runs, invoke):
    latency_times_set_set = []
    tps_set_set = []

    for batch_size in range(min_batch, max_batch+1, batch_step):
        latency_times_set = []
        tps_set = []
        for i in range(runs):
            print "Running client latency measurements for batch size {0} (run {1}).".format(batch_size, i)
            num_transactions = max_batch*shards*3
            clear_core(shards,nodes)
            time.sleep(2)
            start_core(shards,nodes)
            time.sleep(10)
            start_clients(clients)
            os.system('cd ../chainspacecore-0-0 && bash ./runclientservice.sh 0 0')
            time.sleep(10)

            # to make sure contract checker starts
            init_tx = test.init()
            process(init_tx)
            time.sleep(10)

            os.system('screen -dmS tcpdump sudo tcpdump -i lo -A -tt | tee tcpdump_log')

            time.sleep(10)
            simulation_batched(num_transactions, inputs_per_tx=1, clients=clients, batch_size=batch_size, batch_sleep=1, invoke=invoke)
            time.sleep(30)
            
            os.system('screen -X -S tcpdump kill')
            os.system('screen -X -S client-api kill')
            stop_core(shards,nodes)
            time.sleep(10)
            
            all_tps = all_shard_tps(shards)
            all_tps = sum(all_tps)
            tps_set.append(all_tps)
            
            tcpdump_txes = parse_tcpdump('tcpdump_log')
            client_txes = parse_client_simplelog('../chainspacecore-0-0/simplelog_client')
            latency_times = []
            for tx, t in tcpdump_txes.iteritems():
                try:
                    latency_times.append((tcpdump_txes[tx] - client_txes[tx])/1000.0)
                except Exception:
                    pass
            try:
                latency_times = sum(latency_times)/len(latency_times)
            except Exception:
                latency_times = -1
            latency_times_set.append(latency_times)
        tps_set_set.append(tps_set)
        latency_times_set_set.append(latency_times_set)
    return {'tps':tps_set_set, 'latency':latency_times_set_set}

if __name__ == '__main__':
    shards = 3
    nodes = 2
    clients = 1
    min_batch = 240
    max_batch = 240
    batch_step = 20
    runs = 1
    print run(shards,nodes,clients,min_batch, max_batch, batch_step, runs, invoked_template)