import os
import math
import time

def config_core(shards, nodes):
    os.system('rm -rf chainspacecore-*;')
    os.system('rm -rf chainspacecore/contracts;')
    os.system('cp -rf for_testing/contracts chainspacecore/contracts;')

    # config shard config
    command = ''
    command += 'cd ChainSpaceConfig;'
    command += 'printf "" > shardConfig.txt;'
    for i in range(shards):
        command += 'printf "{0} ChainSpaceConfig/shards/s{0}\n" >> shardConfig.txt;'.format(i)
        command += 'cp -r shards/config0 shards/s{0};'.format(i)

        # config hosts.config
        command += 'printf "" > shards/s{0}/hosts.config;'.format(i)
        for j in range(nodes):
            command += 'printf "{1} 127.0.0.1 {2}\n" >> shards/s{0}/hosts.config;'.format(i, j, (10+i*nodes+j)*1000)

        # config system.config
        initial_view = ','.join((str(x) for x in range(nodes)))
        faulty_replicas = (nodes-1)/3
        faulty_replicas = int(math.floor(faulty_replicas))
        command += 'cp shards/config0/system.config.forscript shards/s{0}/system.config.forscript;'.format(i)
        command += 'printf "system.servers.num = {1}\n" >> shards/s{0}/system.config.forscript;'.format(i, nodes)
        command += 'printf "system.servers.f = {1}\n" >> shards/s{0}/system.config.forscript;'.format(i, faulty_replicas)
        command += 'printf "system.initial.view = {1}\n" >> shards/s{0}/system.config.forscript;'.format(i, initial_view)
        command += 'cp shards/s{0}/system.config.forscript shards/s{0}/system.config;'.format(i)

    for i in range(shards):
        for j in range(nodes):
            cmd = 'cd ..;cp -r chainspacecore chainspacecore-{0}-{1};cd chainspacecore-{0}-{1};'.format(i,j) + command
            
            # config client config
            cmd += 'cp -r . ../ChainSpaceClientConfig/;'
            cmd += 'printf "shardConfigFile ChainSpaceConfig/shardConfig.txt\nthisShard {0}\nthisReplica {1}\n" > config.txt;'.format(i, j)
            cmd += 'printf "shardConfigFile ChainSpaceClientConfig/shardConfig.txt\nthisClient {0}\n" > ../ChainSpaceClientConfig/config.txt;'.format(i, j)
            cmd += 'cd ../;'
            cmd += 'rm -rf config;'
            cmd += 'cp -r ChainSpaceConfig/shards/s{0} config;'.format(i)
            os.system(cmd)

def start_node(nodes, i, j):
    cmd = 'cd ../chainspacecore-{0}-{1};screen -dmS s{0}n{1} java -Dchecker.start.port={2} -cp lib/bft-smart-1.2.1-UCL.jar:target/chainspace-1.0-SNAPSHOT-jar-with-dependencies.jar uk.ac.ucl.cs.sec.chainspace.bft.TreeMapServer ChainSpaceConfig/config.txt'.format(i,j,(10+i*nodes+j)*1000+10)
    os.system(cmd)

def start_client(i,j):
    os.system('cd ../chainspacecore-{0}-{1} && bash ./runclientservice.sh {0} {1}'.format(i,j))

def start_clients(num):
    for i in range(num):
        start_client(i,0)

def start_core(shards, nodes):
    for i in range(shards):
        for j in range(nodes):
            start_node(nodes, i, j)
            time.sleep(10)

def stop_core(shards, nodes):
    for i in range(shards):
        for j in range(nodes):
            os.system('screen -X -S s{0}n{1} kill'.format(i,j))

def clear_core(shards, nodes):
    for i in range(shards):
        for j in range(nodes):
            os.system('cd ../chainspacecore-{0}-{1};rm database.sqlite; rm simplelog;'.format(i,j))

def clean_core(shards, nodes):
    os.system('cd ../;rm -rf chainspacecore-*;')
