# Introduction
ClexShard is a layer-two framework which aims to efficiently and securely support arbitrarily complex cross-shard contracts in blockchain sharding systems.

ClexShard has the following salient features:
+ **Compatible.** ClexShard can be easily integrated into existing blockchain sharding systems with minor modifications.
+ **Low latency.** ClexShard can achieve low confirmation latency of millisecond level for cross-shard contracts, which is far lower than the current sharding systems.
+ **Complex contracts supporting.** ClexShard supports the complex cross-shard contract execution, which enhances application areas of sharding systems.
+ **Secure execution.** ClexShard can guarantee the secure execution for cross-shard contracts. Any misbehaviors performed by users or off-chain nodes can be detected. 

# Demos
1. Hello World Contract:
+ Print: return a "Hello World!" message to the user
2. Token Contract:
+ Create: a user creates an account in the Token contract
+ Get: check balance of a user in the Token contract
+ Transfer: a user transfer his token to another user of the Token contract
3. Vote Contract:
+ Nominate: the contract creator adds a set of candidates and specifies a group of users as the voters
+ Vote: a voter gives its ballot to a candidate
4. Auction Contract:
+ AddItem: the contract creator releases a new item
+ Bid: a user bids on a specific item
5. Complex Contract 1:
+ call1: the contract calls another contract
6. Complex Contract 2:
+ call2: the contract calls other two contracts

# How to use?
## step 1: build the Occlum
please see https://github.com/occlum/occlum

## step 2: Start Chainspace
please see *./chainspace/for_testing/README.md*
## step 3: Start test the performance of our model
Enter the demos/Client directory, and refer to related *README* for different tests.
