import sys
from utils.node import *

if __name__ == '__main__':

    shards = int(sys.argv[1])
    nodes  = int(sys.argv[2])

    clean_core(shards,nodes)
    config_core(shards,nodes)
    start_core(shards,nodes)