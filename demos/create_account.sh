#!/bin/bash

occlum-g++ create_account.cpp -o create_account
declare -i address_len=$1
declare -i account_num=$2
./create_account ./Token_tps/$2_state $1 $2
rm -rf ./create_account