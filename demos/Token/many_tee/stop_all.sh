#!/bin/bash
# shell para (N, tee_amount, amount)

declare -i tee_amount=$1
for ((i = 1; i <= $tee_amount; i++))
do
{
    cd occlum_workspace_$i
    occlum stop
    cd ..   
}
done
wait