#!/bin/bash
# shell para (N, tee_amount, amount)

declare -i N=$1
declare -i tee_amount=$2
declare -i amount=$3

for ((i=1;i<=$tee_amount;i++))
do
{
    bash ./run.sh $N $tee_amount $amount &
}
done
wait