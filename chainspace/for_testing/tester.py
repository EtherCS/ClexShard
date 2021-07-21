#!/usr/bin/python
from test import *
from utils.node import *
import os
import sys
import getpass
import requests
import json

# parameters

shards = 2
nodes = 4
clients = 4
min_batch = 20
max_batch = 200
batch_step = 20
runs = 3

def run_test(tester):
    return template.run(shards,nodes,clients,min_batch, max_batch, batch_step, runs,tester.invoke)

def clean():
    # clear_core(shards,nodes)
    stop_core(shards,nodes)
    os.system('killall java')
    os.system('killall tcpdump')

if __name__ == '__main__':

    clean_core(shards,nodes)
    config_core(shards,nodes)

    result = {}
    result['hw'] = {
        'helloworld': run_test(hw_test),
    }
    outfh = open('result.json', 'w')
    outfh.write(json.dumps(result))

    result['token'] = {
        'create': run_test(token_create_test),
        'getBalance': run_test(token_getBalance_test),
        'transfer': run_test(token_transfer_test),
    } 
    outfh = open('result.json', 'w')
    outfh.write(json.dumps(result))

    result['vote'] = {
        'addCandidate': run_test(vote_addCandidate_test),
        'vote': run_test(vote_vote_test),
    }
    outfh = open('result.json', 'w')
    outfh.write(json.dumps(result))

    result['auction'] = {
        'releaseItem': run_test(auction_bidItem_test),
        'bidItem': run_test(auction_releaseItem_test),
    }
    outfh = open('result.json', 'w')
    outfh.write(json.dumps(result))

    clean()

    # webhook
