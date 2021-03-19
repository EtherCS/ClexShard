#!/bin/bash
# shell para (N, tee_amount, amount)
declare -i N=$1
declare -i tee_amount=$2
declare -i amount=$3
start=`date +%s%N`
for ((i = 0; i < $N; i++))
do
{
    for ((j = 1; j <= $tee_amount; j++))
    do
    {
        cd ./occlum_workspace_$j
        occlum run /bin/tps_1 addCandidate 1 /root/state_1.txt $amount
    }&
    done
    wait
}
done
wait
end=`date +%s%N`
time=$((end-start))
echo $((time / N / 1000000))