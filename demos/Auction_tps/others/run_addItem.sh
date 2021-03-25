#!/bin/bash
declare -i N=1
declare -i amount=100000
cd /root/demos/auction_tps/occlum_workspace
start=`date +%s%N`
for ((i=0;i<$N;i++))
do
{
    occlum exec /bin/tps_1 releaseItem macbook /root/state_1.txt $amount &
}
done
wait
end=`date +%s%N`
time=$((end-start))
tps=$((N * amount * 1000000000 / time))
echo $tps
