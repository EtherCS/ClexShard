#!/bin/bash
# ROOT_DIR=/home/gudale/gudale-dev/ClexShard/demos/hw_tps/multiple_TEE_Providers/
declare -i N=$1     #the number of TEE providers
for ((i=1;i<=$N;i++))
do
{
    
    subdir=$i'_occlum'
    dir='./'$subdir
    echo "enter $i th occlum dir $dir"
    cd $dir
    # pwd
    occlum run /bin/$i ./state test 12345678901234567890
    echo "run $i"
    # netstat -anp | grep $i0001
}&
done