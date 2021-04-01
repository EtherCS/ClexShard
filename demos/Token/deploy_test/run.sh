#!/bin/bash
declare -i N=$1
for ((i=1;i<=$N;i++))
do
{
    occlum_name=$i'_occlum'
    rm -rf $occlum_name
    file_name=$i'_deploy_shell.py'
    python3 $file_name
}&
done
wait

