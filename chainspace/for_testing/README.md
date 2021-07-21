## Throughput for different contract functions in Chainspace

in order to compare with the existing sharded blockchain, we simply implemented the same smart contracts with the same logic in chainspace, and measured their throughput and latency.

### START
step 1: build and install  
from /chainspace
```
virtualenv venv
. venv/bin/activate

pip install -U setuptools
pip install -e ./chainspaceapi
pip install -e ./chainspacecontract
pip install petlib

cd chainspacecore
mvn -Dversion=1.0-SNAPSHOT package assembly:single
```

step 2: start core (run 2 shards and 4 nodes/shard)  
from /chainspace/for_testing
```
python easy_start.py 2 4
```

step 3: stop core
```
killall java
```


### Throughput Test

step 1: activate virtual environment
from /chainspace
```
. venv/bin/activate
```

step 2: configure parameters (**optional**)

from /chainspace/for_testing
```
vim tester.py
```
default:
```
# parameters for blockchain
shards = 2
nodes = 4
clients = 4

# parameters to decrease experimental error
min_batch = 20
max_batch = 200
batch_step = 20
runs = 3
```

step 3: start testing

from /chainspace/for_testing
```
python tester.py
```

step 4: result

after performing the previous steps, you will get a `result.json` file in `/chainspace/for_testing`

in format:
```
{
    'contract_name': {
        'method_name': {
            'tps': [],
            'latency': [],
        }
    }
}
```