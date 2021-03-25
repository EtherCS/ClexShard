#!/bin/bash
cd ./occlum_workspace
declare -i N=$1
declare -i amount=$2
start=`date +%s%N`
for ((i=0;i<$N;i++))
do
{
    occlum exec /bin/tps $amount &
}
done
wait
end=`date +%s%N`
time=$((end-start))
tps=$((N * amount * 1000000000 / time))
echo $tps
