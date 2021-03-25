#!/bin/bash
declare -i N=3
path="./occlum_workspace_"
for ((i=1;i<=$N;i++))
do
{
    mkdir $path$i
    cd $path$i
    occlum init
    cp ../all_tps/tps_* image/bin
    SGX_MODE=SIM occlum build
}&
done
wait