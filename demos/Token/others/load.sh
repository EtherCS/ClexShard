#!/bin/bash
declare -i N=50
declare -i amount=1
cd /root/demos/transfer_tps/occlum_workspace
start=`date +%s%N`
for ((i=0;i<$N;i++))
do
{
    occlum run /bin/tps_1 create 1 10 /root/state_1.txt $amount 
#    occlum run /bin/tps_2 create 1 10 /root/state_2.txt $amount &
#    wait
}
done
latency_start=`date +%s%N`
wait
end=`date +%s%N`
time=$((end-start))
tps=$((N * amount * 1000000000 / time))
echo $((time / N / 1000000))
echo $tps